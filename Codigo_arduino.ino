#include <ESP8266WiFi.h>         // Librería WiFi para ESP8266
#include <ESP8266HTTPClient.h>   // Librería HTTPClient para ESP8266
#include <WiFiClient.h>          // Necesaria para la nueva firma de HTTPClient::begin()

const char* ssid       = "SEGOVIA3";
const char* password   = "76840574";
const char* serverUrl  = "https://p-h-iota.vercel.app/api/ph-data"; // Dominio real

const int phPin = A0; // Pin analógico donde está conectado el sensor de pH

WiFiClient wifiClient;  // Objeto WiFiClient que se pasa a HTTPClient

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
}

void loop() {
  // Leer valor de pH (ajusta la fórmula según tu sensor si es necesario)
  float phValue = analogRead(phPin) * (14.0 / 1023.0);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Usar la nueva firma: pasar wifiClient como primer parámetro
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Construir el JSON con el valor de pH y timestamp (si está disponible)
    String json = "{\"ph\":";
    json += String(phValue, 2);
    json += ",\"timestamp\":\"";
    json += getISOTime();
    json += "\"}";

    int httpResponseCode = http.POST(json);
    Serial.print("Enviando: ");
    Serial.println(json);
    Serial.print("Respuesta HTTP: ");
    Serial.println(httpResponseCode);

    http.end();  // Cierra la conexión
  } else {
    Serial.println("WiFi no conectado");
  }

  delay(30000); // Espera 30 segundos antes de la siguiente lectura/envío
}

// Función para obtener la hora en formato ISO (dejar vacía si no usas NTP/RTC)
String getISOTime() {
  // Si no tienes RTC o NTP configurado, simplemente retorna cadena vacía
  return "";
}
