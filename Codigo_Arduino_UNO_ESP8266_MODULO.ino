// ===============================================
// pH METRO ARDUINO UNO + ESP8266 MÓDULO WiFi
// ===============================================
// Arduino UNO: Control principal y lectura sensor pH
// ESP8266: Módulo WiFi (NO requiere programación especial)
// Comunicación: Comandos AT via Serial

// ========== CONFIGURACIÓN DEL SENSOR pH ==========
const int phPin = A0;                    // Pin analógico del sensor pH
const float phMin = 0.0;                 // pH mínimo del sensor
const float phMax = 14.0;                // pH máximo del sensor
const int adcMin = 0;                    // Valor ADC mínimo (0V)
const int adcMax = 1023;                 // Valor ADC máximo (5V en Arduino UNO)

// ========== CONFIGURACIÓN WiFi ==========
const String wifiSSID = "SEGOVIA3";
const String wifiPassword = "76840574";
const String serverURL = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";

// ========== CONFIGURACIÓN DE TIMING ==========
const unsigned long readInterval = 2000;   // Leer sensor cada 2 segundos
const unsigned long sendInterval = 15000;  // Enviar datos cada 15 segundos (MODO PRUEBA)
const int maxReadings = 10;               // Promedio de 10 lecturas

// ========== CONFIGURACIÓN DE DEBUG ==========
const bool debugMode = true;              // Activar logging detallado
const bool simulationMode = true;         // true = SIMULACIÓN DE PRUEBA, false = sensor real

// ========== VARIABLES GLOBALES ==========
float phReadings[maxReadings];           // Array para promedio
int readingIndex = 0;                    // Índice actual
bool readingsReady = false;              // Si tenemos suficientes lecturas
unsigned long lastReadTime = 0;          // Última lectura del sensor
unsigned long lastSendTime = 0;          // Último envío de datos
int transmissionCounter = 0;             // Contador de transmisiones
int consecutiveErrors = 0;               // Errores consecutivos
bool esp8266Ready = false;               // Estado del módulo ESP8266

void setup() {
  Serial.begin(9600);  // Comunicación con ESP8266 (9600 baud estándar)
  delay(2000);
  
  printWelcomeBanner();
  
  // Configurar pin del sensor pH
  pinMode(phPin, INPUT);
  
  // Inicializar array de lecturas
  for (int i = 0; i < maxReadings; i++) {
    phReadings[i] = 7.0;  // Valor inicial neutro
  }
  
  // Inicializar ESP8266
  initializeESP8266();
  
  printReadyBanner();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Leer sensor pH periódicamente
  if (currentTime - lastReadTime >= readInterval) {
    readPhSensor();
    lastReadTime = currentTime;
  }
  
  // Enviar datos cuando sea tiempo y tengamos lecturas suficientes
  if (readingsReady && currentTime - lastSendTime >= sendInterval) {
    transmissionCounter++;
    
    if (debugMode) {
      printTransmissionHeader();
    }
    
    // Calcular pH promedio
    float avgPH = calculateAveragePH();
    
    if (debugMode) {
      printDataDetails(avgPH);
    }
    
    // Transmitir datos
    bool success = transmitDataViaESP8266(avgPH);
    
    // Procesar resultado
    if (success) {
      consecutiveErrors = 0;
      printSuccess();
    } else {
      consecutiveErrors++;
      printError();
      
      // Reinicializar ESP8266 si hay muchos errores
      if (consecutiveErrors >= 5) {
        printReconnectWarning();
        initializeESP8266();
        consecutiveErrors = 0;
      }
    }
    
    lastSendTime = currentTime;
    
    if (debugMode) {
      printNextTransmissionInfo();
    }
  }
  
  delay(100);
}

// ========== FUNCIONES DE INICIALIZACIÓN ==========
void initializeESP8266() {
  Serial.println("🔄 Inicializando módulo ESP8266...");
  
  // Resetear módulo
  sendATCommand("AT+RST", 2000);
  delay(2000);
  
  // Verificar comunicación
  if (sendATCommand("AT", 1000)) {
    Serial.println("✅ ESP8266 responde correctamente");
  } else {
    Serial.println("❌ Error: ESP8266 no responde");
    return;
  }
  
  // Configurar modo estación
  sendATCommand("AT+CWMODE=1", 1000);
  
  // Conectar a WiFi
  String wifiCmd = "AT+CWJAP=\"" + wifiSSID + "\",\"" + wifiPassword + "\"";
  Serial.println("🌐 Conectando a WiFi: " + wifiSSID);
  
  if (sendATCommand(wifiCmd, 10000)) {
    Serial.println("✅ WiFi conectado exitosamente");
    esp8266Ready = true;
  } else {
    Serial.println("❌ Error conectando WiFi");
    esp8266Ready = false;
  }
  
  // Mostrar IP si está conectado
  if (esp8266Ready) {
    sendATCommand("AT+CIFSR", 2000);
  }
}

