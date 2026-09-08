#include "mqtt_device.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "device_tasks.h"

esp_mqtt_client_handle_t mqtt_device_client;
device_config_t *device_cfg;

static const char *TAG = "MQTT";
char hello_topic[50];
char commands_topic[50];
char temperature_cmd_state_topic[75];
char temperature_cmd_value_topic[75];
char temperature_device_state_topic[75];
char temperature_device_value_topic[75];

int mqtt_device_send(const char *topic, const char *payload, bool retain)
{
    return esp_mqtt_client_publish(mqtt_device_client, topic, payload, strlen(payload), 1, retain);
}

int mqtt_send_device_temperature_value(const char *payload, bool retain)
{
    return mqtt_device_send(temperature_device_value_topic, payload, retain);
}

int mqtt_send_device_hello(bool retain)
{
    cJSON *payload_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(payload_json, "id", device_cfg->id);
    cJSON_AddStringToObject(payload_json, "name", device_cfg->name);
    cJSON_AddStringToObject(payload_json, "location", device_cfg->location);
    char *payload = cJSON_Print(payload_json);
    cJSON_Delete(payload_json);
    return mqtt_device_send(hello_topic, payload, retain);
}

void mqtt_topic_controller(esp_mqtt_event_handle_t event)
{
    if (event->topic_len == 0)
        return;

    char topic[75];
    sprintf(topic, "%.*s", event->topic_len, event->topic);
    cJSON *payload;
    if (strcmp(topic, temperature_cmd_state_topic) == 0)
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
            if (is_on)
            {
                start_sending_temperature();
            }
            else
            {
                stop_sending_temperature();
            }
        }
        else
        {
            ESP_LOGE(TAG, "Unable to parse JSON for temperature state: %.*s", event->data_len, event->data);
        }

        cJSON_Delete(payload);
    }
    else if (strcmp(topic, temperature_cmd_value_topic) == 0)
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

        // send "hello" message
        mqtt_send_device_hello(1);
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

void init_dynamic_mqtt_topics(device_config_t *device_config)
{
    sprintf(hello_topic, "HomeHub/hello/%ld", device_config->id);
    sprintf(commands_topic, "HomeHub/hub/commands/%ld/#", device_config->id);
    sprintf(temperature_cmd_state_topic, "HomeHub/hub/commands/%ld/temperature/state", device_config->id);
    sprintf(temperature_cmd_value_topic, "HomeHub/hub/commands/%ld/temperature/value", device_config->id);
    sprintf(temperature_device_state_topic, "HomeHub/hub/device/%ld/temperature/state", device_config->id);
    sprintf(temperature_device_value_topic, "HomeHub/hub/device/%ld/temperature/value", device_config->id);
}

void init_mqtt(device_config_t *init_device_config)
{
    device_cfg = init_device_config;

    // MQTT client config
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        .broker.address.uri = CONFIG_MQTT_BROKER_URI};
    // .session.last_will = {
    //     .topic = "HomeHub/hub/device/on-chip-death",
    //     .msg = "ESP32 died =(",
    //     .msg_len = strlen("ESP32 died =(")}};

    // initialize dynamic topics
    init_dynamic_mqtt_topics(device_cfg);

    mqtt_device_client = esp_mqtt_client_init(&esp_mqtt_client_config);

    ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqtt_device_client, ESP_EVENT_ANY_ID, mqtt_any_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_device_client));
}