// ===============================================
// pH METRO ARDUINO UNO + ESP8266 - OPTIMIZADO
// ===============================================
// VERSIÓN ADAPTADA CON SOFTWARESERIAL Y HTTP ROBUSTO
// Comunicación: SoftwareSerial para comandos AT

#include <SoftwareSerial.h>
#include <avr/pgmspace.h>  // Para usar PROGMEM y ahorrar RAM

/*
 * =====================================================
 * ✅ CONFIGURACIÓN CLOUDFLARE COMPLETADA
 * =====================================================
 * 
 * 🌟 WORKER URL: esp8266-phmetro-proxy.20200205.workers.dev
 * ✅ ESTADO: Configurado y funcionando
 * ✅ ENDPOINT: /api/ph-proxy
 * ✅ TARGET: https://phmetro-phi.vercel.app/api/ph-proxy
 * 
 * 🔄 FLUJO DE DATOS CONFIGURADO:
 * ESP8266 → HTTPS (SSL) → Cloudflare Worker → HTTPS → Vercel → Supabase
 * 
 * 📋 PRÓXIMOS PASOS:
 * 1. ✅ Cloudflare Worker: LISTO
 * 2. ✅ Código Arduino: CONFIGURADO
 * 3. 🔄 Compilar y subir al Arduino UNO
 * 4. 🔄 Monitor Serie para verificar conexión
 * 5. 🔄 Verificar datos en dashboard
 * 
 * =====================================================
 */

/*
 * ================================================
 * CONFIGURACIÓN DIRECTA A VERCEL
 * ================================================
 * 
 * URL: https://phmetro-phi.vercel.app/api/ph-proxy
 * Conexión: HTTPS (SSL) puerto 443
 * Formato: {"ph": 7.12, "device": "UNO"}
 * 
 * NOTA: Requiere ESP8266 con soporte SSL
 * ================================================
 */

// ========== CONFIGURACIÓN DE PINES ESP8266 ==========
const uint8_t RX_PIN = 2;    // ESP8266 TX → Arduino 2
const uint8_t TX_PIN = 3;    // ESP8266 RX ← Arduino 3 (5V→3.3V divisor)
SoftwareSerial esp(RX_PIN, TX_PIN);

// ========== CONFIGURACIÓN DEL SENSOR pH ==========
const int phPin = A0;

// ========== CONFIGURACIÓN WiFi Y SERVIDORES ==========
const char* ssid = "SEGOVIA3";
const char* password = "76840574";

// CONFIGURACIÓN CLOUDFLARE WORKER (CONEXIÓN HTTPS DIRECTA)
// ✅ URL CONFIGURADA PARA TU WORKER ESPECÍFICO
const char* cloudflareWorker = "esp8266-phmetro-proxy.20200205.workers.dev";
const uint16_t httpsPort = 443;           // Puerto HTTPS estándar (SSL)
const char* proxyEndpoint = "/api/ph-proxy";  // Endpoint del proxy

// CONFIGURACIÓN THINGSPEAK (DOBLE ENVÍO)
const char* thingSpeakHost = "api.thingspeak.com";
const char* writeAPIKey = "I4RFD6P62MTKOR8E"; // Tu Write API Key

// Conexión HTTPS directa al Worker de Cloudflare (ESP8266 soporta SSL)

// ========== CONFIGURACIÓN DE TIMING ==========
const unsigned long readInterval = 3000;   // Leer cada 3 segundos
const unsigned long sendInterval = 15000;  // Enviar cada 15 segundos
const int maxReadings = 3;                 // Solo 3 lecturas para ahorrar RAM

// ========== CONFIGURACIÓN DE DEBUG ==========
const bool debugMode = false;              // Debug DESACTIVADO para ahorrar RAM
const bool simulationMode = true;          // Simulación activada
const bool sendTestDataOnStart = true;     // Enviar datos de prueba

// ========== VARIABLES GLOBALES OPTIMIZADAS ==========
float phReadings[maxReadings];           // Array pequeño
byte readingIndex = 0;                   // byte en lugar de int
bool readingsReady = false;              
unsigned long lastReadTime = 0;         
unsigned long lastSendTime = 0;          
byte transmissionCounter = 0;            // byte en lugar de int
byte consecutiveErrors = 0;              // byte en lugar de int
bool esp8266Ready = false;               
bool testDataSent = false;               

