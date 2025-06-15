# MANUAL DE CONEXIÓN ARDUINO UNO + ESP8266 + SENSOR pH

## 🔌 DIAGRAMA DE CONEXIONES

### Arduino Uno (Controlador Principal)
```
Arduino Uno
┌─────────────────┐
│  [DIGITAL]      │
│  0  1  2  3  4  │  ← Pin 0,1: Comunicación Serial con ESP8266
│  5  6  7  8  9  │
│  10 11 12 13    │
│                 │
│  [ANALOG]       │
│  A0 A1 A2 A3    │  ← Pin A0: Sensor pH
│  A4 A5          │
│                 │
│  [POWER]        │
│  3.3V 5V GND    │  ← Alimentación ESP8266 y sensores
│  VIN             │
└─────────────────┘
```

### ESP8266 (Módulo WiFi)
```
ESP8266 NodeMCU/Wemos D1 Mini
┌─────────────────┐
│  3.3V    GND    │  ← Conectar a Arduino 3.3V y GND
│  RX      TX     │  ← RX→Pin1(TX), TX→Pin0(RX) del Arduino
│  D0  D1  D2  D3 │
│  D4  D5  D6  D7 │
│  D8      A0     │
└─────────────────┘
```

### Sensor pH
```
Sensor pH (Tipo analógico)
┌─────────────────┐
│ VCC (5V)        │  ← Conectar a 5V del Arduino
│ GND             │  ← Conectar a GND del Arduino  
│ OUT (Señal)     │  ← Conectar a pin A0 del Arduino
│ (Sonda pH)      │  ← Sumergir en la solución
└─────────────────┘
```

## 🔧 CONEXIONES PASO A PASO

### 1. Conexión del Sensor pH al Arduino Uno
| Sensor pH | Arduino Uno |
|-----------|-------------|
| VCC       | 5V          |
| GND       | GND         |
| OUT       | A0          |

### 2. Conexión del ESP8266 al Arduino Uno
| ESP8266   | Arduino Uno |
|-----------|-------------|
| 3.3V      | 3.3V        |
| GND       | GND         |
| RX        | Pin 1 (TX)  |
| TX        | Pin 0 (RX)  |

### 3. Alimentación
- **Arduino Uno**: USB o Jack DC (7-12V)
- **ESP8266**: Se alimenta del Arduino (3.3V)
- **Sensor pH**: Se alimenta del Arduino (5V)

## 📁 CÓDIGOS A SUBIR

### 1. Arduino Uno - Código Principal
**Archivo**: `Codigo_arduino_uno_completo.ino`
- Lee el sensor pH cada 2 segundos
- Calcula promedio de 10 lecturas
- Envía datos cada 15 segundos via ESP8266
- Modo debug configurable

### 2. ESP8266 - Módulo WiFi
**Archivo**: `Codigo_esp8266_modulo.ino`
- Maneja comandos AT del Arduino
- Conecta a WiFi automáticamente
- Envía datos HTTPS al servidor
- Reconexión automática

## ⚙️ CONFIGURACIÓN

### En el código del Arduino Uno:
```cpp
// Configurar para sensor real o simulación
#define MODE_REAL_SENSOR 1      // 1=sensor real, 0=simulación
#define DEBUG_VERBOSE 1         // 1=logs detallados, 0=silencioso

// Configuración del sensor
const int phPin = A0;           // Pin del sensor
const float phMin = 0.0;        // pH mínimo
const float phMax = 14.0;       // pH máximo

// Configuración de WiFi (para ESP8266)
const String wifiSSID = "SEGOVIA3";
const String wifiPassword = "76840574";
```

### En el código del ESP8266:
```cpp
// Configuración WiFi
const char* ssid = "SEGOVIA3";
const char* password = "76840574";
const char* serverUrl = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";
```

## 🚀 PROCESO DE CARGA

### 1. Programar el ESP8266
1. Conectar ESP8266 al PC via USB
2. Seleccionar placa: "NodeMCU 1.0" o "LOLIN(WEMOS) D1 R2 & mini"
3. Cargar `Codigo_esp8266_modulo.ino`
4. Verificar que se conecte a WiFi en el monitor serial

