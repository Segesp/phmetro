# 🔧 Sistema de Sincronización Automática ThingSpeak → Supabase

Este directorio contiene los scripts para mantener sincronizados automáticamente los datos de pH entre ThingSpeak y Supabase.

## 📋 Archivos del Sistema

### Scripts Principales
- `sync-manual.js` - Sincronización manual (una vez)
- `sync-intelligent.sh` - Sincronización automática inteligente
- `sync-control.sh` - Control del sistema de sincronización
- `auto-sync.sh` - Sincronización básica (legacy)

### Logs
- `sync-auto.log` - Log de la sincronización automática
- `sync.log` - Log de sincronizaciones anteriores

## 🚀 Uso del Sistema

### 1. Sincronización Manual (una vez)
```bash
node sync-manual.js
```

### 2. Control de Sincronización Automática

#### Iniciar sincronización automática:
```bash
./sync-control.sh start
```

#### Detener sincronización:
```bash
./sync-control.sh stop
```

#### Ver estado:
```bash
./sync-control.sh status
```

#### Ver logs:
```bash
./sync-control.sh logs
```

#### Reiniciar:
```bash
./sync-control.sh restart
```

## 📊 Monitoreo

### Ver logs en tiempo real:
```bash
tail -f sync-auto.log
```

### Verificar datos en Supabase:
```bash
curl -s -X GET \
  "https://japrhhtnijidlecqrkpk.supabase.co/rest/v1/ph_readings?device=eq.ThingSpeak&order=timestamp.desc&limit=5" \
  -H "apikey: YOUR_API_KEY" | jq '.'
```

## ⚙️ Configuración

### Intervalos de Sincronización
- **Automática**: Cada 30 segundos
- **Arduino → ThingSpeak**: Cada 15 segundos
- **Manejo de errores**: Pausa de 5 minutos tras 3 errores consecutivos

### Variables de Entorno
Las credenciales están hardcodeadas en `sync-manual.js`:
- `SUPABASE_URL`
- `SUPABASE_ANON_KEY` 
- `THINGSPEAK_CHANNEL_ID`
- `THINGSPEAK_READ_API_KEY`

## 🔄 Flujo de Datos

```
Arduino (ESP8266) → ThingSpeak (HTTP) → Sync Script → Supabase
     [15s]              [30s]              [instant]
```

## 📈 Estado Actual

- ✅ Arduino envía datos a ThingSpeak cada 15 segundos
- ✅ Sincronización automática funcionando cada 30 segundos  
- ✅ Datos aparecen en Supabase como device "ThingSpeak"
- ✅ Sistema de logs y monitoreo activo
- ✅ Control inteligente de errores implementado

## 🐛 Troubleshooting

### Si la sincronización no funciona:
1. Verificar que Node.js y dependencias estén instalados
2. Comprobar conectividad a ThingSpeak y Supabase
3. Revisar logs para errores específicos
4. Reiniciar el sistema de sincronización

### Si hay muchos errores:
El sistema automáticamente pausa 5 minutos tras 3 errores consecutivos para evitar spam de peticiones.