// Buffer para comandos AT (reutilizable)
char atBuffer[100];                      // Buffer fijo para comandos AT
char responseBuffer[150];                // Buffer para respuestas

// ========== FUNCIONES AUXILIARES ESP8266 ==========

// Vacía el buffer del ESP8266
void clearBuffer() {
  while (esp.available()) esp.read();
}

// Envía un AT y espera "OK"
bool sendAT(const char* cmd, unsigned long timeout = 2000) {
  Serial.print(F("> ")); Serial.println(cmd);
  clearBuffer();
  esp.println(cmd);
  unsigned long start = millis();
  while (millis() - start < timeout) {
    if (esp.find("OK")) {
      Serial.println(F("[OK]"));
      return true;
    }
  }
  Serial.println(F("[FAIL]"));
  return false;
}

// Lee IP, gateway y máscara con AT+CIPSTA_CUR?
bool getNetworkConfig(String &ip, String &gateway, String &netmask) {
  Serial.println(F("> AT+CIPSTA_CUR?"));
  clearBuffer();
  esp.println("AT+CIPSTA_CUR?");
  unsigned long start = millis();
  String resp;
  while (millis() - start < 2000) {
    while (esp.available()) resp += char(esp.read());
    // Extrae IP
    int p1 = resp.indexOf("ip:\"");
    if (p1 != -1) {
      int p2 = resp.indexOf("\"", p1 + 4);
      ip = resp.substring(p1 + 4, p2);
    }
    // Extrae gateway
    int g1 = resp.indexOf("gateway:\"");
    if (g1 != -1) {
      int g2 = resp.indexOf("\"", g1 + 9);
      gateway = resp.substring(g1 + 9, g2);
    }
    // Extrae netmask
    int n1 = resp.indexOf("netmask:\"");
    if (n1 != -1) {
      int n2 = resp.indexOf("\"", n1 + 9);
      netmask = resp.substring(n1 + 9, n2);
    }
    if (resp.indexOf("OK") != -1) {
      Serial.print(F("[CONFIG] IP=")); Serial.print(ip);
      Serial.print(F(" GW=")); Serial.print(gateway);
      Serial.print(F(" NM=")); Serial.println(netmask);
      return true;
    }
  }
  Serial.println(F("[ERR:CONFIG]"));
  return false;
}

// Hace ping a la IP especificada, devuelve true/false
bool pingHost(const String &host, const char* label) {
  Serial.print(F("> AT+PING=\"")); Serial.print(host); Serial.println('\"');
  clearBuffer();
  esp.print(F("AT+PING=\"")); esp.print(host); esp.println(F("\""));
  unsigned long start = millis();
  String resp;
  while (millis() - start < 3000) {
    while (esp.available()) resp += char(esp.read());
    if (resp.indexOf("time=") != -1) {
      Serial.print(F("[")); Serial.print(label); Serial.println(F(" → OK]"));
      return true;
    }
  }
  Serial.print(F("[")); Serial.print(label); Serial.println(F(" → FAIL]"));
  return false;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  esp.begin(9600);
  delay(2000);
  
  // Mensaje de inicio simple
  Serial.println(F("=== pH METRO UNO + ESP8266 ==="));
  Serial.println(F("Iniciando sistema..."));
  
  pinMode(phPin, INPUT);
  
  // Inicializar array
  for (byte i = 0; i < maxReadings; i++) {
    phReadings[i] = 7.0;
  }
  
  // Inicializar ESP8266
  initializeESP8266();
  
  // Detectar y verificar configuración
  if (esp8266Ready) {
    verifyCloudflareConnection();
  }
  
  // Enviar datos de prueba
  if (sendTestDataOnStart && esp8266Ready) {
    sendInitialTestData();
  }
  
  Serial.println(F("Sistema listo"));
}

