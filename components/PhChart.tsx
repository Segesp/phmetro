'use client'

import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts'
import { PhReading } from '@/lib/supabase'

interface PhChartProps {
  data: PhReading[]
}

export default function PhChart({ data }: PhChartProps) {
  const chartData = data.map(reading => ({
    time: new Date(reading.created_at).toLocaleTimeString('es-ES', {
      hour: '2-digit',
      minute: '2-digit'
    }),
    ph: reading.ph,
    fullTime: new Date(reading.created_at).toLocaleString('es-ES')
  }))

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
