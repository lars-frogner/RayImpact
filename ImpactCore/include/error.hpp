#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace Impact {

// Prints a message to stdout
inline void printInfoMessage(const char* format, ...)
{
    va_list arguments;

    va_start(arguments, format);

    std::string modified_format(format);
    modified_format = modified_format + "\n";

    #ifdef IMP_IS_WINDOWS
    vfprintf_s(stdout, modified_format.c_str(), arguments);
    #else // Linux
    vfprintf(stdout, modified_format.c_str(), arguments);
    #endif

    va_end(arguments);
}

// Prints a message to stderr
inline void printWarningMessage(const char* format, ...)
{
    va_list arguments;

    va_start(arguments, format);

    std::string modified_format(format);
    modified_format = "Warning: " + modified_format + "\n";

    #ifdef IMP_IS_WINDOWS
    vfprintf_s(stderr, modified_format.c_str(), arguments);
    #else // Linux
    vfprintf(stderr, modified_format.c_str(), arguments);
    #endif

    va_end(arguments);
}

// Prints a message to stderr
inline void printErrorMessage(const char* format, ...)
{
    va_list arguments;

    va_start(arguments, format);

    std::string modified_format(format);
    modified_format = "Error: " + modified_format + "\n";

    #ifdef IMP_IS_WINDOWS
    vfprintf_s(stderr, modified_format.c_str(), arguments);
    #else // Linux
    vfprintf(stderr, modified_format.c_str(), arguments);
    #endif

    va_end(arguments);
}

// Prints a message to stderr and aborts execution
inline void printSevereMessage(const char* format, ...)
{
    va_list arguments;

    va_start(arguments, format);

    std::string modified_format(format);
    modified_format = "Fatal error: " + modified_format + "\n";

    #ifdef IMP_IS_WINDOWS
    vfprintf_s(stderr, modified_format.c_str(), arguments);
    #else // Linux
    vfprintf(stderr, modified_format.c_str(), arguments);
    #endif

    va_end(arguments);

    exit(EXIT_FAILURE);
}

// Define check statement (always active)
#define imp_check(expression) \
    ((expression)? \
        (void)0 : \
        printSevereMessage("check \"%s\" failed in %s, line %d", \
                            #expression, __FILE__, __LINE__))

// Define assert statement (only active in debug mode)
#ifdef NDEBUG
    #define imp_assert(expression) ((void)0)
#else
    #define imp_assert(expression) \
        ((expression)? \
            (void)0 : \
            printSevereMessage("assertion \"%s\" failed in %s, line %d", \
                               #expression, __FILE__, __LINE__))
#endif

} // Impact
