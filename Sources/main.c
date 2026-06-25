#include "FreeRTOS.h"
#include "task.h"

#define RCC_AHB1ENR   (*(volatile uint32_t*)0x40023830)

#define GPIOA_MODER   (*(volatile uint32_t*)0x40020000)
#define GPIOA_IDR     (*(volatile uint32_t*)0x40020010)
#define GPIOA_PUPDR   (*(volatile uint32_t*)0x4002000C)

#define GPIOD_MODER   (*(volatile uint32_t*)0x40020C00)
#define GPIOD_ODR     (*(volatile uint32_t*)0x40020C14)

#define SCB_CPACR     (*(volatile uint32_t*)0xE000ED88)

TaskHandle_t LedTaskHandle = NULL;

/*---------------- LED Task ----------------*/
void LedTask(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        GPIOD_ODR |= (1 << 12);
        vTaskDelay(pdMS_TO_TICKS(500));

        GPIOD_ODR |= (1 << 13);
        vTaskDelay(pdMS_TO_TICKS(500));

        GPIOD_ODR |= (1 << 14);
        vTaskDelay(pdMS_TO_TICKS(500));

        GPIOD_ODR |= (1 << 15);
        vTaskDelay(pdMS_TO_TICKS(500));

        GPIOD_ODR &= ~((1<<12)|(1<<13)|(1<<14)|(1<<15));
    }
}

/*---------------- Button Task ----------------*/
void ButtonTask(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        if (GPIOA_IDR & (1 << 0))
        {
            vTaskDelay(pdMS_TO_TICKS(20));

            if (GPIOA_IDR & (1 << 0))
            {
                xTaskNotifyGive(LedTaskHandle);

                while (GPIOA_IDR & (1 << 0));

                vTaskDelay(pdMS_TO_TICKS(20));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int main(void)
{
    /* Enable FPU */
    SCB_CPACR |= (0xF << 20);

    /* Enable GPIOA & GPIOD Clock */
    RCC_AHB1ENR |= (1<<0);
    RCC_AHB1ENR |= (1<<3);

    /* PA0 Input */
    GPIOA_MODER &= ~(3U << (0*2));

    /* Pull-down on PA0 */
    GPIOA_PUPDR &= ~(3U << (0*2));
    GPIOA_PUPDR |=  (2U << (0*2));

    /* PD12-PD15 Output */
    for(int i=12;i<=15;i++)
    {
        GPIOD_MODER &= ~(3U << (i*2));
        GPIOD_MODER |=  (1U << (i*2));
    }

    xTaskCreate(
        LedTask,
        "LED",
        256,
        NULL,
        2,
        &LedTaskHandle);

    xTaskCreate(
        ButtonTask,
        "BUTTON",
        256,
        NULL,
        3,
        NULL);

    vTaskStartScheduler();

    while(1);
}
