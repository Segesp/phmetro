'use client'

import { PhReading } from '@/lib/supabase'
import { TrendingUp, TrendingDown, Activity, Clock } from 'lucide-react'

interface StatsCardsProps {
  readings: PhReading[]
}

export default function StatsCards({ readings }: StatsCardsProps) {
  if (readings.length === 0) {
    return (
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-8">
        {[1, 2, 3, 4].map(i => (
          <div key={i} className="stat-card animate-pulse">
            <div className="h-4 bg-blue-400 rounded mb-2"></div>
            <div className="h-8 bg-blue-400 rounded"></div>
          </div>
        ))}
      </div>
    )
  }

  // Función para determinar estado del pH (misma lógica que el dashboard)
  const getPhStatusColor = (ph: number) => {
    if (ph < 6.0 || ph > 9.0) return 'from-red-500 to-red-600' // Crítico
    if ((ph >= 6.0 && ph < 6.5) || (ph > 8.5 && ph <= 9.0)) return 'from-orange-500 to-orange-600' // Advertencia
    return 'from-green-500 to-green-600' // Óptimo
  }

  const getPhStatusText = (ph: number) => {
    if (ph < 6.0) return '🚨 Crítico Bajo'
    if (ph > 9.0) return '🚨 Crítico Alto'
    if (ph >= 6.0 && ph < 6.5) return '⚠️ Advertencia Bajo'
    if (ph > 8.5 && ph <= 9.0) return '⚠️ Advertencia Alto'
    return '✅ Óptimo'
  }

  const currentPh = readings[0]?.ph || 0
  const phValues = readings.map(r => r.ph)
  const avgPh = phValues.reduce((a, b) => a + b, 0) / phValues.length
  const minPh = Math.min(...phValues)
  const maxPh = Math.max(...phValues)

  // Calcular tendencia (comparar últimas 5 lecturas con las anteriores 5)
  const recent = phValues.slice(0, 5)
  const previous = phValues.slice(5, 10)
  const recentAvg = recent.reduce((a, b) => a + b, 0) / recent.length
  const previousAvg = previous.reduce((a, b) => a + b, 0) / previous.length
  const trend = recentAvg - previousAvg

  // Calcular estadísticas de estados
  const criticalCount = phValues.filter(ph => ph < 6.0 || ph > 9.0).length
  const warningCount = phValues.filter(ph => (ph >= 6.0 && ph < 6.5) || (ph > 8.5 && ph <= 9.0)).length
  const optimalCount = phValues.filter(ph => ph >= 6.5 && ph <= 8.5).length
  
  const stats = [
    {
      title: 'pH Actual',
      value: currentPh.toFixed(2),
      icon: Activity,
      color: getPhStatusColor(currentPh),
      subtitle: getPhStatusText(currentPh)
    },
    {
      title: 'Estado General',
      value: `${Math.round((optimalCount / readings.length) * 100)}%`,
      icon: TrendingUp,
      color: optimalCount > warningCount + criticalCount ? 'from-green-500 to-green-600' : 
             warningCount > criticalCount ? 'from-orange-500 to-orange-600' : 'from-red-500 to-red-600',
      subtitle: `${optimalCount} óptimas de ${readings.length}`
    },
    {
      title: 'Rango pH',
      value: `${minPh.toFixed(1)}-${maxPh.toFixed(1)}`,
      icon: TrendingDown,
      color: 'from-purple-500 to-purple-600',
      subtitle: `Promedio: ${avgPh.toFixed(2)}`
    },
    {
      title: 'Alertas',
      value: `${criticalCount + warningCount}`,
      icon: trend > 0 ? TrendingUp : TrendingDown,
      color: criticalCount > 0 ? 'from-red-500 to-red-600' : 
             warningCount > 0 ? 'from-orange-500 to-orange-600' : 'from-green-500 to-green-600',
      subtitle: `${criticalCount} críticas, ${warningCount} advertencias`
    }
  ]

  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-8">
      {stats.map((stat, index) => {
        const Icon = stat.icon
        return (
          <div 
            key={index}
            className={`bg-gradient-to-r ${stat.color} text-white rounded-lg p-6 shadow-lg transform hover:scale-105 transition-transform`}
          >
            <div className="flex items-center justify-between">
              <div>
                <p className="text-white/80 text-sm font-medium">
                  {stat.title}
                </p>
                <p className="text-2xl font-bold mt-1">
                  {stat.value}
                </p>
                <p className="text-white/70 text-xs mt-1">
                  {stat.subtitle}
                </p>
              </div>
              <div className="p-3 bg-white/20 rounded-full">
                <Icon className="h-6 w-6 text-white" />
              </div>
            </div>
          </div>
        )
      })}
    </div>
  )
}
