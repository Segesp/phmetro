import { NextResponse } from 'next/server'

export async function GET() {
  const apiDocs = {
    title: "pH Metro API Documentation",
    version: "1.0.0",
    description: "API para recibir y consultar datos del sensor de pH",
    endpoints: {
      "POST /api/ph-data": {
        description: "Enviar nueva lectura de pH desde Arduino",
        body: {
          ph: "number (0-14) - Valor de pH medido",
          timestamp: "string (ISO 8601) - Timestamp opcional"
        },
        responses: {
          200: {
            success: true,
            message: "Datos guardados exitosamente",
            data: "Objeto con los datos insertados"
          },
          400: {
            error: "Valor de pH inválido"
          },
          500: {
            error: "Error interno del servidor"
          }
        },
        example: {
          request: {
            ph: 7.2,
            timestamp: "2024-01-01T12:00:00Z"
          },
          response: {
            success: true,
            message: "Datos guardados exitosamente",
            data: {
              id: "123e4567-e89b-12d3-a456-426614174000",
              ph: 7.2,
              timestamp: "2024-01-01T12:00:00Z",
              created_at: "2024-01-01T12:00:00.123Z"
            }
          }
        }
      },
      "GET /api/ph-data": {
        description: "Obtener últimas lecturas de pH",
        responses: {
          200: {
            success: true,
            data: "Array con las últimas 100 lecturas"
          },
          500: {
            error: "Error al obtener los datos"
          }
        },
        example: {
          response: {
            success: true,
            data: [
              {
                id: "123e4567-e89b-12d3-a456-426614174000",
                ph: 7.2,
                timestamp: "2024-01-01T12:00:00Z",
                created_at: "2024-01-01T12:00:00.123Z"
              }
            ]
          }
        }
      }
    },
    arduino_example: {
      description: "Ejemplo de código para Arduino ESP8266",
      code: `
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "TU_WIFI";
const char* password = "TU_PASSWORD";
const char* serverUrl = "https://tu-proyecto.vercel.app/api/ph-data";

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");
}

void loop() {
  float phValue = analogRead(A0) * (14.0 / 1023.0);
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    String json = "{\\"ph\\":" + String(phValue, 2) + "}";
    int httpResponseCode = http.POST(json);
    
    Serial.println("Response: " + String(httpResponseCode));
    http.end();
  }
  
  delay(30000); // 30 segundos
}
      `
    }
  }

  return NextResponse.json(apiDocs, {
    headers: {
      'Content-Type': 'application/json',
      'Access-Control-Allow-Origin': '*'
    }
  })
}
