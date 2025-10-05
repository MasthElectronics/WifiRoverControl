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
#define EN D8

// Ultrasonic sensor pins
#define trigPin D5
#define echoPin D6

// LED pin
#define ledAlert D7

int distanceThreshold = 10; // Obstacle detection threshold (cm)
String currentCommand = "stop";
bool isMovingForward = false;

ESP8266WebServer server(80);

int motorSpeed = 1023; // PWM speed (0-1023 for ESP8266)

// Stop all motors
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Move forward with obstacle check
void moveForward() {
  long distance = getDistance();
  if (distance == 0 || distance > 400) {
    Serial.print("Ignored distance reading: ");
    Serial.println(distance);
    return;
  }
  if (distance > distanceThreshold) {
    analogWrite(EN, motorSpeed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    digitalWrite(ledAlert, LOW);
  } else {
    currentCommand = "stop";
    stopMotors();
    digitalWrite(ledAlert, HIGH);
    delay(1000);
    digitalWrite(ledAlert, LOW);
    isMovingForward = false;
  }
}

// Move backward
void moveBackward() {
  analogWrite(EN, motorSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Turn left
void moveLeft() {
  analogWrite(EN, motorSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Turn right
void moveRight() {
  analogWrite(EN, motorSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Measure distance using ultrasonic sensor
long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  Serial.println(distance);
  return distance;
}

// HTML page for rover control
String webPage = R"rawliteral(
<html>
  <head>
    <title>Rover Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        text-align:center;
        font-family:sans-serif;
        font-size:1.2em;
        margin:0;
        padding:0;
        background: #f5f7fa;
      }
      h2 {
        margin-top: 20px;
        color: #1976d2;
        letter-spacing: 2px;
        text-shadow: 1px 1px 2px #0001;
      }
      .controls {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: 18px;
        margin-top: 30px;
        background: #fff;
        border-radius: 18px;
        box-shadow: 0 4px 24px #0001;
        padding: 24px 0 32px 0;
        max-width: 480px;
        margin-left: auto;
        margin-right: auto;
      }
      .row {
        display: flex;
        gap: 18px;
        justify-content: center;
      }
      .direction-btn {
        background: #43a047;
        color: white;
        border: none;
        border-radius: 12px;
        font-size: 1.1em;
        font-weight: bold;
        width: 28vw; max-width: 140px; min-width: 80px;
        height: 10vw; max-height: 60px; min-height: 40px;
        box-shadow: 0 2px 8px #0002;
        transition: background 0.2s;
        margin: 0 2px;
      }
      .direction-btn:active { background: #388e3c; }
      .speed-btn {
        background: #fbc02d;
        color: #222;
        border: none;
        border-radius: 12px;
        font-size: 1.1em;
        font-weight: bold;
        width: 24vw; max-width: 110px; min-width: 70px;
        height: 9vw; max-height: 50px; min-height: 35px;
        box-shadow: 0 2px 8px #0002;
        transition: background 0.2s;
        margin: 0 2px;
      }
      .speed-btn:active { background: #f9a825; }
      .slider-row {
        margin: 32px 0 0 0;
        width: 100%;
      }
      .slider-label {
        margin-bottom: 8px;
        font-weight: bold;
        color: #1976d2;
        font-size: 1.05em;
      }
      .slider-container {
        display: flex;
        flex-direction: column;
        align-items: center;
        width: 100%;
      }
      #thresholdSlider {
        width: 90%;
        max-width: 500px;
        min-width: 180px;
        margin: 0 0 8px 0;
      }
      #thresholdValue {
        display: inline-block;
        margin-left: 10px;
        font-weight: bold;
        color: #388e3c;
        font-size: 1.1em;
      }
      button {
        overflow: hidden;
        text-overflow: ellipsis;
        white-space: nowrap;
        touch-action: manipulation;
        outline: none;
      }
      button:focus {
        box-shadow: 0 0 0 2px #1976d2;
      }
      @media (max-width: 600px) {
        .direction-btn, .speed-btn { font-size: 1em; }
        #thresholdSlider { width: 98vw; }
        .controls { max-width: 98vw; }
      }
    </style>
    <script>
      function sendCommand(cmd) {
        fetch('/' + cmd);
      }
      function setupButton(id, cmd) {
        var btn = document.getElementById(id);
        btn.addEventListener('mousedown', function() { sendCommand(cmd); });
        btn.addEventListener('touchstart', function(e) { sendCommand(cmd); e.preventDefault(); });
        btn.addEventListener('mouseup', function() { sendCommand('stop'); });
        btn.addEventListener('mouseleave', function() { sendCommand('stop'); });
        btn.addEventListener('touchend', function() { sendCommand('stop'); });
      }
      function setThreshold(val) {
        document.getElementById('thresholdValue').innerText = val + " cm";
        fetch('/setThreshold?value=' + val);
      }
      window.onload = function() {
        setupButton('forwardBtn', 'forward');
        setupButton('leftBtn', 'left');
        setupButton('rightBtn', 'right');
        setupButton('backwardBtn', 'backward');
        document.getElementById('thresholdSlider').value = %THRESHOLD%;
        document.getElementById('thresholdValue').innerText = %THRESHOLD% + " cm";
      }
    </script>
  </head>
  <body>
    <h2>WiFi Rover Control</h2>
    <div class="controls">
      <div class="row">
        <button id="forwardBtn" class="direction-btn">Forward</button>
      </div>
      <div class="row">
        <button id="leftBtn" class="direction-btn">Left</button>
        <button id="rightBtn" class="direction-btn">Right</button>
      </div>
      <div class="row">
        <button id="backwardBtn" class="direction-btn">Backward</button>
      </div>
      <div class="row">
        <button onclick='sendCommand("turbo")' class="speed-btn">Turbo</button>
        <button onclick='sendCommand("normal")' class="speed-btn">Normal</button>
        <button onclick='sendCommand("slow")' class="speed-btn">Slow</button>
      </div>
      <div class="slider-row">
        <div class="slider-label">Obstacle Threshold</div>
        <div class="slider-container">
          <input type="range" id="thresholdSlider" min="2" max="100" value="%THRESHOLD%" oninput="setThreshold(this.value)">
          <span id="thresholdValue"></span>
        </div>
      </div>
    </div>
  </body>
</html>
)rawliteral";

// Web routes
void handleRoot() {
  String page = webPage;
  page.replace("%THRESHOLD%", String(distanceThreshold));
  server.send(200, "text/html", page);
}

void handleForward() { 
  currentCommand = "forward";
  isMovingForward = true;
  server.send(200, "text/html", "");
}

void handleBackward() { 
  currentCommand = "backward";
  moveBackward(); 
  server.send(200, "text/html", "");
}

void handleLeft() { 
  currentCommand = "left";
  moveLeft(); 
  server.send(200, "text/html", "");
}

void handleRight() { 
  currentCommand = "right";
  moveRight(); 
  server.send(200, "text/html", "");
}

void handleStop() { 
  currentCommand = "stop";
  isMovingForward = false;
  stopMotors(); 
  server.send(200, "text/html", "");
}

void handleTurbo() { motorSpeed = 1023; server.send(200, "text/html", ""); }
void handleNormal() { motorSpeed = 200; server.send(200, "text/html", ""); }
void handleSlow() { motorSpeed = 100; server.send(200, "text/html", ""); }

void handleSetThreshold() {
  if (server.hasArg("value")) {
    int newThreshold = server.arg("value").toInt();
    if (newThreshold >= 2 && newThreshold <= 100) {
      distanceThreshold = newThreshold;
    }
  }
  server.send(200, "text/html", "");
}

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ledAlert, OUTPUT);
  digitalWrite(ledAlert, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  server.on("/turbo", handleTurbo);
  server.on("/normal", handleNormal);
  server.on("/slow", handleSlow);
  server.on("/setThreshold", handleSetThreshold);

  server.begin();
}

void loop() {
  server.handleClient();

  // Continuously check for obstacle while moving forward
  if (isMovingForward) {
    moveForward();
  }
}
