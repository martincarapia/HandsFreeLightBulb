#include <stdio.h>
#include <string.h> // For string functions like strcmp
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
// WiFi
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWD"
// MQTT
#define PORT  1883
#define MQTT_CLIENT_ID "clientID"
#define MQTT_BROKER_IP "YOUR_BROKER_IP"

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

    gpio_init(sensorZero[0]);
    gpio_set_dir(sensorZero[0], GPIO_OUT);
    gpio_init(sensorZero[1]);
    gpio_set_dir(sensorZero[1], GPIO_IN);

    gpio_init(sensorOne[0]);
    gpio_set_dir(sensorOne[0], GPIO_OUT);
    gpio_init(sensorOne[1]);
    gpio_set_dir(sensorOne[1], GPIO_IN);
}

typedef struct {
    mqtt_client_t* mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    uint8_t data[MQTT_OUTPUT_RINGBUF_SIZE];
    uint8_t topic[100];
    uint32_t len;
} MQTT_CLIENT_DATA_T;

MQTT_CLIENT_DATA_T *mqtt;




void publish_count(int given_val) {
    //Publish value based on passed param
    char temp_str[16];
    snprintf(temp_str, sizeof(temp_str), "%d", given_val);
    mqtt_publish(mqtt->mqtt_client_inst, "/lightcontrol", temp_str, strlen(temp_str), 0, 0, NULL, NULL);
}
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    MQTT_CLIENT_DATA_T* mqtt_client = (MQTT_CLIENT_DATA_T*)arg;
    strncpy(mqtt_client->data, data, len);
    mqtt_client->len = len;
    mqtt_client->data[len] = '\0';
    printf("Topic: %s, Message: %s\n", mqtt_client->topic, mqtt_client->data);


    if (strcmp(mqtt->topic, "/led") == 0)
    {
        if (strcmp((const char *)mqtt_client->data, "On") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        else if  (strcmp((const char *)mqtt_client->data, "Off") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    }

}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    MQTT_CLIENT_DATA_T* mqtt_client = (MQTT_CLIENT_DATA_T*)arg;
    strcpy(mqtt_client->topic, topic);
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    MQTT_CLIENT_DATA_T* mqtt_client = (MQTT_CLIENT_DATA_T*)arg;
    if (status == MQTT_CONNECT_ACCEPTED) {
        mqtt_sub_unsub(client, "/led", 0, NULL, arg, 1);
        printf("Connected to the /led topic successfully\n");
    }
}

int main()
{
    // Setup code
    init_all();

    mqtt = (MQTT_CLIENT_DATA_T*)calloc(1, sizeof(MQTT_CLIENT_DATA_T));
    if (!mqtt) {
        printf("Failed to initialize MQTT client \n");
        return 1;
    }
    // MQTT CLIENT INFO
    mqtt->mqtt_client_info.client_id = MQTT_CLIENT_ID;
    mqtt->mqtt_client_info.keep_alive = 60; // Keep alive in secondi

    if (cyw43_arch_init()) {
        printf("Failed to initialize CYW43\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Wi-Fi error\n");
        return 1;
    }
    printf("\nConnected to Wifi\n");

    ip_addr_t addr;
    if (!ip4addr_aton(MQTT_BROKER_IP, &addr)) {
        printf("MQTT ip Address not valid !\n");
        return 1;
    }

    mqtt->mqtt_client_inst = mqtt_client_new();
    if (!mqtt->mqtt_client_inst) {
        printf("MQTT client instance creation error\n");
        return 1;
    }

    if (mqtt_client_connect(mqtt->mqtt_client_inst, &addr, PORT, mqtt_connection_cb, mqtt, &mqtt->mqtt_client_info) != ERR_OK) {
        printf("MQTT broker connection error\n");
        return 1;
    }
    printf("Successfully connected to the MQTT broker\n");
    mqtt_set_inpub_callback(mqtt->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, mqtt);


    while (true)
    {
        float sensorZeroCurrent = readDistance(sensorZero);
        sleep_ms(50);  // Slight delay to avoid sensor interference
        float sensorOneCurrent = readDistance(sensorOne);

        if(sensorZeroCurrent < thresholdInCentimeters && sequence[0] != '0'){
            strcat(sequence, "0");
        } else if(sensorOneCurrent < thresholdInCentimeters && sequence[0] != '1'){
            strcat(sequence, "1");
        }
        if(strcmp(sequence, "01") == 0) {
            publish_count(1);
            sequence[0] = '\0'; // Reset sequence
            sleep_ms(550);
        } else if(strcmp(sequence, "10") == 0) {
            publish_count(-1);
            sequence[0] = '\0'; // Reset sequence
            sleep_ms(550);
        }
        // Resets the sequence if it is invalid or timeouts
        if(strlen(sequence) > 2 || strcmp(sequence, "11") == 0 || strcmp(sequence, "00") == 0 || timeoutCounter > timeoutBailCount) {
            sequence[0] = '\0'; // Reset sequence
        }

        // Check the sequence length for timeout logic
        if(strlen(sequence) == 1) {
            timeoutCounter++;
        } else{
            timeoutCounter = 0;
        }
        printf("Seq: %c %c ", sequence[0], sequence[1]);
        printf("Sensor0: %f Sensor1: %f ", sensorZeroCurrent, sensorOneCurrent); 
    }

    return 0;
}
