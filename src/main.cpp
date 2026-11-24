#include <Arduino.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>

namespace {
constexpr uint8_t SENSOR_PIN = 8;
constexpr uint8_t RELAY_PIN = 7;
constexpr float UPPER_TEMPERATURE_LIMIT = 33.0;
constexpr float LOWER_TEMPERATURE_LIMIT = 30.0;
constexpr bool RELAY_ACTIVE_LOW = true;
constexpr uint8_t LCD_ADDRESS = 0x27;

OneWire oneWire(SENSOR_PIN);
DallasTemperature temperatureSensor(&oneWire);
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

void setRelay(bool on) {
  const uint8_t onLevel = RELAY_ACTIVE_LOW ? LOW : HIGH;
  digitalWrite(RELAY_PIN, on ? onLevel : (onLevel == LOW ? HIGH : LOW));
}

void displayTemperature(float temperatureC, bool sensorAvailable) {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  if (sensorAvailable) {
    lcd.print(temperatureC, 1);
    lcd.print((char)223);
    lcd.print("C");
  } else {
    lcd.print("Error");
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("BA:33C BB:30C");
}
} 

void setup() {
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);
  pinMode(RELAY_PIN, OUTPUT);

  temperatureSensor.begin();
  lcd.init();
  lcd.backlight();
  displayTemperature(0.0, false);
}

void loop() {
  temperatureSensor.requestTemperatures();
  const float temperatureC = temperatureSensor.getTempCByIndex(0);
  const bool sensorAvailable = temperatureC != DEVICE_DISCONNECTED_C && !isnan(temperatureC);

  if (!sensorAvailable) {
    setRelay(false);
  } else if (temperatureC >= UPPER_TEMPERATURE_LIMIT) {
    setRelay(true);
  } else if (temperatureC <= LOWER_TEMPERATURE_LIMIT) {
    setRelay(false);
  }

  displayTemperature(temperatureC, sensorAvailable);
  delay(1000);
}
