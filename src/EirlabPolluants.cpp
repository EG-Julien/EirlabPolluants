#include <EirlabPolluants.hpp>

EirlabPolluants::EirlabPolluants(int __send_pin, int __sens_pin, int __sens_sound)
{
    this->send_pin = __send_pin;
    this->sens_pin = __sens_pin;
    this->sens_sound = __sens_sound;
}

EirlabPolluants::~EirlabPolluants()
{
    ;
}


/**
 * @brief Initialisation function of the class. Sets pinMode for all the pins and initiate peripherals if needed.
*/
void EirlabPolluants::init( void )
{
    Serial.begin(9600);
    Serial.println("Started");
    pinMode(this->send_pin, OUTPUT);
    pinMode(this->sens_pin, INPUT);
    pinMode(this->sens_sound, INPUT);
}

/**
 * @brief Let us measure how much ppm of 2.5µm particule
*/
int EirlabPolluants::get_density( void )
{
    int density;
    digitalWrite(this->send_pin, HIGH);
    delayMicroseconds(280);
    density = analogRead(this->sens_pin);
    delayMicroseconds(40);
    digitalWrite(this->send_pin, LOW);
    delayMicroseconds(9680);

    density = (((density / 1024.00) - 0.0356) * 120000.00 * 0.035);
    if (density < 0)
        density = 0;
    
    this->ppms = density;
    return density;
}


/**
 * @brief Since we can't have the dht class in another class (i'm to bad I guess ...) we set it through this method.
*/
void EirlabPolluants::set_temperature( double __t )
{
    this->temperature = __t;
}

/**
 * @brief Same as before.
*/
void EirlabPolluants::set_humidity( double __h )
{
    this->humidity = __h;
}

/**
 * @brief Not used. Let us filter a value coming from the adc. We should try this on the microphone input. Instead of peak detection as we do for now.
*/
int EirlabPolluants::filter_adc_value(int m)
{
    static int flag_first = 0, _buff[10], sum;
    const int _buff_max = 10;
    int i;
    
    if(flag_first == 0)
    {
        flag_first = 1;

        for(i = 0, sum = 0; i < _buff_max; i++)
        {
        _buff[i] = m;
        sum += _buff[i];
        }
        return m;
    }
    else
    {
        sum -= _buff[0];
        for(i = 0; i < (_buff_max - 1); i++)
        {
        _buff[i] = _buff[i + 1];
        }
        _buff[9] = m;
        sum += _buff[9];
        
        i = sum / 10.0;
        return i;
    }
}


/**
 * @brief Measure ambiant sound level by detecting peaks over 128 measurements. Conversion is made with some kind of LUT.
*/
int EirlabPolluants::get_loudness( void )
{
    int sound, raw_sound;
    int avg_sound = 0;
    // Performs 128 signal readings   
    for ( int i = 0 ; i < 128; i ++)  
    {  
        raw_sound = analogRead(this->sens_sound);  
        if (raw_sound > avg_sound)
        avg_sound = raw_sound;
    }  
    sound = (avg_sound + 83.20) / 2;
    if (sound > 110)
        sound = 110;
    
    this->sound = sound;
    return sound;
}

/**
 * @brief Create the URL to send the data to the api through a GET request.
*/
void EirlabPolluants::publish(EthernetClient __client)
{
    char buffer[128];
    sprintf(buffer, "%s?density=%d&level=%d&temperature=%lf&humidity=%lf&sound=%d", this->serverName, this->ppms, 0, this->temperature, this->humidity, this->sound);
    Serial.println(buffer);

    if (__client.connect(buffer, 80))
    {
        Serial.println("data published.");
    } else {
        Serial.println("data can't be published.");
    }
}