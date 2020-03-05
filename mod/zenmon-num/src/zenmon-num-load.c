// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// system includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>

// dependency includes
#include "gen-types.h"
#include "logging.h"

// local includes
#include "zenmon-cfg.h"
#include "zenmon-types.h"
#include "zenmon-num-load.h"

//======================================================================================================================
// VARIABLES
//======================================================================================================================
static sysLoadType* sDB;          // system usage database
static uint8*       ramUsgFormat; // used to keep the "used/total" string centered
static uint8        ramUsgOffset; // used to keep the "used/total" string centered

//======================================================================================================================
// FUNCTIONS
//======================================================================================================================
static void load_getMemUsg() //-------------------------------------------------------------------------- load_getMemUsg
{
    uint8  line[LEN_LINE];
    uint32 freeMem = 0u;

    static bool checkedOnce = FALSE;

    fseek((*sDB).ramFD, 0, SEEK_SET);
    while(NULL != fgets(line, LEN_LINE, (*sDB).ramFD))
    {
        // this if is VERY barbaric, but it's MUCH faster than strstr
        if(('F' == line[3]) && ('r' == line[4])) // found "MemFree"
        {
            freeMem = ((atoi(line + RAM_AVAIL_OFFSET) / 1000u)); // MiB
            continue;
        }
        else if(('f' == line[2]) && ('f' == line[3])) // found "Buffers"
        {
            freeMem += ((atoi(line + RAM_AVAIL_OFFSET) / 1000u)); // MiB
            continue;
        }
        else if(('C' == line[0]) && ('a' == line[1])) // found "Cached"
        {
            freeMem += ((atoi(line + RAM_AVAIL_OFFSET) / 1000u)); // MiB
            continue;
        }
        else if((FALSE == checkedOnce) && ('m' == line[2]) && ('T' == line[3]) && ('o' == line[4])) // found "MemTotal"
        {
            (*sDB).ramAll = (atoi(line + RAM_TOTAL_OFFSET) / 1000u); // MiB; this value is only acquired once

            // keep the "used/total" string centered
                 if(1000u    > (*sDB).ramAll) { ramUsgOffset = 18u; ramUsgFormat = "%3u/%3u"; }
            else if(10000u   > (*sDB).ramAll) { ramUsgOffset = 17u; ramUsgFormat = "%4u/%4u"; }
            else if(100000u  > (*sDB).ramAll) { ramUsgOffset = 16u; ramUsgFormat = "%5u/%5u"; }
            else if(1000000u > (*sDB).ramAll) { ramUsgOffset = 15u; ramUsgFormat = "%6u/%6u"; }
            else                              { ramUsgOffset =  7u; ramUsgFormat = "your amount of RAM is extreme"; }

            checkedOnce = TRUE; // never reachable again after this
        }
        else;
    }

    (*sDB).ramUsg = (*sDB).ramAll - freeMem;
}

