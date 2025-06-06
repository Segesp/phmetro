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
  delay(2000); // Dar tiempo para que se abra el monitor serial
  
  Serial.println("\n" + String('=') * 50);
  Serial.println("🚀 INICIANDO pH METRO ESP8266 - MODO DEBUG");
  Serial.println(String('=') * 50);
  Serial.println("🧪 MODO SIMULACIÓN ACTIVADO");
  Serial.println("📊 Generando datos de prueba sintéticos");
  Serial.println("🔍 Monitor detallado de transmisión habilitado");
  Serial.println("");
  
  // Mostrar información del sistema
  printSystemInfo();
  
  // Configurar pin del sensor (aunque no se use en simulación)
  pinMode(phPin, INPUT);
  
  // Inicializar generador de números aleatorios
  randomSeed(analogRead(A0) + millis());
  
  // Conectar a WiFi
  connectToWiFi();
  
  Serial.println(String('=') * 50);
  Serial.println("✅ SISTEMA LISTO PARA TRANSMISIÓN");
  Serial.println(String('=') * 50);
  Serial.print("⏰ Intervalo de envío: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  Serial.print("🌐 Servidor destino: ");
  Serial.println(serverUrl);
  Serial.println("🔬 Rango pH simulado: 6.0-8.5");
  Serial.println("📡 Iniciando transmisión de datos...");
  Serial.println(String('-') * 50);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Verificar si es tiempo de enviar datos
  if (currentTime - lastSendTime >= sendInterval) {
    Serial.println("\n" + String('█') * 30);
    Serial.println("🔄 INICIANDO NUEVA TRANSMISIÓN");
    Serial.println(String('█') * 30);
    
    // Mostrar timestamp
    Serial.print("🕐 Tiempo de sistema: ");
    Serial.print(currentTime / 1000);
    Serial.println(" segundos");
    
    // Leer pH del sensor
    Serial.println("\n📊 GENERANDO DATOS DE pH:");
    float phValue = readPH();
    
    // Mostrar datos detallados antes del envío
    printDataDetails(phValue);
    
    // Validar lectura
    if (isValidPH(phValue)) {
      Serial.println("\n📡 INICIANDO TRANSMISIÓN A SERVIDOR:");
      Serial.println(String('-') * 40);
      
      // Enviar datos al servidor
      bool success = sendPhData(phValue);
      
      Serial.println(String('-') * 40);
      if (success) {
        consecutiveErrors = 0;
        Serial.println("✅ TRANSMISIÓN COMPLETADA EXITOSAMENTE");
        Serial.println("🎯 DATOS CONFIRMADOS EN SERVIDOR");
        printTransmissionSummary(true);
      } else {
        consecutiveErrors++;
        Serial.println("❌ TRANSMISIÓN FALLIDA");
        Serial.print("⚠️ Errores consecutivos: ");
        Serial.println(consecutiveErrors);
        printTransmissionSummary(false);
        
        // Si hay muchos errores consecutivos, reiniciar WiFi
        if (consecutiveErrors >= 5) {
          Serial.println("\n🚨 DEMASIADOS ERRORES CONSECUTIVOS");
          Serial.println("🔄 Reiniciando conexión WiFi...");
          connectToWiFi();
          consecutiveErrors = 0;
        }
      }
    } else {
      Serial.println("\n⚠️ LECTURA DE pH INVÁLIDA");
      Serial.print("❌ Valor rechazado: ");
      Serial.println(phValue);
    }
    
    lastSendTime = currentTime;
    
    Serial.println(String('█') * 30);
    Serial.print("⏳ PRÓXIMA TRANSMISIÓN EN: ");
    Serial.print(sendInterval / 1000);
    Serial.println(" SEGUNDOS");
    Serial.println(String('█') * 30);
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
  Serial.println("🧪 Ejecutando simulador de pH...");
  
  static float basePH = 7.0;  // pH base
  static unsigned long lastChange = 0;
  static float trend = 0.0;
  
  unsigned long currentTime = millis();
  
  // Cambiar la tendencia cada 2 minutos
  if (currentTime - lastChange > 120000) {
    float oldTrend = trend;
    trend = (random(-30, 31) / 100.0); // Cambio entre -0.3 y +0.3
    lastChange = currentTime;
    Serial.print("📈 Cambio de tendencia: ");
    Serial.print(oldTrend, 3);
    Serial.print(" → ");
    Serial.println(trend, 3);
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
  
  Serial.print("📊 ADC simulado: ");
  Serial.print(simulatedADC, 0);
  Serial.print(" → pH calculado: ");
  Serial.print(simulatedPH, 3);
  Serial.print(" (base:");
  Serial.print(basePH, 1);
  Serial.print(" + tendencia:");
  Serial.print(trend, 3);
  Serial.print(" + variación:");
  Serial.print(timeVariation, 3);
  Serial.print(" + ruido:");
  Serial.print(randomNoise, 3);
  Serial.println(")");
  
  return simulatedPH;
}

bool isValidPH(float ph) {
  return (ph >= 0.0 && ph <= 14.0);
}

bool sendPhData(float phValue) {
  // Verificar conexión WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ ERROR: WiFi no conectado");
    Serial.print("🌐 Estado WiFi: ");
    Serial.println(WiFi.status());
    return false;
  }
  
  Serial.println("✅ WiFi conectado - Procediendo con transmisión");
  Serial.print("📶 Intensidad de señal: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  
  HTTPClient http;
  bool success = false;
  
  // Intentar envío con reintentos
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.println(String('▼') * 25);
    Serial.print("🚀 INTENTO DE TRANSMISIÓN #");
    Serial.print(attempt);
    Serial.print(" de ");
    Serial.println(maxRetries);
    Serial.println(String('▼') * 25);
    
    // Configurar HTTPClient
    Serial.println("🔧 Configurando cliente HTTP...");
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", "ESP8266-pH-Sensor/1.0");
    http.setTimeout(10000); // Timeout de 10 segundos
    Serial.println("✅ Cliente HTTP configurado");
    
    // Construir JSON con datos de simulación
    String json = "{\"ph\":";
    json += String(phValue, 2);
    json += ",\"timestamp\":\"";
    json += getISOTime();
    json += "\",\"device\":\"ESP8266\",\"sensor\":\"pH_simulado\",\"mode\":\"simulation\",\"test\":true}";
    
    Serial.println("📦 DATOS A TRANSMITIR:");
    Serial.println(String('-') * 30);
    Serial.print("🔗 URL destino: ");
    Serial.println(serverUrl);
    Serial.print("📄 Content-Type: application/json");
    Serial.print("📊 Tamaño del payload: ");
    Serial.print(json.length());
    Serial.println(" bytes");
    Serial.println("📋 JSON completo:");
    Serial.println(json);
    Serial.println(String('-') * 30);
    
    // Enviar datos
    Serial.println("📡 TRANSMITIENDO DATOS...");
    unsigned long startTime = millis();
    int httpResponseCode = http.POST(json);
    unsigned long endTime = millis();
    
    Serial.print("⏱️ Tiempo de transmisión: ");
    Serial.print(endTime - startTime);
    Serial.println(" ms");
    
    Serial.println("📥 RESPUESTA DEL SERVIDOR:");
    Serial.println(String('-') * 30);
    Serial.print("🔢 Código HTTP: ");
    Serial.print(httpResponseCode);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      
      if (httpResponseCode == 200) {
        Serial.println(" ✅ OK - Datos recibidos correctamente");
      } else if (httpResponseCode == 201) {
        Serial.println(" ✅ CREATED - Datos creados exitosamente");
      } else if (httpResponseCode >= 400 && httpResponseCode < 500) {
        Serial.println(" ❌ ERROR DEL CLIENTE");
      } else if (httpResponseCode >= 500) {
        Serial.println(" ❌ ERROR DEL SERVIDOR");
      } else {
        Serial.print(" ⚠️ CÓDIGO DESCONOCIDO");
      }
      
      Serial.print("📄 Respuesta del servidor (");
      Serial.print(response.length());
      Serial.println(" caracteres):");
      Serial.println(response);
      
      if (httpResponseCode == 200 || httpResponseCode == 201) {
        success = true;
        Serial.println("🎯 TRANSMISIÓN EXITOSA CONFIRMADA");
        break;
      }
    } else {
      Serial.println(" ❌ ERROR DE CONEXIÓN");
      Serial.print("🔍 Detalle del error: ");
      Serial.println(http.errorToString(httpResponseCode));
    }
    
    Serial.println(String('-') * 30);
    http.end();
    
    // Si no es el último intento, esperar antes del siguiente
    if (attempt < maxRetries && !success) {
      Serial.print("⏳ Esperando ");
      Serial.print(retryDelay / 1000);
      Serial.println(" segundos antes del siguiente intento...");
      
      for (int i = retryDelay / 1000; i > 0; i--) {
        Serial.print(i);
        Serial.print("... ");
        delay(1000);
      }
      Serial.println("🔄");
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
  Serial.println("🖥️ INFORMACIÓN DEL SISTEMA:");
  Serial.println(String('-') * 35);
  Serial.print("🆔 Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);
  Serial.print("💾 Flash ID: 0x");
  Serial.println(ESP.getFlashChipId(), HEX);
  Serial.print("⚡ Frecuencia CPU: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("🧠 Memoria libre: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.print("📁 Tamaño Sketch: ");
  Serial.print(ESP.getSketchSize());
  Serial.println(" bytes");
  Serial.print("💽 Espacio libre: ");
  Serial.print(ESP.getFreeSketchSpace());
  Serial.println(" bytes");
  Serial.println(String('-') * 35);
}

// Función para mostrar detalles de los datos antes del envío
void printDataDetails(float phValue) {
  Serial.println("📋 DETALLES DE LOS DATOS:");
  Serial.println(String('-') * 25);
  Serial.print("📊 Valor pH: ");
  Serial.print(phValue, 3);
  Serial.println("");
  Serial.print("🎯 Precisión: 3 decimales");
  Serial.println("");
  Serial.print("📏 Rango válido: 0.0 - 14.0");
  Serial.println("");
  Serial.print("✅ Estado: ");
  Serial.println(isValidPH(phValue) ? "VÁLIDO" : "INVÁLIDO");
  
  // Clasificar el pH
  String classification;
  if (phValue < 6.5) classification = "ÁCIDO";
  else if (phValue > 8.5) classification = "BÁSICO";
  else classification = "NEUTRO/ÓPTIMO";
  
  Serial.print("🧪 Clasificación: ");
  Serial.println(classification);
  Serial.println(String('-') * 25);
}

// Función para mostrar resumen de transmisión
void printTransmissionSummary(bool success) {
  static int totalTransmissions = 0;
  static int successfulTransmissions = 0;
  
  totalTransmissions++;
  if (success) successfulTransmissions++;
  
  Serial.println("📈 RESUMEN DE TRANSMISIONES:");
  Serial.println(String('-') * 30);
  Serial.print("📊 Total enviadas: ");
  Serial.println(totalTransmissions);
  Serial.print("✅ Exitosas: ");
  Serial.println(successfulTransmissions);
  Serial.print("❌ Fallidas: ");
  Serial.println(totalTransmissions - successfulTransmissions);
  Serial.print("📈 Tasa de éxito: ");
  Serial.print((float)successfulTransmissions / totalTransmissions * 100, 1);
  Serial.println("%");
  Serial.println(String('-') * 30);
}

// Función para mostrar estado de la simulación
void printSimulationStatus() {
  static int dataCount = 0;
  dataCount++;
  
  Serial.println("🧪 ESTADO DE SIMULACIÓN:");
  Serial.println(String('-') * 25);
  Serial.print("🔢 Datos simulados enviados: ");
  Serial.println(dataCount);
  Serial.print("⏰ Próxima lectura en: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  Serial.print("🔄 Modo: SIMULACIÓN CONTINUA");
  Serial.println("");
  Serial.println(String('-') * 25);
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
