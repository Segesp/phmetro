#!/bin/bash

# Script para desplegar el Cloudflare Worker automáticamente

echo "🚀 DESPLEGANDO CLOUDFLARE WORKER PARA ESP8266"
echo "============================================="

# Verificar si wrangler está instalado
if ! command -v wrangler &> /dev/null; then
    echo "📦 Instalando Wrangler CLI..."
    npm install -g wrangler
else
    echo "✅ Wrangler CLI detectado: $(wrangler --version)"
fi

# Verificar si el usuario está autenticado
echo "🔐 Verificando autenticación..."
if ! wrangler whoami &> /dev/null; then
    echo "🔑 Necesitas autenticarte con Cloudflare"
    echo "   Ejecuta: wrangler login"
    echo "   Luego vuelve a ejecutar este script"
    exit 1
fi

echo "✅ Usuario autenticado: $(wrangler whoami)"

# Desplegar el Worker
echo "📡 Desplegando Worker..."
wrangler publish

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ WORKER DESPLEGADO EXITOSAMENTE!"
    echo ""
    echo "🌐 CONFIGURACIÓN PARA ARDUINO:"
    echo "   const char* proxyDomain = \"esp8266-phmetro-proxy.TU_USUARIO.workers.dev\";"
    echo "   const uint16_t proxyPort = 80;"
    echo "   const char* proxyEndpoint = \"/api/ph-proxy\";"
    echo ""
    echo "📋 PRÓXIMOS PASOS:"
    echo "1. Copiar la URL del Worker mostrada arriba"
    echo "2. Actualizar el código Arduino con esa URL"
    echo "3. Compilar y subir al Arduino UNO"
    echo "4. Los datos irán: ESP8266 → Cloudflare → Vercel → Dashboard"
    echo ""
else
    echo "❌ Error en el deployment"
    echo "Verificar configuración y intentar de nuevo"
fi
