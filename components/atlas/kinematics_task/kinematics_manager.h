#ifndef ATLAS_KINEMATICS_MANAGER_KINEMATICS_MANAGER_H
#define ATLAS_KINEMATICS_MANAGER_KINEMATICS_MANAGER_H

#include "kinematics_config.h"

typedef struct {
    bool is_running;

} kinematics_manager_t;

kinematics_err_t kinematics_manager_initialize(kinematics_manager_t* manager);

#endif // ATLAS_KINEMATICS_MANAGER_KINEMATICS_MANAGER_H