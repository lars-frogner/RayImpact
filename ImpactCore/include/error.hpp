#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace Impact {
	
// Prints a message to stdout
inline void printInfoMessage(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	#ifdef IMP_IS_WINDOWS
	vfprintf_s(stdout, format, arguments);
	#else // Linux
	vfprintf(stdout, format, arguments);
	#endif
	va_end(arguments);
}

// Prints a message to stderr
inline void printWarningMessage(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	#ifdef IMP_IS_WINDOWS
	vfprintf_s(stderr, format, arguments);
	#else // Linux
	vfprintf(stderr, format, arguments);
	#endif
	va_end(arguments);
}

// Prints a message to stderr
inline void printErrorMessage(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	#ifdef IMP_IS_WINDOWS
	vfprintf_s(stderr, format, arguments);
	#else // Linux
	vfprintf(stderr, format, arguments);
	#endif
	va_end(arguments);
}

// Prints a message to stderr and aborts execution
inline void printSevereMessage(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	#ifdef IMP_IS_WINDOWS
	vfprintf_s(stderr, format, arguments);
	#else // Linux
	vfprintf(stderr, format, arguments);
	#endif
	va_end(arguments);

	exit(EXIT_FAILURE);
}

// Define check statement (always active)
#define imp_check(expression) \
	((expression)? \
		(void)0 : \
		printSevereMessage("Check \"%s\" failed in %s, line %d", \
							#expression, __FILE__, __LINE__))

// Define assert statement (only active in debug mode)
#ifdef NDEBUG
	#define imp_assert(expression) ((void)0)
#else
	#define imp_assert(expression) \
		((expression)? \
			(void)0 : \
			printSevereMessage("Assertion \"%s\" failed in %s, line %d", \
							   #expression, __FILE__, __LINE__))
#endif

} // Impact
