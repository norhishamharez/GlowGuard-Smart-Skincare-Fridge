#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

#define BUZZER_PIN 22
#define REED_PIN 32

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(REED_PIN, INPUT_PULLUP);

  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int reedState = digitalRead(REED_PIN);

  if (isnan(temperature)  isnan(humidity)) {
    Serial.println("Failed to read from DHT11!");
    digitalWrite(BUZZER_PIN, LOW);
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  if (reedState == LOW) {
    Serial.println("Door Closed (Magnet Detected)");
  } else {
    Serial.println("Door Open (Magnet Removed)");
  }

  if (temperature > 24  reedState == HIGH) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("BUZZER ON");
  }
  else {
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("BUZZER OFF");
  }

  Serial.println("---------------------");
  delay(2000);
}
