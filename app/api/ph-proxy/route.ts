import { createClient } from '@supabase/supabase-js'
import { NextRequest, NextResponse } from 'next/server'

// Configuración Supabase
const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL || 'https://your-project.supabase.co'
const supabaseKey = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY || 'your-anon-key'
const supabase = createClient(supabaseUrl, supabaseKey)

export async function POST(request: NextRequest) {
  try {
    const body = await request.json()
    
    // Validar datos básicos
    if (!body.ph || isNaN(parseFloat(body.ph))) {
      return NextResponse.json(
        { error: 'pH value is required and must be a number' },
        { status: 400 }
      )
    }

    // Preparar datos para Supabase
    const phData = {
      ph: parseFloat(body.ph),
      timestamp: new Date().toISOString(),
      device: body.device || 'Arduino_HTTP_Proxy',
      sensor: body.sensor || 'pH_sensor',
      transmission: body.transmission || 0,
      readings: body.readings || 1,
      notes: 'Via HTTP Proxy'
    }

    // Insertar en Supabase
    const { data, error } = await supabase
      .from('ph_readings')
      .insert([phData])

    if (error) {
      console.error('Supabase error:', error)
      return NextResponse.json(
        { error: 'Database error', details: error.message },
        { status: 500 }
      )
    }

    // Respuesta exitosa
    return NextResponse.json(
      { 
        success: true, 
        message: 'pH data received and stored',
        data: phData,
        id: data?.[0]?.id
      },
      { 
        status: 200,
        headers: {
          'Access-Control-Allow-Origin': '*',
          'Access-Control-Allow-Methods': 'POST, OPTIONS',
          'Access-Control-Allow-Headers': 'Content-Type',
        }
      }
    )

  } catch (error) {
    console.error('Proxy endpoint error:', error)
    return NextResponse.json(
      { error: 'Internal server error' },
      { status: 500 }
    )
  }
}

// Manejar preflight requests
export async function OPTIONS(request: NextRequest) {
  return new NextResponse(null, {
    status: 200,
    headers: {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'POST, OPTIONS',
      'Access-Control-Allow-Headers': 'Content-Type',
    },
  })
}
