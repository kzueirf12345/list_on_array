#include <stdio.h>
#include <stdint.h>

#include "logger/liblogger.h"
#include "fist/fist.h"
#include "verification/verification.h"
#include "dumb/dumb.h"

int main()
{
    LOGG_ERROR_HANDLE(logger_ctor(),                                                logger_dtor(););
    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL),              logger_dtor(););
    LOGG_ERROR_HANDLE(logger_set_logout_file("./log/logout.log"),                   logger_dtor(););

    DUMB_ERROR_HANDLE(dumb_ctor(),                                     logger_dtor(); dumb_dtor(););
    DUMB_ERROR_HANDLE(dumb_set_out_file("./log/dumb.html"),            logger_dtor(); dumb_dtor(););

    //====================

    fist_t fist = {};
    FIST_ERROR_HANDLE(FIST_CTOR(&fist, sizeof(size_t), 2), 
                                                     logger_dtor(); dumb_dtor(); fist_dtor(&fist););


    size_t add_elem_fix = 5;
    FIST_ERROR_HANDLE(fist_push(&fist, 0, &add_elem_fix), 
                                                     logger_dtor(); dumb_dtor(); fist_dtor(&fist););
    for (size_t add_elem = 10; add_elem < 100; add_elem += 10)
    {
        FIST_ERROR_HANDLE(fist_push(&fist, add_elem/10, &add_elem), 
                                                     logger_dtor(); dumb_dtor(); fist_dtor(&fist););
        FIST_DUMB(&fist, NULL);
    }

    for (size_t i = 10; i > 0; --i)
    {
        const size_t num = i;
        // fprintf(stderr, "\nNUM REMOVE: %zu\n", num);
        FIST_ERROR_HANDLE(fist_pop  (&fist, num),    logger_dtor(); dumb_dtor(); fist_dtor(&fist););
        // FIST_ERROR_HANDLE(fist_print(stderr, &fist), logger_dtor(); dumb_dtor(); fist_dtor(&fist););
    }


    fist_dtor(&fist);

    //====================
    DUMB_ERROR_HANDLE(                                                                 dumb_dtor());
    LOGG_ERROR_HANDLE(                                                               logger_dtor());
    return 0;
}