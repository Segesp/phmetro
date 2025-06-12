// ===============================================
// pH METRO ESP8266 - VERSIÓN PARA PRUEBAS
// ===============================================
// Configurado para envío automático de datos de prueba
// Red WiFi: SEGOVIA3 - Contraseña: 76840574
// Dashboard: https://phmetro-phi.vercel.app

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// ========== CONFIGURACIÓN PRINCIPAL ==========
const char* ssid       = "SEGOVIA3";
const char* password   = "76840574";
const char* serverUrl  = "https://phmetro-phi.vercel.app/api/ph-data";

// ========== CONFIGURACIÓN DEL SENSOR ==========
const int phPin = A0;                    // Pin analógico del sensor pH

// ========== CONFIGURACIÓN DE OPERACIÓN ==========
const unsigned long sendInterval = 15000;  // Envío cada 15 segundos (PRUEBA)
const int maxRetries = 3;                   // Reintentos por transmisión
const int retryDelay = 2000;                // Delay entre reintentos

// ========== CONFIGURACIÓN DE DEBUG ==========
const bool debugMode = true;               // Debug detallado
const bool simulationMode = true;          // SIMULACIÓN ACTIVADA
const bool rapidTestMode = true;           // Modo de prueba rápida
const bool sendTestDataOnStart = true;     // Enviar datos de prueba al inicio

// ========== VARIABLES GLOBALES ==========
WiFiClient wifiClient;
unsigned long lastSendTime = 0;
int transmissionCounter = 0;
int consecutiveErrors = 0;
float lastValidPH = 7.0;
bool testDataSent = false;

// Array de valores de prueba realistas
float testPHValues[] = {6.8, 7.0, 7.2, 7.4, 7.1, 6.9, 7.3, 7.5, 6.7, 7.6};
int testValueIndex = 0;

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
  
  // Enviar datos de prueba al inicio
  if (sendTestDataOnStart) {
    sendInitialTestData();
  }
  
  printReadyBanner();
}

// ========== FUNCIÓN LOOP PRINCIPAL ==========
void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastSendTime >= sendInterval) {
    transmissionCounter++;
    
    if (debugMode) {
      printTransmissionHeader();
    }
    
    // Obtener valor de pH (mezcla simulación + valores de prueba)
    float phValue = getTestPhValue();
    
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
      if (consecutiveErrors >= 3) {
        printReconnectWarning();
        connectToWiFi();
        consecutiveErrors = 0;
      }
    }
    
    lastSendTime = currentTime;
    
    if (debugMode) {
      printNextTransmissionInfo();
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
  Serial.println("📡 SSID: " + String(ssid));
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
    Serial.print(".");
    
    if (attempts % 10 == 0) {
      Serial.print(" [" + String(attempts) + "/20]");
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi CONECTADO EXITOSAMENTE");
    Serial.println("📍 IP: " + WiFi.localIP().toString());
    Serial.println("📶 RSSI: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("🌐 Gateway: " + WiFi.gatewayIP().toString());
  } else {
    Serial.println("\n❌ ERROR CONECTANDO WiFi");
    Serial.println("🔄 Reintentando en 5 segundos...");
    delay(5000);
    connectToWiFi(); // Reintentar
  }
}

// ========== FUNCIONES DE DATOS ==========
float getTestPhValue() {
  // Alternar entre valores de prueba y simulación
  if (transmissionCounter % 2 == 0) {
    // Usar valores de prueba predefinidos
    float testValue = testPHValues[testValueIndex];
    testValueIndex = (testValueIndex + 1) % (sizeof(testPHValues) / sizeof(testPHValues[0]));
    
    if (debugMode) {
      Serial.println("🧪 Usando valor de prueba: " + String(testValue, 2));
    }
    
    return testValue;
  } else {
    // Generar valor simulado
    return generateSimulatedPH();
  }
}

float generateSimulatedPH() {
  if (debugMode) {
    Serial.println("🎲 GENERANDO pH SIMULADO:");
  }
  
  // pH base con variaciones realistas
  float basePH = 7.0;
  float timeVariation = sin(millis() / 45000.0) * 0.4;  // Ciclo de 45 segundos
  float randomNoise = (random(-20, 21) / 100.0);        // ±0.20
  float dailyTrend = sin(transmissionCounter * 0.05) * 0.15; // Tendencia lenta
  
  float simulatedPH = basePH + timeVariation + randomNoise + dailyTrend;
  simulatedPH = constrain(simulatedPH, 6.2, 8.2);
  
  if (debugMode) {
    Serial.println("  📊 Base: " + String(basePH, 2) + 
                   " | Tiempo: " + String(timeVariation, 3) + 
                   " | Ruido: " + String(randomNoise, 3) + 
                   " | Tendencia: " + String(dailyTrend, 3));
    Serial.println("  🎯 pH final: " + String(simulatedPH, 3));
  }
  
  return simulatedPH;
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
    http.addHeader("User-Agent", "ESP8266-pH-Pruebas/1.0");
    http.setTimeout(10000);
    
    // Construir JSON con metadatos de prueba
    String json = buildTestJsonPayload(phValue, attempt);
    
    if (debugMode) {
      Serial.println("📤 JSON (" + String(json.length()) + " bytes):");
      Serial.println(json);
    }
    
    // Enviar datos
    unsigned long startTime = millis();
    int responseCode = http.POST(json);
    unsigned long duration = millis() - startTime;
    
    if (debugMode) {
      Serial.println("📥 Código respuesta: " + String(responseCode) + 
                     " | Tiempo: " + String(duration) + "ms");
    }
    
    // Procesar respuesta
    if (responseCode > 0) {
      String response = http.getString();
      
      if (debugMode && response.length() > 0) {
        Serial.println("📄 Respuesta servidor: " + response);
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
        Serial.println("⏳ Esperando " + String(retryDelay/1000) + "s antes del siguiente intento...");
      }
      delay(retryDelay);
    }
  }
  
  return success;
}

