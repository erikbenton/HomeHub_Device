#ifndef __TEMPERATURE_SENSOR_FACTORY_H__
#define __TEMPERATURE_SENSOR_FACTORY_H__

#include "../interfaces/temperature_sensor_interface.h"

void temperature_sensor_init(const char *type, temperature_sensor_t *sensor, void *ctx);

#endif