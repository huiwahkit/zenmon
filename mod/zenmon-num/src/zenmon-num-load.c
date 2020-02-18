// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// system includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

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
static sysLoadType* sDB; // system usage database

//======================================================================================================================
// FUNCTIONS
//======================================================================================================================
static void load_getMemUsg() //-------------------------------------------------------------------------- load_getMemUsg
{
    uint8  line[LEN_LINE];
    uint32 freeMem = 0u;

    fseek((*sDB).memFD, 0, SEEK_SET);
    while(NULL != fgets(line, LEN_LINE, (*sDB).memFD))
    {
        // this if is VERY barbaric, but it's MUCH faster than strstr
        if(('T' == line[3]) && ('o' == line[4])) // pattern found: "MemTotal"
        {
            (*sDB).memAll = (atoi(line + MEM_TOTAL_OFFSET) / 1000u); // mB
            continue;
        }
        else if(('F' == line[3]) && ('r' == line[4])) // pattern found: "MemFree"
        {
            freeMem = ((atoi(line + MEM_AVAIL_OFFSET) / 1000u)); // mB
            continue;
        }
        else if(('f' == line[2]) && ('f' == line[3])) // pattern found: "Buffers"
        {
            freeMem += ((atoi(line + MEM_AVAIL_OFFSET) / 1000u)); // mB
            continue;
        }
        else if(('C' == line[0]) && ('a' == line[1])) // pattern found: "Cached"
        {
            freeMem += ((atoi(line + MEM_AVAIL_OFFSET) / 1000u)); // mB
            //break; // todo: using break here stops memory updates and I cannot figure out why
        }
        else;
    }

    (*sDB).memUsg = (*sDB).memAll - freeMem;
}

static void load_getCpuUsg(void) //---------------------------------------------------------------------- load_getCpuUsg
{
    // for the 1st iteration of usage calculation, the values will be since boot (since past sum and idle are 0)

    uint8  line[LEN_LINE];
    uint8  lineIndex  = 0u;
    uint8  tokenIndex = 0u;
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

        // save the cpu thread usage permille for the current cycle
        (*sDB).cpuUsg[lineIndex].idle = 1000u - ((idle - (*sDB).cpuUsg[lineIndex].lastIdle) * 1000u / \
                                                 (sum  - (*sDB).cpuUsg[lineIndex].lastSum));

        // save the current values for the next cycle
        (*sDB).cpuUsg[lineIndex].lastSum  = sum;
        (*sDB).cpuUsg[lineIndex].lastIdle = idle;
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
            if(CPU_CORES <= index)
            {
                break; // exit after the physical cores' mHz have been found; the rest are virtual
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
    uint16   segMhz; // mHz to display with segments
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
            else // clear the rest of the boxes in case mHz go down
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

void load_sysInfo(const uint16 xPos, const uint16 yPos) //------------------------------------------------- load_sysInfo
{
    struct sysinfo sysInfo;
    struct utsname  osInfo;

    sysinfo(&sysInfo); // get updated values for uptime, processes and load
    uname(&osInfo);    // get the kernel release; todo: this should only really be called once at startup

    PRINTL(xPos, yPos     , "─────────────────── %sSYS%s ───────────────────", F_BLD, F_RST);
    PRINTL(xPos, yPos + 1u, "   %sKernel%s: %s"               , F_BLD, F_RST , osInfo.release);
    PRINTL(xPos, yPos + 2u, "   %sUptime%s: %02ld:%02ld:%02ld", F_BLD, F_RST , sysInfo.uptime / 3600u    , \
                                                                               sysInfo.uptime / 60u % 60u, \
                                                                               sysInfo.uptime       % 60u);
    PRINTL(xPos, yPos + 3u, "%sProcesses%s: %u   "                           , F_BLD, F_RST, sysInfo.procs);
    PRINTL(xPos, yPos + 4u, "     %sLoad%s: %2.2f %2.2f %2.2f", F_BLD, F_RST , \
                            sysInfo.loads[0] / ((float)(1u << SI_LOAD_SHIFT)), \
                            sysInfo.loads[1] / ((float)(1u << SI_LOAD_SHIFT)), \
                            sysInfo.loads[2] / ((float)(1u << SI_LOAD_SHIFT)));
}

void load_memBar(const uint16 xPos, const uint16 yPos) //--------------------------------------------------- load_memBar
{
    load_getMemUsg(); // call this first to avoid div/0

    // RAM transformed in usage bar characters
    const uint32 mbPerBox = (*sDB).memAll / RAM_BAR_LEN; // worth in MB of a box
    const uint32 mbPerSeg =      mbPerBox / SEG_PER_BOX; // worth in MB of a seg
    const uint16     yBar = yPos + 2u;                   // start printing from the 2nd X coord

    uint8     boxes = (*sDB).memUsg / mbPerBox; // boxes to fill
    uint16    segMB = (*sDB).memUsg % mbPerBox; // mB to display with segments
    sint8*   colour = F_RST;
    uint8  boxIndex;

    PRINTL(xPos      , yPos     , "─────────────────── %sRAM%s ───────────────────", F_BLD, F_RST);
    PRINTL(xPos + 16u, yPos + 1u, "%5u/%5u"                                        , (*sDB).memUsg, (*sDB).memAll);

    if((0u == boxes) && (mbPerSeg > segMB)) segMB = mbPerSeg; // make sure no bar is invisible

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
                 if(segMB <  mbPerSeg      ) { PRINTL(xPos + boxIndex, yBar, "%s "  , colour);         continue; }
            else if(segMB < (mbPerSeg * 2u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258F); continue; }
            else if(segMB < (mbPerSeg * 3u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258E); continue; }
            else if(segMB < (mbPerSeg * 4u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258D); continue; }
            else if(segMB < (mbPerSeg * 5u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258C); continue; }
            else if(segMB < (mbPerSeg * 6u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258B); continue; }
            else if(segMB < (mbPerSeg * 7u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x258A); continue; }
            else if(segMB < (mbPerSeg * 8u)) { PRINTL(xPos + boxIndex, yBar, "%s%lc", colour, 0x2589); continue; }
            else; // unreachable
        }
        else // clear the rest of the boxes in case mem usage goes down
        {
            PRINTL(xPos + boxIndex, yBar, "%s ", colour);
        }
    }
}