// ========== FUNCIONES DE LECTURA DEL SENSOR ==========
void readPhSensor() {
  float phValue;
  
  if (simulationMode) {
    phValue = generateSimulatedPH();
  } else {
    phValue = readRealPhSensor();
  }
  
  // Validar lectura
  if (isValidPH(phValue)) {
    // Agregar lectura al array circular
    phReadings[readingIndex] = phValue;
    readingIndex = (readingIndex + 1) % maxReadings;
    
    // Marcar como listo cuando hayamos llenado el array
    if (!readingsReady && readingIndex == 0) {
      readingsReady = true;
      Serial.println("✅ Buffer de lecturas completado - Listo para transmitir");
    }
    
    if (debugMode) {
      Serial.println("📊 pH leído: " + String(phValue, 3) + 
                     " | Buffer: " + String(readingIndex) + "/" + String(maxReadings));
    }
  } else {
    Serial.println("⚠️ Lectura pH inválida: " + String(phValue, 3));
  }
}

float readRealPhSensor() {
  // Tomar múltiples lecturas para estabilidad
  const int samples = 5;
  long total = 0;
  
  for (int i = 0; i < samples; i++) {
    total += analogRead(phPin);
    delay(10);
  }
  
  float avgReading = total / samples;
  
  // Convertir ADC a voltaje (5V en Arduino UNO)
  float voltage = avgReading * (5.0 / 1023.0);
  
  // Convertir voltaje a pH (calibrar según tu sensor)
  // Fórmula típica para sensor pH genérico:
  // pH 7 = 2.5V, pH 4 = 3.0V, pH 10 = 2.0V
  float phValue = 7.0 + ((2.5 - voltage) / 0.18);
  
  if (debugMode) {
    Serial.println("  ADC: " + String(avgReading, 1) + 
                   " | Voltaje: " + String(voltage, 3) + "V" +
                   " | pH: " + String(phValue, 3));
  }
  
  return phValue;
}

float generateSimulatedPH() {
  // Simulación para pruebas
  static float basePH = 7.0;
  float variation = sin(millis() / 30000.0) * 0.3;
  float noise = (random(-10, 11) / 100.0);
  
  float simulatedPH = basePH + variation + noise;
  simulatedPH = constrain(simulatedPH, 6.0, 8.5);
  
  return simulatedPH;
}

float calculateAveragePH() {
  float total = 0;
  for (int i = 0; i < maxReadings; i++) {
    total += phReadings[i];
  }
  return total / maxReadings;
}

bool isValidPH(float ph) {
  return (ph >= 0.0 && ph <= 14.0 && !isnan(ph));
}

// ========== FUNCIONES DE TRANSMISIÓN ==========
bool transmitDataViaESP8266(float phValue) {
  if (!esp8266Ready) {
    Serial.println("❌ ESP8266 no está listo");
    return false;
  }
  
  // Construir JSON
  String jsonData = "{";
  jsonData += "\"ph\":" + String(phValue, 3) + ",";
  jsonData += "\"timestamp\":\"\",";
  jsonData += "\"device\":\"Arduino_UNO\",";
  jsonData += "\"sensor\":\"pH_real\",";
  jsonData += "\"transmission\":" + String(transmissionCounter) + ",";
  jsonData += "\"readings\":" + String(maxReadings);
  jsonData += "}";
  
  if (debugMode) {
    Serial.println("📤 JSON a enviar: " + jsonData);
  }
  
  // Inicializar conexión HTTP
  String startCmd = "AT+CIPSTART=\"TCP\",\"phmetro-p5w79503p-segesps-projects.vercel.app\",80";
  
  if (!sendATCommand(startCmd, 5000)) {
    Serial.println("❌ Error estableciendo conexión TCP");
    return false;
  }
  
  // Construir request HTTP
  String httpRequest = "POST /api/ph-data HTTP/1.1\r\n";
  httpRequest += "Host: phmetro-p5w79503p-segesps-projects.vercel.app\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Content-Length: " + String(jsonData.length()) + "\r\n";
  httpRequest += "Connection: close\r\n\r\n";
  httpRequest += jsonData;
  
  // Enviar datos
  String sendCmd = "AT+CIPSEND=" + String(httpRequest.length());
  
  if (sendATCommand(sendCmd, 2000)) {
    Serial.print(httpRequest);  // Enviar el HTTP request
    delay(2000);
    
    // Leer respuesta
    String response = "";
    unsigned long timeout = millis() + 5000;
    
    while (millis() < timeout && Serial.available()) {
      response += (char)Serial.read();
    }
    
    if (debugMode) {
      Serial.println("📥 Respuesta del servidor:");
      Serial.println(response);
    }
    
    // Cerrar conexión
    sendATCommand("AT+CIPCLOSE", 1000);
    
    // Verificar si fue exitoso
    return (response.indexOf("200") != -1 || response.indexOf("201") != -1);
  }
  
  return false;
}

