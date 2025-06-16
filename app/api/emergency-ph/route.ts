import { createClient } from '@supabase/supabase-js'
import { NextRequest, NextResponse } from 'next/server'

// VALORES HARDCODEADOS PARA BYPASS DE VARIABLES DE ENTORNO
const HARDCODED_URL = 'https://japrhhtnijidlecqrkpk.supabase.co'
const HARDCODED_KEY = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM'

const supabase = createClient(HARDCODED_URL, HARDCODED_KEY)

export async function POST(request: NextRequest) {
  try {
    console.log('🚨 [EMERGENCY] Usando valores hardcodeados')
    console.log('🔧 [EMERGENCY] Config:', {
      url: HARDCODED_URL,
      keyLength: HARDCODED_KEY.length,
      environment: process.env.NODE_ENV
    })
    
    const body = await request.json()
    console.log('📥 [EMERGENCY] Datos recibidos:', body)
    
    // Validar datos básicos
    if (!body.ph || isNaN(parseFloat(body.ph))) {
      console.log('❌ [EMERGENCY] Validación fallida:', body)
      return NextResponse.json(
        { error: 'pH value is required and must be a number' },
        { status: 400 }
      )
    }

    // Preparar datos para Supabase
    const phData = {
      ph: parseFloat(body.ph),
      timestamp: body.timestamp ? new Date(body.timestamp).toISOString() : null,
      device: body.device || 'EMERGENCY_API'
    }

    console.log('📊 [EMERGENCY] Datos a insertar:', phData)

    // Insertar en Supabase
    console.log('💾 [EMERGENCY] Insertando en Supabase...')
    const { data, error } = await supabase
      .from('ph_readings')
      .insert([phData])
      .select()

    if (error) {
      console.error('❌ [EMERGENCY] Supabase error:', error)
      return NextResponse.json(
        { error: 'Database error', details: error.message },
        { status: 500 }
      )
    }

    console.log('✅ [EMERGENCY] Inserción exitosa:', data)
    
    // Respuesta exitosa
    return NextResponse.json(
      { 
        success: true, 
        message: 'pH data received and stored via EMERGENCY API',
        data: data?.[0],
        timestamp: new Date().toISOString(),
        note: 'Used hardcoded credentials - check Vercel env vars'
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
    console.error('💥 [EMERGENCY] Error:', error)
    return NextResponse.json(
      { error: 'Internal server error', details: (error as Error).message },
      { status: 500 }
    )
  }
}

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
