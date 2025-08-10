#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi credentials
const char* ssid = "Rover";
const char* password = "12345678";

// Motor pins
#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4

// Ultrasonic sensor
#define trigPin D5
#define echoPin D6

// LED
#define ledAlert D7

// Obstacle threshold
int distanceThreshold = 10; // cm
int count = 0;

String currentCommand = "stop";

ESP8266WebServer server(80);

// Function to stop all motors
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Movement functions
void moveForward() {
  if (getDistance() > distanceThreshold) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    currentCommand = "stop";
    stopMotors();
    digitalWrite(ledAlert, HIGH);
    delay(1000);
    digitalWrite(ledAlert, LOW);
  }
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void moveLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Measure distance using ultrasonic
long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout for stability
  long distance = duration * 0.034 / 2;
  Serial.println(distance);
  return distance;
}

// HTML page
String webPage = R"rawliteral(
<html>
  <head><title>Rover Control</title></head>
  <body style='text-align:center; font-family:sans-serif'>
    <h2>WiFi Rover Control</h2>
    <button onclick='location.href="/forward"' style='width:100px;height:50px;'>Forward</button><br><br>
    <button onclick='location.href="/left"' style='width:100px;height:50px;'>Left</button>
    <button onclick='location.href="/stop"' style='width:100px;height:50px;'>Stop</button>
    <button onclick='location.href="/right"' style='width:100px;height:50px;'>Right</button><br><br>
    <button onclick='location.href="/backward"' style='width:100px;height:50px;'>Backward</button>
  </body>
</html>
)rawliteral";

// Web routes
void handleRoot() {
  server.send(200, "text/html", webPage);
}

void handleForward() { 
  currentCommand = "forward";
  moveForward(); 
  server.send(200, "text/html", webPage); 
}

void handleBackward() { 
  currentCommand = "backward";
  moveBackward(); 
  server.send(200, "text/html", webPage); 
}

void handleLeft() { 
  currentCommand = "left";
  moveLeft(); 
  server.send(200, "text/html", webPage); 
}

void handleRight() { 
  currentCommand = "right";
  moveRight(); 
  server.send(200, "text/html", webPage); 
}

void handleStop() { 
  currentCommand = "stop";
  stopMotors(); 
  server.send(200, "text/html", webPage); 
}

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ledAlert, OUTPUT);
  digitalWrite(ledAlert, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP()); // Note down this IP

  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);

  server.begin();
}

void loop() {
  server.handleClient();

  // Obstacle detection only if moving forward
  if (currentCommand == "forward") {
    count++;
    if(count > 10000) {
      count = 0;
      if (getDistance() <= distanceThreshold) {
        currentCommand = "stop"; // stop further motion
        stopMotors();
        digitalWrite(ledAlert, HIGH);
        delay(1000);
        digitalWrite(ledAlert, LOW);
      }
    }
  }
  else {
    count = 0;
  }
}
