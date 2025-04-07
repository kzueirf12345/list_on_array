#ifndef LIST_ON_ARRAY_SRC_DUMB_DUMBER_H
#define LIST_ON_ARRAY_SRC_DUMB_DUMBER_H

#include <stdio.h>
#include <stdbool.h>

const char**    DUMBER_get_html_name        (void);
const char**    DUMBER_get_dot_name         (void);
const char**    DUMBER_get_png_name         (void);
const char**    DUMBER_get_graph_count_name (void);
FILE**          DUMBER_get_dot_file         (void);
FILE**          DUMBER_get_html_file        (void);
size_t*         DUMBER_get_graph_count      (void);

void DUMBER_is_init_lasserts(void);
extern bool is_set_graph_count_;

enum FistDumbError set_graph_count          (void);
enum FistDumbError write_graph_count_in_file(void);


#endif /* LIST_ON_ARRAY_SRC_DUMB_DUMBER_H */