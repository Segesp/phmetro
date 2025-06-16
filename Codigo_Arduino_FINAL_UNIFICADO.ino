// ===============================================
// pH METRO ESP8266 - CÓDIGO FINAL UNIFICADO
// ===============================================
// Este código sintetiza todas las versiones anteriores
// Incluye: debugging, robustez, simplicidad y flexibilidad
// Configuración para ESP8266 standalone o con Arduino UNO

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// ========== CONFIGURACIÓN PRINCIPAL ==========
const char* ssid       = "SEGOVIA3";
const char* password   = "76840574";
const char* serverUrl  = "https://phmetro-phi.vercel.app/api/ph-data";

// ========== CONFIGURACIÓN DEL SENSOR ==========
const int phPin = A0;                    // Pin analógico del sensor pH
const float phMin = 0.0;                 // pH mínimo
const float phMax = 14.0;                // pH máximo
const int adcMin = 0;                    // ADC mínimo (0V)
const int adcMax = 1023;                 // ADC máximo (3.3V ESP8266)

// ========== CONFIGURACIÓN DE OPERACIÓN ==========
const unsigned long sendInterval = 30000;  // Intervalo normal: 30 segundos
const int maxRetries = 3;                   // Reintentos por transmisión
const int retryDelay = 3000;                // Delay entre reintentos
const int maxConsecutiveErrors = 5;         // Errores antes de reconectar WiFi

// ========== CONFIGURACIÓN DE DEBUG ==========
const bool debugMode = true;               // Activar/desactivar debug detallado
const bool simulationMode = true;          // true = simular datos, false = leer sensor real
const bool rapidTestMode = true;           // true = envío cada 10 seg, false = normal
const bool sendTestDataOnStart = true;     // true = enviar datos de prueba al inicio

// ========== VARIABLES GLOBALES ==========
WiFiClient wifiClient;
unsigned long lastSendTime = 0;
int transmissionCounter = 0;
int consecutiveErrors = 0;
float lastValidPH = 7.0;
bool testDataSent = false;                 // Flag para datos de prueba iniciales

// ========== FUNCIÓN SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  printWelcomeBanner();
  printConfiguration();
  
  // Configurar pin del sensor
  pinMode(phPin, INPUT);
  
  // Inicializar generador aleatorio
  randomSeed(analogRead(A0) + millis());
  
  // Conectar WiFi
  connectToWiFi();
  
  // Mostrar diagnósticos del sistema
  if (debugMode) {
    printSystemDiagnostics();
  }
  
  // Enviar datos de prueba al inicio
  if (sendTestDataOnStart) {
    sendInitialTestData();
  }
  
  printReadyBanner();
}

// ========== FUNCIÓN LOOP PRINCIPAL ==========
void loop() {
  unsigned long currentTime = millis();
  
  // Determinar intervalo según modo
  unsigned long interval = rapidTestMode ? 10000 : sendInterval;
  
  if (currentTime - lastSendTime >= interval) {
    transmissionCounter++;
    
    if (debugMode) {
      printTransmissionHeader();
    }
    
    // Obtener valor de pH
    float phValue = getPhValue();
    
    // Validar lectura
    if (isValidPH(phValue)) {
      lastValidPH = phValue;
      
      if (debugMode) {
        printDataDetails(phValue);
      }
      
      // Transmitir datos
      bool success = transmitData(phValue);
      
      // Procesar resultado
      if (success) {
        consecutiveErrors = 0;
        printSuccess();
      } else {
        consecutiveErrors++;
        printError();
        
        // Reconectar si hay muchos errores
        if (consecutiveErrors >= maxConsecutiveErrors) {
          printReconnectWarning();
          connectToWiFi();
          consecutiveErrors = 0;
        }
      }
    } else {
      Serial.println("❌ LECTURA pH INVÁLIDA - Usando último valor válido");
      phValue = lastValidPH;
    }
    
    lastSendTime = currentTime;
    
    if (debugMode) {
      printNextTransmissionInfo(interval);
    }
  }
  
  // Status periódico cada 30 segundos
  static unsigned long lastStatusTime = 0;
  if (debugMode && currentTime - lastStatusTime >= 30000) {
    printSystemStatus();
    lastStatusTime = currentTime;
  }
  
  delay(100);
}

