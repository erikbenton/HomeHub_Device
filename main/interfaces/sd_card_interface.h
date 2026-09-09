#ifndef __SD_CARD_INTERFACE_H__
#define __SD_CARD_INTERFACE_H__

#include "esp_err.h"

typedef struct SD_CARD_STRUCT sd_card_t;

struct SD_CARD_STRUCT
{
    esp_err_t (*init)(sd_card_t *self); // initialize the hardware
    esp_err_t (*read_full_file)(sd_card_t *self, const char *path, char *buffer, long buff_size);
    esp_err_t (*write_file)(sd_card_t *self, char *path, const char *content);
    void *ctx; // any extra configure parameters needed
};

#endif