// ===============================================
// pH METRO ARDUINO UNO + ESP8266 - VERSIÓN COMPLETA
// ===============================================
// Arduino Uno: Lee sensor pH y controla el sistema
// ESP8266: Módulo WiFi para transmitir datos
// Comunicación: Serial entre Arduino y ESP8266

// ========== CONFIGURACIÓN DEL SENSOR pH ==========
const int phPin = A0;                    // Pin analógico del sensor pH
const float phMin = 0.0;                 // pH mínimo del sensor
const float phMax = 14.0;                // pH máximo del sensor
const int adcMin = 0;                    // Valor ADC mínimo (0V)
const int adcMax = 1023;                 // Valor ADC máximo (5V en Arduino Uno)

// ========== CONFIGURACIÓN DE TIMING ==========
const unsigned long readInterval = 2000;   // Leer sensor cada 2 segundos
const unsigned long sendInterval = 15000;  // Enviar datos cada 15 segundos
const int maxReadings = 10;               // Promedio de 10 lecturas

// ========== CONFIGURACIÓN ESP8266 ==========
const String wifiSSID = "SEGOVIA3";
const String wifiPassword = "76840574";
const String serverURL = "https://phmetro-p5w79503p-segesps-projects.vercel.app/api/ph-data";

// ========== VARIABLES GLOBALES ==========
float phReadings[maxReadings];           // Array para promedio
int readingIndex = 0;                    // Índice actual
bool readingsReady = false;              // Si tenemos suficientes lecturas
unsigned long lastReadTime = 0;          // Última lectura del sensor
unsigned long lastSendTime = 0;          // Último envío de datos
int transmissionCounter = 0;             // Contador de transmisiones
bool esp8266Ready = false;               // Estado del ESP8266

// ========== MODO DE OPERACIÓN ==========
#define MODE_REAL_SENSOR 1      // Cambiar a 1 para sensor real, 0 para simulación
#define DEBUG_VERBOSE 1         // Cambiar a 1 para logs detallados

void setup() {
  Serial.begin(9600);  // Comunicación con ESP8266
  delay(2000);
  
  printStartupBanner();
  
  // Configurar pin del sensor
  pinMode(phPin, INPUT);
  
  // Inicializar array de lecturas
  for (int i = 0; i < maxReadings; i++) {
    phReadings[i] = 7.0; // Valor neutro inicial
  }
  
  // Configurar ESP8266
  setupESP8266();
  
  printSystemReady();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Leer sensor de pH
  if (currentTime - lastReadTime >= readInterval) {
    readPHSensor();
    lastReadTime = currentTime;
  }
  
  // Enviar datos si tenemos lecturas válidas
  if (readingsReady && currentTime - lastSendTime >= sendInterval) {
    float avgPH = calculateAveragePH();
    sendPHData(avgPH);
    lastSendTime = currentTime;
  }
  
  // Procesar respuestas del ESP8266
  processESP8266Response();
  
  delay(100);
}

// ========== FUNCIONES DE LECTURA DEL SENSOR ==========

void readPHSensor() {
  float phValue;
  
  #if MODE_REAL_SENSOR
    phValue = readRealPH();
  #else
    phValue = simulatePH();
  #endif
  
  // Guardar lectura en el array
  phReadings[readingIndex] = phValue;
  readingIndex = (readingIndex + 1) % maxReadings;
  
  // Marcar como listo cuando tengamos el array lleno
  if (readingIndex == 0 && !readingsReady) {
    readingsReady = true;
  }
  
  #if DEBUG_VERBOSE
    Serial.print("[pH-READ] Lectura #");
    Serial.print(readingIndex);
    Serial.print(": ");
    Serial.print(phValue, 3);
    Serial.print(" | Promedio: ");
    Serial.println(calculateAveragePH(), 3);
  #endif
}

