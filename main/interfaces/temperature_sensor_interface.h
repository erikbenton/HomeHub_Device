#ifndef __TEMPERATURE_SENSOR_INTERFACE_H__
#define __TEMPERATURE_SENSOR_INTERFACE_H__

#include "esp_err.h"
#include "driver/i2c_master.h"

#define LM75A_TYPE "lm75a"
#define LM75A_DEFAULT_ADDRESS 0x48

typedef struct TEMPERATURE_SENSOR_STRUCT temperature_sensor_t;

struct TEMPERATURE_SENSOR_STRUCT {
    esp_err_t (*init)(temperature_sensor_t *self); // initialize the hardware
    esp_err_t (*read_fahrenheit)(temperature_sensor_t *self, float *temperature); // read the temperature in fahrenheit
    void *ctx; // any extra configure parameters needed
};

typedef struct LM75A_CTX_STRUCT
{
    i2c_master_dev_handle_t i2c_dev_handle;
    uint8_t write_buffer_to_read[1];
    uint8_t read_buffer[2];
    uint8_t i2c_address;
} lm75a_ctx_t;

#endif