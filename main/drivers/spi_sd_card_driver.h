#ifndef __SPI_SD_CARD_DRIVER_H__
#define __SPI_SD_CARD_DRIVER_H__

#include "../interfaces/sd_card_interface.h"

#define SD_PIN_CS 15
#define SD_PIN_CLK 14
#define SD_PIN_MOSI 13
#define SD_PIN_MISO 19

void spi_sd_card_create(sd_card_t *sd_card, void *ctx);

#endif