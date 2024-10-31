#include <string.h>

#include "fist.h"
#include "logger/liblogger.h"
#include "../verification/verification.h"


enum FistError fist_ctor_NOT_USE(fist_t* const fist, const size_t elem_size, const size_t capacity,
                                 const place_in_code_t burn_place)
{
    lassert(fist, "");
    lassert(elem_size, "");
    lassert(capacity, "");

    IF_DEBUG(fist->burn_place = burn_place;)

    fist->data = calloc(1 + capacity, elem_size);
    if (!fist->data)
    {
        perror("Can't calloc fist->data");
        return FIST_ERROR_STANDARD_ERRNO;
    }

    fist->next = calloc(1 + capacity, sizeof(*fist->next));
    if (!fist->next)
    {
        perror("Can't calloc fist->next");
        return FIST_ERROR_STANDARD_ERRNO;
    }

    fist->prev = calloc(1 + capacity, sizeof(*fist->prev));
    if (!fist->prev)
    {
        perror("Can't calloc fist->prev");
        return FIST_ERROR_STANDARD_ERRNO;
    }

    fist->capacity      = capacity;
    fist->elem_size     = elem_size;
    fist->size          = 0;

    fist->free = 1;
    for (size_t ind = 1; ind < fist->capacity; ++ind)
    {
        fist->next[ind] = ind + 1;
    }
    fist->next[fist->capacity] = 0;
    
    FIST_VERIFY(fist, NULL);

    return FIST_ERROR_SUCCESS;
}

void fist_dtor(fist_t* const fist)
{
    FIST_VERIFY(fist, NULL);

    free(fist->data); IF_DEBUG(fist->data = NULL;)
    free(fist->next); IF_DEBUG(fist->next = NULL;)
    free(fist->prev); IF_DEBUG(fist->prev = NULL;)

#ifndef NDEBUG
    fist->burn_place = (place_in_code_t){};
    fist->free       = 0;
    fist->capacity   = 0;
    fist->elem_size  = 0;
    fist->size       = 0;
#endif /*NDEBUG*/
}

enum FistError fist_push(fist_t* const fist, const size_t prev_ind, const void* const add_elem)
{
    FIST_VERIFY(fist, NULL);
    lassert(add_elem, "");
    lassert(prev_ind <= fist->capacity, "");

    lassert(fist->free, ""); // TODO to realloc

    const size_t elem_ind = fist->free;
    const size_t next_ind = (prev_ind == fist->size ? 0 : fist->next[prev_ind]);

    fist->free = fist->next[fist->free];
    memcpy((char*)fist->data + elem_ind * fist->elem_size, add_elem, fist->elem_size);

    fist->next[prev_ind] = fist->prev[next_ind] = elem_ind;

    fist->next[elem_ind] = next_ind;
    fist->prev[elem_ind] = prev_ind;

    ++fist->size;

    FIST_VERIFY(fist, NULL);
    return FIST_ERROR_SUCCESS;
}

enum FistError fist_pop (fist_t* const fist, const size_t del_ind)
{
    FIST_VERIFY(fist, NULL);
    lassert(0 < del_ind && del_ind <= fist->capacity, "");
    lassert(fist->size, "");
    lassert(del_ind == fist->next[0] || fist->prev[del_ind] != 0, "del_ind is invalid");

    const size_t prev_ind = fist->prev[del_ind];
    const size_t next_ind = fist->next[del_ind];
    
    fist->next[del_ind] = fist->free;
    fist->prev[del_ind] = 0;
    fist->free = del_ind;

    fist->next[prev_ind] = next_ind;
    fist->prev[next_ind] = prev_ind;

    --fist->size;

    FIST_VERIFY(fist, NULL);
    return FIST_ERROR_SUCCESS;
}

enum FistError fist_print(FILE* out, const fist_t fist)
{
    FIST_VERIFY(&fist, NULL);
    lassert(out, "");

    fprintf(out, "DATA: ");
    for (size_t ind = fist.next[0]; ind != 0; ind = fist.next[ind])
    {
        if (fprintf(out, "%-3zu ", *((size_t*)fist.data + ind)) <= 0)
        {
            perror("Can't fprintf data elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "NEXT: ");
    for (size_t ind = fist.next[0]; ind != 0; ind = fist.next[ind])
    {
        if (fprintf(out, "%-3zu ", fist.next[ind]) <= 0)
        {
            perror("Can't fprintf next elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "PREV: ");
    for (size_t ind = fist.next[0]; ind != 0; ind = fist.next[ind])
    {
        if (fprintf(out, "%-3zu ", fist.prev[ind]) <= 0)
        {
            perror("Can't fprintf prev elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "FREE: ");
    for (size_t ind = fist.free, cnt = 0; ind != 0 && cnt < 20; ind = fist.next[ind], ++cnt)
    {
        if (fprintf(out, "%-3zu ", ind) <= 0)
        {
            perror("Can't fprintf free elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");
    fprintf(out, "\n");

    fprintf(out, "DATA order: ");
    for (size_t ind = 0; ind < 20; ++ind)
    {
        if (fprintf(out, "%-3zu ", *((size_t*)fist.data + ind)) <= 0)
        {
            perror("Can't fprintf data elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "NEXT order: ");
    for (size_t ind = 0; ind < 20; ++ind)
    {
        if (fprintf(out, "%-3zu ", fist.next[ind]) <= 0)
        {
            perror("Can't fprintf next elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "PREV order: ");
    for (size_t ind = 0; ind < 20; ++ind)
    {
        if (fprintf(out, "%-3zu ", fist.prev[ind]) <= 0)
        {
            perror("Can't fprintf prev elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    FIST_VERIFY(&fist, NULL);
    return FIST_ERROR_SUCCESS;
}