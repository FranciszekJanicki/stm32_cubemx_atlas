#include "stream_buffer_manager.h"
#include <assert.h>

static StreamBufferHandle_t stream_buffers[STREAM_BUFFER_TYPE_STREAM_BUFFER_NUM];

void stream_buffer_manager_set(StreamBufferType_t type, StreamBufferHandle_t stream_buffer)
{
    assert(stream_buffer);
    assert(type < STREAM_BUFFER_TYPE_STREAM_BUFFER_NUM);
    assert(!stream_buffers[type]);

    stream_buffers[type] = stream_buffer;
}

StreamBufferHandle_t stream_buffer_manager_get(StreamBufferType_t type)
{
    assert(type < STREAM_BUFFER_TYPE_STREAM_BUFFER_NUM);
    assert(stream_buffers[type]);

    return stream_buffers[type];
}
