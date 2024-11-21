#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include "dumber.h"
#include "dumb.h"

static struct 
{
    const char* html_name;
    const char* dot_name;
    const char* png_name;
    const char* graph_count_name;

    FILE* dot_file;
    FILE* html_file;

    size_t graph_count;
} DUMBER_;

const char**    DUMBER_get_html_name        (void) {return &DUMBER_.html_name;}
const char**    DUMBER_get_dot_name         (void) {return &DUMBER_.dot_name;}
const char**    DUMBER_get_png_name         (void) {return &DUMBER_.png_name;}
const char**    DUMBER_get_graph_count_name (void) {return &DUMBER_.graph_count_name;}
FILE**          DUMBER_get_dot_file         (void) {return &DUMBER_.dot_file;}
FILE**          DUMBER_get_html_file        (void) {return &DUMBER_.html_file;}
size_t*         DUMBER_get_graph_count      (void) {return &DUMBER_.graph_count;}

void DUMBER_is_init_lasserts(void)
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

    DUMBER_.png_name = "./log/dumb";

    DUMBER_.graph_count_name = "./log/graph_count.txt";

    fprintf(stderr, "dumb_ctor()\n");

    return DUMB_ERROR_SUCCESS;
}

enum DumbError dumb_dtor(void)
{
    DUMBER_is_init_lasserts();

    DUMB_ERROR_HANDLE(write_graph_count_in_file());

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

bool is_set_graph_count_ = false;

enum DumbError set_graph_count(void)
{
    is_set_graph_count_ = true;

    if (access(DUMBER_.graph_count_name, F_OK))
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

enum DumbError write_graph_count_in_file(void)
{
    DUMBER_is_init_lasserts();

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
    DUMBER_is_init_lasserts();
    lassert(filename, "");

    return dumb_set_out_file_(filename, &DUMBER_.html_file, &DUMBER_.html_name, "ab");
}

enum DumbError dumb_set_out_dot_file(const char* const filename)
{
    DUMBER_is_init_lasserts();
    lassert(filename, "");

    return dumb_set_out_file_(filename, &DUMBER_.dot_file, &DUMBER_.dot_name, "wb");
}

enum DumbError dumb_set_out_file_(const char*  const filename, FILE** const file, 
                                  const char** const old_filename, const char* const mode)
{
    DUMBER_is_init_lasserts();
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
    DUMBER_is_init_lasserts();
    lassert(filename, "");

    DUMBER_.png_name = filename;
    return DUMB_ERROR_SUCCESS;
}

enum DumbError dumb_set_out_graph_count_file(const char* const filename)
{
    DUMBER_is_init_lasserts();
    lassert(filename, "");

    DUMBER_.graph_count_name = filename;

    DUMB_ERROR_HANDLE(set_graph_count());

    return DUMB_ERROR_SUCCESS;
}

//==========================================================================================
