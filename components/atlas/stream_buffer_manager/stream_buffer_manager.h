#ifndef ATLAS_STREAM_BUFFER_MANAGER_STREAM_BUFFER_MANAGER_H
#define ATLAS_STREAM_BUFFER_MANAGER_STREAM_BUFFER_MANAGER_H

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include <stdint.h>

typedef enum {
    STREAM_BUFFER_TYPE_UART,
    STREAM_BUFFER_TYPE_STREAM_BUFFER_NUM,
} StreamBufferType_t;

void stream_buffer_manager_set(StreamBufferType_t type, StreamBufferHandle_t stream_buffer);
StreamBufferHandle_t stream_buffer_manager_get(StreamBufferType_t type);

#endif // ATLAS_STREAM_BUFFER_MANAGER_STREAM_BUFFER_MANAGER_H