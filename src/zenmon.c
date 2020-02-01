// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUSIONS
//======================================================================================================================
// system includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>

// dependency includes
#include "gen-types.h"
#include "braille-lib.h"

// local includes
#include "zenmon-cfg.h"
#include "zenmon-types.h"
#include "zenmon-box.h"
#include "zenmon-num.h"
#include "zenmon-dot.h"

//======================================================================================================================
// DEFINES
//======================================================================================================================
#define UNTIL_SIGNAL 1

//======================================================================================================================
// TYPES
//======================================================================================================================
typedef struct coordTypeTag
{
    uint16 xPos;
    uint16 yPos;
} coordType;

//======================================================================================================================
// VARIABLES
//======================================================================================================================
static uint32 sampleRate = 1000000; // default delay of 1s

//======================================================================================================================
// FUNCTIONS
//======================================================================================================================
static void zenmon_signalHandler() //------------------------------------------------------------------ zenmon_signalHandler
{
    num_deinit();
    SHOW_CURSOR;
    CLEAR_SCREEN;
    exit(EXIT_SUCCESS);
}

void zenmon_getopt(int argc, char* const argv[]) //----------------------------------------------------------- zenmon_getopt
{
    sint8 option;
    uint8 exitFlag = FALSE;

    while(-1 != (option = getopt(argc, argv, ":r:h")))
    {
        switch(option)
        {
            case 'r':
                sampleRate = (atoi(optarg) * 1000);
                break;
            case 'h':
                exitFlag = TRUE;
                break;
            case ':': // used for options that require arguments, but don't have them
                printf("%sOption needs a value\n", F_RED);
                exitFlag = TRUE;
                break;
            case '?': // used for unknown options
                printf("%sUnknown option: %c\n", F_RED, optopt);
                exitFlag = TRUE;
                break;
            default:
                break;
        }
    }

    for(; optind < argc; optind++)
    {
        printf("%sOptions received extra arguments: %s\n", F_RED, argv[optind]);
        exitFlag = TRUE;
    }

    if(TRUE == exitFlag)
    {
        printf("%sUsage: ./zenmon -r <sample rate in ms>; default is 1s\n", F_YEL);
        exit(EXIT_SUCCESS);
    }
}

//======================================================================================================================
// MAIN
//======================================================================================================================
int main(int argc, char* const argv[]) //-------------------------------------------------------------------------- main
{
    // set coordinates
    const coordType locStatusWin = {4u, 2u};
    const coordType locEGraphWin = {51u, 2u};
    const coordType locTGraphWin = {51u, 19u};

    signal(SIGINT , zenmon_signalHandler); // handle keyboard interrupt
    signal(SIGTERM, zenmon_signalHandler); // handle termination
    signal(SIGQUIT, zenmon_signalHandler); // handle keyboard quit
    signal(SIGSTOP, zenmon_signalHandler); // handle stop
    signal(SIGABRT, zenmon_signalHandler); // handle abort() call
    setlocale(LC_ALL, "");               // enable Braille support
    zenmon_getopt(argc, argv);
    HIDE_CURSOR;
    CLEAR_SCREEN;

    num_init();
    dot_init();
    braille_init(LEN_GR_C_X); // set graph width in characters

    // draw status boxes
    box_status("SCI2"              , locStatusWin.xPos, locStatusWin.yPos, F_BLU);
    box_eGraph("Electrical status" , locEGraphWin.xPos, locEGraphWin.yPos, F_BLU);
    box_tGraph("Temperature status", locTGraphWin.xPos, locTGraphWin.yPos, F_BLU);

    // fill boxes
    while(UNTIL_SIGNAL)
    {
        // process numeric values
        num_statusWin(locStatusWin.xPos, locStatusWin.yPos);

        // handle graphs
        dot_updateGraphs();
        dot_eGraphWin(locEGraphWin.xPos, locEGraphWin.yPos);
        dot_tGraphWin(locTGraphWin.xPos, locTGraphWin.yPos);

        fflush(stdout);     // stop text flickering
        usleep(sampleRate); // wait
    }

    // normally unreachable
    zenmon_signalHandler();
    return EXIT_SUCCESS;
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
