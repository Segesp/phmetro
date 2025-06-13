# 🔄 SISTEMA DUAL: DASHBOARD + THINGSPEAK

## 📊 **CONFIGURACIÓN COMPLETADA**

El sistema ahora envía datos de pH a **DOS DESTINOS** simultáneamente:

### 1. 🌟 **DASHBOARD PRINCIPAL** (Vercel + Supabase)
```
ESP8266 → HTTPS:443 → Cloudflare Worker → Vercel → Supabase → Dashboard
URL: https://phmetro-phi.vercel.app
```

### 2. 📡 **THINGSPEAK** (Backup/Análisis)
```
ESP8266 → HTTP:80 → ThingSpeak API → Canal 2988488
URL: https://thingspeak.com/channels/2988488
```

---

## ⚙️ **CONFIGURACIÓN ARDUINO**

### Variables añadidas:
```cpp
// ThingSpeak
const char* thingSpeakHost = "api.thingspeak.com";
const char* writeAPIKey = "I4RFD6P62MTKOR8E";

// Cloudflare Worker (existente)
const char* cloudflareWorker = "esp8266-phmetro-proxy.20200205.workers.dev";
```

### Funciones nuevas:
- `postToThingSpeak(float ph)` - Envía datos a ThingSpeak
- `sendInitialTestData()` - Actualizada para envío dual
- Loop principal modificado para envío dual

---

## 📈 **DASHBOARD ACTUALIZADO**

### Nuevas características:
- ✅ **Selector de fuente de datos**
  - 📊 Ambas fuentes
  - 🗄️ Solo Supabase
  - 📡 Solo ThingSpeak

- ✅ **Estadísticas mejoradas**
  - Contador por fuente
  - Identificación del dispositivo
  - Información de origen

### API ThingSpeak integrada:
```javascript
// Endpoint para leer datos
https://api.thingspeak.com/channels/2988488/fields/1.json?api_key=Z6SC5MLLP0FR4PC4&results=50
```

---

## 🚀 **FLUJO DE DATOS COMPLETO**

### **Envío desde Arduino:**
1. **Lectura sensor** cada 3 segundos
2. **Promedio** de 3 lecturas
3. **Envío dual** cada 15 segundos:
   - Dashboard via HTTPS SSL
   - ThingSpeak via HTTP

### **Visualización en Dashboard:**
1. **Datos Supabase** (tiempo real via WebSocket)
2. **Datos ThingSpeak** (polling cada 30s)
3. **Combinación** de ambas fuentes
4. **Gráficos unificados** con Chart.js

---

## 📋 **APIS UTILIZADAS**

### **ThingSpeak:**
```
Write API Key: I4RFD6P62MTKOR8E
Read API Key:  Z6SC5MLLP0FR4PC4
Channel ID:    2988488
```

### **Vercel/Supabase:**
```
Endpoint: https://phmetro-phi.vercel.app/api/ph-proxy
Database: Supabase (tiempo real)
```

---

## 🧪 **MODO DE PRUEBA**

### **Datos de prueba automáticos:**
```cpp
float testValues[] = {6.8, 7.0, 7.2, 7.4, 7.1};
```

### **Secuencia de envío:**
1. pH 6.8 → Dashboard + ThingSpeak
2. Pausa 5 segundos
3. pH 7.0 → Dashboard + ThingSpeak
4. ... continúa con todos los valores

---

## ✅ **VENTAJAS DEL SISTEMA DUAL**

### **🔄 Redundancia:**
- Si falla un destino, el otro sigue funcionando
- Datos asegurados en múltiples plataformas

### **📊 Análisis:**
- Dashboard principal para visualización en vivo
- ThingSpeak para análisis histórico y tendencias

### **🌐 Accesibilidad:**
- Dashboard personalizado con diseño propio
- ThingSpeak como backup público

---

## 🔧 **PRÓXIMOS PASOS**

1. **Compilar y cargar** código Arduino actualizado
2. **Verificar** envío dual en Monitor Serie
3. **Comprobar** datos en ambos dashboards:
   - https://phmetro-phi.vercel.app
   - https://thingspeak.com/channels/2988488
4. **Probar** selector de fuentes en dashboard

---

*Sistema Dual implementado - Junio 2025*
