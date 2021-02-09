#ifndef _CONFIG_H

#define LILYGO_WATCH_2020_V1 //To use T-Watch2020, please uncomment this line
#define LILYGO_WATCH_LVGL    //To use LVGL, you need to enable the macro LVGL
#define TWATCH_USE_PSRAM_ALLOC_LVGL

/*
* Enable non-latin languages support:
*/
#define USE_EXTENDED_CHARSET CHARSET_CYRILLIC

/*
    * firmware version string
    */
#define __FIRMWARE__ "2021012701"

#ifdef __cplusplus // Allows to include config.h from C code
#include <LilyGoWatch.h>
#define _CONFIG_H
#endif

#endif // _CONFIG_H