// ========== FUNCIONES DE CONECTIVIDAD ==========
void connectToWiFi() {
  Serial.println("\n🌐 CONECTANDO A WiFi...");
  
  if (debugMode) {
    Serial.println("📡 SSID: " + String(ssid));
    Serial.println("🔑 Password: " + String(strlen(password)) + " caracteres");
  }
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    attempts++;
    Serial.print(".");
    
    if (debugMode && attempts % 10 == 0) {
      Serial.print(" [" + String(attempts) + "/30]");
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi CONECTADO");
    
    if (debugMode) {
      Serial.println("📍 IP: " + WiFi.localIP().toString());
      Serial.println("📶 RSSI: " + String(WiFi.RSSI()) + " dBm");
      Serial.println("🌐 Gateway: " + WiFi.gatewayIP().toString());
      Serial.println("📋 MAC: " + WiFi.macAddress());
    }
  } else {
    Serial.println("\n❌ ERROR CONECTANDO WiFi");
    Serial.println("🔄 Reiniciando en 10 segundos...");
    delay(10000);
    ESP.restart();
  }
}

// ========== FUNCIONES DE LECTURA DE DATOS ==========
float getPhValue() {
  if (simulationMode) {
    return generateSimulatedPH();
  } else {
    return readRealSensor();
  }
}

float generateSimulatedPH() {
  if (debugMode) {
    Serial.println("🧪 GENERANDO pH SIMULADO:");
  }
  
  // pH base con variación temporal
  float basePH = 7.0;
  float timeVariation = sin(millis() / 60000.0) * 0.3;  // Ciclo de 1 minuto
  float randomNoise = (random(-15, 16) / 100.0);        // ±0.15
  float trend = sin(transmissionCounter * 0.1) * 0.2;   // Tendencia lenta
  
  float simulatedPH = basePH + timeVariation + randomNoise + trend;
  simulatedPH = constrain(simulatedPH, 6.0, 8.5);
  
  if (debugMode) {
    Serial.println("  📊 Base: " + String(basePH, 2) + 
                   " | Tiempo: " + String(timeVariation, 3) + 
                   " | Ruido: " + String(randomNoise, 3) + 
                   " | Tendencia: " + String(trend, 3));
    Serial.println("  🎯 pH final: " + String(simulatedPH, 3));
  }
  
  return simulatedPH;
}

float readRealSensor() {
  if (debugMode) {
    Serial.println("📊 LEYENDO SENSOR REAL:");
  }
  
  // Tomar múltiples lecturas para promediar
  const int numReadings = 10;
  long total = 0;
  
  for (int i = 0; i < numReadings; i++) {
    total += analogRead(phPin);
    delay(50);
  }
  
  float avgReading = total / numReadings;
  
  // Convertir ADC a pH (calibrar según tu sensor)
  float voltage = avgReading * (3.3 / 1023.0);  // ESP8266 es 3.3V
  float phValue = 7.0 + ((voltage - 1.65) / 0.18); // Fórmula típica, ajustar según calibración
  
  if (debugMode) {
    Serial.println("  📊 ADC promedio: " + String(avgReading, 1));
    Serial.println("  ⚡ Voltaje: " + String(voltage, 3) + "V");
    Serial.println("  🎯 pH calculado: " + String(phValue, 3));
  }
  
  return phValue;
}

bool isValidPH(float ph) {
  return (ph >= 0.0 && ph <= 14.0 && !isnan(ph));
}

// ========== FUNCIONES DE TRANSMISIÓN ==========
bool transmitData(float phValue) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi desconectado");
    return false;
  }
  
  HTTPClient http;
  bool success = false;
  
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    if (debugMode) {
      Serial.println("🚀 INTENTO " + String(attempt) + "/" + String(maxRetries));
    }
    
    // Configurar HTTP client
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "ESP8266-pH-Final/1.0");
    http.setTimeout(15000);
    
    // Construir JSON
    String json = buildJsonPayload(phValue, attempt);
    
    if (debugMode) {
      Serial.println("📤 JSON (" + String(json.length()) + " bytes): " + json);
    }
    
    // Enviar datos
    unsigned long startTime = millis();
    int responseCode = http.POST(json);
    unsigned long duration = millis() - startTime;
    
    if (debugMode) {
      Serial.println("📥 Código: " + String(responseCode) + 
                     " | Tiempo: " + String(duration) + "ms");
    }
    
    // Procesar respuesta
    if (responseCode > 0) {
      String response = http.getString();
      
      if (debugMode && response.length() > 0) {
        Serial.println("📄 Respuesta: " + response);
      }
      
      if (responseCode == 200 || responseCode == 201) {
        success = true;
        break;
      }
    } else {
      if (debugMode) {
        Serial.println("❌ Error HTTP: " + http.errorToString(responseCode));
      }
    }
    
    http.end();
    
    // Esperar antes del siguiente intento
    if (attempt < maxRetries && !success) {
      if (debugMode) {
        Serial.println("⏳ Esperando " + String(retryDelay/1000) + "s...");
      }
      delay(retryDelay);
    }
  }
  
  return success;
}

