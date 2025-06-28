#ifndef ATLAS_COMMON_ATLAS_CONFIG_H
#define ATLAS_COMMON_ATLAS_CONFIG_H

#define ATLAS_ERR_CHECK(X) \
    do {                   \
        assert((X) == 0);  \
    } while (0)

#define ATLAS_RET_ON_ERR(X)    \
    do {                       \
        atlas_err_t err = (X); \
        if (err != 0)          \
            return err;        \
    } while (0)

#define ATLAS_LOG_ON_ERR(TAG, X)                            \
    do {                                                    \
        atlas_err_t err = (X);                              \
        if (err != 0)                                       \
            ATLAS_LOG(TAG, "%s", atlas_err_to_string(err)); \
    } while (0)

#define ATLAS_LOG(TAG, FMT, ...) printf("[%s] " FMT "\n\r", TAG, ##__VA_ARGS__)

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

#endif // ATLAS_COMMON_ATLAS_CONFIG_H