#include "temperature_sensor_factory.h"
#include "string.h"
#include "esp_log.h"
#include "../interfaces/temperature_sensor_interface.h"
#include "../drivers/lm75a_temperature_sensor_driver.h"

static const char *TAG = "TEMP SENSE";

void temperature_sensor_init(const char *type, temperature_sensor_t *sensor, void *ctx)
{
    // create the sensor based on type
    if (strcmp(type, LM75A_TYPE) == 0)
    {
        ESP_LOGI(TAG, "Creating LM75A");
        lm75a_ctx_t *lma75a_ctx = (lm75a_ctx_t *)ctx;
        lm75a_create(sensor, lma75a_ctx);
    }

    // initialize the sensor
    sensor->init(sensor);
}