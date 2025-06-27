#ifndef ATLAS_COMMON_ATLAS_NOTIFY_H
#define ATLAS_COMMON_ATLAS_NOTIFY_H

typedef enum {
    JOINT_NOTIFY_DELTA_TIMER = (1 << 0),
    JOINT_NOTIFY_PWM_PULSE = (1 << 1),
    JOINT_NOTIFY_ALL = (JOINT_NOTIFY_DELTA_TIMER | JOINT_NOTIFY_PWM_PULSE),
} joint_notify_t;

typedef enum {
    JOINTS_NOTIFY_DELTA_TIMER = (1 << 0),
    JOINTS_NOTIFY_ALL = (JOINTS_NOTIFY_DELTA_TIMER),
} joints_notify_t;

typedef enum {
    KINEMATICS_NOTIFY_ALL = 0,
} kinematics_notify_t;

#endif // ATLAS_COMMON_ATLAS_NOTIFY_H