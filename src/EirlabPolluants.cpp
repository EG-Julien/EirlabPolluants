#include <EirlabPolluants.hpp>

EirlabPolluants::EirlabPolluants(/* args */)
{
    ;
}

EirlabPolluants::~EirlabPolluants()
{
    ;
}

void EirlabPolluants::init( void )
{
    Serial.begin(9600);
    Serial.println("Started");
    pinMode(SEND_POLLU, OUTPUT);
    pinMode(SENS_POLLU, INPUT);
    pinMode(SENS_SOUND, INPUT);

    pinMode(SENS_DHT11, INPUT_PULLUP);

    WiFi.begin(this->ssid, this->password);

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    dht.setup(SENS_DHT11, DHTesp::DHT11);
}

int EirlabPolluants::get_density( void )
{
    digitalWrite(SEND_POLLU, HIGH);
    delayMicroseconds(280);
    int adc_value = analogRead(SENS_POLLU);
    digitalWrite(SEND_POLLU, LOW);

    //Serial.printf("raw_adc_pollu = %d\n", adc_value);

    int density;
    double voltage = (SYS_VOLTAGE / 4096.00) * adc_value * 11;

    if (voltage >= NO_DUST_VOLTAGE) {
        voltage -= NO_DUST_VOLTAGE;
        density = voltage * COV_RATIO;
    }

    if (density != 0)
        this->ppms = density;

    return density;
}

double EirlabPolluants::get_temperature( void )
{
    TempAndHumidity th = this->dht.getTempAndHumidity();

    if (this->dht.getStatus() != 0) {
		return false;
	}

    this->temperature = th.temperature - 2.00;

    return th.temperature;
}

double EirlabPolluants::get_humidity( void )
{
    TempAndHumidity th = this->dht.getTempAndHumidity();

    if (this->dht.getStatus() != 0) {
		return false;
	}

    this->humidity = th.humidity;

    return th.humidity;
}

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

int EirlabPolluants::get_loudness( void )
{
    int db = 0;
    unsigned long startMillis;
    float average;
    float sample;
    
    startMillis = millis();
    average = 0;

    while (millis() - startMillis < 50) // last is window's width
    {
        sample = analogRead(SENS_SOUND);
        average = (average + sample) / 2.00;
    }

    db = map(average, 20, 3600, 49, 120);
    this->sound = db;

    return db;
}

void EirlabPolluants::publish()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        if ((this->ppms != 0) && (this->temperature != 0) && (this->humidity != 0) && (this->sound != 0))
        {
            char buffer[128];
            sprintf(buffer, "%s?density=%d&level=%d&temperature=%lf&humidity=%lf&sound=%d", this->serverName, this->ppms, 0, this->temperature, this->humidity, this->sound);
            
            http.begin(buffer);
            int response = http.GET();
        }  
    }
}