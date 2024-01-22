#define BLYNK_TEMPLATE_ID "TMPL66VTj5dfj"
#define BLYNK_TEMPLATE_NAME "SOLVED MONITORING"
#define BLYNK_AUTH_TOKEN "q2jl-QoLYedNAkAlezt1cjFttFirUXBy"

#include <Blynk.h>
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define DHT_SENSOR_PIN 4
#define DHT_SENSOR_TYPE DHT22
#define KIPAS_PIN 17
#define EXOS_PIN 5
#define HEATER_PIN 16
#define BUTTON_PIN 18

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Alamat I2C untuk modul 20x4
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

int isFirstPress = 0;
bool lastButtonState = HIGH;
bool currentButtonState;

char ssid[] = "Galaxy A727885";
char pass[] = "ginnamon";

void setup() {
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(KIPAS_PIN, OUTPUT);
  pinMode(EXOS_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Using INPUT_PULLUP to enable internal pull-up resistor
  lcd.init();
  Serial.begin(115200);
  dht_sensor.begin();
  mlx.begin();
  
  lcd.begin(20, 4);        // Inisialisasi LCD 20x4
  lcd.backlight();         // Hidupkan pencahayaan belakang
  lcd.setCursor(7, 0);     // Set kursor ke baris 1, kolom 1
  lcd.print("SoLveD");
  
  digitalWrite(KIPAS_PIN, HIGH);
  digitalWrite(EXOS_PIN, HIGH);
  digitalWrite(HEATER_PIN, HIGH);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi");
  }
}

void loop() {
  float ambient = mlx.readAmbientTempC();
  float object = mlx.readObjectTempC();
  
  Serial.print("Ambient = ");
  Serial.print(ambient);
  Serial.print("*C\tObject = ");
  Serial.print(object);
  Serial.println("*C");
    
  lcd.setCursor(2, 2);
  lcd.print("RUANGAN = ");
  lcd.print(ambient);
  lcd.print("C");
  lcd.setCursor(2, 3);
  lcd.print("PEMANAS = ");
  lcd.print(object);
  lcd.print("C");
  currentButtonState = digitalRead(BUTTON_PIN);

if (currentButtonState == LOW && lastButtonState == HIGH) {
    isFirstPress++;
    if (isFirstPress == 1) {
      lcd.setCursor(9, 1);
      lcd.print("ON");
      Serial.println("1");
      digitalWrite(KIPAS_PIN, LOW);
      digitalWrite(EXOS_PIN, LOW);
      digitalWrite(HEATER_PIN, LOW);
    } else {
      Serial.println("0");
      lcd.setCursor(8, 1);
      lcd.print("OFF!");
      digitalWrite(KIPAS_PIN, HIGH);
      digitalWrite(EXOS_PIN, HIGH);
      digitalWrite(HEATER_PIN, HIGH);
      isFirstPress = 0;
    }
  }

  lastButtonState = currentButtonState;

  delay(1000);
  Blynk.run();
}