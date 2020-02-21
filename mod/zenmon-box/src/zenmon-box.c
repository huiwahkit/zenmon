// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// system includes
#include <stdio.h>
#include <sys/utsname.h>

// dependency includes
#include "braille-lib.h"

// local includes
#include "zenmon-cfg.h"
#include "zenmon-box.h"

//======================================================================================================================
// FUNCTIONS
//======================================================================================================================
static void box_draw(const uint16 xPos , const uint16 yPos , \
                     const uint16 xSize, const uint16 ySize, const sint8* colour) //--------------------------- box_draw
{
    uint16 index;

    // draw corners
    PRINTL(xPos             , yPos             , "%s%lc", colour, 0x250C);
    PRINTL(xPos + xSize - 1u, yPos             , "%s%lc", colour, 0x2510);
    PRINTL(xPos             , yPos + ySize - 1u, "%s%lc", colour, 0x2514);
    PRINTL(xPos + xSize - 1u, yPos + ySize - 1u, "%s%lc", colour, 0x2518);

    // draw sides
    for(index = xPos + 1; index < (xPos + xSize - 1); index++)
    {
        PRINTL(index            , yPos             , "%s%lc", colour, 0x2500);
        PRINTL(index            , yPos + ySize - 1u, "%s%lc", colour, 0x2500);
    }
    for(index = yPos + 1; index < (yPos + ySize - 1); index++)
    {
        PRINTL(xPos             , index            , "%s%lc", colour, 0x2502);
        PRINTL(xPos + xSize - 1u, index            , "%s%lc", colour, 0x2502);
    }
}

//======================================================================================================================
// API
//======================================================================================================================
void box_svi2(const sint8* title, uint16 xPos, uint16 yPos, const sint8* colour) //---------------------------- box_svi2
{
    // these hardcoded positions must be kept in mind in the printer functions, when aligning all text

    box_draw(xPos, yPos, LEN_SVI2_W_X, LEN_SVI2_W_Y, colour);

    xPos += 2u;
    PRINTL(xPos, yPos++, " %s ", title);

    // static labels
    PRINTL(xPos, yPos++, "%s Sensor     Min      Val      Max      Avg", F_BLD);
    PRINTL(xPos, yPos++, "cV·Core");
    PRINTL(xPos, yPos++, " A·Core");
    PRINTL(xPos, yPos++, " W·Core");
    PRINTL(xPos, yPos++, "cV·SoC");
    PRINTL(xPos, yPos++, " A·SoC");
    PRINTL(xPos, yPos++, " W·SoC");
    PRINTL(xPos, yPos++, "───────────────────────────────────────────");
    PRINTL(xPos, yPos++, "°C·Die");
    PRINTL(xPos, yPos++, "°C·Ctl");
    PRINTL(xPos, yPos++, "°C·CCD0");
    PRINTL(xPos, yPos++, "°C·CCD1");
}

void box_load(const sint8* title, uint16 const xPos, uint16 const yPos, const sint8* colour) //---------------- box_load
{
    // these hardcoded positions must be kept in mind in the printer functions, when aligning all text

    struct utsname osInfo;

    uname(&osInfo); // get the kernel name and release

    box_draw(xPos, yPos, LEN_LOAD_W_X, LEN_LOAD_W_Y, colour);
    PRINTL(xPos +  2u, yPos      , " %s ", title);

    // static labels for sysinfo
    PRINTL(xPos +  2u, yPos  + 1u, "─────────────────── %sSYS%s ───────────────────", F_BLD, F_RST);
    PRINTL(xPos +  5u, yPos  + 2u,    "%sKernel%s: %s %s", F_BLD, F_RST , osInfo.sysname, osInfo.release);
    PRINTL(xPos +  5u, yPos  + 3u,    "%sUptime%s:"      , F_BLD, F_RST);
    PRINTL(xPos +  2u, yPos  + 4u, "%sProcesses%s:"      , F_BLD, F_RST);
    PRINTL(xPos +  7u, yPos  + 5u,      "%sLoad%s:"      , F_BLD, F_RST);

    // static labels for meminfo
    PRINTL(xPos +  2u, yPos  + 7u, "─────────────────── %sRAM%s ───────────────────", F_BLD, F_RST);

    // static labels for cpuinfo
    PRINTL(xPos +  2u, yPos + 11u, "─────────────────── %sCPU%s ───────────────────", F_BLD, F_RST);
    PRINTL(xPos + 22u, yPos + 16u,                       "usg");
    PRINTL(xPos +  2u, yPos + 17u,   "CCD0-CCX0 CCD0-CCX1     CCD1-CCX2 CCD1-CCX3");

    PRINTL(xPos + 22u, yPos + 22u,                       "MHz");
    PRINTL(xPos +  2u, yPos + 23u,   "CCD0-CCX0 CCD0-CCX1     CCD1-CCX2 CCD1-CCX3");
}

void box_eGraph(const sint8* title, uint16 const xPos, uint16 const yPos, const sint8* colour) //------------ box_eGraph
{
    box_draw(xPos, yPos, LEN_EG_W_X, LEN_EG_W_Y, colour);
    PRINTL(xPos + 2u, yPos, " %s ", title);
}

void box_tGraph(const sint8* title, uint16 const xPos, const uint16 yPos, const sint8* colour) //------------ box_tGraph
{
    box_draw(xPos, yPos, LEN_CG_W_X, LEN_CG_W_Y, colour);
    PRINTL(xPos + 2u, yPos, " %s ", title);
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
