// Copyright (C) 2020 Denis Isai

//======================================================================================================================
// INCLUDES
//======================================================================================================================
// system includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// dependency includes
#include "logging.h"

// local includes
#include "zenmon-cfg.h"
#include "zenmon-num-svi2.h"
#include "zenmon-num-load.h"
#include "zenmon-num.h"

//======================================================================================================================
// VARIABLES
//======================================================================================================================
static metricsType mDB; // svi2 metrics database
static sysLoadType sDB; // system usage database

//======================================================================================================================
// FUNCTIONS
//======================================================================================================================
static bool num_openFile(FILE** FD, const sint8* const name)
{
    if(-1 != access(name, F_OK))
    {
        *FD = fopen(name, "rb");
        return FALSE;
    }
    else
    {
        PRINT_FAIL("File %s%s%s not found", F_RED, name, F_RST);
        return TRUE;
    }
}

//======================================================================================================================
// API
//======================================================================================================================
void num_init(void) //----------------------------------------------------------------------------------------- num_init
{
    bool failure = FALSE;

    // open file descriptors
    failure |= num_openFile(&(mDB.vcore.FD), FILE_VCORE);
    failure |= num_openFile(&(mDB.acore.FD), FILE_ACORE);
    failure |= num_openFile(&(mDB.wcore.FD), FILE_WCORE);

    failure |= num_openFile(&(mDB.vsoc.FD) , FILE_VSOC);
    failure |= num_openFile(&(mDB.asoc.FD) , FILE_ASOC);
    failure |= num_openFile(&(mDB.wsoc.FD) , FILE_WSOC);

    failure |= num_openFile(&(mDB.cdie.FD) , FILE_CDIE);
    failure |= num_openFile(&(mDB.cctl.FD) , FILE_CCTL);
    failure |= num_openFile(&(mDB.cccd0.FD), FILE_CCCD0);
    failure |= num_openFile(&(mDB.cccd1.FD), FILE_CCCD1);

    failure |= num_openFile(&(sDB.mhzFD)   , FILE_MHZ);
    failure |= num_openFile(&(sDB.usgFD)   , FILE_USG);
    failure |= num_openFile(&(sDB.memFD)   , FILE_MEM);

    if(FALSE != failure) // if any files don't exist then exit program
    {
        num_deinit();
        CURSOR_SHOW;
        PRINT_FAIL("Required files not found; exiting...");
        exit(EXIT_FAILURE);
    }

    // reset min value
    mDB.vcore.min = UINT32_MAX;
    mDB.acore.min = UINT32_MAX;
    mDB.wcore.min = UINT32_MAX;

    mDB.vsoc.min  = UINT32_MAX;
    mDB.asoc.min  = UINT32_MAX;
    mDB.wsoc.min  = UINT32_MAX;

    mDB.cdie.min  = UINT32_MAX;
    mDB.cctl.min  = UINT32_MAX;
    mDB.cccd0.min = UINT32_MAX;
    mDB.cccd1.min = UINT32_MAX;

    // point the sub-modules to the databases
    svi2_init(&mDB);
    load_init(&sDB);
}

void num_deinit(void) //------------------------------------------------------------------------------------- num_deinit
{
    // close file descriptors; technically the OS does this by itself, but it doesn't hurt doing it here
    if(NULL != mDB.vcore.FD) fclose(mDB.vcore.FD);
    if(NULL != mDB.acore.FD) fclose(mDB.acore.FD);
    if(NULL != mDB.wcore.FD) fclose(mDB.wcore.FD);

    if(NULL != mDB.vsoc.FD ) fclose(mDB.vsoc.FD);
    if(NULL != mDB.asoc.FD ) fclose(mDB.asoc.FD);
    if(NULL != mDB.wsoc.FD ) fclose(mDB.wsoc.FD);

    if(NULL != mDB.cdie.FD ) fclose(mDB.cdie.FD);
    if(NULL != mDB.cctl.FD ) fclose(mDB.cctl.FD);
    if(NULL != mDB.cccd0.FD) fclose(mDB.cccd0.FD);
    if(NULL != mDB.cccd1.FD) fclose(mDB.cccd1.FD);

    if(NULL != sDB.mhzFD   ) fclose(sDB.mhzFD);
    if(NULL != sDB.usgFD   ) fclose(sDB.usgFD);
    if(NULL != sDB.memFD   ) fclose(sDB.memFD);
}

void num_svi2(const uint16 xPos, const uint16 yPos) //--------------------------------------------------------- num_svi2
{
    // fill the window elements
    svi2_getStatus();
    svi2_setMinMax();
    svi2_setAvg();
    svi2_printMetrics(xPos, yPos + 2u);
}

void num_load(const uint16 xPos, const uint16 yPos) //--------------------------------------------------------- num_load
{
    // fill the window elements
    load_sysInfo(xPos + 2u, yPos +  1u);
    load_memBar( xPos + 2u, yPos +  7u);
    load_cpuBar( xPos + 2u, yPos + 11u);
}

void num_exportDB(metricsType** const outDB) //------------------------------------------------------------ num_exportDB
{
    *outDB = &mDB; // let the dot drawing functions access the metrics database
}

//======================================================================================================================
// END OF FILE
//======================================================================================================================
