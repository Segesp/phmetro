// ===============================================
// pH METRO ARDUINO UNO + ESP8266 - OPTIMIZADO
// ===============================================
// VERSIÓN OPTIMIZADA PARA ARDUINO UNO (Memoria limitada)
// Comunicación: Comandos AT via Serial

#include <avr/pgmspace.h>  // Para usar PROGMEM y ahorrar RAM

// ========== CONFIGURACIÓN DEL SENSOR pH ==========
const int phPin = A0;

// ========== CONFIGURACIÓN WiFi ==========
// Usar PROGMEM para strings constantes (almacena en Flash, no en RAM)
const char ssid[] PROGMEM = "SEGOVIA3";
const char password[] PROGMEM = "76840574";
const char host[] PROGMEM = "phmetro-phi.vercel.app";  // Tu dominio Vercel
const char api_path[] PROGMEM = "/api/ph-proxy";       // Endpoint proxy HTTP

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

void setup() {
  Serial.begin(9600);
  delay(1000);
  
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
  
  // Enviar datos de prueba
  if (sendTestDataOnStart && esp8266Ready) {
    sendInitialTestData();
  }
  
  Serial.println(F("Sistema listo"));
}

void loop() {
  unsigned long currentTime = millis();
  
  // Leer sensor
  if (currentTime - lastReadTime >= readInterval) {
    readPhSensor();
    lastReadTime = currentTime;
  }
  
  // Enviar datos
  if (readingsReady && currentTime - lastSendTime >= sendInterval) {
    transmissionCounter++;
    
    float avgPH = calculateAveragePH();
    
    Serial.print(F("Enviando pH: "));
    Serial.println(avgPH, 2);
    
    bool success = transmitDataViaESP8266(avgPH);
    
    if (success) {
      consecutiveErrors = 0;
      Serial.println(F("✅ Enviado OK"));
    } else {
      consecutiveErrors++;
      Serial.print(F("❌ Error "));
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
  Serial.println(F("Inicializando ESP8266..."));
  
  // Limpiar buffer serie antes de empezar
  while(Serial.available()) {
    Serial.read();
  }
  
  // Reset con timeout más largo
  Serial.println("AT+RST");
  delay(5000);  // Esperar más tiempo después del reset
  
  // Limpiar buffer después del reset
  while(Serial.available()) {
    Serial.read();
  }
  
  // Test básico
  Serial.println("AT");
  delay(1000);
  
  String response = "";
  unsigned long timeout = millis() + 2000;
  while(millis() < timeout) {
    if(Serial.available()) {
      response += (char)Serial.read();
    }
  }
  
  if (response.indexOf("OK") != -1) {
    Serial.println(F("ESP8266 OK"));
  } else {
    Serial.println(F("ESP8266 Error"));
    esp8266Ready = false;
    return;
  }
  
  // Configurar modo estación
  Serial.println("AT+CWMODE=1");
  delay(1000);
  
  // Limpiar buffer
  while(Serial.available()) {
    Serial.read();
  }
  
  // Conectar WiFi usando comandos simples
  Serial.print("AT+CWJAP=\"");
  Serial.print("SEGOVIA3");
  Serial.print("\",\"");
  Serial.print("76840574");
  Serial.println("\"");
  
  // Esperar conexión WiFi
  delay(15000);
  
  response = "";
  timeout = millis() + 5000;
  while(millis() < timeout) {
    if(Serial.available()) {
      response += (char)Serial.read();
    }
  }
  
  if (response.indexOf("OK") != -1) {
    Serial.println(F("WiFi conectado"));
    esp8266Ready = true;
  } else {
    Serial.println(F("WiFi error"));
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

// ========== TRANSMISIÓN SIMPLIFICADA ==========
bool transmitDataViaESP8266(float phValue) {
  if (!esp8266Ready) return false;
  
  // JSON muy simple
  char jsonBuffer[80];
  snprintf(jsonBuffer, sizeof(jsonBuffer), 
    "{\"ph\":%.2f,\"device\":\"UNO\"}",
    phValue);
  
  Serial.println(F("Iniciando transmision..."));
  
  // Paso 1: Establecer conexión TCP
  Serial.print("AT+CIPSTART=\"TCP\",\"httpbin.org\",80");
  Serial.println();
  delay(3000);
  
  // Limpiar buffer y verificar conexión
  String response = "";
  while(Serial.available()) {
    response += (char)Serial.read();
  }
  
  if (response.indexOf("OK") == -1 && response.indexOf("ALREADY") == -1) {
    Serial.println(F("TCP connection failed"));
    return false;
  }
  
  Serial.println(F("TCP conectado"));
  
  // Paso 2: Preparar HTTP request
  String httpRequest = "POST /post HTTP/1.1\r\n";
  httpRequest += "Host: httpbin.org\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Content-Length: ";
  httpRequest += String(strlen(jsonBuffer));
  httpRequest += "\r\n";
  httpRequest += "Connection: close\r\n\r\n";
  httpRequest += jsonBuffer;
  
  int totalLength = httpRequest.length();
  
  // Paso 3: Enviar comando CIPSEND
  Serial.print("AT+CIPSEND=");
  Serial.println(totalLength);
  delay(1000);
  
  // Verificar prompt >
  response = "";
  unsigned long timeout = millis() + 3000;
  while(millis() < timeout) {
    if(Serial.available()) {
      char c = Serial.read();
      response += c;
      if(c == '>') break;
    }
  }
  
  if (response.indexOf(">") == -1) {
    Serial.println(F("CIPSEND failed"));
    Serial.println("AT+CIPCLOSE");
    return false;
  }
  
  // Paso 4: Enviar datos HTTP
  Serial.print(httpRequest);
  delay(3000);
  
  // Paso 5: Leer respuesta
  response = "";
  timeout = millis() + 5000;
  while(millis() < timeout) {
    if(Serial.available()) {
      response += (char)Serial.read();
    }
  }
  
  Serial.println(F("Respuesta:"));
  Serial.println(response);
  
  // Paso 6: Cerrar conexión
  Serial.println("AT+CIPCLOSE");
  delay(1000);
  
  // Verificar éxito
  bool success = (response.indexOf("200") != -1);
  
  if (success) {
    Serial.println(F("HTTP 200 - Exito!"));
  } else {
    Serial.println(F("No se encontro HTTP 200"));
  }
  
  return success;
}

// ========== FUNCIONES AT OPTIMIZADAS ==========
bool sendATCommand(const char* command, int timeout) {
  Serial.print(F("CMD: "));
  Serial.println(command);
  return waitForResponse_P(PSTR("OK"), timeout);
}

bool sendATCommand_P(const char* command, int timeout) {
  Serial.print(F("CMD: "));
  Serial.println(reinterpret_cast<const __FlashStringHelper*>(command));
  return waitForResponse_P(PSTR("OK"), timeout);
}

bool waitForResponse_P(const char* expected, int timeout) {
  unsigned long startTime = millis();
  byte bufferIndex = 0;
  
  // Limpiar buffer
  memset(responseBuffer, 0, sizeof(responseBuffer));
  
  while (millis() - startTime < timeout) {
    if (Serial.available()) {
      char c = Serial.read();
      if (bufferIndex < sizeof(responseBuffer) - 1) {
        responseBuffer[bufferIndex++] = c;
      }
    }
  }
  
  // Mostrar respuesta para debug
  Serial.print(F("RESP: "));
  Serial.println(responseBuffer);
  
  // Buscar respuesta esperada
  bool found = (strstr_P(responseBuffer, expected) != NULL);
  Serial.print(F("FOUND: "));
  Serial.println(found ? F("YES") : F("NO"));
  
  return found;
}

// ========== DATOS DE PRUEBA OPTIMIZADOS ==========
void sendInitialTestData() {
  Serial.println(F("Enviando datos prueba..."));
  
  // Array de valores fijos (en PROGMEM si fuera necesario)
  float testValues[] = {6.8, 7.0, 7.2, 7.4, 7.1};
  
  for (byte i = 0; i < 5; i++) {
    Serial.print(F("Test "));
    Serial.print(i + 1);
    Serial.print(F("/5 - pH: "));
    Serial.println(testValues[i], 1);
    
    if (esp8266Ready) {
      bool success = transmitDataViaESP8266(testValues[i]);
      Serial.println(success ? F("✅ OK") : F("❌ Error"));
    }
    
    if (i < 4) delay(2000);  // Solo delay si no es el último
  }
  
  Serial.println(F("Pruebas completadas"));
  testDataSent = true;
  delay(1000);
}
