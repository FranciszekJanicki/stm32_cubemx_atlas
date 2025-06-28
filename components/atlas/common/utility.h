#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

#define ERR_CHECK(X)      \
    do {                  \
        assert((X) == 0); \
    } while (0)

#define RET_ON_ERR(X)   \
    do {                \
        int err = (X);  \
        if (err != 0) { \
            return err; \
        }               \
    } while (0)

#define LOG_ON_ERR(TAG, X)       \
    do {                         \
        int err = (X);           \
        if (err != 0) {          \
            LOG(TAG, "%d", err); \
        }                        \
    } while (0)

#define DEBUG

#ifdef DEBUG
#define LOG(TAG, FMT, ...) printf("[%s] " FMT "\n\r", TAG, ##__VA_ARGS__)
#else
#define LOG(TAG, FMT, ...)
#endif

#endif // COMMON_UTILITY_H