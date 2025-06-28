#ifndef COMMON_ATLAS_CONFIG_H
#define COMMON_ATLAS_CONFIG_H

#define JOINTS_DELTA_TIMER_TIMEOUT_S (0.01F)

typedef enum {
    JOINT_NUM_1,
    JOINT_NUM_2,
    JOINT_NUM_3,
    JOINT_NUM_4,
    JOINT_NUM_5,
    JOINT_NUM_6,
    JOINT_NUM = 1,
} joint_num_t;

typedef float float32_t;

typedef struct {
    float32_t x;
    float32_t y;
    float32_t z;
} vec3_float32_t;

typedef struct {
    vec3_float32_t tool_position;
    vec3_float32_t tool_orientation;
} cartesian_space_t;

typedef struct {
    float32_t joint_position[JOINT_NUM];
} joint_space_t;

#endif // COMMON_ATLAS_CONFIG_H