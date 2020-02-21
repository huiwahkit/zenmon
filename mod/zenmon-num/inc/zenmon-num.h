// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_NUM_H
#define ZENMON_NUM_H

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// dependency includes
#include "gen-types.h"

// local includes
#include "zenmon-types.h"

//======================================================================================================================
// API
//======================================================================================================================
// management
void num_init(void);
void num_deinit(void);
void num_exportDB(metricsType** const outDB);

// getters
void num_getSvi2(void);
void num_getLoad(void);

// printers
void num_printSvi2(      uint16 xPos, const uint16 yPos);
void num_printLoad(const uint16 xPos, const uint16 yPos);

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
