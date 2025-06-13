-- =============================================
-- ACTUALIZACIÓN DE TABLA - AGREGAR COLUMNA DEVICE
-- =============================================
-- Ejecutar en Supabase SQL Editor

-- 1. Agregar columna 'device' si no existe
DO $$ 
BEGIN
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns 
                   WHERE table_name = 'ph_readings' AND column_name = 'device') THEN
        ALTER TABLE ph_readings ADD COLUMN device VARCHAR(50) DEFAULT 'Arduino';
    END IF;
END $$;

-- 2. Actualizar registros existentes sin device
UPDATE ph_readings 
SET device = 'Arduino' 
WHERE device IS NULL;

-- 3. Crear índice para device
CREATE INDEX IF NOT EXISTS idx_ph_readings_device ON ph_readings(device);

-- 4. Verificar estructura actualizada
SELECT column_name, data_type, is_nullable, column_default
FROM information_schema.columns 
WHERE table_name = 'ph_readings' 
ORDER BY ordinal_position;

-- 5. Mostrar últimos registros para verificar
SELECT * FROM ph_readings ORDER BY created_at DESC LIMIT 5;
