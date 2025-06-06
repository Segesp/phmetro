'use client'

import { useState, useEffect } from 'react'
import { supabase, PhReading } from '@/lib/supabase'
import PhChart from '@/components/PhChart'
import StatsCards from '@/components/StatsCards'
import RecentReadings from '@/components/RecentReadings'
import { Activity, Droplets, TrendingUp, AlertTriangle } from 'lucide-react'

export default function Dashboard() {
  const [readings, setReadings] = useState<PhReading[]>([])
  const [loading, setLoading] = useState(true)
  const [currentPh, setCurrentPh] = useState<number | null>(null)

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
        }
      )
      .subscribe()

    return () => {
      subscription.unsubscribe()
    }
  }, [])

  const fetchReadings = async () => {
    try {
      const { data, error } = await supabase
        .from('ph_readings')
        .select('*')
        .order('created_at', { ascending: false })
        .limit(100)

      if (error) throw error
      
      setReadings(data || [])
      if (data && data.length > 0) {
        setCurrentPh(data[0].ph)
      }
    } catch (error) {
      console.error('Error fetching readings:', error)
    } finally {
      setLoading(false)
    }
  }

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
        <h1 className="text-3xl font-bold text-gray-900 mb-2">
          Dashboard pH Metro
        </h1>
        <p className="text-gray-600">
          Monitoreo en tiempo real de niveles de pH
        </p>
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
              Última actualización: {new Date().toLocaleString('es-ES')}
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
            </h3>
            <PhChart data={readings.slice(0, 50).reverse()} />
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

      {/* Footer */}
      <div className="text-center text-gray-500 text-sm">
        <p>Sistema de monitoreo pH - Datos actualizados automáticamente</p>
      </div>
    </div>
  )
}
