#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

#include <stdio.h>
#include <string.h>
#include <sstream>

#include "SHT35_i2c.h"
#include "AnalogSensor.h"
#include "k30_i2c.h"
#include "pico_captive_connect.h"

// For the pico_connect lib
static absolute_time_t next_pub = 0;

const uint8_t I2C0_SDA_PIN = 4;
const uint8_t I2C0_SCL_PIN = 5;
const uint8_t I2C1_SDA_PIN = 6;
const uint8_t I2C1_SCL_PIN = 7;
const uint8_t ADC0_PAR_PIN = 26;

#define SHT35_ADDR 0x44
#define K30_ADDR 0x68
const uint8_t adc_par_channel = 0;

#define MQTT_TOPIC "atmino/sensor_out"

auto parMapping = [](float voltage) -> std::map<std::string,float> {
    float par = voltage * 1000.0f ;
    return {{"par", par}};
};

std::string make_json_payload(const std::map<std::string,float>& data){
    std::ostringstream ss;
    ss << "{";
    bool first = true;

    for (const auto &[key, value] : data){
        if (!first) ss<< ", ";
        ss << "\"" << key << "\": " << value;
        first = false;
    }

    ss << "}";

    return ss.str();
}


int main(){

    stdio_init_all();
    sleep_ms(3000);


    if (watchdog_caused_reboot()) {
        printf("[BOOT] System rebooted due to watchdog timeout!\n");
    } else {
        printf("[BOOT] Normal boot or manual reset.\n");
    }

    adc_init();
    i2c_init(i2c0,100*1000);
    i2c_init(i2c1,100*1000);

    gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL_PIN, GPIO_FUNC_I2C);

    AnalogSensor par_sensor(adc_par_channel, ADC0_PAR_PIN, parMapping);
    K30 k30(i2c1, 0x68);
    SHT35 sht35(i2c0, 0x44);

    par_sensor.init();
    k30.init();
    sht35.init();

    sleep_ms(1000);

    std::map<std::string,float> allData;
    std::string payload;
    uint64_t last_log = time_us_64();

    net_init();
    watchdog_enable(30000, 1); 
    

    // main loop
    while(true){

        watchdog_update();
        net_task();
        if (net_is_connected() && !mqtt_is_connected()) {
            mqtt_try_connect();
        }

        if (par_sensor.read()){
            auto par_data = par_sensor.getData(); 
            for (const auto &[key, value]: par_data){
                allData[key] = value;
            }
        }

        if(k30.read()){
            auto k30_data = k30.getData();
             for (const auto &[key, value] : k30_data) {
                allData[key] = value; // overwrite if exists
            }
        }    

        if (sht35.read()){
            auto sht35_data = sht35.getData(); 
            for (const auto &[key, value]: sht35_data){
                allData[key] = value;
            }
        }

        if((time_us_64() - last_log) > 1 * 1000000){
            last_log = time_us_64();

            payload = make_json_payload(allData);
            printf("%s\n", payload.c_str());
        }

        // Publish only if MQTT connected and it's time
        if (mqtt_is_connected() && absolute_time_diff_us(get_absolute_time(), next_pub) < 0) {

            if (publish_mqtt(MQTT_TOPIC, payload.c_str(), payload.length())) {
                // printf("[APP] Published temp message: %.2f\n", temp);
                next_pub = make_timeout_time_ms(1000); // normal period
            } else {
                printf("[APP] Publish failed, backing off\n");
                next_pub = make_timeout_time_ms(5000); // backoff if error
            }
        }

        sleep_ms(1);

    }

}