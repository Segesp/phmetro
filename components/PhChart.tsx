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
        return date.toLocaleTimeString('es-PE', {
          hour: '2-digit',
          minute: '2-digit',
          timeZone: 'America/Lima'
        })
      case 'week':
        // Para 7 días: mostrar día y fecha
        return date.toLocaleDateString('es-PE', {
          weekday: 'short',
          day: '2-digit',
          timeZone: 'America/Lima'
        })
      case 'month':
        // Para 30 días: mostrar día/mes
        return date.toLocaleDateString('es-PE', {
          day: '2-digit',
          month: '2-digit',
          timeZone: 'America/Lima'
        })
      case 'dayOfWeek':
        // Para día específico: mostrar fecha y hora
        return date.toLocaleDateString('es-PE', {
          day: '2-digit',
          month: '2-digit',
          hour: '2-digit',
          minute: '2-digit',
          timeZone: 'America/Lima'
        })
      case 'monthOfYear':
        // Para mes específico: mostrar día
        return date.toLocaleDateString('es-PE', {
          day: '2-digit',
          timeZone: 'America/Lima'
        })
      default:
        // Por defecto: hora:minuto
        return date.toLocaleTimeString('es-PE', {
          hour: '2-digit',
          minute: '2-digit',
          timeZone: 'America/Lima'
        })
    }
  }

  const getTimeFormatForDate = (date: Date, filterType: string) => {
    switch (filterType) {
      case 'day':
        // Para 24 horas: mostrar hora:minuto (ej: "14:30")
        return date.toLocaleTimeString('es-PE', {
          hour: '2-digit',
          minute: '2-digit',
          timeZone: 'America/Lima'
        })
      case 'week':
        // Para 7 días: mostrar día abreviado y fecha (ej: "Lun 16")
        return date.toLocaleDateString('es-PE', {
          weekday: 'short',
          day: '2-digit',
          timeZone: 'America/Lima'
        })
      case 'month':
        // Para 30 días: mostrar día/mes (ej: "16/06")
        return date.toLocaleDateString('es-PE', {
          day: '2-digit',
          month: '2-digit',
          timeZone: 'America/Lima'
        })
      default:
        return date.toLocaleTimeString('es-PE', {
          hour: '2-digit',
          minute: '2-digit',
          timeZone: 'America/Lima'
        })
    }
  }

  // Preparar datos del gráfico - usar datos exactos tal como están

  const chartData = (() => {
    // Siempre usar los datos tal como están, sin interpolación ni promediado
    return data.map(reading => ({
      time: getTimeFormat(reading),
      ph: reading.ph,
      fullTime: new Date(reading.created_at).toLocaleString('es-PE', {
        timeZone: 'America/Lima',
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit'
      }),
      rawDate: new Date(reading.created_at),
      hasData: true
    }))
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
          <p className="text-sm font-semibold text-blue-600">
            {`pH: ${payload[0].value.toFixed(2)}`}
          </p>
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
            dot={false}
            activeDot={{ r: 4, fill: '#1d4ed8' }}
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
            <span className="inline-block w-3 h-0.5 bg-blue-500 mr-1"></span>
            Datos de pH ({data.length} puntos)
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
