#ifndef JOINT_TASK_JOINTS_TASK_H
#define JOINT_TASK_JOINTS_TASK_H

#include "config.h"
#include "joint_manager.h"

typedef struct {
    float32_t joint_angles[JOINT_NUM];
} joints_event_t;

joint_err_t joints_task_initialize(void);
joint_err_t joints_queue_initialize(void);

#endif // JOINT_TASK_JOINTS_TASK_H