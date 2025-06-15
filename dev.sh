#!/bin/bash

# Script de desarrollo para Dashboard pH Metro

echo "🚀 Iniciando Dashboard pH Metro..."

# Verificar que existe package.json
if [ ! -f "package.json" ]; then
    echo "❌ Error: package.json no encontrado"
    exit 1
fi

# Verificar que existen node_modules
if [ ! -d "node_modules" ]; then
    echo "📦 Instalando dependencias..."
    npm install
fi

# Verificar archivo .env.local
if [ ! -f ".env.local" ]; then
    echo "⚠️  Archivo .env.local no encontrado"
    echo "📝 Copiando .env.local.example..."
    cp .env.local.example .env.local
    echo "✏️  Por favor, edita .env.local con tus credenciales de Supabase"
    echo "📖 Ver DEPLOYMENT.md para más detalles"
    read -p "Presiona Enter cuando hayas configurado .env.local..."
fi

# Verificar puerto disponible
PORT=3000
if lsof -Pi :$PORT -sTCP:LISTEN -t >/dev/null ; then
    echo "⚠️  Puerto $PORT está ocupado"
    PORT=3001
    echo "🔄 Usando puerto $PORT"
fi

# Iniciar servidor de desarrollo
echo "🌐 Iniciando servidor en http://localhost:$PORT"
echo "📊 Dashboard pH Metro listo!"
echo "---"

# Ejecutar Next.js
npm run dev -- -p $PORT
