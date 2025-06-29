#include "uart_task.h"
#include "FreeRTOS.h"
#include "atlas_config.h"
#include "atlas_notify.h"
#include "atlas_utility.h"
#include "stream_buffer.h"
#include "stream_buffer_manager.h"
#include "task.h"
#include "task_manager.h"
#include "uart_manager.h"
#include "usart.h"
#include <stdint.h>
#include <string.h>

#define UART_TASK_STACK_DEPTH (2048U / sizeof(StackType_t))
#define UART_TASK_PRIORITY (1U)

#define UART_STREAM_BUFFER_STORAGE_SIZE (1024U)
#define UART_STREAM_BUFFER_TRIGGER (10U)

static void uart_task_func(void*)
{
    uart_manager_t manager;
    uart_manager_initialize(&manager, stream_buffer_manager_get(STREAM_BUFFER_TYPE_UART), &huart2);

    while (1) {
        uart_manager_process(&manager);
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