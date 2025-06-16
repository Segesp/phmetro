'use client'

import { useState, useEffect, useCallback } from 'react'
import { supabase, PhReading } from '@/lib/supabase'
import PhChart from '@/components/PhChart'
import StatsCards from '@/components/StatsCards'
import RecentReadings from '@/components/RecentReadings'
import { Activity, Droplets, TrendingUp, AlertTriangle, Database, RefreshCw } from 'lucide-react'

export default function DashboardFixed() {
  const [readings, setReadings] = useState<PhReading[]>([])
  const [loading, setLoading] = useState(true)
  const [currentPh, setCurrentPh] = useState<number | null>(null)
  const [error, setError] = useState<string | null>(null)
  const [lastUpdate, setLastUpdate] = useState<string>('')

  const fetchReadings = useCallback(async () => {
    console.log('🚀 [DASHBOARD-FIXED] Iniciando consulta...')
    setLoading(true)
    setError(null)
    
    try {
      const { data, error } = await supabase
        .from('ph_readings')
        .select('*')
        .order('created_at', { ascending: false })
        .limit(100)

      console.log('📊 [DASHBOARD-FIXED] Datos recibidos:', {
        success: !error,
        count: data?.length || 0,
        firstRecord: data?.[0]
      })

      if (error) {
        throw new Error(`Error de Supabase: ${error.message}`)
      }

      if (data && data.length > 0) {
        setReadings(data)
        setCurrentPh(data[0].ph)
        setLastUpdate(new Date().toLocaleString())
        console.log('✅ [DASHBOARD-FIXED] Datos cargados exitosamente')
      } else {
        setReadings([])
        setCurrentPh(null)
        console.log('⚠️ [DASHBOARD-FIXED] No hay datos disponibles')
      }
      
    } catch (err) {
      const errorMessage = (err as Error).message
      console.error('❌ [DASHBOARD-FIXED] Error:', errorMessage)
      setError(errorMessage)
      setReadings([])
      setCurrentPh(null)
    } finally {
      setLoading(false)
    }
  }, [])

  useEffect(() => {
    fetchReadings()
    
    // Suscripción en tiempo real
    const subscription = supabase
      .channel('ph_readings_realtime')
      .on('postgres_changes', 
        { event: 'INSERT', schema: 'public', table: 'ph_readings' },
        (payload: any) => {
          console.log('📡 [DASHBOARD-FIXED] Nuevo dato en tiempo real:', payload.new)
          const newReading = payload.new as PhReading
          setReadings(prev => [newReading, ...prev].slice(0, 100))
          setCurrentPh(newReading.ph)
          setLastUpdate(new Date().toLocaleString())
        }
      )
      .subscribe()

    // Auto-refresh cada 30 segundos
    const interval = setInterval(fetchReadings, 30000)

    return () => {
      subscription.unsubscribe()
      clearInterval(interval)
    }
  }, [fetchReadings])

  const getPhStatus = (ph: number) => {
    if (ph >= 6.5 && ph <= 8.5) return { status: 'Óptimo', class: 'ph-safe', color: 'text-green-600' }
    if (ph >= 6.0 && ph <= 9.0) return { status: 'Aceptable', class: 'ph-warning', color: 'text-yellow-600' }
    return { status: 'Crítico', class: 'ph-danger', color: 'text-red-600' }
  }

  // Loading state
  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen">
        <div className="text-center">
          <div className="animate-spin rounded-full h-16 w-16 border-b-2 border-blue-500 mx-auto mb-4"></div>
          <p className="text-gray-600">Cargando datos del pH Metro...</p>
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
              Dashboard pH Metro
            </h1>
            <p className="text-gray-600">
              Monitoreo en tiempo real de niveles de pH
            </p>
          </div>
          
          <div className="text-right">
            <button
              onClick={fetchReadings}
              disabled={loading}
              className="bg-blue-600 hover:bg-blue-700 disabled:bg-blue-400 text-white px-4 py-2 rounded-lg transition-colors flex items-center space-x-2"
            >
              <RefreshCw className={`h-4 w-4 ${loading ? 'animate-spin' : ''}`} />
              <span>{loading ? 'Actualizando...' : 'Actualizar'}</span>
            </button>
            {lastUpdate && (
              <p className="text-xs text-gray-500 mt-1">
                Última actualización: {lastUpdate}
              </p>
            )}
          </div>
        </div>
      </div>

      {/* Error State */}
      {error && (
        <div className="mb-8 p-6 bg-red-50 border border-red-200 rounded-lg">
          <div className="flex items-center space-x-2 mb-2">
            <AlertTriangle className="h-5 w-5 text-red-600" />
            <span className="text-lg font-bold text-red-800">Error de Conexión</span>
          </div>
          <p className="text-red-700 mb-4">{error}</p>
          <button
            onClick={fetchReadings}
            className="bg-red-600 hover:bg-red-700 text-white px-4 py-2 rounded transition-colors"
          >
            Intentar Nuevamente
          </button>
        </div>
      )}

      {/* No Data State */}
      {!error && readings.length === 0 && (
        <div className="mb-8 p-6 bg-yellow-50 border border-yellow-200 rounded-lg">
          <div className="flex items-center space-x-2 mb-2">
            <Database className="h-5 w-5 text-yellow-600" />
            <span className="text-lg font-bold text-yellow-800">Sin Datos Disponibles</span>
          </div>
          <p className="text-yellow-700 mb-4">
            No se encontraron lecturas de pH en la base de datos. 
            Asegúrate de que tu Arduino esté enviando datos.
          </p>
          <div className="text-sm text-yellow-600">
            <p>✅ Conexión a base de datos: OK</p>
            <p>❌ Datos de pH: No disponibles</p>
          </div>
        </div>
      )}

      {/* Current pH Status */}
      {currentPh !== null && (
        <div className="mb-8 p-6 bg-white rounded-lg shadow-md border border-gray-200">
          <div className="flex items-center justify-between">
            <div className="flex items-center space-x-4">
              <div className="p-3 bg-blue-100 rounded-full">
                <Droplets className="h-8 w-8 text-blue-600" />
              </div>
              <div>
                <h2 className="text-3xl font-bold text-gray-900">
                  pH: {currentPh.toFixed(2)}
                </h2>
                <span className={`text-lg font-medium ${getPhStatus(currentPh).color}`}>
                  {getPhStatus(currentPh).status}
                </span>
              </div>
            </div>
            <div className="text-right">
              <div className="bg-green-100 text-green-800 px-3 py-1 rounded-full text-sm font-medium">
                ✅ Datos Reales
              </div>
              <p className="text-xs text-gray-500 mt-1">
                {readings.length} lecturas disponibles
              </p>
            </div>
          </div>
        </div>
      )}

      {/* Data Available - Show Dashboard */}
      {readings.length > 0 && (
        <>
          {/* Stats Cards */}
          <StatsCards readings={readings} />

          {/* Charts and Recent Readings */}
          <div className="grid grid-cols-1 lg:grid-cols-3 gap-8 mb-8">
            <div className="lg:col-span-2">
              <div className="bg-white rounded-lg shadow-md border border-gray-200 p-6">
                <h3 className="text-lg font-semibold text-gray-900 mb-4 flex items-center">
                  <TrendingUp className="h-5 w-5 mr-2" />
                  Gráfico de pH en Tiempo Real
                  <span className="ml-2 bg-green-100 text-green-800 text-xs px-2 py-1 rounded">
                    En Vivo
                  </span>
                </h3>
                <PhChart data={readings.slice(0, 50).reverse()} />
              </div>
            </div>
            
            <div>
              <div className="bg-white rounded-lg shadow-md border border-gray-200 p-6">
                <h3 className="text-lg font-semibold text-gray-900 mb-4 flex items-center">
                  <Activity className="h-5 w-5 mr-2" />
                  Lecturas Recientes
                  <span className="ml-2 bg-green-100 text-green-800 text-xs px-2 py-1 rounded">
                    Real
                  </span>
                </h3>
                <RecentReadings readings={readings.slice(0, 10)} />
              </div>
            </div>
          </div>

          {/* Success Footer */}
          <div className="text-center">
            <div className="bg-green-50 border border-green-200 text-green-800 p-4 rounded-lg inline-block">
              <p className="font-medium">✅ Sistema Operativo</p>
              <p className="text-sm mt-1">
                Mostrando {readings.length} lecturas reales de pH desde Supabase
              </p>
              <p className="text-xs text-green-600 mt-1">
                Última lectura: {new Date(readings[0]?.created_at).toLocaleString()}
              </p>
            </div>
          </div>
        </>
      )}
    </div>
  )
}
