'use client'

import { useState, useEffect, useCallback } from 'react'
import { supabase, PhReading } from '@/lib/supabase'
import PhChart from '@/components/PhChart'
import StatsCards from '@/components/StatsCards'
import RecentReadings from '@/components/RecentReadings'
import { Activity, Droplets, TrendingUp, AlertTriangle, Database, RefreshCw, Wifi } from 'lucide-react'

export default function Dashboard() {
  const [readings, setReadings] = useState<PhReading[]>([])
  const [allReadings, setAllReadings] = useState<PhReading[]>([]) // Todos los datos sin filtrar
  const [thingSpeakData, setThingSpeakData] = useState<PhReading[]>([])
  const [loading, setLoading] = useState(true)
  const [currentPh, setCurrentPh] = useState<number | null>(null)
  const [error, setError] = useState<string | null>(null)
  const [lastUpdate, setLastUpdate] = useState<string>('')
  const [dataSource, setDataSource] = useState<'supabase' | 'thingspeak' | 'both'>('both')
  const [autoSyncStatus, setAutoSyncStatus] = useState<'active' | 'syncing' | 'error'>('active')
  const [lastSyncTime, setLastSyncTime] = useState<string>('')
  
  // Estados para filtros
  const [filterType, setFilterType] = useState<'all' | 'day' | 'week' | 'month' | 'dayOfWeek' | 'monthOfYear'>('all')
  const [selectedDay, setSelectedDay] = useState<string>('')
  const [selectedMonth, setSelectedMonth] = useState<string>('')

  // Función para filtrar datos
  const filterReadings = useCallback((data: PhReading[]) => {
    console.log('🔍 [FILTRO] Iniciando filtrado:', {
      filterType,
      selectedDay,
      selectedMonth,
      totalData: data.length
    })

    if (filterType === 'all') {
      console.log('✅ [FILTRO] Mostrando todos los datos:', data.length)
      return data
    }

    const now = new Date()
    console.log('📅 [FILTRO] Fecha actual:', now.toLocaleString())

    const filtered = data.filter(reading => {
      const readingDate = new Date(reading.created_at)
      
      switch (filterType) {
        case 'day':
          // Últimas 24 horas
          const last24Hours = new Date(now.getTime() - 24 * 60 * 60 * 1000)
          const dayMatch = readingDate >= last24Hours
          console.log('⏰ [FILTRO DÍA] Verificando:', {
            readingDate: readingDate.toLocaleString(),
            last24Hours: last24Hours.toLocaleString(),
            match: dayMatch
          })
          return dayMatch
          
        case 'week':
          // Últimos 7 días
          const last7Days = new Date(now.getTime() - 7 * 24 * 60 * 60 * 1000)
          const weekMatch = readingDate >= last7Days
          console.log('📅 [FILTRO SEMANA] Verificando:', {
            readingDate: readingDate.toLocaleString(),
            last7Days: last7Days.toLocaleString(),
            match: weekMatch
          })
          return weekMatch
          
        case 'month':
          // Últimos 30 días
          const last30Days = new Date(now.getTime() - 30 * 24 * 60 * 60 * 1000)
          const monthMatch = readingDate >= last30Days
          console.log('📊 [FILTRO MES] Verificando:', {
            readingDate: readingDate.toLocaleString(),
            last30Days: last30Days.toLocaleString(),
            match: monthMatch
          })
          return monthMatch
          
        case 'dayOfWeek':
          // Filtrar por día específico de la semana
          if (!selectedDay) return true
          const dayOfWeek = readingDate.getDay() // 0 = Domingo, 1 = Lunes, etc.
          const dayOfWeekMatch = dayOfWeek.toString() === selectedDay
          if (dayOfWeekMatch) console.log('✅ [FILTRO] Día de semana válido:', readingDate.toLocaleString(), 'día:', dayOfWeek)
          return dayOfWeekMatch
          
        case 'monthOfYear':
          // Filtrar por mes específico del año
          if (!selectedMonth) return true
          const readingMonth = readingDate.getMonth() // 0 = Enero, 1 = Febrero, etc.
          const monthOfYearMatch = readingMonth.toString() === selectedMonth
          if (monthOfYearMatch) console.log('✅ [FILTRO] Mes del año válido:', readingDate.toLocaleString(), 'mes:', readingMonth)
          return monthOfYearMatch
          
        default:
          return true
      }
    })
    
    // Ordenar por fecha descendente
    const sorted = filtered.sort((a, b) => new Date(b.created_at).getTime() - new Date(a.created_at).getTime())
    
    console.log('📊 [FILTRO] Resultado final:', {
      filterType,
      original: data.length,
      filtered: filtered.length,
      sorted: sorted.length
    })
    
    return sorted
  }, [filterType, selectedDay, selectedMonth])

  // Aplicar filtros cuando cambien
  useEffect(() => {
    const filtered = filterReadings(allReadings)
    console.log('🔍 [FILTROS] Aplicando filtros:', {
      filterType,
      selectedDay,
      selectedMonth,
      totalReadings: allReadings.length,
      filteredReadings: filtered.length
    })
    setReadings(filtered)
  }, [allReadings, filterReadings, filterType, selectedDay, selectedMonth])

  // Función para leer datos de ThingSpeak
  const fetchThingSpeakData = useCallback(async () => {
    console.log('📡 [THINGSPEAK] Consultando datos...')
    
    try {
      // API de ThingSpeak con tu Read API Key - TODOS los datos disponibles
      const response = await fetch(
        'https://api.thingspeak.com/channels/2988488/fields/1.json?api_key=Z6SC5MLLP0FR4PC4&results=8000',
        { 
          method: 'GET',
          mode: 'cors',
          headers: {
            'Accept': 'application/json',
            'Content-Type': 'application/json'
          }
        }
      )

      if (!response.ok) {
        throw new Error(`ThingSpeak API error: ${response.status}`)
      }

      const data = await response.json()
      console.log('📊 [THINGSPEAK] Datos recibidos:', data)

      if (data.feeds && data.feeds.length > 0) {
        // Convertir formato ThingSpeak a formato PhReading
        const thingSpeakReadings: PhReading[] = data.feeds
          .filter((feed: any) => feed.field1 !== null)
          .map((feed: any, index: number) => ({
            id: `ts_${feed.entry_id || index}`,
            ph: parseFloat(feed.field1),
            device: 'ThingSpeak',
            created_at: feed.created_at
          }))

        setThingSpeakData(thingSpeakReadings)
        console.log('✅ [THINGSPEAK] Datos procesados:', thingSpeakReadings.length)
        return thingSpeakReadings
      }
      
      return []
    } catch (err) {
      console.error('❌ [THINGSPEAK] Error:', err)
      return []
    }
  }, [])

  const fetchReadings = useCallback(async () => {
    console.log('🚀 [DASHBOARD] Iniciando consulta...')
    setLoading(true)
    setError(null)
    
    try {
      // Consultar Supabase - TODOS los datos
      const { data, error } = await supabase
        .from('ph_readings')
        .select('*')
        .order('created_at', { ascending: false })

      console.log('📊 [DASHBOARD] Datos Supabase:', {
        success: !error,
        count: data?.length || 0,
        firstRecord: data?.[0],
        message: `Cargados ${data?.length || 0} registros de Supabase (SIN LÍMITE)`
      })

      // Consultar ThingSpeak en paralelo
      const thingSpeakReadings = await fetchThingSpeakData()
      console.log('📡 [DASHBOARD] Datos ThingSpeak:', {
        count: thingSpeakReadings.length,
        message: `Cargados ${thingSpeakReadings.length} registros de ThingSpeak`
      })

      // Combinar datos según la fuente seleccionada
      let combinedReadings: PhReading[] = []
      
      if (dataSource === 'supabase' && data) {
        combinedReadings = data
      } else if (dataSource === 'thingspeak') {
        combinedReadings = thingSpeakReadings
      } else if (dataSource === 'both') {
        // Combinar ambas fuentes - TODOS los datos
        const supabaseData = data || []
        combinedReadings = [...supabaseData, ...thingSpeakReadings]
          .sort((a, b) => new Date(b.created_at).getTime() - new Date(a.created_at).getTime())
      }

      if (error && dataSource !== 'thingspeak') {
        console.error('⚠️ [SUPABASE] Error:', error.message)
      }

      if (combinedReadings.length > 0) {
        setAllReadings(combinedReadings) // Guardar todos los datos sin filtrar
        setCurrentPh(combinedReadings[0].ph)
        setLastUpdate(new Date().toLocaleString('es-PE', {
          timeZone: 'America/Lima',
          year: 'numeric',
          month: '2-digit',
          day: '2-digit',
          hour: '2-digit',
          minute: '2-digit',
          second: '2-digit'
        }))
        console.log('✅ [DASHBOARD] Datos cargados exitosamente:', {
          total: combinedReadings.length,
          supabase: dataSource === 'supabase' ? combinedReadings.length : (data?.length || 0),
          thingspeak: dataSource === 'thingspeak' ? combinedReadings.length : thingSpeakReadings.length,
          message: `TODOS LOS DATOS CARGADOS: ${combinedReadings.length} registros totales`
        })
      } else {
        setAllReadings([])
        setCurrentPh(null)
        console.log('⚠️ [DASHBOARD] No hay datos disponibles en ninguna fuente')
      }
      
    } catch (err) {
      const errorMessage = (err as Error).message
      console.error('❌ [DASHBOARD] Error:', errorMessage)
      setError(errorMessage)
      setReadings([])
      setCurrentPh(null)
    } finally {
      setLoading(false)
    }
  }, [dataSource, fetchThingSpeakData])

  // Función de auto-sincronización ThingSpeak → Supabase
  const autoSyncThingSpeak = useCallback(async () => {
    try {
      console.log('🔄 [AUTO-SYNC] Iniciando sincronización automática...')
      setAutoSyncStatus('syncing')
      
      const response = await fetch('/api/sync-thingspeak', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        }
      })
      
      if (response.ok) {
        const result = await response.json()
        console.log('✅ [AUTO-SYNC] Sincronización exitosa:', result)
        setAutoSyncStatus('active')
        setLastSyncTime(new Date().toLocaleString('es-PE', {
          timeZone: 'America/Lima',
          hour: '2-digit',
          minute: '2-digit',
          second: '2-digit'
        }))
        
        // Refrescar datos después de la sincronización
        fetchReadings()
      } else {
        console.error('❌ [AUTO-SYNC] Error en sincronización:', response.statusText)
        setAutoSyncStatus('error')
      }
    } catch (error) {
      console.error('❌ [AUTO-SYNC] Error de red:', error)
      setAutoSyncStatus('error')
    }
  }, [fetchReadings])

  useEffect(() => {
    fetchReadings()
    fetchThingSpeakData()
    
    // Suscripción en tiempo real
    const subscription = supabase
      .channel('ph_readings_realtime')
      .on('postgres_changes', 
        { event: 'INSERT', schema: 'public', table: 'ph_readings' },
        (payload: any) => {
          console.log('📡 [DASHBOARD] Nuevo dato en tiempo real:', payload.new)
          const newReading = payload.new as PhReading
          setAllReadings(prev => [newReading, ...prev]) // Agregar a todos los datos
          setCurrentPh(newReading.ph)
          setLastUpdate(new Date().toLocaleString('es-PE', {
            timeZone: 'America/Lima',
            year: 'numeric',
            month: '2-digit',
            day: '2-digit',
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit'
          }))
        }
      )
      .subscribe()

    // Auto-refresh cada 30 segundos
    const interval = setInterval(fetchReadings, 30000)
    
    // Auto-sincronización ThingSpeak → Supabase cada 2 minutos
    const syncInterval = setInterval(autoSyncThingSpeak, 120000) // 2 minutos
    
    // Ejecutar sincronización inicial después de 5 segundos
    const initialSyncTimeout = setTimeout(autoSyncThingSpeak, 5000)

    return () => {
      subscription.unsubscribe()
      clearInterval(interval)
      clearInterval(syncInterval)
      clearTimeout(initialSyncTimeout)
    }
  }, [fetchReadings, fetchThingSpeakData, autoSyncThingSpeak])

  const getPhStatus = (ph: number) => {
    // Rango crítico: fuera de 6-9
    if (ph < 6.0) {
      return { 
        status: 'CRÍTICO - pH Muy Bajo', 
        class: 'ph-critical', 
        color: 'text-red-700',
        bgColor: 'bg-red-100',
        borderColor: 'border-red-300',
        icon: '🚨'
      }
    }
    if (ph > 9.0) {
      return { 
        status: 'CRÍTICO - pH Muy Alto', 
        class: 'ph-critical', 
        color: 'text-red-700',
        bgColor: 'bg-red-100',
        borderColor: 'border-red-300',
        icon: '🚨'
      }
    }
    
    // Zona de advertencia: 6-6.5 (pH bajo)
    if (ph >= 6.0 && ph < 6.5) {
      return { 
        status: 'ADVERTENCIA - pH Bajo', 
        class: 'ph-warning-low', 
        color: 'text-orange-700',
        bgColor: 'bg-orange-100',
        borderColor: 'border-orange-300',
        icon: '⚠️'
      }
    }
    
    // Zona de advertencia: 8.5-9 (pH alto)
    if (ph > 8.5 && ph <= 9.0) {
      return { 
        status: 'ADVERTENCIA - pH Alto', 
        class: 'ph-warning-high', 
        color: 'text-orange-700',
        bgColor: 'bg-orange-100',
        borderColor: 'border-orange-300',
        icon: '⚠️'
      }
    }
    
    // Rango óptimo: 6.5-8.5
    return { 
      status: 'Óptimo', 
      class: 'ph-safe', 
      color: 'text-green-700',
      bgColor: 'bg-green-100',
      borderColor: 'border-green-300',
      icon: '✅'
    }
  }

  const getFilterDescription = () => {
    switch (filterType) {
      case 'day':
        return 'últimas 24 horas'
      case 'week':
        return 'últimos 7 días'
      case 'month':
        return 'últimos 30 días'
      case 'dayOfWeek':
        const days = ['Domingo', 'Lunes', 'Martes', 'Miércoles', 'Jueves', 'Viernes', 'Sábado']
        return selectedDay ? days[parseInt(selectedDay)] : 'día seleccionado'
      case 'monthOfYear':
        const months = ['Enero', 'Febrero', 'Marzo', 'Abril', 'Mayo', 'Junio',
                       'Julio', 'Agosto', 'Septiembre', 'Octubre', 'Noviembre', 'Diciembre']
        return selectedMonth ? months[parseInt(selectedMonth)] : 'mes seleccionado'
      default:
        return 'todos'
    }
  }

  const getDateRange = () => {
    if (readings.length === 0) return null
    
    const sortedReadings = [...readings].sort((a, b) => 
      new Date(a.created_at).getTime() - new Date(b.created_at).getTime()
    )
    
    const oldest = new Date(sortedReadings[0].created_at)
    const newest = new Date(sortedReadings[sortedReadings.length - 1].created_at)
    
    return {
      from: oldest.toLocaleDateString('es-ES'),
      to: newest.toLocaleDateString('es-ES'),
      fromTime: oldest.toLocaleString('es-ES'),
      toTime: newest.toLocaleString('es-ES')
    }
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
        <div className="flex flex-col lg:flex-row lg:items-center lg:justify-between">
          <div className="mb-4 lg:mb-0">
            <h1 className="text-3xl font-bold text-gray-900 mb-2">
              Dashboard pH Metro
            </h1>
            <p className="text-gray-600">
              Monitoreo en tiempo real de niveles de pH
            </p>
          </div>
          
          <div className="flex flex-col sm:flex-row sm:items-center sm:space-x-4 space-y-2 sm:space-y-0">
            {/* Primera fila de controles */}
            <div className="flex flex-wrap items-center gap-2">
              {/* Selector de fuente de datos */}
              <div className="flex items-center space-x-2">
                <label className="text-sm font-medium text-gray-700">Fuente:</label>
                <select
                  value={dataSource}
                  onChange={(e) => setDataSource(e.target.value as 'supabase' | 'thingspeak' | 'both')}
                  className="bg-white border border-gray-300 rounded px-3 py-1 text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
                >
                  <option value="both">📊 Ambas fuentes</option>
                  <option value="supabase">🗄️ Supabase</option>
                  <option value="thingspeak">📡 ThingSpeak</option>
                </select>
              </div>

              {/* Filtros de tiempo */}
              <div className="flex items-center space-x-2">
                <label className="text-sm font-medium text-gray-700">Filtro:</label>
                <select
                  value={filterType}
                  onChange={(e) => {
                    setFilterType(e.target.value as 'all' | 'day' | 'week' | 'month' | 'dayOfWeek' | 'monthOfYear')
                    if (e.target.value !== 'dayOfWeek') setSelectedDay('')
                    if (e.target.value !== 'monthOfYear') setSelectedMonth('')
                  }}
                  className="bg-white border border-gray-300 rounded px-3 py-1 text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
                >
                  <option value="all">📅 Todos los datos</option>
                  <option value="day">⏰ Últimas 24 horas</option>
                  <option value="week">� Últimos 7 días</option>
                  <option value="month">📊 Últimos 30 días</option>
                  <option value="dayOfWeek">� Por día de semana</option>
                  <option value="monthOfYear">�️ Por mes del año</option>
                </select>
              </div>

              {/* Selector de día de la semana */}
              {filterType === 'dayOfWeek' && (
                <div className="flex items-center space-x-2">
                  <select
                    value={selectedDay}
                    onChange={(e) => setSelectedDay(e.target.value)}
                    className="bg-white border border-gray-300 rounded px-3 py-1 text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
                  >
                    <option value="">Seleccionar día</option>
                    <option value="0">🌅 Domingo</option>
                    <option value="1">📝 Lunes</option>
                    <option value="2">💼 Martes</option>
                    <option value="3">🎯 Miércoles</option>
                    <option value="4">🚀 Jueves</option>
                    <option value="5">🎉 Viernes</option>
                    <option value="6">🌟 Sábado</option>
                  </select>
                </div>
              )}

              {/* Selector de mes */}
              {filterType === 'monthOfYear' && (
                <div className="flex items-center space-x-2">
                  <select
                    value={selectedMonth}
                    onChange={(e) => setSelectedMonth(e.target.value)}
                    className="bg-white border border-gray-300 rounded px-3 py-1 text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
                  >
                    <option value="">Seleccionar mes</option>
                    <option value="0">❄️ Enero</option>
                    <option value="1">💝 Febrero</option>
                    <option value="2">🌸 Marzo</option>
                    <option value="3">🌺 Abril</option>
                    <option value="4">🌸 Mayo</option>
                    <option value="5">☀️ Junio</option>
                    <option value="6">🏖️ Julio</option>
                    <option value="7">🌞 Agosto</option>
                    <option value="8">🍂 Septiembre</option>
                    <option value="9">🎃 Octubre</option>
                    <option value="10">🦃 Noviembre</option>
                    <option value="11">🎄 Diciembre</option>
                  </select>
                </div>
              )}
            </div>
            
            {/* Botón de actualizar */}
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
              {lastSyncTime && (
                <div className="flex items-center space-x-1 mt-1">
                  <div className={`w-2 h-2 rounded-full ${
                    autoSyncStatus === 'active' ? 'bg-green-500' :
                    autoSyncStatus === 'syncing' ? 'bg-yellow-500 animate-pulse' :
                    'bg-red-500'
                  }`}></div>
                  <p className="text-xs text-gray-500">
                    Auto-sync: {lastSyncTime}
                    {autoSyncStatus === 'syncing' && ' (sincronizando...)'}
                    {autoSyncStatus === 'error' && ' (error)'}
                  </p>
                </div>
              )}
            </div>
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
        <div className={`mb-8 p-6 rounded-lg shadow-md border-2 ${getPhStatus(currentPh).bgColor} ${getPhStatus(currentPh).borderColor}`}>
          <div className="flex items-center justify-between">
            <div className="flex items-center space-x-4">
              <div className="p-3 bg-blue-100 rounded-full">
                <Droplets className="h-8 w-8 text-blue-600" />
              </div>
              <div>
                <h2 className="text-3xl font-bold text-gray-900 flex items-center gap-2">
                  pH: {currentPh.toFixed(2)}
                  <span className="text-2xl">{getPhStatus(currentPh).icon}</span>
                </h2>
                <span className={`text-lg font-bold ${getPhStatus(currentPh).color}`}>
                  {getPhStatus(currentPh).status}
                </span>
                {/* Información adicional según el estado */}
                <div className="mt-2 text-sm">
                  {currentPh < 6.0 && (
                    <p className="text-red-700 font-medium">
                      🚨 Agua demasiado ácida. Riesgo para la salud.
                    </p>
                  )}
                  {currentPh > 9.0 && (
                    <p className="text-red-700 font-medium">
                      🚨 Agua demasiado alcalina. Riesgo para la salud.
                    </p>
                  )}
                  {currentPh >= 6.0 && currentPh < 6.5 && (
                    <p className="text-orange-700 font-medium">
                      ⚠️ pH ligeramente bajo. Monitorear de cerca.
                    </p>
                  )}
                  {currentPh > 8.5 && currentPh <= 9.0 && (
                    <p className="text-orange-700 font-medium">
                      ⚠️ pH ligeramente alto. Monitorear de cerca.
                    </p>
                  )}
                  {currentPh >= 6.5 && currentPh <= 8.5 && (
                    <p className="text-green-700 font-medium">
                      ✅ Agua en condiciones óptimas para consumo.
                    </p>
                  )}
                </div>
              </div>
            </div>
            <div className="text-right">
              <div className={`px-3 py-1 rounded-full text-sm font-medium ${getPhStatus(currentPh).bgColor} ${getPhStatus(currentPh).color} border ${getPhStatus(currentPh).borderColor}`}>
                📊 Datos en Tiempo Real
              </div>
              <p className="text-xs text-gray-500 mt-1">
                {readings.length} de {allReadings.length} lecturas 
                {filterType !== 'all' && ` (${getFilterDescription()})`}
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
                <div className="flex items-center justify-between mb-4">
                  <h3 className="text-lg font-semibold text-gray-900 flex items-center">
                    <TrendingUp className="h-5 w-5 mr-2" />
                    Gráfico de pH en Tiempo Real
                    <span className="ml-2 bg-green-100 text-green-800 text-xs px-2 py-1 rounded">
                      En Vivo
                    </span>
                  </h3>
                  
                  {/* Indicador de filtro activo */}
                  {filterType !== 'all' && (
                    <div className="text-right">
                      <div className="bg-blue-100 text-blue-800 text-xs px-2 py-1 rounded">
                        📊 {getFilterDescription()}
                      </div>
                      {getDateRange() && (
                        <p className="text-xs text-gray-500 mt-1">
                          {getDateRange()?.from === getDateRange()?.to ? 
                            getDateRange()?.from : 
                            `${getDateRange()?.from} - ${getDateRange()?.to}`
                          }
                        </p>
                      )}
                    </div>
                  )}
                </div>
                
                <PhChart 
                  data={(() => {
                    const chartData = readings.slice().reverse()
                    console.log('📊 [GRÁFICO] Datos para el gráfico:', {
                      filterType,
                      totalFilteredReadings: readings.length,
                      chartDataLength: chartData.length,
                      firstDate: chartData[0]?.created_at,
                      lastDate: chartData[chartData.length - 1]?.created_at
                    })
                    return chartData
                  })()} 
                  filterType={filterType}
                />
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
                Mostrando {readings.length} de {allReadings.length} lecturas de pH
                {filterType !== 'all' && ` filtradas por ${getFilterDescription()}`}
              </p>
              <p className="text-xs text-green-600 mt-1">
                Última lectura: {new Date(readings[0]?.created_at).toLocaleString('es-PE', {
                  timeZone: 'America/Lima',
                  year: 'numeric',
                  month: '2-digit',
                  day: '2-digit',
                  hour: '2-digit',
                  minute: '2-digit',
                  second: '2-digit'
                })} (Hora de Perú)
              </p>
            </div>
          </div>
        </>
      )}
    </div>
  )
}
