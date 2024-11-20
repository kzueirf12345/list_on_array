#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

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

//==========================================================================================

static struct 
{
    const char* html_name;
    const char* dot_name;
    const char* png_name;
    const char* graph_count_name;

    FILE* dot_file;
    FILE* html_file;

    size_t graph_count;
} DUMBER_ = {};

static void DUMBER_is_init_lasserts_(void)
{
    lassert(DUMBER_.html_name,          "DUMBER_ is not init");
    lassert(DUMBER_.html_file,          "DUMBER_ is not init");
    lassert(DUMBER_.dot_name,           "DUMBER_ is not init");
    lassert(DUMBER_.dot_file,           "DUMBER_ is not init");
    lassert(DUMBER_.png_name,           "DUMBER_ is not init");
    lassert(DUMBER_.graph_count_name,   "DUMBER_ is not init");
}

enum DumbError dumb_ctor(void)
{
    lassert(!DUMBER_.html_name         || !DUMBER_.html_file, "");
    lassert(!DUMBER_.dot_name          || !DUMBER_.dot_file,  "");
    lassert(!DUMBER_.png_name,                                "");
    lassert(!DUMBER_.graph_count_name,                        "");

    DUMBER_.html_name = "./log/dumb.html";
    if (!(DUMBER_.html_file = fopen(DUMBER_.html_name, "ab")))
    {
        perror("Can't open html_file");
        return DUMB_ERROR_FAILURE;
    }

    DUMBER_.dot_name = "./log/dumb.dot";
    if (!(DUMBER_.dot_file = fopen(DUMBER_.dot_name, "wb")))
    {
        perror("Can't open dot_file");
        return DUMB_ERROR_FAILURE;
    }

    DUMBER_.png_name = "./log/dumb.png"; // TODO remove .png, num after count

    DUMBER_.graph_count_name = "./log/graph_count.txt";

    return DUMB_ERROR_SUCCESS;
}

enum DumbError set_graph_count_          (void);
enum DumbError write_graph_count_in_file_(void);

enum DumbError dumb_dtor(void)
{
    DUMBER_is_init_lasserts_();

    DUMB_ERROR_HANDLE(write_graph_count_in_file_());

    if (fclose(DUMBER_.html_file))
    {
        perror("Can't close html_file");
        return DUMB_ERROR_FAILURE;
    }
    IF_DEBUG(DUMBER_.html_name = NULL;)

    if (fclose(DUMBER_.dot_file))
    {
        perror("Can't close dot_file");
        return DUMB_ERROR_FAILURE;
    }
    IF_DEBUG(DUMBER_.dot_name = NULL;)

    IF_DEBUG(DUMBER_.png_name = NULL;)

    IF_DEBUG(DUMBER_.graph_count_name = NULL;)

    return DUMB_ERROR_SUCCESS;
}

//==========================================================================================

static bool is_set_graph_count_ = false;
enum DumbError set_graph_count_(void) //NOTE - non assertable
{
    is_set_graph_count_ = true;

    if (access(DUMBER_.graph_count_name, F_OK)) //TODO switch to handle fopen errno
    {
        errno = 0;
        DUMBER_.graph_count = 0;
        return DUMB_ERROR_SUCCESS;
    }

    FILE* const graph_count_file = fopen(DUMBER_.graph_count_name, "rb");
    if (!graph_count_file)
    {
        perror("Can't open graph_count_file");
        return DUMB_ERROR_FAILURE;
    }

    if (fscanf(graph_count_file, "%zu", &DUMBER_.graph_count) <= 0)
    {
        perror("Can't fscanf graph_count");
        return DUMB_ERROR_FAILURE;
    }

    if (fclose(graph_count_file))
    {
        perror("Can't close graph_count_file");
        return DUMB_ERROR_FAILURE;
    }

    return DUMB_ERROR_SUCCESS;
}

enum DumbError write_graph_count_in_file_(void)
{
    DUMBER_is_init_lasserts_();

    FILE* const graph_count_file = fopen(DUMBER_.graph_count_name, "wb");
    if (!graph_count_file)
    {
        perror("Can't open graph_count_file");
        return DUMB_ERROR_FAILURE;
    }

    if (fprintf(graph_count_file, "%zu", DUMBER_.graph_count) <= 0)
    {
        perror("Can't fprintf graph_count");
        return DUMB_ERROR_FAILURE;
    }

