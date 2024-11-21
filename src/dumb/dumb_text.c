#include <string.h>

#include "dumb_text.h"
#include "dumber.h"
#include "../verify_utils/verify_utils.h"
#include "utils.h"

//TODO struct for group func parametrs

#define DUMB_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
            fprintf((*DUMBER_get_html_file()), format, ##__VA_ARGS__);                              \
            fprintf(stderr,       format, ##__VA_ARGS__);                                           \
        } while(0)

//NOTE - non assertable
int dumb_physical_arr_elems_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
                             const elem_to_str_t elem_to_str);

int dumb_physical_arr(const fist_t* const fist, 
                      const void* const arr, const char* const arr_name, const size_t elem_size,
                      const elem_to_str_t elem_to_str)
{
    if (is_valid_ptr(arr_name)) return -1;

    const char* fist_arr_buf = handle_invalid_ptr(arr);
    if (fist_arr_buf)
    {
        DUMB_AND_FPRINTF_("\t%s[%s]\n", arr_name, fist_arr_buf);
        DUMB_AND_FPRINTF_("}");
    }
    else
    {
        if (is_valid_ptr(fist))          return -1;
        if (is_valid_ptr(elem_to_str))   return -1;
        if (!elem_size)                  return -1;

        DUMB_AND_FPRINTF_("\t%s[%-10p] ORDER:\t", arr_name, arr);

        if (dumb_physical_arr_elems_(fist, arr, elem_size, elem_to_str))
        {
            DUMB_AND_FPRINTF_("\nCan't print %s elems\n", arr_name);
            return -1;
        }
    }  

    DUMB_AND_FPRINTF_("\n");

    return 0;
}

int dumb_physical_arr_elems_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
                             const elem_to_str_t elem_to_str)
{
    if (is_valid_ptr(fist))         return -1;
    if (is_valid_ptr(arr))          return -1;
    if (is_valid_ptr(elem_to_str))  return -1;
    if (!elem_size)                 return -1;

    const size_t elem_str_buf_size = 4 * elem_size;
    char*        elem_str_buf      = calloc(1, elem_str_buf_size);

    if (!elem_str_buf)
    {
        DUMB_AND_FPRINTF_("Error calloc elem_str_buf\n");
        return -1;
    }

    for (size_t ind = 0; ind < fist->capacity + 1; ++ind)
    {
        if (elem_to_str((const char*)arr + ind * elem_size, elem_size,
                        &elem_str_buf, elem_str_buf_size))
        {
            DUMB_AND_FPRINTF_("\nError elem_to_str\n");
            free(elem_str_buf); elem_str_buf = NULL;
            return -1;
        }
        DUMB_AND_FPRINTF_("%-7s ", elem_str_buf); 

        if (!memset(elem_str_buf, 0, elem_str_buf_size))
        {
            DUMB_AND_FPRINTF_("\nCan't memset elem_str_buf\n");
            break;
        }
    }

    free(elem_str_buf); elem_str_buf = NULL;

    return 0;
}

//NOTE - non assertable
int dumb_logical_arr_elems_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
                            const elem_to_str_t elem_to_str,
                            const size_t start_ind, const size_t* const ind_arr);

int dumb_logical_arr(const fist_t* const fist, 
                     const void* const arr, const char* const arr_name, const size_t elem_size,
                     const elem_to_str_t elem_to_str,
                     const size_t start_ind, const size_t* const ind_arr)
{
    if (is_valid_ptr(arr_name)) return -1;

    const char* const fist_arr_buf = handle_invalid_ptr(arr);
    if (fist_arr_buf)
    {
        DUMB_AND_FPRINTF_("\t%s[%s]\n", arr_name, fist_arr_buf);
        DUMB_AND_FPRINTF_("}");
    }
    else
    {
        if (is_valid_ptr(fist))         return -1;
        if (is_valid_ptr(elem_to_str))  return -1;
        if (is_valid_ptr(ind_arr))      return -1;
        if (!elem_size)                 return -1;

        DUMB_AND_FPRINTF_("\t%s[%p]:\t", arr_name, arr);

        if (dumb_logical_arr_elems_(fist, arr, elem_size, elem_to_str, start_ind, ind_arr))
        {
            DUMB_AND_FPRINTF_("\nCan't print %s elems\n", arr_name);
            return -1;
        }
    }

    DUMB_AND_FPRINTF_("\n");

    return 0;
}

int dumb_logical_arr_elems_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
                            const elem_to_str_t elem_to_str,
                            const size_t start_ind, const size_t* const ind_arr)
{
    if (is_valid_ptr(arr))          return -1;
    if (is_valid_ptr(fist))         return -1;
    if (is_valid_ptr(elem_to_str))  return -1;
    if (is_valid_ptr(ind_arr))      return -1;
    if (!elem_size)                 return -1;


    const size_t elem_str_buf_size = 4 * elem_size;
    char*        elem_str_buf      = calloc(1, elem_str_buf_size);

    if (!elem_str_buf)
    {
        DUMB_AND_FPRINTF_("\nError calloc elem_str_buf\n");
        return -1;
    }
    
    for (size_t deep = 0, cur_ind = start_ind;
                deep < fist->size && cur_ind != 0; 
              ++deep,                cur_ind  = ind_arr[cur_ind])
    {
        if (cur_ind > fist->capacity)
        {
            DUMB_AND_FPRINTF_("\ncur_ind (%zu) > capacity : (%zu)\n", 
                                    cur_ind,  fist->capacity);
            break;
        }

        if (elem_to_str((const char*)arr + cur_ind * elem_size, elem_size,
            &elem_str_buf, elem_str_buf_size))
        {
            DUMB_AND_FPRINTF_("\nError elem_to_str\n");
            break;
        }
        
        DUMB_AND_FPRINTF_("%-7s ", elem_str_buf); 

        if (!memset(elem_str_buf, 0, elem_str_buf_size))
        {
            DUMB_AND_FPRINTF_("\nCan't memset elem_str_buf\n");
            break;
        }
        
    }

    free(elem_str_buf); elem_str_buf = NULL;
    
    return 0;
}
