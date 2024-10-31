#ifndef LIST_ON_ARRAY_SRC_VERIFICATION_VERIFICATION_H
#define LIST_ON_ARRAY_SRC_VERIFICATION_VERIFICATION_H

#include "assert.h"

#include "../fist/fist_structs.h"
#include "logger/liblogger.h"

enum FistError
{
    FIST_ERROR_SUCCESS              = 0,
    FIST_ERROR_STANDARD_ERRNO       = 1,
    FIST_ERROR_FIST_IS_NULL         = 2,
    FIST_ERROR_FIST_IS_NVALID       = 3,
    FIST_ERROR_DATA_IS_NULL         = 4,
    FIST_ERROR_DATA_IS_NVALID       = 5,
    FIST_ERROR_NEXT_IS_NULL         = 6,
    FIST_ERROR_NEXT_IS_NVALID       = 7,
    FIST_ERROR_PREV_IS_NULL         = 8,
    FIST_ERROR_PREV_IS_NVALID       = 9,
    FIST_ERROR_ELEM_SIZE_IS_ZERO    = 10,
    FIST_ERROR_NEXT_NVALID          = 11,
    FIST_ERROR_PREV_NVALID          = 12,
    FIST_ERROR_NEXT_ELEM_OVERFLOW   = 13,
    FIST_ERROR_PREV_ELEM_OVERFLOW   = 14,
    FIST_ERROR_FREE_ELEM_OVERFLOW   = 15,
    FIST_ERROR_FREE_INTERSECT_NEXT  = 16,
    FIST_ERROR_FREE_NCOMPLETE       = 17,
    FIST_ERROR_SIZE_BIGGER_CAPACITY = 18,
    FIST_ERROR_CAPACITY_IS_ZERO     = 19,
    FIST_ERROR_POP_ARG_NVALID       = 20,
    FIST_ERROR_UNKNOWN              = 30
};
static_assert(FIST_ERROR_SUCCESS == 0);

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


#ifndef NDEBUG

enum FistError fist_verify_NOT_USE(const fist_t* const fist);

#define FIST_VERIFY(fist, elem_to_str)                                                              \
        do {                                                                                        \
            const enum FistError error = fist_verify_NOT_USE(fist);                                 \
            if (error)                                                                              \
            {                                                                                       \
                lassert(false, "Fist error: %s", fist_strerror(error));                             \
            }                                                                                       \
        } while(0)

#else /*NDEBUG*/

#define FIST_VERIFY(fist, elem_to_str) do {} while(0)

#endif /*NDEBUG*/

#endif /*LIST_ON_ARRAY_SRC_VERFICATION_VERIFICATION_H*/