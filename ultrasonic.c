#include <stdio.h>
#include <string.h> // For string functions like strcmp
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"


// [0] = Trigger pin [1] = Echo pin 
int sensorZero[] =  {4, 5};
int sensorOne[] = {14, 15};
/* Sequence of numbers. 1 means zero was tripped first. 2 means one was tripped first.*/
char sequence[3] = "";  // Ensure sequence is initialized
/* Counter for tracking timeout */
int timeoutCounter = 0;
/* Threshold in centimeters. If the distance is less or equal to this than it's considered tripped.*/
int thresholdInCentimeters = 150;
/* Timeout bail count. If the counter goes above this sequence is reset.*/
int timeoutBailCount = 50;
/* Count of people in the room.*/
int currentPeople = 0;

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
    uint32_t signalOff = 0, signalOn = 0;

    while (gpio_get(givenSensor[1]) == 0) {
        signalOff = time_us_32();
    }
    while (gpio_get(givenSensor[1]) == 1) {
        signalOn = time_us_32();
    }

    float distance = ((signalOn - signalOff) * 0.0343f) / 2.0f;
    return distance;
}
/*! \brief Initialize everything needed */
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
    
    while (true)
    {
        float sensorZeroCurrent = readDistance(sensorZero);
        sleep_ms(50);  // Slight delay to avoid sensor interference
        float sensorOneCurrent = readDistance(sensorOne);

        if(sensorZeroCurrent < thresholdInCentimeters && sequence[0] != '1'){
            strcat(sequence, "1");
        } else if(sensorOneCurrent < thresholdInCentimeters && sequence[0] != '2'){
            strcat(sequence, "2");
        }
        if(strcmp(sequence, "12") == 0) {
            currentPeople++;
            sequence[0] = '\0'; // Reset sequence
            sleep_ms(550);
        } else if(strcmp(sequence, "21") == 0) {
            currentPeople--;
            sequence[0] = '\0'; // Reset sequence
            sleep_ms(550);
        }
        // Resets the sequence if it is invalid or timeouts
        if(strlen(sequence) > 2 || strcmp(sequence, "11") == 0 || strcmp(sequence, "22") == 0 || timeoutCounter > timeoutBailCount) {
            sequence[0] = '\0'; // Reset sequence
        }

        // Check the sequence length for timeout logic
        if(strlen(sequence) == 1) {
            timeoutCounter++;
        } else{
            timeoutCounter = 0;
        }

        if (currentPeople > 0){
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        } else if(currentPeople < 0){
            currentPeople = 0;
        } else{
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }
        printf("Seq: %c %c ", sequence[0], sequence[1]);
        printf("Sensor0: %f Sensor1: %f ", sensorZeroCurrent, sensorOneCurrent); 
        printf("Current people: %d\n", currentPeople);
    }

    return 0;
}
