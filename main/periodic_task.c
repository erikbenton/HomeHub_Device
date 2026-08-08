#include <stdlib.h>
#include <stdio.h>
#include "periodic_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "PERIODIC";

void periodic_task(void *pxPeriodicParams)
{
    uint32_t task_state = STOP_PERIODIC_TASK;

    // get the period from the params
    periodic_params_t params = *(periodic_params_t *)pxPeriodicParams;

    TickType_t start_task = xTaskGetTickCount();
    for (;;)
    {
        // if the task is stopped, wait indefinitely
        if (task_state == STOP_PERIODIC_TASK)
        {
            ESP_LOGI(TAG, "Task stopped and waiting. Period: %d", params.period_ms);
            xTaskNotifyWait(0, 0, &task_state, portMAX_DELAY);

            // if the task is restarted
            if (task_state == START_PERIODIC_TASK)
            {
                // update task start time
                start_task = xTaskGetTickCount();
                ESP_LOGI(TAG, "Task restarted.");
            }
        }
        else
        {
            // Do the thing...
            if (params.periodic_cb != NULL)
            {
                params.periodic_cb();
            }

            // set up a TaskDelay for the period length
            vTaskDelayUntil(&start_task, pdMS_TO_TICKS(params.period_ms));

            // check the task state
            xTaskNotifyWait(0x00, 0x00, &task_state, 0);
        }
    }
}