### 2. Programar el Arduino Uno
1. **DESCONECTAR** los cables RX/TX del ESP8266 temporalmente
2. Conectar Arduino Uno al PC via USB
3. Seleccionar placa: "Arduino Uno"
4. Cargar `Codigo_arduino_uno_completo.ino`
5. **RECONECTAR** los cables RX/TX del ESP8266

### 3. Verificar Funcionamiento
1. Abrir monitor serial del Arduino (9600 baud)
2. Verificar que aparezcan los mensajes de inicio
3. Confirmar conexión WiFi del ESP8266
4. Observar las lecturas de pH y transmisiones

## 📊 SALIDA ESPERADA EN MONITOR SERIAL

```
===============================================
    pH METRO ARDUINO UNO + ESP8266
===============================================
Modo: SIMULACION (o SENSOR REAL)
Debug: ACTIVADO
Intervalo lectura: 2 segundos
Intervalo envio: 15 segundos
Promedio de: 10 lecturas
===============================================

[ESP8266] Configurando modulo WiFi...
ESP8266 WiFi Module Ready
Connecting to WiFi: SEGOVIA3
WiFi Connected!
IP: 192.168.1.100

*** SISTEMA LISTO ***
Iniciando monitoreo de pH...

[pH-READ] Lectura #1: 7.234 | Promedio: 7.234
[SENSOR] ADC: 512 | Voltaje: 2.500V | pH: 7.234
[pH-READ] Lectura #2: 7.189 | Promedio: 7.212
...

##################################################
# TRANSMISION #1 - ENVIANDO DATOS AL SERVIDOR
##################################################
Tiempo de funcionamiento: 15 segundos
[JSON] Payload: {"ph":7.215,"device":"Arduino_Uno"...}
[HTTP] Request enviado
```

## 🔍 SOLUCIÓN DE PROBLEMAS

### Problema: No hay lecturas del sensor
- Verificar conexiones del sensor pH
- Comprobar alimentación (5V)
- Cambiar a modo simulación para probar

### Problema: ESP8266 no conecta a WiFi
- Verificar credenciales WiFi
- Comprobar alimentación 3.3V
- Verificar conexiones RX/TX

### Problema: No se envían datos
- Verificar conexión WiFi del ESP8266
- Comprobar URL del servidor
- Revisar monitor serial para errores

### Problema: Datos inconsistentes
- Calibrar sensor pH con soluciones conocidas
- Ajustar fórmula de conversión ADC→pH
- Aumentar número de muestras para promedio

## 📋 LISTA DE VERIFICACIÓN

- [ ] Sensor pH conectado a A0, 5V y GND
- [ ] ESP8266 conectado a pins 0,1 y alimentación 3.3V
- [ ] Código cargado en ambos dispositivos
- [ ] WiFi configurado correctamente
- [ ] Monitor serial muestra actividad
- [ ] Dashboard web recibe datos
- [ ] Sensor calibrado (si se usa sensor real)

## 🔧 CALIBRACIÓN DEL SENSOR pH

Para usar un sensor real de pH, necesitas calibrarlo:

1. **Solución pH 7 (neutra)**:
   - Sumergir sensor en solución pH 7
   - Anotar valor ADC obtenido
   - Este será el punto medio

2. **Solución pH 4 (ácida)**:
   - Sumergir sensor en solución pH 4
   - Anotar valor ADC obtenido

3. **Solución pH 10 (básica)**:
   - Sumergir sensor en solución pH 10
   - Anotar valor ADC obtenido

4. **Calcular fórmula**:
   ```cpp
   // Ejemplo con valores reales
   // pH 4 → ADC 300
   // pH 7 → ADC 512  
   // pH 10 → ADC 724
   
   float voltage = (adcValue * 5.0) / 1024.0;
   float ph = (voltage - 2.5) * 3.0 + 7.0; // Ajustar según calibración
   ```

## 📡 ESTRUCTURA DE DATOS ENVIADOS

```json
{
  "ph": 7.234,
  "device": "Arduino_Uno",
  "sensor": "pH_probe",
  "transmission": 15,
  "mode": "real_sensor",
  "uptime": 225,
  "readings_count": 10
}
```
