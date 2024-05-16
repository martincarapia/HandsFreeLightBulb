#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define triggerPinZero 4
#define echoPinZero 5
#define triggerPinOne 14
#define echoPinOne 15
#define distanceSensitivityInCentimeters 250.0f

/*! \brief Send trigger to given Pin
 *   \param givenTrigPin Trigger pin passed into the method
 */
void sendTrigger(int givenTrigPin)
{
    gpio_put(givenTrigPin, 0);
    sleep_us(2);
    gpio_put(givenTrigPin, 1);
    sleep_us(10);
    gpio_put(givenTrigPin, 0);
}

/*! \brief Read the distance an object is from the sensor
 *  \param givenEchoPin Echo pin passed into the method
 *  \return Distance an object is from sensor
 */
float readDistance(int givenEchoPin)
{
    uint32_t signalOff, signalOn;

    while (gpio_get(givenEchoPin) == 0) {
        signalOff = time_us_32();
    }
    while (gpio_get(givenEchoPin) == 1) {
        signalOn = time_us_32();
    }

    float distance = ((signalOn - signalOff) * 0.0343) / 2;
    return distance;
}

/*! \brief Main method for reading off ultrasonic sensors
 *  \param givenEchoPin Echo pin passed into the method
 *  \param givenTrigPin Trigger pin passed into the method
 *  \return Returns timestamp if distance is less than threshold. Returns 0 if farther than a pre-defined threshold
 */
uint32_t ultra(int givenTrigPin, int givenEchoPin)
{
    sendTrigger(givenTrigPin);
    float distance = readDistance(givenEchoPin);

    printf("Distance of Pin %d: %.2f cm\n", givenTrigPin, distance);

    if (distance < distanceSensitivityInCentimeters)
    {
        return time_us_32();
    }
    else
    {
        return 0;
    }
}

void init_all(){
    stdio_init_all();
    cyw43_arch_init();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    gpio_init(triggerPinZero);
    gpio_set_dir(triggerPinZero, GPIO_OUT);
    gpio_init(echoPinZero);
    gpio_set_dir(echoPinZero, GPIO_IN);

    gpio_init(triggerPinOne);
    gpio_set_dir(triggerPinOne, GPIO_OUT);
    gpio_init(echoPinOne);
    gpio_set_dir(echoPinOne, GPIO_IN);
}

int main()
{
    init_all();
    int counter = 0;

    while (true)
    {
        uint32_t sensorZero = ultra(triggerPinZero, echoPinZero);
        sleep_ms(50);  // Slight delay to avoid sensor interference
        uint32_t sensorOne = ultra(triggerPinOne, echoPinOne);

        if (sensorZero > 0 && sensorOne > 0)
        {
            if (sensorZero < sensorOne)
            {
                printf("Sensor Zero was tripped first   ");
                counter--;
            }
            else if (sensorZero > sensorOne)
            {
                printf("Sensor One was tripped first   ");
                counter++;
            }
            else
            {
                printf("Tripped at the same time   ");
            }

            if (counter < 0) {
                counter = 0;
            }
            printf("Counter: %d\n", counter);
        }
        else
        {
            if (sensorZero > 0)
            {
                printf("Only sensor zero was tripped\n");
            }
            if (sensorOne > 0)
            {
                printf("Only sensor one was tripped\n");
            }
        }

        sensorZero = 0;
        sensorOne = 0;

        sleep_ms(250);
    }

    return 0;
}
