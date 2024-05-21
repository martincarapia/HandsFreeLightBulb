#include <stdio.h>
#include <string.h> // For string functions like strcmp
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define distanceSensitivityInCentimeters 250.0f

// [0] = Trigger pin [1] = Echo pin
int sensorZero[] =  {4, 5};
int sensorOne[] = {14, 15};
char sequence[3];
int timeoutCounter = 0;

/*! \brief Send trigger to given Pin
 *  \param givenSensor An ultrasonic sensor. index 0 is the trigger pin and 1 is the echo pin
 */
void sendTrigger(int givenSensor[])
{
    gpio_put(givenSensor[0], 0);
    sleep_us(2);
    gpio_put(givenSensor[0], 1);
    sleep_us(10);
    gpio_put(givenSensor[0], 0);
}

/*! \brief Read the distance an object is from the sensor
 *  \param givenSensor An ultrasonic sensor. index 0 is the trigger pin and 1 is the echo pin
 *  \return Distance an object is from sensor
 */
float readDistance(int givenSensor[])
{
    sendTrigger(givenSensor);
    uint32_t signalOff, signalOn;

    while (gpio_get(givenSensor[1]) == 0) {
        signalOff = time_us_32();
    }
    while (gpio_get(givenSensor[1]) == 1) {
        signalOn = time_us_32();
    }

    float distance = ((signalOn - signalOff) * 0.0343) / 2;
    return distance;
}

/*! \brief Method for grabbing the timestamp of a sensor that tripped within threshold
 *  \param givenSensor An ultrasonic sensor. index 0 is the trigger pin and 1 is the echo pin
 *  \return Returns timestamp if distance is less than threshold. Returns 0 if farther than a pre-defined threshold
 */
uint32_t getDistanceTimestamp(int givenSensor[])
{
    float distance = readDistance(givenSensor);

    printf("Distance of Pin %d: %.2f cm\n", givenSensor[0], distance);

    if (distance < distanceSensitivityInCentimeters)
    {
        return time_us_32();
    }
    else
    {
        return 0;
    }
}
/*! \brief Method for measuring distance off a specified sensor
 *  \param givenSensor An ultrasonic sensor. index 0 is the trigger pin and 1 is the echo pin
 *  \return Returns read distance from a sensor
 */
uint32_t measureDistance(int givenSensor[])
{
    float distance = readDistance(givenSensor);

    printf("Distance of Pin %d: %.2f cm\n", givenSensor[0], distance);

    return distance;
}


void init_all(){
    stdio_init_all();
    cyw43_arch_init();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    gpio_init(sensorZero[0]);
    gpio_set_dir(sensorZero[0], GPIO_OUT);
    gpio_init(sensorZero[1]);
    gpio_set_dir(sensorZero[1], GPIO_IN);

    gpio_init(sensorOne[0]);
    gpio_set_dir(sensorOne[0], GPIO_OUT);
    gpio_init(sensorOne[1]);
    gpio_set_dir(sensorOne[1], GPIO_IN);
}
int main()
{
    // Setup code
    init_all();
    int currentPeople = 0;
    // Grab the initial distance
    // printf("Sensor0 Init");
    float sensorZeroInitial = readDistance(sensorZero);
    // printf("Sensor1 Init");
    float sensorOneInitial = readDistance(sensorOne);
    while (true)
    {
        uint32_t sensorZeroCurrent = readDistance(sensorZero);
        sleep_ms(50);  // Slight delay to avoid sensor interference
        uint32_t sensorOneCurrent = readDistance(sensorOne);

        if(sensorZeroCurrent < sensorZeroInitial - 30 && sequence[0] != '1'){
            strcat(sequence, "1");
        } else if(sensorOneCurrent < sensorOneInitial - 30 && sequence[0] != '2'){
            strcat(sequence, "2");
        }
        if(strcmp(sequence, "12") == 0) {
            currentPeople++;
            sequence[0] = '\0'; // Reset sequence
            sleep_ms(550);
        } else if(strcmp(sequence, "21") == 0 && currentPeople > 0) {
            currentPeople--;
            sequence[0] = '\0'; // Reset sequence
            sleep_ms(550);
        }
        // Resets the sequence if it is invalid or timeouts
        if(strlen(sequence) > 2 || strcmp(sequence, "11") == 0 || strcmp(sequence, "22") == 0 || timeoutCounter > 200) {
            sequence[0] = '\0'; // Reset sequence
        }

        // Check the sequence length for timeout logic
        if(strlen(sequence) == 1) {
            timeoutCounter++;
        } else {
            timeoutCounter = 0;
        }

        if (currentPeople > 0){
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        } else {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }
        printf("Current people: %d\n", currentPeople);
    }

    return 0;
}
