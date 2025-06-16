#!/bin/bash

# Script para ejecutar sincronización automática cada minuto
# Uso: ./auto-sync.sh

echo "🚀 Iniciando sincronización automática ThingSpeak → Supabase"
echo "📅 $(date)"
echo "⏰ Presiona Ctrl+C para detener"
echo ""

# Contador de ejecuciones
count=0

while true; do
    count=$((count + 1))
    echo "🔄 Sincronización #$count - $(date '+%H:%M:%S')"
    
    # Ejecutar sincronización
    node sync-manual.js
    
    echo "⏸️  Esperando 60 segundos..."
    echo ""
    
    # Esperar 60 segundos
    sleep 60
done
