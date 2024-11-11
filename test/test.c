#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "../src/lab6.c"

TaskHandle_t main_task;

configRUN_TIME_COUNTER_TYPE first_stats, second_stats, elapsed_stats;
TickType_t elapsed_ticks;

void setUp(void) {}

void tearDown(void)
{
    printf("primary %lld secondary %lld elapsed %lld (us) in %d ticks\n",
           first_stats, second_stats, elapsed_stats, elapsed_ticks);
}

void busy_busy(void *args)
{
    char *name = (char *)args;
    printf("start busy_busy %s\n", name);
    for (int i = 0;; i++)
        ;
}

void busy_yield(void *args)
{
    char *name = (char *)args;
    printf("start busy_yield %s\n", name);
    for (int i = 0;; i++)
    {
        taskYIELD();
    }
}

void test_same_priority__busy_busy(void)
{
    test_helper(busy_busy, tskIDLE_PRIORITY, 0, &first_stats,
                busy_busy, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
    TEST_ASSERT(2000000 < first_stats);
    TEST_ASSERT(2000000 < second_stats);
}

void test_same_priority__yield_yield(void)
{
    test_helper(busy_yield, tskIDLE_PRIORITY, 0, &first_stats,
                busy_yield, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
    TEST_ASSERT(first_stats > second_stats);
    TEST_ASSERT(first_stats > 4400000);
    TEST_ASSERT(second_stats > 2000);
}

void test_same_priority__busy_yield(void)
{
    test_helper(busy_busy, tskIDLE_PRIORITY, 0, &first_stats,
                busy_yield, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
}

void test_diff_priority__busy_busy_low(void)
{
    test_helper(busy_busy, tskIDLE_PRIORITY, 0, &first_stats,
                busy_busy, tskIDLE_PRIORITY + 1, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
}

void test_diff_priority__busy_busy_high(void)
{
    test_helper(busy_busy, tskIDLE_PRIORITY + 1, 0, &first_stats,
                busy_busy, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
}

void test_diff_priority__yield_yield(void)
{
    test_helper(busy_yield, tskIDLE_PRIORITY + 1, 0, &first_stats,
                busy_yield, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
}

void main_thread()
{
    while (1)
    {
        UNITY_BEGIN();
        RUN_TEST(test_same_priority__busy_busy);
        RUN_TEST(test_same_priority__yield_yield);
        RUN_TEST(test_same_priority__busy_yield);
        RUN_TEST(test_diff_priority__busy_busy_low);
        RUN_TEST(test_diff_priority__busy_busy_high);
        RUN_TEST(test_diff_priority__yield_yield);
        UNITY_END();
        vTaskDelay(500);
    }
}

int main(void)
{
    stdio_init_all();
    xTaskCreate(main_thread, "MainTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &main_task);
    vTaskStartScheduler();
    return 0;
}
