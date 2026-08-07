#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "esp_log.h"
#include "mqtt_device.h"

void app_main(void)
{
    nvs_flash_init();

    wifi_connect_init();

    // Configure the Wifi SSID and password in the menuconfig
    ESP_ERROR_CHECK(wifi_connect_sta(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD, 10000));

    init_mqtt();
}
