import { createClient } from '@supabase/supabase-js'

// Configuración de Supabase con valores de fallback
const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL || 'https://japrhhtnijidlecqrkpk.supabase.co'
const supabaseAnonKey = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY || 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImphcHJoaHRuaWppZGxlY3Fya3BrIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDYzMTA1MDIsImV4cCI6MjA2MTg4NjUwMn0.61YFYjP1tJ3P1nCwLcmMrq3a8QnFC8E0xburAoTQaOM'

// Debug: Log de configuración (solo en desarrollo)
if (typeof window !== 'undefined' && process.env.NODE_ENV === 'development') {
  console.log('🔧 Supabase Config:', {
    url: supabaseUrl,
    hasKey: !!supabaseAnonKey,
    keyPrefix: supabaseAnonKey?.substring(0, 20) + '...'
  })
}

export const supabase = createClient(supabaseUrl, supabaseAnonKey)

// Tipo para las lecturas de pH
export interface PhReading {
  id: string
  ph: number
  timestamp: string
  created_at: string
}
