-- Script de inicialización para Supabase
-- Ejecutar en el SQL Editor de Supabase

-- Crear tabla para lecturas de pH
CREATE TABLE IF NOT EXISTS ph_readings (
  id UUID DEFAULT gen_random_uuid() PRIMARY KEY,
  ph DECIMAL(4,2) NOT NULL CHECK (ph >= 0 AND ph <= 14),
  timestamp TIMESTAMPTZ,
  created_at TIMESTAMPTZ DEFAULT NOW()
);

-- Crear índice para mejorar el rendimiento de consultas
CREATE INDEX IF NOT EXISTS idx_ph_readings_created_at ON ph_readings(created_at DESC);
CREATE INDEX IF NOT EXISTS idx_ph_readings_ph ON ph_readings(ph);

-- Habilitar Row Level Security
ALTER TABLE ph_readings ENABLE ROW LEVEL SECURITY;

-- Crear política para permitir inserción (desde la API)
DROP POLICY IF EXISTS "Allow insert ph_readings" ON ph_readings;
CREATE POLICY "Allow insert ph_readings" ON ph_readings
  FOR INSERT WITH CHECK (true);

-- Crear política para permitir lectura (desde el frontend)
DROP POLICY IF EXISTS "Allow select ph_readings" ON ph_readings;
CREATE POLICY "Allow select ph_readings" ON ph_readings
  FOR SELECT USING (true);

-- Habilitar realtime para actualizaciones en tiempo real
ALTER PUBLICATION supabase_realtime ADD TABLE ph_readings;

-- Función para limpiar datos antiguos (opcional)
CREATE OR REPLACE FUNCTION cleanup_old_readings()
RETURNS void AS $$
BEGIN
  DELETE FROM ph_readings 
  WHERE created_at < NOW() - INTERVAL '30 days';
END;
$$ LANGUAGE plpgsql;

-- Comentarios para documentación
COMMENT ON TABLE ph_readings IS 'Tabla para almacenar lecturas del sensor de pH';
COMMENT ON COLUMN ph_readings.id IS 'Identificador único de la lectura';
COMMENT ON COLUMN ph_readings.ph IS 'Valor de pH medido (0-14)';
COMMENT ON COLUMN ph_readings.timestamp IS 'Timestamp proporcionado por el Arduino';
COMMENT ON COLUMN ph_readings.created_at IS 'Timestamp de cuando se insertó el registro';

-- Insertar algunos datos de ejemplo (opcional)
INSERT INTO ph_readings (ph, timestamp) VALUES 
  (7.2, NOW() - INTERVAL '5 minutes'),
  (7.1, NOW() - INTERVAL '4 minutes'),
  (7.3, NOW() - INTERVAL '3 minutes'),
  (7.0, NOW() - INTERVAL '2 minutes'),
  (7.2, NOW() - INTERVAL '1 minute');

-- Verificar que todo esté configurado correctamente
SELECT 
  'Tabla creada correctamente' as status,
  COUNT(*) as total_readings
FROM ph_readings;
