#ifndef EIRLABPOLLUANTS_H
#define EIRLABPOLLUANTS_H

#include <Arduino.h>
#include <main.h>
#include <Ethernet.h>

/**
 * @brief This class is used to monitor different sensors and send the data to an api.
 * 
 * @param __send_pin Pin connected to the LED of the smoke sensor.
 * @param __sens_pin Pin connected to the infrared receiver of the smoke sensor.
 * @param __sens_sound Pin connected to the microphone input for sound level measurement.
*/

class EirlabPolluants
{
    private:
        int send_pin;
        int sens_pin;
        int sens_sound;

        int filter_adc_value(int m);
        double matchDB(double vol);
    public:
        EirlabPolluants(int __send_pin, int __sens_pin, int __sens_sound);
        ~EirlabPolluants();
        void init( void );
        int get_density();
        void set_temperature(float __t);
        void set_humidity(float __h);
        int get_loudness(EthernetClient __client, IPAddress __server);
        void publish(EthernetClient __client);

        byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

        int ppms = 10;
        float temperature = 0;
        float humidity = 0;
        int sound = 0;

        char serverName[48] = "api.aircslab.fr";
};


#endif