#ifndef ATLAS_TASK_MANAGER_TASK_MANAGER_H
#define ATLAS_TASK_MANAGER_TASK_MANAGER_H

#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

typedef enum {
    TASK_TYPE_JOINTS,
    TASK_TYPE_KINEMATICS,
    TASK_TYPE_UART,
    TASK_TYPE_TASK_NUM,
} TaskType_t;

void task_manager_set(TaskType_t type, TaskHandle_t task);
TaskHandle_t task_manager_get(TaskType_t type);

#endif // ATLAS_TASK_MANAGER_TASK_MANAGER_H