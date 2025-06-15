#include <SoftwareSerial.h>
#include <avr/pgmspace.h>  // Para PROGMEM (ahorrar RAM)

// ==== PINES UART PARA ESP8266 ====
// ESP TX → Arduino 2 (RX)
// ESP RX ← Arduino 3 (TX) a través de divisor 5 V→3.3 V
SoftwareSerial esp(2, 3);

// ==== MODOS / TIMINGS ====
const bool debugMode         = true;        // Habilita prints de depuración
const unsigned long sendInterval = 15000;  // Enviar datos cada 15 s (límite gratuito)
unsigned long lastSend       = 0;

// ==== CREDENCIALES Wi-Fi ====
const char* ssid     = "SEGOVIA3";
const char* password = "76840574";

// ==== THINGSPEAK API ====
const char* host        = "api.thingspeak.com";
const char* writeAPIKey = "I4RFD6P62MTKOR8E"; // Tu Write API Key

// ---- Vacía buffer del ESP8266 ----
void clearESP() {
  while (esp.available()) esp.read();
}

// ---- Envía comando AT y espera "OK" ----
bool sendAT(const char* cmd, unsigned long timeout = 2000) {
  if (debugMode) {
    Serial.print(F("> ")); Serial.println(cmd);
  }
  clearESP();
  esp.println(cmd);
  unsigned long t0 = millis();
  while (millis() - t0 < timeout) {
    if (esp.find("OK")) {
      if (debugMode) Serial.println(F("[OK]"));
      return true;
    }
  }
  if (debugMode) Serial.println(F("[FAIL]"));
  return false;
}

// ---- Conectar a Wi-Fi ----
void connectWiFi() {
  if (debugMode) Serial.println(F("=== Conectando a Wi-Fi ==="));
  sendAT("AT");                    // Verifica módulo
  sendAT("AT+CWMODE=1");           // Modo estación
  String cmd = String("AT+CWJAP=\"") + ssid + String("\",\"") + password + String("\"");
  if (sendAT(cmd.c_str(), 15000)) {
    if (debugMode) Serial.println(F("=== Wi-Fi Conectado ==="));
  } else {
    if (debugMode) Serial.println(F("!!! Error Wi-Fi !!!"));
  }
}

// ---- Enviar pH fijo = 7.00 a ThingSpeak ----
bool sendToThingSpeak() {
  float ph = 7.00;  // Valor de prueba fijo

  // Construir GET request
  String getStr = "/update?api_key=" + String(writeAPIKey) + "&field1=" + String(ph, 2);
  String req    = "GET " + getStr + " HTTP/1.1\r\n"
                  "Host: " + host + "\r\n"
                  "Connection: close\r\n\r\n";

  if (debugMode) {
    Serial.println(F("=== Enviando a ThingSpeak ==="));
    Serial.print(F("Request: ")); Serial.println(req);
  }

  // 1) Abrir conexión TCP
  esp.print(F("AT+CIPSTART=\"TCP\",\""));
  esp.print(host);
  esp.println(F("\",80"));
  unsigned long t0 = millis();
  bool opened = false;
  while (millis() - t0 < 5000) {
    if (esp.find("OK") || esp.find("CONNECT")) { opened = true; break; }
  }
  if (!opened) {
    if (debugMode) Serial.println(F("!!! CIPSTART FAIL !!!"));
    return false;
  }

  // 2) Preparar envío
  esp.print(F("AT+CIPSEND=")); esp.println(req.length());
  t0 = millis();
  bool prompt = false;
  while (millis() - t0 < 3000) {
    if (esp.find(">")) { prompt = true; break; }
  }
  if (!prompt) {
    if (debugMode) Serial.println(F("!!! CIPSEND PROMPT FAIL !!!"));
    esp.println(F("AT+CIPCLOSE"));
    return false;
  }

  // 3) Enviar datos
  esp.print(req);

  // 4) Leer respuesta completa
  String resp = "";
  t0 = millis();
  while (millis() - t0 < 7000) {
    while (esp.available()) {
      resp += char(esp.read());
    }
  }
  if (debugMode) {
    Serial.println(F("=== Raw response ==="));
    Serial.println(resp);
  }

  // 5) Extraer cuerpo tras "\r\n\r\n"
  int idx = resp.indexOf("\r\n\r\n");
  if (idx == -1) {
    if (debugMode) Serial.println(F("!!! HTTP BODY NOT FOUND !!!"));
    esp.println(F("AT+CIPCLOSE"));
    return false;
  }
  String body = resp.substring(idx + 4);
  body.trim();
  int entryID = body.toInt();

  if (debugMode) {
    Serial.print(F("Entry ID parsed: ")); Serial.println(entryID);
  }

  // 6) Cerrar conexión
  esp.println(F("AT+CIPCLOSE"));

  return (entryID > 0);
}

void setup() {
  Serial.begin(9600); // Monitor Serie a 9600 baudios
  delay(2000);
  if (debugMode) Serial.println(F("=== Inicio pH Metro Test ==="));
  esp.begin(9600);    // Debe coincidir con el baudio de tu ESP8266
  delay(2000);
  connectWiFi();
}

void loop() {
  unsigned long now = millis();
  if (now - lastSend >= sendInterval) {
    if (sendToThingSpeak()) {
      if (debugMode) Serial.println(F("✅ Envío OK"));
    } else {
      if (debugMode) Serial.println(F("❌ Envío FAIL"));
    }
    lastSend = now;
  }
}