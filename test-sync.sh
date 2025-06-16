#!/bin/bash

echo "🧪 PRUEBA DE SINCRONIZACIÓN THINGSPEAK → SUPABASE"
echo "=================================================="

echo ""
echo "1️⃣ Probando endpoint de sincronización..."
curl -s -X POST https://phmetro-phi.vercel.app/api/sync-thingspeak \
  -H "Content-Type: application/json" | jq '.'

echo ""
echo "2️⃣ Verificando datos en Supabase vía API..."
curl -s https://phmetro-phi.vercel.app/api/ph-data | jq '.data[:3]'

echo ""
echo "3️⃣ Probando endpoint de diagnóstico..."
curl -s https://phmetro-phi.vercel.app/api/diagnostic | jq '.'

echo ""
echo "✅ Prueba completada. Verifica que:"
echo "   - La sincronización reporte registros guardados"
echo "   - Los datos aparezcan en la consulta de Supabase"
echo "   - El diagnóstico muestre conexiones OK"
