#ifndef ATLAS_JOINT_TASK_JOINT_TASK_H
#define ATLAS_JOINT_TASK_JOINT_TASK_H

#include "config.h"
#include "joint_manager.h"

joint_err_t joint_task_initialize(joint_num_t num);
joint_err_t joint_queue_initialize(joint_num_t num);

joint_err_t joint_tasks_initialize(void);
joint_err_t joint_queues_initialize(void);

#endif // ATLAS_JOINT_TASK_JOINT_TASK_H