# 🚀 Guía Paso a Paso - Despliegue en Vercel

## ✅ Estado Actual
- ✅ Proyecto completamente funcional
- ✅ Build exitoso (verificado)
- ✅ Supabase configurado y conectado
- ✅ API funcionando correctamente
- ✅ Repositorio actualizado en GitHub

## 🔗 Enlaces Importantes
- **Repositorio GitHub**: https://github.com/Segesp/phmetro
- **Supabase URL**: https://japrhhtnijidlecqrkpk.supabase.co
- **Dashboard Local**: http://localhost:3000

---

## 📋 PASO 1: Acceder a Vercel

1. Ve a **https://vercel.com**
2. Haz clic en **"Sign up"** o **"Log in"**
3. Selecciona **"Continue with GitHub"**
4. Autoriza a Vercel para acceder a tu cuenta de GitHub

## 📋 PASO 2: Importar Proyecto

1. En el dashboard de Vercel, haz clic en **"Add New"**
2. Selecciona **"Project"**
3. Busca el repositorio **"Segesp/phmetro"**
4. Haz clic en **"Import"**

## 📋 PASO 3: Configurar Proyecto

### Framework Detection
- Vercel detectará automáticamente **Next.js**
- Mantén la configuración por defecto:
  - **Framework Preset**: Next.js
  - **Root Directory**: ./
  - **Build Command**: `npm run build`
  - **Output Directory**: .next
  - **Install Command**: `npm install`

### Variables de Entorno
En la sección **"Environment Variables"**, agrega:

```
NEXT_PUBLIC_SUPABASE_URL=https://japrhhtnijidlecqrkpk.supabase.co
```

```
NEXT_PUBLIC_SUPABASE_ANON_KEY=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MzY3MTA4MjEsImV4cCI6MjA1MjI4NjgyMX0.ZyQfWOhJjE15Pt_d1n7NpKtC8QTcxjZdyKG_PQR_s6E
```

```
SUPABASE_SERVICE_ROLE_KEY=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6InNlcnZpY2Vfc29sZSIsImlhdCI6MTczNjcxMDgyMSwiZXhwIjoyMDUyMjg2ODIxfQ.5BH7f_6ooAjPcLyGIWTIWvXuST85-1U4rHqPhUFDEH8
```

## 📋 PASO 4: Desplegar

1. Haz clic en **"Deploy"**
2. Espera a que termine el build (2-3 minutos)
3. Una vez completado, verás el mensaje **"Your project has been deployed"**
4. Haz clic en **"Visit"** para ver tu dashboard en vivo

---

## 🎯 Después del Despliegue

### 1. Obtener URL de Producción
Tu dashboard estará disponible en una URL como:
```
https://phmetro-[random-id].vercel.app
```

### 2. Actualizar Código Arduino
Reemplaza la URL en tu código Arduino:
```cpp
// Cambiar esta línea:
String server = "http://localhost:3000";

// Por tu URL de Vercel:
String server = "https://tu-proyecto.vercel.app";
```

### 3. Verificar Funcionamiento
- ✅ Dashboard carga correctamente
- ✅ Gráficos se muestran
- ✅ API responde en `/api/ph-data`
- ✅ Datos se guardan en Supabase

---

## 🔧 Pruebas Post-Despliegue

### Probar API con curl:
```bash
# Enviar datos de prueba
curl -X POST https://tu-proyecto.vercel.app/api/ph-data \
  -H "Content-Type: application/json" \
  -d '{"ph": 7.2, "temperature": 25.5}'

# Obtener datos
curl https://tu-proyecto.vercel.app/api/ph-data
```

### Verificar en Dashboard:
1. Ve a tu URL de Vercel
2. Deberías ver datos simulados o reales
3. Los gráficos deben funcionar
4. Las estadísticas deben calcularse

---

## 🐛 Solución de Problemas

### Build Failed
- Verifica que las variables de entorno estén correctas
- Revisa los logs de build en Vercel

### API No Funciona
- Verifica las credenciales de Supabase
- Asegúrate de que la tabla `ph_readings` exista

### Dashboard No Carga
- Verifica que la URL sea correcta
- Revisa la consola del navegador para errores

---

## 📱 Bonus: Configuración de Dominio Personalizado

Si tienes un dominio propio:
1. Ve a **Project Settings > Domains**
2. Agrega tu dominio personalizado
3. Configura los DNS según las instrucciones

---

## ✅ Lista de Verificación Final

- [ ] Proyecto desplegado en Vercel
- [ ] Dashboard funciona correctamente
- [ ] API endpoints responden
- [ ] Variables de entorno configuradas
- [ ] Arduino actualizado con nueva URL
- [ ] Datos de prueba enviados exitosamente
- [ ] Gráficos y estadísticas funcionando

¡Tu pH Metro Dashboard está ahora en producción! 🎉
