#ifndef JOINTS_TASK_JOINTS_MANAGER_H
#define JOINTS_TASK_JOINTS_MANAGER_H

#include "FreeRTOS.h"
#include "config.h"
#include "joint_manager.h"
#include "joint_task.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

typedef struct {
    bool is_running;

    struct {
        QueueHandle_t queue;
        joint_event_t event;
        TaskHandle_t task;
    } joints[JOINT_NUM];
} joints_manager_t;

typedef struct {
    joint_task_ctx_t joints_ctxs[JOINT_NUM];
} joints_config_t;

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

typedef enum {
    JOINTS_ERR_OK,
    JOINTS_ERR_FAIL,
    JOINTS_ERR_NOT_RUNNING,
    JOINTS_ERR_ALREADY_RUNNING,
    JOINTS_ERR_UNKNOWN_EVENT,
    JOINTS_ERR_UNKNOWN_NOTIFY,
} joints_err_t;

joints_err_t joints_manager_initialize(joints_manager_t* manager, joints_config_t* config);
joints_err_t joints_manager_process(joints_manager_t* manager);

#endif // JOINTS_TASK_JOINTS_MANAGER_H