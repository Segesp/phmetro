# 📟 CÓDIGO ARDUINO FINAL UNIFICADO - pH Metro

## 🎯 Archivo Único

**`Codigo_Arduino_FINAL_UNIFICADO.ino`** - Código final que sintetiza todas las versiones anteriores

## ✨ Características Integradas

### 🔧 Funcionalidades Principales
- ✅ **ESP8266 Standalone** - Código optimizado para ESP8266 independiente
- ✅ **Simulación de datos** - Genera datos pH realistas para pruebas
- ✅ **Lectura de sensor real** - Soporte para sensor pH físico
- ✅ **Debug configurable** - Logging detallado activable/desactivable
- ✅ **Modo de pruebas rápidas** - Envío cada 10 segundos para testing
- ✅ **Transmisión robusta** - Reintentos automáticos y manejo de errores
- ✅ **Reconexión automática** - WiFi y servidor con recuperación automática
- ✅ **Validación de datos** - Verificación de rangos y datos válidos
- ✅ **Información del sistema** - Diagnósticos completos del ESP8266

### 🎛️ Configuraciones Flexibles

```cpp
// CONFIGURACIÓN PRINCIPAL
const char* ssid       = "SEGOVIA3";
const char* password   = "76840574";
const char* serverUrl  = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";

// CONFIGURACIÓN DE OPERACIÓN
const unsigned long sendInterval = 30000;  // 30 segundos normal
const bool debugMode = true;               // Debug detallado
const bool simulationMode = true;          // Simular datos / sensor real
const bool rapidTestMode = false;          // Modo pruebas rápidas (10 seg)
```

### 📊 Modos de Operación

#### 1. **Modo Producción Normal**
```cpp
const bool debugMode = false;
const bool simulationMode = false;  // Usar sensor real
const bool rapidTestMode = false;   // 30 segundos
```

#### 2. **Modo Debug Completo**
```cpp
const bool debugMode = true;
const bool simulationMode = true;   // Datos simulados
const bool rapidTestMode = false;
```

#### 3. **Modo Pruebas Rápidas**
```cpp
const bool debugMode = true;
const bool simulationMode = true;
const bool rapidTestMode = true;    // 10 segundos
```

#### 4. **Modo Sensor Real con Debug**
```cpp
const bool debugMode = true;
const bool simulationMode = false;  // Leer sensor físico
const bool rapidTestMode = false;
```

## 🔌 Conexión del Hardware

### Configuración Básica ESP8266 + Sensor pH
```
ESP8266 NodeMCU    →    Sensor pH
================       ==========
A0                 →    Señal analógica del sensor
3.3V              →    VCC del sensor  
GND               →    GND del sensor
```

### Configuración Arduino UNO + ESP8266 (Opcional)
```
Arduino UNO        →    ESP8266        →    Sensor pH
===========             ========            ==========
A0                 →                   →    Señal analógica
Digital 0,1        →    RX, TX         
3.3V              →    VCC            →    VCC sensor
GND               →    GND            →    GND sensor
```

## 📋 Instrucciones de Uso

### 1. **Preparación Arduino IDE**
- Instalar soporte ESP8266: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
- Seleccionar Board: `NodeMCU 1.0 (ESP-12E Module)`
- Configurar velocidad: `115200 baud`

### 2. **Configurar el Código**
1. Abrir `Codigo_Arduino_FINAL_UNIFICADO.ino`
2. Verificar configuración WiFi (SSID y password)
3. Verificar URL del servidor
4. Ajustar modos según necesidad
5. Compilar y subir al ESP8266

