#include "queue_manager.h"
#include <assert.h>

static QueueHandle_t queues[QUEUE_TYPE_QUEUE_NUM];

void queue_manager_set(QueueType_t type, QueueHandle_t queue)
{
    assert(queue);
    assert(type < QUEUE_TYPE_QUEUE_NUM);
    assert(!queues[type]);

    queues[type] = queue;
}

QueueHandle_t queue_manager_get(QueueType_t type)
{
    assert(type < QUEUE_TYPE_QUEUE_NUM);
    assert(queues[type]);

    return queues[type];
}