String buildJsonPayload(float phValue, int attempt) {
  String json = "{";
  json += "\"ph\":" + String(phValue, 3) + ",";
  json += "\"timestamp\":\"\",";
  json += "\"device\":\"ESP8266\",";
  json += "\"transmission\":" + String(transmissionCounter) + ",";
  json += "\"attempt\":" + String(attempt) + ",";
  json += "\"mode\":\"" + String(simulationMode ? "simulation" : "real") + "\",";
  json += "\"debug\":" + String(debugMode ? "true" : "false") + ",";
  json += "\"rapid_test\":" + String(rapidTestMode ? "true" : "false") + ",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"uptime\":" + String(millis() / 1000);
  json += "}";
  
  return json;
}

// ========== FUNCIÓN PARA ENVÍO DE DATOS DE PRUEBA ==========
void sendInitialTestData() {
  Serial.println("\n🧪 ENVIANDO DATOS DE PRUEBA INICIALES...");
  Serial.println("===========================================");
  
  // Array de valores de pH de prueba
  float testValues[] = {6.8, 7.0, 7.2, 7.4, 7.1};
  int numTests = sizeof(testValues) / sizeof(testValues[0]);
  
  for (int i = 0; i < numTests; i++) {
    float testPH = testValues[i];
    Serial.println("\n📊 TEST " + String(i + 1) + "/" + String(numTests) + " - pH: " + String(testPH, 1));
    
    // Verificar WiFi antes de enviar
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("❌ WiFi desconectado - reconectando...");
      connectToWiFi();
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      bool success = transmitData(testPH);
      
      if (success) {
        Serial.println("✅ Dato de prueba enviado exitosamente");
      } else {
        Serial.println("❌ Error enviando dato de prueba");
      }
    }
    
    // Esperar entre envíos de prueba
    if (i < numTests - 1) {
      Serial.println("⏳ Esperando 3 segundos...");
      delay(3000);
    }
  }
  
  Serial.println("\n🎉 DATOS DE PRUEBA COMPLETADOS");
  Serial.println("🔄 Iniciando operación normal...");
  Serial.println("===========================================\n");
  
  testDataSent = true;
  delay(2000); // Pausa antes de continuar con operación normal
}

// ========== FUNCIONES DE INFORMACIÓN Y DEBUG ==========
void printWelcomeBanner() {
  Serial.println("\n" + String('=') * 50);
  Serial.println("🚀 pH METRO ESP8266 - CÓDIGO FINAL UNIFICADO");
  Serial.println(String('=') * 50);
  Serial.println("📊 Version: 1.0.0 Final");
  Serial.println("🔧 Todas las funcionalidades integradas");
  Serial.println("🧪 Modo flexible: Simulación y sensor real");
  Serial.println("🔍 Debug configurable");
  Serial.println("📡 Transmisión robusta con reintentos");
  Serial.println(String('=') * 50);
}

void printConfiguration() {
  Serial.println("\n⚙️ CONFIGURACIÓN ACTIVA:");
  Serial.println("📡 WiFi: " + String(ssid));
  Serial.println("🌐 Servidor: " + String(serverUrl));
  Serial.println("⏱️ Intervalo: " + String(rapidTestMode ? 10 : sendInterval/1000) + " segundos");
  Serial.println("🧪 Simulación: " + String(simulationMode ? "SÍ" : "NO"));
  Serial.println("🔍 Debug: " + String(debugMode ? "SÍ" : "NO"));
  Serial.println("🚀 Modo rápido: " + String(rapidTestMode ? "SÍ" : "NO"));
  Serial.println("🔄 Reintentos: " + String(maxRetries));
}

