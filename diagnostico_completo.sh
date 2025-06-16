#!/bin/bash

# ===============================================
# DIAGNÓSTICO SUPABASE - PERMISOS Y RLS
# ===============================================

echo "🔍 DIAGNÓSTICO COMPLETO DE PERMISOS SUPABASE"
echo "============================================="

SUPABASE_URL="https://japrhhtnijidlecqrkpk.supabase.co"
ANON_KEY="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM"

echo "1️⃣ Probando inserción con ANON KEY (como lo hace la API)..."
echo "------------------------------------------------------------"

ANON_TEST=$(curl -s -X POST "${SUPABASE_URL}/rest/v1/ph_readings" \
  -H "apikey: $ANON_KEY" \
  -H "Authorization: Bearer $ANON_KEY" \
  -H "Content-Type: application/json" \
  -H "Prefer: return=representation" \
  -d '{"ph": 3.33, "device": "ANON_KEY_TEST"}')

echo "Resultado ANON_KEY:"
echo "$ANON_TEST" | python3 -m json.tool 2>/dev/null || echo "Error: $ANON_TEST"

echo -e "\n2️⃣ Verificando políticas RLS..."
echo "--------------------------------"

# Consultar configuración de RLS
RLS_STATUS=$(curl -s -X GET "${SUPABASE_URL}/rest/v1/ph_readings?select=count" \
  -H "apikey: $ANON_KEY" \
  -H "Authorization: Bearer $ANON_KEY")

echo "RLS Status (debería mostrar count): $RLS_STATUS"

echo -e "\n3️⃣ Probando inserción via SDK de Supabase (simulando código)..."
echo "----------------------------------------------------------------"

# Crear script temporal de Node.js para probar SDK
cat > /tmp/test_supabase.js << 'EOF'
const { createClient } = require('@supabase/supabase-js');

const supabaseUrl = 'https://japrhhtnijidlecqrkpk.supabase.co';
const supabaseKey = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM';

const supabase = createClient(supabaseUrl, supabaseKey);

async function testInsert() {
  try {
    console.log('Configuración:', { url: supabaseUrl, hasKey: !!supabaseKey });
    
    const { data, error } = await supabase
      .from('ph_readings')
      .insert([{ ph: 2.22, device: 'SDK_TEST' }])
      .select();

    if (error) {
      console.log('❌ Error SDK:', error);
    } else {
      console.log('✅ SDK Success:', data);
    }
  } catch (err) {
    console.log('💥 Exception:', err.message);
  }
}

testInsert();
EOF

# Ejecutar test SDK si Node.js está disponible
if command -v node &> /dev/null; then
    echo "Ejecutando test SDK..."
    cd /workspaces/phmetro && node /tmp/test_supabase.js
else
    echo "Node.js no disponible, saltando test SDK"
fi

echo -e "\n4️⃣ Verificando últimos registros..."
echo "-----------------------------------"

RECENT=$(curl -s -X GET "${SUPABASE_URL}/rest/v1/ph_readings?select=*&order=created_at.desc&limit=5" \
  -H "apikey: $ANON_KEY" \
  -H "Authorization: Bearer $ANON_KEY")

echo "Últimos 5 registros:"
echo "$RECENT" | python3 -m json.tool 2>/dev/null || echo "Error: $RECENT"

echo -e "\n🎯 RESUMEN:"
echo "=========="
echo "Si ANON_KEY funciona pero el SDK falla, el problema está en el código de las APIs"
echo "Si todo funciona aquí pero no en Vercel, el problema son las variables de entorno"

rm -f /tmp/test_supabase.js
