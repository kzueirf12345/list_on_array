#ifndef LIST_ON_ARRAY_SRC_VERIFY_UTILS_VERIFY_UTILS_H
#define LIST_ON_ARRAY_SRC_VERIFY_UTILS_VERIFY_UTILS_H

#include <assert.h>

enum PtrState
{
    PTR_STATES_VALID   = 0,
    PTR_STATES_NULL    = 1,
    PTR_STATES_NVALID  = 2,
    PTR_STATES_ERROR   = 3
};
static_assert(PTR_STATES_VALID == 0);

enum PtrState fist_is_valid_ptr(const void* const ptr);

#endif /*LIST_ON_ARRAY_SRC_VERIFY_UTILS_VERIFY_UTILS_H*/