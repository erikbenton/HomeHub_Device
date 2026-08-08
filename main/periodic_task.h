#ifndef __PERIODIC_TASK_H__
#define __PERIODIC_TASK_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define START_PERIODIC_TASK 'S'
#define STOP_PERIODIC_TASK 'X'

typedef void (*periodic_callback)(void);

typedef struct PERIODIC_PARAMS_STRUCT
{
    TickType_t period_ms;
    periodic_callback periodic_cb;
} periodic_params_t;

void periodic_task(void *pxPeriodicParams);

#endif