String buildTestJsonPayload(float phValue, int attempt) {
  String json = "{";
  json += "\"ph\":" + String(phValue, 3) + ",";
  json += "\"timestamp\":\"\",";
  json += "\"device\":\"ESP8266_Pruebas\",";
  json += "\"network\":\"" + String(ssid) + "\",";
  json += "\"transmission\":" + String(transmissionCounter) + ",";
  json += "\"attempt\":" + String(attempt) + ",";
  json += "\"mode\":\"test_simulation\",";
  json += "\"test_phase\":\"" + String(testDataSent ? "normal" : "initial") + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"ph_classification\":\"" + classifyPH(phValue) + "\"";
  json += "}";
  
  return json;
}

// ========== FUNCIÓN PARA ENVÍO DE DATOS DE PRUEBA INICIALES ==========
void sendInitialTestData() {
  Serial.println("\n🧪 ENVIANDO DATOS DE PRUEBA INICIALES...");
  Serial.println("===========================================");
  
  // Array extendido de valores de pH de prueba
  float initialTestValues[] = {7.0, 6.8, 7.2, 7.4, 6.9, 7.3, 7.1, 7.5};
  int numTests = sizeof(initialTestValues) / sizeof(initialTestValues[0]);
  
  for (int i = 0; i < numTests; i++) {
    float testPH = initialTestValues[i];
    Serial.println("\n📊 PRUEBA INICIAL " + String(i + 1) + "/" + String(numTests) + 
                   " - pH: " + String(testPH, 1) + " (" + classifyPH(testPH) + ")");
    
    // Verificar WiFi antes de enviar
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("❌ WiFi desconectado - reconectando...");
      connectToWiFi();
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      bool success = transmitData(testPH);
      
      if (success) {
        Serial.println("✅ ¡Dato de prueba enviado exitosamente!");
      } else {
        Serial.println("❌ Error enviando dato de prueba");
      }
    }
    
    // Esperar entre envíos de prueba
    if (i < numTests - 1) {
      Serial.println("⏳ Esperando 2 segundos...");
      delay(2000);
    }
  }
  
  Serial.println("\n🎉 ¡DATOS DE PRUEBA INICIALES COMPLETADOS!");
  Serial.println("📊 Se enviaron " + String(numTests) + " valores de pH");
  Serial.println("🔄 Iniciando operación normal cada " + String(sendInterval/1000) + " segundos...");
  Serial.println("===========================================\n");
  
  testDataSent = true;
  delay(1000);
}

// ========== FUNCIONES DE INFORMACIÓN Y DEBUG ==========
void printWelcomeBanner() {
  Serial.println("\n" + String('=') * 50);
  Serial.println("🧪 pH METRO ESP8266 - VERSIÓN PARA PRUEBAS");
  Serial.println(String('=') * 50);
  Serial.println("📊 Configurado para envío automático de datos");
  Serial.println("🌐 Red WiFi: " + String(ssid));
  Serial.println("📱 Dashboard: https://phmetro-phi.vercel.app");
  Serial.println("⚡ Envío cada " + String(sendInterval/1000) + " segundos");
  Serial.println("🧪 Datos de prueba + simulación activados");
  Serial.println(String('=') * 50);
}

