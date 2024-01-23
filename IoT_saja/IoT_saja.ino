#define BLYNK_TEMPLATE_ID "TMPL66VTj5dfj"
#define BLYNK_TEMPLATE_NAME "SOLVED MONITORING"
#define BLYNK_AUTH_TOKEN "q2jl-QoLYedNAkAlezt1cjFttFirUXBy"

#include <Blynk.h>
#define BLYNK_PRINT Serial
#define BUTTON_PIN2 V2
// #define HOURS V3
// #define MINUTES V4
// #define SECONDS V5

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <LiquidCrystal_I2C.h>

#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define EXOS_PIN 4
#define HEATER_PIN 16
#define BUTTON_PIN 5
#define RST_PIN 17

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Alamat I2C untuk modul 20x4
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

int isFirstPress = 0;
int buttonState = 0;

bool lastButtonState = HIGH;
bool currentButtonState;
bool refresh = false;
bool isDeviceOn = false;

char ssid[] = "Galaxy A727885";
char pass[] = "ginnamon";
unsigned long startTime = 0;

BLYNK_WRITE(BUTTON_PIN2){
  buttonState = param.asInt();
}

void setup() {
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(EXOS_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RST_PIN, INPUT_PULLUP); // Using INPUT_PULLUP to enable internal pull-up resistor
  lcd.init();
  Serial.begin(115200);
  mlx.begin();
  
  lcd.begin(20, 4);        // Inisialisasi LCD 20x4
  lcd.backlight();         // Hidupkan pencahayaan belakang
  lcd.setCursor(7, 0);     // Set kursor ke baris 1, kolom 1
  lcd.print("SoLveD");
  
  
  digitalWrite(EXOS_PIN, HIGH);
  digitalWrite(HEATER_PIN, HIGH);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi");
  }
  startTime = millis();

}

void loop() {
  float ambient = mlx.readAmbientTempC();
  float object = mlx.readObjectTempC();
  int hours,minutes,seconds;
  
  if (digitalRead(RST_PIN) == LOW) {
    refresh = true;
  }
  if (refresh) {
      Serial.println("System refreshed");
      delay(1000);
      ESP.restart();
      refresh = false;
  }

  Serial.print("Ambient = ");
  Serial.print(ambient);
  Serial.print("*C\tObject = ");
  Serial.print(object);
  Serial.println("*C");
  Serial.println(isFirstPress);
    
  lcd.setCursor(3, 2);
  lcd.print("SUHU = ");
  lcd.print(object);
  lcd.print(" C");
  currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState == LOW && lastButtonState == HIGH) {
    isFirstPress++;
  } 

  if (isFirstPress == 1 || buttonState == HIGH){
    isDeviceOn = true;
  }
  else if (isFirstPress == 2 || buttonState == LOW){
    isDeviceOn = false;
    isFirstPress = 0;
  }

  if (isDeviceOn) {
    digitalWrite(EXOS_PIN, LOW);
    digitalWrite(HEATER_PIN, LOW);
  } else {
    digitalWrite(EXOS_PIN, HIGH);
    digitalWrite(HEATER_PIN, HIGH);
    }
  
  if (isDeviceOn){
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    hours = elapsedTime / 3600000;
    minutes = (elapsedTime % 3600000) / 60000;
    seconds = ((elapsedTime % 3600000) % 60000) / 1000;
    lcd.clear();
    lcd.setCursor(7, 0);     // Set kursor ke baris 1, kolom 1
    lcd.print("SoLveD");
    lcd.setCursor(9, 1);
    lcd.print("ON");
    lcd.setCursor(3, 2);
    lcd.print("SUHU = ");
    lcd.print(object);
    lcd.print(" C");
    lcd.setCursor(2, 3);
    lcd.print("WAKTU = ");
    lcd.print(hours);
    lcd.print(":");
    lcd.print(minutes);
    lcd.print(":");
    lcd.print(seconds);
  }
  else {
    lcd.clear();
    startTime = millis();
    lcd.setCursor(7, 0);     // Set kursor ke baris 1, kolom 1
    lcd.print("SoLveD");
    lcd.setCursor(8, 1);
    lcd.print("OFF!");
    lcd.setCursor(3, 2);
    lcd.print("SUHU = ");
    lcd.print(object);
    lcd.print(" C");
    lcd.setCursor(2, 3);
    lcd.print("WAKTU = 00:00:00");
  }

  lastButtonState = currentButtonState;

  Blynk.virtualWrite(V0, object);
  // Blynk.virtualWrite(V1, ambient);
  Blynk.virtualWrite(V3, hours);
  Blynk.virtualWrite(V4, minutes);
  Blynk.virtualWrite(V5, seconds);
  // Blynk.virtualWrite(V2, isFirstPressBlynk);

  delay(1000);
  Blynk.run();
}