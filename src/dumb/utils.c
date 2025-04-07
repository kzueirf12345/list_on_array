#include <stdint.h>
#include <string.h>

#include "utils.h"
#include "../verify_utils/verify_utils.h"
#include "utils/utils.h"

#ifndef FIST_INOUT_ELEM_CODE
#define FIST_INOUT_ELEM_CODE "%lX"
#endif /*FIST_INOUT_ELEM_CODE*/

int fist_data_to_str(const void* const data, const size_t size, char* const * str,
                   const size_t str_size)
{
    if (fist_is_valid_ptr(data)) return -1;
    if (fist_is_valid_ptr(str))  return -1;
    if (!size)              return -1;
    
    char temp_str[sizeof(uint64_t) * 4] = {};
    for (size_t offset = 0; offset < size; 
         offset += (size - offset >= sizeof(uint64_t) ? sizeof(uint64_t) : sizeof(uint8_t)))
    {
        if (size - offset >= sizeof(uint64_t))
        {
            if (snprintf(temp_str, sizeof(uint64_t) * 4, FIST_INOUT_ELEM_CODE,
                         *(const uint64_t*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return -1;
            }
        }
        else
        {
            if (snprintf(temp_str, sizeof(uint8_t) * 4, FIST_INOUT_ELEM_CODE, 
                         *(const uint8_t*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return -1;
            }
        }

        if (!strncat(*str, temp_str, str_size))
        {
            perror("Can't stract str and temp_str");
            return -1;
        }
    }

    return 0;
}

int fist_ind_to_str(const void* const data, const size_t size, char* const * str,
                const size_t str_size)
{
    if (fist_is_valid_ptr(data)) return -1;
    if (fist_is_valid_ptr(str))  return -1;
    if (!size)              return -1;

    if (snprintf(*str, str_size, "%zu", *(const size_t*)data) <= 0)
    {
        perror("Can't snprintf fist_ind_to_str");
        return -1;
    }

    return 0;
}

int fist_is_empty_file(FILE* file)
{
    if (fist_is_valid_ptr(file))
    {
        fprintf(stderr, "Is empty file nvalid\n");
        return -1;
    }

    int seek_temp = SEEK_CUR;

    if (fseek(file, 0, SEEK_END))
    {
        fprintf(stderr, "Can't fseek file\n");
        return -1;
    }

    const int res = ftell(file) > 2;

    if (fseek(file, 0, seek_temp))
    {
        fprintf(stderr, "Can't fseek file\n");
        return -1;
    }

    return res;
}

const char* fist_handle_invalid_ptr(const void* const check_ptr)
{
    switch (fist_is_valid_ptr(check_ptr))
    {
    case PTR_STATES_VALID:
        return NULL;
    case PTR_STATES_NULL:
        return "NULL";
    case PTR_STATES_NVALID:
        return "INVALID";
    case PTR_STATES_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }

    return "MIPT SHIT";
}