void printConfiguration() {
  Serial.println("\n⚙️ CONFIGURACIÓN ACTIVA:");
  Serial.println("📡 WiFi SSID: " + String(ssid));
  Serial.println("🔑 WiFi Password: " + String(strlen(password)) + " caracteres");
  Serial.println("🌐 Servidor: " + String(serverUrl));
  Serial.println("⏱️ Intervalo: " + String(sendInterval/1000) + " segundos");
  Serial.println("🧪 Simulación: ACTIVADA");
  Serial.println("🔍 Debug: ACTIVADO");
  Serial.println("🚀 Modo pruebas: ACTIVADO");
  Serial.println("🔄 Reintentos: " + String(maxRetries));
  Serial.println("📊 Datos iniciales: " + String(sendTestDataOnStart ? "SÍ" : "NO"));
}

void printReadyBanner() {
  Serial.println("\n" + String('✅') * 20);
  Serial.println("🚀 SISTEMA LISTO PARA PRUEBAS");
  Serial.println(String('✅') * 20);
  Serial.println("🎯 Enviando datos de pH automáticamente");
  Serial.println("📱 Ve los datos en: https://phmetro-phi.vercel.app");
  Serial.println("📊 Monitorea este monitor serie para ver el progreso");
  Serial.println(String('🚀') * 20);
}

void printTransmissionHeader() {
  Serial.println("\n" + String('▼') * 40);
  Serial.println("📡 TRANSMISIÓN #" + String(transmissionCounter));
  Serial.println("🕐 Uptime: " + String(millis() / 1000) + "s");
  Serial.println("🌐 WiFi: " + String(WiFi.status() == WL_CONNECTED ? "✅ CONECTADO" : "❌ DESCONECTADO"));
  Serial.println("📶 RSSI: " + String(WiFi.RSSI()) + " dBm");
  if (testDataSent) {
    Serial.println("🧪 Estado: OPERACIÓN NORMAL");
  } else {
    Serial.println("🧪 Estado: ENVIANDO DATOS INICIALES");
  }
  Serial.println(String('▼') * 40);
}

void printDataDetails(float phValue) {
  Serial.println("📋 DATOS A TRANSMITIR:");
  Serial.println("  📊 pH: " + String(phValue, 3));
  Serial.println("  🧪 Clasificación: " + classifyPH(phValue));
  Serial.println("  ✅ Validación: " + String(isValidPH(phValue) ? "VÁLIDO" : "INVÁLIDO"));
  Serial.println("  🎲 Fuente: " + String((transmissionCounter % 2 == 0) ? "Valores de prueba" : "Simulación"));
  Serial.println("  💾 RAM libre: " + String(ESP.getFreeHeap()) + " bytes");
}

String classifyPH(float ph) {
  if (ph < 6.5) return "ÁCIDO";
  else if (ph > 8.5) return "BÁSICO";
  else return "NEUTRO";
}

void printSuccess() {
  Serial.println("✅ ¡TRANSMISIÓN EXITOSA!");
  Serial.println("📊 pH: " + String(lastValidPH, 2) + " | Total enviados: " + String(transmissionCounter));
  Serial.println("📱 Ve el dato en: https://phmetro-phi.vercel.app");
}

void printError() {
  Serial.println("❌ TRANSMISIÓN FALLIDA");
  Serial.println("⚠️ Errores consecutivos: " + String(consecutiveErrors) + "/3");
  if (consecutiveErrors >= 2) {
    Serial.println("🔄 Preparando reconexión WiFi...");
  }
}

void printReconnectWarning() {
  Serial.println("\n🚨 DEMASIADOS ERRORES CONSECUTIVOS");
  Serial.println("🔄 Reconectando WiFi para estabilizar conexión...");
}

void printNextTransmissionInfo() {
  Serial.println("⏳ Próxima transmisión en " + String(sendInterval / 1000) + " segundos");
  Serial.println("📊 Dashboard: https://phmetro-phi.vercel.app");
  Serial.println(String('▲') * 40);
}

void printSystemStatus() {
  Serial.println("\n💓 STATUS SISTEMA:");
  Serial.println("🕐 Uptime: " + String(millis() / 1000) + " segundos");
  Serial.println("💾 RAM libre: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("📶 WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
  Serial.println("📡 Transmisiones exitosas: " + String(transmissionCounter - consecutiveErrors));
  Serial.println("❌ Errores consecutivos: " + String(consecutiveErrors));
  Serial.println("📱 Dashboard: https://phmetro-phi.vercel.app");
}
