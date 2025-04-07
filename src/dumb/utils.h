#ifndef LIST_ON_ARRAY_SRC_DUMB_UTILS_H
#define LIST_ON_ARRAY_SRC_DUMB_UTILS_H

#include <stdio.h>

//NOTE - non assertable
int fist_data_to_str(const void* const data, const size_t size, char* const * str, 
                const size_t str_size); 
//NOTE - non assertable
int fist_ind_to_str(const void* const data, const size_t size, char* const * str,
               const size_t str_size);
//NOTE - non assertable
int fist_is_empty_file(FILE* html_file);
//NOTE - non assertable
const char* fist_handle_invalid_ptr(const void* const check_ptr);

#endif /* LIST_ON_ARRAY_SRC_DUMB_UTILS_H */