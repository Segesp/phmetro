# 🌊 pH Metro Dashboard - Sistema Dual IoT

![pH Meter](https://img.shields.io/badge/pH%20Meter-IoT%20System-blue)
![Next.js](https://img.shields.io/badge/Next.js-14-black)
![Supabase](https://img.shields.io/badge/Supabase-Database-green)
![ThingSpeak](https://img.shields.io/badge/ThingSpeak-IoT%20Cloud-orange)
![ESP8266](https://img.shields.io/badge/ESP8266-WiFi%20Module-red)

## 🎯 **Sistema de Monitoreo de pH en Tiempo Real**

Dashboard web profesional para monitoreo de niveles de pH con **sistema dual de almacenamiento** y **transmisión redundante** desde Arduino UNO + ESP8266.

### 🌐 **Demo Live**
- **Dashboard Principal**: [phmetro-phi.vercel.app](https://phmetro-phi.vercel.app)
- **ThingSpeak Channel**: [Canal 2988488](https://thingspeak.com/channels/2988488)
---

## �️ **Arquitectura del Sistema**

```
┌─────────────────┐    HTTPS SSL     ┌─────────────────┐    HTTPS    ┌─────────────────┐
│   ESP8266       │ ──────────────► │ Cloudflare      │ ──────────► │   Dashboard     │
│   Arduino UNO   │     Port 443     │ Worker (Proxy)  │             │   (Vercel)      │
└─────────────────┘                  └─────────────────┘             └─────────────────┘
       │                                                                       │
       │                                                                       ▼
       │             HTTP                                               ┌─────────────────┐
       └─────────────────────────────────────────────────────────────► │   Supabase      │
                    Port 80                                             │   Database      │
                      │                                                 └─────────────────┘
                      ▼                                                          ▲
               ┌─────────────────┐                                              │
               │   ThingSpeak    │ ─────────────────────────────────────────────┘
               │   IoT Cloud     │           Dashboard lee ambas fuentes
               └─────────────────┘
```

## ✨ **Características Principales**

### 📊 **Dashboard Inteligente**
- **Gráficos en tiempo real** con Chart.js
- **Selector de fuente de datos** (Supabase/ThingSpeak/Ambas)
- **Estadísticas avanzadas** por fuente
- **Responsive design** con Tailwind CSS
- **Actualizaciones automáticas** cada 30 segundos

### 🔄 **Sistema Dual de Transmisión**
- **Primario**: Dashboard via HTTPS SSL → Vercel → Supabase
- **Backup**: ThingSpeak via HTTP → IoT Cloud
- **Redundancia**: Si falla uno, el otro continúa
- **Análisis**: Dashboard personalizado + herramientas IoT

### 🌐 **ESP8266 WiFi Robusto**
- **Conexión HTTPS SSL** nativa
- **Manejo de errores** avanzado
- **Reconexión automática**
- **Logs detallados** por destino

---

## 🚀 **Inicio Rápido**

### **1. Clonar Repositorio**
```bash
git clone https://github.com/Segesp/phmetro.git
cd phmetro
```

### **2. Instalar Dependencias**
```bash
npm install
```

### **3. Configurar Variables de Entorno**
```bash
cp .env.local.example .env.local
# Editar .env.local con tus credenciales de Supabase
```

### **4. Ejecutar Dashboard**
```bash
npm run dev
# Abrir http://localhost:3000
```

### **5. Configurar Arduino**
```cpp
// En Codigo_Arduino_UNO_ESP8266_OPTIMIZADO.ino
const char* ssid = "TU_WIFI";
const char* password = "TU_PASSWORD";
```

---

## 📁 **Archivos Principales**

| Archivo | Descripción |
|---------|-------------|
| `Codigo_Arduino_UNO_ESP8266_OPTIMIZADO.ino` ⭐ | Código Arduino principal (sistema dual) |
| `Codigo_Arduino_UNO_ESP8266_ULTRA_SIMPLE.ino` 🔧 | Código Arduino simplificado (backup) |
| `app/page.tsx` | Dashboard principal con selector de fuentes |
| `cloudflare-worker.js` | Worker proxy HTTPS |
| `test_dual_envio.sh` | Script de pruebas dual |
| `diagnostico_https.sh` | Script diagnóstico conectividad |

---

## 🔧 **APIs Configuradas**

### **ThingSpeak**
```bash
Write API Key: I4RFD6P62MTKOR8E
Read API Key:  Z6SC5MLLP0FR4PC4
Channel ID:    2988488
URL: https://thingspeak.com/channels/2988488
```

### **Dashboard**
```bash
Cloudflare Worker: esp8266-phmetro-proxy.20200205.workers.dev
Vercel Dashboard: phmetro-phi.vercel.app
Database: Supabase (tiempo real)
```

---

## 🧪 **Testing**

### **Probar Sistema Dual**
```bash
./test_dual_envio.sh 7.35
```

### **Diagnóstico HTTPS**
```bash
./diagnostico_https.sh
```

### **API Manual**
```bash
# Dashboard
curl -X POST https://esp8266-phmetro-proxy.20200205.workers.dev/api/ph-proxy \
  -H "Content-Type: application/json" \
  -d '{"ph": 7.0, "device": "TEST"}'

# ThingSpeak
curl "https://api.thingspeak.com/update?api_key=I4RFD6P62MTKOR8E&field1=7.0"
```

---

## 📚 **Documentación**

- 📖 **[Sistema Dual](SISTEMA_DUAL_DASHBOARD_THINGSPEAK.md)** - Configuración completa
- 🔒 **[HTTPS Setup](ACTUALIZACION_HTTPS_SSL.md)** - Configuración SSL
- ☁️ **[Cloudflare Guide](CLOUDFLARE_SETUP_GUIDE.md)** - Worker setup
- 📱 **[Arduino Codes](CODIGOS_ARDUINO_HTTPS_FINAL.md)** - Resumen códigos

---

## 🎯 **Estado del Proyecto**

**✅ Dashboard: FUNCIONANDO**  
**✅ Arduino: LISTO**  
**✅ APIs: CONFIGURADAS**  
**✅ Sistema Dual: IMPLEMENTADO**

---

*Desarrollado con ❤️ para monitoreo IoT profesional*