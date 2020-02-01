// Copyright (C) 2020 Denis Isai

#ifndef ZENMON_CFG_H
#define ZENMON_CFG_H

//======================================================================================================================
// DEFINES
//======================================================================================================================
#define AVG_CYCLES       10u

// status window
#define LEN_STATUS_W_X   47u                         // window X size in characters
#define LEN_STATUS_W_Y   14u                         // window Y size in characters

// graphs
#define LEN_GR_D_X       120u                        // graph X size in Braille dots; multiple of 2
#define LEN_GR_D_Y       20u                         // graph Y size in Braille dots; multiple of 4
#define LEN_GR_C_X       (LEN_GR_D_X / 2u)           // graph X size in characters
#define LEN_GR_C_Y       (LEN_GR_D_Y / 4u)           // graph Y size in characters
#define LEN_GR_BUF       (LEN_GR_C_X * LEN_GR_C_Y)

// electrical windows; 3 stacked graphs
#define LEN_EG_W_X       (LEN_GR_D_X / 2u     ) + 4u // window X size in characters
#define LEN_EG_W_Y       (LEN_GR_D_Y / 4u * 3u) + 2u // window Y size in characters

// temperature windows; 4 stacked graphs
#define LEN_TG_W_X       (LEN_GR_D_X / 2u     ) + 4u // window X size in characters
#define LEN_TG_W_Y       (LEN_GR_D_Y / 4u * 4u) + 2u // window Y size in characters


// value thresholds; the scales depend of the previous definitions of graph sizes
#define T_D_VALUE        5u
#define LIM_T            60u                         // °C; multiple of (T_D_VALUE * LEN_GR_D_Y)
#define LIM_T_PIX        (LIM_T / T_D_VALUE)         // each Braille dot represents 5 °C; 0 - 100 scale

#define V_D_VALUE        75u
#define LIM_V            1200u                       // milivolts; multiple of (V_D_VALUE * LEN_GR_D_Y)
#define LIM_V_PIX        (LIM_V / V_D_VALUE)         // each Braille dot represents 75 milivolts; 0 - 1500 scale

#define A_D_VALUE        8u
#define LIM_A            128                         // amperes; multiple of (A_D_VALUE * LEN_GR_D_Y)
#define LIM_A_PIX        (LIM_A / A_D_VALUE)         // each Braille dot represents 8 amperes; 0 - 160 scale

#define W_D_VALUE        10u
#define LIM_W            160u                        // watts; multiple of (W_D_VALUE * LEN_GR_D_Y)
#define LIM_W_PIX        (LIM_W / W_D_VALUE)         // each Braille dot represents 10 watts; 0 - 200 scale

// file paths
#define FILE_VCPU "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/in1_input"
#define FILE_ACPU "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/curr1_input"
#define FILE_WCPU "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/power1_input"
#define FILE_VSOC "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/in2_input"
#define FILE_ASOC "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/curr2_input"
#define FILE_WSOC "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/power2_input"
#define FILE_TDIE "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/temp1_input"
#define FILE_TCTL "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/temp2_input"
#define FILE_TCD1 "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/temp3_input"
#define FILE_TCD2 "/sys/devices/pci0000:00/0000:00:18.3/hwmon/hwmon0/temp4_input"

#endif

//======================================================================================================================
// END OF FILE
//======================================================================================================================
