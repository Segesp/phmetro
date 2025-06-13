#!/bin/bash

# ===============================================
# SCRIPT DE DIAGNÓSTICO - CONECTIVIDAD HTTPS
# ===============================================

echo "🔍 DIAGNÓSTICO DE CONECTIVIDAD HTTPS"
echo "===================================="
echo

# 1. Verificar Worker Cloudflare HTTPS
echo "1️⃣ Probando Worker Cloudflare HTTPS..."
if curl -s --max-time 10 https://esp8266-phmetro-proxy.20200205.workers.dev/info | grep -q "ESP8266"; then
    echo "✅ Worker Cloudflare responde correctamente"
else
    echo "❌ Worker Cloudflare no responde"
fi
echo

# 2. Verificar endpoint proxy HTTPS
echo "2️⃣ Probando endpoint proxy HTTPS..."
response=$(curl -s --max-time 10 -X POST https://esp8266-phmetro-proxy.20200205.workers.dev/api/ph-proxy \
    -H "Content-Type: application/json" \
    -d '{"ph": 7.0, "device": "DIAGNOSTIC"}' \
    -w "%{http_code}")

if [[ "$response" == *"200"* ]] || [[ "$response" == *"201"* ]]; then
    echo "✅ Endpoint proxy funciona correctamente"
else
    echo "❌ Endpoint proxy falló: $response"
fi
echo

# 3. Verificar Vercel directo HTTPS
echo "3️⃣ Probando Vercel directo HTTPS..."
vercel_response=$(curl -s --max-time 10 -X POST https://phmetro-phi.vercel.app/api/ph-proxy \
    -H "Content-Type: application/json" \
    -d '{"ph": 7.0, "device": "DIAGNOSTIC_DIRECT"}' \
    -w "%{http_code}")

if [[ "$vercel_response" == *"200"* ]] || [[ "$vercel_response" == *"201"* ]]; then
    echo "✅ Vercel directo funciona correctamente"
else
    echo "❌ Vercel directo falló: $vercel_response"
fi
echo

# 4. Verificar certificados SSL
echo "4️⃣ Verificando certificados SSL..."
echo "Cloudflare Worker:"
openssl s_client -connect esp8266-phmetro-proxy.20200205.workers.dev:443 -servername esp8266-phmetro-proxy.20200205.workers.dev < /dev/null 2>/dev/null | grep -E "(subject=|issuer=|Verification:)" | head -3
echo

echo "Vercel:"
openssl s_client -connect phmetro-phi.vercel.app:443 -servername phmetro-phi.vercel.app < /dev/null 2>/dev/null | grep -E "(subject=|issuer=|Verification:)" | head -3
echo

# 5. Resumen
echo "📋 RESUMEN DEL DIAGNÓSTICO"
echo "=========================="
echo "✅ = Funcionando   ❌ = Error"
echo
echo "Opciones para el ESP8266:"
echo "1. 🌟 RECOMENDADO: Worker Cloudflare HTTPS (puerto 443)"
echo "2. 🔄 ALTERNATIVO: Vercel directo HTTPS (puerto 443)"
echo "3. ❌ NO USAR: HTTP puerto 80 (bloqueado)"
echo
echo "📝 Configuración recomendada para Arduino:"
echo "- Host: esp8266-phmetro-proxy.20200205.workers.dev"
echo "- Puerto: 443"
echo "- Protocolo: AT+CIPSTART=\"SSL\""
echo "- Timeout: 15 segundos (SSL necesita más tiempo)"
echo
