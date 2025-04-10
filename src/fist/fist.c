#include <string.h>

#include "fist.h"
#include "logger/liblogger.h"
#include "../verification/verification.h"


enum FistError fist_ctor_NOT_USE(fist_t* const fist, const size_t elem_size, const size_t capacity,
                                 const char* const name, const place_in_code_t burn_place)
{
    lassert(fist, "");
    lassert(elem_size, "");
    lassert(capacity, "");

    IF_DEBUG(fist->burn_place = burn_place;)
    IF_DEBUG(fist->name = name;)

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

    FIST_VERIFY_ASSERT(fist, NULL);

    return FIST_ERROR_SUCCESS;
}

void fist_dtor(fist_t* const fist)
{
    FIST_VERIFY_ASSERT(fist, NULL);

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

static enum FistError fist_resize_(fist_t* const fist);

enum FistError fist_push(fist_t* const fist, const size_t prev_ind, const void* const add_elem)
{
    FIST_VERIFY_ASSERT(fist, NULL);
    lassert(add_elem, "");
    lassert(prev_ind <= fist->capacity, "");

    FIST_ERROR_HANDLE(fist_resize_(fist));

    const size_t elem_ind = fist->free;
    const size_t next_ind = (prev_ind == fist->size ? 0 : fist->next[prev_ind]);

    fist->free = fist->next[fist->free];
    memcpy((char*)fist->data + elem_ind * fist->elem_size, add_elem, fist->elem_size);

    fist->next[prev_ind] = fist->prev[next_ind] = elem_ind;

    fist->next[elem_ind] = next_ind;
    fist->prev[elem_ind] = prev_ind;

    ++fist->size;

    FIST_VERIFY_ASSERT(fist, NULL);
    return FIST_ERROR_SUCCESS;
}

enum FistError fist_pop (fist_t* const fist, const size_t del_ind)
{
    FIST_VERIFY_ASSERT(fist, NULL);
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

    FIST_VERIFY_ASSERT(fist, NULL);
    return FIST_ERROR_SUCCESS;
}

//-------------------------------------

static enum FistError fist_resize_arr_(void** arr,                const size_t elem_size,
                                       const size_t old_capacity, const size_t new_capacity);

static enum FistError fist_resize_(fist_t* const fist)
{
    FIST_VERIFY_ASSERT(fist, NULL);

    size_t new_capacity = 0;

    if (fist->size == fist->capacity)
    {
        new_capacity = (fist->capacity + 1) * 2;
    }

    if (new_capacity)
    {
        FIST_ERROR_HANDLE(
            fist_resize_arr_(&fist->data, fist->elem_size,             fist->capacity + 1, 
                                                                         new_capacity)
        );
        FIST_ERROR_HANDLE(
            fist_resize_arr_((void**)&fist->next, sizeof(*fist->next), fist->capacity + 1, 
                                                                         new_capacity)
        );
        FIST_ERROR_HANDLE(
            fist_resize_arr_((void**)&fist->prev, sizeof(*fist->next), fist->capacity + 1, 
                                                                         new_capacity)
        );

        fist->free = fist->capacity + 1;
        for (size_t free_ind = fist->free; free_ind < new_capacity - 1; ++free_ind)
        {
            fist->next[free_ind] = free_ind + 1;
        }

        fist->capacity = new_capacity - 1;
    }

    FIST_VERIFY_ASSERT(fist, NULL);
    return FIST_ERROR_SUCCESS;
}


static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size);

static enum FistError fist_resize_arr_(void** arr,                const size_t elem_size,
                                       const size_t old_capacity, const size_t new_capacity)
{
    lassert(arr, "");
    lassert(*arr, "");
    lassert(elem_size, "");
    lassert(old_capacity, "");
    lassert(new_capacity, "");
    
    void* temp_arr = recalloc_(*arr, old_capacity, elem_size,
                                     new_capacity, elem_size);
    if (!temp_arr)
    {
        fprintf(stderr, "Can't recalloc_\n");
        return FIST_ERROR_STANDARD_ERRNO;
    }
    *arr = temp_arr; IF_DEBUG(temp_arr = NULL;)

    return FIST_ERROR_SUCCESS;
}


static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size)
{
    lassert(ptrmem, "");
    lassert(number, "");
    lassert(size  , "");

    if (number * size == old_number * old_size)
        return ptrmem;

    if (!(ptrmem = realloc(ptrmem, number * size)))
    {
        perror("Can't realloc in recalloc_");
        return NULL;
    }

    if (number * size > old_number * old_size
        && !memset((char*)ptrmem + old_number * old_size, 0, number * size - old_number * old_size))
    {
        perror("Can't memset in recalloc_");
        return NULL;
    }

    return ptrmem;
}

