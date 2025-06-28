#include "err.h"

char const* error_to_string(error_t err)
{
    switch (err) {
        case ERROR_OK:
            return "ERROR_OK";
        case ERROR_FAIL:
            return "ERROR_FAIL";
        case ERROR_NOT_RUNNING:
            return "ERROR_NOT_RUNNING";
        case ERROR_ALREADY_RUNNING:
            return "ERROR_ALREADY_RUNNING";
        case ERROR_UNKNOWN_EVENT:
            return "ERROR_UNKNOWN_EVENT";
        case ERROR_UNKNOWN_NOTIFY:
            return "ERROR_UNKNOWN_NOTIFY";
    }
}