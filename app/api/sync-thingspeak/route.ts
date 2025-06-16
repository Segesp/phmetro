import { createClient } from '@supabase/supabase-js'
import { NextRequest, NextResponse } from 'next/server'

// Configuración Supabase
const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL || 'https://japrhhtnijidlecqrkpk.supabase.co'
const supabaseKey = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY || 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM'
const supabase = createClient(supabaseUrl, supabaseKey)

// Configuración ThingSpeak
const THINGSPEAK_CHANNEL_ID = '2988488'
const THINGSPEAK_READ_API_KEY = 'Z6SC5MLLP0FR4PC4'

export async function POST(request: NextRequest) {
  try {
    console.log('🔄 [SYNC] Iniciando sincronización ThingSpeak → Supabase')
    
    // 1. Obtener últimos datos de ThingSpeak
    const thingSpeakUrl = `https://api.thingspeak.com/channels/${THINGSPEAK_CHANNEL_ID}/feeds.json?api_key=${THINGSPEAK_READ_API_KEY}&results=10`
    
    console.log('📡 [SYNC] Consultando ThingSpeak:', thingSpeakUrl)
    
    const thingSpeakResponse = await fetch(thingSpeakUrl)
    if (!thingSpeakResponse.ok) {
      throw new Error(`ThingSpeak API error: ${thingSpeakResponse.status}`)
    }
    
    const thingSpeakData = await thingSpeakResponse.json()
    console.log('📊 [SYNC] Datos de ThingSpeak:', {
      channel: thingSpeakData.channel?.name,
      feeds: thingSpeakData.feeds?.length || 0
    })
    
    if (!thingSpeakData.feeds || thingSpeakData.feeds.length === 0) {
      return NextResponse.json({
        success: true,
        message: 'No hay datos nuevos en ThingSpeak',
        synced: 0
      })
    }
    
    // 2. Obtener último timestamp de Supabase para evitar duplicados
    const { data: lastRecord } = await supabase
      .from('ph_readings')
      .select('created_at')
      .eq('device', 'ThingSpeak')
      .order('created_at', { ascending: false })
      .limit(1)
    
    const lastSupabaseTime = lastRecord?.[0]?.created_at ? new Date(lastRecord[0].created_at) : new Date(0)
    console.log('⏰ [SYNC] Último registro en Supabase:', lastSupabaseTime.toISOString())
    
    // 3. Filtrar solo datos nuevos de ThingSpeak
    const newFeeds = thingSpeakData.feeds.filter(feed => {
      if (!feed.field1 || feed.field1 === null) return false
      
      const feedTime = new Date(feed.created_at)
      return feedTime > lastSupabaseTime
    })
    
    console.log(`📈 [SYNC] Datos nuevos encontrados: ${newFeeds.length}`)
    
    if (newFeeds.length === 0) {
      return NextResponse.json({
        success: true,
        message: 'No hay datos nuevos para sincronizar',
        synced: 0
      })
    }
    
    // 4. Preparar datos para insertar en Supabase
    const recordsToInsert = newFeeds.map(feed => ({
      ph: parseFloat(feed.field1),
      timestamp: feed.created_at,
      device: 'ThingSpeak'
    }))
    
    console.log('💾 [SYNC] Insertando en Supabase:', recordsToInsert)
    
    // 5. Insertar en Supabase
    const { data, error } = await supabase
      .from('ph_readings')
      .insert(recordsToInsert)
      .select()
    
    if (error) {
      console.error('❌ [SYNC] Error Supabase:', error)
      return NextResponse.json({
        success: false,
        error: 'Error al guardar en Supabase',
        details: error.message
      }, { status: 500 })
    }
    
    console.log('✅ [SYNC] Sincronización exitosa:', data?.length || 0, 'registros')
    
    return NextResponse.json({
      success: true,
      message: `Sincronizados ${data?.length || 0} registros de ThingSpeak a Supabase`,
      synced: data?.length || 0,
      records: data
    })
    
  } catch (error) {
    console.error('💥 [SYNC] Error:', error)
    return NextResponse.json({
      success: false,
      error: 'Error en sincronización',
      details: (error as Error).message
    }, { status: 500 })
  }
}

// GET endpoint para sincronización manual/automática
export async function GET(request: NextRequest) {
  return POST(request)
}
