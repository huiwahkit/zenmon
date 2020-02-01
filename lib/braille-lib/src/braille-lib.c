// Copyright (C) 2020 Denis Isai

// The Braille character handling from braille-lib.h and braille-lib.c have been adapted from github.com/dcat/tplot

//======================================================================================================================
// INCLUSIONS
//======================================================================================================================
// system includes
#include <stdlib.h>

// local includes
#include "braille-lib.h"

//======================================================================================================================
// DEFINES
//======================================================================================================================
#define UNTIL_BREAK 1

//======================================================================================================================
// VARIABLES
//======================================================================================================================
uint8 screenWidth;

//======================================================================================================================
// API
//======================================================================================================================
void braille_init(const uint8 width) //-------------------------------------------------------------------- braille_init
{
    screenWidth = width;
}

void braille_dot(uint8* const gBuff, const uint16 xPos, const uint16 yPos) //------------------------------- braille_dot
{
    uint16 xChar = xPos / 2u;
    uint16 yChar = yPos / 4u;

    // flip Braille dot
    switch(yPos % 4u)
    {
        case 3u: // A B
        {
            *(gBuff + (yChar * screenWidth) + xChar) |= (xPos & 1) ? FIELD_B : FIELD_A;
            break;
        }
        case 2u: // C D
        {
            *(gBuff + (yChar * screenWidth) + xChar) |= (xPos & 1) ? FIELD_D : FIELD_C;
            break;
        }
        case 1u: // E F
        {
            *(gBuff + (yChar * screenWidth) + xChar) |= (xPos & 1) ? FIELD_F : FIELD_E;
            break;
        }
        case 0u: // G H
        {
            *(gBuff + (yChar * screenWidth) + xChar) |= (xPos & 1) ? FIELD_H : FIELD_G;
            break;
        }
    }
}

void braille_line(uint8* const gBuff, uint16 xPos1, uint16 yPos1, const uint16 xPos2, const uint16 yPos2) //braille_line
{
    uint16 xDelta = abs(xPos2 - xPos1);
    uint16 yDelta = abs(yPos2 - yPos1);
     sint8 xDir   = (xPos1 < xPos2) ? 1 : -1;
     sint8 yDir   = (yPos1 < yPos2) ? 1 : -1;
    sint16 eDelta = ((xDelta > yDelta) ? xDelta : -yDelta) / 2;
    sint16 err    = 0;

    while(UNTIL_BREAK)
    {
        braille_dot(gBuff, xPos1, yPos1);

        if((xPos1 == xPos2) && (yPos1 == yPos2))
        {
            break;
        }

        err = eDelta;
        if(err > -xDelta)
        {
            eDelta -= yDelta;
            xPos1  += xDir;
        }

        if(err < yDelta)
        {
            eDelta += xDelta;
            yPos1  += yDir;
        }
    }
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
