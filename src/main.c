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

    FIST_DUMB_ERROR_HANDLE(fist_dumb_ctor(),                                     logger_dtor(); fist_dumb_dtor(););
    FIST_DUMB_ERROR_HANDLE(fist_dumb_set_out_html_file("./log/dumb.html"),       logger_dtor(); fist_dumb_dtor(););
    FIST_DUMB_ERROR_HANDLE(fist_dumb_set_out_dot_file ("./log/dumb.dot"),        logger_dtor(); fist_dumb_dtor(););
    FIST_DUMB_ERROR_HANDLE(fist_dumb_set_out_png_file ("./log/dumb"),            logger_dtor(); fist_dumb_dtor(););

    //====================

    fist_t fist = {};
    FIST_ERROR_HANDLE(FIST_CTOR(&fist, sizeof(size_t), 2), 
                                                     logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););


    size_t add_elem_fix = 5;
    FIST_ERROR_HANDLE(fist_push(&fist, 0, &add_elem_fix), 
                                                     logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    for (size_t add_elem = 10; add_elem < 150; add_elem += 10)
    {
        FIST_ERROR_HANDLE(fist_push(&fist, add_elem/10, &add_elem), 
                                                     logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    } 
    FIST_ERROR_HANDLE(fist_pop  (&fist, 1),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 12),         logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 6),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 3),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 15),         logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 10),         logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 9),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 8),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);

    FIST_ERROR_HANDLE(fist_linearize(&fist, 0),      logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);

    for (size_t add_elem = 10; add_elem < 150; add_elem += 10)
    {
        FIST_ERROR_HANDLE(fist_push(&fist, add_elem/10, &add_elem), 
                                                     logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    } 
    FIST_ERROR_HANDLE(fist_pop  (&fist, 1),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 12),         logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 6),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_linearize(&fist, 0),      logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 3),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 15),         logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 10),         logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 9),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_pop  (&fist, 8),          logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_linearize(&fist, 0),      logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);
    FIST_ERROR_HANDLE(fist_linearize(&fist, 0),      logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    FIST_DUMB(&fist, NULL);

    // for (size_t i = 10; i > 0; --i)
    // {
    //     const size_t num = i;
    //     FIST_ERROR_HANDLE(fist_pop  (&fist, num),    logger_dtor(); fist_dumb_dtor(); fist_dtor(&fist););
    //     FIST_DUMB(&fist, NULL);
    // }


    fist_dtor(&fist);

    //====================
    FIST_DUMB_ERROR_HANDLE(                                                                 fist_dumb_dtor());
    LOGG_ERROR_HANDLE(                                                               logger_dtor());
    return 0;
}