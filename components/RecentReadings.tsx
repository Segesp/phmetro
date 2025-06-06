'use client'

import { PhReading } from '@/lib/supabase'
import { formatDistanceToNow } from 'date-fns'
import { es } from 'date-fns/locale'

interface RecentReadingsProps {
  readings: PhReading[]
}

export default function RecentReadings({ readings }: RecentReadingsProps) {
  const getPhStatusColor = (ph: number) => {
    if (ph >= 6.5 && ph <= 8.5) return 'bg-green-100 text-green-800'
    if (ph >= 6.0 && ph <= 9.0) return 'bg-yellow-100 text-yellow-800'
    return 'bg-red-100 text-red-800'
  }

  const getPhStatusText = (ph: number) => {
    if (ph >= 6.5 && ph <= 8.5) return 'Óptimo'
    if (ph >= 6.0 && ph <= 9.0) return 'Aceptable'
    return 'Crítico'
  }

  if (readings.length === 0) {
    return (
      <div className="text-center py-8 text-gray-500">
        <p>No hay lecturas disponibles</p>
      </div>
    )
  }

  return (
    <div className="space-y-3">
      {readings.map((reading, index) => (
        <div 
          key={reading.id}
          className="flex items-center justify-between p-3 bg-gray-50 rounded-lg border border-gray-200 hover:bg-gray-100 transition-colors"
        >
          <div className="flex items-center space-x-3">
            <div className="w-3 h-3 rounded-full bg-blue-500"></div>
            <div>
              <div className="font-semibold text-gray-900">
                pH {reading.ph.toFixed(2)}
              </div>
              <div className="text-xs text-gray-500">
                {formatDistanceToNow(new Date(reading.created_at), {
                  addSuffix: true,
                  locale: es
                })}
              </div>
            </div>
          </div>
          <div>
            <span className={`px-2 py-1 rounded-full text-xs font-medium ${getPhStatusColor(reading.ph)}`}>
              {getPhStatusText(reading.ph)}
            </span>
          </div>
        </div>
      ))}
      
      {readings.length === 0 && (
        <div className="text-center py-4 text-gray-500">
          <p className="text-sm">No hay lecturas recientes</p>
        </div>
      )}
    </div>
  )
}
