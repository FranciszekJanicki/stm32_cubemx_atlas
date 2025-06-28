#ifndef JOINTS_TASK_JOINTS_MANAGER_H
#define JOINTS_TASK_JOINTS_MANAGER_H

#include "FreeRTOS.h"
#include "config.h"
#include "err.h"
#include "queue.h"
#include "task.h"
#include "tim.h"
#include <stdbool.h>

typedef struct {
    bool is_running;

    TIM_HandleTypeDef* delta_timer;

    struct {
        TaskHandle_t task;
        QueueHandle_t queue;
        bool is_ready;
    } joint_ctxs[JOINT_NUM];
} joints_manager_t;

error_t joints_manager_initialize(joints_manager_t* manager);
error_t joints_manager_process(joints_manager_t* manager);

#endif // JOINTS_TASK_JOINTS_MANAGER_H