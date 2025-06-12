# 🔧 SOLUCIÓN PASO A PASO: Error "Invalid API key" en Vercel

## 🎯 Problema Identificado
Las variables están configuradas en Vercel pero el error persiste. Esto indica:
1. Las variables pueden no haberse propagado después del último despliegue
2. Los valores pueden ser incorrectos
3. Falta forzar un nuevo despliegue

---

## ✅ SOLUCIÓN COMPLETA - PASO A PASO

### 🔑 Paso 1: Verificar/Actualizar Variables en Vercel

Ve a tu proyecto en Vercel y verifica que estas variables estén **EXACTAMENTE** así:

#### Variable 1: URL de Supabase
- **Name:** `NEXT_PUBLIC_SUPABASE_URL`
- **Value:** `https://japrhhtnijidlecqrkpk.supabase.co`
- **Environments:** ✅ Production ✅ Preview ✅ Development

#### Variable 2: Clave Anónima (ACTUALIZADA)
- **Name:** `NEXT_PUBLIC_SUPABASE_ANON_KEY`
- **Value:** `eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM`
- **Environments:** ✅ Production ✅ Preview ✅ Development

⚠️ **IMPORTANTE:** Copia la clave completa, sin espacios al principio o final.

---

### 🔄 Paso 2: Forzar Redespliegue Completo

**Opción A - Desde Vercel Dashboard:**
1. Ve a tu proyecto en Vercel
2. Pestaña **"Deployments"**
3. Busca el último deployment exitoso
4. Haz clic en los **3 puntos** (⋯)
5. Selecciona **"Redeploy"**
6. ✅ Marca **"Use existing Build Cache"** (DESHABILITADO)
7. Confirma el redespliegue

**Opción B - Forzar desde Git:**
```bash
# Hacer un pequeño cambio y push
git commit --allow-empty -m "force redeploy: fix env vars"
git push origin main
```

---

### 🧪 Paso 3: Verificar con Diagnósticos

Una vez completado el redespliegue (2-3 minutos):

1. **Página de diagnóstico:** https://phmetro-phi.vercel.app/debug
2. **Test de Supabase:** https://phmetro-phi.vercel.app/test-supabase
3. **Dashboard principal:** https://phmetro-phi.vercel.app

### Resultados Esperados:
- ✅ **URL configurada:** `https://japrhhtnijidlecqrkpk.supabase.co`
- ✅ **Clave configurada:** `Configurada ✅`
- ✅ **Conexión exitosa:** Datos reales de pH visibles

---

## 🆘 Si TODAVÍA no funciona:

### Diagnóstico Avanzado:

#### 1. Verificar Variables en Tiempo Real
Ve a: https://phmetro-phi.vercel.app/test-supabase

Deberías ver:
```
📊 Configuración
URL: https://japrhhtnijidlecqrkpk.supabase.co
Clave: Configurada ✅
```

Si ves "No configurada" o valores incorrectos, las variables no se propagaron.

#### 2. Eliminar y Recrear Variables
1. **Elimina** todas las variables de entorno en Vercel
2. **Espera 1 minuto**
3. **Agrega** las variables una por una nuevamente
4. **Redespliega** inmediatamente después

#### 3. Verificar desde la Consola de Vercel
1. Ve a **Functions** en tu proyecto Vercel
2. Busca logs de errores recientes
3. Verifica si aparecen mensajes sobre variables faltantes

---

## 🔧 Solución Alternativa: Variables Hardcodeadas Temporalmente

Si nada funciona, puedes probar temporalmente con valores hardcodeados:

```typescript
// Agregar en lib/supabase.ts TEMPORALMENTE
const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL || 'https://japrhhtnijidlecqrkpk.supabase.co'
const supabaseAnonKey = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY || 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM'
```

---

## 📋 Checklist de Verificación:

- [ ] Variables configuradas en Vercel con valores exactos
- [ ] Seleccionados todos los entornos (Production, Preview, Development)
- [ ] Redespliegue forzado sin cache
- [ ] Esperado 3+ minutos después del redespliegue
- [ ] Verificado en páginas de diagnóstico

---

## 🎯 Próximo Paso:

**Ejecuta el Paso 1 y 2 exactamente como se describe** y avísame qué ves en las páginas de diagnóstico después del redespliegue.

Si el problema persiste, podemos implementar la solución alternativa temporal.