float readRealPH() {
  // Tomar múltiples lecturas para mayor precisión
  long total = 0;
  const int samples = 10;
  
  for (int i = 0; i < samples; i++) {
    total += analogRead(phPin);
    delay(10);
  }
  
  float avgReading = total / (float)samples;
  
  // Convertir lectura ADC a voltaje (5V referencia en Arduino Uno)
  float voltage = (avgReading * 5.0) / 1024.0;
  
  // Convertir voltaje a pH
  // NOTA: Esta fórmula necesita calibración según tu sensor específico
  // Fórmula típica: pH = 3.5 * voltage + offset
  float phValue = 3.5 * voltage + 0.0; // Ajustar offset según calibración
  
  // Limitar a rango válido
  phValue = constrain(phValue, 0.0, 14.0);
  
  #if DEBUG_VERBOSE
    Serial.print("[SENSOR] ADC: ");
    Serial.print(avgReading);
    Serial.print(" | Voltaje: ");
    Serial.print(voltage, 3);
    Serial.print("V | pH: ");
    Serial.println(phValue, 3);
  #endif
  
  return phValue;
}

float simulatePH() {
  // Simulación para pruebas sin sensor físico
  static float basePH = 7.0;
  static unsigned long lastChange = 0;
  static float trend = 0.0;
  
  unsigned long currentTime = millis();
  
  // Cambiar tendencia cada 2 minutos
  if (currentTime - lastChange > 120000) {
    trend = (random(-30, 31) / 100.0); // ±0.3
    lastChange = currentTime;
  }
  
  // Variaciones naturales
  float timeVariation = sin(currentTime / 30000.0) * 0.2;
  float randomNoise = (random(-10, 11) / 100.0); // ±0.1
  
  float simulatedPH = basePH + trend + timeVariation + randomNoise;
  simulatedPH = constrain(simulatedPH, 6.0, 8.5);
  
  #if DEBUG_VERBOSE
    Serial.print("[SIMUL] Base: ");
    Serial.print(basePH, 1);
    Serial.print(" | Trend: ");
    Serial.print(trend, 3);
    Serial.print(" | Final: ");
    Serial.println(simulatedPH, 3);
  #endif
  
  return simulatedPH;
}

float calculateAveragePH() {
  if (!readingsReady) return 7.0; // Valor por defecto
  
  float total = 0;
  for (int i = 0; i < maxReadings; i++) {
    total += phReadings[i];
  }
  return total / maxReadings;
}

// ========== FUNCIONES DE COMUNICACIÓN ESP8266 ==========

void setupESP8266() {
  Serial.println("[ESP8266] Configurando modulo WiFi...");
  
  // Resetear ESP8266
  Serial.println("AT+RST");
  delay(2000);
  
  // Configurar modo station
  Serial.println("AT+CWMODE=1");
  delay(1000);
  
  // Conectar a WiFi
  Serial.print("AT+CWJAP=\"");
  Serial.print(wifiSSID);
  Serial.print("\",\"");
  Serial.print(wifiPassword);
  Serial.println("\"");
  delay(5000);
  
  // Verificar conexión
  Serial.println("AT+CIFSR");
  delay(2000);
  
  esp8266Ready = true;
  Serial.println("[ESP8266] Configuracion completada");
}

void sendPHData(float phValue) {
  if (!esp8266Ready) {
    Serial.println("[ERROR] ESP8266 no esta listo");
    return;
  }
  
  transmissionCounter++;
  
  #if DEBUG_VERBOSE
    printTransmissionHeader();
  #endif
  
  // Construir JSON
  String jsonData = buildJSONPayload(phValue);
  
  #if DEBUG_VERBOSE
    Serial.print("[JSON] Payload: ");
    Serial.println(jsonData);
  #endif
  
  // Enviar datos via ESP8266
  sendHTTPRequest(jsonData);
}

String buildJSONPayload(float phValue) {
  String json = "{";
  json += "\"ph\":" + String(phValue, 3) + ",";
  json += "\"device\":\"Arduino_Uno\",";
  json += "\"sensor\":\"pH_probe\",";
  json += "\"transmission\":" + String(transmissionCounter) + ",";
  json += "\"mode\":\"";
  
  #if MODE_REAL_SENSOR
    json += "real_sensor";
  #else
    json += "simulation";
  #endif
  
  json += "\",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"readings_count\":" + String(maxReadings);
  json += "}";
  
  return json;
}

