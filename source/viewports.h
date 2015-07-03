/*
Copyright 2015 - Zandy Who

This file is part of the 3D game Piggy Engine

The Game Piggy should run on 3DS or run on the 3DS Emulator on PC.
More details on README.md

Develop Environment:Mac OS
--However,we recommend you to install the citra 3ds emulator in a Windows environment.

Dependencies:devkitPro,ctrulib
--Download ==> http://sourceforge.net/projects/devkitpro/files/Automated%20Installer/
--Just $perl devkitARMupdater.pl
--This is all what you should do.

Citra 3DS Emulator
--See https://github.com/xem/3DShomebrew/wiki
*/

/*
** Name:Piggy
** Desc:Portable Gameboy/NintendoDS/Nintendo3DS Advanced 3D engine
** Auth:Zandy Who
**
** Copyright 2015 - Zandy Who.All Rights Reserved.
**
** PPPPP  IIIIII  GGGG   GGGG  YY  YY
** PP  PP   II   GG     GG     YY  YY
** PPPPP    II   GG   G GG   G  YYYY
** PP       II   GG  GG GG  GG   YY
** PP     IIIIII  GGG G  GGG G   YY
** 
*/

#ifndef __VIEWPORTS_H__
#define __VIEWPORTS_H__

#define __3DS__

/******************************************************************************/

/*
** Some Porting Notes:
**
** - The standard pixel format is 5:5:5. There is a function in piggy.c which
**   will copy the viewport to a video display that supports a different pixel
**   format.
** - Define PIGGY_ROM as const for all ROM based ports.
** - You can patch functions by using the __PATCH?????__ macros.
** - Make sure you define the platform macro in your IDE/make scripts.
** - Viewports larger than 320x240 are not recommended unless you patch the
**   default texture mapper. The standard DDA code is limited to small displays.
**   Look at the OpenGL port for how todo this.
**
*/

/******************************************************************************/

#ifdef __GBA__

#define PIGGY_RGB555
#define PIGGY_LOW_QUALITY_WALLS

#define PIGGY_ROM const

#define PIGGY_VIEWPORT_INTERVAL 14
#define PIGGY_VIEWPORT_INTERVAL_ERROR 7

#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 160
#define PIGGY_VIEWPORT_Y2 128
#define PIGGY_VIEWPORT_WIDTH 160
#define PIGGY_VIEWPORT_HEIGHT 128

//#define __PATCH_DRAW_POLY__

#define CODE_IN_IWRAM __attribute__ ((section (".iwram"), long_call))
#define CODE_IN_ROM __attribute__ ((section (".text"), long_call))
#define IN_IWRAM __attribute__ ((section (".iwram")))
#define IN_EWRAM __attribute__ ((section (".ewram")))

#endif

/******************************************************************************/

#ifdef __SERIES60__

#define PIGGY_RGB444
#define PIGGY_LOW_QUALITY_WALLS

#define PIGGY_ROM const

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 320
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __GP32__

#define PIGGY_RGB565
#define PIGGY_LOW_QUALITY_WALLS

#define PIGGY_ROM

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 320
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __3DS__

#define PIGGY_RGB555
// #define PIGGY_LOW_QUALITY_WALLS

#define PIGGY_ROM

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 400
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 400
#define PIGGY_VIEWPORT_HEIGHT 240

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __POCKETPC__

#define PIGGY_RGB565
#define PIGGY_LOW_QUALITY_WALLS
#define PIGGY_ROM

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 320
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __SVGALIB__

#define PIGGY_RGB555
#define PIGGY_ROM

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 320
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __GAMECUBE__

#define PIGGY_RGB565
#define PIGGY_ROM

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 320
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __WIN32__

#define PIGGY_RGB555
#define PIGGY_ROM 

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#ifdef __PIGGY_EDITOR__

#define PIGGY_VIEWPORT_X2 320
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240

#else

#define PIGGY_VIEWPORT_X2 640
#define PIGGY_VIEWPORT_Y2 480
#define PIGGY_VIEWPORT_WIDTH 640
#define PIGGY_VIEWPORT_HEIGHT 480

#endif

#define PIGGY_VIDEO_LUT
#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __MSDOS__

#define PIGGY_RGB555
#define PIGGY_ROM

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 320
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __PSONE__

#define PIGGY_RGB555
#define PIGGY_ROM

#define PIGGY_VIEWPORT_INTERVAL 3
#define PIGGY_VIEWPORT_INTERVAL_ERROR 1

#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 320
#define PIGGY_VIEWPORT_Y2 240
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240

#define __PATCH_DRAW_CLIPPED_POLY__

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/

#ifdef __APPLE__

#define PIGGY_RGB555
#define PIGGY_ROM 

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 640
#define PIGGY_VIEWPORT_Y2 480
#define PIGGY_VIEWPORT_WIDTH 640
#define PIGGY_VIEWPORT_HEIGHT 480
#define PIGGY_VIDEO_LUT

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/
#ifdef _arch_dreamcast

#define PIGGY_RGB555
#define PIGGY_ROM 

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 639
#define PIGGY_VIEWPORT_Y2 479
#define PIGGY_VIEWPORT_WIDTH 640
#define PIGGY_VIEWPORT_HEIGHT 480
#define PIGGY_VIDEO_LUT

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif

/******************************************************************************/
#ifdef __PIGGY_SDL__

#define PIGGY_RGB555
#define PIGGY_ROM 

#define PIGGY_VIEWPORT_INTERVAL 35
#define PIGGY_VIEWPORT_X1 0
#define PIGGY_VIEWPORT_Y1 0
#define PIGGY_VIEWPORT_X2 319
#define PIGGY_VIEWPORT_Y2 239
#define PIGGY_VIEWPORT_WIDTH 320
#define PIGGY_VIEWPORT_HEIGHT 240
#define PIGGY_VIDEO_LUT

#define CODE_IN_IWRAM
#define CODE_IN_ROM
#define IN_IWRAM
#define IN_EWRAM

#endif
/******************************************************************************/
#endif

 
