#include <ESP8266WiFi.h>         // Librería WiFi para ESP8266
#include <ESP8266HTTPClient.h>   // Librería HTTPClient para ESP8266
#include <WiFiClient.h>          // Necesaria para la nueva firma de HTTPClient::begin()
#include <ArduinoJson.h>         // Para manejo JSON más robusto (opcional)

// ========== CONFIGURACIÓN WIFI ==========
const char* ssid       = "SEGOVIA3";
const char* password   = "76840574";

// ========== CONFIGURACIÓN SERVIDOR ==========
const char* serverUrl  = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";

// ========== CONFIGURACIÓN SENSOR pH ==========
const int phPin = A0;                    // Pin analógico donde está conectado el sensor
const float phMin = 0.0;                 // pH mínimo del sensor
const float phMax = 14.0;                // pH máximo del sensor
const int adcMin = 0;                    // Valor ADC mínimo (0V)
const int adcMax = 1023;                 // Valor ADC máximo (3.3V en ESP8266)

// ========== CONFIGURACIÓN TIMING ==========
const unsigned long sendInterval = 15000;  // Intervalo de envío en ms (15 segundos para pruebas)
const int maxRetries = 3;                   // Máximo número de reintentos
const int retryDelay = 3000;                // Delay entre reintentos en ms (reducido para pruebas)

// ========== VARIABLES GLOBALES ==========
WiFiClient wifiClient;
unsigned long lastSendTime = 0;
int consecutiveErrors = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== INICIANDO pH METRO ESP8266 ===");
  Serial.println("🧪 MODO SIMULACIÓN ACTIVADO");
  Serial.println("📊 Generando datos de prueba sintéticos");
  
  // Configurar pin del sensor (aunque no se use en simulación)
  pinMode(phPin, INPUT);
  
  // Inicializar generador de números aleatorios
  randomSeed(analogRead(A0) + millis());
  
  // Conectar a WiFi
  connectToWiFi();
  
  Serial.println("=== SISTEMA LISTO ===");
  Serial.print("Enviando datos cada ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  Serial.print("Servidor: ");
  Serial.println(serverUrl);
  Serial.println("🔬 Datos de pH simulados - Rango: 6.0-8.5");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Verificar si es tiempo de enviar datos
  if (currentTime - lastSendTime >= sendInterval) {
    // Leer pH del sensor
    float phValue = readPH();
    
    // Validar lectura
    if (isValidPH(phValue)) {
      // Enviar datos al servidor
      bool success = sendPhData(phValue);
      
      if (success) {
        consecutiveErrors = 0;
        Serial.println("✅ DATOS SIMULADOS enviados exitosamente");
        printSimulationStatus();
      } else {
        consecutiveErrors++;
        Serial.print("❌ Error enviando datos. Errores consecutivos: ");
        Serial.println(consecutiveErrors);
        
        // Si hay muchos errores consecutivos, reiniciar WiFi
        if (consecutiveErrors >= 5) {
          Serial.println("🔄 Demasiados errores, reconectando WiFi...");
          connectToWiFi();
          consecutiveErrors = 0;
        }
      }
    } else {
      Serial.print("⚠️ Lectura de pH inválida: ");
      Serial.println(phValue);
    }
    
    lastSendTime = currentTime;
  }
  
  // Pequeño delay para no sobrecargar el loop
  delay(100);
}

// ========== FUNCIONES ==========

void connectToWiFi() {
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Señal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\n❌ Error conectando WiFi");
    Serial.println("🔄 Reiniciando en 10 segundos...");
    delay(10000);
    ESP.restart();
  }
}

float readPH() {
  // ========== MODO SIMULACIÓN - DATOS DE PRUEBA ==========
  // Generar datos de pH simulados que varían de forma realista
  
  static float basePH = 7.0;  // pH base
  static unsigned long lastChange = 0;
  static float trend = 0.0;
  
  unsigned long currentTime = millis();
  
  // Cambiar la tendencia cada 2 minutos
  if (currentTime - lastChange > 120000) {
    trend = (random(-30, 31) / 100.0); // Cambio entre -0.3 y +0.3
    lastChange = currentTime;
  }
  
  // Simular variaciones naturales
  float timeVariation = sin(currentTime / 30000.0) * 0.2;  // Variación temporal
  float randomNoise = (random(-10, 11) / 100.0);           // Ruido aleatorio ±0.1
  
  // Calcular pH simulado
  float simulatedPH = basePH + trend + timeVariation + randomNoise;
  
  // Mantener en rango realista (6.0 - 8.5)
  simulatedPH = constrain(simulatedPH, 6.0, 8.5);
  
  // Simular lectura ADC para mostrar en logs
  float simulatedADC = map(simulatedPH * 100, phMin * 100, phMax * 100, adcMin, adcMax);
  
  Serial.print("🧪 SIMULACIÓN - ADC: ");
  Serial.print(simulatedADC, 0);
  Serial.print(" → pH: ");
  Serial.print(simulatedPH, 2);
  Serial.print(" (tendencia: ");
  Serial.print(trend > 0 ? "+" : "");
  Serial.print(trend, 2);
  Serial.println(")");
  
  return simulatedPH;
}

