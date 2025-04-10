#ifndef LIST_ON_ARRAY_SRC_FIST_FIST_STRUCTS_H
#define LIST_ON_ARRAY_SRC_FIST_FIST_STRUCTS_H

#include <stdio.h>

#include "utils/utils.h"
#include "logger/liblogger.h"

typedef struct Fist
{
    const char* name;
    place_in_code_t burn_place;

    size_t elem_size;
    size_t capacity;
    size_t size;

    void* data;

    size_t* next;
    size_t* prev;

    size_t free;
} fist_t;

#endif /*LIST_ON_ARRAY_SRC_FIST_FIST_STRUCTS_H*/