#include "FreeRTOS.h"
#include "stream_buffer_manager.h"
#include "task.h"

__attribute__((used)) int _write(int file, char* ptr, int len)
{
    if (!xPortIsInsideInterrupt()) {
        return xStreamBufferSend(stream_buffer_manager_get(STREAM_BUFFER_TYPE_UART),
                                 ptr,
                                 len,
                                 pdMS_TO_TICKS(10));
    }

    return 0;
}
