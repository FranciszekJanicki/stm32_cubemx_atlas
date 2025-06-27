#include "main.h"
#include "FreeRTOS.h"
#include "atlas_notify.h"
#include "gpio.h"
#include "joints_task.h"
#include "kinematics_task.h"
#include "task.h"
#include "task_manager.h"
#include "tim.h"
#include "uart_task.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();

    HAL_Delay(500);

    uart_stream_buffer_initialize();
    uart_task_initialize();

    kinematics_queue_initialize();
    kinematics_task_initialize();

    vTaskStartScheduler();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    BaseType_t task_woken = pdFALSE;

    if (htim->Instance == TIM4) {
        HAL_IncTick();
    } else if (htim->Instance == TIM2) {
        xTaskNotifyFromISR(task_manager_get(TASK_TYPE_JOINTS),
                           JOINTS_NOTIFY_DELTA_TIMER,
                           eSetBits,
                           &task_woken);
    }

    portYIELD_FROM_ISR(task_woken);
}
