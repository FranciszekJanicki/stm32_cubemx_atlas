#ifndef COMMON_ERROR_H
#define COMMON_ERROR_H

typedef enum {
    ERROR_OK,
    ERROR_FAIL,
    ERROR_NOT_RUNNING,
    ERROR_ALREADY_RUNNING,
    ERROR_UNKNOWN_EVENT,
    ERROR_UNKNOWN_NOTIFY,
} error_t;

char const* error_to_string(error_t err);

#endif // COMMON_ERROR_H