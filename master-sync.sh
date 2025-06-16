#!/bin/bash

# Script maestro para gestionar sincronización ThingSpeak → Supabase
# Uso: ./master-sync.sh [option]

show_help() {
    echo "🔧 Sistema de Sincronización ThingSpeak → Supabase"
    echo "=================================================="
    echo ""
    echo "Uso: $0 [option]"
    echo ""
    echo "Opciones:"
    echo "  start-local     - Iniciar sincronización local (30s)"
    echo "  start-vercel    - Iniciar sincronización vía Vercel (60s)"
    echo "  stop            - Detener todas las sincronizaciones"
    echo "  status          - Ver estado de sincronizaciones"
    echo "  sync-now        - Ejecutar sincronización manual"
    echo "  logs            - Ver logs de sincronización"
    echo "  configure       - Mostrar instrucciones para webhook"
    echo "  test            - Probar conectividad y datos"
    echo ""
    echo "Ejemplos:"
    echo "  $0 start-local     # Iniciar sincronización cada 30s"
    echo "  $0 sync-now        # Sincronizar ahora mismo"
    echo "  $0 status          # Ver qué está corriendo"
}

start_local() {
    echo "🚀 Iniciando sincronización local (cada 30s)..."
    ./sync-control.sh start
}

start_vercel() {
    echo "🌐 Iniciando sincronización vía Vercel (cada 60s)..."
    echo "⚠️  Nota: Este proceso corre en foreground. Usa Ctrl+C para detener."
    echo "💡 Para background: nohup ./sync-vercel.sh > sync-vercel.log 2>&1 &"
    echo ""
    ./sync-vercel.sh
}

stop_all() {
    echo "🛑 Deteniendo todas las sincronizaciones..."
    ./sync-control.sh stop
    pkill -f sync-vercel.sh
    pkill -f sync-intelligent.sh
    echo "✅ Procesos detenidos"
}

check_status() {
    echo "📊 Estado del Sistema de Sincronización"
    echo "======================================="
    echo ""
    
    # Estado sincronización local
    ./sync-control.sh status
    echo ""
    
    # Procesos de sincronización Vercel
    if pgrep -f sync-vercel.sh > /dev/null; then
        echo "✅ Sincronización Vercel corriendo (PID: $(pgrep -f sync-vercel.sh))"
    else
        echo "❌ Sincronización Vercel no está corriendo"
    fi
    echo ""
    
    # Último dato en ThingSpeak vs Supabase
    echo "📡 Verificando datos..."
    
    latest_ts=$(curl -s "https://api.thingspeak.com/channels/2988488/feeds.json?api_key=Z6SC5MLLP0FR4PC4&results=1" | jq -r '.feeds[0].created_at')
    latest_sb=$(curl -s -X GET "https://japrhhtnijidlecqrkpk.supabase.co/rest/v1/ph_readings?device=eq.ThingSpeak&order=timestamp.desc&limit=1" -H "apikey: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM" | jq -r '.[0].timestamp')
    
    echo "   ThingSpeak (último): $latest_ts"
    echo "   Supabase (último):   $latest_sb"
    
    if [ "$latest_ts" = "$latest_sb" ]; then
        echo "✅ Datos sincronizados"
    else
        echo "⚠️  Datos desactualizados - ejecutar sync-now"
    fi
}

sync_now() {
    echo "🔄 Ejecutando sincronización manual..."
    response=$(curl -s -X POST "https://phmetro.vercel.app/api/sync-thingspeak")
    synced=$(echo "$response" | jq -r '.synced')
    
    if [ "$synced" != "null" ]; then
        echo "✅ Sincronizados $synced registros"
    else
        echo "❌ Error en sincronización"
        echo "$response" | jq '.'
    fi
}

show_logs() {
    echo "📝 Logs de Sincronización"
    echo "========================="
    
    if [ -f "sync-auto.log" ]; then
        echo ""
        echo "=== Log Local ==="
        tail -10 sync-auto.log
    fi
    
    if [ -f "sync-vercel.log" ]; then
        echo ""
        echo "=== Log Vercel ==="
        tail -10 sync-vercel.log
    fi
}

test_system() {
    echo "🧪 Probando Sistema de Sincronización"
    echo "====================================="
    echo ""
    
    echo "1. Conectividad ThingSpeak..."
    if curl -s "https://api.thingspeak.com/channels/2988488/feeds.json?api_key=Z6SC5MLLP0FR4PC4&results=1" > /dev/null; then
        echo "   ✅ ThingSpeak accesible"
    else
        echo "   ❌ Error accediendo a ThingSpeak"
    fi
    
    echo "2. Conectividad Supabase..."
    if curl -s -X GET "https://japrhhtnijidlecqrkpk.supabase.co/rest/v1/ph_readings?limit=1" -H "apikey: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM" > /dev/null; then
        echo "   ✅ Supabase accesible"
    else
        echo "   ❌ Error accediendo a Supabase"
    fi
    
    echo "3. Endpoint de sincronización..."
    if curl -s "https://phmetro.vercel.app/api/sync-thingspeak" > /dev/null; then
        echo "   ✅ Endpoint de Vercel accesible"
    else
        echo "   ❌ Error accediendo al endpoint"
    fi
    
    echo ""
    echo "✅ Test completado"
}

case "$1" in
    start-local)
        start_local
        ;;
    start-vercel)
        start_vercel
        ;;
    stop)
        stop_all
        ;;
    status)
        check_status
        ;;
    sync-now)
        sync_now
        ;;
    logs)
        show_logs
        ;;
    configure)
        ./configure-webhook.sh
        ;;
    test)
        test_system
        ;;
    *)
        show_help
        ;;
esac
