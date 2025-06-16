// ===============================================
// pH METRO ARDUINO UNO + ESP8266 - DIAGNÓSTICO
// ===============================================
// Versión simple para diagnosticar problemas de conexión

const char ssid[] = "SEGOVIA3";
const char password[] = "76840574";

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("=== DIAGNÓSTICO ESP8266 ===");
  Serial.println("Iniciando pruebas...");
  
  // Test básico de comunicación
  testBasicCommunication();
  
  // Test de WiFi
  testWiFiConnection();
  
  // Test de conexión TCP simple
  testTCPConnection();
  
  Serial.println("=== DIAGNÓSTICO COMPLETADO ===");
}

void loop() {
  // Loop vacío - solo diagnóstico en setup
  delay(1000);
}

void testBasicCommunication() {
  Serial.println("\n1. TEST COMUNICACIÓN BÁSICA:");
  
  // Reset
  Serial.println("Enviando AT+RST...");
  Serial.println("AT+RST");
  delay(3000);
  
  // Clear serial buffer
  while(Serial.available()) {
    Serial.read();
  }
  
  // Test AT
  Serial.println("Enviando AT...");
  Serial.println("AT");
  
  String response = waitForResponse(2000);
  Serial.println("Respuesta: " + response);
  
  if (response.indexOf("OK") != -1) {
    Serial.println("✅ ESP8266 responde correctamente");
  } else {
    Serial.println("❌ ESP8266 NO responde");
    Serial.println("Verifica conexiones:");
    Serial.println("- VCC a 3.3V");
    Serial.println("- GND a GND");
    Serial.println("- RX a Pin 2");
    Serial.println("- TX a Pin 3");
    Serial.println("- CH_PD a 3.3V");
    return;
  }
}

void testWiFiConnection() {
  Serial.println("\n2. TEST CONEXIÓN WiFi:");
  
  // Configurar modo estación
  Serial.println("AT+CWMODE=1");
  String response = waitForResponse(2000);
  Serial.println("Modo estación: " + response);
  
  // Conectar WiFi
  String wifiCmd = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"";
  Serial.println("Conectando a: " + String(ssid));
  Serial.println(wifiCmd);
  
  response = waitForResponse(15000);
  Serial.println("Respuesta WiFi: " + response);
  
  if (response.indexOf("OK") != -1) {
    Serial.println("✅ WiFi conectado");
    
    // Obtener IP
    Serial.println("AT+CIFSR");
    response = waitForResponse(3000);
    Serial.println("Info IP: " + response);
  } else {
    Serial.println("❌ Error conectando WiFi");
    Serial.println("Verifica:");
    Serial.println("- SSID: " + String(ssid));
    Serial.println("- Password correcto");
    Serial.println("- Señal WiFi suficiente");
  }
}

void testTCPConnection() {
  Serial.println("\n3. TEST CONEXIÓN TCP:");
  
  // Probar conexión a un servidor HTTP simple
  Serial.println("Probando conexión a httpbin.org...");
  Serial.println("AT+CIPSTART=\"TCP\",\"httpbin.org\",80");
  
  String response = waitForResponse(10000);
  Serial.println("Respuesta TCP: " + response);
  
  if (response.indexOf("OK") != -1) {
    Serial.println("✅ Conexión TCP exitosa");
    
    // Cerrar conexión
    Serial.println("AT+CIPCLOSE");
    waitForResponse(2000);
  } else {
    Serial.println("❌ Error conexión TCP");
    Serial.println("Posibles causas:");
    Serial.println("- WiFi no conectado");
    Serial.println("- Firewall bloqueando");
    Serial.println("- DNS no funciona");
  }
  
  // Probar también con IP directa
  Serial.println("\nProbando con IP directa...");
  Serial.println("AT+CIPSTART=\"TCP\",\"8.8.8.8\",80");
  
  response = waitForResponse(10000);
  Serial.println("Respuesta IP: " + response);
  
  if (response.indexOf("OK") != -1) {
    Serial.println("✅ Conexión por IP exitosa");
    Serial.println("AT+CIPCLOSE");
    waitForResponse(2000);
  } else {
    Serial.println("❌ Error conexión por IP");
  }
}

String waitForResponse(int timeout) {
  String response = "";
  unsigned long startTime = millis();
  
  while (millis() - startTime < timeout) {
    if (Serial.available()) {
      response += (char)Serial.read();
    }
  }
  
  return response;
}
