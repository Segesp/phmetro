#!/bin/bash

# Script para generar datos de pH de demostración
# Simula lecturas de sensor cada pocos segundos

echo "🧪 Generando datos de pH de demostración..."
echo "Presiona Ctrl+C para detener"

# URL de la API (cambiar por la URL de Vercel cuando esté desplegado)
API_URL="http://localhost:3001/api/ph-data"

# Array de valores de pH realistas
ph_values=(6.9 7.0 7.1 7.2 7.3 7.1 6.8 7.4 7.0 7.2 7.1 6.9 7.3 7.0 7.2)

counter=0

while true; do
    # Seleccionar un valor de pH del array
    ph=${ph_values[$((counter % ${#ph_values[@]}))]}
    
    # Agregar una pequeña variación aleatoria
    variation=$(echo "scale=2; (RANDOM % 21 - 10) / 100" | bc 2>/dev/null || echo "0")
    final_ph=$(echo "scale=2; $ph + $variation" | bc 2>/dev/null || echo "$ph")
    
    # Asegurar que esté en un rango realista
    if (( $(echo "$final_ph < 6.0" | bc -l 2>/dev/null || echo "0") )); then
        final_ph="6.0"
    elif (( $(echo "$final_ph > 8.5" | bc -l 2>/dev/null || echo "0") )); then
        final_ph="8.5"
    fi
    
    # Enviar datos a la API
    echo "📊 Enviando pH: $final_ph"
    curl -s -X POST "$API_URL" \
        -H "Content-Type: application/json" \
        -d "{\"ph\": $final_ph}" > /dev/null
    
    counter=$((counter + 1))
    
    # Esperar entre 5-15 segundos
    sleep_time=$((RANDOM % 11 + 5))
    sleep $sleep_time
done
