#ifndef QUEUE_MANAGER_QUEUE_MANAGER_H
#define QUEUE_MANAGER_QUEUE_MANAGER_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>

typedef enum {
    QUEUE_TYPE_JOINTS,
    QUEUE_TYPE_KINEMATICS,
    QUEUE_TYPE_QUEUE_NUM,
} QueueType_t;

void queue_manager_set(QueueType_t type, QueueHandle_t queue);
QueueHandle_t queue_manager_get(QueueType_t type);

#endif // QUEUE_MANAGER_QUEUE_MANAGER_H