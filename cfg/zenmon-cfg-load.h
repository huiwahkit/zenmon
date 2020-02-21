// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_CFG_LOAD_H
#define ZENMON_CFG_LOAD_H

// Block characters for CPU/RAM bars
//         0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
// U+258x  ▀  ▁  ▂  ▃  ▄  ▅  ▆  ▇  █  ▉  ▊  ▋  ▌  ▍  ▎  ▏
// U+259x  ▐  ░  ▒  ▓  ▔  ▕  ▖  ▗  ▘  ▙  ▚  ▛  ▜  ▝  ▞  ▟

//======================================================================================================================
// DEFINES
//======================================================================================================================
// file paths; these are checked at runtime and if any are missing the program will notify and exit
#define FILE_MHZ          "/proc/cpuinfo"     // cpu core   frequency
#define FILE_USG          "/proc/stat"        // cpu thread usage
#define FILE_RAM          "/proc/meminfo"     // RAM        usage

// load window size
#define LEN_LOAD_W_X      47u                 // window X size in characters
#define LEN_LOAD_W_Y      25u                 // window Y size in characters

// static configuration
#define CPU_CORES         16u
#define CPU_THREADS       32u
#define SEG_PER_BOX       8u                  // the full block has 8 segments
#define LEN_LINE          512u                // I don't expect any lines to be longet than this while reading

// RAM usage bar config
#define RAM_BAR_LEN       (LEN_LOAD_W_X - 4u) // RAM bar length in characters
#define RAM_BAR_LIM       (RAM_BAR_LEN  / 2u) // bar turns red at 50% RAM usage
#define RAM_AVAIL_OFFSET  13u                 // characters to skip when reading the available RAM line
#define RAM_TOTAL_OFFSET  9u                  // characters to skip when reading the total     RAM line

// core MHz bars config
#define MHZ_CPU_MAX       5000u               // max value of the MHz bar
#define MHZ_BAR_LEN       5u                  // bar height in lines
#define MHZ_BAR_LIM       4u                  // bar turns red at 80% MHz of MAX
#define MHZ_PER_BOX       1000u               // (MHZ_CPU_MAX / MHZ_BAR_LEN)
#define MHZ_PER_SEG       125u                // (MHZ_PER_BOX / SUBBARS_PER_BOX)
#define MHZ_OFFSET        10u                 // characters to skip when reading the MHz line

// thread usage bars config; permille instead of percent is used because of high bar granularity
#define USG_CPU_MAX       1000u               // used for the usg bar
#define USG_BAR_LEN       5u                  // bar height in lines
#define USG_BAR_LIM       4u                  // bar turns red at 80% usg of MAX
#define USG_PER_BOX       200u                // (USG_CPU_MAX / USG_BAR_LEN)
#define USG_PER_SEG       25u                 // (USG_PER_BOX / SUBBARS_PER_BOX)

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
