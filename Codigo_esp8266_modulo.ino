// ===============================================
// ESP8266 COMO MÓDULO WiFi PARA ARDUINO UNO
// ===============================================
// Este código va en el ESP8266 cuando se usa como módulo WiFi
// Recibe comandos AT del Arduino Uno y maneja la conectividad

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// ========== CONFIGURACIÓN WIFI ==========
const char* ssid = "SEGOVIA3";
const char* password = "76840574";
const char* serverUrl = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";

// ========== VARIABLES GLOBALES ==========
WiFiClient wifiClient;
bool wifiConnected = false;
String inputBuffer = "";

void setup() {
  Serial.begin(9600);  // Comunicación con Arduino Uno
  delay(1000);
  
  Serial.println("ESP8266 WiFi Module Ready");
  
  // Conectar a WiFi automáticamente
  connectToWiFi();
  
  Serial.println("AT Commands Ready");
}

void loop() {
  // Leer comandos del Arduino Uno
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        processATCommand(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }
  
  // Verificar estado de WiFi periódicamente
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 30000) { // Cada 30 segundos
    checkWiFiStatus();
    lastCheck = millis();
  }
  
  delay(10);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    wifiConnected = true;
  } else {
    Serial.println("\nWiFi Connection Failed!");
    wifiConnected = false;
  }
}

void processATCommand(String command) {
  command.trim();
  
  if (command == "AT") {
    Serial.println("OK");
  }
  else if (command == "AT+RST") {
    Serial.println("OK");
    delay(1000);
    ESP.restart();
  }
  else if (command == "AT+CWMODE=1") {
    WiFi.mode(WIFI_STA);
    Serial.println("OK");
  }
  else if (command.startsWith("AT+CWJAP=")) {
    // Extraer SSID y password del comando
    // Formato: AT+CWJAP="SSID","PASSWORD"
    int firstQuote = command.indexOf('"');
    int secondQuote = command.indexOf('"', firstQuote + 1);
    int thirdQuote = command.indexOf('"', secondQuote + 1);
    int fourthQuote = command.indexOf('"', thirdQuote + 1);
    
    if (firstQuote != -1 && secondQuote != -1 && thirdQuote != -1 && fourthQuote != -1) {
      String ssidParam = command.substring(firstQuote + 1, secondQuote);
      String passParam = command.substring(thirdQuote + 1, fourthQuote);
      
      WiFi.begin(ssidParam.c_str(), passParam.c_str());
      
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WIFI CONNECTED");
        Serial.println("OK");
        wifiConnected = true;
      } else {
        Serial.println("WIFI DISCONNECT");
        Serial.println("ERROR");
        wifiConnected = false;
      }
    } else {
      Serial.println("ERROR");
    }
  }
  else if (command == "AT+CIFSR") {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("+CIFSR:STAIP,\"");
      Serial.print(WiFi.localIP());
      Serial.println("\"");
      Serial.print("+CIFSR:STAMAC,\"");
      Serial.print(WiFi.macAddress());
      Serial.println("\"");
    }
    Serial.println("OK");
  }
  else if (command.startsWith("AT+CIPSTART=")) {
    // Comando para establecer conexión TCP
    // Formato: AT+CIPSTART="TCP","host",port
    if (wifiConnected) {
      Serial.println("CONNECT");
      Serial.println("OK");
    } else {
      Serial.println("ERROR");
    }
  }
  else if (command.startsWith("AT+CIPSEND=")) {
    // Preparar para recibir datos
    Serial.println(">");
    
    // Leer el número de bytes a enviar
    int length = command.substring(11).toInt();
    
    // Leer los datos HTTP del Arduino
    String httpData = "";
    unsigned long startTime = millis();
    int bytesRead = 0;
    
    while (bytesRead < length && (millis() - startTime) < 10000) {
      if (Serial.available()) {
        httpData += (char)Serial.read();
        bytesRead++;
      }
    }
    
    // Enviar datos al servidor
    if (httpData.length() > 0) {
      bool success = sendHTTPData(httpData);
      if (success) {
        Serial.println("SEND OK");
      } else {
        Serial.println("SEND FAIL");
      }
    }
  }
  else if (command == "AT+CIPCLOSE") {
    Serial.println("CLOSED");
    Serial.println("OK");
  }
  else {
    Serial.println("ERROR");
  }
}

bool sendHTTPData(String httpRequest) {
  if (!wifiConnected) {
    return false;
  }
  
  HTTPClient http;
  WiFiClientSecure client;
  
  // Para HTTPS, necesitamos configurar el cliente seguro
  client.setInsecure(); // Para pruebas - en producción usar certificados
  
  http.begin(client, serverUrl);
  
  // Extraer el JSON del request HTTP
  int jsonStart = httpRequest.lastIndexOf('\n') + 1;
  String jsonData = httpRequest.substring(jsonStart);
  
  // Configurar headers
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "Arduino-ESP8266/1.0");
  
  // Enviar request
  int httpResponseCode = http.POST(jsonData);
  
  // Procesar respuesta
  if (httpResponseCode > 0) {
    String response = http.getString();
    
    // Enviar respuesta al Arduino (simplificada)
    Serial.println("+IPD," + String(response.length()) + ":" + response);
    
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      http.end();
      return true;
    }
  }
  
  http.end();
  return false;
}

void checkWiFiStatus() {
  if (WiFi.status() != WL_CONNECTED && wifiConnected) {
    Serial.println("WiFi connection lost! Reconnecting...");
    wifiConnected = false;
    connectToWiFi();
  }
}

// Función para diagnósticos del ESP8266
void printDiagnostics() {
  Serial.println("\n=== ESP8266 Diagnostics ===");
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  Serial.println("==========================");
}
