#ifndef LAB6_H
#define LAB6_H

/**
 * Helper function for the tests comparing the runtimes of 2 threads. This function will start
 * the first thread after a specified delay, assigning the given function to that thread. It
 * will then do the same with the second thread. After all threads are done running, it will 
 * gather information about runtime overall and for each thread, save that information to the 
 * given global variables, and then delete both threads to exit.
 * 
 * @param func_1 Function to run in thread 1
 * @param priority_1 The priority of thread 1
 * @param delay_1 How long to wait before starting thread 1
 * @param duration_1 The address of a global variable in which the duration that thread 1 ran will be stored
 * @param func_2 Function to run in thread 2
 * @param priority_2 The priority of thread 2
 * @param delay_2 How long to wait before starting thread 2
 * @param duration_2 The address of a global variable in which the duration that thread 2 ran will be stored
 * @param duration The address of a global variable in which the duration that both threads ran will be stored
 * @param total_ticks The address of a global variable in which the number of ticks the test function took will be stored.
 */
void test_helper(TaskFunction_t func_1, int priority_1, uint32_t delay_1, configRUN_TIME_COUNTER_TYPE *duration_1,
                 TaskFunction_t func_2, int priority_2, uint32_t delay_2, configRUN_TIME_COUNTER_TYPE *duration_2,
                 configRUN_TIME_COUNTER_TYPE *duration, TickType_t *total_ticks);
                 
#endif