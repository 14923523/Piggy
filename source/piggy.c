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

#include "piggy.h"

/******************************************************************************/

/*
** Name: palette_overbright
** Desc: Converts a standard palette to a overbright palette. Use this on
**       platforms that bypass the LUA. 
*/
void palette_overbright(palette_t dst, palette_t src, int brightness)
{
  int i;
  
  for (i = 0; i < 256; i++)
  {
    int r = src[i][0] * brightness / FIXONE;
    int g = src[i][1] * brightness / FIXONE;
    int b = src[i][2] * brightness / FIXONE;

    dst[i][0] = CLAMP(r, 0, 255);
    dst[i][1] = CLAMP(g, 0, 255);
    dst[i][2] = CLAMP(b, 0, 255);    
  }
}

/*
** Name: rgb_convert
** Desc: Converts a rgb color to another format.
*/
int rgb_convert(int color, int rmask, int gmask, int bmask)
{
  return
    ((RGB_RED  (color) * rmask / 31) & rmask) |
    ((RGB_GREEN(color) * gmask / 31) & gmask) |
    ((RGB_BLUE (color) * bmask / 31) & bmask) ;
}

/*
** Name: viewport_to_video
** Desc: The Piggy uses 5:5:5 colour, so if your platform uses a different
**       format, then one solution is to convert the pixels when blitting to
**       the video buffer.
*/
#ifdef PIGGY_VIDEO_LUT
void viewport_to_video(
  rgb555_t* video, int pitch,
  viewport_t* vp,
  int rmask, int gmask, int bmask)
{
  int i, y;

  if (!vp->video_lut_filled)
  {
    vp->video_lut_filled = TRUE;

    for (i = RGB_MAX; i--;)
    {
      vp->video_lut[i] = rgb_convert(i, rmask, gmask, bmask);
    }
  }
  for (y = 0; y < PIGGY_VIEWPORT_HEIGHT; y++)
  {
    rgb555_t* fb = video;
    rgb555_t* tb= vp->back->pixels[y];
    i = PIGGY_VIEWPORT_WIDTH;
#define AFFINE(I) {fb[I] = vp->video_lut[*tb++];}
    AFFINE_LOOP
#undef AFFINE
    video = (rgb555_t*)((int)video + pitch);
  }
}
#endif

/******************************************************************************/

/*
** Name: cell_init
** Desc: Initialize a piggy cell to its defaults.
*/
void cell_init(cell_t* cell, int issolid)
{
  CLEARMEM(cell);
  
  if (issolid)
  {
    cell->top = i2f(0);
    cell->bot = i2f(0);
  }
  else
  {
    cell->top = i2f(4);
    cell->bot = i2f(0);
  }
#ifdef __PIGGY_EDITOR__
  cell->tos = cell->top;
  cell->bos = cell->bot;
#endif
  cell->lit = PIGGY_LIGHT_MAX;
  cell->ttx = 7;
  cell->wtx = 0;
  cell->btx = 28;
}

/*
** Name: piggy_entity
** Desc: Creates a new entity and returns a entity pointer.
*/
entity_t* piggy_entity(piggy_t* piggy, int x, int y, int z, entity_behaviour_t behaviour)
{
  entity_t* e = &piggy->entities[piggy->nentities++];

  CLEARMEM(e);
  
  e->x = x;
  e->y = y;
  e->z = z;
  e->ontick = behaviour;
  e->radius = i2fdiv2(1);
  e->life = 100;
  e->piggy = piggy;

  return e;
}

/*
** Name: piggy_light
** Desc: Renders a spot light at a given cell location.
*/
void piggy_light(piggy_t* piggy, int lightx, int lighty)
{
  int i, lit, xhit, yhit;

  for (i = 0; i < 2048; i += 8)
  {
    int x = lightx, xx = fixsin(i) >> 1;
    int y = lighty, yy = fixcos(i) >> 1;

    for (lit = 63; lit > 0; )
    {
      x += xx; if ((xhit = CELL_IS_SOLID(&piggy->cells[f2i(y)][f2i(x)])) != 0) xx = -xx;
      y += yy; if ((yhit = CELL_IS_SOLID(&piggy->cells[f2i(y)][f2i(x)])) != 0) yy = -yy;

      piggy->cells[f2i(y + FIXHALF)][f2i(x + FIXHALF)].lit += lit;

      if (xhit || yhit) lit >>= 2;
    }
  }
}

