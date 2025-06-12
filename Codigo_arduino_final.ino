// ===============================================
// pH METRO ESP8266 - VERSIÓN DEBUG ULTRA-VERBOSE CORREGIDO
// ===============================================
// Esta versión imprime absolutamente todo lo que sucede
// Ideal para debugging y verificación de transmisión
// CORREGIDO: Sin caracteres Unicode y con funciones helper

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

// ========== FUNCIONES HELPER ==========
String repeatChar(char c, int count) {
  String result = "";
  for (int i = 0; i < count; i++) {
    result += c;
  }
  return result;
}

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
  Serial.println("\n" + repeatChar('=', 60));
  Serial.println("[DEBUG] pH METRO ESP8266 - MODO DEBUG ULTRA-VERBOSE");
  Serial.println("[LABS] VERSION DE DEBUGGING COMPLETO");
  Serial.println("[MONITOR] TODAS LAS OPERACIONES SERAN MONITOREADAS");
  Serial.println(repeatChar('=', 60));
  Serial.println("[BOOT] Iniciando secuencia de arranque...");
}

void printSystemDiagnostics() {
  Serial.println("\n[SYSTEM] DIAGNOSTICOS DEL SISTEMA:");
  Serial.println(repeatChar('-', 40));
  Serial.print("[ID] ESP Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);
  Serial.print("[FLASH] Flash Chip ID: 0x");
  Serial.println(ESP.getFlashChipId(), HEX);
  Serial.print("[CPU] CPU Frequency: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("[RAM] Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.print("[SIZE] Sketch Size: ");
  Serial.print(ESP.getSketchSize());
  Serial.println(" bytes");
  Serial.print("[SPACE] Free Sketch Space: ");
  Serial.print(ESP.getFreeSketchSpace());
  Serial.println(" bytes");
  Serial.print("[SDK] SDK Version: ");
  Serial.println(ESP.getSdkVersion());
  Serial.println(repeatChar('-', 40));
}

void connectToWiFiVerbose() {
  Serial.println("\n[WIFI] INICIANDO CONEXION WiFi:");
  Serial.println(repeatChar('-', 30));
  Serial.print("[SSID] SSID: ");
  Serial.println(ssid);
  Serial.print("[PASS] Password length: ");
  Serial.print(strlen(password));
  Serial.println(" caracteres");
  
  Serial.println("[CONNECT] Iniciando conexion...");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    attempts++;
    Serial.print("[WAIT] Intento ");
    Serial.print(attempts);
    Serial.print("/30 - Estado: ");
    Serial.println(getWiFiStatusText(WiFi.status()));
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[SUCCESS] WiFi CONECTADO EXITOSAMENTE");
    Serial.println(repeatChar('-', 30));
    Serial.print("[IP] IP Local: ");
    Serial.println(WiFi.localIP());
    Serial.print("[GATEWAY] Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("[DNS] DNS: ");
    Serial.println(WiFi.dnsIP());
    Serial.print("[SIGNAL] RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("[MAC] MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.println(repeatChar('-', 30));
  } else {
    Serial.println("\n[ERROR] ERROR DE CONEXION WiFi");
    Serial.println("[RESTART] Reiniciando sistema en 10 segundos...");
    delay(10000);
    ESP.restart();
  }
}

void printReadyBanner() {
  Serial.println("\n" + repeatChar('+', 50));
  Serial.println("[READY] SISTEMA COMPLETAMENTE OPERATIVO");
  Serial.println(repeatChar('+', 50));
  Serial.print("[INTERVAL] Intervalo de transmision: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  Serial.print("[SERVER] Servidor destino: ");
  Serial.println(serverUrl);
  Serial.println("[MODE] Modo: SIMULACION DE DATOS pH");
  Serial.println("[DEBUG] Debug level: ULTRA-VERBOSE");
  Serial.println(repeatChar('+', 50));
}

void printTransmissionHeader() {
  Serial.println("\n" + repeatChar('#', 50));
  Serial.print("[TX] TRANSMISION #");
  Serial.print(transmissionCounter);
  Serial.println(" - INICIANDO PROCESO COMPLETO");
  Serial.println(repeatChar('#', 50));
  Serial.print("[TIME] Timestamp: ");
  Serial.print(millis() / 1000);
  Serial.println(" segundos desde arranque");
}

float generateAndShowPHData() {
  Serial.println("\n[pH] GENERANDO DATOS DE pH SIMULADOS:");
  Serial.println(repeatChar('-', 35));
  
  // Usar el número de transmisión para crear patrones
  float basePH = 7.0;
  float variation = sin(transmissionCounter * 0.5) * 0.4; // Variación senoidal
  float noise = (random(-15, 16) / 100.0); // Ruido ±0.15
  
  float simulatedPH = basePH + variation + noise;
  simulatedPH = constrain(simulatedPH, 6.0, 8.5);
  
  Serial.print("[BASE] pH base: ");
  Serial.println(basePH, 3);
  Serial.print("[WAVE] Variacion senoidal: ");
  Serial.println(variation, 3);
  Serial.print("[NOISE] Ruido aleatorio: ");
  Serial.println(noise, 3);
  Serial.print("[CALC] pH final calculado: ");
  Serial.println(simulatedPH, 3);
  Serial.print("[FINAL] pH constrainido (6.0-8.5): ");
  Serial.println(simulatedPH, 3);
  Serial.println(repeatChar('-', 35));
  
  return simulatedPH;
}

void printPreTransmissionDetails(float phValue) {
  Serial.println("\n[PRE-TX] DETALLES PRE-TRANSMISION:");
  Serial.println(repeatChar('-', 30));
  Serial.print("[VALUE] Valor a transmitir: ");
  Serial.print(phValue, 3);
  Serial.println("");
  Serial.print("[VALID] Validacion: ");
  Serial.println((phValue >= 0.0 && phValue <= 14.0) ? "VALIDO [OK]" : "INVALIDO [ERROR]");
  Serial.print("[WIFI] Estado WiFi: ");
  Serial.println(getWiFiStatusText(WiFi.status()));
  Serial.print("[SIGNAL] Intensidad señal: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("[RAM] Memoria libre: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.println(repeatChar('-', 30));
}

bool performDetailedTransmission(float phValue) {
  Serial.println("\n[TX-START] INICIANDO TRANSMISION DETALLADA:");
  Serial.println(repeatChar('>', 40));
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[ABORT] WiFi desconectado");
    return false;
  }
  
  HTTPClient http;
  bool success = false;
  
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.println(repeatChar('-', 25));
    Serial.print("[ATTEMPT] INTENTO ");
    Serial.print(attempt);
    Serial.print(" de ");
    Serial.println(maxRetries);
    Serial.println(repeatChar('-', 25));
    
    // Configuración HTTP
    Serial.println("[CONFIG] Configurando cliente HTTP...");
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "ESP8266-Debug/1.0");
    http.setTimeout(15000);
    Serial.println("[OK] Cliente HTTP configurado");
    
    // Construcción del JSON
    Serial.println("[JSON] Construyendo payload JSON...");
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
    
    Serial.println("[PAYLOAD] JSON generado:");
    Serial.println(json);
    Serial.print("[SIZE] Tamaño payload: ");
    Serial.print(json.length());
    Serial.println(" bytes");
    
    // Transmisión
    Serial.println("[SEND] TRANSMITIENDO...");
    unsigned long startTime = millis();
    int responseCode = http.POST(json);
    unsigned long endTime = millis();
    
    Serial.print("[TIME] Tiempo transmision: ");
    Serial.print(endTime - startTime);
    Serial.println(" ms");
    
    // Análisis de respuesta
    Serial.println("[RESPONSE] ANALIZANDO RESPUESTA:");
    Serial.print("[CODE] HTTP Code: ");
    Serial.print(responseCode);
    Serial.print(" (");
    Serial.print(getHTTPStatusText(responseCode));
    Serial.println(")");
    
    if (responseCode > 0) {
      String response = http.getString();
      Serial.print("[LENGTH] Response length: ");
      Serial.print(response.length());
      Serial.println(" caracteres");
      Serial.println("[BODY] Response body:");
      Serial.println(response);
      
      if (responseCode == 200 || responseCode == 201) {
        Serial.println("[SUCCESS] TRANSMISION EXITOSA CONFIRMADA");
        success = true;
        break;
      }
    } else {
      Serial.print("[ERROR] Error de conexion: ");
      Serial.println(http.errorToString(responseCode));
    }
    
    http.end();
    
    if (attempt < maxRetries && !success) {
      Serial.print("[WAIT] Esperando ");
      Serial.print(retryDelay / 1000);
      Serial.println(" segundos...");
      delay(retryDelay);
    }
  }
  
  return success;
}

void printTransmissionResult(bool success) {
  Serial.println(repeatChar('<', 40));
  if (success) {
    Serial.println("[RESULT] TRANSMISION COMPLETADA CON EXITO");
    Serial.println("[CONFIRM] Datos confirmados en servidor");
  } else {
    Serial.println("[RESULT] TRANSMISION FALLIDA");
    Serial.println("[ERROR] No se pudo entregar los datos");
  }
  Serial.println(repeatChar('<', 40));
}

void printNextTransmissionInfo() {
  Serial.println(repeatChar('~', 30));
  Serial.print("[NEXT] Proxima transmision en: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  Serial.print("[COUNT] Sera la transmision #");
  Serial.println(transmissionCounter + 1);
  Serial.println(repeatChar('~', 30));
}

void printSystemStatus() {
  Serial.println("\n[STATUS] ESTADO DEL SISTEMA:");
  Serial.print("[UPTIME] Uptime: ");
  Serial.print(millis() / 1000);
  Serial.print("s | [RAM] RAM: ");
  Serial.print(ESP.getFreeHeap());
  Serial.print(" bytes | [SIGNAL] RSSI: ");
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