static void load_getCpuUsg(void) //---------------------------------------------------------------------- load_getCpuUsg
{
    // for the 1st iteration of usage calculation, the values will be since boot (since past sum and idle are 0)

    uint8  line[LEN_LINE];
    uint8  lineIndex   = 0u; // go through the lines  in the file
    uint8  tokenIndex  = 0u; // go through the tokens in the line
    uint8  threadIndex = 0u; // arrange the threads properly to match their core
    uint8* token;
    uint32 sum;  // total OS ticks
    uint32 idle; //  idle OS ticks

    fseek((*sDB).usgFD, 0, SEEK_SET);    // go back to the start of the file
    fgets(line, LEN_LINE, (*sDB).usgFD); // ignore the first line of /proc/stat, which is overall, not thread-specific

    for(lineIndex = 0u; lineIndex < CPU_THREADS; lineIndex++)
    {
        // reset values for current cycle
        tokenIndex = 0u;
        sum        = 0u;
        idle       = 0u;

        fgets(line, LEN_LINE, (*sDB).usgFD); // get the current line
        token = strtok(line, " ");           // skip the 1st token, the cpu thread number

        while(NULL != token) // get the tokens, add them and save the idle separately
        {
            token = strtok(NULL, " ");

            if(NULL != token)
            {
                sum += atoi(token);
                if(tokenIndex == 3u) idle = atoi(token);
                tokenIndex++;
            }
        }

        // the first half of the threads are physical cores; the rest are virtual cores (on SMT CPUs)
        // normally this is fine, but when displaying the usg, each core is associated with its 2 threads
        // this association is not perfect: there is a delay between getting the usg and the MHz
        // these values are adjusted VERY VERY fast by the CPU, so high usg won't always mean high MHz when displaying
        if(CPU_CORES > lineIndex) // these are the physical cores (0-15 on my machine)
        {
            threadIndex = lineIndex * 2u; // select even indexes
        }
        else // these are the virtual cores (16-31 on my machine); on CPUs without SMT this is unreachable
        {
            threadIndex = (lineIndex * 2u) - CPU_THREADS + 1u; // select odd indexes
        }

        // save the cpu thread usage permille for the current cycle
        (*sDB).cpuUsg[threadIndex].idle = 1000u - ((idle - (*sDB).cpuUsg[threadIndex].lastIdle) * 1000u / \
                                                   (sum  - (*sDB).cpuUsg[threadIndex].lastSum));

        // save the current values for the next cycle
        (*sDB).cpuUsg[threadIndex].lastSum  = sum;
        (*sDB).cpuUsg[threadIndex].lastIdle = idle;
    }
}

static void load_getCpuMhz(void) //---------------------------------------------------------------------- load_getCpuMhz
{
    uint8 line[LEN_LINE];
    uint8 index = 0u;

    fseek((*sDB).mhzFD, 0, SEEK_SET); // go back to the start of the file
    while(NULL != fgets(line, LEN_LINE, (*sDB).mhzFD))
    {
        // this if is VERY barbaric, but it's MUCH faster than strstr
        if((' ' == line[3]) && ('M' == line[4])) // pattern found: "cpu Mhz"
        {
            // the first half of the threads are physical cores; the rest are virtual cores (on SMT CPUs)
            if(CPU_CORES <= index)
            {
                break; // exit after the physical cores' MHz have been found
            }
            else
            {
                (*sDB).cpuMhz[index] = atoi(line + MHZ_OFFSET);
                index++;
            }
        }
    }
}

static void load_printCpuUsg(uint16 xPos, uint16 yPos, const cpuUsgType* array, const uint8 start, const uint8 end) // load_printCpuUsg
{
    uint8     boxes; // boxes to fill
    uint16   segUsg; // usg (in permille) to display with segments
    sint8*   colour;
    uint8  barIndex;
    uint8  cpuIndex;

    yPos += (USG_BAR_LEN - 1u); // print the bars from the botton up

    for(cpuIndex = start; cpuIndex <= end; cpuIndex++)
    {
        boxes  = ((*(array + cpuIndex)).idle) / USG_PER_BOX;
        segUsg = ((*(array + cpuIndex)).idle) % USG_PER_BOX;
        colour = F_RST;

        if((0u == boxes) && (USG_PER_SEG > segUsg)) segUsg = USG_PER_SEG; // make sure no bar is invisible

        for(barIndex = 0u; barIndex < USG_BAR_LEN; barIndex++)
        {
            if(barIndex >= USG_BAR_LIM) // set the warning colour if needed
            {
                colour = F_RED;
            }
            else if(0u == (cpuIndex % 2u)) // alternatively change the thread columns' colours
            {
                colour = F_BLU;
            }
            else;

            if(barIndex < boxes) // print a full box
            {
                PRINTL(xPos, yPos - barIndex, "%s%lc", colour, 0x2588);
                continue;
            }
            else if(barIndex == boxes) // print the last character, the segment, with 8 possible values
            {
                     if(segUsg <  USG_PER_SEG      ) {PRINTL(xPos, yPos - barIndex, "%s "  , colour);         continue;}
                else if(segUsg < (USG_PER_SEG * 2u)) {PRINTL(xPos, yPos - barIndex, "%s%lc", colour, 0x2581); continue;}
                else if(segUsg < (USG_PER_SEG * 3u)) {PRINTL(xPos, yPos - barIndex, "%s%lc", colour, 0x2582); continue;}
                else if(segUsg < (USG_PER_SEG * 4u)) {PRINTL(xPos, yPos - barIndex, "%s%lc", colour, 0x2583); continue;}
                else if(segUsg < (USG_PER_SEG * 5u)) {PRINTL(xPos, yPos - barIndex, "%s%lc", colour, 0x2584); continue;}
                else if(segUsg < (USG_PER_SEG * 6u)) {PRINTL(xPos, yPos - barIndex, "%s%lc", colour, 0x2585); continue;}
                else if(segUsg < (USG_PER_SEG * 7u)) {PRINTL(xPos, yPos - barIndex, "%s%lc", colour, 0x2586); continue;}
                else if(segUsg < (USG_PER_SEG * 8u)) {PRINTL(xPos, yPos - barIndex, "%s%lc", colour, 0x2587); continue;}
                else; // unreachable
            }
            else // clear the rest of the boxes in case usg goes down
            {
                PRINTL(xPos, yPos - barIndex, "%s ", colour);
            }
        }

        xPos++;
    }
}