    if (fclose(graph_count_file))
    {
        perror("Can't close graph_count_file");
        return DUMB_ERROR_FAILURE;
    }

    return DUMB_ERROR_SUCCESS;
}

//==========================================================================================

enum DumbError dumb_set_out_file_(const char*  const filename, FILE** const file, 
                                  const char** const old_filename, const char* const mode);

enum DumbError dumb_set_out_html_file(const char* const filename)
{
    DUMBER_is_init_lasserts_();
    lassert(filename, "");

    return dumb_set_out_file_(filename, &DUMBER_.html_file, &DUMBER_.html_name, "ab");
}

enum DumbError dumb_set_out_dot_file(const char* const filename)
{
    DUMBER_is_init_lasserts_();
    lassert(filename, "");

    return dumb_set_out_file_(filename, &DUMBER_.dot_file, &DUMBER_.dot_name, "wb");
}

enum DumbError dumb_set_out_file_(const char*  const filename, FILE** const file, 
                                  const char** const old_filename, const char* const mode)
{
    DUMBER_is_init_lasserts_();
    lassert(filename, "");
    lassert(file, "");
    lassert(old_filename, "");

    *old_filename = filename;

    if (*file && fclose(*file))
    {  
        perror("Can't close file");
        return DUMB_ERROR_FAILURE;
    }
    
    if (!(*file = fopen(*old_filename, mode))){
        perror("Can't open file");
        return DUMB_ERROR_FAILURE;
    }
    
    return DUMB_ERROR_SUCCESS;
}


enum DumbError dumb_set_out_png_file(const char* const filename)
{
    DUMBER_is_init_lasserts_();
    lassert(filename, "");

    DUMBER_.png_name = filename;
    return DUMB_ERROR_SUCCESS;
}

enum DumbError dumb_set_out_graph_count_file(const char* const filename)
{
    DUMBER_is_init_lasserts_();
    lassert(filename, "");

    DUMBER_.graph_count_name = filename;

    DUMB_ERROR_HANDLE(set_graph_count_());

    return DUMB_ERROR_SUCCESS;
}

//==========================================================================================

static const size_t MAX_PRINT_COUNT = 7;

//NOTE - non assertable
int data_to_str_        (const void* const data, const size_t size, char* const * str,
                         const size_t str_size); 
//NOTE - non assertable
int ind_to_str_         (const void* const data, const size_t size, char* const * str,
                         const size_t str_size);

//NOTE - non assertable
int is_empty_file_      (FILE* html_file);

//NOTE - non assertable
static const char* 
    handle_invalid_ptr_ (const void* const check_ptr);


//NOTE - non assertable
int dumb_order_arr_(const fist_t* const fist, 
                    const void* const arr, const char* const arr_name, const size_t elem_size,
                    const size_t print_count, const elem_to_str_t elem_to_str);

//NOTE - non assertable
int dumb_norder_arr_(const fist_t* const fist, 
                     const void* const arr, const char* const arr_name, const size_t elem_size,
                     const size_t print_count, const elem_to_str_t elem_to_str,
                     const size_t start_ind, const size_t* const ind_arr);


//NOTE - non assertable
int create_fist_dot_    (const fist_t* const fist, size_t print_count, 
                         const elem_to_str_t elem_to_str);

//NOTE - non assertable
int insert_fist_png_    (void);

//NOTE - non assertable
int create_fist_png_    (void);


