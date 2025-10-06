#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "OPT4048_i2c.h"
#include "pico_captive_connect.h"

const uint8_t I2C0_SDA_PIN = 4;
const uint8_t I2C0_SCL_PIN = 5;

#define OPT4048_ADDR 0x44


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

    i2c_init(i2c0,100*1000);
    gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);

    OPT4048 opt4048(i2c0, 0x44);

    opt4048.init();
    sleep_ms(1000);

    std::map<std::string,float> allData;
    std::string payload;

    uint64_t last_log = time_us_64();

    // main loop
    while(true){

        if (opt4048.read()){
            auto opt4048_data = opt4048.getData(); 
            for (const auto &[key, value]: opt4048_data){
                allData[key] = value;
            }
        }

        if((time_us_64() - last_log) > 1 * 1000000){
            last_log = time_us_64();

            payload = make_json_payload(allData);
            printf("%s\n", payload.c_str());
        }

        sleep_ms(1);


    }


}