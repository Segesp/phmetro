# 🔧 Configuración del Webhook ThingSpeak → Supabase

## Problema Identificado

La sincronización automática desde el frontend tiene limitaciones:
- ⚠️ Requiere que el dashboard esté abierto para funcionar
- ⚠️ Depende de que los usuarios mantengan la página activa
- ⚠️ Puede tener retrasos de hasta 2 minutos

## Solución: Webhook Directo

Para garantizar que **TODOS** los datos se guarden en tiempo real, configura ThingSpeak para enviar los datos directamente a Supabase mediante webhook.

### 📡 URL del Webhook
```
https://phmetro-phi.vercel.app/api/thingspeak-webhook
```

### 🔧 Configuración en ThingSpeak

1. **Accede a tu canal de ThingSpeak**: https://thingspeak.com/channels/2988488
2. **Ve a la pestaña "Apps"**
3. **Selecciona "ThingHTTP"**
4. **Crea una nueva ThingHTTP** con estos parámetros:

```
Name: Supabase Webhook
URL: https://phmetro-phi.vercel.app/api/thingspeak-webhook
HTTP Method: POST
Content Type: application/json
Headers: (vacío)
Body:
{
  "field1": "%%channel_2988488_field_1%%",
  "created_at": "%%datetime%%"
}
```

5. **Guarda la configuración**

### 🎯 Configurar React (Trigger)

1. **Ve a la pestaña "Apps" nuevamente**
2. **Selecciona "React"**
3. **Crea un nuevo React** con estos parámetros:

```
React Name: Auto Webhook
Condition Type: On Data Insertion
Action: ThingHTTP
ThingHTTP: Supabase Webhook (el que acabas de crear)
```

4. **Guarda la configuración**

### ✅ Verificación

Una vez configurado:
- ✅ Cada dato nuevo del Arduino se enviará automáticamente a Supabase
- ✅ No dependerá del dashboard estar abierto
- ✅ Sincronización en tiempo real (< 1 segundo)
- ✅ No habrá pérdida de datos

### 🧪 Prueba Manual

Puedes probar el webhook manualmente:

```bash
curl -X POST https://phmetro-phi.vercel.app/api/thingspeak-webhook \
  -H "Content-Type: application/json" \
  -d '{"field1": "7.5", "created_at": "2025-06-16T21:30:00Z"}'
```

### 📊 Estado Actual del Sistema

**Método Actual**: Sincronización desde frontend cada 2 minutos
- ✅ Funciona cuando el dashboard está abierto
- ⚠️ Puede tener retrasos
- ⚠️ Requiere intervención manual

**Método Recomendado**: Webhook directo
- ✅ Funciona 24/7 automáticamente
- ✅ Tiempo real
- ✅ Sin pérdida de datos
- ✅ Sin intervención manual

## 🚀 Siguiente Paso

**Configura el webhook en ThingSpeak para garantizar que todos los datos se guarden en tiempo real.**
