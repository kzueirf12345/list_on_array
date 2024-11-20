#include "verification.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../verify_utils/verify_utils.h"
#include "logger/liblogger.h"

#define CASE_ENUM_TO_STRING_(error) \
    case error:                     \
        return #error
const char* fist_strerror(const enum FistError error) {
    switch (error) {
        CASE_ENUM_TO_STRING_(FIST_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(FIST_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(FIST_ERROR_FIST_IS_NULL);
        CASE_ENUM_TO_STRING_(FIST_ERROR_FIST_IS_NVALID);
        CASE_ENUM_TO_STRING_(FIST_ERROR_DATA_IS_NULL);
        CASE_ENUM_TO_STRING_(FIST_ERROR_DATA_IS_NVALID);
        CASE_ENUM_TO_STRING_(FIST_ERROR_NEXT_IS_NULL);
        CASE_ENUM_TO_STRING_(FIST_ERROR_NEXT_IS_NVALID);
        CASE_ENUM_TO_STRING_(FIST_ERROR_PREV_IS_NULL);
        CASE_ENUM_TO_STRING_(FIST_ERROR_PREV_IS_NVALID);
        CASE_ENUM_TO_STRING_(FIST_ERROR_ELEM_SIZE_IS_ZERO);
        CASE_ENUM_TO_STRING_(FIST_ERROR_NEXT_NVALID);
        CASE_ENUM_TO_STRING_(FIST_ERROR_NEXT_ELEM_OVERFLOW);
        CASE_ENUM_TO_STRING_(FIST_ERROR_PREV_NVALID);
        CASE_ENUM_TO_STRING_(FIST_ERROR_PREV_ELEM_OVERFLOW);
        CASE_ENUM_TO_STRING_(FIST_ERROR_FREE_ELEM_OVERFLOW);
        CASE_ENUM_TO_STRING_(FIST_ERROR_FREE_INTERSECT_NEXT);
        CASE_ENUM_TO_STRING_(FIST_ERROR_FREE_NCOMPLETE);
        CASE_ENUM_TO_STRING_(FIST_ERROR_SIZE_BIGGER_CAPACITY);
        CASE_ENUM_TO_STRING_(FIST_ERROR_CAPACITY_IS_ZERO);
        CASE_ENUM_TO_STRING_(FIST_ERROR_POP_ARG_NVALID);
        CASE_ENUM_TO_STRING_(FIST_ERROR_FREE_CIRCLE);
        CASE_ENUM_TO_STRING_(FIST_ERROR_LOGGER_ERROR);
        CASE_ENUM_TO_STRING_(FIST_ERROR_UNKNOWN);
        default:
            return "UNKNOWN_FIST_ERROR";
    }
    return "UNKNOWN_FIST_ERROR";
}
#undef CASE_ENUM_TO_STRING_

#ifndef NDEBUG

enum FistError fist_verify_NOT_USE(const fist_t* const fist) {
    switch (is_valid_ptr(fist)) {
        case PTR_STATES_VALID:
            break;
        case PTR_STATES_NULL:
            return FIST_ERROR_FIST_IS_NULL;
        case PTR_STATES_NVALID:
            return FIST_ERROR_FIST_IS_NVALID;
        case PTR_STATES_ERROR:
            return FIST_ERROR_STANDARD_ERRNO;

        default:
            fprintf(stderr, "Unknown PtrState enum fist, it's soooo bad\n");
            return FIST_ERROR_UNKNOWN;
    }

    switch (is_valid_ptr(fist->data)) {
        case PTR_STATES_VALID:
            break;
        case PTR_STATES_NULL:
            return FIST_ERROR_DATA_IS_NULL;
        case PTR_STATES_NVALID:
            return FIST_ERROR_DATA_IS_NVALID;
        case PTR_STATES_ERROR:
            return FIST_ERROR_STANDARD_ERRNO;

        default:
            fprintf(stderr, "Unknown PtrState enum fist->data, it's soooo bad\n");
            return FIST_ERROR_UNKNOWN;
    }

    switch (is_valid_ptr(fist->next)) {
        case PTR_STATES_VALID:
            break;
        case PTR_STATES_NULL:
            return FIST_ERROR_NEXT_IS_NULL;
        case PTR_STATES_NVALID:
            return FIST_ERROR_NEXT_IS_NVALID;
        case PTR_STATES_ERROR:
            return FIST_ERROR_STANDARD_ERRNO;

        default:
            fprintf(stderr, "Unknown PtrState enum fist->next, it's soooo bad\n");
            return FIST_ERROR_UNKNOWN;
    }

    switch (is_valid_ptr(fist->prev)) {
        case PTR_STATES_VALID:
            break;
        case PTR_STATES_NULL:
            return FIST_ERROR_PREV_IS_NULL;
        case PTR_STATES_NVALID:
            return FIST_ERROR_PREV_IS_NVALID;
        case PTR_STATES_ERROR:
            return FIST_ERROR_STANDARD_ERRNO;

        default:
            fprintf(stderr, "Unknown PtrState enum fist->prev, it's soooo bad\n");
            return FIST_ERROR_UNKNOWN;
    }

    if (!fist->capacity) return FIST_ERROR_CAPACITY_IS_ZERO;
    if (!fist->elem_size) return FIST_ERROR_ELEM_SIZE_IS_ZERO;
    if (fist->capacity < fist->size) return FIST_ERROR_SIZE_BIGGER_CAPACITY;

    for (size_t max_deep = 1; max_deep < fist->size; ++max_deep) {
        size_t cur_ind = 0;
        size_t deep = 0;

        for (; deep < max_deep && cur_ind != fist->prev[0]; ++deep, cur_ind = fist->next[cur_ind]) {
            if (cur_ind > fist->capacity) return FIST_ERROR_NEXT_ELEM_OVERFLOW;
        }
        if ((max_deep == fist->size) ^ (cur_ind == fist->prev[0]) ^ (fist->next[cur_ind] == 0))
            return FIST_ERROR_NEXT_NVALID;

        for (; deep > 0 && cur_ind != fist->next[0]; --deep, cur_ind = fist->prev[cur_ind]) {
            if (cur_ind > fist->capacity) return FIST_ERROR_PREV_ELEM_OVERFLOW;
        }
        if ((max_deep == fist->size) ^ (cur_ind == fist->next[0]) ^ (fist->prev[cur_ind] == 0))
            return FIST_ERROR_PREV_NVALID;
    }

    size_t free_size = 0;
    for (size_t free_ind = fist->free; free_ind != 0 && free_size < fist->capacity;
         free_ind = fist->next[free_ind]) {
        if (free_ind > fist->capacity) return FIST_ERROR_FREE_ELEM_OVERFLOW;

        for (size_t cur_ind = 0; cur_ind != 0; cur_ind = fist->next[cur_ind]) {
            if (free_ind == cur_ind) return FIST_ERROR_FREE_INTERSECT_NEXT;
        }
        ++free_size;
    }

    if (free_size > fist->capacity) return FIST_ERROR_FREE_CIRCLE;

    if (free_size + fist->size != fist->capacity) return FIST_ERROR_FREE_NCOMPLETE;

    return FIST_ERROR_SUCCESS;
}

#endif /*NDEBUG*/