#define DUMB_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
            fprintf(DUMBER_.html_file, format, ##__VA_ARGS__);                                      \
            fprintf(stderr,       format, ##__VA_ARGS__);                                           \
        } while(0)


//NOTE - non assertable
void fist_dumb_NOT_USE (const fist_t* const fist, const place_in_code_t call_place, 
                        elem_to_str_t elem_to_str)
{
    if (!elem_to_str) elem_to_str = data_to_str_;

    if (is_empty_file_(DUMBER_.html_file) <= 0) fprintf(DUMBER_.html_file, HTML_INTRO_);

    fprintf(DUMBER_.html_file, "</pre><hr /><pre>\n");   

    DUMB_AND_FPRINTF_("\n==FIST DUMB==\nDate: %s\nTime: %s\n\n", __DATE__, __TIME__);

    const char* fist_buf  = handle_invalid_ptr_(fist);
    const char* file_buf  = handle_invalid_ptr_(call_place.file);
    const char* func_buf  = handle_invalid_ptr_(call_place.func);
    file_buf =  file_buf  ? file_buf :          call_place.file;
    func_buf =  func_buf  ? func_buf :          call_place.func;
    const int   line_buf  =                     call_place.line <= 0
                                                ? CODE_LINE_POISON
                                                : call_place.line;

    if (fist_buf)
    {
        DUMB_AND_FPRINTF_("fist_t [%s] at %s:%d (%s())\n", fist_buf, file_buf, line_buf, func_buf);
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


    DUMB_AND_FPRINTF_("fist_t %s[%p] at %s:%d (%s()) bUUUrn at %s:%d (%s())\n",
                     fist_name_buf, fist,
                     file_buf, line_buf, func_buf,
                     fist_file_burn_buf, fist_line_burn_buf, fist_func_burn_buf);

    DUMB_AND_FPRINTF_("{\n");
    DUMB_AND_FPRINTF_("\telem_size = %zu\n",   fist->elem_size);
    DUMB_AND_FPRINTF_("\tsize      = %zu\n",   fist->size);
    DUMB_AND_FPRINTF_("\tcapacity  = %zu\n\n", fist->capacity);

    DUMB_AND_FPRINTF_("\thead      = %zu\n",   fist->next[0]);
    DUMB_AND_FPRINTF_("\ttail      = %zu\n",   fist->prev[0]);
    DUMB_AND_FPRINTF_("\tfree      = %zu\n\n", fist->free);

    //------------------------------------------------------------------------------------------

    size_t SKIP_SIZE = 32;

    DUMB_AND_FPRINTF_("\t");
    for (size_t i = 0; i < SKIP_SIZE; ++i)
        DUMB_AND_FPRINTF_(" ");

    size_t print_count = MIN(fist->capacity + 1, MAX_PRINT_COUNT);
    const size_t print_count_graph = print_count;

    for (size_t ind = 0; ind < print_count; ++ind)
    {
        DUMB_AND_FPRINTF_("%-7zu ", ind);
    }
    DUMB_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    if (dumb_order_arr_(fist, fist->data, "data", fist->elem_size, print_count, elem_to_str))
    {
        DUMB_AND_FPRINTF_("\nCan't dumb_order_arr_ data\n");
    }

    if (dumb_order_arr_(fist, fist->next, "next", sizeof(*fist->next), print_count, ind_to_str_))
    {
        DUMB_AND_FPRINTF_("\nCan't dumb_order_arr_ data\n");
    }

    if (dumb_order_arr_(fist, fist->prev, "prev", sizeof(*fist->prev), print_count, ind_to_str_))
    {
        DUMB_AND_FPRINTF_("\nCan't dumb_order_arr_ data\n");
    }

    DUMB_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    SKIP_SIZE = 24;

    DUMB_AND_FPRINTF_("\t");
    for (size_t i = 0; i < SKIP_SIZE; ++i)
        DUMB_AND_FPRINTF_(" ");

           print_count      = MIN(print_count,     fist->size);
    size_t print_count_free = MIN(MAX_PRINT_COUNT, fist->capacity - print_count);

    for (size_t ind = 1; ind < MAX(print_count, print_count_free) + 1; ++ind)
    {
        DUMB_AND_FPRINTF_("%-7zu ", ind);
    }
    DUMB_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    dumb_norder_arr_(fist, fist->data, "data", fist->elem_size,     print_count, elem_to_str, 
                     fist->next[0], fist->next);

    dumb_norder_arr_(fist, fist->next, "next", sizeof(*fist->next), print_count, ind_to_str_, 
                     fist->next[0], fist->next);
                     
    dumb_norder_arr_(fist, fist->prev, "prev", sizeof(*fist->prev), print_count, ind_to_str_, 
                     fist->next[0], fist->next);

    DUMB_AND_FPRINTF_("\n");   

    dumb_norder_arr_(fist, fist->next, "free", sizeof(*fist->next), print_count, ind_to_str_, 
                     fist->free,    fist->next);


    //------------------------------------------------------------------------------------------

    DUMB_AND_FPRINTF_("}\n");    

    if (!is_set_graph_count_ && set_graph_count_())
    {
        fprintf(stderr, "Can't set graph_count_\n");
        return;
    }


    if (create_fist_dot_(fist, print_count_graph, elem_to_str))
    {
        fprintf(stderr, "Can't create fist png\n");
        return;
    }

    if (create_fist_png_())
    {
        fprintf(stderr, "Can't create fist png\n");
        return;
    }

    if (insert_fist_png_())
    {
        fprintf(stderr, "Can't insert fist png\n");
        return;
    }
    ++DUMBER_.graph_count;

    fprintf(DUMBER_.html_file, "</pre><hr /><pre>\n");
}

//==========================================================================================

#ifndef FIST_INOUT_ELEM_CODE
#define FIST_INOUT_ELEM_CODE "%lX"
#endif /*FIST_INOUT_ELEM_CODE*/

int data_to_str_(const void* const data, const size_t size, char* const * str,
                   const size_t str_size)
{
    if (is_valid_ptr(data)) return -1;
    if (is_valid_ptr(str))  return -1;
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

int ind_to_str_(const void* const data, const size_t size, char* const * str,
                const size_t str_size)
{
    if (is_valid_ptr(data)) return -1;
    if (is_valid_ptr(str))  return -1;
    if (!size)              return -1;

    if (snprintf(*str, str_size, "%zu", *(const size_t*)data) <= 0)
    {
        perror("Can't snprintf ind_to_str");
        return -1;
    }

    return 0;
}

int is_empty_file_(FILE* file)
{
    if (is_valid_ptr(file))
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

//==========================================================================================
//TODO split file graph and normal
//TODO rename physical and logical order
//TODO struct for group func parametrs

//NOTE - non assertable
int dumb_order_arr_elems_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
                          const size_t print_count, const elem_to_str_t elem_to_str);

int dumb_order_arr_(const fist_t* const fist, 
                    const void* const arr, const char* const arr_name, const size_t elem_size,
                    const size_t print_count, const elem_to_str_t elem_to_str)
{
    if (is_valid_ptr(arr_name)) return -1;

    const char* fist_arr_buf = handle_invalid_ptr_(arr);
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

        if (dumb_order_arr_elems_(fist, arr, elem_size, print_count, elem_to_str))
        {
            DUMB_AND_FPRINTF_("\nCan't print %s elems\n", arr_name);
            return -1;
        }
    }  

    DUMB_AND_FPRINTF_("\n");

    return 0;
}

int dumb_order_arr_elems_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
                          const size_t print_count, const elem_to_str_t elem_to_str)
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

    for (size_t ind = 0; ind < print_count; ++ind)
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
int dumb_norder_arr_elems_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
                           const size_t print_count, const elem_to_str_t elem_to_str,
                           const size_t start_ind, const size_t* const ind_arr);

