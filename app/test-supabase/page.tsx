import { useState, useEffect, useCallback } from 'react'
import { supabase, PhReading } from '@/lib/supabase'

// Componente de prueba para verificar conexión directa a Supabase
export default function TestSupabase() {
  const [testResults, setTestResults] = useState<any>(null)
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    const testConnection = async () => {
      console.log('🧪 INICIANDO PRUEBA DE SUPABASE...')
      
      try {
        // Información de configuración
        console.log('📊 NEXT_PUBLIC_SUPABASE_URL:', process.env.NEXT_PUBLIC_SUPABASE_URL)
        console.log('🔑 Key presente:', !!process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY)
        
        // Prueba 1: Consulta básica
        console.log('🔍 Haciendo consulta a ph_readings...')
        const { data, error, count } = await supabase
          .from('ph_readings')
          .select('*', { count: 'exact' })
          .limit(5)
        
        console.log('📥 Resultado:', { data: data?.length, error, count })
        
        if (error) {
          console.error('❌ Error de Supabase:', error)
          setTestResults({ 
            success: false, 
            error: error.message,
            config: {
              url: process.env.NEXT_PUBLIC_SUPABASE_URL,
              hasKey: !!process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY
            }
          })
        } else {
          console.log('✅ Conexión exitosa!')
          console.log('📊 Datos obtenidos:', data)
          setTestResults({
            success: true,
            dataCount: data?.length || 0,
            totalRecords: count,
            sampleData: data?.[0],
            config: {
              url: process.env.NEXT_PUBLIC_SUPABASE_URL,
              hasKey: !!process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY
            }
          })
        }
      } catch (err) {
        console.error('💥 Error inesperado:', err)
        setTestResults({ 
          success: false, 
          error: err instanceof Error ? err.message : 'Error desconocido',
          config: {
            url: process.env.NEXT_PUBLIC_SUPABASE_URL,
            hasKey: !!process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY
          }
        })
      } finally {
        setLoading(false)
      }
    }
    
    testConnection()
  }, [])

  if (loading) {
    return (
      <div className="p-8">
        <h1 className="text-2xl font-bold mb-4">🧪 Probando Conexión a Supabase...</h1>
        <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-blue-500"></div>
      </div>
    )
  }

  return (
    <div className="p-8 max-w-4xl mx-auto">
      <h1 className="text-3xl font-bold mb-6">🧪 Diagnóstico de Supabase</h1>
      
      <div className="bg-gray-100 p-6 rounded-lg mb-6">
        <h2 className="text-xl font-semibold mb-4">📊 Configuración</h2>
        <div className="space-y-2 font-mono text-sm">
          <div>URL: {testResults?.config?.url || 'No configurada'}</div>
          <div>Clave: {testResults?.config?.hasKey ? 'Configurada ✅' : 'No configurada ❌'}</div>
        </div>
      </div>

      {testResults?.success ? (
        <div className="bg-green-100 border border-green-400 p-6 rounded-lg">
          <h2 className="text-xl font-semibold text-green-800 mb-4">✅ Conexión Exitosa</h2>
          <div className="space-y-3">
            <div><strong>Registros encontrados:</strong> {testResults.dataCount}</div>
            <div><strong>Total en BD:</strong> {testResults.totalRecords}</div>
            {testResults.sampleData && (
              <div>
                <strong>Datos de muestra:</strong>
                <pre className="bg-gray-800 text-green-400 p-3 rounded mt-2 text-sm overflow-auto">
                  {JSON.stringify(testResults.sampleData, null, 2)}
                </pre>
              </div>
            )}
          </div>
          
          <div className="mt-6">
            <a 
              href="/" 
              className="bg-blue-600 text-white px-4 py-2 rounded hover:bg-blue-700"
            >
              🚀 Ir al Dashboard Principal
            </a>
          </div>
        </div>
      ) : (
        <div className="bg-red-100 border border-red-400 p-6 rounded-lg">
          <h2 className="text-xl font-semibold text-red-800 mb-4">❌ Error de Conexión</h2>
          <div className="text-red-700">
            <strong>Error:</strong> {testResults?.error}
          </div>
          
          <div className="mt-4 space-y-2">
            <h3 className="font-semibold">🔧 Posibles soluciones:</h3>
            <ul className="list-disc list-inside space-y-1 text-sm">
              <li>Verificar variables de entorno en .env.local</li>
              <li>Comprobar que la URL de Supabase sea correcta</li>
              <li>Verificar que la clave anónima sea válida</li>
              <li>Confirmar que la tabla 'ph_readings' exista</li>
            </ul>
          </div>
        </div>
      )}
      
      <div className="mt-8 text-center">
        <button 
          onClick={() => window.location.reload()}
          className="bg-gray-600 text-white px-4 py-2 rounded hover:bg-gray-700"
        >
          🔄 Volver a Probar
        </button>
      </div>
    </div>
  )
}
