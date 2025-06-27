#ifndef JOINT_TASK_JOINT_CONFIG_H
#define JOINT_TASK_JOINT_CONFIG_H

#include "pid_regulator_config.h"

typedef enum {
    JOINT_ERR_OK,
    JOINT_ERR_FAIL,
    JOINT_ERR_NOT_RUNNING,
    JOINT_ERR_ALREADY_RUNNING,
    JOINT_ERR_UNKNOWN_EVENT,
    JOINT_ERR_UNKNOWN_NOTIFY,
} joint_err_t;

typedef struct {
    float32_t kp;
    float32_t ki;
    float32_t kd;
    float32_t kc;
    float32_t min_angle;
    float32_t max_angle;
    float32_t min_speed;
    float32_t max_speed;
    float32_t step_change;
    float32_t current_limit;
} joint_config_t;

typedef enum {
    JOINT_EVENT_TYPE_START,
    JOINT_EVENT_TYPE_STOP,
    JOINT_EVENT_TYPE_UPDATE,
} joint_event_type_t;

typedef union {
    struct {
        float32_t position;
        float32_t delta_time;
    } update;
} joint_event_payload_t;

typedef struct {
    joint_event_type_t type;
    joint_event_payload_t payload;
} joint_event_t;

typedef enum {
    JOINT_NOTIFY_DELTA_TIMER = (1 << 0),
    JOINT_NOTIFY_PWM_PULSE = (1 << 1),
    JOINT_NOTIFY_ALL = (JOINT_NOTIFY_DELTA_TIMER | JOINT_NOTIFY_PWM_PULSE),
} joint_notify_t;

#endif // JOINT_TASK_JOINT_CONFIG_H