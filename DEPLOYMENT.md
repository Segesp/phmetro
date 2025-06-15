# Guía de Despliegue - Dashboard pH Metro

## 📋 Requisitos Previos

- [ ] Cuenta de GitHub
- [ ] Cuenta de Supabase
- [ ] Cuenta de Vercel
- [ ] Arduino ESP8266 configurado

## 🗄️ Configuración de Supabase

### 1. Crear Proyecto
1. Ir a [supabase.com](https://supabase.com)
2. Crear nuevo proyecto
3. Elegir región más cercana

### 2. Configurar Base de Datos
1. Ir a SQL Editor
2. Ejecutar el contenido de `supabase-init.sql`
3. Verificar que la tabla `ph_readings` se creó correctamente

### 3. Obtener Credenciales
```
Project URL: https://tu-proyecto.supabase.co
Anon Key: eyJ... (clave pública)
Service Role Key: eyJ... (clave privada)
```

## 🚀 Despliegue en Vercel

### 1. Conectar Repositorio
1. Ir a [vercel.com](https://vercel.com)
2. Hacer login con GitHub
3. Importar repositorio
4. Seleccionar framework: Next.js

### 2. Configurar Variables de Entorno
En la configuración del proyecto en Vercel, agregar:

```
NEXT_PUBLIC_SUPABASE_URL=https://tu-proyecto.supabase.co
NEXT_PUBLIC_SUPABASE_ANON_KEY=tu_anon_key
SUPABASE_SERVICE_ROLE_KEY=tu_service_role_key
```

### 3. Desplegar
- Vercel desplegará automáticamente
- URL final: `https://tu-proyecto.vercel.app`

## 🔧 Configuración del Arduino

### 1. Actualizar Código Arduino
En `Codigo_arduino.ino`, cambiar:

```cpp
const char* serverUrl = "https://tu-proyecto.vercel.app/api/ph-data";
```

### 2. Configurar WiFi
```cpp
const char* ssid = "TU_WIFI";
const char* password = "TU_PASSWORD";
```

### 3. Cargar al ESP8266
1. Conectar ESP8266 por USB
2. Seleccionar puerto correcto
3. Subir código

## ✅ Verificación

### 1. Probar API
```bash
curl -X POST https://tu-proyecto.vercel.app/api/ph-data \
  -H "Content-Type: application/json" \
  -d '{"ph": 7.2, "timestamp": "2024-01-01T12:00:00Z"}'
```

### 2. Verificar Dashboard
- Abrir `https://tu-proyecto.vercel.app`
- Debe mostrar gráficos y datos en tiempo real

### 3. Probar Arduino
- Monitor serie debe mostrar: "Respuesta HTTP: 200"
- Dashboard debe actualizar automáticamente

## 🔧 Solución de Problemas

### Error 500 en API
- Verificar variables de entorno en Vercel
- Verificar que la tabla existe en Supabase
- Revisar logs en Vercel

### Arduino no se conecta
- Verificar credenciales WiFi
- Verificar URL de la API
- Verificar conectividad a internet

### Dashboard no actualiza
- Verificar que Realtime está habilitado en Supabase
- Verificar políticas RLS en la tabla
- Verificar conexión a internet

## 📊 Monitoreo

### Logs de Vercel
- Ir a tu proyecto > Functions > Ver logs
- Monitorear errores y respuestas

### Logs de Supabase
- Ir a tu proyecto > Logs > API
- Verificar consultas y errores

### Monitor Serie Arduino
- Abrir IDE Arduino > Monitor Serie
- Verificar conexión WiFi y respuestas HTTP

## 🔄 Actualizaciones

### Actualizar Código
1. Commit cambios a GitHub
2. Vercel desplegará automáticamente
3. Verificar que funciona correctamente

### Actualizar Arduino
1. Modificar código Arduino
2. Cargar al ESP8266
3. Verificar en monitor serie

## 📞 Soporte

Si tienes problemas:
1. Revisar logs de Vercel y Supabase
2. Verificar configuración de variables
3. Probar API manualmente
4. Verificar conexión Arduino

---

**¡Tu sistema de monitoreo pH está listo! 🎉**
