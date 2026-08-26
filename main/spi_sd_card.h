#ifndef __SPI_SD_CARD_H__
#define __SPI_SD_CARD_H__

void init_spi_sd_card(void);
void sd_read_file(const char *path, char *buffer, int buff_size);
void sd_write_file(char *path, const char *content);

#endif