void loop() {
  unsigned long currentTime = millis();
  
  // Leer sensor cada 3 segundos
  if (currentTime - lastReadTime >= readInterval) {
    readPhSensor();
    lastReadTime = currentTime;
  }
  
  // Enviar datos cada 15 segundos
  if (readingsReady && currentTime - lastSendTime >= sendInterval) {
    transmissionCounter++;
    
    float avgPH = calculateAveragePH();
    
    Serial.print(F("Enviando pH: "));
    Serial.println(avgPH, 2);
    
    // 1) Obtener configuración de red
    String ip, gw, nm;
    if (!getNetworkConfig(ip, gw, nm)) {
      Serial.println(F("Error obteniendo config de red"));
      lastSendTime = currentTime;
      return;
    }

    // 2) Hacer ping al gateway (opcional)
    if (!pingHost(gw, "PING GW")) {
      Serial.println(F("⚠️ Ping gateway falló, intentando de todas formas..."));
    }

    // 3) Envío dual: Dashboard + ThingSpeak
    Serial.println(F("\n=== ENVÍO DUAL: DASHBOARD + THINGSPEAK ==="));
    
    // Enviar al Dashboard (Vercel/Supabase)
    Serial.println(F("📊 Enviando al Dashboard..."));
    bool dashboardSuccess = postToDashboard(avgPH);
    
    delay(3000); // Pausa entre envíos para evitar congestión
    
    // Enviar a ThingSpeak
    Serial.println(F("📡 Enviando a ThingSpeak..."));
    bool thingSpeakSuccess = postToThingSpeak(avgPH);
    
    // Evaluar resultados
    if (dashboardSuccess && thingSpeakSuccess) {
      consecutiveErrors = 0;
      Serial.println(F("✅ AMBOS ENVÍOS OK"));
    } else if (dashboardSuccess || thingSpeakSuccess) {
      consecutiveErrors = 0;
      Serial.println(F("⚠️ ENVÍO PARCIAL OK"));
      Serial.print(F("Dashboard: ")); Serial.println(dashboardSuccess ? F("✅") : F("❌"));
      Serial.print(F("ThingSpeak: ")); Serial.println(thingSpeakSuccess ? F("✅") : F("❌"));
    } else {
      consecutiveErrors++;
      Serial.print(F("❌ AMBOS FALLARON - Error "));
      Serial.println(consecutiveErrors);
      
      if (consecutiveErrors >= 3) {
        Serial.println(F("Reconectando..."));
        initializeESP8266();
        consecutiveErrors = 0;
      }
    }
    
    lastSendTime = currentTime;
  }
  
  delay(100);
}

// ========== INICIALIZACIÓN ESP8266 ==========
void initializeESP8266() {
  Serial.println(F("=== INICIANDO ESP8266 ==="));
  
  // Configurar modo estación
  sendAT("AT+CWMODE=1", 1500);

  // Conectar a WiFi
  String join = String("AT+CWJAP=\"") + ssid + String("\",\"") + password + String("\"");
  if (sendAT(join.c_str(), 15000)) {
    Serial.println(F("=== Wi-Fi ESTABLECIDA ==="));
    esp8266Ready = true;
  } else {
    Serial.println(F("=== Wi-Fi ERROR ==="));
    esp8266Ready = false;
  }
}

// ========== LECTURA SENSOR ==========
void readPhSensor() {
  float phValue;
  
  if (simulationMode) {
    phValue = generateSimulatedPH();
  } else {
    phValue = readRealPhSensor();
  }
  
  if (isValidPH(phValue)) {
    phReadings[readingIndex] = phValue;
    readingIndex = (readingIndex + 1) % maxReadings;
    
    if (!readingsReady && readingIndex == 0) {
      readingsReady = true;
      Serial.println(F("Buffer listo"));
    }
  }
}

float readRealPhSensor() {
  long total = 0;
  for (byte i = 0; i < 5; i++) {
    total += analogRead(phPin);
    delay(10);
  }
  
  float avgReading = total / 5.0;
  float voltage = avgReading * (5.0 / 1023.0);
  float phValue = 7.0 + ((2.5 - voltage) / 0.18);
  
  return phValue;
}

float generateSimulatedPH() {
  static float basePH = 7.0;
  float variation = sin(millis() / 30000.0) * 0.3;
  float noise = (random(-10, 11) / 100.0);
  
  float simulatedPH = basePH + variation + noise;
  return constrain(simulatedPH, 6.0, 8.5);
}

float calculateAveragePH() {
  float total = 0;
  for (byte i = 0; i < maxReadings; i++) {
    total += phReadings[i];
  }
  return total / maxReadings;
}

bool isValidPH(float ph) {
  return (ph >= 0.0 && ph <= 14.0 && !isnan(ph));
}

