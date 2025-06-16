#!/bin/bash

# Script de sincronización automática inteligente
# Sincroniza cada 30 segundos y maneja errores automáticamente

PID_FILE="/tmp/phmetro-sync.pid"
LOG_FILE="sync.log"

# Función para limpiar al salir
cleanup() {
    echo "🛑 Deteniendo sincronización automática..."
    rm -f "$PID_FILE"
    exit 0
}

# Capturar señales para limpieza
trap cleanup SIGTERM SIGINT

# Verificar si ya hay una instancia corriendo
if [ -f "$PID_FILE" ]; then
    OLD_PID=$(cat "$PID_FILE")
    if ps -p "$OLD_PID" > /dev/null 2>&1; then
        echo "❌ Ya hay una instancia de sincronización corriendo (PID: $OLD_PID)"
        echo "   Para detenerla: kill $OLD_PID"
        exit 1
    else
        rm -f "$PID_FILE"
    fi
fi

# Guardar PID actual
echo $$ > "$PID_FILE"

echo "🚀 Sincronización Automática Iniciada"
echo "📅 $(date)"
echo "🔧 PID: $$"
echo "📝 Log: $LOG_FILE"
echo "🛑 Para detener: kill $$"
echo "⏰ Intervalo: 30 segundos"
echo ""

count=0
errors=0

while true; do
    count=$((count + 1))
    timestamp=$(date '+%H:%M:%S')
    
    echo "🔄 Sync #$count - $timestamp"
    
    # Ejecutar sincronización y capturar resultado
    if node sync-manual.js > /tmp/sync_output.log 2>&1; then
        # Sincronización exitosa
        synced=$(grep "registros guardados" /tmp/sync_output.log | grep -o '[0-9]\+' | tail -1)
        if [ "$synced" = "0" ]; then
            echo "✅ Sin datos nuevos"
        else
            echo "✅ $synced nuevos registros"
        fi
        errors=0
    else
        # Error en sincronización
        errors=$((errors + 1))
        echo "❌ Error #$errors"
        
        # Si hay muchos errores seguidos, esperar más tiempo
        if [ $errors -ge 3 ]; then
            echo "⚠️  Muchos errores, esperando 5 minutos..."
            sleep 300
            errors=0
            continue
        fi
    fi
    
    echo "⏸️  Esperando 30s..."
    sleep 30
done