// ========== FUNCIONES AT COMMANDS ==========
bool sendATCommand(String command, int timeout) {
  Serial.println(command);  // Enviar comando al ESP8266
  
  String response = "";
  unsigned long startTime = millis();
  
  while (millis() - startTime < timeout) {
    if (Serial.available()) {
      response += (char)Serial.read();
    }
  }
  
  if (debugMode) {
    Serial.println("AT Response: " + response);
  }
  
  return (response.indexOf("OK") != -1);
}

// ========== FUNCIONES DE INFORMACIÓN ==========
void printWelcomeBanner() {
  Serial.println("===============================================");
  Serial.println("🚀 pH METRO ARDUINO UNO + ESP8266 MÓDULO");
  Serial.println("===============================================");
  Serial.println("📊 Arduino UNO: Control principal y sensor pH");
  Serial.println("📡 ESP8266: Módulo WiFi (AT Commands)");
  Serial.println("🌐 WiFi: " + wifiSSID);
  Serial.println("🎯 Servidor: " + serverURL);
  Serial.println("⏱️ Intervalo: " + String(sendInterval/1000) + " segundos");
  Serial.println("🧪 Simulación: " + String(simulationMode ? "SÍ" : "NO"));
  Serial.println("🔍 Debug: " + String(debugMode ? "SÍ" : "NO"));
  Serial.println("===============================================");
}

void printReadyBanner() {
  Serial.println("✅ SISTEMA LISTO PARA OPERAR");
  Serial.println("📊 Leyendo sensor cada " + String(readInterval/1000) + " segundos");
  Serial.println("📡 Transmitiendo cada " + String(sendInterval/1000) + " segundos");
  Serial.println("🧮 Promediando " + String(maxReadings) + " lecturas por transmisión");
  Serial.println("===============================================");
}

void printTransmissionHeader() {
  Serial.println("\n📡 TRANSMISIÓN #" + String(transmissionCounter));
  Serial.println("🕐 Tiempo: " + String(millis()/1000) + "s");
  Serial.println("=========================");
}

void printDataDetails(float phValue) {
  Serial.println("📋 DATOS A TRANSMITIR:");
  Serial.println("  📊 pH promedio: " + String(phValue, 3));
  Serial.println("  🧪 Clasificación: " + classifyPH(phValue));
  Serial.println("  📏 Muestras: " + String(maxReadings));
  Serial.println("  ✅ ESP8266: " + String(esp8266Ready ? "LISTO" : "ERROR"));
}

String classifyPH(float ph) {
  if (ph < 6.5) return "ÁCIDO";
  else if (ph > 8.5) return "BÁSICO";
  else return "NEUTRO/ÓPTIMO";
}

void printSuccess() {
  Serial.println("✅ TRANSMISIÓN EXITOSA");
  if (!debugMode) {
    Serial.println("📊 Datos enviados correctamente");
  }
}

void printError() {
  Serial.println("❌ TRANSMISIÓN FALLIDA");
  Serial.println("⚠️ Errores consecutivos: " + String(consecutiveErrors) + "/5");
}

void printReconnectWarning() {
  Serial.println("🚨 Demasiados errores - Reinicializando ESP8266...");
}

void printNextTransmissionInfo() {
  Serial.println("⏳ Próxima transmisión en " + String(sendInterval/1000) + " segundos");
  Serial.println("=========================");
}
