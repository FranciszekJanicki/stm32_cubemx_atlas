#ifndef JOINTS_TASK_JOINTS_MANAGER_H
#define JOINTS_TASK_JOINTS_MANAGER_H

#include "FreeRTOS.h"
#include "atlas_config.h"
#include "atlas_err.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

typedef struct {
    bool is_running;

    struct {
        TaskHandle_t task;
        QueueHandle_t queue;
    } joint_ctxs[JOINT_NUM];
} joints_manager_t;

atlas_err_t joints_manager_initialize(joints_manager_t* manager);
atlas_err_t joints_manager_process(joints_manager_t* manager);

#endif // JOINTS_TASK_JOINTS_MANAGER_H