#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "pico.h"
#include "hardware/timer.h"

int main() {

    int counter = 0;
    const uint IRSensor1 = 14;
    gpio_init(IRSensor1);
    gpio_set_input_enabled(IRSensor1, true);
    gpio_pull_up(IRSensor1);

    const uint IRSensor2 = 13;
    gpio_init(IRSensor2);
    gpio_set_input_enabled(IRSensor2, true);
    gpio_pull_up(IRSensor2);

    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed");
        return -1;
    }
    uint32_t IRSensor1TriggerTime = 0;
    uint32_t IRSensor2TriggerTime = 0;
    while (true) {
        // After it senses one movement, skip over to the lighting code

        // If we sense movement turn the led on the board o
        //printf(gpio_get(IRSensor1))
        //printf("%d\n", gpio_get(IRSensor1));

        if (gpio_get(IRSensor1) && IRSensor1TriggerTime == 0){
            printf("One is triggered!\n");
            IRSensor1TriggerTime = time_us_32();
        }
        if (gpio_get(IRSensor2) && IRSensor2TriggerTime == 0){
            printf("Two is triggered!\n");
            IRSensor2TriggerTime = time_us_32();
        }

        if (IRSensor1TriggerTime > 0 && IRSensor2TriggerTime > 0){
            if(IRSensor1TriggerTime < IRSensor2TriggerTime){
                counter++;
            } else {
                counter--;
            }

        }
        else{
            continue;
        }
        /*
        if (absolute_time_diff_us(IRSensor1TriggerTime, IRSensor2TriggerTime) > 0){
        //printf("Add one person to the room\n");
            // add one to people counter
            
            
        }
        else if (absolute_time_diff_us(IRSensor1TriggerTime, IRSensor2TriggerTime) < 0){
            //printf("Remove one person to the room\n");
                // remove one to people counter
                counter--;
        } 
        else{
            //printf("Someone is probably standing in front of the sensor\n");
        }
        */
        printf("Sensor 1 trigger time: %d\n", IRSensor1TriggerTime);
        printf("Sensor 2 trigger time: %d\n", IRSensor2TriggerTime);
        //printf("Time difference... %d\n", absolute_time_diff_us(IRSensor1TriggerTime, IRSensor2TriggerTime));
        IRSensor1TriggerTime = 0;
        IRSensor2TriggerTime = 0;
        printf("Counter is: %d\n", counter);

        // Resetting values 
        
        //IRSensor1TriggerTime = absolute_time_t(0);

        //skip to here after censoring stuff is working
        if(counter > 0){
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        }
        else{
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }
        // Delay between people walking
        sleep_ms(1000);
    }

}