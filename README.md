# WiFi Rover Control 🚗📡

A NodeMCU (ESP8266)-based WiFi-controlled rover with obstacle detection using an ultrasonic sensor.
Control the rover from your phone or computer through a simple web interface — no mobile app required!

## 📹 Full Tutorial Videos

- **Basic Version:**  
  ▶ https://youtu.be/2LIX3Oo7ABs?si=gYh67pYeaMk8LkcX

- **Enhanced Version:**  
  ▶ [New Features & Web UI](https://youtu.be/SX6liczncKA?si=QMpjcPTlP6debbfq)

## ✨ Features

* WiFi-based rover control from any browser.
* Forward, backward, left, right, and stop commands.
* Automatic obstacle detection and stopping.
* LED alert when obstacle is detected.

## 🛠 Hardware Used

* NodeMCU ESP8266
* L293D Motor Driver (or similar)
* 2 DC Motors with wheels (N20 micro metal gear motor)
* Ultrasonic Sensor (HC-SR04)
* LED (for obstacle alert)
* Chassis, wiring, and power supply (Battery)

---

## 📁 Project Structure

- `WifiRoverControl/`  
  - Original version:  
    - Circuit diagram: `WifiRoverControlCircuitDiagram.png`  
    - Code: `WifiRoverControl.ino`
- `WifiRoverControlEnhanced/`  
  - Enhanced version with new web features:  
    - Circuit diagram: `WifiRoverControlEnhancedCircuitDiagram.png`  
    - Code: `WifiRoverControlEnhanced.ino`
