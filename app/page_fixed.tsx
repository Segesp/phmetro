'use client'

import { useState, useEffect, useCallback } from 'react'
import { supabase, PhReading } from '@/lib/supabase'
import PhChart from '@/components/PhChart'
import StatsCards from '@/components/StatsCards'
import RecentReadings from '@/components/RecentReadings'
import { Activity, Droplets, TrendingUp, AlertTriangle, Database } from 'lucide-react'

export default function Dashboard() {
  const [readings, setReadings] = useState<PhReading[]>([])
  const [loading, setLoading] = useState(true)
  const [currentPh, setCurrentPh] = useState<number | null>(null)
  const [isUsingMockData, setIsUsingMockData] = useState(false)

  const generateSampleData = (): PhReading[] => {
    // Generar datos de ejemplo para las últimas horas
    const sampleData: PhReading[] = []
    const now = new Date()
    
    for (let i = 0; i < 48; i++) {
      const timestamp = new Date(now.getTime() - (i * 30 * 60 * 1000)) // Cada 30 minutos
      const baseTime = timestamp.getHours() + timestamp.getMinutes() / 60
      
      // Simular variaciones naturales de pH con patrones realistas
      let ph = 7.0 + Math.sin(baseTime * Math.PI / 12) * 0.5 + (Math.random() - 0.5) * 0.3
      ph = Math.max(6.0, Math.min(8.5, ph)) // Mantener en rango realista
      
      sampleData.push({
        id: `demo-${i}`,
        ph: Math.round(ph * 100) / 100,
        timestamp: timestamp.toISOString(),
        created_at: timestamp.toISOString()
      })
    }
    
    return sampleData.reverse() // Ordenar cronológicamente
  }

  const fetchReadings = useCallback(async () => {
    try {
      const { data, error } = await supabase
        .from('ph_readings')
        .select('*')
        .order('created_at', { ascending: false })
        .limit(100)

      if (error) throw error
      
      // Si no hay datos, usar datos de ejemplo
      if (!data || data.length === 0) {
        console.log('No hay datos en la base de datos, mostrando datos de demostración...')
        const sampleData = generateSampleData()
        setReadings(sampleData)
        setCurrentPh(sampleData[sampleData.length - 1].ph)
        setIsUsingMockData(true)
      } else {
        setReadings(data || [])
        if (data && data.length > 0) {
          setCurrentPh(data[0].ph)
        }
        setIsUsingMockData(false)
      }
    } catch (error) {
      console.error('Error fetching readings:', error)
      // En caso de error, también usar datos de ejemplo
      console.log('Error en conexión, mostrando datos de demostración...')
      const sampleData = generateSampleData()
      setReadings(sampleData)
      setCurrentPh(sampleData[sampleData.length - 1].ph)
      setIsUsingMockData(true)
    } finally {
      setLoading(false)
    }
  }, [])

  const addMockDataToDatabase = async () => {
    try {
      // Generar algunos datos de ejemplo y agregarlos a la base de datos
      const mockData = []
      const now = new Date()
      
      for (let i = 0; i < 5; i++) {
        const timestamp = new Date(now.getTime() - (i * 10 * 60 * 1000)) // Cada 10 minutos
        const ph = 7.0 + (Math.random() - 0.5) * 0.6 // pH entre 6.7 y 7.3
        
        mockData.push({
          ph: Math.round(ph * 100) / 100,
          timestamp: timestamp.toISOString()
        })
      }
      
      const { error } = await supabase
        .from('ph_readings')
        .insert(mockData)
        
      if (!error) {
        fetchReadings() // Recargar datos
      }
    } catch (error) {
      console.error('Error adding mock data:', error)
    }
  }

  useEffect(() => {
    fetchReadings()
    
    // Suscripción en tiempo real
    const subscription = supabase
      .channel('ph_readings')
      .on('postgres_changes', 
        { event: 'INSERT', schema: 'public', table: 'ph_readings' },
        (payload: any) => {
          setReadings(prev => [payload.new as PhReading, ...prev].slice(0, 100))
          setCurrentPh((payload.new as PhReading).ph)
          setIsUsingMockData(false)
        }
      )
      .subscribe()

    return () => {
      subscription.unsubscribe()
    }
  }, [fetchReadings])

  const getPhStatus = (ph: number) => {
    if (ph >= 6.5 && ph <= 8.5) return { status: 'Óptimo', class: 'ph-safe' }
    if (ph >= 6.0 && ph <= 9.0) return { status: 'Aceptable', class: 'ph-warning' }
    return { status: 'Crítico', class: 'ph-danger' }
  }

  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen">
        <div className="animate-spin rounded-full h-32 w-32 border-b-2 border-blue-500"></div>
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
          
          {isUsingMockData && (
            <div className="text-right">
              <div className="bg-yellow-100 border border-yellow-400 text-yellow-800 px-4 py-2 rounded-lg mb-2">
                <div className="flex items-center space-x-2">
                  <Database className="h-4 w-4" />
                  <span className="text-sm font-medium">Datos de Demostración</span>
                </div>
              </div>
              <button
                onClick={addMockDataToDatabase}
                className="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-lg text-sm transition-colors"
              >
                Agregar Datos Reales
              </button>
            </div>
          )}
        </div>
      </div>

      {/* Current pH Status */}
      {currentPh !== null && (
        <div className="mb-8 p-6 bg-white rounded-lg shadow-md border border-gray-200">
          <div className="flex items-center justify-between">
            <div className="flex items-center space-x-4">
              <div className="p-3 bg-blue-100 rounded-full">
                <Droplets className="h-8 w-8 text-blue-600" />
              </div>
              <div>
                <h2 className="text-2xl font-bold text-gray-900">
                  pH: {currentPh.toFixed(2)}
                </h2>
                <span className={`ph-indicator ${getPhStatus(currentPh).class}`}>
                  {getPhStatus(currentPh).status}
                </span>
              </div>
            </div>
            <div className="text-right text-sm text-gray-500">
              <div>Última actualización: {new Date().toLocaleString('es-ES')}</div>
              {isUsingMockData && (
                <div className="text-yellow-600 font-medium">
                  (Datos de demostración)
                </div>
              )}
            </div>
          </div>
        </div>
      )}

      {/* Stats Cards */}
      <StatsCards readings={readings} />

      {/* Charts and Recent Readings */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-8 mb-8">
        <div className="lg:col-span-2">
          <div className="dashboard-card">
            <h3 className="text-lg font-semibold text-gray-900 mb-4 flex items-center">
              <TrendingUp className="h-5 w-5 mr-2" />
              Gráfico de pH en Tiempo Real
              {isUsingMockData && (
                <span className="ml-2 bg-yellow-100 text-yellow-800 text-xs px-2 py-1 rounded">
                  Demo
                </span>
              )}
            </h3>
            <PhChart data={readings.slice(0, 50).reverse()} />
          </div>
        </div>
        
        <div>
          <div className="dashboard-card">
            <h3 className="text-lg font-semibold text-gray-900 mb-4 flex items-center">
              <Activity className="h-5 w-5 mr-2" />
              Lecturas Recientes
              {isUsingMockData && (
                <span className="ml-2 bg-yellow-100 text-yellow-800 text-xs px-2 py-1 rounded">
                  Demo
                </span>
              )}
            </h3>
            <RecentReadings readings={readings.slice(0, 10)} />
          </div>
        </div>
      </div>

      {/* Info Footer */}
      <div className="text-center text-gray-500 text-sm space-y-2">
        <p>Sistema de monitoreo pH - Datos actualizados automáticamente</p>
        {isUsingMockData && (
          <div className="bg-blue-50 border border-blue-200 text-blue-800 p-4 rounded-lg">
            <p className="font-medium">🚀 ¡Dashboard Funcionando!</p>
            <p className="text-sm mt-1">
              Estos son datos de demostración. Conecta tu Arduino ESP8266 para ver datos reales en tiempo real.
            </p>
            <p className="text-xs mt-2 text-blue-600">
              Endpoint API: <code>/api/ph-data</code> • Base de datos: Supabase
            </p>
          </div>
        )}
      </div>
    </div>
  )
}
