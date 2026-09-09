#ifndef __LM75A_CONTEXT_H__
#define __LM75A_CONTEXT_H__

#include "driver/i2c_master.h"

#define LM75A_TYPE "lm75a"
#define LM75A_DEFAULT_ADDRESS 0x48

typedef struct LM75A_CTX_STRUCT
{
    i2c_master_dev_handle_t i2c_dev_handle;
    uint8_t write_buffer_to_read[1];
    uint8_t read_buffer[2];
    uint8_t i2c_address;
} lm75a_ctx_t;

#endif