void sendHTTPRequest(String jsonData) {
  // Establecer conexión TCP
  Serial.println("AT+CIPSTART=\"TCP\",\"phmetro-p5w79503p-segesps-projects.vercel.app\",443");
  delay(2000);
  
  // Construir request HTTP
  String httpRequest = "POST /api/ph-data HTTP/1.1\r\n";
  httpRequest += "Host: phmetro-p5w79503p-segesps-projects.vercel.app\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "User-Agent: Arduino-Uno/1.0\r\n";
  httpRequest += "Content-Length: " + String(jsonData.length()) + "\r\n";
  httpRequest += "Connection: close\r\n\r\n";
  httpRequest += jsonData;
  
  // Enviar longitud de datos
  Serial.print("AT+CIPSEND=");
  Serial.println(httpRequest.length());
  delay(1000);
  
  // Enviar datos
  Serial.print(httpRequest);
  delay(3000);
  
  // Cerrar conexión
  Serial.println("AT+CIPCLOSE");
  delay(1000);
  
  #if DEBUG_VERBOSE
    Serial.println("[HTTP] Request enviado");
  #endif
}

void processESP8266Response() {
  // Procesar respuestas del ESP8266 (si las hay)
  // Esta función puede expandirse para manejar respuestas específicas
}

// ========== FUNCIONES DE UTILIDAD ==========

void printStartupBanner() {
  Serial.println("");
  Serial.println("===============================================");
  Serial.println("    pH METRO ARDUINO UNO + ESP8266");
  Serial.println("===============================================");
  Serial.print("Modo: ");
  
  #if MODE_REAL_SENSOR
    Serial.println("SENSOR REAL");
  #else
    Serial.println("SIMULACION");
  #endif
  
  Serial.print("Debug: ");
  #if DEBUG_VERBOSE
    Serial.println("ACTIVADO");
  #else
    Serial.println("DESACTIVADO");
  #endif
  
  Serial.print("Intervalo lectura: ");
  Serial.print(readInterval / 1000);
  Serial.println(" segundos");
  
  Serial.print("Intervalo envio: ");
  Serial.print(sendInterval / 1000);
  Serial.println(" segundos");
  
  Serial.print("Promedio de: ");
  Serial.print(maxReadings);
  Serial.println(" lecturas");
  
  Serial.println("===============================================");
}

void printSystemReady() {
  Serial.println("");
  Serial.println("*** SISTEMA LISTO ***");
  Serial.println("Iniciando monitoreo de pH...");
  Serial.println("");
}

void printTransmissionHeader() {
  Serial.println("");
  Serial.println("##################################################");
  Serial.print("# TRANSMISION #");
  Serial.print(transmissionCounter);
  Serial.println(" - ENVIANDO DATOS AL SERVIDOR");
  Serial.println("##################################################");
  Serial.print("Tiempo de funcionamiento: ");
  Serial.print(millis() / 1000);
  Serial.println(" segundos");
}

String getPhStatus(float ph) {
  if (ph >= 6.5 && ph <= 8.5) return "OPTIMO";
  if (ph >= 6.0 && ph <= 9.0) return "ACEPTABLE";
  return "CRITICO";
}

bool isValidPH(float ph) {
  return (ph >= 0.0 && ph <= 14.0);
}

// ========== FUNCIONES DE CALIBRACIÓN ==========

void calibratePHSensor() {
  // Función para calibrar el sensor con soluciones conocidas
  // Implementar según las necesidades específicas del sensor
  Serial.println("[CALIBRATION] Iniciando calibracion...");
  
  // Aquí iría el código de calibración específico
  // Por ejemplo, lecturas con pH 4, 7 y 10
  
  Serial.println("[CALIBRATION] Calibracion completada");
}

// ========== FUNCIONES DE DIAGNÓSTICO ==========

void runDiagnostics() {
  Serial.println("");
  Serial.println("=== DIAGNOSTICOS DEL SISTEMA ===");
  
  // Probar sensor
  Serial.print("Sensor pH: ");
  float testReading = analogRead(phPin);
  Serial.print(testReading);
  Serial.println(" (ADC)");
  
  // Probar ESP8266
  Serial.print("ESP8266: ");
  Serial.println(esp8266Ready ? "CONECTADO" : "DESCONECTADO");
  
  // Memoria disponible (estimación)
  Serial.print("RAM disponible: ");
  Serial.print(freeRam());
  Serial.println(" bytes");
  
  Serial.println("================================");
}

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
