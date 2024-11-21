#ifndef LIST_ON_ARRAY_SRC_DUMB_DUMB_GRAPHICAL_H
#define LIST_ON_ARRAY_SRC_DUMB_DUMB_GRAPHICAL_H

#include <stdio.h>

#include "../fist/fist.h"
#include "dumb.h"

//NOTE - non assertable
int create_fist_dot(const fist_t* const fist, const elem_to_str_t elem_to_str);

//NOTE - non assertable
int insert_fist_png(void);

//NOTE - non assertable
int create_fist_png(void);

#endif /* LIST_ON_ARRAY_SRC_DUMB_DUMB_GRAPHICAL_H */