#include "FreeRTOS.h"
#include "atlas_config.h"
#include "stm32l4xx_hal.h"
#include "stream_buffer.h"
#include "stream_buffer_manager.h"
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
    // HAL_UART_Transmit(&huart2, ptr, len, 100);
#endif

    return len;
}