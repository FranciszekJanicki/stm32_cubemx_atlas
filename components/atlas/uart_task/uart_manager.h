#ifndef UART_TASK_UART_MANAGER_H
#define UART_TASK_UART_MANAGER_H

#include "FreeRTOS.h"
#include "atlas_err.h"
#include "stm32l4xx_hal.h"
#include "stream_buffer.h"
#include <stdint.h>

typedef struct {
    StreamBufferHandle_t stream_buffer;
    UART_HandleTypeDef* uart;
    uint8_t uart_buffer[100];
} uart_manager_t;

atlas_err_t uart_manager_initialize(uart_manager_t* manager,
                                    StreamBufferHandle_t stream_buffer,
                                    UART_HandleTypeDef* uart);
atlas_err_t uart_manager_process(uart_manager_t* manager);

#endif // UART_TASK_UART_MANAGER_H