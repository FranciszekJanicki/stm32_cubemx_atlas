#include "FreeRTOS.h"
#include "tim.h"
#include "usart.h"

extern void joints_delta_timer_callback(void);
extern void kinematics_delta_timer_callback(void);
extern void joint_pwm_pulse_callback(void);
extern void uart_transmit_cplt_callback(void);

__attribute__((used)) void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM4) {
        HAL_IncTick();
    } else if (htim->Instance == TIM2) {
        joints_delta_timer_callback();
    } else if (htim->Instance == TIM3) {
        kinematics_delta_timer_callback();
    }
}

__attribute__((used)) void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == USART2) {
        uart_transmit_cplt_callback();
    }
}