#include "fist.h"
#include "logger/liblogger.h"
#include "../verification/verification.h"


enum FistError fist_ctor_NOT_USE(fist_t* const fist, const size_t elem_size, const size_t capacity,
                                 const place_in_code_t burn_place)
{
    lassert(fist, "");
    lassert(elem_size, "");

    IF_DEBUG(fist->burn_place = burn_place;)

    fist->data = calloc(capacity, elem_size);
    if (!fist->data)
    {
        perror("Can't calloc fist->data");
        return FIST_ERROR_STANDARD_ERRNO;
    }

    fist->next = calloc(capacity, sizeof(*fist->next));
    if (!fist->next)
    {
        perror("Can't calloc fist->next");
        return FIST_ERROR_STANDARD_ERRNO;
    }

    fist->prev = calloc(capacity, sizeof(*fist->prev));
    if (!fist->prev)
    {
        perror("Can't calloc fist->prev");
        return FIST_ERROR_STANDARD_ERRNO;
    }

    fist->free          = fist->next;
    fist->capacity      = capacity;
    fist->elem_size     = elem_size;
    fist->size          = 0;

    return FIST_ERROR_SUCCESS;
}

void fist_dtor(fist_t* const fist)
{
    lassert(fist, "");
    lassert(fist->data, "");
    lassert(fist->next, "");
    lassert(fist->prev, "");

    free(fist->data); IF_DEBUG(fist->data = NULL;)
    free(fist->next); IF_DEBUG(fist->next = NULL;)
    free(fist->prev); IF_DEBUG(fist->prev = NULL;)

#ifndef NDEBUG
    fist->burn_place = (place_in_code_t){};
    fist->free       = NULL;
    fist->capacity   = 0;
    fist->elem_size  = 0;
    fist->size       = 0;
#endif /*NDEBUG*/
}