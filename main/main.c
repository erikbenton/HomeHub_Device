#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "esp_log.h"
#include "mqtt_device.h"
#include "i2c_lm75a.h"
#include "device_tasks.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    init_i2c_lm75a();

    wifi_connect_init();

    // Configure the Wifi SSID and password in the menuconfig
    ESP_ERROR_CHECK(wifi_connect_sta(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD, 10000));

    init_mqtt();

    init_tasks();
}
