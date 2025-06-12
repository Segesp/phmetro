# 🔌 CONEXIÓN ARDUINO UNO + ESP8266 MÓDULO + SENSOR pH

## 🎯 Tu Configuración Exacta

### 📋 Componentes:
- **Arduino UNO** (control principal)
- **ESP8266** (módulo WiFi - NO programar)
- **Sensor pH** (conectado a Arduino UNO)

---

## 🔌 Diagrama de Conexiones

```
SENSOR pH          ARDUINO UNO         ESP8266 MÓDULO
=========          ===========         ==============

VCC (Rojo)    →    5V                 
GND (Negro)   →    GND           →    GND
Señal         →    A0                 
              
                   3.3V          →    VCC
                   GND           →    GND  
                   Pin 0 (RX)    →    TX
                   Pin 1 (TX)    →    RX
```

### 🔧 Conexiones Detalladas:

#### Sensor pH → Arduino UNO:
- **VCC del sensor** → **5V de Arduino**
- **GND del sensor** → **GND de Arduino**  
- **Señal analógica** → **A0 de Arduino**

#### ESP8266 → Arduino UNO:
- **VCC del ESP8266** → **3.3V de Arduino** (⚠️ IMPORTANTE: 3.3V, NO 5V)
- **GND del ESP8266** → **GND de Arduino**
- **TX del ESP8266** → **Pin 0 (RX) de Arduino**
- **RX del ESP8266** → **Pin 1 (TX) de Arduino**

---

## ⚠️ Puntos Importantes:

### 1. **ESP8266 - NO programar**
- El ESP8266 viene con **firmware AT commands** de fábrica
- **NO necesitas subir código al ESP8266**
- Solo funciona como módulo WiFi controlado por Arduino UNO

### 2. **Voltaje del ESP8266**
- **3.3V solamente** - NO conectar a 5V
- Si tu Arduino no tiene pin 3.3V suficiente, usar regulador de voltaje

### 3. **Comunicación Serial**
- Arduino UNO controla ESP8266 via **comandos AT**
- Velocidad: **9600 baud** (estándar)
- Protocolo: **AT+comando**

---

## 💻 Código a Usar:

**SOLO subir código al Arduino UNO:**
- ✅ `Codigo_Arduino_UNO_ESP8266_MODULO.ino` → **Arduino UNO**
- ❌ ESP8266 → **Sin código (usa firmware AT de fábrica)**

---

## 🧪 Proceso de Funcionamiento:

1. **Arduino UNO** lee sensor pH cada 2 segundos
2. **Arduino UNO** promedia 10 lecturas  
3. **Arduino UNO** envía comandos AT al ESP8266:
   - `AT+CWJAP="SEGOVIA3","76840574"` (conectar WiFi)
   - `AT+CIPSTART="TCP","servidor",80` (conectar servidor)
   - `AT+CIPSEND=tamaño` (enviar datos)
4. **ESP8266** transmite datos al servidor
5. **Arduino UNO** recibe confirmación
6. Repetir cada 30 segundos

---

## 🔍 Verificación:

### Monitor Serial (9600 baud):
```
🚀 pH METRO ARDUINO UNO + ESP8266 MÓDULO
📡 ESP8266: Módulo WiFi (AT Commands)
🌐 WiFi: SEGOVIA3

🔄 Inicializando módulo ESP8266...
✅ ESP8266 responde correctamente
🌐 Conectando a WiFi: SEGOVIA3
✅ WiFi conectado exitosamente

📊 pH leído: 7.234 | Buffer: 1/10
📊 pH leído: 7.189 | Buffer: 2/10
...
✅ Buffer de lecturas completado - Listo para transmitir

📡 TRANSMISIÓN #1
📊 pH promedio: 7.215
🧪 Clasificación: NEUTRO/ÓPTIMO
📤 JSON a enviar: {"ph":7.215,"device":"Arduino_UNO"...}
✅ TRANSMISIÓN EXITOSA
```

---

## 🎯 Ventajas de esta Configuración:

### ✅ Arduino UNO:
- **Control total** del sistema
- **Lectura precisa** del sensor pH
- **Procesamiento robusto** de datos
- **Promedio de múltiples lecturas**

### ✅ ESP8266 como módulo:
- **Sin programación** necesaria
- **Firmware estable** de fábrica  
- **Comandos AT estándar**
- **Confiabilidad máxima**

### ✅ Sistema completo:
- **Separación de responsabilidades** clara
- **Fácil debugging** por separado
- **Mantenimiento simple**
- **Escalabilidad** futura

---

## 🛠️ Troubleshooting:

### ❌ ESP8266 no responde:
- Verificar conexiones RX/TX
- Verificar voltaje 3.3V
- Probar comandos AT manualmente

### ❌ WiFi no conecta:
- Verificar SSID y password
- Verificar rango de señal
- Resetear ESP8266: `AT+RST`

### ❌ Datos no llegan al servidor:
- Verificar conexión internet
- Verificar URL del servidor
- Revisar formato JSON

---

¡Tu configuración es la más **robusta y confiable**! 🚀
