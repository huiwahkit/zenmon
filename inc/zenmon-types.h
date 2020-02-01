// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_TYPES_H
#define ZENMON_TYPES_H

//======================================================================================================================
// INCLUSIONS
//======================================================================================================================
// dependency includes
#include "gen-types.h"

// local includes
#include "zenmon-cfg.h"

//======================================================================================================================
// TYPES
//======================================================================================================================
typedef struct metricTypeTag // stores data related to a single metric
{
      FILE* FD;
    uint32  min;
    uint32  now;
    uint32  max;
    uint32  avg;
    uint16  hst[LEN_GR_D_X];
} metricType;

typedef struct metricsTypeTag // stores all metrics
{
    metricType vcpu; // milivolts
    metricType acpu; //     amperes
    metricType wcpu; //     watts
    metricType vsoc; // milivolts
    metricType asoc; //     amperes
    metricType wsoc; //     watts
    metricType tdie; //    °Celsius
    metricType tctl; //    °Celsius
    metricType tcd1; //    °Celsius
    metricType tcd2; //    °Celsius
} metricsType;

typedef struct graphBufferTypeTag // stores the graph screen buffers
{
    uint8 vcpu[LEN_GR_BUF]; // milivolts
    uint8 acpu[LEN_GR_BUF]; //     amperes
    uint8 wcpu[LEN_GR_BUF]; //     watts
    uint8 vsoc[LEN_GR_BUF]; // milivolts
    uint8 asoc[LEN_GR_BUF]; //     amperes
    uint8 wsoc[LEN_GR_BUF]; //     watts
    uint8 tcpu[LEN_GR_BUF]; //    °Celsius
    uint8 tctl[LEN_GR_BUF]; //    °Celsius
    uint8 tcd1[LEN_GR_BUF]; //    °Celsius
    uint8 tcd2[LEN_GR_BUF]; //    °Celsius
} graphBufferType;

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
