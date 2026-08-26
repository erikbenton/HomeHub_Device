#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "esp_log.h"
#include "mqtt_device.h"
#include "i2c_lm75a.h"
#include "device_tasks.h"
#include "spi_sd_card.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    init_i2c_lm75a();

    // init SPI SD card
    init_spi_sd_card();

    // get device config from SD card
    device_config_t device_config;
    read_device_config("default_config.json", &device_config);

    wifi_connect_init();

    // Configure the Wifi SSID and password in the menuconfig
    ESP_ERROR_CHECK(
        wifi_connect_sta(
            device_config.wifi_ssid,
            device_config.wifi_password,
            10000));

    init_mqtt(&device_config);

    init_tasks();
}