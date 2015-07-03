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

#ifndef __MODEL_H__
#define __MODEL_H__

#include "piggy.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct
{
  float u, v;
  int vindex;
} md2command_t;

typedef struct 
{ 
   int magic; 
   int version; 
   int skinWidth; 
   int skinHeight; 
   int frameSize; 
   int numSkins; 
   int numVertices; 
   int numTexCoords; 
   int numTriangles; 
   int numGlCommands; 
   int numFrames; 
   int offsetSkins; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetGlCommands;
   int offsetEnd; 
} model_t;

typedef struct
{
   short vertexIndices[3];
   short textureIndices[3];
} triangle_t;

typedef struct
{
   short s, t;
} textureCoordinate_t;

typedef struct
{
   byte vertex[3];
   byte lightNormalIndex;
} triangleVertex_t;

typedef struct
{
   float scale[3];
   float translate[3];
   char name[16];
   triangleVertex_t vertices[1];
} frame_t;

void md2_draw(entity_t* e);

#ifdef __cplusplus
};
#endif

#endif
