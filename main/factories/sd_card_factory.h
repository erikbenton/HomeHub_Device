#ifndef __SD_CARD_FACTORY_H__
#define __SD_CARD_FACTORY_H__

#include "../interfaces/sd_card_interface.h"

#define SPI_SD_TYPE "spi_sd"

void sd_card_init(const char *type, sd_card_t *sd_card, void *ctx);

#endif