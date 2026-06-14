#include <DHT.h>
#include <ESP32Servo.h>

#define DHTPIN 4
#define DHTTYPE DHT11

#define BUZZER_PIN 22
#define LED_PIN 23
#define FAN_RELAY_PIN 18
#define IR_PIN 35
#define SERVO_PIN 19

#define TEMP_LIMIT 24

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;

bool servoAlreadyMoved = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("System Starting...");

  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, HIGH); // fan OFF, active LOW relay

  myServo.attach(SERVO_PIN);
  myServo.write(90); // stop/center

  Serial.println("Setup Done.");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int irState = digitalRead(IR_PIN);
  bool objectDetected = (irState == LOW);

  Serial.println("---------------------");

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT11!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  Serial.print("IR State: ");
  Serial.println(irState);

  Serial.print("Object: ");
  Serial.println(objectDetected ? "Detected" : "Not Detected");

  // LED, buzzer, fan ON only if temp above 24
  if (!isnan(temperature) && temperature > TEMP_LIMIT) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(FAN_RELAY_PIN, LOW); // fan ON

    Serial.println("TEMP HIGH: BUZZER ON, LED ON, FAN ON");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(FAN_RELAY_PIN, HIGH); // fan OFF

    Serial.println("TEMP NORMAL: BUZZER OFF, LED OFF, FAN OFF");
  }

  // Servo moves only when object is detected
  if (objectDetected && servoAlreadyMoved == false) {
    Serial.println("Object detected: Servo moving...");

    myServo.write(100);   // outward
    delay(700);

    myServo.write(90);    // stop/center
    delay(1000);

    myServo.write(80);    // inward
    delay(700);

    myServo.write(90);    // stop

    servoAlreadyMoved = true;
    Serial.println("Servo movement complete.");
  }

  if (!objectDetected) {
    servoAlreadyMoved = false;
  }

  delay(2000);
}
