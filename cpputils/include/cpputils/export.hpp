#pragma once

#if defined _WIN32 || defined __CYGWIN__
    #ifdef __GNUC__
        #define CU_EXPORT __attribute__((dllexport))
    #else
        #define CU_EXPORT __declspec(dllexport)
    #endif
#else
    #if defined __GNUC__
        #define CU_EXPORT __attribute__((visibility("default")))
    #else
        #error "Unsupported compiler"
    #endif
#endif