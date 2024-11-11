#ifndef LAB6_H
#define LAB6_H

void test_helper(TaskFunction_t func_1, int priority_1, uint32_t delay_1, configRUN_TIME_COUNTER_TYPE *duration_1,
                 TaskFunction_t func_2, int priority_2, uint32_t delay_2, configRUN_TIME_COUNTER_TYPE *duration_2,
                 configRUN_TIME_COUNTER_TYPE *duration, TickType_t *total_ticks);
                 
#endif