// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUSIONS
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
static graphBufferType graphBuffer;
static metricsType*    metricsDB;

//======================================================================================================================
// FUNCTIONS
//======================================================================================================================
static void dot_drawGraph(metricType* const metric, uint8* const graph, const uint16 width, \
                                                                         const uint8 dot) //-------------- dot_drawGraph
{
    uint16 index;

    // advance the history graph
    for(index = 0u; index < (width - 1u); index++)
    {
        (*metric).hst[index] = (*metric).hst[index + 1u];
    }

    // set latest value
    (*metric).hst[width - 1u] = (*metric).now;

    // draw the new graph
    for(index = 0u; index < (width - 1u); index++)
    {
        braille_line(graph, index, (*metric).hst[index] / dot, index + 1u, (*metric).hst[index + 1u] / dot);
    }
}

static void dot_eGraph(uint8* const gBuff, const uint16 xPos, const uint16 yPos) //-------------------------- dot_eGraph
{
    uint16  xLen = 0u;
    sint16  yLen = 0;
     uint8* colour;

    /* print the graph while keeping in mind the colours based on the limits */
    for(yLen = LEN_GR_C_Y - 1; yLen >= 0; yLen--)
    {
        for(xLen = 0u; xLen < LEN_GR_C_X; xLen++)
        {
            if(yLen >= LEN_GR_C_Y - 1) // colour change is hardcoded based on LIM_V, LIM_A and LIM_W
            {
                colour = F_RED;
            }
            else
            {
                colour = F_GRN;
            }

            PRINTL(xPos + xLen + 1u, yPos + LEN_GR_C_Y - yLen, "%s%lc", colour, *(gBuff + (yLen * LEN_GR_C_X) + xLen) | BRAILLE_NULL);
        }
    }
}

static void dot_tGraph(uint8* const gBuff, const uint16 xPos, const uint16 yPos) //-------------------------- dot_tGraph
{
    uint16  xLen = 0u;
    sint16  yLen = 0;
     uint8* colour;

    /* print the graph while keeping in mind the colours based on the limits */
    for(yLen = LEN_GR_C_Y - 1; yLen >= 0; yLen--)
    {
        for(xLen = 0u; xLen < LEN_GR_C_X; xLen++)
        {
            if(yLen >= LEN_GR_C_Y - 2) // colour change is hardcoded based on LIM_T
            {
                colour = F_RED;
            }
            else
            {
                colour = F_CYN;
            }

            PRINTL(xPos + xLen + 1u, yPos + LEN_GR_C_Y - yLen, "%s%lc", colour, *(gBuff + (yLen * LEN_GR_C_X) + xLen) | BRAILLE_NULL);
        }
    }
}

//======================================================================================================================
// API
//======================================================================================================================
void dot_init() //--------------------------------------------------------------------------------------------- dot_init
{
    memset(&graphBuffer, 0u, sizeof(graphBuffer)); // clear braille buffers
    num_exportDB(&metricsDB);                      // import the numeric databases
}

void dot_updateGraphs(void) //------------------------------------------------------------------------- dot_updateGraphs
{
    // clear the previous screen state
    memset(&graphBuffer, 0u, sizeof(graphBuffer));

    // electrical
    dot_drawGraph(&((*metricsDB).vcpu), graphBuffer.vcpu, LEN_GR_D_X, V_D_VALUE);
    dot_drawGraph(&((*metricsDB).acpu), graphBuffer.acpu, LEN_GR_D_X, A_D_VALUE);
    dot_drawGraph(&((*metricsDB).wcpu), graphBuffer.wcpu, LEN_GR_D_X, W_D_VALUE);

    dot_drawGraph(&((*metricsDB).vsoc), graphBuffer.vsoc, LEN_GR_D_X, V_D_VALUE);
    dot_drawGraph(&((*metricsDB).asoc), graphBuffer.asoc, LEN_GR_D_X, A_D_VALUE);
    dot_drawGraph(&((*metricsDB).wsoc), graphBuffer.wsoc, LEN_GR_D_X, W_D_VALUE);

    // temperatur
    dot_drawGraph(&((*metricsDB).tdie), graphBuffer.tcpu, LEN_GR_D_X, T_D_VALUE);
    dot_drawGraph(&((*metricsDB).tctl), graphBuffer.tctl, LEN_GR_D_X, T_D_VALUE);
    dot_drawGraph(&((*metricsDB).tcd1), graphBuffer.tcd1, LEN_GR_D_X, T_D_VALUE);
    dot_drawGraph(&((*metricsDB).tcd2), graphBuffer.tcd2, LEN_GR_D_X, T_D_VALUE);
}

void dot_eGraphWin(const uint16 xPos, const uint16 yPos) //----------------------------------------------- dot_eGraphWin
{
    // draw graphs
    dot_eGraph(graphBuffer.vcpu, xPos + 1u, yPos                    );
    dot_eGraph(graphBuffer.acpu, xPos + 1u, yPos +  LEN_GR_C_Y      );
    dot_eGraph(graphBuffer.wcpu, xPos + 1u, yPos + (LEN_GR_C_Y * 2u));

    // print graph labels
    PRINTL(xPos + 2u, yPos +  LEN_GR_C_Y      , "%sV %d:%s%u ", F_RST, V_D_VALUE, F_RED, LIM_V);
    PRINTL(xPos + 2u, yPos + (LEN_GR_C_Y * 2u), "%sA %d:%s%u ", F_RST, A_D_VALUE, F_RED, LIM_A);
    PRINTL(xPos + 2u, yPos + (LEN_GR_C_Y * 3u), "%sW %d:%s%u ", F_RST, W_D_VALUE, F_RED, LIM_W);
}

void dot_tGraphWin(const uint16 xPos, const uint16 yPos) //----------------------------------------------- dot_tGraphWin
{
    // draw graphs
    dot_tGraph(graphBuffer.tcpu, xPos + 1u, yPos                    );
    dot_tGraph(graphBuffer.tctl, xPos + 1u, yPos +  LEN_GR_C_Y      );
    dot_tGraph(graphBuffer.tcd1, xPos + 1u, yPos + (LEN_GR_C_Y * 2u));
    dot_tGraph(graphBuffer.tcd2, xPos + 1u, yPos + (LEN_GR_C_Y * 3u));

    // print graph labels
    PRINTL(xPos + 2u, yPos +  LEN_GR_C_Y      , "%sTCPU %d:%s%u ", F_RST, T_D_VALUE, F_RED, LIM_T);
    PRINTL(xPos + 2u, yPos + (LEN_GR_C_Y * 2u), "%sTCTL %d:%s%u ", F_RST, T_D_VALUE, F_RED, LIM_T);
    PRINTL(xPos + 2u, yPos + (LEN_GR_C_Y * 3u), "%sTCD1 %d:%s%u ", F_RST, T_D_VALUE, F_RED, LIM_T);
    PRINTL(xPos + 2u, yPos + (LEN_GR_C_Y * 4u), "%sTCD2 %d:%s%u ", F_RST, T_D_VALUE, F_RED, LIM_T);
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
