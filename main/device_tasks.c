#include "device_tasks.h"
#include <stdlib.h>
#include <stdio.h>
#include "periodic_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "cJSON.h"
#include "i2c_lm75a.h"
#include "mqtt_device.h"

/*

File for defining/aggregating all of the tasks.

*/

static const char *TAG = "TASKS";

TaskHandle_t periodic_handler = NULL;
periodic_params_t periodic_params;

void send_temperature(void)
{
    float temperature;
    esp_err_t i2c_res = get_temperature_f(&temperature);
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

void init_tasks(void)
{
    // configure the params
    periodic_params.period_ms = 1000; // 1 second
    periodic_params.periodic_cb = send_temperature;

    // Create the tasks
    xTaskCreate(periodic_task, "Periodic Task", 2048, (void *)&periodic_params, 1, &periodic_handler);
}