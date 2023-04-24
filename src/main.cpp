#include <Arduino.h>
#include <EirlabPolluants.hpp>
#include <Ticker.h>

// https://github.com/beegee-tokyo/DHTesp/blob/master/examples/DHT_ESP32/DHT_ESP32.ino

/**
 * TODO :
 * - Mettre en place la connexion WiFi
 * - Tester les capteurs de son et de temperature/humidity
 * - Envoyer les datas sur l'api aiRCS Lab
*/

#include <main.h>

EirlabPolluants sensors;

Ticker periodicDensity;
Ticker periodicTemperature;
Ticker periodicHumidity;
Ticker periodicSound;
Ticker periodicPublisher;

void density() {
  Serial.print("density : ");
  Serial.println(sensors.get_density(), 2);
}

void temperature() {
  Serial.print("temperature : ");
  Serial.println(sensors.get_temperature());
}

void humidity() {
  Serial.print("humidity : ");
  Serial.println(sensors.get_humidity());
}

void sound() {
  Serial.print("sound : ");
  Serial.println(sensors.get_loudness());
}

void setup() {
  sensors.init();

  periodicDensity.attach(2, density);
  delay(500);
  periodicTemperature.attach(2, temperature);
  delay(500);
  periodicHumidity.attach(2, humidity);
  delay(500);
  periodicSound.attach(2, sound);
}

unsigned long time_1 = 0;
unsigned long time_2;

void loop() {
  time_2 = millis();
  if ((time_2 - time_1) > 10000) {
    sensors.publish();
    time_1 = millis();
  }
}