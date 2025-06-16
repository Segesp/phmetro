#!/bin/bash

# Script de sincronización que usa el endpoint de Vercel
# Más confiable que el script local

echo "🌐 Sincronización Automática usando Vercel Endpoint"
echo "======================================================"
echo "📅 $(date)"
echo "⏰ Intervalo: 60 segundos"
echo "🛑 Para detener: Ctrl+C"
echo ""

count=0

while true; do
    count=$((count + 1))
    timestamp=$(date '+%H:%M:%S')
    
    echo "🔄 Sync #$count - $timestamp"
    
    # Usar el endpoint de Vercel para sincronización
    response=$(curl -s -X POST "https://phmetro.vercel.app/api/sync-thingspeak")
    
    # Extraer el número de registros sincronizados
    synced=$(echo "$response" | grep -o '"synced":[0-9]*' | grep -o '[0-9]*')
    
    if [ "$synced" = "0" ]; then
        echo "✅ Sin datos nuevos"
    elif [ -n "$synced" ]; then
        echo "✅ $synced nuevos registros sincronizados"
    else
        echo "❌ Error en sincronización"
    fi
    
    echo "⏸️  Esperando 60s..."
    sleep 60
done
