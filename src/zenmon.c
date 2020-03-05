// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// system includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <sys/ioctl.h>

// dependency includes
#include "gen-types.h"
#include "logging.h"
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
#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 0

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
static void zenmon_signalHandler() //-------------------------------------------------------------- zenmon_signalHandler
{
    num_deinit();
    CURSOR_SHOW;
    CLEAR_SCREEN;
    exit(EXIT_SUCCESS);
}

void zenmon_getopt(int argc, char* const argv[]) //------------------------------------------------------- zenmon_getopt
{
    sint8 option;
    uint8 exitFlag = 0u; // 0 for normal operation, 1 for help/error, 2 for version

    opterr = 0;
    while(-1 != (option = getopt(argc, argv, "r:hv")))
    {
        switch(option)
        {
            case 'r':
            {
                sampleRate = (atoi(optarg) * 1000);
                break;
            }
            case 'h':
            {
                exitFlag = 1u;
                break;
            }
            case 'v':
            {
                exitFlag = 2u;
                break;
            }
            case '?':
            {
                if('r' == optopt)
                {
                    PRINT_FAIL("Option needs an argument: -%c", optopt);
                }
                else
                {
                    PRINT_FAIL("Unknown option: -%c", optopt);
                }
                exitFlag = 1u;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    for(; optind < argc; optind++)
    {
        PRINT_FAIL("Non-option argument: %s", argv[optind]);
        exitFlag = TRUE;
    }

    if(1u == exitFlag)
    {
        PRINT_INFO("Usage:");
        PRINT_TEXT("-r\t\tset the sample rate in ms; default is 1000");
        PRINT_TEXT("-h\t\tprint this message");
        PRINT_TEXT("-v\t\tprint the build version");
        exit(EXIT_SUCCESS);
    }
    else if(2u == exitFlag)
    {
        PRINT_INFO("zenmon version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
        exit(EXIT_SUCCESS);
    }
}

//======================================================================================================================
// MAIN
//======================================================================================================================
int main(int argc, char* const argv[]) //-------------------------------------------------------------------------- main
{
    // info related to the terminal window sizes
    struct winsize oldTerm;
    struct winsize nowTerm;

    // set window coordinates
    const coordType svi2Pos   = { 4u,  2u};
    const coordType loadPos   = { 4u, 16u};
    const coordType eGraphPos = {51u,  2u};
    const coordType tGraphPos = {51u, 19u};

    // initialize to 0, so in the first iteration the window borders will be printed
    oldTerm.ws_row = 0;
    oldTerm.ws_col = 0;

    signal(SIGINT , zenmon_signalHandler); // handle keyboard interrupt
    signal(SIGTERM, zenmon_signalHandler); // handle termination
    signal(SIGQUIT, zenmon_signalHandler); // handle keyboard quit
    signal(SIGSTOP, zenmon_signalHandler); // handle stop
    signal(SIGABRT, zenmon_signalHandler); // handle abort() call
    setlocale(LC_ALL, "");                 // enable Braille support
    zenmon_getopt(argc, argv);
    CURSOR_HIDE;
    TERM_NAME("zenmon");

    num_init();
    dot_init();
    braille_init(LEN_GR_C_X); // set graph width in characters

    // fill boxes
    while(UNTIL_SIGNAL)
    {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &nowTerm); // get the terminal size

        // if the terminal size changed, refresh the static status boxes
        if((oldTerm.ws_row != nowTerm.ws_row) || \
           (oldTerm.ws_col != nowTerm.ws_col))
        {
            CLEAR_SCREEN;
            box_svi2(  "SVI2 Overview"   ,   svi2Pos.xPos,   svi2Pos.yPos, F_BLU);
            box_load(  "System Load"     ,   loadPos.xPos,   loadPos.yPos, F_BLU);
            box_eGraph("SVI2 Core cV/A/W", eGraphPos.xPos, eGraphPos.yPos, F_BLU);
            box_tGraph("SVI2 CPU °C"     , tGraphPos.xPos, tGraphPos.yPos, F_BLU);
        }

        // check the terminal window size
        if(((LEN_SVI2_W_X + LEN_EG_W_X   + 5) > nowTerm.ws_col) || \
           ((LEN_SVI2_W_Y + LEN_LOAD_W_Y + 2) > nowTerm.ws_row))
        {
            CLEAR_SCREEN;
            PRINT_FAIL("Terminal size too small; resize it to display data...");

            // acquire and process the data even when nothing is shown so that min/max/avg/history are updated
            num_getSvi2();
            num_getLoad();
            dot_refresh();
        }
        else // start normal operation
        {
            // acquire and process the data
            num_getSvi2();
            num_getLoad();
            dot_refresh();

            // print all numeric data, then refresh the graphs and print them
            num_printSvi2(svi2Pos.xPos,   svi2Pos.yPos);
            num_printLoad(loadPos.xPos,   loadPos.yPos);
            dot_eGraph( eGraphPos.xPos, eGraphPos.yPos);
            dot_tGraph( tGraphPos.xPos, tGraphPos.yPos);

            fflush(stdout); // stop text flickering
        }

        // save the current terminal size to compare against in the next iteration
        oldTerm.ws_row = nowTerm.ws_row;
        oldTerm.ws_col = nowTerm.ws_col;

        usleep(sampleRate); // wait
    }

    // unreachable
    zenmon_signalHandler();
    return EXIT_SUCCESS;
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
