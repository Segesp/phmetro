#!/bin/bash

# Script para enviar datos de prueba al dashboard
echo "🧪 Enviando datos de prueba al dashboard..."

# Función para enviar un dato
send_ph_data() {
    local ph=$1
    local device=${2:-"test_script"}
    
    echo "📊 Enviando pH: $ph"
    
    curl -X POST http://localhost:3000/api/ph-data \
        -H "Content-Type: application/json" \
        -d "{
            \"ph\": $ph,
            \"device\": \"$device\",
            \"timestamp\": \"\",
            \"test\": true
        }" \
        --silent --show-error --max-time 5
    
    echo -e "\n"
}

# Generar y enviar datos de prueba
echo "🚀 Generando 10 lecturas de pH de prueba..."

for i in {1..10}; do
    # Generar pH aleatorio entre 6.5 y 7.5
    ph=$(python3 -c "import random; print(round(6.5 + random.random() * 1.0, 2))")
    send_ph_data $ph "test_script_$i"
    sleep 1
done

echo "✅ Datos de prueba enviados completamente"
echo "🌐 Verifica el dashboard en: http://localhost:3000"
