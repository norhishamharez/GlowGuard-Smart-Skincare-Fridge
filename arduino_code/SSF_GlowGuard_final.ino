#define BLYNK_TEMPLATE_ID "TMPL6rcZsQwaX"
#define BLYNK_TEMPLATE_NAME "Smart Skincare Fridge"
#define BLYNK_AUTH_TOKEN "aAQoiFALzJC0ZvPOExH1xdXNCgw8yLFA"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h>

char ssid[] = "aisha 🧸";
char pass[] = "yukibell";

#define DHTPIN 18
#define DHTTYPE DHT11

#define FAN_RELAY_PIN 25
#define SERVO_PIN 19
#define BUZZER_PIN 23
#define WARNING_LED_PIN 32
#define INTERIOR_LIGHT_PIN 26
#define DOOR_PIN 13
#define LDR_PIN 34
#define IR_PIN 4

#define TEMP_LIMIT 33
#define LDR_THRESHOLD 400
#define DOOR_LIMIT_TIME 10000

#define SERVO_STOP 90
#define SERVO_CLOCKWISE 0
#define SERVO_ANTICLOCKWISE 180
#define SERVO_OPEN_TIME 115
#define SERVO_CLOSE_TIME 120
#define SERVO_WAIT_TIME 10000

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;
BlynkTimer timer;

bool autoMode = false;

bool servoBusy = false;
bool servoAlreadyTriggered = false;
unsigned long servoActionStartTime = 0;
int servoStep = 0;

unsigned long doorOpenStartTime = 0;
bool doorTimerStarted = false;
bool doorOpenTooLong = false;

BLYNK_WRITE(V4) {
  int fanState = param.asInt();

  if (!autoMode) {
    digitalWrite(FAN_RELAY_PIN, fanState == 1 ? LOW : HIGH);
    Serial.println(fanState == 1 ? "Fan: ON from Blynk" : "Fan: OFF from Blynk");
  }
}

BLYNK_WRITE(V6) {
  autoMode = (param.asInt() == 1);
  Serial.println(autoMode ? "Auto Mode: ON" : "Auto Mode: OFF");
}

void startServoSequence() {
  Serial.println("Object detected. Servo opening clockwise...");

  myServo.write(SERVO_CLOCKWISE);
  servoActionStartTime = millis();
  servoBusy = true;
  servoStep = 1;
  servoAlreadyTriggered = true;
}

void updateServoSequence() {
  if (!servoBusy) return;

  unsigned long currentTime = millis();

  if (servoStep == 1 && currentTime - servoActionStartTime >= SERVO_OPEN_TIME) {
    myServo.write(SERVO_STOP);
    servoActionStartTime = currentTime;
    servoStep = 2;
    Serial.println("Servo opened. Waiting 10 seconds...");
  }

  else if (servoStep == 2 && currentTime - servoActionStartTime >= SERVO_WAIT_TIME) {
    myServo.write(SERVO_ANTICLOCKWISE);
    servoActionStartTime = currentTime;
    servoStep = 3;
    Serial.println("Servo closing anti-clockwise...");
  }

  else if (servoStep == 3 && currentTime - servoActionStartTime >= SERVO_CLOSE_TIME) {
    myServo.write(SERVO_STOP);
    servoBusy = false;
    servoStep = 0;
    Serial.println("Servo closed and stopped.");
  }
}

void sendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int doorState = digitalRead(DOOR_PIN);
  bool doorOpen = (doorState == HIGH);

  int irState = digitalRead(IR_PIN);
  bool objectDetected = (irState == LOW);

  int ldrValue = analogRead(LDR_PIN);

  Serial.println("================================");

Serial.print("WiFi Status: ");
if (WiFi.status() == WL_CONNECTED) {
  Serial.println("CONNECTED");
} else {
  Serial.println("DISCONNECTED");
}

Serial.print("Blynk Status: ");
if (Blynk.connected()) {
  Serial.println("CONNECTED");
} else {
  Serial.println("DISCONNECTED");
}

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 ERROR!");
    return;
  }

  bool tempNotSuitable = (temperature > TEMP_LIMIT);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Door raw: ");
  Serial.println(doorState);

  Serial.print("Door: ");
  Serial.println(doorOpen ? "OPEN / Magnet Not Detected" : "CLOSED / Magnet Detected");

  Serial.print("IR: ");
  Serial.println(objectDetected ? "Object Detected" : "No Object");

  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V3, doorOpen ? "Open" : "Closed");
  }

  if (ldrValue > LDR_THRESHOLD) {
    digitalWrite(INTERIOR_LIGHT_PIN, HIGH);
    Serial.println("Interior Light: ON");
    if (Blynk.connected()) {
      Blynk.virtualWrite(V5, "Open");
    }
  } else {
    digitalWrite(INTERIOR_LIGHT_PIN, LOW);
    Serial.println("Interior Light: OFF");
    if (Blynk.connected()) {
      Blynk.virtualWrite(V5, "Closed");
    }
  }

  if (doorOpen) {
    if (!doorTimerStarted) {
      doorOpenStartTime = millis();
      doorTimerStarted = true;
      doorOpenTooLong = false;
      Serial.println("Door opened. Timer started.");
    }

    if (millis() - doorOpenStartTime >= DOOR_LIMIT_TIME) {
      doorOpenTooLong = true;
      Serial.println("Door open too long!");
    }
  } else {
    doorTimerStarted = false;
    doorOpenTooLong = false;
  }

  if (tempNotSuitable) {
    digitalWrite(WARNING_LED_PIN, HIGH);
    digitalWrite(FAN_RELAY_PIN, LOW);

    Serial.println("Temperature NOT suitable");
    Serial.println("Warning LED ON");
    Serial.println("Fan ON");
  } else {
    digitalWrite(WARNING_LED_PIN, LOW);

    if (!autoMode) {
      digitalWrite(FAN_RELAY_PIN, HIGH);
    }

    Serial.println("Temperature SAFE");
    Serial.println("Warning LED OFF");
  }

  if (doorOpenTooLong) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Buzzer ON: Door open too long");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Buzzer OFF");
  }

  if (Blynk.connected()) {
    Blynk.virtualWrite(V2, (tempNotSuitable || doorOpenTooLong) ? "🔴 DANGER" : "🟢 SAFE");
    Blynk.virtualWrite(V4, tempNotSuitable ? 1 : 0);
  }

  if (objectDetected && !servoBusy && !servoAlreadyTriggered) {
    startServoSequence();
  }

  if (!objectDetected && !servoBusy) {
    servoAlreadyTriggered = false;
  }

  Serial.println("================================");
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(WARNING_LED_PIN, OUTPUT);
  pinMode(INTERIOR_LIGHT_PIN, OUTPUT);
  pinMode(DOOR_PIN, INPUT_PULLUP);
  pinMode(IR_PIN, INPUT);

  digitalWrite(FAN_RELAY_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(WARNING_LED_PIN, LOW);
  digitalWrite(INTERIOR_LIGHT_PIN, LOW);

  dht.begin();

  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_STOP);

  Serial.println("Starting Smart Skincare Fridge...");

  WiFi.begin(ssid, pass);

  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 20) {
    delay(500);
    Serial.print(".");
    count++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect(3000);
  } else {
    Serial.println("\nWiFi failed. Running offline mode.");
  }

  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.run();
  }

  timer.run();
  updateServoSequence();
}
