#!/bin/bash

# ===============================================
# PRUEBA DE SINCRONIZACIÓN THINGSPEAK → SUPABASE
# ===============================================

echo "🔄 PRUEBA DE SINCRONIZACIÓN"
echo "==========================="

echo "1️⃣ Verificando últimos datos en ThingSpeak..."
echo "---------------------------------------------"

# Consultar ThingSpeak directamente
THINGSPEAK_DATA=$(curl -s "https://api.thingspeak.com/channels/2988488/feeds.json?api_key=Z6SC5MLLP0FR4PC4&results=3")

echo "Últimos 3 registros en ThingSpeak:"
echo "$THINGSPEAK_DATA" | python3 -c "
import sys, json
data = json.load(sys.stdin)
if 'feeds' in data:
    for feed in data['feeds'][-3:]:
        print(f\"- pH: {feed['field1']} | Fecha: {feed['created_at']} | Entry: {feed['entry_id']}\")
else:
    print('Error al leer ThingSpeak')
" 2>/dev/null || echo "Error al procesar datos de ThingSpeak"

echo -e "\n2️⃣ Verificando últimos datos en Supabase..."
echo "--------------------------------------------"

SUPABASE_URL="https://japrhhtnijidlecqrkpk.supabase.co"
SUPABASE_KEY="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM"

SUPABASE_DATA=$(curl -s -X GET "${SUPABASE_URL}/rest/v1/ph_readings?select=*&device=eq.ThingSpeak&order=created_at.desc&limit=3" \
  -H "apikey: $SUPABASE_KEY" \
  -H "Authorization: Bearer $SUPABASE_KEY")

echo "Últimos 3 registros de ThingSpeak en Supabase:"
echo "$SUPABASE_DATA" | python3 -c "
import sys, json
try:
    data = json.load(sys.stdin)
    if isinstance(data, list) and len(data) > 0:
        for record in data:
            print(f\"- pH: {record['ph']} | Fecha: {record['created_at']} | Device: {record['device']}\")
    else:
        print('No hay registros de ThingSpeak en Supabase')
except:
    print('Error al procesar datos de Supabase')
" 2>/dev/null || echo "Error al consultar Supabase"

echo -e "\n3️⃣ Ejecutando sincronización manual..."
echo "--------------------------------------"

# Ejecutar sincronización
SYNC_RESULT=$(curl -s -X POST https://phmetro-phi.vercel.app/api/sync-thingspeak --max-time 30)

echo "Resultado de sincronización:"
echo "$SYNC_RESULT" | python3 -m json.tool 2>/dev/null || echo "Error: $SYNC_RESULT"

echo -e "\n4️⃣ Verificando datos después de sincronización..."
echo "------------------------------------------------"

sleep 2

SUPABASE_DATA_AFTER=$(curl -s -X GET "${SUPABASE_URL}/rest/v1/ph_readings?select=*&device=eq.ThingSpeak&order=created_at.desc&limit=5" \
  -H "apikey: $SUPABASE_KEY" \
  -H "Authorization: Bearer $SUPABASE_KEY")

echo "Registros de ThingSpeak en Supabase (después):"
echo "$SUPABASE_DATA_AFTER" | python3 -c "
import sys, json
try:
    data = json.load(sys.stdin)
    if isinstance(data, list) and len(data) > 0:
        print(f'Total de registros: {len(data)}')
        for record in data:
            print(f\"- pH: {record['ph']} | Fecha: {record['created_at']} | Device: {record['device']}\")
    else:
        print('No hay registros de ThingSpeak en Supabase')
except:
    print('Error al procesar datos de Supabase')
" 2>/dev/null || echo "Error al consultar Supabase"

echo -e "\n🎯 PRUEBA COMPLETADA"
echo "==================="
echo "Si ves registros nuevos, la sincronización está funcionando!"
