import { createClient } from '@supabase/supabase-js'
import { NextRequest, NextResponse } from 'next/server'

// Configuración Supabase con valores hardcodeados para bypass
const supabaseUrl = 'https://japrhhtnijidlecqrkpk.supabase.co'
const supabaseKey = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM'
const supabase = createClient(supabaseUrl, supabaseKey)

export async function POST(request: NextRequest) {
  try {
    console.log('📡 [WEBHOOK] ThingSpeak webhook recibido')
    
    // ThingSpeak envía los datos en el body
    const body = await request.json()
    console.log('📥 [WEBHOOK] Datos recibidos:', body)
    
    // Extraer datos del webhook de ThingSpeak
    // ThingSpeak webhook format: { "field1": "7.00", "created_at": "2025-06-16T01:07:38Z", ... }
    const ph = parseFloat(body.field1)
    const timestamp = body.created_at
    
    if (isNaN(ph) || !timestamp) {
      console.log('❌ [WEBHOOK] Datos inválidos:', { ph, timestamp })
      return NextResponse.json({
        success: false,
        error: 'Datos inválidos en webhook'
      }, { status: 400 })
    }
    
    // Verificar si este registro ya existe para evitar duplicados
    const { data: existing } = await supabase
      .from('ph_readings')
      .select('id')
      .eq('device', 'ThingSpeak')
      .eq('timestamp', timestamp)
      .limit(1)
    
    if (existing && existing.length > 0) {
      console.log('⚠️ [WEBHOOK] Registro ya existe, saltando...')
      return NextResponse.json({
        success: true,
        message: 'Registro ya existe',
        skipped: true
      })
    }
    
    // Preparar datos para Supabase
    const recordToInsert = {
      ph: ph,
      timestamp: timestamp,
      device: 'ThingSpeak'
    }
    
    console.log('💾 [WEBHOOK] Insertando en Supabase:', recordToInsert)
    
    // Insertar en Supabase
    const { data, error } = await supabase
      .from('ph_readings')
      .insert([recordToInsert])
      .select()
    
    if (error) {
      console.error('❌ [WEBHOOK] Error Supabase:', error)
      return NextResponse.json({
        success: false,
        error: 'Error al guardar en Supabase',
        details: error.message
      }, { status: 500 })
    }
    
    console.log('✅ [WEBHOOK] Inserción exitosa:', data)
    
    return NextResponse.json({
      success: true,
      message: 'Datos de ThingSpeak guardados en Supabase',
      data: data?.[0],
      timestamp: new Date().toISOString()
    })
    
  } catch (error) {
    console.error('💥 [WEBHOOK] Error:', error)
    return NextResponse.json({
      success: false,
      error: 'Error en webhook',
      details: (error as Error).message
    }, { status: 500 })
  }
}

// GET endpoint para verificar que está funcionando
export async function GET(request: NextRequest) {
  return NextResponse.json({
    service: 'ThingSpeak to Supabase Webhook',
    status: 'active',
    timestamp: new Date().toISOString(),
    info: 'Configure this URL as webhook in ThingSpeak'
  })
}
