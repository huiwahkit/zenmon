// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUDES
//======================================================================================================================
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
    box_draw(xPos, yPos, LEN_SVI2_W_X, LEN_SVI2_W_Y, colour);

    xPos += 2u;
    PRINTL(xPos, yPos++, " %s ", title);

    // print labels
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
    box_draw(xPos, yPos, LEN_LOAD_W_X, LEN_LOAD_W_Y, colour);
    PRINTL(xPos + 2u, yPos, " %s ", title);
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
