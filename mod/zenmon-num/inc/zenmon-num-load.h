// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_NUM_LOAD_H
#define ZENMON_NUM_LOAD_H

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// system inclues
#include <sys/sysinfo.h>

// dependency includes
#include "gen-types.h"

// local includes
#include "zenmon-types.h"

//======================================================================================================================
// API
//======================================================================================================================
// management
void load_init(sysLoadType* const inDB);

// getters; sysInfo and RAM do not need getters because their data does not depend on the previous polling cycle
void load_getCpuBar(void);

// printers
void load_printSysInfo(const uint16 xPos, const uint16 yPos);
void load_printRamBar( const uint16 xPos, const uint16 yPos);
void load_printCpuBar( const uint16 xPos, const uint16 yPos);

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
