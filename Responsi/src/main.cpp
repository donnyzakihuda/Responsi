#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;

const char* ssid = "NARUTO SEASON 7 JANUARY";
const char* password = "bapaknyadevfor";

String apiKey = "5XWHTTEGMTM4M2C1";  // GANTI dengan API Key kalian
const int pinLED = 18;
const int pwmChannel = 0;
const int freq = 5000;
const int res = 8;
const int buzzerPin = 19;

const unsigned long intervalThingspeak = 20000;
unsigned long lastSend = 0;


void setup() {
  Serial.begin(115200);

  Wire.begin();
  mpu.begin();
  ledcSetup(pwmChannel, freq, res);
  ledcAttachPin(pinLED, pwmChannel);
  pinMode(buzzerPin, OUTPUT);
  Serial.print("Koneksi WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (millis() - lastSend >= intervalThingspeak) {
    lastSend = millis();

    float x = a.acceleration.x;
    float y = a.acceleration.y;
    float z = a.acceleration.z;

    int pinValue = analogRead(x);
    for(int dutyCycle=0; dutyCycle <= pinValue; dutyCycle++) {
      ledcWrite(pwmChannel,dutyCycle);
      delay(15);
    }
    if(x > 5) {
      digitalWrite(buzzerPin, HIGH);
      Serial.println("MIRING");
    }else {
      Serial.print("X: ");
      Serial.print(x);
      Serial.print("  | y: ");
      Serial.println(y);
      Serial.print("  | z: ");
      Serial.println(z);
      Serial.println("DATAR");
    }

    digitalWrite(pinLED, HIGH);

    HTTPClient http;
    String url = "http://api.thingspeak.com/update?api_key=" + apiKey +
                 "&field1=" + String(x) +
                 "&field2=" + String(y) + 
                 "&field3=" + String(z);

    http.begin(url);
    int httpCode = http.GET();
    String respon = http.getString();
    http.end();

    if (httpCode == 200 && respon != "0") {
      Serial.println("Data terkirim ke ThingSpeak, entry ke-" + respon);
    } else {
      Serial.println("Gagal kirim ke ThingSpeak (kode " + String(httpCode) + ")");
    }
  }
}