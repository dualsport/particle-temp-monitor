/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "c:/Users/Conrad/source/Particle/Photon/temp-monitor/src/temp-monitor.ino"
// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT_Particle.h>

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

void setup();
void loop();
int current(String unit);
#line 7 "c:/Users/Conrad/source/Particle/Photon/temp-monitor/src/temp-monitor.ino"
#define DHTPIN D4     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11     // DHT 11 
#define DHTTYPE DHT22       // DHT 22 (AM2302)
//#define DHTTYPE DHT21     // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

// Repeat time in seconds
// Example 900 will repeat every 15 minutes at :00, :15, :30, :45
const int period = 1800;

// time sync interval in seconds
// simple interval, repeat every n seconds
const int sync_interval = 43200;
time_t next_sync;

time_t current_time;
time_t next_read;

int attempts = 0;


void setup() {
    Particle.publish("status", "start", PRIVATE);
    Particle.function("current_conditions", current);
    dht.begin();

    current_time = Time.now();
    next_read = current_time - (current_time % period) + period;
    next_sync = current_time + sync_interval;
    delay(2000);
}

void loop() {
    current_time = Time.now();
    if(current_time >= next_read) {
        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old'
        float t_c = dht.getTempCelcius();
        float dp_c = dht.getDewPoint();
        float h = dht.getHumidity();

        // Check if any reads failed and if so try again.
        if (isnan(h) || isnan(t_c) || isnan(dp_c)) {
            Particle.publish("status", "read failed", PRIVATE);
            if (attempts < 3) {
                // Wait a moment then try reading again
                delay(5000);
                attempts++;
            }
            else {
                //give up, try again next period
                next_read = current_time - (current_time % period) + period;
            }
        }
        else {
            String result = String::format("{\"Temp_C\": %4.2f, \"DewPt_C\": %4.2f, \"RelHum\": %4.2f}", t_c, dp_c, h);
            Particle.publish("readings", result, PRIVATE);
            delay(1000);

            // Alternately publish as separate values
            // Particle.publish("temp_c", String::format("%4.2f", t_c), PRIVATE);
            // delay(1000);
            // Particle.publish("dewpt_c", String::format("%4.2f", dp_c), PRIVATE);
            // delay(1000);
            // Particle.publish("humidity", String::format("%4.2f", h), PRIVATE);

            next_read = current_time - (current_time % period) + period;
            attempts = 0;
        }
    }
    // sync time
    if(current_time >= next_sync) {
        Particle.syncTime();
        delay(5000);
        if (Particle.syncTimePending()) {
            Particle.publish("status", "time sync failed", PRIVATE);
        }
        else {
            current_time = Time.now();
            next_sync = current_time + sync_interval;
            Particle.publish("status", "time sync success", PRIVATE);
        }
    }
}

int current(String unit) {
    String result = "Invalid unit given. Allowed units are 'celsius' or 'fahrenheit'.";
    if (unit == "celsius") {
        float t_c = dht.getTempCelcius();
        float dp_c = dht.getDewPoint();
        float h = dht.getHumidity();
        result = String::format("{\"Temp_C\": %4.2f, \"DewPt_C\": %4.2f, \"RelHum\": %4.2f}", t_c, dp_c, h);
    }
    else if (unit == "fahrenheit") {
        float t_f = dht.getTempFarenheit();
        float dp_c = dht.getDewPoint();
        float dp_f = (dp_c * 9 / 5) + 32;
        float h = dht.getHumidity();
        result = String::format("{\"Temp_F\": %4.2f, \"DewPt_F\": %4.2f, \"RelHum\": %4.2f}", t_f, dp_f, h);
    }
    Particle.publish("current_conditions", result, PRIVATE);
    return 1;
}
        

