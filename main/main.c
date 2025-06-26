#include "main.h"
#include "FreeRTOS.h"
#include "gpio.h"
#include "joint_task.h"
#include "task.h"
#include "tim.h"
#include "usart.h"
#include "config.h"
#include "task_manager.h"
#include <assert.h>
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

    assert(joint_queues_initialize() == JOINT_ERR_OK);
    assert(joint_tasks_initialize() == JOINT_ERR_OK);

    vTaskStartScheduler();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    BaseType_t task_woken = pdTRUE;

    if (htim->Instance == TIM2) {
        xTaskNotifyFromISR(task_manager_get(TASK_TYPE_JOINT_1),
                           JOINT_NOTIFY_DELTA_TIMER,
                           eSetBits,
                           &task_woken);
    } else if (htim->Instance == TIM3) {
        HAL_IncTick();
    }

    portYIELD_FROM_ISR(task_woken);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    BaseType_t task_woken = pdTRUE;

    if (htim->Instance == TIM1) {
        xTaskNotifyFromISR(task_manager_get(TASK_TYPE_JOINT_1),
                           JOINT_NOTIFY_PWM_PULSE,
                           eSetBits,
                           &task_woken);
    }

    portYIELD_FROM_ISR(task_woken);
}