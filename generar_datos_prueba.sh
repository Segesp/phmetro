#!/bin/bash

# Script para generar datos de prueba del pH Metro
# Simula los datos que enviaría el Arduino

SERVER_URL="https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data"

echo "🧪 GENERADOR DE DATOS DE PRUEBA - pH METRO"
echo "=========================================="
echo "📡 Servidor: $SERVER_URL"
echo "🕐 Generando datos cada 5 segundos..."
echo "⭐ Presiona Ctrl+C para detener"
echo ""

# Array de valores pH para simular diferentes condiciones
ph_values=(7.25 6.85 7.45 8.12 6.35 7.89 7.02 6.78 8.25 7.15 6.92 7.58 8.45 6.15 7.75)

counter=1

while true; do
    # Seleccionar pH aleatorio del array
    index=$((RANDOM % ${#ph_values[@]}))
    ph_value=${ph_values[$index]}
    
    # Agregar variación pequeña (+/- 0.1)
    variation=$((RANDOM % 21 - 10))  # -10 to 10
    ph_adjusted=$(echo "$ph_value + $variation * 0.01" | bc -l)
    
    # Formatear a 2 decimales
    ph_final=$(printf "%.2f" $ph_adjusted)
    
    # Crear timestamp actual
    timestamp=$(date -u +"%Y-%m-%dT%H:%M:%S.%3NZ")
    
    # Construir JSON
    json_data="{
        \"ph\": $ph_final,
        \"timestamp\": \"$timestamp\",
        \"device\": \"SIMULADOR_PRUEBA\",
        \"test_number\": $counter,
        \"mode\": \"script_test\",
        \"classification\": \"$(classify_ph $ph_final)\"
    }"
    
    echo "📊 Envío #$counter - pH: $ph_final"
    echo "📤 JSON: $json_data"
    
    # Enviar datos al servidor
    response=$(curl -s -X POST "$SERVER_URL" \
        -H "Content-Type: application/json" \
        -d "$json_data")
    
    if [ $? -eq 0 ]; then
        echo "✅ Enviado exitosamente"
        echo "📥 Respuesta: $response"
    else
        echo "❌ Error en el envío"
    fi
    
    echo "----------------------------------------"
    
    ((counter++))
    sleep 5
done

# Función para clasificar pH
classify_ph() {
    ph=$1
    if (( $(echo "$ph < 6.5" | bc -l) )); then
        echo "ÁCIDO"
    elif (( $(echo "$ph > 8.5" | bc -l) )); then
        echo "BÁSICO"  
    else
        echo "NEUTRO"
    fi
}