bool isValidPH(float ph) {
  return (ph >= 0.0 && ph <= 14.0);
}

bool sendPhData(float phValue) {
  // Verificar conexión WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi no conectado");
    return false;
  }
  
  HTTPClient http;
  bool success = false;
  
  // Intentar envío con reintentos
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.print("📡 Enviando datos (intento ");
    Serial.print(attempt);
    Serial.print("/");
    Serial.print(maxRetries);
    Serial.println(")...");
    
    // Configurar HTTPClient
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "ESP8266-pH-Sensor/1.0");
    http.setTimeout(10000); // Timeout de 10 segundos
    
    // Construir JSON con datos de simulación
    String json = "{\"ph\":";
    json += String(phValue, 2);
    json += ",\"timestamp\":\"";
    json += getISOTime();
    json += "\",\"device\":\"ESP8266\",\"sensor\":\"pH_simulado\",\"mode\":\"simulation\",\"test\":true}";
    
    Serial.print("📤 JSON (SIMULACIÓN): ");
    Serial.println(json);
    
    // Enviar datos
    int httpResponseCode = http.POST(json);
    
    Serial.print("📥 Respuesta HTTP: ");
    Serial.println(httpResponseCode);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("📄 Respuesta: ");
      Serial.println(response);
      
      if (httpResponseCode == 200 || httpResponseCode == 201) {
        success = true;
        break;
      }
    } else {
      Serial.print("❌ Error HTTP: ");
      Serial.println(http.errorToString(httpResponseCode));
    }
    
    http.end();
    
    // Si no es el último intento, esperar antes del siguiente
    if (attempt < maxRetries && !success) {
      Serial.print("⏳ Esperando ");
      Serial.print(retryDelay / 1000);
      Serial.println(" segundos antes del siguiente intento...");
      delay(retryDelay);
    }
  }
  
  http.end();
  return success;
}

String getISOTime() {
  // Si no tienes RTC o NTP configurado, retorna cadena vacía
  // El servidor usará su propio timestamp
  return "";
}

// Función para mostrar información del sistema (opcional)
void printSystemInfo() {
  Serial.println("\n=== INFORMACIÓN DEL SISTEMA ===");
  Serial.print("Chip ID: ");
  Serial.println(ESP.getChipId(), HEX);
  Serial.print("Flash ID: ");
  Serial.println(ESP.getFlashChipId(), HEX);
  Serial.print("Frecuencia CPU: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("Memoria libre: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.print("Tamaño Sketch: ");
  Serial.print(ESP.getSketchSize());
  Serial.println(" bytes");
  Serial.print("Espacio libre: ");
  Serial.print(ESP.getFreeSketchSpace());
  Serial.println(" bytes");
  Serial.println("================================\n");
}

// Función para mostrar estado de la simulación
void printSimulationStatus() {
  static int dataCount = 0;
  dataCount++;
  
  Serial.print("🧪 Datos simulados enviados: ");
  Serial.println(dataCount);
  Serial.print("📊 Próxima lectura en: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
}

// Función para generar escenarios específicos de pH (opcional)
float getScenarioPH(int scenario) {
  switch(scenario) {
    case 1: // pH ácido
      return 6.2 + (random(0, 31) / 100.0); // 6.2 - 6.5
    case 2: // pH neutro  
      return 6.8 + (random(0, 41) / 100.0); // 6.8 - 7.2
    case 3: // pH básico
      return 7.8 + (random(0, 51) / 100.0); // 7.8 - 8.3
    default:
      return 7.0 + (random(-50, 51) / 100.0); // 6.5 - 7.5
  }
}
