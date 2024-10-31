#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "dumb.h"
#include "../fist/fist.h"
#include "logger/liblogger.h"
#include "utils.h"
#include "../verify_utils/verify_utils.h"

static const char* const HTML_INTRO_ =
    "\n<!DOCTYPE html>\n"
    "<html lang='en'>\n"
        "<head>\n"
            "<meta charset='UTF-8'>\n"
            "<meta http-equiv='X-UA-Compatible' content='IE=edge'>\n"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
            "<title>MEGA MEGA MEGA DUMB</title>\n"
        "</head>\n"
        "<body>\n"
            "<pre>\n";


#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* dumb_strerror(const enum DumbError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(DUMB_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(DUMB_ERROR_FAILURE);
    default:
        return "UNKNOWN_DUMB_ERROR";
    }
    return "UNKNOWN_DUMB_ERROR";
}
#undef CASE_ENUM_TO_STRING_


static struct 
{
    const char* name;
    FILE* file;
} DUMBER_ = {};

static void DUMBER_is_init_lasserts_(void)
{
    lassert(DUMBER_.name, "DUMBER_ is not init");
    lassert(DUMBER_.file, "DUMBER_ is not init");
}

enum DumbError dumb_ctor(void)
{
    lassert(!DUMBER_.name || !DUMBER_.file, "");

    DUMBER_.name = "./log/dumb.dot";
    if (!(DUMBER_.file = fopen(DUMBER_.name, "ab")))
    {
        perror("Can't open file");
        return DUMB_ERROR_FAILURE;
    }

    return DUMB_ERROR_SUCCESS;
}

enum DumbError dumb_dtor(void)
{
    DUMBER_is_init_lasserts_();

    if (fclose(DUMBER_.file))
    {
        perror("Can't close file");
        return DUMB_ERROR_FAILURE;
    }
    IF_DEBUG(DUMBER_.name = NULL;)

    return DUMB_ERROR_SUCCESS;
}

enum DumbError dumb_set_out_file(const char* const filename)
{
    DUMBER_is_init_lasserts_();
    lassert(filename, "");

    DUMBER_.name = filename;

    if (DUMBER_.file && fclose(DUMBER_.file))
    {  
        perror("Can't close file");
        return DUMB_ERROR_FAILURE;
    }
    
    if (!(DUMBER_.file = fopen(filename, "a+b"))){
        perror("Can't open file");
        return DUMB_ERROR_FAILURE;
    }
    
    return DUMB_ERROR_SUCCESS;
}

//==========================================================================================

static const char* 
    handle_invalid_ptr_ (const void* const check_ptr);
int is_empty_file_      (FILE* file);
int data_to_str_      (const void* const data, const size_t size, char* const * str,
                         const size_t str_size);
int dumb_arr_           (const fist_t* const fist, const void* const arr, const size_t elem_size, 
                         const size_t print_count, 
                         int (*elem_to_str)
                                (const void* const elem, const size_t   elem_size,
                                 char* const *     str,  const size_t mx_str_size));


