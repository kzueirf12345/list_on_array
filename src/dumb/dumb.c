#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "dumb.h"
#include "../fist/fist.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "../verify_utils/verify_utils.h"
#include "dumber.h"
#include "utils.h"
#include "dumb_text.h"
#include "dumb_graphical.h"

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
const char* dumb_strerror(const enum FistDumbError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(FIST_DUMB_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(FIST_DUMB_ERROR_FAILURE);
    default:
        return "UNKNOWN_DUMB_ERROR";
    }
    return "UNKNOWN_DUMB_ERROR";
}
#undef CASE_ENUM_TO_STRING_


#define DUMB_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
            fprintf((*DUMBER_get_html_file()), format, ##__VA_ARGS__);                              \
            fprintf(stderr,       format, ##__VA_ARGS__);                                           \
        } while(0)


//NOTE - non assertable
void fist_dumb_NOT_USE (const fist_t* const fist, const place_in_code_t call_place, 
                        elem_to_str_t elem_to_str)
{
    if (!elem_to_str) elem_to_str = fist_data_to_str;

    if (fist_is_empty_file((*DUMBER_get_html_file())) <= 0) 
        fprintf((*DUMBER_get_html_file()), HTML_INTRO_);

    fprintf((*DUMBER_get_html_file()), "</pre><hr /><pre>\n");   

    DUMB_AND_FPRINTF_("\n==FIST DUMB==\nDate: %s\nTime: %s\n\n", __DATE__, __TIME__);

    const char* fist_buf  = fist_handle_invalid_ptr(fist);
    const char* file_buf  = fist_handle_invalid_ptr(call_place.file);
    const char* func_buf  = fist_handle_invalid_ptr(call_place.func);
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

    const char*           fist_name_buf      = fist_handle_invalid_ptr( fist->name           );
    const char*           fist_file_burn_buf = fist_handle_invalid_ptr( fist->burn_place.file);
    const char*           fist_func_burn_buf = fist_handle_invalid_ptr( fist->burn_place.func);
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

    for (size_t ind = 0; ind < fist->capacity + 1; ++ind)
    {
        DUMB_AND_FPRINTF_("%-7zu ", ind);
    }
    DUMB_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    if (dumb_physical_arr(fist, fist->data, "data", fist->elem_size, elem_to_str))
    {
        DUMB_AND_FPRINTF_("\nCan't dumb_physical_arr data\n");
    }

    if (dumb_physical_arr(fist, fist->next, "next", sizeof(*fist->next), fist_ind_to_str))
    {
        DUMB_AND_FPRINTF_("\nCan't dumb_physical_arr data\n");
    }

    if (dumb_physical_arr(fist, fist->prev, "prev", sizeof(*fist->prev), fist_ind_to_str))
    {
        DUMB_AND_FPRINTF_("\nCan't dumb_physical_arr data\n");
    }

    DUMB_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    SKIP_SIZE = 24;

    DUMB_AND_FPRINTF_("\t");
    for (size_t i = 0; i < SKIP_SIZE; ++i)
        DUMB_AND_FPRINTF_(" ");

    for (size_t ind = 1; ind < MAX(fist->size, fist->capacity - fist->size) + 1; ++ind)
    {
        DUMB_AND_FPRINTF_("%-7zu ", ind);
    }
    DUMB_AND_FPRINTF_("\n");

    //------------------------------------------------------------------------------------------

    dumb_logical_arr(fist, fist->data, "data", fist->elem_size, elem_to_str, 
                     fist->next[0], fist->next);

    dumb_logical_arr(fist, fist->next, "next", sizeof(*fist->next), fist_ind_to_str, 
                     fist->next[0], fist->next);
                     
    dumb_logical_arr(fist, fist->prev, "prev", sizeof(*fist->prev), fist_ind_to_str, 
                     fist->next[0], fist->next);

    DUMB_AND_FPRINTF_("\n");   

    dumb_logical_arr(fist, fist->next, "free", sizeof(*fist->next), fist_ind_to_str, 
                     fist->free,    fist->next);


    //------------------------------------------------------------------------------------------

    DUMB_AND_FPRINTF_("}\n");    

    if (!is_set_graph_count_ && set_graph_count())
    {
        fprintf(stderr, "Can't set graph_count_\n");
        return;
    }


    if (create_fist_dot(fist, elem_to_str))
    {
        fprintf(stderr, "Can't create fist png\n");
        return;
    }

    if (create_fist_png())
    {
        fprintf(stderr, "Can't create fist png\n");
        return;
    }

    if (insert_fist_png())
    {
        fprintf(stderr, "Can't insert fist png\n");
        return;
    }
    ++(*DUMBER_get_graph_count());

    fprintf(*DUMBER_get_html_file(), "</pre><hr /><pre>\n");
}
#undef LOGG_AND_FPRINTF_

//==========================================================================================