/*
** Name: entity_motion
** Desc: Simple map to entity motion & collision.
*/
void entity_motion(entity_t* e)
{
  piggy_t* piggy = (piggy_t*)e->piggy;

  e->x += e->xx; e->y += e->yy; e->z += e->zz;
  e->r += e->rr; e->t += e->tt; e->p += e->pp;

  if (e->radius)
  {
    int x, y;
    cell_t* cell;

#define IS_COLLISION ((e->z > cell->top) || (e->z < cell->bot))

    cell = &piggy->cells[f2i(e->y)][f2i(x = e->x - e->radius)];

    if (IS_COLLISION)
    {
      e->x += FIXONE - (x & FIXCEIL);
      e->xx = 0;
    }

    cell = &piggy->cells[f2i(e->y)][f2i(x = e->x + e->radius)];

    if (IS_COLLISION)
    {
      e->x -= (x & FIXCEIL);
      e->xx = 0;
    }

    cell = &piggy->cells[f2i(y = e->y - e->radius)][f2i(e->x)];

    if (IS_COLLISION)
    {
      e->y += FIXONE - (y & FIXCEIL);
      e->yy = 0;
    }

    cell = &piggy->cells[f2i(y = e->y + e->radius)][f2i(e->x)];

    if (IS_COLLISION)
    {
      e->y -= (y & FIXCEIL);
      e->yy = 0;
    }
  }
}

/******************************************************************************/

void piggy_clear_entities(piggy_t* piggy)
{
  int i;

  piggy->nentities = 0;

  piggy->camera = piggy_entity(piggy,
    i2fdiv2(PIGGY_MAP_WIDTH),
    i2fdiv2(PIGGY_MAP_HEIGHT),
    i2f(0), 0);

  for (i = 0; i < PIGGY_BULLET_MAX; i++)
  {
    piggy->bullets[i] = piggy_entity(piggy, 0, 0, 0, 0);
  }
}

/*
** Name: piggy_load_cell
** Desc: Loads a cell from a ROM based structure.
*/
void piggy_load_cell(cell_t* dst, rom_cell_t* src)
{
  dst->swi = src->swi;
  dst->ent = src->ent;
  dst->top = src->top;
  dst->mid = src->mid;
  dst->bot = src->bot;
  dst->wtx = src->wtx;
  dst->ttx = src->ttx;
  dst->btx = src->btx;
  dst->lit = src->lit;
}

/*
** Name: piggy_save_cell
** Desc: Saves a cell from a ROM based structure.
*/
void piggy_save_cell(cell_t* src, rom_cell_t* dst)
{
  dst->swi = src->swi;
  dst->ent = src->ent;
  dst->top = src->top;
  dst->mid = src->mid;
  dst->bot = src->bot;
  dst->wtx = src->wtx;
  dst->ttx = src->ttx;
  dst->btx = src->btx;
  dst->lit = src->lit;
}

void piggy_load_map(piggy_t* piggy, rom_map_t* src)
{
  int x, y;

  for (y = 0; y < PIGGY_MAP_HEIGHT; y++)
  {
    for (x = 0; x < PIGGY_MAP_WIDTH; x++)
    {
      piggy_load_cell(&piggy->cells[y][x], &src->cells[y][x]);
    }
  }
  if (!piggy->camera) piggy_clear_entities(piggy);
  
  for (y = 0; y < PIGGY_MAP_HEIGHT; y++)
  {
    for (x = 0; x < PIGGY_MAP_WIDTH; x++)
    {
      if (piggy->cells[y][x].ent == 1)
      {
        piggy->camera->x = i2f(x) + FIXHALF;
        piggy->camera->y = i2f(y) + FIXHALF;
      }
    }
  }
  piggy->camera->t = 0;
  piggy->camera->r = 0;
  piggy->camera->p = i2f(256);
}

