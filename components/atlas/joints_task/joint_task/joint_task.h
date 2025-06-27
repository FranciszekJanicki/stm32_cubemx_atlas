#ifndef ATLAS_JOINT_TASK_JOINT_TASK_H
#define ATLAS_JOINT_TASK_JOINT_TASK_H

#include "config.h"
#include "joint_manager.h"

joint_err_t joint_task_initialize(joint_num_t num);
joint_err_t joint_queue_initialize(joint_num_t num);

#endif // ATLAS_JOINT_TASK_JOINT_TASK_H