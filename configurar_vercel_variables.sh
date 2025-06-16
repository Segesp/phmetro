#!/bin/bash

# ===============================================
# CONFIGURAR VARIABLES DE ENTORNO EN VERCEL
# ===============================================

echo "🔧 CONFIGURACIÓN DE VERCEL"
echo "=========================="

echo "ℹ️  Las variables de entorno deben configurarse en:"
echo "   https://vercel.com/dashboard/phmetro-phi/settings/environment-variables"
echo ""

echo "📋 Variables requeridas:"
echo "------------------------"
echo "NEXT_PUBLIC_SUPABASE_URL = https://japrhhtnijidlecqrkpk.supabase.co"
echo "NEXT_PUBLIC_SUPABASE_ANON_KEY = eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM"
echo ""

echo "⚡ Aplicar a todos los entornos:"
echo "- [x] Production"
echo "- [x] Preview"  
echo "- [x] Development"
echo ""

echo "🔄 Después de agregar las variables:"
echo "1. Ir a Deployments tab"
echo "2. Hacer Redeploy del último deployment"
echo "3. Esperar que termine el build"
echo ""

echo "🧪 Luego verificar con:"
echo "curl https://phmetro-phi.vercel.app/api/diagnostic"
echo ""

echo "⚠️  IMPORTANTE: Sin estas variables, Supabase no funcionará en Vercel"
