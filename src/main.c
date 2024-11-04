#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "pico/cyw43_arch.h"

TaskHandle_t low_priority;
TaskHandle_t high_priority;

SemaphoreHandle_t semaphore;

void higher_priority(void *param)
{
    while (1)
    {
        printf("We are in the higher priority task\n");

        xSemaphoreTake(semaphore, portMAX_DELAY); // Blocks until semaphore can be taken

        printf("We are still in the higher priority task\n");

        xSemaphoreGive(semaphore);
    }
}

void lower_priority(void *param)
{
    uint8_t count = 0;
    bool led_state = true;

    while (1)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

        xSemaphoreTake(semaphore, portMAX_DELAY); // Blocks until semaphore can be taken

        if (count == 0)
        {
            xTaskCreate(higher_priority, "HigherPriorityTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &high_priority);
            count = 1;
        }

        while (1)
        {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
            led_state = !led_state;
            vTaskDelay(100);
        }
    }
}

void main()
{
    hard_assert(cyw43_arch_init() == PICO_OK);     // Initializes the on-board LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0); // Start the on-board LED as off
    semaphore = xSemaphoreCreateBinary();
    xTaskCreate(lower_priority, "LowerPriorityTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &low_priority);
    vTaskStartScheduler();
}