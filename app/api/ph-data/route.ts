import { NextRequest, NextResponse } from 'next/server'
import { createClient } from '@supabase/supabase-js'

const supabase = createClient(
  process.env.NEXT_PUBLIC_SUPABASE_URL!,
  process.env.SUPABASE_SERVICE_ROLE_KEY!
)

export async function POST(request: NextRequest) {
  try {
    const body = await request.json()
    const { ph, timestamp } = body

    // Validar datos
    if (typeof ph !== 'number' || ph < 0 || ph > 14) {
      return NextResponse.json(
        { error: 'Valor de pH inválido' },
        { status: 400 }
      )
    }

    // Insertar en la base de datos
    const { data, error } = await supabase
      .from('ph_readings')
      .insert({
        ph: ph,
        timestamp: timestamp || new Date().toISOString(),
      })
      .select()

    if (error) {
      console.error('Error inserting data:', error)
      return NextResponse.json(
        { error: 'Error al guardar los datos' },
        { status: 500 }
      )
    }

    return NextResponse.json({
      success: true,
      message: 'Datos guardados exitosamente',
      data: data?.[0]
    })

  } catch (error) {
    console.error('Error processing request:', error)
    return NextResponse.json(
      { error: 'Error interno del servidor' },
      { status: 500 }
    )
  }
}

export async function GET() {
  try {
    const { data, error } = await supabase
      .from('ph_readings')
      .select('*')
      .order('created_at', { ascending: false })
      .limit(100)

    if (error) {
      return NextResponse.json(
        { error: 'Error al obtener los datos' },
        { status: 500 }
      )
    }

    return NextResponse.json({
      success: true,
      data: data
    })

  } catch (error) {
    console.error('Error fetching data:', error)
    return NextResponse.json(
      { error: 'Error interno del servidor' },
      { status: 500 }
    )
  }
}
