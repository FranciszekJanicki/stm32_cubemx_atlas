#ifndef JOINTS_TASK_JOINTS_CONFIG_H
#define JOINTS_TASK_JOINTS_CONFIG_H

#include "joint_manager.h"
#include "joint_task.h"

typedef enum {
    JOINTS_ERR_OK,
    JOINTS_ERR_FAIL,
    JOINTS_ERR_NOT_RUNNING,
    JOINTS_ERR_ALREADY_RUNNING,
    JOINTS_ERR_UNKNOWN_EVENT,
    JOINTS_ERR_UNKNOWN_NOTIFY,
} joints_err_t;

typedef enum {
    JOINTS_EVENT_TYPE_START,
    JOINTS_EVENT_TYPE_STOP,
    JOINTS_EVENT_TYPE_UPDATE,
} joints_event_type_t;

typedef union {
    struct {
        float32_t position;
        float32_t delta_time;
    } update[JOINT_NUM];
} joints_event_payload_t;

typedef struct {
    joints_event_type_t type;
    joints_event_payload_t payload;
} joints_event_t;

typedef enum {
    JOINTS_NOTIFY_DELTA_TIMER = (1 << 0),
    JOINTS_NOTIFY_ALL = (JOINTS_NOTIFY_DELTA_TIMER),
} joints_notify_t;

#endif // JOINTS_TASK_JOINTS_CONFIG_H