static void load_printCpuMhz(uint16 xPos, uint16 yPos, const uint16* array, const uint8 start, const uint8 end) // load_printCpuMhz
{
    uint8     boxes; // boxes to fill
    uint16   segMhz; // MHz to display with segments
    sint8*   colour;
    uint8  barIndex;
    uint8  cpuIndex;

    yPos += MHZ_BAR_LEN - 1u; // print the bars from the botton up

    for(cpuIndex = start; cpuIndex <= end; cpuIndex++)
    {
        boxes  = *(array + cpuIndex) / MHZ_PER_BOX;
        segMhz = *(array + cpuIndex) % MHZ_PER_BOX;
        colour = F_RST;

        if((0u == boxes) && (MHZ_PER_SEG > segMhz)) segMhz = MHZ_PER_SEG; // make sure no bar is invisible

        for(barIndex = 0u; barIndex < MHZ_BAR_LEN; barIndex++)
        {
            if(barIndex >= MHZ_BAR_LIM) // set the warning colour if needed
            {
                colour = F_RED;
            }
            else if(0u == (cpuIndex % 2u)) // alternatively change the core columns' colours
            {
                colour = F_BLU;
            }
            else;

            if(barIndex < boxes) // print a full box
            {
                PRINTL(xPos     , yPos - barIndex, "%s%lc", colour, 0x2588);
                PRINTL(xPos + 1u, yPos - barIndex, "%s%lc", colour, 0x2588);
                continue;
            }
            else if(barIndex == boxes) // print the last character, the segment, with 8 possible values
            {
                     if(segMhz <  MHZ_PER_SEG      ) { PRINTL(xPos     , yPos - barIndex, "%s "  , colour);         \
                                                       PRINTL(xPos + 1u, yPos - barIndex, "%s "  , colour);         \
                                                       continue; }
                else if(segMhz < (MHZ_PER_SEG * 2u)) { PRINTL(xPos     , yPos - barIndex, "%s%lc", colour, 0x2581); \
                                                       PRINTL(xPos + 1u, yPos - barIndex, "%s%lc", colour, 0x2581); \
                                                       continue; }
                else if(segMhz < (MHZ_PER_SEG * 3u)) { PRINTL(xPos     , yPos - barIndex, "%s%lc", colour, 0x2582); \
                                                       PRINTL(xPos + 1u, yPos - barIndex, "%s%lc", colour, 0x2582); \
                                                       continue; }
                else if(segMhz < (MHZ_PER_SEG * 4u)) { PRINTL(xPos     , yPos - barIndex, "%s%lc", colour, 0x2583); \
                                                       PRINTL(xPos + 1u, yPos - barIndex, "%s%lc", colour, 0x2583); \
                                                       continue; }
                else if(segMhz < (MHZ_PER_SEG * 5u)) { PRINTL(xPos     , yPos - barIndex, "%s%lc", colour, 0x2584); \
                                                       PRINTL(xPos + 1u, yPos - barIndex, "%s%lc", colour, 0x2584); \
                                                       continue; }
                else if(segMhz < (MHZ_PER_SEG * 6u)) { PRINTL(xPos     , yPos - barIndex, "%s%lc", colour, 0x2585); \
                                                       PRINTL(xPos + 1u, yPos - barIndex, "%s%lc", colour, 0x2585); \
                                                       continue; }
                else if(segMhz < (MHZ_PER_SEG * 7u)) { PRINTL(xPos     , yPos - barIndex, "%s%lc", colour, 0x2586); \
                                                       PRINTL(xPos + 1u, yPos - barIndex, "%s%lc", colour, 0x2586); \
                                                       continue; }
                else if(segMhz < (MHZ_PER_SEG * 8u)) { PRINTL(xPos     , yPos - barIndex, "%s%lc", colour, 0x2587); \
                                                       PRINTL(xPos + 1u, yPos - barIndex, "%s%lc", colour, 0x2587); \
                                                       continue; }
                else; // unreachable
            }
            else // clear the rest of the boxes in case MHz go down
            {
                PRINTL(xPos     , yPos - barIndex, "%s ", colour);
                PRINTL(xPos + 1u, yPos - barIndex, "%s ", colour);
            }
        }

        xPos += 2;
    }
}

