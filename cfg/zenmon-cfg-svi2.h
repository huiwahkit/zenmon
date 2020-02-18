// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_CFG_SVI2_H
#define ZENMON_CFG_SVI2_H

//======================================================================================================================
// DEFINES
//======================================================================================================================
// file paths; these are checked at runtime and if any are missing the program will notify and exit
#define FILE_VCORE "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/in1_input"    // core voltage
#define FILE_ACORE "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/curr1_input"  // core current
#define FILE_WCORE "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/power1_input" // core power
#define FILE_VSOC  "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/in2_input"    // soc  voltage
#define FILE_ASOC  "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/curr2_input"  // soc  current
#define FILE_WSOC  "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/power2_input" // soc  power
#define FILE_CDIE  "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/temp1_input"  // core temperature
#define FILE_CCTL  "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/temp2_input"  // ctl  temperature
#define FILE_CCCD0 "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/temp3_input"  // ccd0 temperature
#define FILE_CCCD1 "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/temp4_input"  // ccd1 temperature

// svi2 window size
#define LEN_SVI2_W_X 47u   // window X size in characters
#define LEN_SVI2_W_Y 14u   // window Y size in characters

// static config
#define AVG_CYCLES   10u

// graph scales and alarm thresholds; these numbers depend of the definitions of graph sizes from zenmon-cfg.h
#define C_D_VALUE    5u    // each Braille dot represents 5 °C; 0 - 100 scale
#define LIM_C        60u   // °C; multiple of (C_D_VALUE * LEN_GR_D_Y)

#define V_D_VALUE    75u   // each Braille dot represents 75 milivolts; 0 - 1500 scale
#define LIM_V        1200u // milivolts; multiple of (V_D_VALUE * LEN_GR_D_Y)

#define A_D_VALUE    8u    // each Braille dot represents 8 amperes; 0 - 160 scale
#define LIM_A        128   // amperes; multiple of (A_D_VALUE * LEN_GR_D_Y)

#define W_D_VALUE    10u   // each Braille dot represents 10 watts; 0 - 200 scale
#define LIM_W        160u  // watts; multiple of (W_D_VALUE * LEN_GR_D_Y)

//======================================================================================================================
// END OF FILE
//======================================================================================================================

#endif
