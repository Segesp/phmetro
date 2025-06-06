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
const unsigned long sendInterval = 30000;  // Intervalo de envío en ms (30 segundos)
const int maxRetries = 3;                   // Máximo número de reintentos
const int retryDelay = 5000;                // Delay entre reintentos en ms

// ========== VARIABLES GLOBALES ==========
WiFiClient wifiClient;
unsigned long lastSendTime = 0;
int consecutiveErrors = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== INICIANDO pH METRO ESP8266 ===");
  
  // Configurar pin del sensor
  pinMode(phPin, INPUT);
  
  // Conectar a WiFi
  connectToWiFi();
  
  Serial.println("=== SISTEMA LISTO ===");
  Serial.print("Enviando datos cada ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  Serial.print("Servidor: ");
  Serial.println(serverUrl);
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
        Serial.println("✅ Datos enviados exitosamente");
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
  // Tomar múltiples lecturas para mayor precisión
  long total = 0;
  const int samples = 10;
  
  for (int i = 0; i < samples; i++) {
    total += analogRead(phPin);
    delay(10);
  }
  
  float avgReading = total / (float)samples;
  
  // Convertir lectura ADC a pH
  // NOTA: Esta fórmula puede necesitar calibración según tu sensor específico
  float phValue = map(avgReading, adcMin, adcMax, phMin * 100, phMax * 100) / 100.0;
  
  Serial.print("📊 Lectura ADC: ");
  Serial.print(avgReading);
  Serial.print(" → pH: ");
  Serial.println(phValue, 2);
  
  return phValue;
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
    
    // Construir JSON
    String json = "{\"ph\":";
    json += String(phValue, 2);
    json += ",\"timestamp\":\"";
    json += getISOTime();
    json += "\",\"device\":\"ESP8266\",\"sensor\":\"pH\"}";
    
    Serial.print("📤 JSON: ");
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