### 3. **Monitoreo Serial**
```
=== pH METRO ESP8266 - CÓDIGO FINAL UNIFICADO ===
📊 Version: 1.0.0 Final
🔧 Todas las funcionalidades integradas
🧪 Modo flexible: Simulación y sensor real

⚙️ CONFIGURACIÓN ACTIVA:
📡 WiFi: SEGOVIA3
🌐 Servidor: https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data
⏱️ Intervalo: 30 segundos
🧪 Simulación: SÍ
🔍 Debug: SÍ

🌐 CONECTANDO A WiFi...
✅ WiFi CONECTADO
📍 IP: 192.168.1.100
📶 RSSI: -45 dBm

✅ SISTEMA COMPLETAMENTE OPERATIVO
🎯 Listo para recibir y transmitir datos

📡 TRANSMISIÓN #1
🧪 GENERANDO pH SIMULADO:
  📊 pH final: 7.234
📋 DATOS A TRANSMITIR:
  📊 pH: 7.234
  🧪 Clasificación: NEUTRO/ÓPTIMO
🚀 INTENTO 1/3
📤 JSON (156 bytes): {"ph":7.234,"timestamp":"","device":"ESP8266"...}
📥 Código: 200 | Tiempo: 234ms
✅ TRANSMISIÓN EXITOSA
```

## 🧪 Comandos Serie (Opcionales)

El código incluye comandos serie para control en tiempo real:
- `debug_on` - Activar debug mode
- `debug_off` - Desactivar debug mode  
- `rapid_on` - Activar modo pruebas rápidas
- `rapid_off` - Desactivar modo pruebas rápidas
- `status` - Mostrar estado del sistema
- `test` - Enviar datos de prueba

## 🔧 Personalización

### Calibración del Sensor pH
Si usas sensor real, ajusta la función `readRealSensor()`:
```cpp
float readRealSensor() {
  // ... código de lectura ...
  
  // AJUSTAR ESTA FÓRMULA SEGÚN TU SENSOR:
  float phValue = 7.0 + ((voltage - 1.65) / 0.18);
  
  // Valores típicos de calibración:
  // pH 7 = 1.65V (punto medio)
  // pH 4 = 1.11V  
  // pH 10 = 2.19V
  
  return phValue;
}
```

### Intervalos de Transmisión
```cpp
// Para monitoreo continuo (cada 10 segundos)
const unsigned long sendInterval = 10000;

// Para aplicaciones normales (cada 30 segundos)  
const unsigned long sendInterval = 30000;

// Para ahorro de energía (cada 5 minutos)
const unsigned long sendInterval = 300000;
```

## 🎯 Ventajas del Código Unificado

### ✅ Simplicidad
- **Un solo archivo** para todas las funciones
- **Configuración clara** en la parte superior
- **Modos predefinidos** para diferentes casos de uso

### ✅ Flexibilidad
- **Sensor real o simulación** con un solo parámetro
- **Debug detallado o silencioso** según necesidad
- **Intervalos configurables** para diferentes aplicaciones

### ✅ Robustez
- **Manejo de errores completo** con reintentos
- **Reconexión automática** WiFi y servidor
- **Validación de datos** antes de transmitir
- **Recuperación de errores** sin reiniciar

### ✅ Debugging
- **Logging detallado** de todas las operaciones
- **Información del sistema** en tiempo real
- **Análisis de transmisión** paso a paso
- **Comandos serie** para control dinámico

## 🌐 Verificación del Sistema

### 1. **Verificar Conectividad**
```bash
# Ping al servidor:
ping phmetro-p5w79503p-segesps-projects.vercel.app
```

### 2. **Probar API**
```bash
curl -X POST https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data \
  -H "Content-Type: application/json" \
  -d '{"ph": 7.25}'
```

### 3. **Ver Dashboard**
Abrir: https://phmetro-p5w79503p-segesps-projects.vercel.app/

## 📈 Próximos Pasos

1. **Subir código** al ESP8266
2. **Verificar transmisión** en Serial Monitor  
3. **Confirmar datos** en el dashboard web
4. **Calibrar sensor** si usas hardware real
5. **Ajustar intervalos** según tu aplicación

---

## 🎉 ¡Código Completamente Unificado!

Este código final incluye **todas las funcionalidades** de las versiones anteriores en un **único archivo limpio y configurable**. Ya no necesitas múltiples versiones - simplemente ajusta los parámetros según tus necesidades.

**¡Tu sistema pH Metro está listo para funcionar!** 🚀
