#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ESP32Servo.h>

// Configuración de red WiFi
const char* ssid = "NOMBRE_WIFI";
const char* password = "CLAVe_WIFI";

// Configuración de pines
const int servoPin1 = 14;
const int motorPin = 27;
const int ledPin = 2;

Servo servo1;

bool listening = true;

// Inicialización del servidor WebSocket en el puerto 81
WebSocketsServer webSocket(81);

// Función para manejar comandos
void handleCommand(String command) {
  String hw = "";
  String id = "";
  String action = "";

  int hwIndex = command.indexOf("hw=");
  int idIndex = command.indexOf("id=");
  int atIndex = command.indexOf("at=");

  if (hwIndex != -1) hw = command.substring(hwIndex + 3, command.indexOf("&", hwIndex + 3));
  if (idIndex != -1) id = command.substring(idIndex + 3, command.indexOf("&", idIndex + 3));
  if (atIndex != -1) action = command.substring(atIndex + 3, command.length());

  Serial.println("cmd-"+hw+"-id-"+id+"-at-"+action);

  // Si el ESP32 está en modo de escucha, procesamos los comandos
  if (listening) {
    if (hw == "srv") {
      if (id == "01") {
        // Convertir la accion (que es un string) a un entero (grados)
        int grado = action.toInt();  
        if (grado >= 0 && grado <= 180) {
          servo1.write(grado);
          Serial.print("Servo 1 movido a: ");
          Serial.println(grado);
        } else {
          Serial.println("Grado fuera de rango (0-180)");
        }      
      } else if (id == "02") {
        // Convertir la accion (que es un string) a un entero (grados)
        int grado = action.toInt();  
        if (grado >= 0 && grado <= 180) {
          servo1.write(grado);
          Serial.print("Servo 1 movido a: ");
          Serial.println(grado);
        } else {
          Serial.println("Grado fuera de rango (0-180)");
        } 
      }
    } else if (hw == "mtr") {
      if (id == "01") {
        if (action == "001") {
          digitalWrite(motorPin, HIGH);
          Serial.println("Motor iniciado");
        } else if (action == "000") {
          digitalWrite(motorPin, LOW);
          Serial.println("Motor detenido");
        }        
      } else if (id == "02") {
      
      }
    } else if (hw == "led" && (id == "01")) {
      if (id == "01") {
        if (action == "001") {
          digitalWrite(ledPin, HIGH);
          Serial.println("LED iniciado");
        } else if (action == "000") {
          digitalWrite(ledPin, LOW);
          Serial.println("LED detenido");
        }        
      }
    } else {
      Serial.println("Comando no válido");
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char*)payload);
    Serial.println("Mensaje recibido: " + message);
    handleCommand(message);
    webSocket.sendTXT(num, "Comando procesado: " + message);
  }
}

void setup() {
  Serial.begin(115200);

  // Configuración de pines
  pinMode(motorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  servo1.setPeriodHertz(50);
  servo1.attach(servoPin1);

  digitalWrite(motorPin, LOW);
  digitalWrite(ledPin, LOW);

  // Conexión a WiFi
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConexión establecida");
  Serial.println("Dirección IP: " + WiFi.localIP().toString());

  // Iniciar el servidor WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("Servidor WebSocket iniciado.");
}

void loop() {
  if (listening) {
    // Procesar mensajes WebSocket
    webSocket.loop(); 
  } else {
     // Si no estamos escuchando, espera
    delay(1000);
  }
}