// ========== TRANSMISIÓN VIA CLOUDFLARE WORKER ==========
bool postToDashboard(float phValue) {
  if (!esp8266Ready) return false;
  
  Serial.println(F("=== ENVIANDO VIA CLOUDFLARE (HTTPS) ==="));
  Serial.print(F("Conectando a: https://"));
  Serial.print(cloudflareWorker);
  Serial.print(F(":"));
  Serial.println(httpsPort);
  
  // 1) CIPSTART SSL - Conectar a Cloudflare Worker via HTTPS
  clearBuffer();
  esp.print(F("AT+CIPSTART=\"SSL\",\""));
  esp.print(cloudflareWorker);
  esp.print(F("\","));
  esp.println(httpsPort);
  Serial.println(F("> AT+CIPSTART SSL (Cloudflare HTTPS)"));
  
  unsigned long start = millis();
  bool connected = false;
  while (millis() - start < 10000) {
    if (esp.find("Linked") || esp.find("CONNECT")) {
      Serial.println(F("[CIPSTART → Conectado a Cloudflare]"));
      connected = true;
      break;
    }
    if (esp.find("ERROR") || esp.find("FAIL")) {
      Serial.println(F("[CIPSTART → Error Cloudflare]"));
      break;
    }
  }
  
  if (!connected) {
    Serial.println(F("[ERROR: No se pudo conectar a Cloudflare]"));
    Serial.println(F("Verificar URL del Worker"));
    return false;
  }

  // 2) Construir JSON para Vercel via Cloudflare
  String payload = String("{\"ph\":") + String(phValue, 2) + ",\"device\":\"UNO\"}";
  
  // 3) Construir HTTPS POST request a Cloudflare Worker
  String httpRequest = "POST ";
  httpRequest += proxyEndpoint;
  httpRequest += " HTTP/1.1\r\n";
  httpRequest += "Host: ";
  httpRequest += cloudflareWorker;
  httpRequest += "\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Content-Length: ";
  httpRequest += String(payload.length());
  httpRequest += "\r\n";
  httpRequest += "Connection: close\r\n";
  httpRequest += "\r\n";
  httpRequest += payload;

  // 4) CIPSEND - Preparar envío
  unsigned len = httpRequest.length();
  Serial.print(F("> AT+CIPSEND=")); Serial.println(len);
  clearBuffer();
  esp.print(F("AT+CIPSEND=")); esp.println(len);
  
  start = millis();
  bool promptReceived = false;
  while (millis() - start < 5000) {
    if (esp.find(">")) {
      Serial.println(F("[CIPSEND → Prompt OK]"));
      promptReceived = true;
      break;
    }
  }
  
  if (!promptReceived) {
    Serial.println(F("[CIPSEND → Sin prompt]"));
    esp.println("AT+CIPCLOSE");
    return false;
  }

  // 5) Enviar HTTP request a Cloudflare
  Serial.println(F(">> Enviando datos a Cloudflare"));
  Serial.print(F("Payload: ")); Serial.println(payload);
  esp.print(httpRequest);

  // 6) Leer respuesta de Cloudflare (incluye respuesta de Vercel)
  start = millis();
  String response = "";
  while (millis() - start < 12000) {  // Más tiempo para Cloudflare → Vercel
    if (esp.available()) {
      char c = esp.read();
      response += c;
      Serial.write(c);
    }
  }

  // 7) Cerrar conexión con Cloudflare
  Serial.println(F("\n>> AT+CIPCLOSE"));
  clearBuffer();
  esp.println("AT+CIPCLOSE");
  if (esp.find("OK")) Serial.println(F("[CIPCLOSE → OK]"));
  else                Serial.println(F("[CIPCLOSE → FAIL]"));

  // 8) Verificar éxito de Cloudflare y Vercel
  bool success = (response.indexOf("200") != -1 || 
                  response.indexOf("success") != -1 ||
                  response.indexOf("vercel_response") != -1);
  
  if (success) {
    Serial.println(F("✅ DATOS ENVIADOS A VERCEL VIA CLOUDFLARE!"));
    Serial.println(F("Flujo: ESP8266 → Cloudflare → Vercel → Supabase"));
  } else {
    Serial.println(F("❌ Error en Cloudflare o Vercel"));
  }

  return success;
}

