import { NextRequest, NextResponse } from 'next/server'
import { createClient } from '@supabase/supabase-js'

const supabase = createClient(
  process.env.NEXT_PUBLIC_SUPABASE_URL || 'https://japrhhtnijidlecqrkpk.supabase.co',
  process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY || 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM'
)

export async function POST(request: NextRequest) {
  try {
    console.log('🔧 [PH-DATA] Config:', {
      url: process.env.NEXT_PUBLIC_SUPABASE_URL || 'https://japrhhtnijidlecqrkpk.supabase.co',
      hasKey: !!(process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY || 'fallback'),
      environment: process.env.NODE_ENV
    })
    
    const body = await request.json()
    console.log('📥 [PH-DATA] Datos recibidos:', body)
    const { ph, timestamp } = body

    // Validar datos
    if (typeof ph !== 'number' || ph < 0 || ph > 14) {
      console.log('❌ [PH-DATA] Validación fallida:', { ph, type: typeof ph })
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
