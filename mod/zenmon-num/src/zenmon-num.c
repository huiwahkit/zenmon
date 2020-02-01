// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUSIONS
//======================================================================================================================
// system includes
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

// local includes
#include "zenmon-cfg.h"
#include "zenmon-num.h"

//======================================================================================================================
// DEFINES
//======================================================================================================================
#define CHECK_LIMIT(value, threshold, color) ((value >= threshold) ? color : F_RST)

//======================================================================================================================
// VARIABLES
//======================================================================================================================
static metricsType mDB;

//======================================================================================================================
// FUNCTIONS
//======================================================================================================================
static void num_printMetrics(const uint16 xPos, const uint16 yPos) //---------------------------------- num_printMetrics
{
    uint16 yIndex = yPos;

    // minimum values; these are the only ones without warning colours
    PRINTL(xPos + 11, yIndex++, "%s%4d", F_RST, mDB.vcpu.min);
    PRINTL(xPos + 12, yIndex++, "%s%3d", F_RST, mDB.acpu.min);
    PRINTL(xPos + 12, yIndex++, "%s%3d", F_RST, mDB.wcpu.min);

    PRINTL(xPos + 11, yIndex++, "%s%4d", F_RST, mDB.vsoc.min);
    PRINTL(xPos + 12, yIndex++, "%s%3d", F_RST, mDB.asoc.min);
    PRINTL(xPos + 12, yIndex++, "%s%3d", F_RST, mDB.wsoc.min);

    yIndex++;
    PRINTL(xPos + 12, yIndex++, "%s%3d", F_RST, mDB.tdie.min);
    PRINTL(xPos + 12, yIndex++, "%s%3d", F_RST, mDB.tctl.min);
    PRINTL(xPos + 12, yIndex++, "%s%3d", F_RST, mDB.tcd1.min);
    PRINTL(xPos + 12, yIndex++, "%s%3d", F_RST, mDB.tcd2.min);

    // current values
    yIndex = yPos;
    PRINTL(xPos + 20, yIndex++, "%s%s%4d", F_RST, CHECK_LIMIT(mDB.vcpu.now, LIM_V, F_RED), mDB.vcpu.now);
    PRINTL(xPos + 21, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.acpu.now, LIM_A, F_RED), mDB.acpu.now);
    PRINTL(xPos + 21, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.wcpu.now, LIM_W, F_RED), mDB.wcpu.now);

    PRINTL(xPos + 20, yIndex++, "%s%s%4d", F_RST, CHECK_LIMIT(mDB.vsoc.now, LIM_V, F_RED), mDB.vsoc.now);
    PRINTL(xPos + 21, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.asoc.now, LIM_A, F_RED), mDB.asoc.now);
    PRINTL(xPos + 21, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.wsoc.now, LIM_W, F_RED), mDB.wsoc.now);

    yIndex++;
    PRINTL(xPos + 21, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tdie.now, LIM_T, F_RED), mDB.tdie.now);
    PRINTL(xPos + 21, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tctl.now, LIM_T, F_RED), mDB.tctl.now);
    PRINTL(xPos + 21, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tcd1.now, LIM_T, F_RED), mDB.tcd1.now);
    PRINTL(xPos + 21, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tcd2.now, LIM_T, F_RED), mDB.tcd2.now);

    // maximum values
    yIndex = yPos;
    PRINTL(xPos + 29, yIndex++, "%s%s%4d", F_RST, CHECK_LIMIT(mDB.vcpu.max, LIM_V, F_YEL), mDB.vcpu.max);
    PRINTL(xPos + 30, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.acpu.max, LIM_A, F_YEL), mDB.acpu.max);
    PRINTL(xPos + 30, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.wcpu.max, LIM_W, F_YEL), mDB.wcpu.max);

    PRINTL(xPos + 29, yIndex++, "%s%s%4d", F_RST, CHECK_LIMIT(mDB.vsoc.max, LIM_V, F_YEL), mDB.vsoc.max);
    PRINTL(xPos + 30, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.asoc.max, LIM_A, F_YEL), mDB.asoc.max);
    PRINTL(xPos + 30, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.wsoc.max, LIM_W, F_YEL), mDB.wsoc.max);

    yIndex++;
    PRINTL(xPos + 30, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tdie.max, LIM_T, F_YEL), mDB.tdie.max);
    PRINTL(xPos + 30, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tctl.max, LIM_T, F_YEL), mDB.tctl.max);
    PRINTL(xPos + 30, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tcd1.max, LIM_T, F_YEL), mDB.tcd1.max);
    PRINTL(xPos + 30, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tcd2.max, LIM_T, F_YEL), mDB.tcd2.max);

    // average values
    yIndex = yPos;
    PRINTL(xPos + 38, yIndex++, "%s%s%4d", F_RST, CHECK_LIMIT(mDB.vcpu.avg, LIM_V, F_RED), mDB.vcpu.avg);
    PRINTL(xPos + 39, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.acpu.avg, LIM_A, F_RED), mDB.acpu.avg);
    PRINTL(xPos + 39, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.wcpu.avg, LIM_W, F_RED), mDB.wcpu.avg);

    PRINTL(xPos + 38, yIndex++, "%s%s%4d", F_RST, CHECK_LIMIT(mDB.vsoc.avg, LIM_V, F_RED), mDB.vsoc.avg);
    PRINTL(xPos + 39, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.asoc.avg, LIM_A, F_RED), mDB.asoc.avg);
    PRINTL(xPos + 39, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.wsoc.avg, LIM_W, F_RED), mDB.wsoc.avg);

    yIndex++;
    PRINTL(xPos + 39, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tdie.avg, LIM_T, F_RED), mDB.tdie.avg);
    PRINTL(xPos + 39, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tctl.avg, LIM_T, F_RED), mDB.tctl.avg);
    PRINTL(xPos + 39, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tcd1.avg, LIM_T, F_RED), mDB.tcd1.avg);
    PRINTL(xPos + 39, yIndex++, "%s%s%3d", F_RST, CHECK_LIMIT(mDB.tcd2.avg, LIM_T, F_RED), mDB.tcd2.avg);
}

