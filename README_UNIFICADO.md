# 🌊 pH Metro Dashboard - Sistema IoT Completo

![pH Meter](https://img.shields.io/badge/pH%20Meter-IoT%20System-blue)
![Next.js](https://img.shields.io/badge/Next.js-14-black)
![Supabase](https://img.shields.io/badge/Supabase-Database-green)
![ThingSpeak](https://img.shields.io/badge/ThingSpeak-IoT%20Cloud-orange)
![ESP8266](https://img.shields.io/badge/ESP8266-WiFi%20Module-red)

## 🎯 **Sistema de Monitoreo de pH en Tiempo Real**

Sistema completo de monitoreo IoT con **transmisión dual redundante** desde Arduino UNO + ESP8266 hacia dashboard web profesional y plataforma ThingSpeak.

### 🌐 **URLs Activas**
- **Dashboard Principal**: [phmetro-phi.vercel.app](https://phmetro-phi.vercel.app)
- **ThingSpeak Channel**: [Canal 2988488](https://thingspeak.com/channels/2988488)
- **Cloudflare Worker**: esp8266-phmetro-proxy.20200205.workers.dev

---

## 🏗️ **Arquitectura del Sistema**

```
                    SISTEMA DUAL REDUNDANTE
┌─────────────────┐
│   ESP8266       │ ──HTTPS (443)──► Cloudflare Worker ──► Vercel ──► Supabase
│   Arduino UNO   │                                                     │
│   + Sensor pH   │ ──HTTP (80)───► ThingSpeak API                      │
└─────────────────┘                                                     ▼
                                                              ┌─────────────────┐
                                                              │   Dashboard     │
                                                              │  (Ambas fuentes)│
                                                              └─────────────────┘
```

## ✨ **Características del Sistema**

### 📊 **Dashboard Inteligente**
- **Gráficos en tiempo real** con Chart.js responsive
- **Selector de fuente** (Supabase/ThingSpeak/Ambas)
- **Estadísticas duales** con contadores por origen
- **Auto-refresh** cada 30 segundos
- **PWA Support** para móviles

### 🔄 **Transmisión Dual Redundante**
- **Canal Principal**: ESP8266 → HTTPS → Cloudflare → Vercel → Supabase
- **Canal Backup**: ESP8266 → HTTP → ThingSpeak API
- **Reconexión automática** en caso de fallo
- **Logs detallados** por destino

### 🔒 **Seguridad y Conectividad**
- **HTTPS SSL nativo** en ESP8266
- **Cloudflare Worker** como proxy seguro
- **API Keys protegidas** en variables de entorno
- **Timeouts optimizados** para conexiones SSL

---

## 🚀 **Inicio Rápido**

### **1. Hardware - Conexiones Arduino UNO + ESP8266**
```
Arduino UNO    ESP8266
-----------    --------
3.3V     →     VCC
GND      →     GND
Pin 2    →     TX
Pin 3    →     RX
Pin 8    →     CH_PD (Enable)
Pin 9    →     RST (Reset)
```

### **2. Software - Configurar y Subir Código**
```cpp
// En Codigo_Arduino_UNO_ESP8266_OPTIMIZADO.ino
const char* ssid = "TU_WIFI";
const char* password = "TU_PASSWORD";
```

### **3. Dashboard - Instalar y Ejecutar**
```bash
git clone https://github.com/Segesp/phmetro.git
cd phmetro
npm install
cp .env.local.example .env.local
# Configurar variables de entorno
npm run dev
```

### **4. Verificar Funcionamiento**
- **Monitor Serie**: 9600 baudios para logs
- **Dashboard**: http://localhost:3000
- **ThingSpeak**: Verificar datos en canal

---

## 📁 **Archivos Principales**

| Archivo | Descripción | Estado |
|---------|-------------|--------|
| `Codigo_Arduino_UNO_ESP8266_OPTIMIZADO.ino` ⭐ | Código principal dual | ✅ LISTO |
| `Codigo_Arduino_UNO_ESP8266_ULTRA_SIMPLE.ino` | Código backup simple | ✅ LISTO |
| `app/page.tsx` | Dashboard con selector dual | ✅ OPERATIVO |
| `cloudflare-worker.js` | Proxy HTTPS SSL | ✅ DESPLEGADO |
| `test_dual_envio.sh` | Script pruebas sistema | ✅ FUNCIONAL |
| `diagnostico_https.sh` | Diagnóstico conectividad | ✅ FUNCIONAL |

---

## 🔧 **Configuración del Sistema**

### **APIs Configuradas**
```bash
# ThingSpeak
Write API Key: I4RFD6P62MTKOR8E
Read API Key:  Z6SC5MLLP0FR4PC4
Channel ID:    2988488

# Cloudflare Worker
URL: esp8266-phmetro-proxy.20200205.workers.dev

# Dashboard
Vercel: phmetro-phi.vercel.app
Database: Supabase (tiempo real)
```

### **Configuración HTTPS SSL**
El ESP8266 se conecta usando **SSL nativo** (puerto 443) resolviendo problemas de compatibilidad:

```cpp
// ANTES (HTTP - No funcionaba)
AT+CIPSTART="TCP","server.com",80

// DESPUÉS (HTTPS - Funciona)
AT+CIPSTART="SSL","server.com",443
```

### **Cloudflare Worker Setup**
Para implementar tu propio Worker:

1. **Crear cuenta** en [cloudflare.com](https://cloudflare.com)
2. **Workers & Pages** → Create Application → Create Worker
3. **Copiar código** de `cloudflare-worker.js`
4. **Deploy** y obtener URL
5. **Actualizar** URL en código Arduino

---

## 🧪 **Testing y Diagnóstico**

### **Probar Sistema Dual**
```bash
# Enviar valor de prueba a ambos destinos
./test_dual_envio.sh 7.35

# Verificar respuestas
curl -X POST https://esp8266-phmetro-proxy.20200205.workers.dev/api/ph-proxy \
  -H "Content-Type: application/json" \
  -d '{"ph": 7.0, "device": "TEST"}'

curl "https://api.thingspeak.com/update?api_key=I4RFD6P62MTKOR8E&field1=7.0"
```

### **Diagnóstico HTTPS**
```bash
# Verificar conectividad SSL
./diagnostico_https.sh

# Verificar certificados
openssl s_client -connect esp8266-phmetro-proxy.20200205.workers.dev:443
```

### **Monitor Arduino**
En el Monitor Serie verás:
```
=== INICIO SISTEMA DUAL ===
Conectando WiFi...
✅ WiFi conectado: 192.168.1.100
📡 Enviando datos de prueba...
✅ Dashboard: 200 OK
✅ ThingSpeak: 200 OK
=== OPERACIÓN NORMAL ===
```

---

## 🛠️ **Solución de Problemas**

### **ESP8266 No Conecta**
1. **Verificar cableado** según diagrama
2. **Baudrate** correcto (9600)
3. **WiFi credentials** válidas
4. **Firmware ESP8266** v1.6.0+

### **Dashboard No Recibe Datos**
1. **Verificar Cloudflare Worker** activo
2. **Variables Supabase** configuradas
3. **CORS** habilitado en Worker
4. **SSL handshake** exitoso

### **ThingSpeak Falla**
1. **API Key** correcta
2. **Channel ID** válido
3. **Rate limit** (15 segundos entre envíos)
4. **HTTP connection** disponible

---

## 📊 **Estructura del Proyecto**

```
phmetro/
├── 🔧 Arduino/
│   ├── Codigo_Arduino_UNO_ESP8266_OPTIMIZADO.ino (Principal)
│   └── Codigo_Arduino_UNO_ESP8266_ULTRA_SIMPLE.ino (Backup)
├── 📱 Dashboard/
│   ├── app/ (Next.js pages)
│   ├── components/ (Componentes React)
│   └── lib/ (Conexiones BD)
├── ☁️ Infrastructure/
│   ├── cloudflare-worker.js (Proxy SSL)
│   ├── supabase-init.sql (DB setup)
│   └── vercel.json (Deploy config)
└── 🧪 Testing/
    ├── test_dual_envio.sh
    └── diagnostico_https.sh
```

---

## 🎯 **Estado del Proyecto: ✅ COMPLETO**

- **✅ Arduino**: Código optimizado con transmisión dual
- **✅ Dashboard**: Operativo con selector de fuentes
- **✅ HTTPS**: SSL nativo funcionando en ESP8266
- **✅ ThingSpeak**: Backup redundante configurado
- **✅ Testing**: Scripts de prueba y diagnóstico
- **✅ Deploy**: Automático via GitHub → Vercel

---

## 📚 **Documentación Técnica Completa**

### **Códigos Arduino Disponibles**
- **OPTIMIZADO**: Sistema dual completo con HTTPS SSL, gestión de errores y diagnósticos avanzados
- **ULTRA SIMPLE**: Código mínimo para debugging, conexión directa HTTPS a Vercel

### **Actualización HTTPS SSL**
El problema original era que ESP8266 no podía conectar por HTTP (puerto 80) a Cloudflare. La solución fue cambiar a HTTPS SSL (puerto 443) usando comandos AT nativos.

### **Sistema Dual Dashboard + ThingSpeak**
Implementación de envío simultáneo a dos destinos para redundancia y análisis. El dashboard puede leer de ambas fuentes con un selector intuitivo.

### **Configuración Cloudflare Worker**
Proxy intermedio que recibe datos HTTPS del ESP8266 y los reenvía a Vercel, resolviendo problemas de compatibilidad SSL.

---

*Desarrollado con ❤️ para monitoreo IoT profesional*

**¡Sistema listo para uso en producción!** 🚀
