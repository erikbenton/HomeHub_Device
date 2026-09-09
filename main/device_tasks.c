#include "device_tasks.h"
#include <stdlib.h>
#include <stdio.h>
#include "periodic_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "cJSON.h"
#include "mqtt_device.h"
#include "interfaces/temperature_sensor_interface.h"
#include "interfaces/sd_card_interface.h"

/*

File for defining/aggregating all of the tasks.

*/

static const char *TAG = "TASKS";

TaskHandle_t periodic_handler = NULL;
periodic_params_t periodic_params;

temperature_sensor_t *lm75a_temp_sensor;

void send_temperature(void)
{
    float temperature;
    esp_err_t i2c_res = lm75a_temp_sensor->read_fahrenheit(lm75a_temp_sensor, &temperature);
    if (i2c_res == ESP_OK)
    {
        cJSON *payload_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(payload_json, "fahrenheit", temperature);
        char *payload = cJSON_Print(payload_json);
        cJSON_Delete(payload_json);
        mqtt_send_device_temperature_value(payload, false);
    }
    else
    {
        ESP_LOGE(TAG, "Failure connecting to I2C device: %d", i2c_res);
    }
}

void start_sending_temperature(void)
{
    ESP_LOGI(TAG, "Starting task");
    if (xTaskNotify(periodic_handler, START_PERIODIC_TASK, eSetValueWithOverwrite) == pdTRUE)
    {
        ESP_LOGI(TAG, "Started task");
    }
    else
    {
        ESP_LOGE(TAG, "Unable to start task.");
    }
}

void stop_sending_temperature(void)
{
    ESP_LOGI(TAG, "Stopping task");
    if (xTaskNotify(periodic_handler, STOP_PERIODIC_TASK, eSetValueWithOverwrite) == pdTRUE)
    {
        ESP_LOGI(TAG, "Stopped task");
    }
    else
    {
        ESP_LOGI(TAG, "Unable to stop task.");
    }
}

void read_device_config(sd_card_t *sd_card, const char *file_name, device_config_t *device_config)
{
    // put this config on the heap temporarily
    const uint16_t size = 255 * 5;
    char *config = (char *)malloc(size);
    char *file_path = (char *)malloc(255);

    // incase the user has a different file name
    sprintf(file_path, "/store/%s", file_name);

    // read the config file
    sd_card->read_full_file(sd_card, file_path, config, size);

    // parse the JSON string
    cJSON *payload = cJSON_Parse(config);

    if (payload != NULL)
    {
        cJSON *item_payload;

        // get the id
        item_payload = cJSON_GetObjectItem(payload, "id");
        device_config->id = item_payload != NULL ? cJSON_GetNumberValue(item_payload) : CONFIG_MQTT_DEVICE_ID;

        // get the name
        item_payload = cJSON_GetObjectItem(payload, "name");
        sprintf(device_config->name, "%s", item_payload != NULL ? cJSON_GetStringValue(item_payload) : CONFIG_DEVICE_NAME);

        // get the location
        item_payload = cJSON_GetObjectItem(payload, "location");
        sprintf(device_config->location, "%s", item_payload != NULL ? cJSON_GetStringValue(item_payload) : CONFIG_DEVICE_LOCATION);

        // get the wifi_ssid
        item_payload = cJSON_GetObjectItem(payload, "wifi_ssid");
        sprintf(device_config->wifi_ssid, "%s", item_payload != NULL ? cJSON_GetStringValue(item_payload) : CONFIG_WIFI_SSID);

        // get the wifi_password
        item_payload = cJSON_GetObjectItem(payload, "wifi_password");
        sprintf(device_config->wifi_password, "%s", item_payload != NULL ? cJSON_GetStringValue(item_payload) : CONFIG_WIFI_PASSWORD);
    }
    else
    {
        // use the menuconfig defaults
        device_config->id = CONFIG_MQTT_DEVICE_ID;
        sprintf(device_config->name, "%s", CONFIG_DEVICE_NAME);
        sprintf(device_config->location, "%s", CONFIG_DEVICE_LOCATION);
        sprintf(device_config->wifi_ssid, "%s", CONFIG_WIFI_SSID);
        sprintf(device_config->wifi_password, "%s", CONFIG_WIFI_PASSWORD);
    }

    // free up the heap
    free(config);
    free(file_path);

    // print out the values being used
    ESP_LOGI(TAG, "Device ID: %d", device_config->id);
    ESP_LOGI(TAG, "Device Name: %s", device_config->name);
    ESP_LOGI(TAG, "Device Location: %s", device_config->location);
    ESP_LOGI(TAG, "Device WiFi SSID: %s", device_config->wifi_ssid);
}

void init_tasks(temperature_sensor_t *temp_sensor)
{
    lm75a_temp_sensor = temp_sensor;

    // configure the params
    periodic_params.period_ms = 1000; // 1 second
    periodic_params.periodic_cb = send_temperature;

    // Create the tasks
    xTaskCreate(periodic_task, "Periodic Task", 2048, (void *)&periodic_params, 1, &periodic_handler);
}