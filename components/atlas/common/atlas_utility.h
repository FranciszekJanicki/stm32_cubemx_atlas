#ifndef COMMON_ATLAS_UTILITY_H
#define COMMON_ATLAS_UTILITY_H

#include "stm32l4xx.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ATLAS_DEBUG

#define ATLAS_PANIC()             \
    do {                          \
        taskDISABLE_INTERRUPTS(); \
        while (1)                 \
            ;                     \
    } while (0)

#define ATLAS_ERR_CHECK(ERR)         \
    do {                             \
        if ((ERR) != ATLAS_ERR_OK) { \
            ATLAS_PANIC();           \
        }                            \
    } while (0)

#define ATLAS_RET_ON_ERR(ERR)      \
    do {                           \
        atlas_err_t err = (ERR);   \
        if (err != ATLAS_ERR_OK) { \
            return err;            \
        }                          \
    } while (0)

#ifdef ATLAS_DEBUG

#define ATLAS_LOG(TAG, FMT, ...) printf("[%s] " FMT "\n\r", TAG, ##__VA_ARGS__)

#define ATLAS_LOG_ON_ERR(TAG, ERR)                          \
    do {                                                    \
        atlas_err_t err = (ERR);                            \
        if (err != ATLAS_ERR_OK) {                          \
            ATLAS_LOG(TAG, "%s", atlas_err_to_string(err)); \
        }                                                   \
    } while (0)

#define ATLAS_ASSERT(EXPR)                                                                  \
    do {                                                                                    \
        if (!(EXPR)) {                                                                      \
            ATLAS_LOG("ASSERT", "%s: %d: Assertion failed: %s", __FILE__, __LINE__, #EXPR); \
            vTaskDelay(100);                                                                \
            ATLAS_PANIC();                                                                  \
        }                                                                                   \
    } while (0)

#else

#define ATLAS_LOG(TAG, FMT, ...) \
    do {                         \
    } while (0)

#define ATLAS_LOG_ON_ERR(TAG, ERR) \
    do {                           \
    } while (0)

#define ATLAS_ASSERT(EXPR) \
    do {                   \
        (void)(EXPR);      \
    } while (0)

#endif // ATLAS_DEBUG

#endif // COMMON_ATLAS_UTILITY_H