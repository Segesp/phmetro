#!/bin/bash

# Script para iniciar el servidor proxy HTTP para ESP8266
# Este servidor permite que ESP8266 envíe datos a Vercel sin SSL

echo "🚀 INICIANDO SERVIDOR PROXY HTTP PARA ESP8266"
echo "=============================================="

# Verificar si Node.js está instalado
if ! command -v node &> /dev/null; then
    echo "❌ Node.js no está instalado"
    echo "Instalar desde: https://nodejs.org/"
    exit 1
fi

# Verificar si npm está instalado
if ! command -v npm &> /dev/null; then
    echo "❌ npm no está disponible"
    exit 1
fi

echo "✅ Node.js detectado: $(node --version)"

# Instalar dependencias si no existen
if [ ! -d "node_modules" ]; then
    echo "📦 Instalando dependencias..."
    npm install express axios
else
    echo "✅ Dependencias ya instaladas"
fi

# Obtener IP local
LOCAL_IP=$(hostname -I | awk '{print $1}' 2>/dev/null || ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1' | head -n1)

echo ""
echo "🌐 CONFIGURACIÓN DE RED:"
echo "   IP Local: $LOCAL_IP"
echo "   Puerto: 8080"
echo "   Target: https://phmetro-phi.vercel.app/api/ph-proxy"

echo ""
echo "📋 CONFIGURAR EN ARDUINO:"
echo "   const char* proxyIP = \"$LOCAL_IP\";"
echo "   const uint16_t proxyPort = 8080;"

echo ""
echo "🔄 Iniciando servidor proxy..."
echo "   Presiona Ctrl+C para detener"
echo ""

# Iniciar el servidor proxy
node http-proxy-server.js