/*
** Name: piggy_save_map
** Desc: Saves the current map to a rom based map.
*/
void piggy_save_map(piggy_t* piggy, rom_map_t* map)
{
  int x, y;

  for (y = 0; y < PIGGY_MAP_HEIGHT; y++)
  {
    for (x = 0; x < PIGGY_MAP_WIDTH; x++)
    {
      piggy_save_cell(&piggy->cells[y][x], &map->cells[y][x]);
    }
  }
}

/*
** Name: piggy_init_map
** Desc: Clears a piggy map to its default settings.
*/
void piggy_init_map(piggy_t* piggy)
{
  int x, y;
  
  for (y = 0; y < PIGGY_MAP_HEIGHT; y++)
  {
    for (x = 0; x < PIGGY_MAP_WIDTH; x++)
    {
      cell_t* cell = &piggy->cells[y][x];
      cell_init(cell,
        x == 0 || y == 0 ||
        (x == PIGGY_MAP_WIDTH  - 1) ||
        (y == PIGGY_MAP_HEIGHT - 1));

      if (!((x+8)&15) && !((y+8)&15))
      {
        cell->swi |= CELL_SWI_LIGHT;
      }
    } 
  }
}

/******************************************************************************/

/*
** Name: isqrt
** Desc: Integer square root. Take the square root of an integer.
*/
int isqrt(int value)
{
  int root = 0;

#define STEP(shift) \
    if((0x40000000 >> shift) + root <= value)          \
    {                                                   \
        value -= (0x40000000 >> shift) + root;          \
        root = (root >> 1) | (0x40000000 >> shift);     \
    }                                                   \
    else                                                \
    {                                                   \
        root >>= 1;                                     \
    }

  STEP( 0); STEP( 2); STEP( 4); STEP( 6);
  STEP( 8); STEP(10); STEP(12); STEP(14);
  STEP(16); STEP(18); STEP(20); STEP(22);
  STEP(24); STEP(26); STEP(28); STEP(30);

  // round to the nearest integer, cuts max error in half

  if (root < value) root++;

  return root;
}

/******************************************************************************/

/*
** Name: piggy_init
** Desc: Setup a default map and position the camera.
*/
void piggy_init(
  piggy_t* piggy,
  framebuffer_t* front,
  framebuffer_t* back,
  texture_t* textures,
  palette_t palette,
  lua_t lua)
{
  int i;

  CLEARMEM(piggy);

  for (i = 0; i < PIGGY_TEXTURE_MAX; i++)
  {
    piggy->surfaces[i].xsize = 6;
    piggy->surfaces[i].ysize = 6;
  }

  piggy->surfaces[PIGGY_TEXTURE_SKY].xsize = 5;
  piggy->surfaces[PIGGY_TEXTURE_SKY].ysize = 5;

  piggy->surfaces[0].xsize = 4;
  piggy->surfaces[0].ysize = 4;
  piggy->surfaces[2].xsize = 5;
  piggy->surfaces[2].ysize = 5;

  piggy_init_map(piggy);
  
  piggy->viewport.front = front;
  piggy->viewport.back  = back;

  piggy->textures    = textures;
  piggy->palette     = palette;
  piggy->lighting[0] = lua;
  piggy->lighting[1] = lua;
  piggy->lighting[2] = lua;
  piggy->lighting[3] = lua;
}

