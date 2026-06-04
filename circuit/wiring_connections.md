# Wiring Connections

## ESP32 Pin Connections

| Component | Pin | ESP32 Connection |
|---|---|---|
| DHT11 | VCC | 3.3V |
| DHT11 | GND | GND |
| DHT11 | DATA | GPIO 4 |
| LDR Sensor | VCC | 3.3V |
| LDR Sensor | GND | GND |
| LDR Sensor | AO/SIGNAL | GPIO 34 |
| IR Proximity Sensor | VCC | 3.3V |
| IR Proximity Sensor | GND | GND |
| IR Proximity Sensor | OUT | GPIO 27 |
| Reed Sensor | One side | GND |
| Reed Sensor | Other side | GPIO 15 |
| Buzzer | + | GPIO 18 |
| Buzzer | - | GND |
| LED | Anode (+) | GPIO 2 through 220Ω resistor |
| LED | Cathode (-) | GND |
| Servo Motor | Signal | GPIO 13 |
| Servo Motor | VCC | 5V |
| Servo Motor | GND | GND |
| Fan | Control | Relay/Transistor via GPIO 23 |
