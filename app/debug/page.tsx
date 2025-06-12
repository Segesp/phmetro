'use client'

import { useState, useEffect } from 'react'
import { supabase } from '@/lib/supabase'

export default function DebugPage() {
  const [debugInfo, setDebugInfo] = useState<any>({})
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    const runDiagnostic = async () => {
      const debug: any = {
        timestamp: new Date().toISOString(),
        env: {
          supabaseUrl: process.env.NEXT_PUBLIC_SUPABASE_URL,
          hasAnonKey: !!process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY,
          anonKeyPrefix: process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY?.substring(0, 20) + '...'
        },
        supabaseClient: {
          exists: !!supabase,
          type: typeof supabase
        }
      }

      try {
        // Test 1: Conectividad básica
        console.log('🔧 Test 1: Conectividad básica')
        const { data: testData, error: testError } = await supabase
          .from('ph_readings')
          .select('count')
          .single()

        debug.connectivityTest = {
          success: !testError,
          error: testError?.message || null
        }

        // Test 2: Obtener datos
        console.log('🔧 Test 2: Obtener datos')
        const { data, error } = await supabase
          .from('ph_readings')
          .select('*')
          .order('created_at', { ascending: false })
          .limit(5)

        debug.dataTest = {
          success: !error,
          error: error?.message || null,
          recordCount: data?.length || 0,
          firstRecord: data?.[0] || null,
          allRecords: data || []
        }

        // Test 3: API local
        console.log('🔧 Test 3: API local')
        try {
          const apiResponse = await fetch('/api/ph-data')
          const apiData = await apiResponse.json()
          
          debug.apiTest = {
            success: apiResponse.ok,
            status: apiResponse.status,
            dataLength: apiData?.data?.length || 0,
            firstRecord: apiData?.data?.[0] || null
          }
        } catch (apiError) {
          debug.apiTest = {
            success: false,
            error: (apiError as Error).message
          }
        }

      } catch (generalError) {
        debug.generalError = (generalError as Error).message
      }

      setDebugInfo(debug)
      setLoading(false)
    }

    runDiagnostic()
  }, [])

  if (loading) {
    return (
      <div className="container mx-auto px-4 py-8">
        <h1 className="text-2xl font-bold mb-4">🔧 Diagnóstico del Sistema</h1>
        <div className="bg-blue-100 border border-blue-400 text-blue-700 px-4 py-3 rounded">
          Ejecutando diagnósticos...
        </div>
      </div>
    )
  }

  return (
    <div className="container mx-auto px-4 py-8 max-w-4xl">
      <h1 className="text-2xl font-bold mb-6">🔧 Diagnóstico del Sistema pH</h1>
      
      <div className="space-y-6">
        {/* Información de entorno */}
        <div className="bg-white border border-gray-200 rounded-lg p-6">
          <h2 className="text-lg font-semibold mb-4 text-gray-900">📋 Variables de Entorno</h2>
          <div className="grid md:grid-cols-2 gap-4 text-sm">
            <div>
              <span className="font-medium">Supabase URL:</span>
              <div className="bg-gray-100 p-2 rounded mt-1 font-mono text-xs">
                {debugInfo.env?.supabaseUrl || 'NO CONFIGURADA'}
              </div>
            </div>
            <div>
              <span className="font-medium">Anon Key:</span>
              <div className="bg-gray-100 p-2 rounded mt-1 font-mono text-xs">
                {debugInfo.env?.hasAnonKey ? 
                  `✅ ${debugInfo.env.anonKeyPrefix}` : 
                  '❌ NO CONFIGURADA'
                }
              </div>
            </div>
          </div>
        </div>

        {/* Test de conectividad */}
        <div className="bg-white border border-gray-200 rounded-lg p-6">
          <h2 className="text-lg font-semibold mb-4 text-gray-900">🔌 Test de Conectividad</h2>
          <div className={`p-4 rounded ${debugInfo.connectivityTest?.success ? 'bg-green-100 border border-green-400 text-green-700' : 'bg-red-100 border border-red-400 text-red-700'}`}>
            {debugInfo.connectivityTest?.success ? (
              <div className="flex items-center">
                <span className="text-green-600 mr-2">✅</span>
                Conexión a Supabase exitosa
              </div>
            ) : (
              <div>
                <div className="flex items-center mb-2">
                  <span className="text-red-600 mr-2">❌</span>
                  Error de conexión a Supabase
                </div>
                <div className="text-sm bg-red-200 p-2 rounded">
                  {debugInfo.connectivityTest?.error}
                </div>
              </div>
            )}
          </div>
        </div>

        {/* Test de datos */}
        <div className="bg-white border border-gray-200 rounded-lg p-6">
          <h2 className="text-lg font-semibold mb-4 text-gray-900">📊 Test de Datos</h2>
          <div className={`p-4 rounded ${debugInfo.dataTest?.success ? 'bg-green-100 border border-green-400 text-green-700' : 'bg-red-100 border border-red-400 text-red-700'}`}>
            {debugInfo.dataTest?.success ? (
              <div>
                <div className="flex items-center mb-2">
                  <span className="text-green-600 mr-2">✅</span>
                  Datos obtenidos exitosamente
                </div>
                <div className="text-sm">
                  <p><strong>Registros encontrados:</strong> {debugInfo.dataTest.recordCount}</p>
                  {debugInfo.dataTest.firstRecord && (
                    <div className="mt-2">
                      <p><strong>Último registro:</strong></p>
                      <div className="bg-green-200 p-2 rounded mt-1 font-mono text-xs">
                        pH: {debugInfo.dataTest.firstRecord.ph} | 
                        Timestamp: {debugInfo.dataTest.firstRecord.created_at}
                      </div>
                    </div>
                  )}
                </div>
              </div>
            ) : (
              <div>
                <div className="flex items-center mb-2">
                  <span className="text-red-600 mr-2">❌</span>
                  Error obteniendo datos
                </div>
                <div className="text-sm bg-red-200 p-2 rounded">
                  {debugInfo.dataTest?.error}
                </div>
              </div>
            )}
          </div>
        </div>

        {/* Test de API */}
        <div className="bg-white border border-gray-200 rounded-lg p-6">
          <h2 className="text-lg font-semibold mb-4 text-gray-900">🌐 Test de API Local</h2>
          <div className={`p-4 rounded ${debugInfo.apiTest?.success ? 'bg-green-100 border border-green-400 text-green-700' : 'bg-red-100 border border-red-400 text-red-700'}`}>
            {debugInfo.apiTest?.success ? (
              <div>
                <div className="flex items-center mb-2">
                  <span className="text-green-600 mr-2">✅</span>
                  API local funcionando
                </div>
                <div className="text-sm">
                  <p><strong>Status:</strong> {debugInfo.apiTest.status}</p>
                  <p><strong>Registros devueltos:</strong> {debugInfo.apiTest.dataLength}</p>
                  {debugInfo.apiTest.firstRecord && (
                    <div className="mt-2">
                      <p><strong>Primer registro:</strong></p>
                      <div className="bg-green-200 p-2 rounded mt-1 font-mono text-xs">
                        pH: {debugInfo.apiTest.firstRecord.ph} | 
                        ID: {debugInfo.apiTest.firstRecord.id}
                      </div>
                    </div>
                  )}
                </div>
              </div>
            ) : (
              <div>
                <div className="flex items-center mb-2">
                  <span className="text-red-600 mr-2">❌</span>
                  Error en API local
                </div>
                <div className="text-sm bg-red-200 p-2 rounded">
                  {debugInfo.apiTest?.error}
                </div>
              </div>
            )}
          </div>
        </div>

        {/* Datos completos para debugging */}
        <div className="bg-white border border-gray-200 rounded-lg p-6">
          <h2 className="text-lg font-semibold mb-4 text-gray-900">🔍 Información Completa de Debug</h2>
          <div className="bg-gray-100 p-4 rounded">
            <pre className="text-xs overflow-auto">
              {JSON.stringify(debugInfo, null, 2)}
            </pre>
          </div>
        </div>

        {/* Acciones */}
        <div className="bg-white border border-gray-200 rounded-lg p-6">
          <h2 className="text-lg font-semibold mb-4 text-gray-900">🛠️ Acciones</h2>
          <div className="space-y-2">
            <button
              onClick={() => window.location.reload()}
              className="w-full md:w-auto bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded transition-colors"
            >
              🔄 Ejecutar diagnóstico nuevamente
            </button>
            <a
              href="/"
              className="block w-full md:w-auto bg-green-600 hover:bg-green-700 text-white px-4 py-2 rounded transition-colors text-center"
            >
              🏠 Ir al Dashboard Principal
            </a>
            <a
              href="/test-supabase"
              className="block w-full md:w-auto bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded transition-colors text-center"
            >
              🧪 Probar Supabase
            </a>
          </div>
        </div>
      </div>
    </div>
  )
}
