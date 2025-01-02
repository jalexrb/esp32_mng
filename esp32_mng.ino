#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ESP32Servo.h>

// Configuración de red WiFi
const char* ssid = "NOMBRE_WIFI";
const char* password = "CLAVe_WIFI";

// Configuración de pines
const int ledPin = 2;

const int servoPin1 = 23;
const int servoPin2 = 22;
const int servoPin3 = 1;
const int servoPin4 = 3;

// Motor A
int motor1Pin1 = 32; 
int motor1Pin2 = 33; 
int motor1EnablePin = 25;
int motor1PwmChannel = 0; 

// Motor B
int motor2Pin1 = 26; 
int motor2Pin2 = 27; 
int motor2EnablePin = 14;
int motor2PwmChannel = 1; 

// Configuración de PWM
const int freq = 30000;
const int resolution = 8;
int dutyCycle = 210;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

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
  if (atIndex != -1) action = command.substring(atIndex + 3);

  Serial.println("cmd-" + hw + "-id-" + id + "-at-" + action);

  if (hw == "srv") {
    int grado = action.toInt();  
    if (grado >= 0 && grado <= 180) {
      if (id == "01") {
        servo1.write(grado);
        Serial.print("Servo 1 movido a: ");
      } else if (id == "02") {
        servo2.write(grado);
        Serial.print("Servo 2 movido a: ");
      } else if (id == "03") {
        servo3.write(grado);
        Serial.print("Servo 3 movido a: ");
      } else if (id == "04") {
        servo4.write(grado);
        Serial.print("Servo 4 movido a: ");
      } else {
        Serial.println("ID de servo no válido.");
      }
      Serial.println(grado);
    } else {
      Serial.println("Grado fuera de rango (0-180)");
    }      
  } else if (hw == "mtr") {
    if (action == "000") motorsStop();
    else if (action == "001") motorsForward();
    else if (action == "002") motorsBackward();
    else if (action == "003") motorsLeft();
    else if (action == "004") motorsRight();
    else Serial.println("Acción de motor no válida.");
  } else if (hw == "led" && id == "01") {
    if (action == "001") digitalWrite(ledPin, HIGH);
    else if (action == "000") digitalWrite(ledPin, LOW);
    else Serial.println("Acción de LED no válida.");
  } else {
    Serial.println("Comando no válido");
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
  pinMode(ledPin, OUTPUT);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor1EnablePin, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(motor2EnablePin, OUTPUT);

  // configure LEDC PWM
  ledcAttachChannel(motor1EnablePin, freq, resolution, motor1PwmChannel);
  ledcAttachChannel(motor2EnablePin, freq, resolution, motor2PwmChannel);

  ledcWrite(motor1EnablePin, dutyCycle);
  ledcWrite(motor2EnablePin, dutyCycle);

  digitalWrite(ledPin, LOW);

  servo1.setPeriodHertz(50);
  servo1.attach(servoPin1);
  servo2.setPeriodHertz(50);
  servo2.attach(servoPin2);
  servo3.setPeriodHertz(50);
  servo3.attach(servoPin3);
  servo4.setPeriodHertz(50);
  servo4.attach(servoPin4);

  motorsStop();

  // Conexión a WiFi
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  int timeout = 20; // 20 segundos de espera máxima
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(1000);
    Serial.print(".");
    timeout--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConexión establecida");
    Serial.println("Dirección IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nNo se pudo conectar a WiFi.");
  }

  // Iniciar el servidor WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("Servidor WebSocket iniciado.");
}

void loop() {
  webSocket.loop();
}

void motorsStop() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}

void motorsForward() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void motorsBackward() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void motorsLeft() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void motorsRight() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}
