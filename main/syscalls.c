#include "FreeRTOS.h"
#include "stdio.h"
#include "stream_buffer.h"
#include "stream_buffer_manager.h"
#include "task.h"
#include "usart.h"
#include <string.h>

int _write(int file, char* ptr, int len)
{
    if (!xPortIsInsideInterrupt()) {
        xStreamBufferSend(stream_buffer_manager_get(STREAM_BUFFER_TYPE_UART),
                          ptr,
                          len,
                          pdMS_TO_TICKS(1));
        return len;
    }

    return 0;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char* pcTaskName)
{
    taskDISABLE_INTERRUPTS();
    for (;;)
        HAL_UART_Transmit(&huart2,
                          (uint8_t*)"vApplicationStackOverflowHook\n\r",
                          strlen("vApplicationStackOverflowHook\n\r"),
                          100);
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;)
        HAL_UART_Transmit(&huart2,
                          (uint8_t*)"vApplicationMallocFailedHook\n\r",
                          strlen("vApplicationMallocFailedHook\n\r"),
                          100);
}