void piggy_tick(piggy_t* piggy)
{
  int i;

  piggy->tick++;

  for (i = 0; i < piggy->nentities; i++)
  {
    entity_t* e = &piggy->entities[i];
    if (e->ontick) e->ontick(e);
  }

  for (i = 0; i < piggy->nentities; i++)
  {
    entity_t* e1 = &piggy->entities[i];

    entity_motion(e1);

    if (e1->radius)
    {
      int j;
      
      for (j = i + 1; j < piggy->nentities; j++)
      {
        entity_t* e2 = &piggy->entities[j];

        if (e2->radius)
        {
          int size = e1->radius + e2->radius;
          int x = e1->x - e2->x;
          int y = e1->y - e2->y;
          int z = e1->z - e2->z;

          if (
            x < size && x > -size &&
            y < size && y > -size &&
            z < size && z > -size )
          {
            if (e1->onhit) e1->onhit(e1, e2);
            if (e2->onhit) e2->onhit(e2, e1);

            if (!(e1->swi & ENTITY_SWI_NO_COLLISION_RESPONSE))
            {
              if (x > 0) {e1->x += ( size - x) >> 1;}
              if (x < 0) {e1->x += (-size - x) >> 1;}
              if (y > 0) {e1->y += ( size - y) >> 1;}
              if (y < 0) {e1->y += (-size - y) >> 1;}
            }
            if (!(e2->swi & ENTITY_SWI_NO_COLLISION_RESPONSE))
            {
              if (x > 0) {e2->x -= ( size - x) >> 1;}
              if (x < 0) {e2->x -= (-size - x) >> 1;}
              if (y > 0) {e2->y -= ( size - y) >> 1;}
              if (y < 0) {e2->y -= (-size - y) >> 1;}
            }
          }
        }
      }
    }
  }
  piggy->keyboard._a       = piggy->keyboard.a;
  piggy->keyboard._b       = piggy->keyboard.b;
  piggy->keyboard._select  = piggy->keyboard.select;
  piggy->keyboard._left    = piggy->keyboard.left;
  piggy->keyboard._right   = piggy->keyboard.right;
  piggy->keyboard._up      = piggy->keyboard.up;
  piggy->keyboard._down    = piggy->keyboard.down;
  piggy->keyboard._r       = piggy->keyboard.r;
  piggy->keyboard._l       = piggy->keyboard.l;

  piggy->surfaces[PIGGY_TEXTURE_SKY].xpan = -(piggy->camera->x>>1) + (piggy->tick<<2);
  piggy->surfaces[PIGGY_TEXTURE_SKY].ypan = -(piggy->camera->y>>1) + (piggy->tick<<2);
  piggy->surfaces[PIGGY_TEXTURE_WINDOW].xpan = -(piggy->camera->y>>1) + (piggy->tick<<2);

  piggy->surfaces[PIGGY_TEXTURE_LAVA ].xpan = fixsin(piggy->tick << 4) >> 2;
  piggy->surfaces[PIGGY_TEXTURE_LAVA ].ypan = fixcos(piggy->tick << 4) >> 2;
  piggy->surfaces[PIGGY_TEXTURE_WATER].xpan = fixsin(piggy->tick << 4) >> 2;
  piggy->surfaces[PIGGY_TEXTURE_WATER].ypan = fixcos(piggy->tick << 4) >> 2;
}

/*
** Name: piggy_ambient_lighting
** Desc: Setup a standard ambient lighting. Current lighting is removed.
** Note: This is obsolete since we have better lighting code now.
*/
void piggy_ambient_lighting(piggy_t* piggy, int lit)
{
  int x, y;

  for (y = 0; y < PIGGY_MAP_HEIGHT; y++)
  {
    for (x = 0; x < PIGGY_MAP_WIDTH; x++)
    {
      piggy->cells[y][x].lit = lit;
    }
  }
}

/*
** Name: piggy_default_lighting
** Desc: Renders a default lighting setup for maps that dont have lights. This
**       is currently the only lighting system and is designed to give good
**       results for all map designs.
*/
void piggy_default_lighting(piggy_t* piggy)
{
  int x, y;

  piggy_ambient_lighting(piggy, i2f(0));

  for (y = 1; y < PIGGY_MAP_HEIGHT - 1; y++)
  {
    for (x = 1; x < PIGGY_MAP_WIDTH - 1; x++)
    {
      cell_t* cell = &piggy->cells[y][x];
      
      if (cell->swi & CELL_SWI_LIGHT)
      {
        piggy_light(piggy, i2f(x) + FIXHALF, i2f(y) + FIXHALF);
      }
    }
  }
}

/******************************************************************************/

void* piggy_memcpy(void* d, void* s, unsigned n)
{
  while (n--) ((u8*)d)[n] = ((u8*)s)[n];
  return d;
}

void* piggy_memset(void *s, int c, unsigned n)
{
  while (n--) ((u8*)s)[n] = c;
  return s;
}

/******************************************************************************/

