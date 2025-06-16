'use client'

import { useState, useEffect } from 'react'
import { supabase, PhReading } from '@/lib/supabase'

export default function TestDashboard() {
  const [readings, setReadings] = useState<PhReading[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)
  const [debugInfo, setDebugInfo] = useState<any>({})

  useEffect(() => {
    const testFetch = async () => {
      console.log('🧪 TEST: Iniciando consulta directa a Supabase...')
      
      try {
        // Consulta directa sin funciones auxiliares
        const { data, error } = await supabase
          .from('ph_readings')
          .select('*')
          .order('created_at', { ascending: false })
          .limit(10)

        console.log('🧪 TEST: Resultado:', { data, error })

        setDebugInfo({
          timestamp: new Date().toISOString(),
          hasData: !!data,
          dataLength: data?.length || 0,
          hasError: !!error,
          errorMessage: error?.message,
          firstRecord: data?.[0],
          allRecords: data
        })

        if (error) {
          setError(error.message)
          setReadings([])
        } else {
          setError(null)
          setReadings(data || [])
        }
      } catch (err) {
        console.error('🧪 TEST: Error catch:', err)
        setError((err as Error).message)
        setReadings([])
      } finally {
        setLoading(false)
      }
    }

    testFetch()
  }, [])

  return (
    <div className="container mx-auto px-4 py-8 max-w-4xl">
      <h1 className="text-2xl font-bold mb-6">🧪 Test Dashboard Simplificado</h1>
      
      {loading && (
        <div className="bg-blue-100 border border-blue-400 text-blue-700 px-4 py-3 rounded mb-4">
          Cargando datos...
        </div>
      )}

      {error && (
        <div className="bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded mb-4">
          <strong>Error:</strong> {error}
        </div>
      )}

      {!loading && !error && readings.length === 0 && (
        <div className="bg-yellow-100 border border-yellow-400 text-yellow-700 px-4 py-3 rounded mb-4">
          No se encontraron datos en la base de datos
        </div>
      )}

      {!loading && !error && readings.length > 0 && (
        <div className="bg-green-100 border border-green-400 text-green-700 px-4 py-3 rounded mb-4">
          <strong>✅ Datos encontrados:</strong> {readings.length} registros
        </div>
      )}

      {/* Información de debug */}
      <div className="bg-white border border-gray-200 rounded-lg p-6 mb-6">
        <h2 className="text-lg font-semibold mb-4">🔍 Información de Debug</h2>
        <pre className="bg-gray-100 p-4 rounded text-xs overflow-auto">
          {JSON.stringify(debugInfo, null, 2)}
        </pre>
      </div>

      {/* Lista de lecturas */}
      {readings.length > 0 && (
        <div className="bg-white border border-gray-200 rounded-lg p-6">
          <h2 className="text-lg font-semibold mb-4">📊 Lecturas de pH</h2>
          <div className="space-y-3">
            {readings.map((reading, index) => (
              <div key={reading.id} className="flex items-center justify-between p-3 bg-gray-50 rounded border">
                <div>
                  <div className="font-bold text-lg">pH {reading.ph}</div>
                  <div className="text-sm text-gray-600">ID: {reading.id}</div>
                </div>
                <div className="text-right">
                  <div className="text-sm">{new Date(reading.created_at).toLocaleString()}</div>
                  <div className="text-xs text-gray-500">#{index + 1}</div>
                </div>
              </div>
            ))}
          </div>
        </div>
      )}

      {/* Acciones */}
      <div className="mt-6 space-x-4">
        <button
          onClick={() => window.location.reload()}
          className="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded"
        >
          🔄 Recargar
        </button>
        <a
          href="/"
          className="bg-green-600 hover:bg-green-700 text-white px-4 py-2 rounded inline-block"
        >
          🏠 Dashboard Principal
        </a>
        <a
          href="/debug"
          className="bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded inline-block"
        >
          🔧 Diagnóstico Completo
        </a>
      </div>
    </div>
  )
}