static sint32 num_readFile(FILE* const file) //------------------------------------------------------------ num_readFile
{
    sint8  fileString[10] = {0};
    uint32 length;

    fseek(file, 0, SEEK_END);
    length = ftell (file);
    fseek(file, 0, SEEK_SET);
    fread(fileString, 1, length, file);

    return atoi(fileString);
}

static void num_getStatus(void) //------------------------------------------------------------------------ num_getStatus
{
    // electrical
    mDB.vcpu.now = num_readFile(mDB.vcpu.FD);                           // milivolts
    mDB.acpu.now = num_readFile(mDB.acpu.FD); mDB.acpu.now /= 1000u;    //     amperes
    mDB.wcpu.now = num_readFile(mDB.wcpu.FD); mDB.wcpu.now /= 1000000u; //     watts

    mDB.vsoc.now = num_readFile(mDB.vsoc.FD);                           // milivolts
    mDB.asoc.now = num_readFile(mDB.asoc.FD); mDB.asoc.now /= 1000u;    //     amperes
    mDB.wsoc.now = num_readFile(mDB.wsoc.FD); mDB.wsoc.now /= 1000000u; //     watts

    // temperature
    mDB.tdie.now = num_readFile(mDB.tdie.FD); mDB.tdie.now /= 1000u;    //    °Celsius
    mDB.tctl.now = num_readFile(mDB.tctl.FD); mDB.tctl.now /= 1000u;    //    °Celsius
    mDB.tcd1.now = num_readFile(mDB.tcd1.FD); mDB.tcd1.now /= 1000u;    //    °Celsius
    mDB.tcd2.now = num_readFile(mDB.tcd2.FD); mDB.tcd2.now /= 1000u;    //    °Celsius
}

static void num_metricMinMax(metricType* const type) //------------------------------------------------ num_metricMinMax
{
    if((*type).min > (*type).now)
    {
        (*type).min = (*type).now;
    }
    else
    {
        // value is higher than min
    }

    if((*type).now > (*type).max)
    {
        (*type).max = (*type).now;
    }
    else
    {
        // value is lower than max
    }
}

