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
    int raw_density;
    float voltage, density;
    digitalWrite(this->send_pin, HIGH);
    delayMicroseconds(280);
    raw_density = analogRead(this->sens_pin);
    delayMicroseconds(40);
    digitalWrite(this->send_pin, LOW);
    delayMicroseconds(9680);

    raw_density = filter_adc_value(raw_density);

    voltage = float(raw_density) * (5000 / 1024.00) * 11;

    if(voltage >= 400)
    {
        voltage -= 400;
        
        density = voltage * 0.2;
    }
    else
        density = 0;

    Serial.print("adc:");
    Serial.print(raw_density);
    Serial.print(", voltage:");
    Serial.print(voltage);
    Serial.print(", dens:");
    Serial.print(density);

    
    this->ppms = density;
    return density;
}


/**
 * @brief Since we can't have the dht class in another class (i'm to bad I guess ...) we set it through this method.
*/
void EirlabPolluants::set_temperature(float __t)
{
    this->temperature = __t;
}

/**
 * @brief Same as before.
*/
void EirlabPolluants::set_humidity(float __h)
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
int EirlabPolluants::get_loudness(EthernetClient __client, IPAddress __server)
{
    if (__client.connect(__server, 80))
    {
        __client.println("GET /sound HTTP/1.1");
        __client.println("Host: 192.168.0.113");
        __client.println("Connection: close");
        __client.println();
        Serial.println("data published.");
    } else {
        Serial.println("data can't be published.");
    }

    char buffer[64];
    int i = 0;
    while(__client.connected()) {
      if(__client.available()){
        // read an incoming byte from the server and print it to serial monitor:
        char c = __client.read();
        if (c == '\n' || c == '\r')
        {
            i = 0;
            for (int i = 0; i < 64; i++)
            {
                buffer[i] = 0;
            }
            
        }
        buffer[i] = c;
        i++;
      }
    }
    int sound;

    char sounc[2] = {buffer[12], buffer[13]};
    sound = atoi(sounc);
    if (sound > 40) {
        this->sound = sound; // Handle small error readings.
    }
    Serial.println(sound);
    __client.stop(); //stop client
    return sound;
}

/**
 * @brief Create the URL to send the data to the api through a GET request.
*/
void EirlabPolluants::publish(EthernetClient __client)
{
    char buffer[128];
    char temp_buff[6];
    char hum_buff[6];

    dtostrf(this->temperature, 4, 2, temp_buff);
    dtostrf(this->humidity, 4, 2, hum_buff);

    sprintf(buffer, "GET /api/eirlab_measurements?density=%d&level=%d&temperature=%s&humidity=%s&sound=%d HTTP/1.1", this->ppms, 0, temp_buff, hum_buff, this->sound);
    Serial.println(buffer);

    if (__client.connect(this->serverName, 80))
    {
        __client.println(buffer);
        __client.println("Host: api.aircslab.fr");
        __client.println("Connection: close");
        __client.println();
        Serial.println("data published.");
    } else {
        Serial.println("data can't be published.");
    }
    __client.stop(); //stop client
}

double EirlabPolluants::matchDB(double vol)
{
	if (vol < 150)
	{
		return 38;
	}
	if (vol < 500)
	{
		return 0.062 * vol + 39;
	}
	double temp = 0.02 * vol + 60;
	if(temp > 100) temp = 100;
	return temp;
}