#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#define ERROR_CHECK(X)    \
    do {                  \
        assert((X) == 0); \
    } while (0)

#define RET_ON_ERROR(X)    \
    do {                   \
        error_t err = (X); \
        if (err != 0)      \
            return err;    \
    } while (0)

#define LOG_ON_ERROR(TAG, X)                      \
    do {                                          \
        error_t err = (X);                        \
        if (err != 0)                             \
            LOG(TAG, "%s", error_to_string(err)); \
    } while (0)

#define LOG(TAG, FMT, ...) printf("[%s] " FMT "\n\r", TAG, ##__VA_ARGS__)

#define DEBUG

#define JOINTS_DELTA_TIMER_TIMEOUT_S (0.01F)

typedef enum {
    JOINT_NUM_1,
    JOINT_NUM_2,
    JOINT_NUM_3,
    JOINT_NUM_4,
    JOINT_NUM_5,
    JOINT_NUM_6,
    JOINT_NUM,
} joint_num_t;

#endif // COMMON_CONFIG_H