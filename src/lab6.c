#include "lab6.h"

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

void test_helper(TaskFunction_t func_1, int priority_1, uint32_t delay_1, configRUN_TIME_COUNTER_TYPE *duration_1,
                 TaskFunction_t func_2, int priority_2, uint32_t delay_2, configRUN_TIME_COUNTER_TYPE *duration_2,
                 configRUN_TIME_COUNTER_TYPE *duration, TickType_t *total_ticks)
{
    TaskHandle_t task_1, task_2;

    TickType_t start_ticks = xTaskGetTickCount(); // Get the tick count at the start of the test
    configRUN_TIME_COUNTER_TYPE start_count = portGET_RUN_TIME_COUNTER_VALUE();

    /* Create task 1 after the specified delay with given priority */
    vTaskDelay(delay_1);
    xTaskCreate(func_1, "Task 1", configMINIMAL_STACK_SIZE, NULL, priority_1, &task_1);

    /* Create task 2 after the specified delay with given priority */
    vTaskDelay(delay_2);
    xTaskCreate(func_2, "Task 2", configMINIMAL_STACK_SIZE, NULL, priority_2, &task_2);

    /* Get how long many times each thread ran */
    configRUN_TIME_COUNTER_TYPE end_count = portGET_RUN_TIME_COUNTER_VALUE();
    configRUN_TIME_COUNTER_TYPE first = ulTaskGetRunTimeCounter(task_1);
    configRUN_TIME_COUNTER_TYPE second = ulTaskGetRunTimeCounter(task_2);
    configRUN_TIME_COUNTER_TYPE elapsed = end_count - start_count;

    TickType_t end_ticks = xTaskGetTickCount(); // Get the tick count at the end of the test

    /* Pass runtime information back to global variables */
    *duration_1 = first;
    *duration_2 = second;
    *duration = elapsed;
    *total_ticks = end_ticks - start_ticks;

    /* Delete all threads */
    vTaskDelete(task_1);
    vTaskDelete(task_2);
}