static void num_setMinMax(void) //------------------------------------------------------------------------ num_setMinMax
{
    // electrical
    num_metricMinMax(&(mDB.vcpu)); // milivolts
    num_metricMinMax(&(mDB.acpu)); //     amperes
    num_metricMinMax(&(mDB.wcpu)); //     watts

    num_metricMinMax(&(mDB.vsoc)); // milivolts
    num_metricMinMax(&(mDB.asoc)); //     amperes
    num_metricMinMax(&(mDB.wsoc)); //     watts

    // temperature
    num_metricMinMax(&(mDB.tdie)); //    °Celsius
    num_metricMinMax(&(mDB.tctl)); //    °Celsius
    num_metricMinMax(&(mDB.tcd1)); //    °Celsius
    num_metricMinMax(&(mDB.tcd2)); //    °Celsius
}
static void num_metricAvg(metricType* const metric) //---------------------------------------------------- num_metricAvg
{
    uint8 index;

    (*metric).avg = 0;

    for(index = LEN_GR_D_X - AVG_CYCLES; index < LEN_GR_D_X; index++)
    {
        (*metric).avg += (*metric).hst[index];
    }

    (*metric).avg /= AVG_CYCLES;
}

static void num_setAvg(void) //------------------------------------------------------------------------------ num_setAvg
{
    static uint8 waitCycles = 1u;

    if(AVG_CYCLES < waitCycles) // wait 10 cycles
    {
        // electrical
        num_metricAvg(&(mDB.vcpu)); // milivolts
        num_metricAvg(&(mDB.acpu)); //     amperes
        num_metricAvg(&(mDB.wcpu)); //     watts

        num_metricAvg(&(mDB.vsoc)); // milivolts
        num_metricAvg(&(mDB.asoc)); //     amperes
        num_metricAvg(&(mDB.wsoc)); //     watts

        // temperature
        num_metricAvg(&(mDB.tdie)); //    °Celsius
        num_metricAvg(&(mDB.tctl)); //    °Celsius
        num_metricAvg(&(mDB.tcd1)); //    °Celsius
        num_metricAvg(&(mDB.tcd2)); //    °Celsius
    }
    else
    {
        waitCycles++;
    }
}

//======================================================================================================================
// API
//======================================================================================================================
void num_init(void) //----------------------------------------------------------------------------------------- num_init
{
    // open file descriptors
    mDB.vcpu.FD = fopen(FILE_VCPU, "rb");
    mDB.acpu.FD = fopen(FILE_ACPU, "rb");
    mDB.wcpu.FD = fopen(FILE_WCPU, "rb");

    mDB.vsoc.FD = fopen(FILE_VSOC, "rb");
    mDB.asoc.FD = fopen(FILE_ASOC, "rb");
    mDB.wsoc.FD = fopen(FILE_WSOC, "rb");

    mDB.tdie.FD = fopen(FILE_TDIE, "rb");
    mDB.tctl.FD = fopen(FILE_TCTL, "rb");
    mDB.tcd1.FD = fopen(FILE_TCD1, "rb");
    mDB.tcd2.FD = fopen(FILE_TCD2, "rb");

    // reset min value
    mDB.vcpu.min = UINT32_MAX;
    mDB.acpu.min = UINT32_MAX;
    mDB.wcpu.min = UINT32_MAX;

    mDB.vsoc.min = UINT32_MAX;
    mDB.asoc.min = UINT32_MAX;
    mDB.wsoc.min = UINT32_MAX;

    mDB.tdie.min = UINT32_MAX;
    mDB.tctl.min = UINT32_MAX;
    mDB.tcd1.min = UINT32_MAX;
    mDB.tcd2.min = UINT32_MAX;
}

void num_deinit(void) //------------------------------------------------------------------------------------- num_deinit
{
    // close file descriptors; technically the OS does this by itself, but it doesn't hurt doing it here
    fclose(mDB.vcpu.FD);
    fclose(mDB.acpu.FD);
    fclose(mDB.wcpu.FD);

    fclose(mDB.vsoc.FD);
    fclose(mDB.asoc.FD);
    fclose(mDB.wsoc.FD);

    fclose(mDB.tdie.FD);
    fclose(mDB.tctl.FD);
    fclose(mDB.tcd1.FD);
    fclose(mDB.tcd2.FD);
}

void num_statusWin(const uint16 xPos, const uint16 yPos) //------------------------------------------------ dot_eStatWin
{
    num_getStatus();
    num_setMinMax();
    num_setAvg();
    num_printMetrics(xPos, yPos + 2u);
}

void num_exportDB(metricsType** const outputDB) //--------------------------------------------------------- num_exportDB
{
    // let the dot drawing functions access the metrics database
    *outputDB = &mDB;
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
