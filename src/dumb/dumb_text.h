#ifndef LIST_ON_ARRAY_SRC_DUMB_DUMB_TEXT_H
#define LIST_ON_ARRAY_SRC_DUMB_DUMB_TEXT_H

#include <stdio.h>

#include "../fist/fist_structs.h"
#include "dumb.h"

//NOTE - non assertable
int dumb_physical_arr(const fist_t* const fist, 
                      const void* const arr, const char* const arr_name, const size_t elem_size,
                      const elem_to_str_t elem_to_str);

//NOTE - non assertable
int dumb_logical_arr(const fist_t* const fist, 
                     const void* const arr, const char* const arr_name, const size_t elem_size,
                     const elem_to_str_t elem_to_str,
                     const size_t start_ind, const size_t* const ind_arr);

#endif /* LIST_ON_ARRAY_SRC_DUMB_DUMB_TEXT_H */