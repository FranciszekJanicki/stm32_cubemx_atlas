#include "FreeRTOS.h"
#include "atlas_config.h"
#include "stdio.h"
#include "stream_buffer.h"
#include "stream_buffer_manager.h"
#include "task.h"
#include "usart.h"

int _write(int file, char* ptr, int len)
{
#ifdef DEBUG
    if (!xPortIsInsideInterrupt()) {
        xStreamBufferSend(stream_buffer_manager_get(STREAM_BUFFER_TYPE_UART),
                          ptr,
                          len,
                          pdMS_TO_TICKS(1));
    }
#endif
    return len;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    printf("Stack overflow in task: %s\n", pcTaskName);
    while (1)
        ;
}

void vApplicationMallocFailedHook(void)
{
    printf("Malloc failed!\n");
    while (1)
        ;
}
