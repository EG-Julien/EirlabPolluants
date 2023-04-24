#ifndef EIRLABPOLLUANTS_H
#define EIRLABPOLLUANTS_H

#include <Arduino.h>
#include <main.h>

class EirlabPolluants
{
    private:
        int ppms = 10;
        double temperature = 0;
        double humidity = 0;
        int sound = 0;

        const char* ssid = "aiRCS_Lab";
        const char* password = "Cafsouris220";

        char serverName[48] = "https://api.aircslab.fr/api/eirlab_measurements";

        DHTesp dht;
        TaskHandle_t SoundMeter;
        TaskHandle_t Publisher;

        int filter_adc_value(int m);
    public:
        EirlabPolluants();
        ~EirlabPolluants();
        void init( void );
        int get_density();
        double get_temperature();
        double get_humidity();
        int get_loudness( void );
        void publish();
};


#endif