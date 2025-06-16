/**
 * CLOUDFLARE WORKER - PROXY HTTP A HTTPS PARA ESP8266
 * 
 * Este Worker recibe peticiones HTTP del ESP8266 y las reenvía 
 * como HTTPS a tu endpoint de Vercel
 * 
 * DEPLOYMENT:
 * 1. Crear cuenta en Cloudflare
 * 2. Ir a Workers & Pages
 * 3. Crear nuevo Worker
 * 4. Pegar este código
 * 5. Deploy
 * 6. Obtener URL del Worker (ej: esp8266-proxy.tu-usuario.workers.dev)
 */

// Configuración del endpoint de Vercel
const VERCEL_ENDPOINT = 'https://phmetro-phi.vercel.app/api/emergency-ph'; // TEMPORAL: usando endpoint de emergencia
const BACKUP_ENDPOINT = 'https://phmetro-phi.vercel.app/api/ph-proxy';

// Headers CORS para permitir peticiones desde cualquier origen
const CORS_HEADERS = {
  'Access-Control-Allow-Origin': '*',
  'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
  'Access-Control-Allow-Headers': 'Content-Type',
};

export default {
  async fetch(request, env, ctx) {
    const url = new URL(request.url);
    
    // Manejar preflight CORS
    if (request.method === 'OPTIONS') {
      return new Response(null, {
        status: 200,
        headers: CORS_HEADERS,
      });
    }

    // Endpoint principal para recibir datos del ESP8266
    if (url.pathname === '/api/ph-proxy' && request.method === 'POST') {
      return handlePhData(request);
    }

    // Endpoint de información
    if (url.pathname === '/' || url.pathname === '/info') {
      return new Response(JSON.stringify({
        name: 'ESP8266 HTTP to HTTPS Proxy',
        version: '1.0.0',
        cloudflare_worker: true,
        vercel_target: VERCEL_ENDPOINT,
        endpoints: {
          proxy: '/api/ph-proxy',
          info: '/info'
        },
        usage: {
          arduino_config: {
            domain: url.hostname,
            port: 80,
            endpoint: '/api/ph-proxy',
            ssl: false
          }
        }
      }, null, 2), {
        status: 200,
        headers: {
          'Content-Type': 'application/json',
          ...CORS_HEADERS,
        },
      });
    }

    // 404 para otras rutas
    return new Response('Not Found', { 
      status: 404,
      headers: CORS_HEADERS,
    });
  },
};

// Función para manejar datos de pH del ESP8266
async function handlePhData(request) {
  try {
    console.log('🔄 PROXY: Petición recibida del ESP8266');
    
    // Leer datos del ESP8266
    const body = await request.json();
    console.log('📦 Datos recibidos:', body);
    
    // Validar datos básicos
    if (!body.ph || isNaN(parseFloat(body.ph))) {
      return new Response(JSON.stringify({
        success: false,
        error: 'pH value is required and must be a number',
        proxy_note: 'Validation failed in Cloudflare Worker'
      }), {
        status: 400,
        headers: {
          'Content-Type': 'application/json',
          ...CORS_HEADERS,
        },
      });
    }

    // Reenviar a Vercel usando HTTPS
    console.log(`📡 Reenviando a Vercel: ${VERCEL_ENDPOINT}`);
    
    const vercelResponse = await fetch(VERCEL_ENDPOINT, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'User-Agent': 'ESP8266-Cloudflare-Proxy',
      },
      body: JSON.stringify(body),
    });

    const vercelData = await vercelResponse.json();
    
    console.log('✅ Respuesta de Vercel:', vercelResponse.status);
    console.log('📄 Data:', vercelData);

    // Devolver respuesta combinada
    return new Response(JSON.stringify({
      success: true,
      proxy: 'cloudflare-worker',
      esp8266_data: body,
      vercel_response: vercelData,
      vercel_status: vercelResponse.status,
      timestamp: new Date().toISOString(),
      note: 'Data successfully proxied to Vercel via Cloudflare Worker'
    }), {
      status: vercelResponse.status,
      headers: {
        'Content-Type': 'application/json',
        ...CORS_HEADERS,
      },
    });

  } catch (error) {
    console.error('❌ Error en proxy:', error.message);
    
    return new Response(JSON.stringify({
      success: false,
      error: error.message,
      proxy: 'cloudflare-worker',
      note: 'Failed to proxy data to Vercel'
    }), {
      status: 500,
      headers: {
        'Content-Type': 'application/json',
        ...CORS_HEADERS,
      },
    });
  }
}
