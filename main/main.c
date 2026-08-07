#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"

esp_mqtt_client_handle_t mqtt_device_client;

static const char *TAG = "MQTT";
char commands_topic[50];
char temperature_state_topic[75];
char temperature_value_topic[75];

void mqtt_topic_controller(esp_mqtt_event_handle_t event)
{
    cJSON *payload;
    if (strcmp(event->topic, temperature_state_topic) == 0)
    {
        ESP_LOGI(TAG, "temperature state endpoint hit");
        // temperature state
        payload = cJSON_ParseWithLength(event->data, event->data_len);
        if (payload != NULL)
        {
            // state: for now will be boolean for on/off, can make enum later
            cJSON *temp_state_json = cJSON_GetObjectItem(payload, "state");
            bool is_on = cJSON_IsTrue(temp_state_json);
            ESP_LOGI(TAG, "Device state: %s", is_on ? "on" : "off");
        }
        cJSON_Delete(payload);
    }
    else if (strcmp(event->topic, temperature_value_topic) == 0)
    {
        // temperature value
        ESP_LOGI(TAG, "temperature value endpoint hit");
    }
}

void mqtt_any_event_handler(void *event_handler_arg,
                            esp_event_base_t event_base,
                            int32_t event_id,
                            void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_ANY:
        ESP_LOGI(TAG, "MQTT_EVENT_ANY");
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "ERROR %s", strerror(event->error_handle->esp_transport_sock_errno));
        break;
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        // subscribe to necessary topics
        esp_mqtt_client_subscribe(mqtt_device_client, commands_topic, 1);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("topic: %.*s\n", event->topic_len, event->topic);
        printf("message: %.*s\n", event->data_len, event->data);
        mqtt_topic_controller(event);
        break;
    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
        break;
    case MQTT_EVENT_DELETED:
        ESP_LOGI(TAG, "MQTT_EVENT_DELETED");
        break;
    case MQTT_USER_EVENT:
        ESP_LOGI(TAG, "MQTT_USER_EVENT");
        break;

    default:
        break;
    }
}

void init_dynamic_mqtt_topics(void)
{
    sprintf(commands_topic, "HomeHelper/hub/commands/%d/#", CONFIG_MQTT_DEVICE_ID);
    sprintf(temperature_state_topic, "HomeHelper/hub/commands/%d/temperature/state", CONFIG_MQTT_DEVICE_ID);
    sprintf(temperature_value_topic, "HomeHelper/hub/commands/%d/temperature/value", CONFIG_MQTT_DEVICE_ID);
}

void app_main(void)
{
    nvs_flash_init();

    wifi_connect_init();

    // Configure the Wifi SSID and password in the menuconfig
    ESP_ERROR_CHECK(wifi_connect_sta(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD, 10000));

    // MQTT client config
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        .broker.address.uri = CONFIG_MQTT_BROKER_URI,
        .session.last_will = {
            .topic = "animal/on-chip-death",
            .msg = "ESP32 died =(",
            .msg_len = strlen("ESP32 died =(")}};

    // initialize dynamic topics
    init_dynamic_mqtt_topics();

    mqtt_device_client = esp_mqtt_client_init(&esp_mqtt_client_config);

    esp_mqtt_client_register_event(mqtt_device_client, ESP_EVENT_ANY_ID, mqtt_any_event_handler, NULL);
    esp_mqtt_client_start(mqtt_device_client);
}