#define LOGG_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
            fprintf(DUMBER_.file, format, ##__VA_ARGS__);                                           \
            fprintf(stderr,       format, ##__VA_ARGS__);                                           \
        } while(0)

void fist_dumb_NOT_USE (const fist_t* const fist, const place_in_code_t call_place, 
                        int (*elem_to_str)
                            (const void* const elem, const size_t   elem_size,
                             char* const *     str,  const size_t mx_str_size))
{
    if (!elem_to_str) elem_to_str = data_to_str_;

    if (is_empty_file_(DUMBER_.file) <= 0) fprintf(DUMBER_.file, HTML_INTRO_);

    LOGG_AND_FPRINTF_("\n==FIST DUMB==\nDate: %s\nTime: %s\n\n", __DATE__, __TIME__);

    const char* stack_buf = handle_invalid_ptr_(fist);
    const char* file_buf  = handle_invalid_ptr_(call_place.file);
    const char* func_buf  = handle_invalid_ptr_(call_place.func);
    file_buf =  file_buf  ? file_buf :          call_place.file;
    func_buf =  func_buf  ? func_buf :          call_place.func;
    const int   line_buf  =                     call_place.line <= 0
                                                ? CODE_LINE_POISON
                                                : call_place.line;

    if (stack_buf)
    {
        LOGG_AND_FPRINTF_("fist_t [%s] at %s:%d (%s())\n", stack_buf, file_buf, line_buf, func_buf);
        fprintf(stderr, "\n");
        return;
    }

    const char*           fist_name_buf      = handle_invalid_ptr_( fist->name           );
    const char*           fist_file_burn_buf = handle_invalid_ptr_( fist->burn_place.file);
    const char*           fist_func_burn_buf = handle_invalid_ptr_( fist->burn_place.func);
    fist_name_buf       = fist_name_buf      ? fist_name_buf      : fist->name;
    fist_file_burn_buf  = fist_file_burn_buf ? fist_file_burn_buf : fist->burn_place.file;
    fist_func_burn_buf  = fist_func_burn_buf ? fist_func_burn_buf : fist->burn_place.func;
    const int             fist_line_burn_buf = fist->burn_place.line <= 0
                                               ? CODE_LINE_POISON
                                               : fist->burn_place.line;


    LOGG_AND_FPRINTF_("fist_t %s[%p] at %s:%d (%s()) bUUUrn at %s:%d (%s())\n",
                     fist_name_buf, fist,
                     file_buf, line_buf, func_buf,
                     fist_file_burn_buf, fist_line_burn_buf, fist_func_burn_buf);

    LOGG_AND_FPRINTF_("{\n");
    LOGG_AND_FPRINTF_("\telem_size = %zu\n",   fist->elem_size);
    LOGG_AND_FPRINTF_("\tsize      = %zu\n",   fist->size);
    LOGG_AND_FPRINTF_("\tcapacity  = %zu\n\n", fist->capacity);

    LOGG_AND_FPRINTF_("\thead      = %zu\n",   fist->next[0]);
    LOGG_AND_FPRINTF_("\ttail      = %zu\n",   fist->prev[0]);
    LOGG_AND_FPRINTF_("\tfree      = %zu\n\n", fist->free);

    //------------------------------------------------------------------------------------------

                 size_t SKIP_SIZE       = 32;
    static const size_t MAX_PRINT_COUNT = 100;

    LOGG_AND_FPRINTF_("\t");
    for (size_t i = 0; i < SKIP_SIZE; ++i)
        LOGG_AND_FPRINTF_(" ");

    size_t print_count = MIN(fist->capacity, MAX_PRINT_COUNT);

    for (size_t ind = 0; ind < print_count; ++ind)
    {
        LOGG_AND_FPRINTF_("%-2zu ", ind);
    }
    LOGG_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    const char* fist_data_buf = handle_invalid_ptr_(fist->data);
    if (fist_data_buf)
    {
        LOGG_AND_FPRINTF_("\tdata[%s]\n", fist_data_buf);
        LOGG_AND_FPRINTF_("}\n");
        fprintf(stderr, "\n");
    }
    else
    {
        LOGG_AND_FPRINTF_("\tdata[%-10p] ORDER:\t", fist->data);

        if (dumb_arr_(fist, fist->data, fist->elem_size, print_count, elem_to_str))
        {
            LOGG_AND_FPRINTF_("\nCan't print fist->data\n");
        }
    }  

    const char* fist_next_buf = handle_invalid_ptr_(fist->next);
    if (fist_next_buf)
    {
        LOGG_AND_FPRINTF_("\tnext[%s]\n", fist_next_buf);
        LOGG_AND_FPRINTF_("}\n");
        fprintf(stderr, "\n");
    }
    else
    {
        LOGG_AND_FPRINTF_("\tnext[%-10p] ORDER:\t", fist->next); 

        //FIXME other print func
        if (dumb_arr_(fist, fist->next, sizeof(*fist->next), print_count, elem_to_str))
        {
            LOGG_AND_FPRINTF_("\nCan't print fist->next\n");
        }
    }

    const char* fist_prev_buf = handle_invalid_ptr_(fist->prev);
    if (fist_prev_buf)
    {
        LOGG_AND_FPRINTF_("\tprev[%s]\n", fist_prev_buf);
        LOGG_AND_FPRINTF_("}\n");
        fprintf(stderr, "\n");
    }
    else
    {
        LOGG_AND_FPRINTF_("\tprev[%-10p] ORDER:\t", fist->prev);
        
        //FIXME other print func
        if (dumb_arr_(fist, fist->prev, sizeof(*fist->prev), print_count, elem_to_str))
        {
            LOGG_AND_FPRINTF_("\nCan't print fist->prev\n");
        }
    }

    LOGG_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    SKIP_SIZE = 24;

    LOGG_AND_FPRINTF_("\t");
    for (size_t i = 0; i < SKIP_SIZE; ++i)
        LOGG_AND_FPRINTF_(" ");

           print_count      = MIN(print_count,     fist->size);
    size_t print_count_free = MIN(MAX_PRINT_COUNT, fist->capacity - print_count);

    for (size_t ind = 1; ind < MAX(print_count, print_count_free) + 1; ++ind)
    {
        LOGG_AND_FPRINTF_("%-2zu ", ind);
    }
    LOGG_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    if (fist_data_buf)
    {
        LOGG_AND_FPRINTF_("\tdata[%s]\n", fist_data_buf);
        LOGG_AND_FPRINTF_("}\n");
        fprintf(stderr, "\n");
    }
    else
    {
        const size_t elem_str_buf_size = 4 * MAX(fist->elem_size, sizeof(*fist->next));
        char*        elem_str_buf = calloc(1, elem_str_buf_size);

        if (!elem_str_buf)
        {
            LOGG_AND_FPRINTF_("\nError calloc elem_str_buf\n");
        }
        else
        {
            LOGG_AND_FPRINTF_("\tdata[%-10p]:\t", fist->data);

            for (size_t deep = 0, cur_ind = fist->next[0]; 
                        deep < print_count && cur_ind != 0; 
                    ++deep,                 cur_ind  = fist->next[cur_ind])
            {
                if (cur_ind > fist->capacity)
                {
                    LOGG_AND_FPRINTF_("\ncur_ind data (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
                    break;
                }
                if (elem_to_str((char*)fist->data + cur_ind * fist->elem_size, fist->elem_size,
                                &elem_str_buf, elem_str_buf_size))
                {
                    LOGG_AND_FPRINTF_("\nError elem_to_str\n");
                    break;
                }
                LOGG_AND_FPRINTF_("%-2s ", elem_str_buf); 

                if (!memset(elem_str_buf, 0, elem_str_buf_size))
                {
                    LOGG_AND_FPRINTF_("\nCan't memset elem_str_buf\n");
                    break;
                }
            }

            free(elem_str_buf); elem_str_buf = NULL;

            LOGG_AND_FPRINTF_("\n");
        }
    }


    if (fist_next_buf)
    {
        LOGG_AND_FPRINTF_("\tnext[%s]\n", fist_next_buf);
        LOGG_AND_FPRINTF_("}\n");
        fprintf(stderr, "\n");
    }
    else
    {
        LOGG_AND_FPRINTF_("\tnext[%-10p]:\t", fist->next);

        for (size_t deep = 0, cur_ind = fist->next[0]; 
                    deep < print_count && cur_ind != 0; 
                  ++deep,                 cur_ind  = fist->next[cur_ind])
        {
            if (cur_ind > fist->capacity)
            {
                LOGG_AND_FPRINTF_("\ncur_ind next (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
                break;
            }
            LOGG_AND_FPRINTF_("%-2zu ", cur_ind); 
        }
        LOGG_AND_FPRINTF_("\n");
    }

    if (fist_prev_buf)
    {
        LOGG_AND_FPRINTF_("\tprev[%s]\n", fist_prev_buf);
        LOGG_AND_FPRINTF_("}\n");
        fprintf(stderr, "\n");
    }
    else
    {
        LOGG_AND_FPRINTF_("\tprev[%-10p]:\t", fist->prev);

        for (size_t deep = 0, cur_ind = fist->prev[0]; 
                    deep < print_count && cur_ind != 0; 
                  ++deep,                 cur_ind  = fist->prev[cur_ind])
        {
            if (cur_ind > fist->capacity)
            {
                LOGG_AND_FPRINTF_("\ncur_ind prev (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
                break;
            }
            LOGG_AND_FPRINTF_("%-2zu ", cur_ind); 
        }
        LOGG_AND_FPRINTF_("\n");
    }

    LOGG_AND_FPRINTF_("\tfree[%-10p]:\t", &fist->free);

    print_count = MIN(MAX_PRINT_COUNT, fist->capacity - print_count);

    for (size_t deep = 0, cur_ind = fist->free; 
                deep < print_count && cur_ind != 0; 
                ++deep,               cur_ind  = fist->next[cur_ind])
    {
        if (cur_ind > fist->capacity)
        {
            LOGG_AND_FPRINTF_("\ncur_ind free (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
            break;
        }
        LOGG_AND_FPRINTF_("%-2zu ", cur_ind); 
    }
    LOGG_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    LOGG_AND_FPRINTF_("}");    
    fprintf(stderr, "\n");
}

//==========================================================================================

int is_empty_file_(FILE* file)
{
    if (!file)
    {
        fprintf(stderr, "Is empty file null\n");
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

int dumb_arr_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
              const size_t print_count, int (*elem_to_str)
                                            (const void* const elem, const size_t   elem_size,
                                            char* const *     str,  const size_t mx_str_size))
{
    const size_t elem_str_buf_size = 4 * MAX(elem_size, sizeof(*fist->next));
    char*        elem_str_buf = calloc(1, elem_str_buf_size);

    if (!elem_str_buf)
    {
        LOGG_AND_FPRINTF_("Error calloc elem_str_buf\n");
        return 1;
    }

    for (size_t ind = 0; ind < print_count; ++ind)
    {
        if (elem_to_str((const char*)arr + ind * elem_size, elem_size,
                        &elem_str_buf, elem_str_buf_size))
        {
            LOGG_AND_FPRINTF_("\nError elem_to_str\n");
            free(elem_str_buf); elem_str_buf = NULL;
            return 1;
        }
        LOGG_AND_FPRINTF_("%-2s ", elem_str_buf); 

        if (!memset(elem_str_buf, 0, elem_str_buf_size))
        {
            LOGG_AND_FPRINTF_("\nCan't memset elem_str_buf\n");
            break;
        }
    }
    LOGG_AND_FPRINTF_("\n");

    free(elem_str_buf); elem_str_buf = NULL;

    return 0;
}
#undef LOGG_AND_FPRINTF_

#ifndef FIST_INOUT_ELEM_CODE
#define FIST_INOUT_ELEM_CODE "%lX"
#endif /*FIST_INOUT_ELEM_CODE*/

int data_to_str_(const void* const data, const size_t size, char* const * str,
                   const size_t str_size)
{
    lassert(data, "");
    lassert(size, "");
    lassert(str, "");
    
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
                return 1;
            }
        }
        else
        {
            if (snprintf(temp_str, sizeof(uint8_t) * 4, FIST_INOUT_ELEM_CODE, 
                         *(const uint8_t*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return 1;
            }
        }

        if (!strncat(*str, temp_str, str_size))
        {
            perror("Can't stract str and temp_str");
            return 1;
        }
    }

    return 0;
}

static const char* handle_invalid_ptr_(const void* const check_ptr)
{
    switch (is_valid_ptr(check_ptr))
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