int dumb_norder_arr_(const fist_t* const fist, 
                     const void* const arr, const char* const arr_name, const size_t elem_size,
                     const size_t print_count, const elem_to_str_t elem_to_str,
                     const size_t start_ind, const size_t* const ind_arr)
{
    if (is_valid_ptr(arr_name)) return -1;

    const char* const fist_arr_buf = handle_invalid_ptr_(arr);
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

        if (dumb_norder_arr_elems_(fist, arr, elem_size, print_count, elem_to_str, start_ind, 
                                   ind_arr))
        {
            DUMB_AND_FPRINTF_("\nCan't print %s elems\n", arr_name);
            return -1;
        }
    }

    DUMB_AND_FPRINTF_("\n");

    return 0;
}

int dumb_norder_arr_elems_(const fist_t* const fist, const void* const arr, const size_t elem_size, 
                           const size_t print_count, const elem_to_str_t elem_to_str,
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
                deep < print_count && cur_ind != 0; 
              ++deep,                 cur_ind  = ind_arr[cur_ind])
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

#undef LOGG_AND_FPRINTF_

//==========================================================================================

//NOTE - non assertable
int dot_create_node_(const fist_t* const, const size_t ind, const elem_to_str_t elem_to_str);

int create_fist_dot_(const fist_t* const fist, size_t print_count, 
                     const elem_to_str_t elem_to_str)
{
    if (is_valid_ptr(fist))          return -1;
    if (is_valid_ptr(elem_to_str))   return -1;
    if (is_valid_ptr(fist->data))    return -1;
    if (is_valid_ptr(fist->next))    return -1;
    if (is_valid_ptr(fist->prev))    return -1;
    if (!print_count)                return -1;

    fprintf(DUMBER_.dot_file, "digraph {\n"
                              "rankdir=LR;\n");

    fprintf(DUMBER_.dot_file, "node_other_free [shape=Mrecord; label = \"...\" ];\n");
    fprintf(DUMBER_.dot_file, "node_other_next_prev [shape=Mrecord; label = \"...\" ];\n");
    fprintf(DUMBER_.dot_file, "node_free  [shape=Mrecord; label = \"FREE\"];\n");

    fprintf(DUMBER_.dot_file, "node_free -> node%-4zu [weight=1; color=black;];\n", fist->free);

    print_count = MIN(print_count, fist->size);
    size_t cur_ind = __LONG_LONG_MAX__;

//-------------------Create base nodes-------------------------------------------------------

    for (size_t ind = 0; ind <= print_count; ++ind)
    {
        if (dot_create_node_(fist, ind, elem_to_str))
            break;
    }
    fprintf(DUMBER_.dot_file, "\n");

//--------------------create fist edges---------------------------------------------


    const size_t print_count_free = MIN(MAX_PRINT_COUNT, fist->capacity - print_count);
    size_t*              free_arr = (size_t*)calloc(print_count_free, sizeof(size_t));

    if (!free_arr)
    {
        fprintf(stderr, "\nCan't calloc free_arr\n");
        return -1;
    }

    if (!memset(free_arr, 1, print_count_free * sizeof(*free_arr)))
    {
        fprintf(stderr, "\nCan't memset free arr\n");
    }

    cur_ind = fist->free;
    for (size_t deep = 0; 
                deep < print_count_free && fist->next[cur_ind] != 0; 
              ++deep,                      cur_ind  = fist->next[cur_ind])
    {
        if (cur_ind > fist->capacity)
        {
            fprintf(stderr, "\ncur_ind free (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
            break;
        }

        fprintf(DUMBER_.dot_file, "node%-4zu -> node%-4zu [weight=1; color = gray;];\n",
                cur_ind, fist->next[cur_ind]);

        free_arr[deep] = cur_ind;
    }
    if (fist->next[cur_ind] != 0)
        fprintf(DUMBER_.dot_file, 
                "node%-4zu -> node_other_free [weight=1; color = gray;];\n",
                cur_ind);

// TODO is_free
//------------------------create base next and prev edges------------------------------------

    for (size_t ind = 0; ind <= print_count; ++ind)
    {
        bool is_find_ind_on_free = false;
        for (size_t free_ind = 0; free_ind < print_count_free; ++free_ind)
        {
            if (ind == free_arr[free_ind])
            {
                is_find_ind_on_free = true;
                break;
            }
        }

        if (!is_find_ind_on_free)
        {
            fprintf(DUMBER_.dot_file, "node%-4zu -> node%-4zu [weight=100; color = red;];\n",
                    ind, fist->next[ind]);

            fprintf(DUMBER_.dot_file, "node%-4zu -> node%-4zu [weight=100; color = blue;];\n",
                    ind, fist->prev[ind]);
        }
    }

    free(free_arr); free_arr = NULL;

//--------------------------------create next edges-----------------------------------

    cur_ind = fist->next[0];
    for (size_t deep = 0; 
                deep < print_count && cur_ind != 0; 
              ++deep,                 cur_ind  = fist->next[cur_ind])
    {
        if (cur_ind > fist->capacity)
        {
            fprintf(stderr, "\ncur_ind next (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
            break;
        }

        if (cur_ind <= print_count) continue;

        dot_create_node_(fist, cur_ind, elem_to_str);

        fprintf(DUMBER_.dot_file, "node%-4zu -> node%-4zu [weight=1; color = red;];\n",
                cur_ind, fist->next[cur_ind]);
    }
    if (cur_ind != 0 && cur_ind > print_count)
        fprintf(DUMBER_.dot_file, 
                "node%-4zu -> node_other_next_prev [weight=1; color = red;];\n", 
                cur_ind);


//---------------------------------create prev edges---------------------------------------

    cur_ind = fist->prev[0];
    for (size_t deep = 0; 
                deep < print_count && cur_ind != 0; 
              ++deep,                 cur_ind  = fist->prev[cur_ind])
    {
        if (cur_ind > fist->capacity)
        {
            fprintf(stderr, "\ncur_ind prev (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
            break;
        }

        if (cur_ind <= print_count) continue;

        dot_create_node_(fist, cur_ind, elem_to_str);

        fprintf(DUMBER_.dot_file, "node%-4zu -> node%-4zu [weight=1; color = blue;];\n",
                cur_ind, fist->prev[cur_ind]);
    }
    if (cur_ind != 0 && cur_ind > print_count)
        fprintf(DUMBER_.dot_file, 
                "node%-4zu -> node_other_next_prev [weight=1; color = blue;];\n",
                cur_ind);


    fprintf(DUMBER_.dot_file, "}");

    return 0;
}

int dot_create_node_(const fist_t* const fist, const size_t ind, const elem_to_str_t elem_to_str)
{
    if (is_valid_ptr(fist))         return -1;
    if (is_valid_ptr(elem_to_str))  return -1;

    const size_t elem_str_buf_size = 4 * MAX(fist->elem_size, sizeof(*fist->next));
    char*        elem_str_buf = calloc(1, elem_str_buf_size);

    if (elem_to_str((char*)fist->data + ind * fist->elem_size, fist->elem_size,
                        &elem_str_buf, elem_str_buf_size))
        {
            fprintf(stderr, "\nError elem_to_str\n");
            free(elem_str_buf); elem_str_buf = NULL;
            return -1;
        }

    if (ind == 0)
    {
        fprintf(DUMBER_.dot_file, 
                    "node%-4zu [shape=Mrecord; style=\"filled\"; fillcolor=\"lightgrey\"; "
                    "label = "
                    "\" %-4zu | data = %-7s | next = %-7zu | prev = %-7zu \"];\n",
                ind, ind, elem_str_buf, fist->next[ind], fist->prev[ind]);
    }
    else
    {
        fprintf(DUMBER_.dot_file, 
                    "node%-4zu [shape=Mrecord; label = "
                    "\" %-4zu | data = %-7s | next = %-7zu | prev = %-7zu \"];\n",
                ind, ind, elem_str_buf, fist->next[ind], fist->prev[ind]);
    }

    if (!memset(elem_str_buf, 0, elem_str_buf_size))
    {
        fprintf(stderr, "\nCan't memset elem_str_buf\n");
        free(elem_str_buf); elem_str_buf = NULL;
        return -1;
    }

    free(elem_str_buf); elem_str_buf = NULL;
    return 0;
}

int create_fist_png_(void)
{
    if (fclose(DUMBER_.dot_file))
    {
        perror("Can't fclose dot file");
        return -1;
    }

    static const size_t CREATE_PNG_CMD_SIZE = 256;
    char* create_png_cmd = calloc(CREATE_PNG_CMD_SIZE, sizeof(char));

    if (snprintf(create_png_cmd, CREATE_PNG_CMD_SIZE, 
                 "dot -Tpng %s -o %s%zu >/dev/null", 
                 DUMBER_.dot_name, DUMBER_.png_name, DUMBER_.graph_count) <= 0)
    {
        fprintf(stderr, "Can't snprintf creare_png_cmd\n");
        free(create_png_cmd); create_png_cmd = NULL;
        return -1;
    }
    
    if (system(create_png_cmd))
    {
        fprintf(stderr, "Can't call system create png\n");
        free(create_png_cmd); create_png_cmd = NULL;
        return -1;
    }

    free(create_png_cmd); create_png_cmd = NULL;

    if (!(DUMBER_.dot_file = fopen(DUMBER_.dot_name, "wb")))
    {
        perror("Can't fopen dot file");
        return -1;
    }

    return 0;
}

int insert_fist_png_(void)
{
    const char* filename_without_path = DUMBER_.png_name;
    while (strchr(filename_without_path, '/') != NULL)
    {
        filename_without_path = strchr(filename_without_path, '/') + 1;
    }

    fprintf(DUMBER_.html_file, "<img src=%s%zu width=80%%>\n", 
            filename_without_path, DUMBER_.graph_count);

    return 0;
}
