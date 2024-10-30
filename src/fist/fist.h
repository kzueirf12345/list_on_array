#ifndef LIST_ON_ARRAY_SRC_FIST_H
#define LIST_ON_ARRAY_SRC_FIST_H

#include <stdio.h>

#include "utils.h"
#include "logger/liblogger.h"


typedef struct Fist
{
    IF_DEBUG(place_in_code_t burn_place;)

    size_t elem_size;
    size_t capacity;
    size_t size;

    void* data;

    size_t* next;
    size_t* prev;

    size_t* free;
} fist_t;

enum FistError fist_ctor_NOT_USE(fist_t* const fist, const size_t elem_size, const size_t capacity,
                                 const place_in_code_t burn_place);

#ifndef NDEBUG

#define FIST_CTOR(fist, elem_size, capacity)                                                        \
        fist_ctor_NOT_USE(fist, elem_size, capacity,                                                \
                         (place_in_code_t)                                                          \
                         {.file = __FILE__, .func = __func__, .line = __LINE__})

#else /*NDEBUG*/

#define FIST_CTOR(fist, elem_size, capacity)                                                        \
        fist_ctor_NOT_USE(fist, elem_size, capacity, (place_in_code_t) {})

#endif /*NDEBUG*/

void fist_dtor(fist_t* const fist);

#endif /*LIST_ON_ARRAY_SRC_FIST_H*/