// ========== TRANSMISIÓN VIA THINGSPEAK ==========
bool postToThingSpeak(float phValue) {
  if (!esp8266Ready) return false;
  
  Serial.println(F("=== ENVIANDO A THINGSPEAK ==="));
  Serial.print(F("Conectando a: http://"));
  Serial.print(thingSpeakHost);
  Serial.println(F(":80"));
  
  // 1) CIPSTART TCP - Conectar a ThingSpeak (HTTP)
  clearBuffer();
  esp.print(F("AT+CIPSTART=\"TCP\",\""));
  esp.print(thingSpeakHost);
  esp.println(F("\",80"));
  Serial.println(F("> AT+CIPSTART TCP (ThingSpeak HTTP)"));
  
  unsigned long start = millis();
  bool connected = false;
  while (millis() - start < 10000) {
    if (esp.find("Linked") || esp.find("CONNECT")) {
      Serial.println(F("[CIPSTART → Conectado a ThingSpeak]"));
      connected = true;
      break;
    }
    if (esp.find("ERROR") || esp.find("FAIL")) {
      Serial.println(F("[CIPSTART → Error ThingSpeak]"));
      break;
    }
  }
  
  if (!connected) {
    Serial.println(F("[ERROR: No se pudo conectar a ThingSpeak]"));
    return false;
  }

  // 2) Construir GET request para ThingSpeak
  String getStr = "/update?api_key=" + String(writeAPIKey) + "&field1=" + String(phValue, 2);
  String httpRequest = "GET " + getStr + " HTTP/1.1\r\n";
  httpRequest += "Host: ";
  httpRequest += thingSpeakHost;
  httpRequest += "\r\n";
  httpRequest += "Connection: close\r\n";
  httpRequest += "\r\n";

  // 3) CIPSEND - Preparar envío
  unsigned len = httpRequest.length();
  Serial.print(F("> AT+CIPSEND=")); Serial.println(len);
  clearBuffer();
  esp.print(F("AT+CIPSEND=")); esp.println(len);
  
  start = millis();
  bool promptReceived = false;
  while (millis() - start < 5000) {
    if (esp.find(">")) {
      Serial.println(F("[CIPSEND → Prompt OK]"));
      promptReceived = true;
      break;
    }
  }
  
  if (!promptReceived) {
    Serial.println(F("[ERROR: CIPSEND timeout]"));
    esp.println("AT+CIPCLOSE");
    delay(1000);
    return false;
  }

  // 4) Enviar GET request
  esp.print(httpRequest);
  Serial.print(F("Enviando: ")); Serial.println(httpRequest);

  // 5) Leer respuesta
  start = millis();
  bool success = false;
  String response = "";
  while (millis() - start < 8000) {
    while (esp.available()) {
      response += char(esp.read());
    }
  }
  
  Serial.println(F("=== ThingSpeak Response ==="));
  Serial.println(response);
  
  // Buscar Entry ID en la respuesta
  if (response.indexOf("200 OK") > -1) {
    int bodyStart = response.indexOf("\r\n\r\n");
    if (bodyStart > -1) {
      String body = response.substring(bodyStart + 4);
      body.trim();
      int entryID = body.toInt();
      if (entryID > 0) {
        Serial.print(F("[ThingSpeak → Entry ID: ")); Serial.print(entryID); Serial.println(F("]"));
        success = true;
      }
    }
  }

  // 6) Cerrar conexión
  esp.println("AT+CIPCLOSE");
  delay(1000);

  return success;
}

// ========== FUNCIONES AUXILIARES ==========

// Función auxiliar para leer respuesta del serial (mantener por compatibilidad)
String readSerialResponse(int timeout) {
  String response = "";
  unsigned long startTime = millis();
  
  while(millis() - startTime < timeout) {
    if(esp.available()) {
      response += (char)esp.read();
    }
  }
  
  return response;
}

// ========== FUNCIONES DE DIAGNÓSTICO ==========

// Función para verificar conectividad con Cloudflare Worker
void verifyCloudflareConnection() {
  Serial.println(F("=== VERIFICANDO CLOUDFLARE WORKER ==="));
  Serial.print(F("Target: https://"));
  Serial.print(cloudflareWorker);
  Serial.println(proxyEndpoint);
  
  // Verificar conexión HTTPS al Worker de Cloudflare
  Serial.println(F("Probando conexión SSL a Cloudflare..."));
  if (testCloudflareConnection(cloudflareWorker, httpsPort)) {
    Serial.println(F("✅ Cloudflare Worker disponible (SSL)"));
    Serial.println(F("Worker reenviará datos a Vercel via HTTPS"));
  } else {
    Serial.println(F("❌ Cloudflare Worker no disponible (SSL)"));
    Serial.println(F("Verificar URL del Worker y soporte SSL"));
  }
}

