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

/**
 * This tests 2 threads with the same priority, each running the busy_busy function.
 * The second thread starts 1ms after the first thread does. The test passes if both
 * threads run for less than 20 seconds.
 */
void test_same_priority__busy_busy(void)
{
    test_helper(busy_busy, tskIDLE_PRIORITY, 0, &first_stats,
                busy_busy, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
    TEST_ASSERT(2000000 < first_stats);
    TEST_ASSERT(2000000 < second_stats);
}

/**
 * This tests 2 threads with the same priority, each running the busy_yield function.
 * The second thread starts 1ms after the first thread does. The test passes if the
 * first thread runs more than the second thread (since it started first), if the
 * first thread runs for more than 40 seconds, and the second thread runs for more than
 * 2 seconds.
 */
void test_same_priority__yield_yield(void)
{
    test_helper(busy_yield, tskIDLE_PRIORITY, 0, &first_stats,
                busy_yield, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
    TEST_ASSERT(first_stats > second_stats);
    TEST_ASSERT(first_stats > 4000000);
    TEST_ASSERT(second_stats > 2000);
}

/**
 * This tests 2 threads with the same priority, with the first thread running the 
 * busy_busy function and the second thread running the busy_yield function. The
 * second thread starts 1ms after the first thread does. The test passes if the
 * first thread (running busy_busy) runs more than the second thread (running 
 * busy_yield).
 */
void test_same_priority__busy_yield(void)
{
    test_helper(busy_busy, tskIDLE_PRIORITY, 0, &first_stats,
                busy_yield, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
    TEST_ASSERT(first_stats > second_stats); // Check that busy busy ran more than busy yield
}

/**
 * This tests 2 threads with different priorities, where both run the busy_busy
 * function. The first thread has lower priority than the second thread, and also
 * starts 1ms sooner than the second thread. Because of this, the higher priority
 * thread should block the lower priority thread, and the first thread should
 * barely run at all. This test passes if the first thread runs for less than 1
 * second and the second thread runs for more than 45 seconds.
 */
void test_diff_priority__busy_busy_low(void)
{
    test_helper(busy_busy, tskIDLE_PRIORITY, 0, &first_stats,
                busy_busy, tskIDLE_PRIORITY + 1, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
    TEST_ASSERT(1000 > first_stats);
    TEST_ASSERT(4500000 < second_stats);
}

/**
 * This tests 2 threads with different priorities, where both run the busy_busy
 * function. The first thread has higher priority than the second thread, and also
 * starts 1ms sooner than the second thread. Because of this, the higher priority
 * thread should block the lower priority thread, and the second thread should
 * barely run at all. This test passes if the second thread runs for less than 1
 * second and the first thread runs for more than 45 seconds.
 */
void test_diff_priority__busy_busy_high(void)
{
    test_helper(busy_busy, tskIDLE_PRIORITY + 1, 0, &first_stats,
                busy_busy, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
    TEST_ASSERT(4500000 < first_stats);
    TEST_ASSERT(1000 > second_stats);
}

/**
 * This tests 2 threads with different priorities, where both run the busy_yield
 * function. The first thread has a higher priority than the second thread, and also
 * starts 1ms sooner than the second thread. Both functions should consistently
 * yield their time to the other thread, but the thread that was started first, and
 * has the higher priority, should run for more time. The test passes if the first
 * thread runs more than the second thread, the first thread runs for more than 40
 * seconds, and the second thread runs for more than 2 seconds.
 */
void test_diff_priority__yield_yield(void)
{
    test_helper(busy_yield, tskIDLE_PRIORITY + 1, 0, &first_stats,
                busy_yield, tskIDLE_PRIORITY, 1, &second_stats,
                &elapsed_stats, &elapsed_ticks);
    TEST_ASSERT(first_stats > second_stats);
    TEST_ASSERT(first_stats > 4000000);
    TEST_ASSERT(second_stats > 2000);
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
