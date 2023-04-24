#include <Arduino.h>
#include <DHT.h>
#include <EirlabPolluants.hpp>

/**
 * Note:
 * This firmware could be used on more/less anything rather than an Arduino UNO. These types of sensors need 5V IO pins for communication
 * or lights up infrared led. So the code will work without any problem, but sensors won't respond like they should.
*/

#include <main.h>

IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient client;

DHT dht(SENS_DHT11, DHT11);
EirlabPolluants sensors(SEND_POLLU, SENS_POLLU, SENS_SOUND);

void setup() {
  Serial.begin(115200);
  sensors.init();

  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(sensors.mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(sensors.mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
}

void loop() {
  sensors.set_temperature(dht.readTemperature());
  sensors.set_humidity(dht.readHumidity());

  sensors.get_density();
  sensors.get_loudness();

  sensors.publish(client);

  delay(2000);
}