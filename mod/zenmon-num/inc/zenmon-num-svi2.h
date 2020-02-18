// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_NUM_SVI2_H
#define ZENMON_NUM_SVI2_H

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// local includes
#include "zenmon-types.h"

//======================================================================================================================
// API
//======================================================================================================================
void svi2_init(metricsType* const inDB);
void svi2_getStatus(void);
void svi2_setMinMax(void);
void svi2_setAvg(void);
void svi2_printMetrics(uint16 xPos, const uint16 yPos);

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
