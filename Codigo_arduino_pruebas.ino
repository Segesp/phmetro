// ===============================================
// pH METRO ESP8266 - VERSIÓN DE PRUEBAS RÁPIDAS
// ===============================================
// Este código envía datos simulados cada 10 segundos
// para probar rápidamente la conectividad con el dashboard

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// ========== CONFIGURACIÓN ==========
const char* ssid       = "SEGOVIA3";
const char* password   = "76840574";
const char* serverUrl  = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";

// Configuración para pruebas rápidas
const unsigned long sendInterval = 10000;  // 10 segundos
const int maxRetries = 2;
const int retryDelay = 2000;

// Variables globales
WiFiClient wifiClient;
unsigned long lastSendTime = 0;
int testCounter = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n🧪 === MODO PRUEBAS RÁPIDAS pH METRO ===");
  Serial.println("📡 Enviando datos cada 10 segundos");
  
  randomSeed(analogRead(A0));
  connectToWiFi();
  
  Serial.println("🚀 Sistema listo para pruebas");
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastSendTime >= sendInterval) {
    testCounter++;
    
    // Generar pH de prueba
    float testPH = generateTestPH();
    
    // Enviar datos
    Serial.print("\n📊 PRUEBA #");
    Serial.print(testCounter);
    Serial.print(" - pH: ");
    Serial.println(testPH, 2);
    
    bool success = sendPhData(testPH);
    
    if (success) {
      Serial.println("✅ PRUEBA EXITOSA");
    } else {
      Serial.println("❌ PRUEBA FALLIDA");
    }
    
    Serial.print("⏱️ Próxima prueba en 10 segundos...\n");
    lastSendTime = currentTime;
  }
  
  delay(100);
}

void connectToWiFi() {
  Serial.print("🔗 Conectando a: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi conectado");
    Serial.print("📍 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Error de conexión WiFi");
    delay(5000);
    ESP.restart();
  }
}

float generateTestPH() {
  // Generar diferentes tipos de pH para las pruebas
  int scenario = testCounter % 5;
  
  switch(scenario) {
    case 0: // pH ácido
      return 6.1 + (random(0, 40) / 100.0);
    case 1: // pH ligeramente ácido  
      return 6.5 + (random(0, 30) / 100.0);
    case 2: // pH neutro
      return 6.9 + (random(0, 20) / 100.0);
    case 3: // pH ligeramente básico
      return 7.2 + (random(0, 40) / 100.0);
    case 4: // pH básico
      return 7.8 + (random(0, 50) / 100.0);
    default:
      return 7.0;
  }
}

bool sendPhData(float phValue) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Sin conexión WiFi");
    return false;
  }
  
  HTTPClient http;
  
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.print("📤 Enviando (intento ");
    Serial.print(attempt);
    Serial.println(")...");
    
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(8000);
    
    // JSON simple para pruebas
    String json = "{";
    json += "\"ph\":" + String(phValue, 2) + ",";
    json += "\"device\":\"ESP8266_TEST\",";
    json += "\"test_number\":" + String(testCounter) + ",";
    json += "\"mode\":\"fast_test\"";
    json += "}";
    
    int responseCode = http.POST(json);
    
    Serial.print("📥 Respuesta: ");
    Serial.print(responseCode);
    
    if (responseCode == 200 || responseCode == 201) {
      Serial.println(" ✅");
      http.end();
      return true;
    } else {
      Serial.println(" ❌");
      if (attempt < maxRetries) {
        delay(retryDelay);
      }
    }
  }
  
  http.end();
  return false;
}
