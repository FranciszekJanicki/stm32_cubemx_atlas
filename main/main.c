#include "main.h"
#include "FreeRTOS.h"
#include "gpio.h"
#include "joints_task.h"
#include "kinematics_task.h"
#include "task.h"
#include "tim.h"
#include "uart_task.h"
#include "usart.h"
#include <string.h>

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();

    HAL_Delay(200);

    uart_stream_buffer_initialize();
    joints_queue_initialize();
    kinematics_queue_initialize();

    joints_task_initialize();
    uart_task_initialize();
    kinematics_task_initialize();

    vTaskStartScheduler();
}
