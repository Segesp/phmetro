'use client'

import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts'
import { PhReading } from '@/lib/supabase'

interface PhChartProps {
  data: PhReading[]
  filterType?: 'all' | 'day' | 'week' | 'month' | 'dayOfWeek' | 'monthOfYear'
}

export default function PhChart({ data, filterType = 'all' }: PhChartProps) {
  const getTimeFormat = (reading: PhReading) => {
    const date = new Date(reading.created_at)
    
    switch (filterType) {
      case 'day':
        // Para 24 horas: mostrar hora:minuto
        return date.toLocaleTimeString('es-ES', {
          hour: '2-digit',
          minute: '2-digit'
        })
      case 'week':
        // Para 7 días: mostrar día y hora
        return date.toLocaleDateString('es-ES', {
          weekday: 'short',
          hour: '2-digit',
          minute: '2-digit'
        })
      case 'month':
        // Para 30 días: mostrar día/mes
        return date.toLocaleDateString('es-ES', {
          day: '2-digit',
          month: '2-digit'
        })
      case 'dayOfWeek':
        // Para día específico: mostrar fecha y hora
        return date.toLocaleDateString('es-ES', {
          day: '2-digit',
          month: '2-digit',
          hour: '2-digit',
          minute: '2-digit'
        })
      case 'monthOfYear':
        // Para mes específico: mostrar día
        return date.toLocaleDateString('es-ES', {
          day: '2-digit'
        })
      default:
        // Por defecto: hora:minuto
        return date.toLocaleTimeString('es-ES', {
          hour: '2-digit',
          minute: '2-digit'
        })
    }
  }

  const generateCompleteTimeRange = (data: PhReading[], filterType: string) => {
    if (data.length === 0) return []
    
    const now = new Date()
    let startDate: Date
    let interval: number
    let timeUnit: 'hour' | 'day' | 'month'
    
    switch (filterType) {
      case 'day':
        // Últimas 24 horas, intervalos de 1 hora
        startDate = new Date(now.getTime() - 24 * 60 * 60 * 1000)
        interval = 60 * 60 * 1000 // 1 hora en ms
        timeUnit = 'hour'
        break
      case 'week':
        // Últimos 7 días, intervalos de 1 día
        startDate = new Date(now.getTime() - 7 * 24 * 60 * 60 * 1000)
        startDate.setHours(0, 0, 0, 0) // Empezar al inicio del día
        interval = 24 * 60 * 60 * 1000 // 1 día en ms
        timeUnit = 'day'
        break
      case 'month':
        // Últimos 30 días, intervalos de 1 día
        startDate = new Date(now.getTime() - 30 * 24 * 60 * 60 * 1000)
        startDate.setHours(0, 0, 0, 0) // Empezar al inicio del día
        interval = 24 * 60 * 60 * 1000 // 1 día en ms
        timeUnit = 'day'
        break
      default:
        // Para otros filtros, usar los datos tal como están
        return data.map(reading => ({
          time: getTimeFormat(reading),
          ph: reading.ph,
          fullTime: new Date(reading.created_at).toLocaleString('es-ES'),
          rawDate: new Date(reading.created_at),
          hasData: true
        }))
    }
    
    // Crear array de todos los puntos de tiempo en el rango
    const timePoints: Array<{
      time: string
      ph: number | null
      fullTime: string
      rawDate: Date
      hasData: boolean
    }> = []
    
    const endDate = new Date(now)
    let currentDate = new Date(startDate)
    
    while (currentDate <= endDate) {
      // Buscar datos reales para este punto de tiempo
      const tolerance = interval / 2 // Tolerancia de media hora/medio día
      const matchingReading = data.find(reading => {
        const readingDate = new Date(reading.created_at)
        return Math.abs(readingDate.getTime() - currentDate.getTime()) < tolerance
      })
      
      const timeFormat = getTimeFormatForDate(currentDate, filterType)
      
      if (matchingReading) {
        timePoints.push({
          time: timeFormat,
          ph: matchingReading.ph,
          fullTime: currentDate.toLocaleString('es-ES'),
          rawDate: new Date(currentDate),
          hasData: true
        })
      } else {
        timePoints.push({
          time: timeFormat,
          ph: null,
          fullTime: currentDate.toLocaleString('es-ES'),
          rawDate: new Date(currentDate),
          hasData: false
        })
      }
      
      currentDate = new Date(currentDate.getTime() + interval)
    }
    
    return timePoints
  }

  const getTimeFormatForDate = (date: Date, filterType: string) => {
    switch (filterType) {
      case 'day':
        return date.toLocaleTimeString('es-ES', {
          hour: '2-digit',
          minute: '2-digit'
        })
      case 'week':
        return date.toLocaleDateString('es-ES', {
          weekday: 'short',
          day: '2-digit'
        })
      case 'month':
        return date.toLocaleDateString('es-ES', {
          day: '2-digit',
          month: '2-digit'
        })
      default:
        return date.toLocaleTimeString('es-ES', {
          hour: '2-digit',
          minute: '2-digit'
        })
    }
  }

  const chartData = (() => {
    if (filterType === 'day' || filterType === 'week' || filterType === 'month') {
      const completeRange = generateCompleteTimeRange(data, filterType)
      console.log('📊 [GRÁFICO COMPLETO] Rango generado:', {
        filterType,
        originalDataPoints: data.length,
        completeRangePoints: completeRange.length,
        pointsWithData: completeRange.filter(p => p.hasData).length,
        pointsWithoutData: completeRange.filter(p => !p.hasData).length,
        firstPoint: completeRange[0]?.time,
        lastPoint: completeRange[completeRange.length - 1]?.time
      })
      return completeRange
    } else {
      return data.map(reading => ({
        time: getTimeFormat(reading),
        ph: reading.ph,
        fullTime: new Date(reading.created_at).toLocaleString('es-ES'),
        rawDate: new Date(reading.created_at),
        hasData: true
      }))
    }
  })()

  // Calcular intervalo de ticks según la cantidad de datos y el tipo de filtro
  const getTickInterval = () => {
    const dataLength = chartData.length
    if (dataLength <= 10) return 0 // Mostrar todos
    if (dataLength <= 50) return Math.ceil(dataLength / 10)
    if (dataLength <= 100) return Math.ceil(dataLength / 8)
    return Math.ceil(dataLength / 6)
  }

  const CustomTooltip = ({ active, payload, label }: any) => {
    if (active && payload && payload.length) {
      const data = payload[0].payload
      return (
        <div className="bg-white p-3 border border-gray-300 rounded-lg shadow-lg">
          <p className="text-sm text-gray-600">{`Hora: ${data.fullTime}`}</p>
          {data.hasData ? (
            <p className="text-sm font-semibold text-blue-600">
              {`pH: ${payload[0].value.toFixed(2)}`}
            </p>
          ) : (
            <p className="text-sm font-medium text-gray-500">
              Sin datos disponibles
            </p>
          )}
        </div>
      )
    }
    return null
  }

  return (
    <div className="w-full h-80">
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={chartData}>
          <CartesianGrid strokeDasharray="3 3" stroke="#f0f0f0" />
          <XAxis 
            dataKey="time" 
            stroke="#666"
            fontSize={12}
            interval={getTickInterval()}
            angle={filterType === 'week' || filterType === 'month' ? -45 : 0}
            textAnchor={filterType === 'week' || filterType === 'month' ? 'end' : 'middle'}
            height={filterType === 'week' || filterType === 'month' ? 60 : 40}
          />
          <YAxis 
            domain={[0, 14]}
            stroke="#666"
            fontSize={12}
          />
          <Tooltip content={<CustomTooltip />} />
          <Line 
            type="monotone" 
            dataKey="ph" 
            stroke="#3b82f6" 
            strokeWidth={2}
            dot={(props) => {
              const { payload } = props
              if (!payload?.hasData) {
                return <circle cx={props.cx} cy={props.cy} r={3} fill="#e5e7eb" stroke="#9ca3af" strokeWidth={1} />
              }
              return <circle cx={props.cx} cy={props.cy} r={4} fill="#3b82f6" stroke="#3b82f6" strokeWidth={2} />
            }}
            activeDot={{ r: 6, fill: '#1d4ed8' }}
            connectNulls={false}
          />
          {/* Líneas de referencia para niveles óptimos */}
          <Line 
            type="monotone" 
            dataKey={() => 6.5} 
            stroke="#22c55e" 
            strokeDasharray="5 5"
            strokeWidth={1}
            dot={false}
          />
          <Line 
            type="monotone" 
            dataKey={() => 8.5} 
            stroke="#22c55e" 
            strokeDasharray="5 5"
            strokeWidth={1}
            dot={false}
          />
        </LineChart>
      </ResponsiveContainer>
      <div className="mt-2 flex justify-between items-center text-xs text-gray-500">
        <div className="flex items-center space-x-4">
          <div className="flex items-center">
            <span className="inline-block w-3 h-0.5 bg-green-500 mr-1"></span>
            Rango óptimo: 6.5 - 8.5 pH
          </div>
          <div className="flex items-center">
            <span className="inline-block w-2 h-2 bg-blue-500 rounded-full mr-1"></span>
            Con datos
          </div>
          <div className="flex items-center">
            <span className="inline-block w-2 h-2 bg-gray-300 border border-gray-400 rounded-full mr-1"></span>
            Sin datos
          </div>
        </div>
        {(filterType === 'day' || filterType === 'week' || filterType === 'month') && (
          <div className="text-right">
            <span className="text-gray-600">
              {filterType === 'day' && 'Últimas 24 horas'}
              {filterType === 'week' && 'Últimos 7 días'}
              {filterType === 'month' && 'Últimos 30 días'}
            </span>
          </div>
        )}
      </div>
    </div>
  )
}
