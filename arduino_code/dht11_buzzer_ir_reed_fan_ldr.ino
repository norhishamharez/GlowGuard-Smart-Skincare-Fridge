#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

#define BUZZER_PIN 22
#define LED_PIN 23
#define FAN_RELAY_PIN 18
#define IR_PIN 35
#define REED_PIN 32
#define LDR_PIN 34

#define TEMP_LIMIT 24
#define LDR_DARK_LIMIT 1000

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(REED_PIN, INPUT_PULLUP);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Relay usually ACTIVE LOW
  digitalWrite(FAN_RELAY_PIN, HIGH);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int irState = digitalRead(IR_PIN);
  int reedState = digitalRead(REED_PIN);
  int ldrValue = analogRead(LDR_PIN);

  bool tempHigh = false;
  bool objectDetected = (irState == LOW);
  bool doorOpen = (reedState == HIGH);
  bool darkDetected = (ldrValue < LDR_DARK_LIMIT);

  if (isnan(temperature)  isnan(humidity)) {
    Serial.println("Failed to read from DHT11!");
  } else {
    tempHigh = temperature > TEMP_LIMIT;

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  Serial.print("IR Proximity: ");
  Serial.println(objectDetected ? "Object Detected" : "No Object");

  Serial.print("Door: ");
  Serial.println(doorOpen ? "Open" : "Closed");

  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  Serial.print("Light Condition: ");
  Serial.println(darkDetected ? "Dark" : "Bright");

  if (tempHigh  objectDetected  doorOpen  darkDetected) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(FAN_RELAY_PIN, LOW); // fan ON

    Serial.println("BUZZER ON");
    Serial.println("LED ON");
    Serial.println("FAN ON");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(FAN_RELAY_PIN, HIGH); // fan OFF

    Serial.println("BUZZER OFF");
    Serial.println("LED OFF");
    Serial.println("FAN OFF");
  }

  Serial.println("---------------------");
  delay(2000);
}
