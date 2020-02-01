// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_NUM_H
#define ZENMON_NUM_H

//======================================================================================================================
// INCLUSIONS
//======================================================================================================================
// dependency includes
#include "gen-types.h"

// local includes
#include "zenmon-types.h"

//======================================================================================================================
// API
//======================================================================================================================
void num_init(void);
void num_deinit(void);
void num_statusWin(const uint16 xPos, const uint16 yPos);
void num_exportDB(metricsType** const outputDB);

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
