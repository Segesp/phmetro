// ===============================================
// pH METRO ESP8266 - VERSIÓN DEBUG ULTRA-VERBOSE
// ===============================================
// Esta versión imprime absolutamente todo lo que sucede
// Ideal para debugging y verificación de transmisión

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// ========== CONFIGURACIÓN ==========
const char* ssid       = "SEGOVIA3";
const char* password   = "76840574";
const char* serverUrl  = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";

// Configuración para debug
const unsigned long sendInterval = 20000;  // 20 segundos para observar mejor
const int maxRetries = 2;
const int retryDelay = 3000;

// Variables globales
WiFiClient wifiClient;
unsigned long lastSendTime = 0;
int transmissionCounter = 0;

void setup() {
  Serial.begin(115200);
  delay(3000); // Tiempo extra para abrir monitor serial
  
  printWelcomeBanner();
  printSystemDiagnostics();
  connectToWiFiVerbose();
  printReadyBanner();
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastSendTime >= sendInterval) {
    transmissionCounter++;
    
    printTransmissionHeader();
    
    // Generar y mostrar datos
    float phValue = generateAndShowPHData();
    
    // Mostrar detalles pre-transmisión
    printPreTransmissionDetails(phValue);
    
    // Realizar transmisión con logging detallado
    bool success = performDetailedTransmission(phValue);
    
    // Mostrar resultado final
    printTransmissionResult(success);
    
    lastSendTime = currentTime;
    printNextTransmissionInfo();
  }
  
  // Mostrar estado cada 5 segundos
  static unsigned long lastStatusTime = 0;
  if (currentTime - lastStatusTime >= 5000) {
    printSystemStatus();
    lastStatusTime = currentTime;
  }
  
  delay(100);
}

void printWelcomeBanner() {
  Serial.println("\n" + String('=') * 60);
  Serial.println("🔍 pH METRO ESP8266 - MODO DEBUG ULTRA-VERBOSE");
  Serial.println("🧪 VERSIÓN DE DEBUGGING COMPLETO");
  Serial.println("📊 TODAS LAS OPERACIONES SERÁN MONITOREADAS");
  Serial.println(String('=') * 60);
  Serial.println("🚀 Iniciando secuencia de arranque...");
}

