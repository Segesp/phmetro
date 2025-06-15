#!/bin/bash

# ===============================================
# DIAGNÓSTICO COMPLETO - SUPABASE CONEXIÓN
# ===============================================

echo "🔍 DIAGNÓSTICO COMPLETO DE SUPABASE"
echo "====================================="

# Variables de configuración
SUPABASE_URL="https://japrhhtnijidlecqrkpk.supabase.co"
SUPABASE_KEY="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM"

echo "📊 1. Verificando últimos 5 registros..."
echo "----------------------------------------"
curl -s -X GET "${SUPABASE_URL}/rest/v1/ph_readings?select=*&order=created_at.desc&limit=5" \
  -H "apikey: $SUPABASE_KEY" \
  -H "Authorization: Bearer $SUPABASE_KEY" | \
  python3 -m json.tool 2>/dev/null || echo "Error en formato JSON"

echo -e "\n📈 2. Contando total de registros..."
echo "-----------------------------------"
TOTAL=$(curl -s -X GET "${SUPABASE_URL}/rest/v1/ph_readings?select=count" \
  -H "apikey: $SUPABASE_KEY" \
  -H "Authorization: Bearer $SUPABASE_KEY" | \
  python3 -c "import sys, json; print(json.load(sys.stdin)[0]['count'])" 2>/dev/null || echo "Error")
echo "Total de registros: $TOTAL"

echo -e "\n🧪 3. Insertando registro de prueba..."
echo "-------------------------------------"
TEST_PH=$(echo "scale=2; $RANDOM/32767 * 6 + 6" | bc)
echo "Insertando pH: $TEST_PH"

RESULT=$(curl -s -X POST "${SUPABASE_URL}/rest/v1/ph_readings" \
  -H "apikey: $SUPABASE_KEY" \
  -H "Authorization: Bearer $SUPABASE_KEY" \
  -H "Content-Type: application/json" \
  -H "Prefer: return=representation" \
  -d "{\"ph\": $TEST_PH}")

if [[ "$RESULT" == *"id"* ]]; then
    echo "✅ Inserción exitosa"
    echo "$RESULT" | python3 -m json.tool 2>/dev/null
else
    echo "❌ Error en inserción: $RESULT"
fi

echo -e "\n🔗 4. Probando endpoints Vercel..."
echo "---------------------------------"

# Probar endpoint directo de Vercel
echo "Probando /api/ph-proxy directo..."
VERCEL_DIRECT=$(curl -s -X POST https://phmetro-phi.vercel.app/api/ph-proxy \
  -H "Content-Type: application/json" \
  -d "{\"ph\": 7.77, \"device\": \"DIAGNOSTIC\"}" \
  --max-time 10)

if [[ "$VERCEL_DIRECT" == *"success"* ]]; then
    echo "✅ Vercel directo: OK"
else
    echo "❌ Vercel directo: FAIL"
    echo "Respuesta: $VERCEL_DIRECT"
fi

# Probar Cloudflare Worker
echo -e "\nProbando Cloudflare Worker..."
WORKER_RESULT=$(curl -s -X POST https://esp8266-phmetro-proxy.20200205.workers.dev/api/ph-proxy \
  -H "Content-Type: application/json" \
  -d "{\"ph\": 8.88, \"device\": \"WORKER_TEST\"}" \
  --max-time 10)

if [[ "$WORKER_RESULT" == *"success"* ]]; then
    echo "✅ Cloudflare Worker: OK"
else
    echo "❌ Cloudflare Worker: FAIL"
    echo "Respuesta: $WORKER_RESULT"
fi

echo -e "\n📊 5. Verificando últimos registros después de pruebas..."
echo "--------------------------------------------------------"
curl -s -X GET "${SUPABASE_URL}/rest/v1/ph_readings?select=*&order=created_at.desc&limit=3" \
  -H "apikey: $SUPABASE_KEY" \
  -H "Authorization: Bearer $SUPABASE_KEY" | \
  python3 -m json.tool 2>/dev/null || echo "Error en formato JSON"

echo -e "\n🎯 DIAGNÓSTICO COMPLETADO"
echo "========================="
