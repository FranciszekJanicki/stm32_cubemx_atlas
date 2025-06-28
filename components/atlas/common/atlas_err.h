#ifndef ATLAS_COMMON_ATLAS_ERR_H
#define ATLAS_COMMON_ATLAS_ERR_H

typedef enum {
    ATLAS_ERR_OK,
    ATLAS_ERR_FAIL,
    ATLAS_ERR_NOT_RUNNING,
    ATLAS_ERR_ALREADY_RUNNING,
    ATLAS_ERR_UNKNOWN_EVENT,
    ATLAS_ERR_UNKNOWN_NOTIFY,
} atlas_err_t;

char const* atlas_err_to_string(atlas_err_t err);

#endif // ATLAS_COMMON_ATLAS_ERR_H