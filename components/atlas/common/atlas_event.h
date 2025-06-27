#ifndef ATLAS_COMMON_ATLAS_EVENT_H
#define ATLAS_COMMON_ATLAS_EVENT_H

#include "atlas_config.h"

typedef enum {
    JOINT_EVENT_TYPE_START,
    JOINT_EVENT_TYPE_STOP,
    JOINT_EVENT_TYPE_UPDATE,
} joint_event_type_t;

typedef union {
    struct {
        float position;
        float delta_time;
    } update;
} joint_event_payload_t;

typedef struct {
    joint_event_type_t type;
    joint_event_payload_t payload;
} joint_event_t;

typedef enum {
    JOINTS_EVENT_TYPE_START,
    JOINTS_EVENT_TYPE_STOP,
    JOINTS_EVENT_TYPE_UPDATE,
} joints_event_type_t;

typedef union {
    struct {
        float position;
        float delta_time;
    } update[JOINT_NUM];
} joints_event_payload_t;

typedef struct {
    joints_event_type_t type;
    joints_event_payload_t payload;
} joints_event_t;

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

#endif // ATLAS_COMMON_ATLAS_EVENT_H