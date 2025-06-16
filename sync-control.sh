#!/bin/bash

# Script de control para la sincronización automática
# Uso: ./sync-control.sh [start|stop|status|restart|logs]

PID_FILE="/tmp/phmetro-sync.pid"
LOG_FILE="sync-auto.log"

case "$1" in
    start)
        if [ -f "$PID_FILE" ]; then
            PID=$(cat "$PID_FILE")
            if ps -p "$PID" > /dev/null 2>&1; then
                echo "✅ Sincronización ya está corriendo (PID: $PID)"
                exit 0
            fi
        fi
        
        echo "🚀 Iniciando sincronización automática..."
        nohup ./sync-intelligent.sh > "$LOG_FILE" 2>&1 &
        echo "✅ Sincronización iniciada en background"
        echo "📝 Ver logs: ./sync-control.sh logs"
        ;;
        
    stop)
        if [ -f "$PID_FILE" ]; then
            PID=$(cat "$PID_FILE")
            if ps -p "$PID" > /dev/null 2>&1; then
                echo "🛑 Deteniendo sincronización (PID: $PID)..."
                kill "$PID"
                rm -f "$PID_FILE"
                echo "✅ Sincronización detenida"
            else
                echo "❌ No hay proceso corriendo con PID: $PID"
                rm -f "$PID_FILE"
            fi
        else
            echo "❌ No hay sincronización corriendo"
        fi
        ;;
        
    status)
        if [ -f "$PID_FILE" ]; then
            PID=$(cat "$PID_FILE")
            if ps -p "$PID" > /dev/null 2>&1; then
                echo "✅ Sincronización corriendo (PID: $PID)"
                echo "⏰ Iniciada: $(ps -p $PID -o lstart= 2>/dev/null)"
                if [ -f "$LOG_FILE" ]; then
                    echo "📊 Último sync: $(tail -1 "$LOG_FILE" | grep "Sync #" || echo "Sin datos")"
                fi
            else
                echo "❌ Proceso no encontrado (PID: $PID)"
                rm -f "$PID_FILE"
            fi
        else
            echo "❌ No hay sincronización corriendo"
        fi
        ;;
        
    restart)
        echo "🔄 Reiniciando sincronización..."
        "$0" stop
        sleep 2
        "$0" start
        ;;
        
    logs)
        if [ -f "$LOG_FILE" ]; then
            echo "📝 Últimas 20 líneas del log:"
            echo "================================"
            tail -20 "$LOG_FILE"
            echo ""
            echo "💡 Para seguir en tiempo real: tail -f $LOG_FILE"
        else
            echo "❌ No se encontró archivo de log: $LOG_FILE"
        fi
        ;;
        
    *)
        echo "🔧 Control de Sincronización Automática"
        echo "Uso: $0 [start|stop|status|restart|logs]"
        echo ""
        echo "Comandos:"
        echo "  start   - Iniciar sincronización automática"
        echo "  stop    - Detener sincronización"
        echo "  status  - Ver estado actual"
        echo "  restart - Reiniciar sincronización"
        echo "  logs    - Ver logs recientes"
        ;;
esac
