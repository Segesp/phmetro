#!/bin/bash

# ===============================================
# SINCRONIZACIÓN MANUAL SIMPLE THINGSPEAK → SUPABASE
# ===============================================

echo "🔄 SINCRONIZACIÓN MANUAL THINGSPEAK → SUPABASE"
echo "==============================================="

SUPABASE_URL="https://japrhhtnijidlecqrkpk.supabase.co"
SUPABASE_KEY="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM"

echo "1️⃣ Obteniendo últimos datos de ThingSpeak..."
echo "---------------------------------------------"

# Obtener últimos 5 registros de ThingSpeak
THINGSPEAK_DATA=$(curl -s "https://api.thingspeak.com/channels/2988488/feeds.json?api_key=Z6SC5MLLP0FR4PC4&results=5")

# Procesar y mostrar datos
echo "Últimos registros en ThingSpeak:"
echo "$THINGSPEAK_DATA" | python3 -c "
import sys, json
try:
    data = json.load(sys.stdin)
    if 'feeds' in data and data['feeds']:
        feeds = [f for f in data['feeds'] if f['field1']][-5:]
        for feed in feeds:
            print(f\"  pH: {feed['field1']} | {feed['created_at']} | Entry: {feed['entry_id']}\")
        print(f'\\nEncontrados {len(feeds)} registros válidos')
        # Guardar para siguiente paso
        with open('/tmp/thingspeak_feeds.json', 'w') as f:
            json.dump(feeds, f)
    else:
        print('No hay datos en ThingSpeak')
        exit(1)
except Exception as e:
    print(f'Error: {e}')
    exit(1)
"

if [ $? -ne 0 ]; then
    echo "❌ Error al obtener datos de ThingSpeak"
    exit 1
fi

echo -e "\n2️⃣ Sincronizando datos con Supabase..."
echo "--------------------------------------"

# Leer datos guardados y sincronizar uno por uno
python3 -c "
import json, requests, time

# Cargar datos de ThingSpeak
with open('/tmp/thingspeak_feeds.json', 'r') as f:
    feeds = json.load(f)

supabase_url = '${SUPABASE_URL}/rest/v1/ph_readings'
headers = {
    'apikey': '${SUPABASE_KEY}',
    'Authorization': 'Bearer ${SUPABASE_KEY}',
    'Content-Type': 'application/json',
    'Prefer': 'return=representation'
}

synced = 0
for feed in feeds:
    try:
        # Verificar si ya existe
        check_url = f\"${SUPABASE_URL}/rest/v1/ph_readings?select=id&device=eq.ThingSpeak&timestamp=eq.{feed['created_at']}\"
        check_resp = requests.get(check_url, headers=headers, timeout=10)
        
        if check_resp.status_code == 200 and len(check_resp.json()) > 0:
            print(f\"⚠️  pH {feed['field1']} ya existe, saltando...\")
            continue
        
        # Insertar nuevo registro
        data = {
            'ph': float(feed['field1']),
            'timestamp': feed['created_at'],
            'device': 'ThingSpeak'
        }
        
        resp = requests.post(supabase_url, json=data, headers=headers, timeout=10)
        
        if resp.status_code in [200, 201]:
            print(f\"✅ pH {feed['field1']} sincronizado exitosamente\")
            synced += 1
        else:
            print(f\"❌ Error sincronizando pH {feed['field1']}: {resp.status_code} - {resp.text}\")
        
        time.sleep(0.5)  # Evitar rate limiting
        
    except Exception as e:
        print(f\"❌ Error procesando pH {feed['field1']}: {e}\")

print(f'\\n🎯 Sincronización completada: {synced} registros sincronizados')
"

echo -e "\n3️⃣ Verificando resultados..."
echo "-----------------------------"

# Verificar datos sincronizados
SYNCED_DATA=$(curl -s -X GET "${SUPABASE_URL}/rest/v1/ph_readings?select=*&device=eq.ThingSpeak&order=created_at.desc&limit=5" \
  -H "apikey: $SUPABASE_KEY" \
  -H "Authorization: Bearer $SUPABASE_KEY")

echo "Registros de ThingSpeak en Supabase:"
echo "$SYNCED_DATA" | python3 -c "
import sys, json
try:
    data = json.load(sys.stdin)
    if isinstance(data, list) and len(data) > 0:
        print(f'Total encontrado: {len(data)} registros')
        for record in data:
            print(f\"  pH: {record['ph']} | {record['timestamp']} | Device: {record['device']}\")
    else:
        print('No hay registros de ThingSpeak en Supabase')
except Exception as e:
    print(f'Error: {e}')
"

# Limpiar archivos temporales
rm -f /tmp/thingspeak_feeds.json

echo -e "\n✅ SINCRONIZACIÓN COMPLETADA"
echo "============================"