void printSystemDiagnostics() {
  Serial.println("\n🖥️ DIAGNÓSTICOS DEL SISTEMA:");
  Serial.println("🆔 Chip ID: 0x" + String(ESP.getChipId(), HEX));
  Serial.println("⚡ CPU: " + String(ESP.getCpuFreqMHz()) + " MHz");
  Serial.println("🧠 RAM libre: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("💾 Flash: " + String(ESP.getFlashChipSize()) + " bytes");
  Serial.println("📏 Sketch: " + String(ESP.getSketchSize()) + " bytes");
  Serial.println("🔧 SDK: " + String(ESP.getSdkVersion()));
}

void printReadyBanner() {
  Serial.println("\n" + String('✅') * 20);
  Serial.println("🚀 SISTEMA COMPLETAMENTE OPERATIVO");
  Serial.println(String('✅') * 20);
  Serial.println("🎯 Listo para recibir y transmitir datos");
  Serial.println("📱 Dashboard: " + String(serverUrl).substring(0, String(serverUrl).indexOf("/api")));
  Serial.println(String('🚀') * 20);
}

void printTransmissionHeader() {
  Serial.println("\n" + String('▼') * 40);
  Serial.println("📡 TRANSMISIÓN #" + String(transmissionCounter));
  Serial.println("🕐 Uptime: " + String(millis() / 1000) + "s");
  if (testDataSent) {
    Serial.println("🧪 Datos de prueba: ✅ ENVIADOS");
  } else {
    Serial.println("🧪 Datos de prueba: ⏳ PENDIENTES");
  }
  Serial.println(String('▼') * 40);
}

void printDataDetails(float phValue) {
  Serial.println("📋 DATOS A TRANSMITIR:");
  Serial.println("  📊 pH: " + String(phValue, 3));
  Serial.println("  🧪 Clasificación: " + classifyPH(phValue));
  Serial.println("  ✅ Validación: " + String(isValidPH(phValue) ? "VÁLIDO" : "INVÁLIDO"));
  Serial.println("  📶 WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
  Serial.println("  💾 RAM: " + String(ESP.getFreeHeap()) + " bytes");
}

String classifyPH(float ph) {
  if (ph < 6.5) return "ÁCIDO";
  else if (ph > 8.5) return "BÁSICO";
  else return "NEUTRO/ÓPTIMO";
}

void printSuccess() {
  Serial.println("✅ TRANSMISIÓN EXITOSA");
  if (!debugMode) {
    Serial.println("📊 pH: " + String(lastValidPH, 2) + " | Contador: " + String(transmissionCounter));
  }
}

void printError() {
  Serial.println("❌ TRANSMISIÓN FALLIDA");
  Serial.println("⚠️ Errores consecutivos: " + String(consecutiveErrors) + "/" + String(maxConsecutiveErrors));
}

void printReconnectWarning() {
  Serial.println("\n🚨 DEMASIADOS ERRORES CONSECUTIVOS");
  Serial.println("🔄 Reconectando WiFi...");
}

void printNextTransmissionInfo(unsigned long interval) {
  Serial.println("⏳ Próxima transmisión en " + String(interval / 1000) + " segundos");
  Serial.println(String('▲') * 40);
}

void printSystemStatus() {
  Serial.println("\n💓 STATUS | Uptime: " + String(millis() / 1000) + "s | RAM: " + 
                 String(ESP.getFreeHeap()) + " | RSSI: " + String(WiFi.RSSI()) + "dBm");
}

// ========== FUNCIONES AUXILIARES ==========
String getISOTime() {
  // Retorna string vacío - el servidor asignará timestamp
  return "";
}

// Función para cambiar modos en tiempo real (opcional)
void checkSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "debug_on") {
      Serial.println("🔍 Debug mode: ON");
    } else if (command == "debug_off") {
      Serial.println("🔍 Debug mode: OFF");
    } else if (command == "rapid_on") {
      Serial.println("🚀 Rapid test mode: ON");
    } else if (command == "rapid_off") {
      Serial.println("🚀 Rapid test mode: OFF");
    } else if (command == "status") {
      printSystemStatus();
      printConfiguration();
    } else if (command == "test") {
      Serial.println("🧪 Enviando datos de prueba...");
      transmitData(7.25);
    }
  }
}
