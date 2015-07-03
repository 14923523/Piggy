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

#include <3ds.h>
#include "game.h"

/******************************************************************************/

PIGGY_ROM s8 cube[] =
{
  1,              // Number of vertex lists (Animation Frames)
  8,              // Number of verices per list.
  -100,   100,  100, // Vertex X, Y, Z. Range (-120..120)
   100,   100,  100,
   100,   100, -100,
  -100,   100, -100,
  -100,  -100,  100,
   100,  -100,  100,
   100,  -100, -100,
  -100,  -100, -100,
  6,              // Number of faces in the polygon mesh.
                  // Each face is (NPoints, VID, U, V) + texture id.
  4, 0, 0, 63, 1, 63, 63, 2, 63, 0, 3, 0, 0, 8,
  4, 3, 0, 63, 2, 63, 63, 6, 63, 0, 7, 0, 0, 8,
  4, 2, 0, 63, 1, 63, 63, 5, 63, 0, 6, 0, 0, 8,
  4, 0, 0, 63, 4, 63, 63, 5, 63, 0, 1, 0, 0, 8,
  4, 0, 0, 63, 3, 63, 63, 7, 63, 0, 4, 0, 0, 8,
  4, 4, 0, 63, 7, 63, 63, 6, 63, 0, 5, 0, 0, 8
};

/******************************************************************************/

int friction(int a, int b)
{
  return a - f2i((b = a * b) < 0 ? b - FIXCEIL : b + FIXCEIL);
}

void entity_friction(entity_t* e, int amount)
{
  e->xx = friction(e->xx, amount);
  e->yy = friction(e->yy, amount);
}

void entity_move_forward_(entity_t* e, s16 p)
{
  e->xx += ((p*fixsin(f2i(e->t))) >> 4)/0x9C;
  e->yy += ((p*fixcos(f2i(e->t))) >> 4)/0x9C;
}

void entity_move_side_(entity_t* e, s16 p)
{
  e->xx += ((p*fixcos(f2i(e->t))) >> 4)/0x9C;
  e->yy -= ((p*fixsin(f2i(e->t))) >> 4)/0x9C;
}

void entity_move_forward(entity_t* e)
{
  e->xx += fixsin(f2i(e->t)) >> 4;
  e->yy += fixcos(f2i(e->t)) >> 4;
}

void entity_move_backwards(entity_t* e)
{
  e->xx -= fixsin(f2i(e->t)) >> 4;
  e->yy -= fixcos(f2i(e->t)) >> 4;
}

void entity_turn_right(entity_t* e)
{
  e->tt += i2f(25);
}

void entity_turn_left(entity_t* e)
{
  e->tt -= i2f(25);
}

void entity_turn_towards(entity_t* e, int x, int y)
{
  int x1 = x - (e->x + fixsin(f2i(e->t + i2f(100))));
  int y1 = y - (e->y + fixcos(f2i(e->t + i2f(100))));
  int x2 = x - (e->x + fixsin(f2i(e->t - i2f(100))));
  int y2 = y - (e->y + fixcos(f2i(e->t - i2f(100))));

  if ((x1 * x1 + y1 * y1) < (x2 * x2 + y2 * y2))
  {
    entity_turn_right(e);
  }
  else
  {
    entity_turn_left(e);
  }
}

void entity_kill(entity_t* e)
{
  e->ontick = 0;
  e->onhit  = 0;
  e->visual.data = 0;
  e->radius = 0;
}

/******************************************************************************/

void entity_default(entity_t* e, int isjumping, int iscrawling)
{
  piggy_t* piggy = (piggy_t*)e->piggy;

  e->xx = friction(e->xx, 30);
  e->yy = friction(e->yy, 30);
  e->tt = friction(e->tt, 80);

  e->r = friction(e->r, 20);
  e->p = friction(e->p, 20);

  {
    cell_t* cell = &piggy->cells[f2i(e->y)][f2i(e->x)];
    int bot = cell->bot + (iscrawling ? piggy->game.crawling : piggy->game.walking);
    int top = cell->top - piggy->game.ceiling;

    if (e->z <= bot)
    {
      e->z  -= ((e->z - bot) >> 2);
      e->zz -= (e->zz >> 2);
      if (isjumping && e->z < top)
      {
        e->z = bot;
        e->zz += piggy->game.jumping;
      }
    }
    else
    {
      e->zz += piggy->game.gravity;
    }

    if (e->z >= top)
    {
      e->z -= ((e->z - top) >> 2);
    }
  }
  if (e->life < 0) entity_kill(e);
}

void bullet_collision(entity_t* e1, entity_t* e2)
{
  piggy_t* piggy = (piggy_t*)e1->piggy;

  if (e2 != piggy->camera)
  {
    e2->life -= 10;
    entity_kill(e1);
  }
}

