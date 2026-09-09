#ifndef __TEMPERATURE_SENSOR_INTERFACE_H__
#define __TEMPERATURE_SENSOR_INTERFACE_H__

#include "esp_err.h"

typedef struct TEMPERATURE_SENSOR_STRUCT temperature_sensor_t;

struct TEMPERATURE_SENSOR_STRUCT
{
    esp_err_t (*init)(temperature_sensor_t *self);                                // initialize the hardware
    esp_err_t (*read_fahrenheit)(temperature_sensor_t *self, float *temperature); // read the temperature in fahrenheit
    void *ctx;                                                                    // any extra context for implementing operations
};

#endif