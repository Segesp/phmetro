#!/bin/bash

# ===============================================
# SCRIPT DE PRUEBA DUAL - DASHBOARD + THINGSPEAK
# ===============================================

echo "🧪 PRUEBA DUAL: Dashboard + ThingSpeak"
echo "======================================"

# Valor de pH de prueba
PH_VALUE=${1:-7.25}
echo "📊 Enviando pH: $PH_VALUE"
echo

# 1. Enviar al Dashboard (Vercel/Supabase via Cloudflare Worker)
echo "1️⃣ Enviando al Dashboard..."
DASHBOARD_RESPONSE=$(curl -s -X POST \
  https://esp8266-phmetro-proxy.20200205.workers.dev/api/ph-proxy \
  -H "Content-Type: application/json" \
  -d "{\"ph\": $PH_VALUE, \"device\": \"TEST_DUAL\"}" \
  -w "%{http_code}")

if [[ "$DASHBOARD_RESPONSE" == *"200"* ]] || [[ "$DASHBOARD_RESPONSE" == *"201"* ]]; then
    echo "✅ Dashboard: OK"
else
    echo "❌ Dashboard: FAIL ($DASHBOARD_RESPONSE)"
fi

echo

# 2. Enviar a ThingSpeak
echo "2️⃣ Enviando a ThingSpeak..."
THINGSPEAK_RESPONSE=$(curl -s \
  "https://api.thingspeak.com/update?api_key=I4RFD6P62MTKOR8E&field1=$PH_VALUE")

if [[ "$THINGSPEAK_RESPONSE" =~ ^[0-9]+$ ]] && [[ "$THINGSPEAK_RESPONSE" -gt 0 ]]; then
    echo "✅ ThingSpeak: OK (Entry ID: $THINGSPEAK_RESPONSE)"
else
    echo "❌ ThingSpeak: FAIL ($THINGSPEAK_RESPONSE)"
fi

echo

# 3. Verificar datos
echo "3️⃣ Verificando datos..."
sleep 3

echo "🌐 Dashboard: https://phmetro-phi.vercel.app"
echo "📡 ThingSpeak: https://thingspeak.com/channels/2988488"

echo
echo "📊 Últimos datos ThingSpeak:"
curl -s "https://api.thingspeak.com/channels/2988488/fields/1.json?api_key=Z6SC5MLLP0FR4PC4&results=3" | \
  jq -r '.feeds[] | "  " + .created_at + " → pH: " + .field1'

echo
echo "✅ Prueba dual completada"
