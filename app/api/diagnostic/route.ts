import { NextRequest, NextResponse } from 'next/server'

export async function GET(request: NextRequest) {
  try {
    const diagnosticData = {
      timestamp: new Date().toISOString(),
      environment: process.env.NODE_ENV,
      supabase: {
        url: process.env.NEXT_PUBLIC_SUPABASE_URL,
        hasKey: !!process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY,
        keyPrefix: process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY?.substring(0, 20) || 'NOT_FOUND'
      },
      vercel: {
        region: process.env.VERCEL_REGION || 'NOT_VERCEL',
        deployment: process.env.VERCEL_URL || 'LOCAL'
      },
      headers: {
        host: request.headers.get('host'),
        userAgent: request.headers.get('user-agent')?.substring(0, 50)
      }
    }

    // Test básico de Supabase
    let supabaseTest = 'NOT_TESTED'
    try {
      if (process.env.NEXT_PUBLIC_SUPABASE_URL && process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY) {
        const { createClient } = require('@supabase/supabase-js')
        const supabase = createClient(
          process.env.NEXT_PUBLIC_SUPABASE_URL,
          process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY
        )
        
        const { data, error } = await supabase
          .from('ph_readings')
          .select('count')
          .limit(1)
          .single()
        
        supabaseTest = error ? `ERROR: ${error.message}` : 'CONNECTION_OK'
      }
    } catch (err) {
      supabaseTest = `EXCEPTION: ${(err as Error).message}`
    }

    return NextResponse.json({
      ...diagnosticData,
      supabaseTest,
      status: 'OK'
    })

  } catch (error) {
    return NextResponse.json({
      error: (error as Error).message,
      status: 'ERROR',
      timestamp: new Date().toISOString()
    }, { status: 500 })
  }
}
