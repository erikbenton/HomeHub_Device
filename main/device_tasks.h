#ifndef __DEVICE_TASKS_H__
#define __DEVICE_TASKS_H__

#include <stdint.h>
#include "temperature_sensor_driver.h"

typedef struct DEVICE_CONFIG_STRUCT
{
    uint32_t id;
    char name[255];
    char location[255];
    char wifi_ssid[255];
    char wifi_password[255];
} device_config_t;

void init_tasks(temperature_sensor_t *temp_sensor);
void stop_sending_temperature(void);
void start_sending_temperature(void);
void read_device_config(const char *file_name, device_config_t *device_config);

#endif