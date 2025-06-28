#include "uart_task.h"
#include "FreeRTOS.h"
#include "atlas_config.h"
#include "stream_buffer.h"
#include "stream_buffer_manager.h"
#include "task.h"
#include "task_manager.h"
#include "usart.h"
#include <stdint.h>
#include <string.h>

#define UART_TASK_STACK_DEPTH (1024U / sizeof(StackType_t))
#define UART_TASK_PRIORITY (1U)

#define UART_STREAM_BUFFER_STORAGE_SIZE (1024U)
#define UART_STREAM_BUFFER_TRIGGER (100U)

static void uart_task_func(void*)
{
    static uint8_t buffer[100];
    memset(buffer, 0, sizeof(buffer));

    while (1) {
        size_t bytes_received =
            xStreamBufferReceive(stream_buffer_manager_get(STREAM_BUFFER_TYPE_UART),
                                 buffer,
                                 sizeof(buffer),
                                 pdMS_TO_TICKS(100));

        HAL_UART_Transmit(&huart2, buffer, bytes_received, 100);
        memset(buffer, 0, sizeof(buffer));

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void uart_task_initialize(void)
{
    static StaticTask_t uart_task_buffer;
    static StackType_t uart_task_stack[UART_TASK_STACK_DEPTH];

    TaskHandle_t uart_task = xTaskCreateStatic(uart_task_func,
                                               "uart_task",
                                               UART_TASK_STACK_DEPTH,
                                               NULL,
                                               UART_TASK_PRIORITY,
                                               uart_task_stack,
                                               &uart_task_buffer);

    task_manager_set(TASK_TYPE_UART, uart_task);
}

void uart_stream_buffer_initialize(void)
{
    static StaticStreamBuffer_t uart_stream_buffer_buffer;
    static uint8_t uart_stream_buffer_storage[UART_STREAM_BUFFER_STORAGE_SIZE];

    StreamBufferHandle_t uart_stream_buffer =
        xStreamBufferCreateStatic(UART_STREAM_BUFFER_STORAGE_SIZE,
                                  UART_STREAM_BUFFER_TRIGGER,
                                  uart_stream_buffer_storage,
                                  &uart_stream_buffer_buffer);

    stream_buffer_manager_set(STREAM_BUFFER_TYPE_UART, uart_stream_buffer);
}

#undef UART_TASK_STACK_DEPTH
#undef UART_TASK_PRIORITY

#undef UART_STREAM_BUFFER_STORAGE_SIZE
#undef UART_STREAM_BUFFER_TRIGGER