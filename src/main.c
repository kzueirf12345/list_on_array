#include <stdio.h>
#include "logger/liblogger.h"

int main()
{
    LOGG_ERROR_HANDLE(logger_ctor(), logger_dtor(););

    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL), logger_dtor(););
    LOGG_ERROR_HANDLE(logger_set_logout_file("./log/logout.log"), logger_dtor(););

    logg(LOG_LEVEL_DETAILS_INFO, "Hello, World!\n");

    LOGG_ERROR_HANDLE(logger_dtor());
    return 0;
}