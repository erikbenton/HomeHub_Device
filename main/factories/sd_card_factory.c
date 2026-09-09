#include "sd_card_interface.h"
#include "sd_card_factory.h"
#include "string.h"
#include "../drivers/spi_sd_card_driver.h"
#include "esp_log.h"

const char *TAG = "SD_FACTORY";

void sd_card_init(const char *type, sd_card_t *sd_card, void *ctx)
{
    // create appropriate hardware
    if (strcmp(type, SPI_SD_TYPE) == 0)
    {
        ESP_LOGI(TAG, "Creating SPI SD card");
        spi_sd_card_create(sd_card, ctx);
    }

    // inititalize the sd card
    sd_card->init(sd_card);
}
