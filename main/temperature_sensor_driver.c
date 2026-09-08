#include "temperature_sensor_driver.h"
#include "LM75A_temperature_sensor.h"
#include "string.h"
#include "esp_log.h"

static const char *TAG = "TEMP SENSE";

void temperature_sensor_init(const char *type, temperature_sensor_t *sensor, void *ctx)
{
    // create the sensor based on type
    if (strcmp(type, LM75A) == 0)
    {
        ESP_LOGI(TAG, "Creating LM75A");
        lm75a_ctx_t *lma75a_ctx = (lm75a_ctx_t *)ctx;
        lm75a_create(sensor, lma75a_ctx);
    }

    // initialize the sensor
    sensor->init(sensor);
}