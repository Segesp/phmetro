// ===============================================
// pH METRO ARDUINO UNO + ESP8266 - ULTRA SIMPLE
// ===============================================
// VERSIÓN SIMPLIFICADA PARA PRUEBAS BÁSICAS
// Conexión DIRECTA A VERCEL SIN PROXY (HTTPS)

#include <SoftwareSerial.h>

// ========== CONFIGURACIÓN DE PINES ESP8266 ==========
const uint8_t RX_PIN = 2;    // ESP8266 TX → Arduino 2
const uint8_t TX_PIN = 3;    // ESP8266 RX ← Arduino 3 (con divisor de voltaje)
SoftwareSerial esp(RX_PIN, TX_PIN);

// ========== CONFIGURACIÓN DEL SENSOR pH ==========
const int phPin = A0;

// ========== CONFIGURACIÓN WiFi ==========
const char* ssid = "SEGOVIA3";
const char* password = "76840574";

// ========== CONFIGURACIÓN VERCEL DIRECTO (HTTPS) ==========
const char* vercelHost = "phmetro-phi.vercel.app";
const uint16_t httpsPort = 443;
const char* endpoint = "/api/ph-proxy";

// ========== VARIABLES GLOBALES ==========
bool esp8266Ready = false;
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000;  // Enviar cada 10 segundos

// ========== FUNCIONES AUXILIARES ESP8266 ==========

void clearBuffer() {
  while (esp.available()) esp.read();
}

bool sendAT(const char* cmd, unsigned long timeout = 3000) {
  Serial.print(F("> ")); Serial.println(cmd);
  clearBuffer();
  esp.println(cmd);
  unsigned long start = millis();
  while (millis() - start < timeout) {
    if (esp.find("OK")) {
      Serial.println(F("[OK]"));
      return true;
    }
  }
  Serial.println(F("[FAIL]"));
  return false;
}

// ========== CONFIGURACIÓN INICIAL ==========
void setup() {
  Serial.begin(9600);
  esp.begin(115200);
  
  Serial.println(F("=== pH METRO ULTRA SIMPLE ==="));
  delay(2000);
  
  // Configurar ESP8266
  setupESP8266();
  
  Serial.println(F("Sistema listo!"));
  Serial.println(F("Enviando datos cada 10 segundos..."));
}

void setupESP8266() {
  Serial.println(F("Configurando ESP8266..."));
  
  // Reset y configuración básica
  sendAT("AT+RST", 5000);
  delay(3000);
  sendAT("AT");
  sendAT("AT+CWMODE=1");
  
  // Conectar WiFi
  Serial.print(F("Conectando WiFi: "));
  Serial.println(ssid);
  
  String wifiCmd = "AT+CWJAP=\"";
  wifiCmd += ssid;
  wifiCmd += "\",\"";
  wifiCmd += password;
  wifiCmd += "\"";
  
  if (sendAT(wifiCmd.c_str(), 15000)) {
    Serial.println(F("WiFi conectado!"));
    esp8266Ready = true;
  } else {
    Serial.println(F("Error WiFi"));
  }
}

// ========== ENVÍO DE DATOS HTTPS DIRECTO ==========
bool postToVercel(float phValue) {
  if (!esp8266Ready) return false;
  
  Serial.println(F("=== ENVIANDO A VERCEL (HTTPS) ==="));
  
  // 1) Conectar SSL a Vercel
  clearBuffer();
  esp.print(F("AT+CIPSTART=\"SSL\",\""));
  esp.print(vercelHost);
  esp.print(F("\","));
  esp.println(httpsPort);
  
  Serial.println(F("Conectando SSL a Vercel..."));
  
  unsigned long start = millis();
  bool connected = false;
  while (millis() - start < 15000) {
    if (esp.find("Linked") || esp.find("CONNECT")) {
      Serial.println(F("✅ Conectado a Vercel SSL"));
      connected = true;
      break;
    }
    if (esp.find("ERROR") || esp.find("FAIL")) {
      Serial.println(F("❌ Error conexión SSL"));
      break;
    }
  }
  
  if (!connected) {
    Serial.println(F("No se pudo conectar a Vercel"));
    return false;
  }
  
  // 2) Construir request HTTPS
  String payload = "{\"ph\":" + String(phValue, 2) + ",\"device\":\"UNO\"}";
  
  String request = "POST ";
  request += endpoint;
  request += " HTTP/1.1\r\n";
  request += "Host: ";
  request += vercelHost;
  request += "\r\n";
  request += "Content-Type: application/json\r\n";
  request += "Content-Length: ";
  request += String(payload.length());
  request += "\r\n";
  request += "Connection: close\r\n";
  request += "\r\n";
  request += payload;
  
  // 3) Enviar datos
  unsigned len = request.length();
  clearBuffer();
  esp.print(F("AT+CIPSEND="));
  esp.println(len);
  
  start = millis();
  bool promptOK = false;
  while (millis() - start < 5000) {
    if (esp.find(">")) {
      promptOK = true;
      break;
    }
  }
  
  if (!promptOK) {
    Serial.println(F("Error CIPSEND"));
    esp.println("AT+CIPCLOSE");
    return false;
  }
  
  // 4) Enviar request
  esp.print(request);
  
  // 5) Leer respuesta
  start = millis();
  bool success = false;
  while (millis() - start < 10000) {
    if (esp.find("200 OK") || esp.find("201")) {
      Serial.println(F("✅ Datos enviados exitosamente"));
      success = true;
      break;
    }
    if (esp.find("400") || esp.find("500")) {
      Serial.println(F("❌ Error del servidor"));
      break;
    }
  }
  
  // 6) Cerrar conexión
  esp.println("AT+CIPCLOSE");
  delay(1000);
  
  return success;
}

// ========== SIMULACIÓN DE LECTURA pH ==========
float readPH() {
  // Simulación: pH entre 6.5 y 7.5
  float ph = 6.5 + (random(0, 100) / 100.0);
  return ph;
}

// ========== LOOP PRINCIPAL ==========
void loop() {
  unsigned long currentTime = millis();
  
  // Enviar datos cada 10 segundos
  if (currentTime - lastSendTime >= sendInterval) {
    float ph = readPH();
    
    Serial.print(F("pH simulado: "));
    Serial.println(ph);
    
    if (postToVercel(ph)) {
      Serial.println(F("Transmisión exitosa"));
    } else {
      Serial.println(F("Error en transmisión"));
    }
    
    lastSendTime = currentTime;
    
    Serial.println(F("---"));
  }
  
  delay(1000);  // Esperar 1 segundo
}
