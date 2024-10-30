#ifndef LIST_ON_ARRAY_SRC_VERIFICATION_VERIFICATION_H
#define LIST_ON_ARRAY_SRC_VERIFICATION_VERIFICATION_H

#include "../fist/fist.h"
#include "logger/liblogger.h"

enum FistError
{
    FIST_ERROR_SUCCESS          = 0,
    FIST_ERROR_STANDARD_ERRNO   = 1
};

const char* fist_strerror(const enum FistError error);

#define FIST_ERROR_HANDLE(call_func, ...)                                                           \
    do {                                                                                            \
        enum FistError error_handler = call_func;                                                   \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            fist_strerror(error_handler));                                          \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)


// enum FistError fist_verify_NOT_USE(const fist_t* const fist, const place_in_code_t call_place);


#endif /*LIST_ON_ARRAY_SRC_VERFICATION_VERIFICATION_H*/