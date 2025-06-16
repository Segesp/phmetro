'use client'

import { useState, useEffect } from 'react'
import { supabase, PhReading } from '@/lib/supabase'
import PhChart from '@/components/PhChart'
import StatsCards from '@/components/StatsCards'
import RecentReadings from '@/components/RecentReadings'
import { Activity, Droplets, TrendingUp, AlertTriangle, Database, RefreshCw } from 'lucide-react'

export default function DashboardFixed() {
  const [readings, setReadings] = useState<PhReading[]>([])
  const [loading, setLoading] = useState(true)
  const [currentPh, setCurrentPh] = useState<number | null>(null)
  const [lastUpdate, setLastUpdate] = useState<string>('')
  const [debugInfo, setDebugInfo] = useState<any>(null)

  const fetchReadings = async () => {
    console.log('🔄 Iniciando fetchReadings...')
    setLoading(true)
    
    try {
      console.log('📊 Configuración Supabase:')
      console.log('  URL:', process.env.NEXT_PUBLIC_SUPABASE_URL)
      console.log('  Key disponible:', !!process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY)
      
      const { data, error } = await supabase
        .from('ph_readings')
        .select('*')
        .order('created_at', { ascending: false })
        .limit(50)

      console.log('📥 Respuesta de Supabase:')
      console.log('  Data length:', data?.length || 0)
      console.log('  Error:', error)
      console.log('  Primer registro:', data?.[0])

      if (error) {
        console.error('❌ Error de Supabase:', error)
        setDebugInfo({ error: error.message, hasData: false })
        return
      }

      if (data && data.length > 0) {
        console.log('✅ Datos obtenidos exitosamente:', data.length, 'registros')
        setReadings(data)
        setCurrentPh(data[0].ph)
        setLastUpdate(new Date().toLocaleString())
        setDebugInfo({ 
          hasData: true, 
          count: data.length, 
          latest: data[0],
          source: 'supabase_real'
        })
      } else {
        console.log('⚠️ No hay datos en Supabase')
        setDebugInfo({ hasData: false, count: 0, source: 'empty' })
      }
    } catch (err) {
      console.error('💥 Error inesperado:', err)
      setDebugInfo({ 
        error: err instanceof Error ? err.message : 'Error desconocido',
        hasData: false 
      })
    } finally {
      setLoading(false)
    }
  }

  useEffect(() => {
    console.log('🚀 Componente montado - Cargando datos...')
    fetchReadings()
  }, [])

  const getPhStatus = (ph: number) => {
    if (ph >= 6.5 && ph <= 8.5) return { status: 'Óptimo', class: 'ph-safe' }
    if (ph >= 6.0 && ph <= 9.0) return { status: 'Aceptable', class: 'ph-warning' }
    return { status: 'Crítico', class: 'ph-danger' }
  }

  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen">
        <div className="text-center">
          <div className="animate-spin rounded-full h-32 w-32 border-b-2 border-blue-500 mx-auto mb-4"></div>
          <p className="text-gray-600">Cargando datos de pH...</p>
        </div>
      </div>
    )
  }

  return (
    <div className="container mx-auto px-4 py-8">
      {/* Header */}
      <div className="mb-8">
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold text-gray-900 mb-2">
              Dashboard pH Metro - CORREGIDO
            </h1>
            <p className="text-gray-600">
              Monitoreo en tiempo real de niveles de pH
            </p>
            {lastUpdate && (
              <p className="text-sm text-blue-600 mt-1">
                Última actualización: {lastUpdate}
              </p>
            )}
          </div>
          
          <div className="text-right space-y-2">
            <button
              onClick={fetchReadings}
              className="flex items-center space-x-2 bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-lg transition-colors"
            >
              <RefreshCw className="h-4 w-4" />
              <span>Actualizar</span>
            </button>
            
            {debugInfo && (
              <div className="bg-gray-100 border p-3 rounded text-xs">
                <div><strong>Debug:</strong></div>
                <div>Datos: {debugInfo.hasData ? '✅' : '❌'}</div>
                <div>Registros: {debugInfo.count || 0}</div>
                <div>Fuente: {debugInfo.source || 'unknown'}</div>
                {debugInfo.error && <div className="text-red-600">Error: {debugInfo.error}</div>}
              </div>
            )}
          </div>
        </div>
      </div>

      {/* Current pH Status */}
      {currentPh !== null && (
        <div className="mb-8">
          <div className={`dashboard-card ${getPhStatus(currentPh).class}`}>
            <div className="flex items-center justify-between">
              <div className="flex items-center space-x-4">
                <Droplets className="h-8 w-8 text-blue-600" />
                <div>
                  <h2 className="text-lg font-semibold text-gray-900">pH Actual</h2>
                  <p className="text-3xl font-bold">{currentPh.toFixed(2)}</p>
                </div>
              </div>
              <div className="text-right">
                <span className="text-sm font-medium">{getPhStatus(currentPh).status}</span>
                <p className="text-xs text-gray-500 mt-1">
                  {readings.length > 0 ? new Date(readings[0].created_at).toLocaleString() : 'Sin datos'}
                </p>
              </div>
            </div>
          </div>
        </div>
      )}

      {/* Información sobre datos */}
      <div className="mb-6">
        {readings.length > 0 ? (
          <div className="bg-green-50 border border-green-200 text-green-800 p-4 rounded-lg">
            <div className="flex items-center space-x-2">
              <Database className="h-5 w-5" />
              <span className="font-medium">✅ Datos Reales Activos</span>
            </div>
            <p className="text-sm mt-1">
              Mostrando {readings.length} lecturas reales de la base de datos Supabase
            </p>
          </div>
        ) : (
          <div className="bg-orange-50 border border-orange-200 text-orange-800 p-4 rounded-lg">
            <div className="flex items-center space-x-2">
              <AlertTriangle className="h-5 w-5" />
              <span className="font-medium">⚠️ Sin Datos Reales</span>
            </div>
            <p className="text-sm mt-1">
              No se encontraron datos en la base de datos. Conecta tu Arduino para ver datos reales.
            </p>
          </div>
        )}
      </div>

      {readings.length > 0 ? (
        <>
          {/* Stats Cards */}
          <div className="mb-8">
            <StatsCards readings={readings} />
          </div>

          {/* Charts and Recent Readings */}
          <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
            <div className="lg:col-span-2">
              <div className="dashboard-card">
                <h3 className="text-lg font-semibold text-gray-900 mb-4 flex items-center">
                  <TrendingUp className="h-5 w-5 mr-2" />
                  Tendencia de pH - Datos Reales
                </h3>
                <PhChart data={readings.slice(0, 24)} />
              </div>
            </div>

            <div>
              <div className="dashboard-card">
                <h3 className="text-lg font-semibold text-gray-900 mb-4 flex items-center">
                  <Activity className="h-5 w-5 mr-2" />
                  Lecturas Recientes
                </h3>
                <RecentReadings readings={readings.slice(0, 10)} />
              </div>
            </div>
          </div>
        </>
      ) : (
        <div className="text-center py-12">
          <Database className="h-16 w-16 text-gray-400 mx-auto mb-4" />
          <h3 className="text-xl font-semibold text-gray-900 mb-2">Sin Datos Disponibles</h3>
          <p className="text-gray-600 mb-4">
            Conecta tu Arduino ESP8266 para comenzar a recibir datos de pH en tiempo real.
          </p>
          <button
            onClick={fetchReadings}
            className="bg-blue-600 hover:bg-blue-700 text-white px-6 py-2 rounded-lg"
          >
            Intentar de Nuevo
          </button>
        </div>
      )}

      {/* Debug Footer */}
      <div className="mt-8 text-center text-gray-500 text-sm">
        <p>Dashboard pH Metro - Sistema corregido para mostrar datos reales</p>
        <p className="text-xs mt-1">
          API: /api/ph-data • Base de datos: Supabase • Modo: Datos reales únicamente
        </p>
      </div>
    </div>
  )
}
