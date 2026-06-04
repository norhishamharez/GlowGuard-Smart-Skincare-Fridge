#define BLYNK_TEMPLATE_ID "TMPL6rcZsQwaX"
#define BLYNK_TEMPLATE_NAME "Smart Skincare Fridge"
#define BLYNK_AUTH_TOKEN "aAQoiFALzJC0ZvPOExH1xdXNCgw8yLFA"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "ena";
char pass[] = "Mama234288";

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void sendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT11");
    return;
  }

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);

  if (temperature <= 12 && humidity <= 60) {
    Blynk.virtualWrite(V2, "🟢 SAFE");
  } else if (temperature <= 15 && humidity <= 70) {
    Blynk.virtualWrite(V2, "🟡 WARNING");
  } else {
    Blynk.virtualWrite(V2, "🔴 DANGER");
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.println(humidity);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
