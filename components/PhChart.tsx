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

  const chartData = data.map(reading => ({
    time: getTimeFormat(reading),
    ph: reading.ph,
    fullTime: new Date(reading.created_at).toLocaleString('es-ES'),
    rawDate: new Date(reading.created_at)
  }))

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
            dot={{ fill: '#3b82f6', strokeWidth: 2, r: 4 }}
            activeDot={{ r: 6, fill: '#1d4ed8' }}
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
      <div className="mt-2 text-xs text-gray-500 text-center">
        <span className="inline-block w-3 h-0.5 bg-green-500 mr-1"></span>
        Rango óptimo: 6.5 - 8.5 pH
      </div>
    </div>
  )
}
