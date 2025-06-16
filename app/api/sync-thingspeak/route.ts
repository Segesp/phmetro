import { createClient } from '@supabase/supabase-js'
import { NextRequest, NextResponse } from 'next/server'

const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL || 'https://japrhhtnijidlecqrkpk.supabase.co'
const supabaseKey = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY || 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM'
const supabase = createClient(supabaseUrl, supabaseKey)

const THINGSPEAK_CHANNEL_ID = '2988488'
const THINGSPEAK_READ_API_KEY = 'Z6SC5MLLP0FR4PC4'

export async function POST(request: NextRequest) {
  try {
    console.log('🔄 [SYNC] Iniciando sincronización ThingSpeak → Supabase')
    
    const thingSpeakUrl = `https://api.thingspeak.com/channels/${THINGSPEAK_CHANNEL_ID}/feeds.json?api_key=${THINGSPEAK_READ_API_KEY}&results=10`
    
    const thingSpeakResponse = await fetch(thingSpeakUrl)
    if (!thingSpeakResponse.ok) {
      throw new Error(`ThingSpeak API error: ${thingSpeakResponse.status}`)
    }
    
    const thingSpeakData: any = await thingSpeakResponse.json()
    
    if (!thingSpeakData.feeds || thingSpeakData.feeds.length === 0) {
      return NextResponse.json({
        success: true,
        message: 'No hay datos nuevos en ThingSpeak',
        synced: 0
      })
    }
    
    const { data: lastRecord } = await supabase
      .from('ph_readings')
      .select('created_at')
      .eq('device', 'ThingSpeak')
      .order('created_at', { ascending: false })
      .limit(1)
    
    const lastSupabaseTime = lastRecord?.[0]?.created_at ? new Date(lastRecord[0].created_at) : new Date(0)
    
    const newFeeds = thingSpeakData.feeds.filter((feed: any) => {
      if (!feed.field1 || feed.field1 === null) return false
      const feedTime = new Date(feed.created_at)
      return feedTime > lastSupabaseTime
    })
    
    if (newFeeds.length === 0) {
      return NextResponse.json({
        success: true,
        message: 'No hay datos nuevos para sincronizar',
        synced: 0
      })
    }
    
    const recordsToInsert = newFeeds.map((feed: any) => ({
      ph: parseFloat(feed.field1 || '0'),
      created_at: feed.created_at,
      device: 'ThingSpeak'
    }))
    
    const { data, error } = await supabase
      .from('ph_readings')
      .insert(recordsToInsert)
      .select()
    
    if (error) {
      return NextResponse.json({
        success: false,
        error: 'Error al guardar en Supabase',
        details: error.message
      }, { status: 500 })
    }
    
    return NextResponse.json({
      success: true,
      message: `Sincronizados ${data?.length || 0} registros de ThingSpeak a Supabase`,
      synced: data?.length || 0,
      records: data
    })
    
  } catch (error) {
    return NextResponse.json({
      success: false,
      error: 'Error en sincronización',
      details: (error as Error).message
    }, { status: 500 })
  }
}

export async function GET(request: NextRequest) {
  return POST(request)
}
