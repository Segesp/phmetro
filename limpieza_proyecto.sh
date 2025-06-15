#!/bin/bash

# ===============================================
# SCRIPT DE LIMPIEZA - PROYECTO pH METRO
# ===============================================

echo "🧹 LIMPIEZA DEL PROYECTO pH METRO"
echo "=================================="

# Crear directorio de archivos obsoletos
mkdir -p archivos_obsoletos

# === CÓDIGOS ARDUINO OBSOLETOS ===
echo "📂 Moviendo códigos Arduino obsoletos..."

# Mantener solo los 2 principales
mv "Codigo_Arduino_DIAGNOSTICO_ESP8266.ino" archivos_obsoletos/ 2>/dev/null
mv "Codigo_Arduino_FINAL_UNIFICADO.ino" archivos_obsoletos/ 2>/dev/null
mv "Codigo_Arduino_PRUEBAS_AUTOMATICAS.ino" archivos_obsoletos/ 2>/dev/null
mv "Codigo_Arduino_UNO_ESP8266_MODULO.ino" archivos_obsoletos/ 2>/dev/null
mv "Codigo_arduino_debug_ultraverbose_CORREGIDO.ino" archivos_obsoletos/ 2>/dev/null
mv "Codigo_arduino_pruebas.ino" archivos_obsoletos/ 2>/dev/null
mv "Codigo_arduino_uno_completo.ino" archivos_obsoletos/ 2>/dev/null
mv "Codigo_esp8266_modulo.ino" archivos_obsoletos/ 2>/dev/null

echo "✅ Códigos Arduino obsoletos movidos"

# === DOCUMENTACIÓN OBSOLETA ===
echo "📂 Moviendo documentación obsoleta..."

mv "ARDUINO_MANUAL.md" archivos_obsoletos/ 2>/dev/null
mv "ARDUINO_UNO_ESP8266_ACTUALIZADO.md" archivos_obsoletos/ 2>/dev/null
mv "CODIGO_ARDUINO_FINAL_UNIFICADO.md" archivos_obsoletos/ 2>/dev/null
mv "CONEXION_ARDUINO_UNO_ESP8266.md" archivos_obsoletos/ 2>/dev/null
mv "CONFIGURAR_VERCEL_VARIABLES.md" archivos_obsoletos/ 2>/dev/null
mv "DEPLOYMENT.md" archivos_obsoletos/ 2>/dev/null
mv "DESPLEGAR_AHORA.md" archivos_obsoletos/ 2>/dev/null
mv "ESTADO_FINAL.md" archivos_obsoletos/ 2>/dev/null
mv "GUIA_DESPLIEGUE_VERCEL.md" archivos_obsoletos/ 2>/dev/null
mv "LIMPIEZA_COMPLETADA.md" archivos_obsoletos/ 2>/dev/null
mv "MANUAL_ARDUINO_UNO_ESP8266.md" archivos_obsoletos/ 2>/dev/null
mv "PROBLEMA_RESUELTO.md" archivos_obsoletos/ 2>/dev/null
mv "PROYECTO_COMPLETO.md" archivos_obsoletos/ 2>/dev/null
mv "SOLUCION_API_KEY_ERROR.md" archivos_obsoletos/ 2>/dev/null
mv "SOLUCION_ERROR_VERCEL.md" archivos_obsoletos/ 2>/dev/null
mv "SOLUCION_ERROR_VERCEL_FINAL.md" archivos_obsoletos/ 2>/dev/null
mv "SOLUCION_HTTPS_PROXY.md" archivos_obsoletos/ 2>/dev/null

echo "✅ Documentación obsoleta movida"

# === SCRIPTS OBSOLETOS ===
echo "📂 Moviendo scripts obsoletos..."

mv "deploy.sh" archivos_obsoletos/ 2>/dev/null
mv "dev.sh" archivos_obsoletos/ 2>/dev/null
mv "enviar_datos_prueba.sh" archivos_obsoletos/ 2>/dev/null
mv "generar_datos_prueba.sh" archivos_obsoletos/ 2>/dev/null
mv "generate_ph_data.sh" archivos_obsoletos/ 2>/dev/null
mv "http-proxy-server.js" archivos_obsoletos/ 2>/dev/null
mv "start-proxy.sh" archivos_obsoletos/ 2>/dev/null
mv "thingspeak-dashboard.html" archivos_obsoletos/ 2>/dev/null

echo "✅ Scripts obsoletos movidos"

# === DIRECTORIOS VACÍOS ===
echo "📂 Eliminando directorios vacíos..."

rm -rf carpeta/ 2>/dev/null

echo "✅ Directorios vacíos eliminados"

echo
echo "📊 ARCHIVOS PRINCIPALES MANTENIDOS:"
echo "===================================="
echo "📋 Códigos Arduino (2):"
echo "  - Codigo_Arduino_UNO_ESP8266_OPTIMIZADO.ino ⭐ (Principal)"
echo "  - Codigo_Arduino_UNO_ESP8266_ULTRA_SIMPLE.ino 🔧 (Backup)"
echo
echo "📋 Documentación (5):"
echo "  - README.md ⭐ (Principal)"
echo "  - SISTEMA_DUAL_DASHBOARD_THINGSPEAK.md 📊 (Sistema Dual)"
echo "  - ACTUALIZACION_HTTPS_SSL.md 🔒 (HTTPS)"
echo "  - CLOUDFLARE_SETUP_GUIDE.md ☁️ (Cloudflare)"
echo "  - CODIGOS_ARDUINO_HTTPS_FINAL.md 📱 (Resumen)"
echo
echo "📋 Scripts útiles (2):"
echo "  - diagnostico_https.sh 🔍 (Diagnóstico)"
echo "  - test_dual_envio.sh 🧪 (Pruebas)"
echo
echo "📋 Configuración (4):"
echo "  - cloudflare-worker.js ☁️ (Worker)"
echo "  - deploy-cloudflare.sh 🚀 (Deploy)"
echo "  - supabase-init.sql 🗄️ (DB Init)"
echo "  - supabase-update-device.sql 🔄 (DB Update)"
echo
echo "✅ LIMPIEZA COMPLETADA"
echo "📁 Archivos obsoletos en: ./archivos_obsoletos/"
