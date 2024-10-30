#include "verification.h"
#include "logger/liblogger.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* fist_strerror(const enum FistError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(FIST_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(FIST_ERROR_STANDARD_ERRNO);
    default:
        return "UNKNOWN_FIST_ERROR";
    }
    return "UNKNOWN_FIST_ERROR";
}
#undef CASE_ENUM_TO_STRING_

// enum FistError fist_verify_NOT_USE(const fist_t* const fist, const place_in_code_t call_place)
// {
//     lassert()
// }