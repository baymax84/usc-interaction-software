#ifndef SPARKY_MACROS_H_
#define SPARKY_MACROS_H_

#include <sparky/global_flags.h>

#if sparky_flags_DEBUG_LEVEL >= sparky_flags_OUTPUT_LEVEL_INFO
#define PRINT_INFO( args... ) printf( "[INFO]  " args )
#else
#define PRINT_INFO( args... )
#endif

#if sparky_flags_DEBUG_LEVEL >= sparky_flags_OUTPUT_LEVEL_WARN
#define PRINT_WARN( args... ) printf( "[WARN]  " args )
#else
#define PRINT_WARN( args... )
#endif

#if sparky_flags_DEBUG_LEVEL >= sparky_flags_OUTPUT_LEVEL_DEBUG
#define PRINT_DEBUG( args... ) printf( "[DEBUG] " args )
#else
#define PRINT_DEBUG( args... )
#endif

#endif // SPARKY_MACROS_H_
