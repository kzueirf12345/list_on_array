#ifndef LIST_ON_ARRAY_SRC_DUMB_DUMB_H
#define LIST_ON_ARRAY_SRC_DUMB_DUMB_H

#include <stdio.h>

#include "../fist/fist.h"
#include "logger/liblogger.h"

enum FistDumbError
{
    FIST_DUMB_ERROR_SUCCESS = 0,
    FIST_DUMB_ERROR_FAILURE = 1
};
static_assert(FIST_DUMB_ERROR_SUCCESS == 0, "");

const char* fist_dumb_strerror(const enum FistDumbError error);

#define FIST_DUMB_ERROR_HANDLE(call_func, ...)                                                      \
    do {                                                                                            \
        enum FistDumbError error_handler = call_func;                                               \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            fist_dumb_strerror(error_handler));                                     \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)


enum FistDumbError fist_dumb_ctor(void);
enum FistDumbError fist_dumb_dtor(void);

enum FistDumbError fist_dumb_set_out_html_file           (const char* const filename);
enum FistDumbError fist_dumb_set_out_dot_file            (const char* const filename);
enum FistDumbError fist_dumb_set_out_png_file            (const char* const filename);
enum FistDumbError fist_dumb_set_out_graph_count_file    (const char* const filename);

typedef int (*elem_to_str_t) (const void* const elem, const size_t   elem_size,
                              char* const *     str,  const size_t mx_str_size);

void fist_dumb_NOT_USE (const fist_t* const fist, const place_in_code_t call_place, 
                        elem_to_str_t elem_to_str);

#define FIST_DUMB(fist, elem_to_str)                                                                \
        fist_dumb_NOT_USE(fist,                                                                     \
                          (place_in_code_t)                                                         \
                          { .file = __FILE__, .func = __func__, .line = __LINE__ },                 \
                          elem_to_str)

#endif /*LIST_ON_ARRAY_SRC_DUMB_DUMB_H*/