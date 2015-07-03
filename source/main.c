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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>

#include "piggy.h"

u8 leftOrRight;

void piggyUpdateKeyboard(piggy_t* y)
{
	hidScanInput();
	u32 keys=hidKeysHeld();

	y->keyboard.a       = (keys&KEY_A)!=0;
	y->keyboard.b       = (keys&KEY_B)!=0;
	y->keyboard.select  = (keys&KEY_SELECT)!=0;
	y->keyboard.left    = (keys&KEY_DLEFT)!=0;
	y->keyboard.right   = (keys&KEY_DRIGHT)!=0;
	y->keyboard.up      = (keys&KEY_DUP)!=0;
	y->keyboard.down    = (keys&KEY_DDOWN)!=0;
	y->keyboard.r       = (keys&KEY_R)!=0;
	y->keyboard.l       = (keys&KEY_L)!=0;
}

piggy_t piggy;

int main()
{
	srvInit();	
	aptInit();
	hidInit(NULL);
	irrstInit(NULL);
	gfxInitDefault();

	piggy_init(
		&piggy,
		(framebuffer_t*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL),
		(framebuffer_t*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL),
		textures, palette, lua
	);

	gfxSet3D(true);

	game_init(&piggy);

	while(aptMainLoop())
	{
		int i;
		for(i=0;i<2;i++)
		{
			piggy.viewport.front = piggy.viewport.back;
			piggy.viewport.back = (framebuffer_t*)gfxGetFramebuffer(GFX_TOP, leftOrRight?GFX_LEFT:GFX_RIGHT, NULL, NULL);
		
			game_draw(&piggy);

			leftOrRight^=1;
		}

		piggyUpdateKeyboard(&piggy);
		game_tick(&piggy);

		if(hidKeysDown()&KEY_START)break;

		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForEvent(GSPEVENT_VBlank0, true);
	}

	gfxExit();
	irrstExit();
	hidExit();
	aptExit();
	srvExit();
	return 0;
}
