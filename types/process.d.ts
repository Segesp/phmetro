// Tipos para process.env
declare global {
  var process: {
    env: {
      NEXT_PUBLIC_SUPABASE_URL?: string
      NEXT_PUBLIC_SUPABASE_ANON_KEY?: string
      SUPABASE_SERVICE_ROLE_KEY?: string
      NODE_ENV?: string
      [key: string]: string | undefined
    }
  }
}

export {}
