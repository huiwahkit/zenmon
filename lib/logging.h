// Copyright (C) 2020 Denis Isai

#ifndef LOGGING_H
#define LOGGING_H

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// system includes
#include <stdio.h>

// dependency includes
#include "gen-types.h"

//======================================================================================================================
// DEFINES
//======================================================================================================================
#define PRINT_OKAY(...)     printf("[ %sOKAY%s ] ", F_GRN, F_RST); printf(__VA_ARGS__); printf("%s\n", F_RST)
#define PRINT_WARN(...)     printf("[ %sWARN%s ] ", F_YEL, F_RST); printf(__VA_ARGS__); printf("%s\n", F_RST)
#define PRINT_FAIL(...)     printf("[ %sFAIL%s ] ", F_RED, F_RST); printf(__VA_ARGS__); printf("%s\n", F_RST)
#define PRINT_INFO(...)     printf("[ %sINFO%s ] ", F_CYN, F_RST); printf(__VA_ARGS__); printf("%s\n", F_RST)
#define PRINT_TEXT(...)     printf("  %s    %s   ", F_RST, F_RST); printf(__VA_ARGS__); printf("%s\n", F_RST)
#if defined (PRINT_VERB)
    #define PRINT_VERB(...) printf("[ %sVERB%s ] ", F_MAG, F_RST); printf(__VA_ARGS__); printf("%s\n", F_RST)
#else
    #define PRINT_VERB(...)
#endif

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