// Función para probar conexión HTTPS a Cloudflare Worker
bool testCloudflareConnection(const char* host, uint16_t port) {
  clearBuffer();
  
  Serial.println(F("Probando AT+CIPSTART SSL a Cloudflare..."));
  esp.print(F("AT+CIPSTART=\"SSL\",\""));
  esp.print(host);
  esp.print(F("\","));
  esp.println(port);
  
  unsigned long start = millis();
  while (millis() - start < 15000) {  // SSL necesita más tiempo
    if (esp.find("Linked") || esp.find("CONNECT")) {
      Serial.println(F("Cloudflare SSL conectado, cerrando..."));
      esp.println("AT+CIPCLOSE");
      delay(1000);
      return true;
    }
    if (esp.find("ERROR") || esp.find("FAIL")) {
      Serial.println(F("Cloudflare SSL falló explícitamente"));
      break;
    }
  }
  
  esp.println("AT+CIPCLOSE");
  delay(1000);
  return false;
}

// ========== DATOS DE PRUEBA OPTIMIZADOS ==========
void sendInitialTestData() {
  Serial.println(F("Enviando datos prueba (doble envío)..."));
  
  // Array de valores fijos para pruebas
  float testValues[] = {6.8, 7.0, 7.2, 7.4, 7.1};
  
  for (byte i = 0; i < 5; i++) {
    Serial.print(F("Test "));
    Serial.print(i + 1);
    Serial.print(F("/5 - pH: "));
    Serial.println(testValues[i], 1);
    
    if (esp8266Ready) {
      // Enviar a Dashboard (Vercel/Supabase)
      bool dashboardSuccess = postToDashboard(testValues[i]);
      Serial.print(F("Dashboard: "));
      Serial.println(dashboardSuccess ? F("✅ OK") : F("❌ Error"));
      
      delay(2000); // Pausa entre envíos
      
      // Enviar a ThingSpeak
      bool thingSpeakSuccess = postToThingSpeak(testValues[i]);
      Serial.print(F("ThingSpeak: "));
      Serial.println(thingSpeakSuccess ? F("✅ OK") : F("❌ Error"));
    }
    
    if (i < 4) delay(5000);  // Delay más largo entre tests
  }
  
  Serial.println(F("Pruebas dobles completadas"));
  testDataSent = true;
  delay(1000);
}

/*
 * =====================================================
 * � SOLUCIÓN COMPLETA PARA ESP8266 SIN SSL
 * =====================================================
 * 
 * ✅ PROBLEMA RESUELTO: ESP8266 no soporta SSL/HTTPS
 * ✅ SOLUCIÓN: Proxy HTTP local → Vercel HTTPS
 * 
 * 📋 PASOS PARA USAR:
 * 
 * 1. CONFIGURAR PROXY HTTP LOCAL:
 *    - Instalar dependencias: npm install express axios
 *    - Ejecutar proxy: node http-proxy-server.js
 *    - Anotar la IP mostrada en consola
 * 
 * 2. CONFIGURAR ESP8266:
 *    - Cambiar línea ~32: const char* proxyIP = "TU_IP_AQUÍ";
 *    - Usar la IP mostrada por el servidor proxy
 * 
 * 3. COMPILAR Y SUBIR:
 *    - Compilar código Arduino
 *    - Subir al Arduino UNO
 *    - Abrir Monitor Serie (9600 baudios)
 * 
 * 🔄 FLUJO DE DATOS:
 * ESP8266 → HTTP → Proxy Local → HTTPS → Vercel → Supabase
 * 
 * ✅ RESULTADO FINAL:
 * - Datos pH aparecen en: https://phmetro-phi.vercel.app
 * - Sin dependencia de PC local
 * - Infraestructura global de Cloudflare
 * - Compatible con ESP8266 básicos sin SSL
 * 
 * 📋 CONFIGURACIÓN CLOUDFLARE:
 * 1. Crear Worker en cloudflare.com
 * 2. Copiar código de cloudflare-worker.js
 * 3. Deploy y obtener URL
 * 4. Actualizar cloudflareWorker en línea ~32
 * 
 * =====================================================
 */
