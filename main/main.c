#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "esp_log.h"
#include "mqtt_device.h"
#include "device_tasks.h"
#include "interfaces/temperature_sensor_interface.h"
#include "interfaces/sd_card_interface.h"
#include "factories/temperature_sensor_factory.h"
#include "factories/sd_card_factory.h"
#include "contexts/lm75a_context.h"

temperature_sensor_t temperature_sensor;
lm75a_ctx_t lm75a_ctx;

sd_card_t sd_card;
device_config_t device_config;

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    lm75a_ctx.i2c_address = LM75A_DEFAULT_ADDRESS;
    temperature_sensor_init(LM75A_TYPE, &temperature_sensor, &lm75a_ctx);

    // init SPI SD card
    sd_card_init(SPI_SD_TYPE, &sd_card, NULL);

    // get device config from SD card
    read_device_config(&sd_card, "device_config.json", &device_config);

    wifi_connect_init();

    // Configure the Wifi SSID and password in the menuconfig
    ESP_ERROR_CHECK(
        wifi_connect_sta(
            device_config.wifi_ssid,
            device_config.wifi_password,
            10000));

    init_mqtt(&device_config);

    init_tasks(&temperature_sensor);
}