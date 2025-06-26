#ifndef ATLAS_QUEUE_MANAGER_QUEUE_MANAGER_H
#define ATLAS_QUEUE_MANAGER_QUEUE_MANAGER_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>

typedef enum {
    QUEUE_TYPE_JOINT_1,
    QUEUE_TYPE_JOINT_2,
    QUEUE_TYPE_JOINT_3,
    QUEUE_TYPE_JOINT_4,
    QUEUE_TYPE_JOINT_5,
    QUEUE_TYPE_JOINT_6,
    QUEUE_TYPE_QUEUE_NUM,
} QueueType_t;

void queue_manager_set(QueueType_t type, QueueHandle_t queue);
QueueHandle_t queue_manager_get(QueueType_t type);

#endif // ATLAS_QUEUE_MANAGER_QUEUE_MANAGER_H