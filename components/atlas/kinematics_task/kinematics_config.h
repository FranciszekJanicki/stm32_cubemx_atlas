#ifndef KINEMATICS_TASK_KINEMATICS_CONFIG_H
#define KINEMATICS_TASK_KINEMATICS_CONFIG_H

typedef enum {
    KINEMATICS_ERR_OK,
    KINEMATICS_ERR_FAIL,
    KINEMATICS_ERR_ALREADY_RUNNING,
    KINEMATICS_ERR_NOT_RUNNING,
    KINEMATICS_ERR_UNKNOWN_EVENT,
    KINEMATICS_ERR_UNKNOWN_NOTIFY,
} kinematics_err_t;

typedef enum {
    KINEMATICS_NOTIFY_ALL = 0,
} kinematics_notify_t;

typedef enum {
    KINEMATICS_EVENT_TYPE_START,
    KINEMATICS_EVENT_TYPE_STOP,
} kinematics_event_type_t;

typedef union {
} kinematics_event_payload_t;

typedef struct {
    kinematics_event_type_t type;
    kinematics_event_payload_t payload;
} kinematics_event_t;

#endif // KINEMATICS_TASK_KINEMATICS_CONFIG_H