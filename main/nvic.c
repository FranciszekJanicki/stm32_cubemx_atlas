#include "FreeRTOS.h"
#include "atlas_notify.h"
#include "task.h"
#include "task_manager.h"
#include "tim.h"

__attribute__((used)) void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
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

__attribute__((used)) void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    BaseType_t task_woken = pdFALSE;

    if (huart->Instance == USART2) {
        xTaskNotifyFromISR(task_manager_get(TASK_TYPE_UART),
                           UART_NOTIFY_TX_CPLT,
                           eSetBits,
                           &task_woken);
    }

    portYIELD_FROM_ISR(task_woken);
}