//======================================================================================================================
// API
//======================================================================================================================
void load_init(sysLoadType* const inDB) //-------------------------------------------------------------------- load_init
{
    sDB = inDB;
}

void load_getCpuBar(void)
{
    load_getCpuUsg();
    load_getCpuMhz();
}

void load_printSysInfo(const uint16 xPos, const uint16 yPos) //--------------------------------------- load_printSysInfo
{
    // data acquired here is not dependent on the last polling cycle
    // so there is no need to acquire it when the terminal is too small to display the windows

    // these hardcoded positions depend on where the static labels are printed in zenmon-box.c

    struct sysinfo sysInfo;

    sysinfo(&sysInfo); // get updated values for uptime, processes and load

    PRINTL(xPos + 11u, yPos + 2u, "%02ld:%02ld:%02ld", sysInfo.uptime / 3600u    , \
                                                       sysInfo.uptime / 60u % 60u, \
                                                       sysInfo.uptime       % 60u);
    PRINTL(xPos + 11u, yPos + 3u, "%u"               , sysInfo.procs);
    PRINTL(xPos + 11u, yPos + 4u, "%2.2f %2.2f %2.2f", sysInfo.loads[0] / ((float)(1u << SI_LOAD_SHIFT)), \
                                                       sysInfo.loads[1] / ((float)(1u << SI_LOAD_SHIFT)), \
                                                       sysInfo.loads[2] / ((float)(1u << SI_LOAD_SHIFT)));
}

