# 📟 Código Arduino ESP8266 - pH Metro

## 🚀 Versiones Disponibles

### 1. `Codigo_arduino.ino` - Versión Básica ✅
- **Estado**: Listo para usar
- **URL**: Actualizada con la URL real de Vercel
- **WiFi**: Configurado con credenciales correctas
- **Características**: Código simple y funcional

### 2. `Codigo_arduino_mejorado.ino` - Versión Avanzada 🔧
- **Estado**: Versión mejorada con funciones adicionales
- **Características adicionales**:
  - Manejo de errores robusto
  - Reintentos automáticos
  - Reconexión WiFi automática
  - Múltiples muestras para mayor precisión
  - Información de diagnóstico
  - Validación de datos

## ⚙️ Configuración Actual

```cpp
// WiFi
const char* ssid = "SEGOVIA3";
const char* password = "76840574";

// Servidor
const char* serverUrl = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";

// Sensor
const int phPin = A0; // Pin analógico
```

## 🔌 Conexión del Hardware

```
ESP8266 NodeMCU    →    Sensor pH
================       ==========
A0                 →    Señal analógica del sensor
3.3V              →    VCC del sensor
GND               →    GND del sensor
```

## 📋 Instrucciones de Uso

### 1. **Preparar el Entorno Arduino IDE**
```bash
# Instalar ESP8266 Board Package en Arduino IDE:
# File → Preferences → Additional Board Manager URLs:
# http://arduino.esp8266.com/stable/package_esp8266com_index.json

# Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP-12E Module)
```

### 2. **Librerías Necesarias**
```cpp
// Librerías incluidas en ESP8266:
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Opcional para versión mejorada:
#include <ArduinoJson.h>  // Instalar desde Library Manager
```

### 3. **Configuración del Board**
```
Board: "NodeMCU 1.0 (ESP-12E Module)"
Upload Speed: "115200"
CPU Frequency: "80 MHz"
Flash Size: "4MB (FS:2MB OTA:~1019KB)"
Port: Seleccionar puerto COM correcto
```

### 4. **Subir el Código**
1. Conectar ESP8266 via USB
2. Seleccionar puerto correcto
3. Compilar y subir el código
4. Abrir Serial Monitor (115200 baud)

## 📊 Monitoreo Serial

Al ejecutarse, verás salidas como:
```
=== INICIANDO pH METRO ESP8266 ===
Conectando a WiFi: SEGOVIA3
....
✅ WiFi conectado
IP: 192.168.1.100
Señal: -45 dBm
=== SISTEMA LISTO ===
Enviando datos cada 30 segundos
Servidor: https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data

📊 Lectura ADC: 512 → pH: 7.00
📡 Enviando datos (intento 1/3)...
📤 JSON: {"ph":7.00,"timestamp":"","device":"ESP8266","sensor":"pH"}
📥 Respuesta HTTP: 200
📄 Respuesta: {"success":true,"id":"..."}
✅ Datos enviados exitosamente
```

## 🔧 Calibración del Sensor

### Método de Calibración Básica:
1. **Solución pH 7 (neutra)**:
   - Sumergir sensor en solución pH 7
   - Anotar valor ADC mostrado en Serial Monitor
   - Ajustar constantes si es necesario

2. **Solución pH 4 (ácida)**:
   - Repetir proceso con solución pH 4
   - Calcular pendiente de calibración

3. **Ajustar fórmula** en el código:
   ```cpp
   // Ejemplo de calibración personalizada:
   float phValue = ((avgReading - OFFSET) * SLOPE) + INTERCEPT;
   ```

### Valores Típicos:
- **pH 7**: ~512 ADC (punto medio)
- **pH 4**: ~341 ADC 
- **pH 10**: ~682 ADC

## 🌐 Verificación del Sistema

### 1. **Verificar Conectividad**
```bash
# Ping al servidor desde tu computadora:
ping phmetro-p5w79503p-segesps-projects.vercel.app
```

### 2. **Probar API manualmente**
```bash
curl -X POST https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data \
  -H "Content-Type: application/json" \
  -d '{"ph": 7.25, "timestamp": ""}'
```

### 3. **Ver Dashboard**
Abrir en navegador: https://phmetro-p5w79503p-segesps-projects.vercel.app/

## 🛠️ Solución de Problemas

### ❌ WiFi no conecta
- Verificar SSID y contraseña
- Verificar que ESP8266 esté en rango
- Resetear ESP8266 con botón físico

### ❌ HTTP Error -1
- Verificar conexión a internet
- Verificar URL del servidor
- Revisar certificados SSL (problema poco común)

### ❌ Lecturas de pH erráticas
- Verificar conexiones del sensor
- Calibrar sensor con soluciones conocidas
- Aumentar número de muestras promedio

### ❌ Error 404 en API
- Verificar que la URL esté correcta
- Verificar que el servidor esté desplegado

## 📈 Próximos Pasos

1. **Calibración precisa** del sensor con soluciones buffer
2. **Configuración NTP** para timestamps reales
3. **Modo deep sleep** para ahorro de energía
4. **OTA updates** para actualizaciones remotas
5. **Sensor de temperatura** para compensación

## 📞 Soporte

Si tienes problemas:
1. Verifica conexiones físicas
2. Revisa Serial Monitor para errores
3. Verifica que el dashboard web esté funcionando
4. Asegúrate de que la red WiFi tenga acceso a internet

---
🔗 **Dashboard**: https://phmetro-p5w79503p-segesps-projects.vercel.app/
🔧 **API Endpoint**: `/api/ph-data`
📊 **Base de datos**: Supabase (automática)
