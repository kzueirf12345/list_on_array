#ifndef LIST_ON_ARRAY_SRC_DUMB_DUMB_H
#define LIST_ON_ARRAY_SRC_DUMB_DUMB_H

#include <stdio.h>

#include "../fist/fist.h"
#include "logger/liblogger.h"

enum DumbError
{
    DUMB_ERROR_SUCCESS = 0,
    DUMB_ERROR_FAILURE = 1
};
static_assert(DUMB_ERROR_SUCCESS == 0);

const char* dumb_strerror(const enum DumbError error);

#define DUMB_ERROR_HANDLE(call_func, ...)                                                           \
    do {                                                                                            \
        enum DumbError error_handler = call_func;                                                   \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            dumb_strerror(error_handler));                                          \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)


enum DumbError dumb_ctor(void);
enum DumbError dumb_dtor(void);

enum DumbError dumb_set_out_html_file       (const char* const filename);
enum DumbError dumb_set_out_dot_file        (const char* const filename);
enum DumbError dumb_set_out_png_file        (const char* const filename);
enum DumbError dumb_set_out_graph_count_file(const char* const filename);

void fist_dumb_NOT_USE (const fist_t* const fist, const place_in_code_t call_place, 
                        int (*elem_to_str)
                            (const void* const elem, const size_t   elem_size,
                             char* const *     str,  const size_t mx_str_size));

#define FIST_DUMB(fist, elem_to_str)                                                                \
        fist_dumb_NOT_USE(fist,                                                                     \
                          (place_in_code_t)                                                         \
                          { .file = __FILE__, .func = __func__, .line = __LINE__ },                 \
                          elem_to_str)

#endif /*LIST_ON_ARRAY_SRC_DUMB_DUMB_H*/