void load_printRamBar(const uint16 xPos, const uint16 yPos) //----------------------------------------- load_printRamBar
{
    // data acquired here is not dependent on last polling cycle
    // so there is no need to acquire it when the terminal is too small to display the windows

    // these hardcoded positions depend on where the static labels are printed in zenmon-box.c

    load_getMemUsg(); // call this first to avoid div/0

    // RAM transformed in usage bar characters
    const uint32 mibPerBox = (*sDB).ramAll / RAM_BAR_LEN; // worth in MiB of a box
    const uint32 mibPerSeg =     mibPerBox / SEG_PER_BOX; // worth in MiB of a seg
    const uint16      yBar = yPos + 2u;                   // start printing from the 2nd X coord

    uint8     boxes = (*sDB).ramUsg / mibPerBox; // boxes to fill
    uint16   segMiB = (*sDB).ramUsg % mibPerBox; // MiB to display with segments
    sint8*   colour = F_RST;
    uint8  boxIndex;

    PRINTL(xPos + ramUsgOffset, yPos + 1u, ramUsgFormat, (*sDB).ramUsg, (*sDB).ramAll); // print "used/total"

    if((0u == boxes) && (mibPerSeg > segMiB)) segMiB = mibPerSeg; // make sure no bar is invisible

    for(boxIndex = 0u; boxIndex < RAM_BAR_LEN; boxIndex++)
    {
        if(boxIndex >= RAM_BAR_LIM) colour = F_RED; // set the colour based on the progress through the entire bar

        if(boxIndex < boxes) // print a full box
        {
            PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x2588);
            continue;
        }
        else if(boxIndex == boxes) // print the last character, the segment, with 8 possible values
        {
                 if(segMiB <  mibPerSeg      ) { PRINTL(xPos + boxIndex, yBar, "%s "  , colour);         continue; }
            else if(segMiB < (mibPerSeg * 2u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258F); continue; }
            else if(segMiB < (mibPerSeg * 3u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258E); continue; }
            else if(segMiB < (mibPerSeg * 4u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258D); continue; }
            else if(segMiB < (mibPerSeg * 5u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258C); continue; }
            else if(segMiB < (mibPerSeg * 6u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258B); continue; }
            else if(segMiB < (mibPerSeg * 7u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258A); continue; }
            else if(segMiB < (mibPerSeg * 8u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x2589); continue; }
            else; // unreachable
        }
        else // clear the rest of the boxes in case RAM usage goes down
        {
            PRINTL(xPos + boxIndex, yBar, "%s ", colour);
        }
    }
}

void load_printCpuBar(const uint16 xPos, const uint16 yPos) //----------------------------------------- load_printCpuBar
{
    // these hardcoded positions depend on where the static labels are printed in zenmon-box.c

    const uint16 yUsg = yPos + 1u;
    const uint16 yMhz = yPos + 7u;

    // print usage bars for CCD0
    load_printCpuUsg(xPos      , yUsg, (*sDB).cpuUsg,  0u,  3u); // print usg bars for 4 threads
    load_printCpuUsg(xPos +  5u, yUsg, (*sDB).cpuUsg,  4u,  7u); // print usg bars for 4 threads
    load_printCpuUsg(xPos + 10u, yUsg, (*sDB).cpuUsg,  8u, 11u); // print usg bars for 4 threads
    load_printCpuUsg(xPos + 15u, yUsg, (*sDB).cpuUsg, 12u, 15u); // print usg bars for 4 threads

    // print usage bars for CCD0
    load_printCpuUsg(xPos + 24u, yUsg, (*sDB).cpuUsg, 16u, 19u); // print usg bars for 4 threads
    load_printCpuUsg(xPos + 29u, yUsg, (*sDB).cpuUsg, 20u, 23u); // print usg bars for 4 threads
    load_printCpuUsg(xPos + 34u, yUsg, (*sDB).cpuUsg, 24u, 27u); // print usg bars for 4 threads
    load_printCpuUsg(xPos + 39u, yUsg, (*sDB).cpuUsg, 28u, 31u); // print usg bars for 4 threads

    // print MHz bars for CCD0
    load_printCpuMhz(xPos      , yMhz, (*sDB).cpuMhz,  0u,  1u); // print MHz bars for 2 cores
    load_printCpuMhz(xPos +  5u, yMhz, (*sDB).cpuMhz,  2u,  3u); // print MHz bars for 2 cores
    load_printCpuMhz(xPos + 10u, yMhz, (*sDB).cpuMhz,  4u,  5u); // print MHz bars for 2 cores
    load_printCpuMhz(xPos + 15u, yMhz, (*sDB).cpuMhz,  6u,  7u); // print MHz bars for 2 cores

    // print MHz bars for CCD0
    load_printCpuMhz(xPos + 24u, yMhz, (*sDB).cpuMhz,  8u,  9u); // print MHz bars for 2 cores
    load_printCpuMhz(xPos + 29u, yMhz, (*sDB).cpuMhz, 10u, 11u); // print MHz bars for 2 cores
    load_printCpuMhz(xPos + 34u, yMhz, (*sDB).cpuMhz, 12u, 13u); // print MHz bars for 2 cores
    load_printCpuMhz(xPos + 39u, yMhz, (*sDB).cpuMhz, 14u, 15u); // print MHz bars for 2 cores
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
