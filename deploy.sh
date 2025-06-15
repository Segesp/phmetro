#!/bin/bash

# Script de despliegue automatizado para pH Metro Dashboard
# Autor: GitHub Copilot
# Fecha: $(date)

echo "🚀 DESPLEGANDO pH METRO DASHBOARD EN VERCEL"
echo "==========================================="

# Verificar que estamos en el directorio correcto
if [ ! -f "package.json" ]; then
    echo "❌ Error: No se encontró package.json. Ejecute desde el directorio raíz del proyecto."
    exit 1
fi

# Instalar Vercel CLI si no está instalado
if ! command -v vercel &> /dev/null; then
    echo "📦 Instalando Vercel CLI..."
    npm install -g vercel
fi

# Verificar que las dependencias están instaladas
if [ ! -d "node_modules" ]; then
    echo "📦 Instalando dependencias..."
    npm install
fi

# Verificar que el build funciona
echo "🔨 Verificando build..."
npm run build

if [ $? -ne 0 ]; then
    echo "❌ Error en el build. Revisar errores antes de desplegar."
    exit 1
fi

echo "✅ Build exitoso!"

# Mostrar información del proyecto
echo ""
echo "📋 INFORMACIÓN DEL PROYECTO:"
echo "- Proyecto: pH Metro Dashboard"
echo "- Framework: Next.js 14"
echo "- Base de datos: Supabase"
echo "- Repositorio: $(git remote get-url origin)"
echo "- Último commit: $(git log --oneline -1)"

echo ""
echo "⚙️ VARIABLES DE ENTORNO REQUERIDAS:"
echo "- NEXT_PUBLIC_SUPABASE_URL"
echo "- NEXT_PUBLIC_SUPABASE_ANON_KEY" 
echo "- SUPABASE_SERVICE_ROLE_KEY"

echo ""
echo "🔗 INSTRUCCIONES DE DESPLIEGUE:"
echo "1. Ir a https://vercel.com"
echo "2. Hacer login con GitHub"
echo "3. Importar este repositorio"
echo "4. Configurar variables de entorno"
echo "5. Desplegar"

echo ""
echo "📱 DESPUÉS DEL DESPLIEGUE:"
echo "1. Actualizar URL en código Arduino"
echo "2. Probar API endpoints"
echo "3. Verificar dashboard en producción"

echo ""
echo "✅ Script completado. Repositorio listo para despliegue en Vercel."
