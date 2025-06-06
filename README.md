# Dashboard pH Metro

Sistema de monitoreo de pH en tiempo real con Arduino ESP8266, Next.js y Supabase.

## 🚀 Características

- 📊 **Dashboard en tiempo real** con gráficos interactivos
- 🔄 **Actualizaciones automáticas** mediante WebSockets
- 📱 **Diseño responsivo** compatible con móviles y escritorio
- 📈 **Estadísticas detalladas** (promedio, tendencias, rangos)
- 🎯 **Alertas visuales** para niveles críticos de pH
- 🗄️ **Base de datos Supabase** para almacenamiento confiable
- ☁️ **Desplegado en Vercel** para acceso global

## 🛠️ Tecnologías

### Frontend
- **Next.js 14** (App Router)
- **React 18** con TypeScript
- **Tailwind CSS** para estilos
- **Recharts** para gráficos
- **Lucide React** para iconos

### Backend
- **Supabase** (PostgreSQL)
- **API Routes** de Next.js
- **Real-time subscriptions**

### Hardware
- **ESP8266** (NodeMCU)
- **Sensor de pH** analógico
- **WiFi** para conectividad

## 📋 Requisitos

- Node.js 18+
- Cuenta de Supabase
- Cuenta de Vercel
- Arduino ESP8266

## 🔧 Configuración

### 1. Clonar el repositorio

```bash
git clone https://github.com/tu-usuario/phmetro-dashboard.git
cd phmetro-dashboard
```

### 2. Instalar dependencias

```bash
npm install
```

### 3. Configurar Supabase

1. Crear una cuenta en [Supabase](https://supabase.com)
2. Crear un nuevo proyecto
3. Ejecutar el siguiente SQL en el SQL Editor:

```sql
-- Crear tabla para lecturas de pH
CREATE TABLE ph_readings (
  id UUID DEFAULT gen_random_uuid() PRIMARY KEY,
  ph DECIMAL(4,2) NOT NULL CHECK (ph >= 0 AND ph <= 14),
  timestamp TIMESTAMPTZ,
  created_at TIMESTAMPTZ DEFAULT NOW()
);

-- Crear índice para mejorar consultas
CREATE INDEX idx_ph_readings_created_at ON ph_readings(created_at DESC);

-- Habilitar Row Level Security
ALTER TABLE ph_readings ENABLE ROW LEVEL SECURITY;

-- Crear política para permitir inserción
CREATE POLICY "Allow insert ph_readings" ON ph_readings
  FOR INSERT WITH CHECK (true);

-- Crear política para permitir lectura
CREATE POLICY "Allow select ph_readings" ON ph_readings
  FOR SELECT USING (true);

-- Habilitar realtime
ALTER PUBLICATION supabase_realtime ADD TABLE ph_readings;
```

### 4. Variables de entorno

Crear `.env.local` basado en `.env.local.example`:

```env
NEXT_PUBLIC_SUPABASE_URL=tu_supabase_url
NEXT_PUBLIC_SUPABASE_ANON_KEY=tu_supabase_anon_key
SUPABASE_SERVICE_ROLE_KEY=tu_supabase_service_role_key
```

### 5. Ejecutar en desarrollo

```bash
npm run dev
```

Abrir [http://localhost:3000](http://localhost:3000)

## 📱 Configuración del Arduino

1. Instalar librerías necesarias:
   - ESP8266WiFi
   - ESP8266HTTPClient

2. Configurar el código Arduino:
   - Actualizar credenciales WiFi
   - Verificar la URL de la API
   - Calibrar el sensor de pH

3. Cargar el programa al ESP8266

## 🚀 Despliegue en Vercel

### 1. Conectar repositorio

1. Ir a [Vercel](https://vercel.com)
2. Importar el repositorio de GitHub
3. Configurar las variables de entorno

### 2. Variables de entorno en Vercel

Agregar las mismas variables del archivo `.env.local`:
- `NEXT_PUBLIC_SUPABASE_URL`
- `NEXT_PUBLIC_SUPABASE_ANON_KEY`
- `SUPABASE_SERVICE_ROLE_KEY`

### 3. Desplegar

Vercel desplegará automáticamente en cada push a la rama principal.

## 📊 API Endpoints

### POST /api/ph-data
Recibe datos del Arduino:
```json
{
  "ph": 7.2,
  "timestamp": "2024-01-01T12:00:00Z"
}
```

### GET /api/ph-data
Obtiene las últimas 100 lecturas:
```json
{
  "success": true,
  "data": [...]
}
```

## 🔧 Funcionalidades del Dashboard

- **Gráfico en tiempo real**: Visualización de tendencias de pH
- **Tarjetas de estadísticas**: pH actual, promedio, rango y tendencia
- **Lecturas recientes**: Lista de las últimas mediciones
- **Indicadores de estado**: Códigos de color para niveles óptimos
- **Actualizaciones automáticas**: Sin necesidad de recargar la página

## 🎯 Rangos de pH

- **Óptimo**: 6.5 - 8.5 (Verde)
- **Aceptable**: 6.0 - 9.0 (Amarillo)
- **Crítico**: < 6.0 o > 9.0 (Rojo)

## 📝 Personalización

### Modificar rangos de pH
Editar los valores en `app/page.tsx` y `components/StatsCards.tsx`

### Cambiar frecuencia de lecturas
Modificar el `delay()` en el código Arduino

### Personalizar estilos
Editar `tailwind.config.js` y `app/globals.css`

## 🤝 Contribuir

1. Fork el proyecto
2. Crear rama feature (`git checkout -b feature/AmazingFeature`)
3. Commit cambios (`git commit -m 'Add AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abrir Pull Request

## 📄 Licencia

Este proyecto está bajo la Licencia MIT.

## 🆘 Soporte

Para reportar bugs o solicitar funcionalidades, crear un [issue](https://github.com/tu-usuario/phmetro-dashboard/issues).

---

**Desarrollado con ❤️ para monitoreo de pH en tiempo real**