#include <string.h>

#include "dumb_graphical.h"
#include "dumber.h"
#include "../verify_utils/verify_utils.h"
#include "utils.h"

//NOTE - non assertable
int dot_create_node_(const fist_t* const, const size_t ind, const elem_to_str_t elem_to_str);

int create_fist_dot(const fist_t* const fist, const elem_to_str_t elem_to_str)
{
    if (is_valid_ptr(fist))          return -1;
    if (is_valid_ptr(elem_to_str))   return -1;
    if (is_valid_ptr(fist->data))    return -1;
    if (is_valid_ptr(fist->next))    return -1;
    if (is_valid_ptr(fist->prev))    return -1;

    fprintf((*DUMBER_get_dot_file()), "digraph {\n"
                              "rankdir=LR;\n");

    fprintf((*DUMBER_get_dot_file()), "node_other_free [shape=Mrecord; label = \"...\" ];\n");
    fprintf((*DUMBER_get_dot_file()), "node_other_next_prev [shape=Mrecord; label = \"...\" ];\n");
    fprintf((*DUMBER_get_dot_file()), "node_free  [shape=Mrecord; label = \"FREE\"];\n");

    fprintf((*DUMBER_get_dot_file()), "node_free -> node%-4zu [weight=1; color=black;];\n", fist->free);

    size_t cur_ind = __LONG_LONG_MAX__;

//-------------------Create base nodes-------------------------------------------------------

    for (size_t ind = 0; ind <= fist->size; ++ind)
    {
        if (dot_create_node_(fist, ind, elem_to_str))
            break;
    }
    fprintf((*DUMBER_get_dot_file()), "\n");

//--------------------create fist edges---------------------------------------------


    const size_t print_count_free = fist->capacity - fist->size;
    size_t* free_arr = (size_t*)calloc(print_count_free, sizeof(size_t));

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

        fprintf((*DUMBER_get_dot_file()), "node%-4zu -> node%-4zu [weight=1; color = gray;];\n",
                cur_ind, fist->next[cur_ind]);

        free_arr[deep] = cur_ind;
    }
    if (fist->next[cur_ind] != 0)
        fprintf((*DUMBER_get_dot_file()), 
                "node%-4zu -> node_other_free [weight=1; color = gray;];\n",
                cur_ind);

// TODO is_free
//------------------------create base next and prev edges------------------------------------

    for (size_t ind = 0; ind <= fist->size; ++ind)
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
            fprintf((*DUMBER_get_dot_file()), "node%-4zu -> node%-4zu [weight=100; color = red;];\n",
                    ind, fist->next[ind]);

            fprintf((*DUMBER_get_dot_file()), "node%-4zu -> node%-4zu [weight=100; color = blue;];\n",
                    ind, fist->prev[ind]);
        }
    }

    free(free_arr); free_arr = NULL;

//--------------------------------create next edges-----------------------------------

    cur_ind = fist->next[0];
    for (size_t deep = 0; 
                deep < fist->size && cur_ind != 0; 
              ++deep,                 cur_ind  = fist->next[cur_ind])
    {
        if (cur_ind > fist->capacity)
        {
            fprintf(stderr, "\ncur_ind next (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
            break;
        }

        if (cur_ind <= fist->size) continue;

        dot_create_node_(fist, cur_ind, elem_to_str);

        fprintf((*DUMBER_get_dot_file()), "node%-4zu -> node%-4zu [weight=1; color = red;];\n",
                cur_ind, fist->next[cur_ind]);
    }
    if (cur_ind != 0 && cur_ind > fist->size)
        fprintf((*DUMBER_get_dot_file()), 
                "node%-4zu -> node_other_next_prev [weight=1; color = red;];\n", 
                cur_ind);


//---------------------------------create prev edges---------------------------------------

    cur_ind = fist->prev[0];
    for (size_t deep = 0; 
                deep < fist->size && cur_ind != 0; 
              ++deep,                 cur_ind  = fist->prev[cur_ind])
    {
        if (cur_ind > fist->capacity)
        {
            fprintf(stderr, "\ncur_ind prev (%zu) > capacity : (%zu)\n", cur_ind, fist->capacity);
            break;
        }

        if (cur_ind <= fist->size) continue;

        dot_create_node_(fist, cur_ind, elem_to_str);

        fprintf((*DUMBER_get_dot_file()), "node%-4zu -> node%-4zu [weight=1; color = blue;];\n",
                cur_ind, fist->prev[cur_ind]);
    }
    if (cur_ind != 0 && cur_ind > fist->size)
        fprintf((*DUMBER_get_dot_file()), 
                "node%-4zu -> node_other_next_prev [weight=1; color = blue;];\n",
                cur_ind);


    fprintf((*DUMBER_get_dot_file()), "}");

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
        fprintf((*DUMBER_get_dot_file()), 
                    "node%-4zu [shape=Mrecord; style=\"filled\"; fillcolor=\"lightgrey\"; "
                    "label = "
                    "\" %-4zu | data = %-7s | next = %-7zu | prev = %-7zu \"];\n",
                ind, ind, elem_str_buf, fist->next[ind], fist->prev[ind]);
    }
    else
    {
        fprintf((*DUMBER_get_dot_file()), 
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

int create_fist_png(void)
{
    if (fclose((*DUMBER_get_dot_file())))
    {
        perror("Can't fclose dot file");
        return -1;
    }

    static const size_t CREATE_PNG_CMD_SIZE = 256;
    char* create_png_cmd = calloc(CREATE_PNG_CMD_SIZE, sizeof(char));

    if (snprintf(create_png_cmd, CREATE_PNG_CMD_SIZE, 
                 "dot -Tpng %s -o %s%zu.png >/dev/null", 
                 (*DUMBER_get_dot_name()), (*DUMBER_get_png_name()), (*DUMBER_get_graph_count())) 
        <= 0)
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

    if (!((*DUMBER_get_dot_file()) = fopen((*DUMBER_get_dot_name()), "wb")))
    {
        perror("Can't fopen dot file");
        return -1;
    }

    return 0;
}

int insert_fist_png(void)
{
    const char* filename_without_path = (*DUMBER_get_png_name());
    while (strchr(filename_without_path, '/') != NULL)
    {
        filename_without_path = strchr(filename_without_path, '/') + 1;
    }

    fprintf((*DUMBER_get_html_file()), "<img src=%s%zu.png width=80%%>\n", 
            filename_without_path, (*DUMBER_get_graph_count()));

    return 0;
}