// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_BOX_H
#define ZENMON_BOX_H

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// dependency includes
#include "gen-types.h"

//======================================================================================================================
// API
//======================================================================================================================
void box_svi2(  const sint8* title,       uint16 xPos,       uint16 yPos, const sint8* colour);
void box_load(  const sint8* title, const uint16 xPos, const uint16 yPos, const sint8* colour);
void box_eGraph(const sint8* title, const uint16 xPos, const uint16 yPos, const sint8* colour);
void box_tGraph(const sint8* title, const uint16 xPos, const uint16 yPos, const sint8* colour);

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
