#ifndef LIST_ON_ARRAY_SRC_FIST_H
#define LIST_ON_ARRAY_SRC_FIST_H

#include <stdio.h>

#include "fist_structs.h"
#include "utils.h"
#include "logger/liblogger.h"


enum FistError fist_ctor_NOT_USE(fist_t* const fist, const size_t elem_size, const size_t capacity,
                                 const char* const name, const place_in_code_t burn_place);

#ifndef NDEBUG

#define FIST_CTOR(fist, elem_size, capacity)                                                        \
        fist_ctor_NOT_USE(fist, elem_size, capacity, #fist,                                         \
                         (place_in_code_t)                                                          \
                         {.file = __FILE__, .func = __func__, .line = __LINE__})

#else /*NDEBUG*/

#define FIST_CTOR(fist, elem_size, capacity)                                                        \
        fist_ctor_NOT_USE(fist, elem_size, capacity, (place_in_code_t) {})

#endif /*NDEBUG*/

void fist_dtor(fist_t* const fist);

enum FistError fist_push(fist_t* const fist, const size_t prev_ind, const void* const add_elem);
enum FistError fist_pop (fist_t* const fist, const size_t del_ind);

enum FistError fist_print(FILE* out, const fist_t* const fist);



#endif /*LIST_ON_ARRAY_SRC_FIST_H*/