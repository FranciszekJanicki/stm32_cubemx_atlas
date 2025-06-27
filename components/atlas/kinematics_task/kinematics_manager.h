#ifndef ATLAS_KINEMATICS_MANAGER_KINEMATICS_MANAGER_H
#define ATLAS_KINEMATICS_MANAGER_KINEMATICS_MANAGER_H

#include "atlas_err.h"
#include <stdbool.h>

typedef struct {
    bool is_running;
} kinematics_manager_t;

atlas_err_t kinematics_manager_initialize(kinematics_manager_t* manager);

#endif // ATLAS_KINEMATICS_MANAGER_KINEMATICS_MANAGER_H