enum FistError fist_linearize(fist_t* const fist, size_t new_capacity)
{
    FIST_VERIFY_ASSERT(fist, NULL);
    
    if (new_capacity == 0) new_capacity = fist->capacity;

    //data

    void* temp_arr = calloc(new_capacity + 1, fist->elem_size);
    if ( !temp_arr)
    {
        perror("Can't calloc temp_arr");
        return FIST_ERROR_STANDARD_ERRNO;
    }

    for (size_t ind_new = 1, ind_old = fist->next[0]; 
         ind_new <= fist->size; 
         ++ind_new, ind_old = fist->next[ind_old])
    {
        memcpy((char*)temp_arr + ind_new * fist->elem_size, 
               (char*)fist->data + ind_old * fist->elem_size,
               fist->elem_size);
    }
    free(fist->data);
    fist->data = temp_arr;

    //next

    FIST_ERROR_HANDLE(fist_resize_arr_((void**)&fist->next, fist->elem_size,
                                       fist->capacity + 1, new_capacity + 1));

    for (size_t ind = 0; ind < new_capacity; ++ind)
    {
        fist->next[ind] = ind + 1;
    }
    fist->next[fist->size] = 0;

    //prev

    FIST_ERROR_HANDLE(fist_resize_arr_((void**)&fist->prev, fist->elem_size, 
                                       fist->capacity + 1, new_capacity + 1));

    for (size_t ind = 1; ind < fist->size; ++ind)
    {
        fist->prev[ind + 1] = ind;
    }
    memset((char*)fist->prev + (fist->size + 1) * sizeof(*fist->prev), 0, new_capacity - fist->size);
    fist->prev[0] = fist->size;

    //other

    fist->free     = fist->size + 1;
    fist->capacity = new_capacity;

    FIST_VERIFY_ASSERT(fist, NULL);
    return FIST_ERROR_SUCCESS;
}

//-------------------------------------

size_t fist_find(const fist_t* const fist, const void* const elem)
{
    FIST_VERIFY_ASSERT(fist, NULL);
    lassert(elem, "");

    for (size_t ind = fist->next[0]; ind != 0; ind = fist->next[ind])
    {
        if (memcmp(elem, (char*)fist->data + ind * fist->elem_size, fist->elem_size) == 0)
        {
            return ind;
        }
    }
    
    return 0;
}

enum FistError fist_print(FILE* out, const fist_t* const fist)
{
    FIST_VERIFY_ASSERT(fist, NULL);
    lassert(out, "");


    fprintf(out, "DATA: ");
    for (size_t ind = fist->next[0]; ind != 0; ind = fist->next[ind])
    {
        if (fprintf(out, "%-3zu ", *((size_t*)fist->data + ind)) <= 0)
        {
            perror("Can't fprintf data elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "NEXT: ");
    for (size_t ind = fist->next[0]; ind != 0; ind = fist->next[ind])
    {
        if (fprintf(out, "%-3zu ", fist->next[ind]) <= 0)
        {
            perror("Can't fprintf next elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "PREV: ");
    for (size_t ind = fist->next[0]; ind != 0; ind = fist->next[ind])
    {
        if (fprintf(out, "%-3zu ", fist->prev[ind]) <= 0)
        {
            perror("Can't fprintf prev elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    static const size_t LIMIT_SIZE = 20; 

    fprintf(out, "FREE: ");
    for (size_t ind = fist->free, cnt = 0; ind != 0 && cnt < LIMIT_SIZE; ind = fist->next[ind], ++cnt)
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
    for (size_t ind = 0; ind < MIN(LIMIT_SIZE, fist->capacity +1); ++ind)
    {
        if (fprintf(out, "%-3zu ", *((size_t*)fist->data + ind)) <= 0)
        {
            perror("Can't fprintf data elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "NEXT order: ");
    for (size_t ind = 0; ind < MIN(LIMIT_SIZE, fist->capacity +1); ++ind)
    {
        if (fprintf(out, "%-3zu ", fist->next[ind]) <= 0)
        {
            perror("Can't fprintf next elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "PREV order: ");
    for (size_t ind = 0; ind < MIN(LIMIT_SIZE, fist->capacity +1); ++ind)
    {
        if (fprintf(out, "%-3zu ", fist->prev[ind]) <= 0)
        {
            perror("Can't fprintf prev elem fist");
            return FIST_ERROR_STANDARD_ERRNO;
        }
    }
    fprintf(out, "\n");

    fprintf(out, "\n");

    FIST_VERIFY_ASSERT(fist, NULL);
    return FIST_ERROR_SUCCESS;
}