void bullet_behaviour(entity_t* e)
{
  e->onhit = bullet_collision;
  if (!e->xx || !e->yy)
  {
    entity_kill(e);
  }
}

void shoot_bullet(entity_t* e)
{
  piggy_t* piggy = (piggy_t*)e->piggy;

  entity_t* b = PIGGY_BULLET(piggy);

  b->t  = e->t;
  b->p  = e->p;
  b->xx = (fixsin(f2i( e->t))) + (e->xx);
  b->yy = (fixcos(f2i( e->t))) + (e->yy);
  b->zz = (fixsin(f2i(-e->p))) + (e->zz);
  b->x  = e->x;
  b->y  = e->y;
  b->z  = e->z;
  b->visual.data = spr_ball1;
  b->visual.width = 1;
  b->visual.height = 1;
  b->visual.mode = 1;
  b->radius = 16;
  b->ontick = bullet_behaviour;
}

u16 tx_o;
u8 tused_o=0;

void camera_behaviour(entity_t* e)
{
  piggy_t* piggy = (piggy_t *)e->piggy;

  touchPosition tpos;
  hidTouchRead(&tpos);
  u8 tused=0;
  u16 tx=tpos.px;

  if(keysHeld()&KEY_TOUCH)tused=1;

  if(tused && tused_o)e->tt+=i2f(((s32)tx-(s32)tx_o)*2);
  tused_o=tused;
  tx_o=tx;

  circlePosition cpos;
  hidCircleRead(&cpos);
  if (cpos.dx > -41 && cpos.dx < 41)
    cpos.dx = 0;
  if (cpos.dy > -41 && cpos.dy < 41)
    cpos.dy = 0;

  circlePosition cstick;
  hidCstickRead(&cstick);

  e->tt+=i2f(cstick.dx/4);

  entity_move_forward_(e, (cpos.dy*3)/4);
  entity_move_side_(e, (cpos.dx*3)/4);

  if (piggy->keyboard.left) entity_turn_left(e);
  if (piggy->keyboard.right) entity_turn_right(e);
  if (piggy->keyboard.up) entity_move_forward(e);
  if (piggy->keyboard.down) entity_move_backwards(e);
  entity_default(e, piggy->keyboard.b, FALSE);

  if ((piggy->keyboard.a && !piggy->keyboard._a) || (piggy->keyboard.r && !piggy->keyboard._r) || (piggy->keyboard.l && !piggy->keyboard._l))
  {
    shoot_bullet(e);
  }
}

void guard_collision(entity_t* e1, entity_t* e2)
{
//
}

void guard_behaviour(entity_t* e)
{
  piggy_t* piggy = (piggy_t *)e->piggy;

  e->visual.data   = sprites[(piggy->tick >> 2) & 3];
  e->visual.width  = 8;
  e->visual.height = 12;
  e->radius = 200;
  e->onhit = guard_collision;

  entity_turn_towards(e, piggy->camera->x, piggy->camera->y);

  entity_move_forward(e);
  e->xx = friction(e->xx, 50);
  e->yy = friction(e->yy, 50);

  entity_default(e, FALSE, FALSE);
}

void game_init(piggy_t* piggy)
{
  int x, y;

  piggy->game.gravity = -16;      // Gravity force.
  piggy->game.jumping = 80;       // Jumping force.
  piggy->game.crawling = i2f(1);  // Crawling height.
  piggy->game.walking = 400;      // Walking height.
  piggy->game.ceiling = 200;      // Crouch distance.

  piggy_load_map(piggy, &e1m1);

  //piggy->overlay = spr_ball1;
  piggy->camera->ontick = camera_behaviour;

#ifndef __PIGGY_EDITOR__

  for (y = 0; y < PIGGY_MAP_HEIGHT; y++)
  {
    for (x = 0; x < PIGGY_MAP_WIDTH; x++)
    {
      if (piggy->cells[y][x].ent == 2)
      {
        piggy_entity(piggy, i2f(x), i2f(y), i2f(1), guard_behaviour);
      }
    }
  }
#endif
}

void game_draw_item(piggy_t* piggy, int x, int y, int u, int v)
{
  /*
  u <<= 6;
  v <<= 6;
  stretch_sprite_di(piggy,
    -i2f(1) - x, y + i2f(1), i2f(1) - x, y - i2f(1), i2f(8)+50, spr_items,
    u, v, u + 64, v + 64, 1);
  */
}

void game_draw(piggy_t* piggy)
{
  piggy_draw(piggy);
  game_draw_item(piggy, i2f(7), i2f(7), 0, 0);
  game_draw_item(piggy, i2f(5), i2f(7), 1, 0);
  game_draw_item(piggy, i2f(3), i2f(7), 2, 0);
}

void game_tick(piggy_t* piggy)
{
  piggy_tick(piggy);
}

void game_loop(piggy_t* piggy)
{
  game_tick(piggy);
  game_draw(piggy);
}

