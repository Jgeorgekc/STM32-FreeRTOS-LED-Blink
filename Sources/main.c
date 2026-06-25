#include "FreeRTOS.h"
#include "task.h"

#define RCC_AHB1ENR (*(volatile uint32_t*)0x40023830)
#define GPIOD_MODER (*(volatile uint32_t*)0x40020C00)
#define GPIOD_ODR   (*(volatile uint32_t*)0x40020C14)
#define SCB_CPACR   (*(volatile uint32_t*)0xE000ED88)

/*==========================
  Task 1 : Green LED
==========================*/
void GreenTask(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        GPIOD_ODR ^= (1 << 12);      // Toggle Green LED
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/*==========================
  Task 2 : Orange LED
==========================*/
void OrangeTask(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        GPIOD_ODR ^= (1 << 13);      // Toggle Orange LED
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    /* Enable FPU */
    SCB_CPACR |= (0xF << 20);

    /* Enable GPIOD Clock */
    RCC_AHB1ENR |= (1 << 3);

    /* PD12 -> Output (Green LED) */
    GPIOD_MODER &= ~(3U << (12 * 2));
    GPIOD_MODER |=  (1U << (12 * 2));

    /* PD13 -> Output (Orange LED) */
    GPIOD_MODER &= ~(3U << (13 * 2));
    GPIOD_MODER |=  (1U << (13 * 2));

    /* Create Tasks */
    xTaskCreate(
        GreenTask,
        "Green",
        256,
        NULL,
        1,
        NULL);

    xTaskCreate(
        OrangeTask,
        "Orange",
        256,
        NULL,
        1,
        NULL);

    /* Start Scheduler */
    vTaskStartScheduler();

    /* Should never reach here */
    while (1)
    {
    }
}
