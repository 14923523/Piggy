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

#ifndef __SPRITES__
#define __SPRITES__
#include "piggy.h"
#define PIGGY_SPRITE_MAX 5
extern PIGGY_ROM sprite_t sprites[];
extern PIGGY_ROM u16 spr_00[];
extern PIGGY_ROM u16 spr_01[];
extern PIGGY_ROM u16 spr_02[];
extern PIGGY_ROM u16 spr_03[];
extern PIGGY_ROM u16 spr_ball1[];
#endif