void printSystemDiagnostics() {
  Serial.println("\n🖥️ DIAGNÓSTICOS DEL SISTEMA:");
  Serial.println(String('-') * 40);
  Serial.print("🆔 ESP Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);
  Serial.print("💾 Flash Chip ID: 0x");
  Serial.println(ESP.getFlashChipId(), HEX);
  Serial.print("⚡ CPU Frequency: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("🧠 Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.print("📏 Sketch Size: ");
  Serial.print(ESP.getSketchSize());
  Serial.println(" bytes");
  Serial.print("💽 Free Sketch Space: ");
  Serial.print(ESP.getFreeSketchSpace());
  Serial.println(" bytes");
  Serial.print("🔧 SDK Version: ");
  Serial.println(ESP.getSdkVersion());
  Serial.println(String('-') * 40);
}

void connectToWiFiVerbose() {
  Serial.println("\n🌐 INICIANDO CONEXIÓN WiFi:");
  Serial.println(String('-') * 30);
  Serial.print("📡 SSID: ");
  Serial.println(ssid);
  Serial.print("🔑 Password length: ");
  Serial.print(strlen(password));
  Serial.println(" caracteres");
  
  Serial.println("🔄 Iniciando conexión...");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    attempts++;
    Serial.print("⏳ Intento ");
    Serial.print(attempts);
    Serial.print("/30 - Estado: ");
    Serial.println(getWiFiStatusText(WiFi.status()));
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi CONECTADO EXITOSAMENTE");
    Serial.println(String('-') * 30);
    Serial.print("📍 IP Local: ");
    Serial.println(WiFi.localIP());
    Serial.print("🌐 Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("🔍 DNS: ");
    Serial.println(WiFi.dnsIP());
    Serial.print("📶 RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("📋 MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.println(String('-') * 30);
  } else {
    Serial.println("\n❌ ERROR DE CONEXIÓN WiFi");
    Serial.println("🔄 Reiniciando sistema en 10 segundos...");
    delay(10000);
    ESP.restart();
  }
}

void printReadyBanner() {
  Serial.println("\n" + String('✅') * 20);
  Serial.println("🚀 SISTEMA COMPLETAMENTE OPERATIVO");
  Serial.println(String('✅') * 20);
  Serial.print("⏰ Intervalo de transmisión: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  Serial.print("🎯 Servidor destino: ");
  Serial.println(serverUrl);
  Serial.println("🧪 Modo: SIMULACIÓN DE DATOS pH");
  Serial.println("🔍 Debug level: ULTRA-VERBOSE");
  Serial.println(String('🚀') * 20);
}

void printTransmissionHeader() {
  Serial.println("\n" + String('█') * 50);
  Serial.print("📡 TRANSMISIÓN #");
  Serial.print(transmissionCounter);
  Serial.println(" - INICIANDO PROCESO COMPLETO");
  Serial.println(String('█') * 50);
  Serial.print("🕐 Timestamp: ");
  Serial.print(millis() / 1000);
  Serial.println(" segundos desde arranque");
}

float generateAndShowPHData() {
  Serial.println("\n🧪 GENERANDO DATOS DE pH SIMULADOS:");
  Serial.println(String('-') * 35);
  
  // Usar el número de transmisión para crear patrones
  float basePH = 7.0;
  float variation = sin(transmissionCounter * 0.5) * 0.4; // Variación senoidal
  float noise = (random(-15, 16) / 100.0); // Ruido ±0.15
  
  float simulatedPH = basePH + variation + noise;
  simulatedPH = constrain(simulatedPH, 6.0, 8.5);
  
  Serial.print("📊 pH base: ");
  Serial.println(basePH, 3);
  Serial.print("🌊 Variación senoidal: ");
  Serial.println(variation, 3);
  Serial.print("🎲 Ruido aleatorio: ");
  Serial.println(noise, 3);
  Serial.print("🎯 pH final calculado: ");
  Serial.println(simulatedPH, 3);
  Serial.print("✅ pH constrainido (6.0-8.5): ");
  Serial.println(simulatedPH, 3);
  Serial.println(String('-') * 35);
  
  return simulatedPH;
}

void printPreTransmissionDetails(float phValue) {
  Serial.println("\n📋 DETALLES PRE-TRANSMISIÓN:");
  Serial.println(String('-') * 30);
  Serial.print("📊 Valor a transmitir: ");
  Serial.print(phValue, 3);
  Serial.println("");
  Serial.print("🔍 Validación: ");
  Serial.println((phValue >= 0.0 && phValue <= 14.0) ? "VÁLIDO ✅" : "INVÁLIDO ❌");
  Serial.print("🌐 Estado WiFi: ");
  Serial.println(getWiFiStatusText(WiFi.status()));
  Serial.print("📶 Intensidad señal: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("💾 Memoria libre: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.println(String('-') * 30);
}

bool performDetailedTransmission(float phValue) {
  Serial.println("\n🚀 INICIANDO TRANSMISIÓN DETALLADA:");
  Serial.println(String('▼') * 40);
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ ABORT: WiFi desconectado");
    return false;
  }
  
  HTTPClient http;
  bool success = false;
  
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.println(String('─') * 25);
    Serial.print("🔄 INTENTO ");
    Serial.print(attempt);
    Serial.print(" de ");
    Serial.println(maxRetries);
    Serial.println(String('─') * 25);
    
    // Configuración HTTP
    Serial.println("⚙️ Configurando cliente HTTP...");
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "ESP8266-Debug/1.0");
    http.setTimeout(15000);
    Serial.println("✅ Cliente HTTP configurado");
    
    // Construcción del JSON
    Serial.println("📦 Construyendo payload JSON...");
    String json = "{";
    json += "\"ph\":" + String(phValue, 3) + ",";
    json += "\"transmission\":" + String(transmissionCounter) + ",";
    json += "\"attempt\":" + String(attempt) + ",";
    json += "\"device\":\"ESP8266_DEBUG\",";
    json += "\"mode\":\"ultra_verbose\",";
    json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"uptime\":" + String(millis() / 1000);
    json += "}";
    
    Serial.println("📄 JSON generado:");
    Serial.println(json);
    Serial.print("📏 Tamaño payload: ");
    Serial.print(json.length());
    Serial.println(" bytes");
    
    // Transmisión
    Serial.println("📡 TRANSMITIENDO...");
    unsigned long startTime = millis();
    int responseCode = http.POST(json);
    unsigned long endTime = millis();
    
    Serial.print("⏱️ Tiempo transmisión: ");
    Serial.print(endTime - startTime);
    Serial.println(" ms");
    
    // Análisis de respuesta
    Serial.println("📥 ANALIZANDO RESPUESTA:");
    Serial.print("🔢 HTTP Code: ");
    Serial.print(responseCode);
    Serial.print(" (");
    Serial.print(getHTTPStatusText(responseCode));
    Serial.println(")");
    
    if (responseCode > 0) {
      String response = http.getString();
      Serial.print("📄 Response length: ");
      Serial.print(response.length());
      Serial.println(" caracteres");
      Serial.println("📄 Response body:");
      Serial.println(response);
      
      if (responseCode == 200 || responseCode == 201) {
        Serial.println("🎯 TRANSMISIÓN EXITOSA CONFIRMADA");
        success = true;
        break;
      }
    } else {
      Serial.print("❌ Error de conexión: ");
      Serial.println(http.errorToString(responseCode));
    }
    
    http.end();
    
    if (attempt < maxRetries && !success) {
      Serial.print("⏳ Esperando ");
      Serial.print(retryDelay / 1000);
      Serial.println(" segundos...");
      delay(retryDelay);
    }
  }
  
  return success;
}

void printTransmissionResult(bool success) {
  Serial.println(String('▲') * 40);
  if (success) {
    Serial.println("🎉 TRANSMISIÓN COMPLETADA CON ÉXITO");
    Serial.println("✅ Datos confirmados en servidor");
  } else {
    Serial.println("💥 TRANSMISIÓN FALLIDA");
    Serial.println("❌ No se pudo entregar los datos");
  }
  Serial.println(String('▲') * 40);
}

void printNextTransmissionInfo() {
  Serial.println(String('⏳') * 30);
  Serial.print("📅 Próxima transmisión en: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  Serial.print("🔢 Será la transmisión #");
  Serial.println(transmissionCounter + 1);
  Serial.println(String('⏳') * 30);
}

void printSystemStatus() {
  Serial.println("\n💓 ESTADO DEL SISTEMA:");
  Serial.print("⏰ Uptime: ");
  Serial.print(millis() / 1000);
  Serial.print("s | 🧠 RAM: ");
  Serial.print(ESP.getFreeHeap());
  Serial.print(" bytes | 📶 RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

String getWiFiStatusText(int status) {
  switch(status) {
    case WL_IDLE_STATUS: return "IDLE";
    case WL_NO_SSID_AVAIL: return "NO_SSID";
    case WL_SCAN_COMPLETED: return "SCAN_COMPLETED";
    case WL_CONNECTED: return "CONNECTED";
    case WL_CONNECT_FAILED: return "CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "CONNECTION_LOST";
    case WL_DISCONNECTED: return "DISCONNECTED";
    default: return "UNKNOWN";
  }
}

String getHTTPStatusText(int code) {
  if (code == 200) return "OK";
  if (code == 201) return "CREATED";
  if (code == 400) return "BAD REQUEST";
  if (code == 401) return "UNAUTHORIZED";
  if (code == 403) return "FORBIDDEN";
  if (code == 404) return "NOT FOUND";
  if (code == 500) return "SERVER ERROR";
  if (code == -1) return "CONNECTION FAILED";
  if (code == -2) return "SEND FAILED";
  if (code == -3) return "NO STREAM";
  if (code == -4) return "NO HTTP SERVER";
  if (code == -5) return "TOO LESS RAM";
  if (code == -6) return "ENCODING ERROR";
  if (code == -7) return "STREAM WRITE ERROR";
  if (code == -8) return "READ TIMEOUT";
  return "UNKNOWN";
}
