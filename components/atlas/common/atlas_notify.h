#ifndef COMMON_ATLAS_NOTIFY_H
#define COMMON_ATLAS_NOTIFY_H

#include "atlas_config.h"

typedef enum {
    JOINT_NOTIFY_DELTA_TIMER = (1 << 0),
    JOINT_NOTIFY_PWM_PULSE = (1 << 1),
    JOINT_NOTIFY_ALL = (JOINT_NOTIFY_DELTA_TIMER | JOINT_NOTIFY_PWM_PULSE),
} joint_notify_t;

typedef enum {
    JOINTS_NOTIFY_JOINT_READY = ((1 << JOINT_NUM_1) | (1 << JOINT_NUM_2) | (1 << JOINT_NUM_3) |
                                 (1 << JOINT_NUM_4) | (1 << JOINT_NUM_5) | (1 << JOINT_NUM_6)),
    JOINTS_NOTIFY_DELTA_TIMER = (1 << 6),
    JOINTS_NOTIFY_ALL = (JOINTS_NOTIFY_JOINT_READY | JOINTS_NOTIFY_DELTA_TIMER),
} joints_notify_t;

typedef enum {
    KINEMATICS_NOTIFY_JOINTS_READY = (1 << 0),
    KINEMATICS_NOTIFY_ALL = (KINEMATICS_NOTIFY_JOINTS_READY),
} kinematics_notify_t;

#endif // COMMON_ATLAS_NOTIFY_H