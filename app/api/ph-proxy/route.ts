import { createClient } from '@supabase/supabase-js'
import { NextRequest, NextResponse } from 'next/server'

// Configuración Supabase
const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL || 'https://japrhhtnijidlecqrkpk.supabase.co'
const supabaseKey = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY || 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM'
const supabase = createClient(supabaseUrl, supabaseKey)

export async function POST(request: NextRequest) {
  try {
    // Log de configuración para debug
    console.log('🔧 [PH-PROXY] Config:', {
      url: supabaseUrl,
      hasKey: !!supabaseKey,
      keyLength: supabaseKey?.length,
      environment: process.env.NODE_ENV
    })
    
    const body = await request.json()
    console.log('📥 [PH-PROXY] Datos recibidos:', body)
    
    // Validar datos básicos
    if (!body.ph || isNaN(parseFloat(body.ph))) {
      console.log('❌ [PH-PROXY] Validación fallida:', body)
      return NextResponse.json(
        { error: 'pH value is required and must be a number' },
        { status: 400 }
      )
    }

    // Preparar datos para Supabase (solo columnas existentes)
    const phData = {
      ph: parseFloat(body.ph),
      timestamp: body.timestamp ? new Date(body.timestamp).toISOString() : null
    }

    console.log('📊 [PH-PROXY] Datos a insertar:', phData)

    // Insertar en Supabase
    console.log('💾 [PH-PROXY] Insertando en Supabase...')
    const { data, error } = await supabase
      .from('ph_readings')
      .insert([phData])

    if (error) {
      console.error('❌ [PH-PROXY] Supabase error:', error)
      return NextResponse.json(
        { error: 'Database error', details: error.message },
        { status: 500 }
      )
    }

    console.log('✅ [PH-PROXY] Inserción exitosa:', data)
    
    // Respuesta exitosa
    return NextResponse.json(
      { 
        success: true, 
        message: 'pH data received and stored',
        data: { ...phData, device: body.device || 'Arduino' },
        id: data?.[0]?.id,
        timestamp: new Date().toISOString()
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
