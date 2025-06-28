#ifndef COMMON_ATLAS_EVENT_H
#define COMMON_ATLAS_EVENT_H

#include "atlas_config.h"

typedef enum {
    JOINT_EVENT_TYPE_START,
    JOINT_EVENT_TYPE_STOP,
    JOINT_EVENT_TYPE_UPDATE,
} joint_event_type_t;

typedef struct {
} joint_event_payload_start_t;

typedef struct {
} joint_event_payload_stop_t;

typedef struct {
    float joint_position;
    float delta_time;
} joint_event_payload_update_t;

typedef union {
    joint_event_payload_start_t start;
    joint_event_payload_stop_t stop;
    joint_event_payload_update_t update;
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

typedef struct {
} joints_event_payload_start_t;

typedef struct {
} joints_event_payload_stop_t;

typedef struct {
    joint_space_t joint_space;
} joints_event_payload_update_t;

typedef union {
    joints_event_payload_start_t start;
    joints_event_payload_stop_t stop;
    joints_event_payload_update_t update;
} joints_event_payload_t;

typedef struct {
    joints_event_type_t type;
    joints_event_payload_t payload;
} joints_event_t;

typedef enum {
    KINEMATICS_EVENT_TYPE_START,
    KINEMATICS_EVENT_TYPE_STOP,
    KINEMATICS_EVENT_TYPE_DIRECT,
    KINEMATICS_EVENT_TYPE_INVERSE,
} kinematics_event_type_t;

typedef struct {
} kinematics_event_payload_start_t;

typedef struct {
} kinematics_event_payload_stop_t;

typedef struct {
    joint_space_t joint_space;
} kinematics_event_payload_direct_t;

typedef struct {
    cartesian_space_t cartesian_space;
} kinematics_event_payload_inverse_t;

typedef union {
    kinematics_event_payload_start_t start;
    kinematics_event_payload_stop_t stop;
    kinematics_event_payload_direct_t direct;
    kinematics_event_payload_inverse_t inverse;
} kinematics_event_payload_t;

typedef struct {
    kinematics_event_type_t type;
    kinematics_event_payload_t payload;
} kinematics_event_t;

#endif // COMMON_ATLAS_EVENT_H