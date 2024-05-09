#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define TRIGGER_PIN 4
#define ECHO_PIN 5

void ultra()
{
    gpio_put(TRIGGER_PIN, 0);
    sleep_us(2);
    gpio_put(TRIGGER_PIN, 1);
    sleep_us(5);
    gpio_put(TRIGGER_PIN, 0);
    uint32_t signaloff, signalon;
    float distanceSensitivity = 250.0f;
    while (gpio_get(ECHO_PIN) == 0)
    {
        signaloff = time_us_32();
    }
    while (gpio_get(ECHO_PIN) == 1)
    {
        signalon = time_us_32();
    }
    uint32_t timepassed = signalon - signaloff;
    float distance = (timepassed * 0.0343) / 2;
    if (distance < distanceSensitivity){
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    else{
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    }
    
    printf("The distance from object is %.2f cm\n", distance);
}

int main()
{
    stdio_init_all();
    cyw43_arch_init();

    gpio_init(TRIGGER_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);

    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    while (true)
    {
        ultra();
        sleep_ms(500);
    }

    return 0;
}
