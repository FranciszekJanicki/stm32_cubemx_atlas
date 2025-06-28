#ifndef KINEMATICS_MANAGER_KINEMATICS_MANAGER_H
#define KINEMATICS_MANAGER_KINEMATICS_MANAGER_H

#include "err.h"
#include <stdbool.h>

typedef struct {
    bool is_running;
    bool is_joints_ready;
} kinematics_manager_t;

error_t kinematics_manager_initialize(kinematics_manager_t* manager);
error_t kinematics_manager_process(kinematics_manager_t* manager);

#endif // KINEMATICS_MANAGER_KINEMATICS_MANAGER_H