#include <stdio.h>
#include <stdint.h>

#include "logger/liblogger.h"
#include "verification/verification.h"
#include "fist/fist.h"

int main()
{
    LOGG_ERROR_HANDLE(logger_ctor(), logger_dtor(););

    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL), logger_dtor(););
    LOGG_ERROR_HANDLE(logger_set_logout_file("./log/logout.log"), logger_dtor(););

    //====================

    fist_t fist = {};
    FIST_ERROR_HANDLE(FIST_CTOR(&fist, sizeof(int32_t), 1000), fist_dtor(&fist););

    fist_dtor(&fist);

    //====================
    LOGG_ERROR_HANDLE(logger_dtor());
    return 0;
}