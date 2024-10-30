#ifndef LIST_ON_ARRAY_UTILS_H
#define LIST_ON_ARRAY_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "concole.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifndef NDEBUG
#define IF_DEBUG(...) __VA_ARGS__
#define IF_ELSE_DEBUG(smth, other_smth) smth
#else /*NDEBUG*/
#define IF_DEBUG(...)
#define IF_ELSE_DEBUG(smth, other_smth) other_smth
#endif /*NDEBUG*/

#endif /*LIST_ON_ARRAY_UTILS_H*/