void load_cpuBar(const uint16 xPos, const uint16 yPos) //--------------------------------------------------- load_cpuBar
{
    const uint16 yUsg = yPos + 1u;
    const uint16 yMhz = yPos + 7u;

    load_getCpuUsg();
    load_getCpuMhz();

    PRINTL(xPos      , yPos      , "─────────────────── %sCPU%s ───────────────────", F_BLD, F_RST);
    PRINTL(xPos + 20u, yPos +  5u, "usg");
    PRINTL(xPos      , yPos +  6u, "CCD0-CCX0 CCD0-CCX1     CCD1-CCX2 CCD1-CCX3");

    PRINTL(xPos + 20u, yPos + 11u, "mhz");
    PRINTL(xPos      , yPos + 12u, "CCD0-CCX0 CCD0-CCX1     CCD1-CCX2 CCD1-CCX3");

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

    // print mHz bars for CCD0
    load_printCpuMhz(xPos      , yMhz, (*sDB).cpuMhz,  0u,  1u); // print mHz bars for 2 cores
    load_printCpuMhz(xPos +  5u, yMhz, (*sDB).cpuMhz,  2u,  3u); // print mHz bars for 2 cores
    load_printCpuMhz(xPos + 10u, yMhz, (*sDB).cpuMhz,  4u,  5u); // print mHz bars for 2 cores
    load_printCpuMhz(xPos + 15u, yMhz, (*sDB).cpuMhz,  6u,  7u); // print mHz bars for 2 cores

    // print mHz bars for CCD0
    load_printCpuMhz(xPos + 24u, yMhz, (*sDB).cpuMhz,  8u,  9u); // print mHz bars for 2 cores
    load_printCpuMhz(xPos + 29u, yMhz, (*sDB).cpuMhz, 10u, 11u); // print mHz bars for 2 cores
    load_printCpuMhz(xPos + 34u, yMhz, (*sDB).cpuMhz, 12u, 13u); // print mHz bars for 2 cores
    load_printCpuMhz(xPos + 39u, yMhz, (*sDB).cpuMhz, 14u, 15u); // print mHz bars for 2 cores
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================