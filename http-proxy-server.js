#!/usr/bin/env node

/**
 * SERVIDOR PROXY HTTP PARA ESP8266
 * 
 * Este servidor recibe peticiones HTTP del ESP8266 
 * y las reenvía como HTTPS a Vercel
 * 
 * USO:
 * 1. npm install express axios
 * 2. node http-proxy-server.js
 * 3. Configurar ESP8266 para enviar a tu IP local puerto 8080
 */

const express = require('express');
const axios = require('axios');
const app = express();
const PORT = 8080;

// Configuración de Vercel
const VERCEL_URL = 'https://phmetro-phi.vercel.app/api/ph-proxy';

// Middleware
app.use(express.json());
app.use(express.text());

// Función para obtener IP local
function getLocalIP() {
    const { networkInterfaces } = require('os');
    const nets = networkInterfaces();
    
    for (const name of Object.keys(nets)) {
        for (const net of nets[name]) {
            if (net.family === 'IPv4' && !net.internal) {
                return net.address;
            }
        }
    }
    return 'localhost';
}

// Endpoint proxy para pH data
app.post('/api/ph-proxy', async (req, res) => {
    console.log('\n🔄 PROXY: Petición recibida del ESP8266');
    console.log('📦 Body:', req.body);
    
    try {
        // Reenviar a Vercel usando HTTPS
        console.log(`📡 Reenviando a: ${VERCEL_URL}`);
        
        const response = await axios.post(VERCEL_URL, req.body, {
            headers: {
                'Content-Type': 'application/json',
                'User-Agent': 'ESP8266-HTTP-Proxy'
            },
            timeout: 10000
        });
        
        console.log('✅ Respuesta de Vercel:', response.status);
        console.log('📄 Data:', response.data);
        
        // Devolver respuesta del servidor
        res.status(response.status).json({
            success: true,
            vercel_response: response.data,
            proxy_note: 'Data sent to Vercel successfully via HTTP proxy'
        });
        
    } catch (error) {
        console.error('❌ Error enviando a Vercel:', error.message);
        
        res.status(500).json({
            success: false,
            error: error.message,
            proxy_note: 'Failed to send to Vercel'
        });
    }
});

// Endpoint de prueba
app.get('/test', (req, res) => {
    res.json({
        message: 'Proxy HTTP funcionando',
        vercel_target: VERCEL_URL,
        local_ip: getLocalIP(),
        port: PORT
    });
});

// Endpoint raíz
app.get('/', (req, res) => {
    res.json({
        name: 'ESP8266 HTTP to HTTPS Proxy',
        version: '1.0.0',
        vercel_target: VERCEL_URL,
        endpoints: {
            proxy: '/api/ph-proxy',
            test: '/test'
        },
        usage: `Configure ESP8266 to send to: http://${getLocalIP()}:${PORT}/api/ph-proxy`
    });
});

// Iniciar servidor
app.listen(PORT, '0.0.0.0', () => {
    const localIP = getLocalIP();
    console.log('\n🚀 SERVIDOR PROXY HTTP INICIADO');
    console.log(`📍 Local: http://localhost:${PORT}`);
    console.log(`🌐 Red: http://${localIP}:${PORT}`);
    console.log(`🎯 Target: ${VERCEL_URL}`);
    console.log('\n📋 CONFIGURACIÓN PARA ESP8266:');
    console.log(`   const char* proxyIP = "${localIP}";`);
    console.log(`   const uint16_t proxyPort = ${PORT};`);
    console.log(`   const char* proxyEndpoint = "/api/ph-proxy";`);
    console.log('\n🔄 Listo para recibir datos del ESP8266...\n');
});

// Manejo de errores
process.on('uncaughtException', (error) => {
    console.error('💥 Error no capturado:', error);
});

process.on('unhandledRejection', (reason, promise) => {
    console.error('💥 Promesa rechazada:', reason);
});
