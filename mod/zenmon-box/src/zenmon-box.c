// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUSIONS
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
void box_status(const sint8* title, const uint16 xPos, const uint16 yPos, const sint8* colour) //------------ box_status
{
    uint16 xText = xPos + 2u;
    uint16 yText = yPos;

    box_draw(xPos, yText, LEN_STATUS_W_X, LEN_STATUS_W_Y, colour);
    PRINTL(xText, yText++, " %s%s ", F_RST, title);

    // print labels
    PRINTL(xText, yText++, "%s%s%s" , F_RST, F_BLD, "Sensor    Min      Val      Max      Avg");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "V_CPU");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "A_CPU");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "W_CPU");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "V_SOC");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "A_SOC");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "W_SOC");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "───────────────────────────────────────────");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "T_DIE");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "T_CTL");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "T_CD1");
    PRINTL(xText, yText++, "%s%s"   , F_RST,        "T_CD2");
}

void box_eGraph(const sint8* title, uint16 const xPos, uint16 const yPos, const sint8* colour) //------------ box_eGraph
{
    box_draw(xPos, yPos, LEN_EG_W_X, LEN_EG_W_Y, colour);
    PRINTL(xPos + 2u, yPos + 0u, " %s%s ", F_RST, title);
}

void box_tGraph(const sint8* title, uint16 const xPos, const uint16 yPos, const sint8* colour) //------------ box_tGraph
{
    box_draw(xPos, yPos, LEN_TG_W_X, LEN_TG_W_Y, colour);
    PRINTL(xPos + 2u, yPos + 0u, " %s%s ", F_RST, title);
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
