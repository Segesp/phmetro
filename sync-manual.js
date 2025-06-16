#!/usr/bin/env node

const { createClient } = require('@supabase/supabase-js');

// Configuración
const SUPABASE_URL = 'https://japrhhtnijidlecqrkpk.supabase.co';
const SUPABASE_ANON_KEY = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM';
const THINGSPEAK_CHANNEL_ID = '2988488';
const THINGSPEAK_READ_API_KEY = 'Z6SC5MLLP0FR4PC4';

const supabase = createClient(SUPABASE_URL, SUPABASE_ANON_KEY);

async function syncThingSpeakToSupabase() {
    try {
        console.log('🔄 Iniciando sincronización ThingSpeak → Supabase');
        
        // 1. Obtener datos de ThingSpeak
        const thingSpeakUrl = `https://api.thingspeak.com/channels/${THINGSPEAK_CHANNEL_ID}/feeds.json?api_key=${THINGSPEAK_READ_API_KEY}&results=20`;
        console.log('📡 Consultando ThingSpeak...');
        
        const response = await fetch(thingSpeakUrl);
        if (!response.ok) {
            throw new Error(`ThingSpeak API error: ${response.status}`);
        }
        
        const data = await response.json();
        console.log(`📊 Feeds encontrados: ${data.feeds?.length || 0}`);
        
        if (!data.feeds || data.feeds.length === 0) {
            console.log('✅ No hay datos en ThingSpeak');
            return;
        }
        
        // 2. Obtener último timestamp de Supabase
        const { data: lastRecord } = await supabase
            .from('ph_readings')
            .select('created_at')
            .eq('device', 'ThingSpeak')
            .order('created_at', { ascending: false })
            .limit(1);
        
        const lastSupabaseTime = lastRecord?.[0]?.created_at ? new Date(lastRecord[0].created_at) : new Date(0);
        console.log(`⏰ Último registro en Supabase: ${lastSupabaseTime.toISOString()}`);
        
        // 3. Filtrar solo datos nuevos
        const newFeeds = data.feeds.filter(feed => {
            if (!feed.field1 || feed.field1 === null) return false;
            const feedTime = new Date(feed.created_at);
            return feedTime > lastSupabaseTime;
        });
        
        console.log(`📈 Datos nuevos para sincronizar: ${newFeeds.length}`);
        
        if (newFeeds.length === 0) {
            console.log('✅ Todos los datos ya están sincronizados');
            return;
        }
        
        // 4. Preparar datos para Supabase
        const recordsToInsert = newFeeds.map(feed => ({
            ph: parseFloat(feed.field1),
            timestamp: feed.created_at,
            device: 'ThingSpeak'
        }));
        
        console.log('💾 Insertando en Supabase:');
        recordsToInsert.forEach((record, i) => {
            console.log(`  ${i+1}. pH: ${record.ph}, Time: ${record.timestamp}`);
        });
        
        // 5. Insertar en Supabase
        const { data: insertedData, error } = await supabase
            .from('ph_readings')
            .insert(recordsToInsert)
            .select();
        
        if (error) {
            console.error('❌ Error en Supabase:', error);
            return;
        }
        
        console.log(`✅ Sincronización exitosa: ${insertedData?.length || 0} registros guardados`);
        
    } catch (error) {
        console.error('💥 Error:', error.message);
    }
}

// Ejecutar si es llamado directamente
if (require.main === module) {
    syncThingSpeakToSupabase();
}

module.exports = { syncThingSpeakToSupabase };
