// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// system includes
#include <string.h>

// dependency includes
#include "braille-lib.h"

// local includes
#include "zenmon-cfg.h"
#include "zenmon-num.h"
#include "zenmon-dot.h"

//======================================================================================================================
// VARIABLES
//======================================================================================================================
static graphBufferType gDB;
static metricsType*    mDB;

//======================================================================================================================
// FUNCTIONS
//======================================================================================================================
static void dot_graphRefresh(metricType* const metric, uint8* const graph, const uint16 width, \
                                                                           const uint8  dot) //-------- dot_graphRefresh
{
    uint16 index;

    // advance the history graph
    for(index = 0u; index < (width - 1u); index++)
    {
        (*metric).hst[index] = (*metric).hst[index + 1];
    }

    // set latest value
    (*metric).hst[width - 1] = (*metric).now;

    // draw the new graph
    for(index = 0u; index < (width - 1u); index++)
    {
        braille_line(graph, index, (*metric).hst[index] / dot, index + 1u, (*metric).hst[index + 1] / dot);
    }
}

static void dot_eGraphDraw(uint8* const graph, const uint16 xPos, const uint16 yPos) //------------------ dot_eGraphDraw
{
    uint16  xLen = 0u;
    sint16  yLen = 0;
     uint8* colour;

    /* print the graph while keeping in mind the colours based on the limits */
    for(yLen = LEN_GR_C_Y - 1; yLen >= 0; yLen--)
    {
        for(xLen = 0u; xLen < LEN_GR_C_X; xLen++)
        {
            // colour change is hardcoded based on LIM_V, LIM_A and LIM_W
            ((LEN_GR_C_Y - 1) <= yLen) ? (colour = F_RED) : (colour = F_GRN);

            // print the Braille symbol in the correct position
            PRINTL(xPos + xLen + 1u, yPos + LEN_GR_C_Y - yLen, "%s%lc", colour, \
                 *(graph + (yLen * LEN_GR_C_X) + xLen) | BRAILLE_NULL);
        }
    }
}

static void dot_tGraphDraw(uint8* const graph, const uint16 xPos, const uint16 yPos) //------------------ dot_tGraphDraw
{
    uint16  xLen = 0u;
    sint16  yLen = 0;
     uint8* colour;

    /* print the graph while keeping in mind the colours based on the limits */
    for(yLen = LEN_GR_C_Y - 1; yLen >= 0; yLen--)
    {
        for(xLen = 0u; xLen < LEN_GR_C_X; xLen++)
        {
            // colour change is hardcoded based on LIM_C
            ((LEN_GR_C_Y - 2) <= yLen) ? (colour = F_RED) : (colour = F_CYN);

            // print the Braille symbol in the correct position
            PRINTL(xPos + xLen + 1u, yPos + LEN_GR_C_Y - yLen, "%s%lc", colour, \
                 *(graph + (yLen * LEN_GR_C_X) + xLen) | BRAILLE_NULL);
        }
    }
}

//======================================================================================================================
// API
//======================================================================================================================
void dot_init(void) //----------------------------------------------------------------------------------------- dot_init
{
    num_exportDB(&mDB); // import the numeric databases
}

void dot_refresh(void) //----------------------------------------------------------------------------------- dot_refresh
{
    // clear the previous screen state
    memset(&gDB, 0u, sizeof(gDB));

    // electrical
    dot_graphRefresh(&((*mDB).vcore), gDB.vcore, LEN_GR_D_X, V_D_VALUE);
    dot_graphRefresh(&((*mDB).acore), gDB.acore, LEN_GR_D_X, A_D_VALUE);
    dot_graphRefresh(&((*mDB).wcore), gDB.wcore, LEN_GR_D_X, W_D_VALUE);

    dot_graphRefresh(&((*mDB).vsoc) , gDB.vsoc , LEN_GR_D_X, V_D_VALUE);
    dot_graphRefresh(&((*mDB).asoc) , gDB.asoc , LEN_GR_D_X, A_D_VALUE);
    dot_graphRefresh(&((*mDB).wsoc) , gDB.wsoc , LEN_GR_D_X, W_D_VALUE);

    // temperatur
    dot_graphRefresh(&((*mDB).cdie) , gDB.cdie , LEN_GR_D_X, C_D_VALUE);
    dot_graphRefresh(&((*mDB).cctl) , gDB.cctl , LEN_GR_D_X, C_D_VALUE);
    dot_graphRefresh(&((*mDB).cccd0), gDB.cccd0, LEN_GR_D_X, C_D_VALUE);
    dot_graphRefresh(&((*mDB).cccd1), gDB.cccd1, LEN_GR_D_X, C_D_VALUE);
}

void dot_eGraph(uint16 xPos, const uint16 yPos) //----------------------------------------------------------- dot_eGraph
{
    // draw graphs
    xPos += 1u;
    dot_eGraphDraw(gDB.vcore, xPos, yPos);
    dot_eGraphDraw(gDB.acore, xPos, yPos +  LEN_GR_C_Y);
    dot_eGraphDraw(gDB.wcore, xPos, yPos + (LEN_GR_C_Y * 2u));

    // print current values; voltage is converted from mV to cV
    xPos += (LEN_GR_C_X - 3u);
    PRINTL(xPos, yPos +  LEN_GR_C_Y      , " %s%3u", CHK_LIM((*mDB).vcore.now, LIM_V, F_RED), (*mDB).vcore.now / 10u);
    PRINTL(xPos, yPos + (LEN_GR_C_Y * 2u), " %s%3u", CHK_LIM((*mDB).acore.now, LIM_A, F_RED), (*mDB).acore.now);
    PRINTL(xPos, yPos + (LEN_GR_C_Y * 3u), " %s%3u", CHK_LIM((*mDB).wcore.now, LIM_W, F_RED), (*mDB).wcore.now);
}

void dot_tGraph(uint16 xPos, const uint16 yPos) //----------------------------------------------------------- dot_tGraph
{
    // draw graphs
    xPos += 1;
    dot_tGraphDraw(gDB.cdie , xPos, yPos);
    dot_tGraphDraw(gDB.cctl , xPos, yPos +  LEN_GR_C_Y);
    dot_tGraphDraw(gDB.cccd0, xPos, yPos + (LEN_GR_C_Y * 2u));
    dot_tGraphDraw(gDB.cccd1, xPos, yPos + (LEN_GR_C_Y * 3u));

    // print current values
    xPos += LEN_GR_C_X - 2u;
    PRINTL(xPos, yPos +  LEN_GR_C_Y      , " %s%2u", CHK_LIM((*mDB).cdie.now , LIM_C, F_RED), (*mDB).cdie.now);
    PRINTL(xPos, yPos + (LEN_GR_C_Y * 2u), " %s%2u", CHK_LIM((*mDB).cctl.now , LIM_C, F_RED), (*mDB).cctl.now);
    PRINTL(xPos, yPos + (LEN_GR_C_Y * 3u), " %s%2u", CHK_LIM((*mDB).cccd0.now, LIM_C, F_RED), (*mDB).cccd0.now);
    PRINTL(xPos, yPos + (LEN_GR_C_Y * 4u), " %s%2u", CHK_LIM((*mDB).cccd1.now, LIM_C, F_RED), (*mDB).cccd1.now);
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
