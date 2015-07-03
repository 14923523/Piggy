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

#define CONFIG_3D_SLIDERSTATE (*(float*)0x1FF81080)

/******************************************************************************/

void CODE_IN_IWRAM matrix_rotate_world(matrx_t m, int alp, int bet, int gam)
{
  int cosalp = fixcos(alp);
  int sinalp = fixsin(alp);
  int cosbet = fixcos(bet);
  int sinbet = fixsin(bet);
  int cosgam = fixcos(gam);
  int singam = fixsin(gam);

  m[0][0] =  fixmul(singam, fixmul(sinbet, sinalp)) + fixmul(cosgam, cosalp);
  m[0][1] =  fixmul(cosbet, sinalp);
  m[0][2] =  fixmul(singam, cosalp) - fixmul(cosgam, fixmul(sinbet, sinalp));

  m[1][0] =  fixmul(singam, fixmul(sinbet, cosalp)) - fixmul(cosgam, sinalp);
  m[1][1] =  fixmul(cosbet, cosalp);
  m[1][2] = -fixmul(cosgam, fixmul(sinbet, cosalp)) - fixmul(singam, sinalp);

  m[2][0] = -fixmul(singam, cosbet);
  m[2][1] =  sinbet;
  m[2][2] =  fixmul(cosgam, cosbet);
}

void CODE_IN_IWRAM matrix_rotate_object(matrx_t m, int alp, int bet, int gam)
{
  int cosalp = fixcos(alp);
  int sinalp = fixsin(alp);
  int cosbet = fixcos(bet);
  int sinbet = fixsin(bet);
  int cosgam = fixcos(gam);
  int singam = fixsin(gam);

  m[0][0] =  fixmul(cosalp, cosgam) - fixmul(sinalp, fixmul(sinbet, singam));
  m[0][1] =  fixmul(sinalp, cosgam) + fixmul(cosalp, fixmul(sinbet, singam));
  m[0][2] =  fixmul(cosbet, singam);

  m[1][0] = -fixmul(sinalp, cosbet);
  m[1][1] =  fixmul(cosalp, cosbet);
  m[1][2] = -sinbet;

  m[2][0] = -fixmul(cosalp, singam) - fixmul(sinalp, fixmul(sinbet, cosgam));
  m[2][1] =  fixmul(cosalp, fixmul(sinbet, cosgam)) - fixmul(sinalp, singam);
  m[2][2] =  fixmul(cosbet, cosgam);
}

/******************************************************************************/

/*
** Name: vertex_project
** Desc: Projects a vertex into screen space. (ie: 3D to 2D viewport space)
*/
void CODE_IN_IWRAM vertex_project(vertex_t* v)
{
  if (v->z > PIGGY_RECIPROCAL_MIN && v->z < PIGGY_RECIPROCAL_MAX)
  {
    int z = reciprocal[v->z];

    v->sx =  (((PIGGY_VIEWPORT_X2-PIGGY_VIEWPORT_X1)*v->x*z)>>9)+i2fdiv2(PIGGY_VIEWPORT_X2+PIGGY_VIEWPORT_X1);
    v->sy = -(((PIGGY_VIEWPORT_Y2-PIGGY_VIEWPORT_Y1)*v->y*z)>>9)+i2fdiv2(PIGGY_VIEWPORT_Y2+PIGGY_VIEWPORT_Y1);
  }
  else
  {
    v->sx = v->sy = 0;
  }
}

/******************************************************************************/

/*
** Name: draw_sprite
** Desc: Draws a 16bit sprite to a framebuffer. The sprite is clipped to the
**       given rectangle.
*/
/* This function has been altered to allow the Dreamcast port to get a valid Z value for the sprite.
  The software renderer may be broken here, as I haven't tested it although an attempt has been made 
  to preserve the correct behavior. */
#ifndef __PATCH_DRAW_SPRITE__
void CODE_IN_IWRAM draw_sprite(
  piggy_t* piggy, vertex_t a, vertex_t b,
  PIGGY_ROM sprite_t sprite,
  int cl, int ct, int cr, int cb, int mode)
{
  int x1 = f2i(a.sx), y1 = f2i(a.sy), u1 = a.u, v1 = a.v;
  int x2 = f2i(b.sx), y2 = f2i(b.sy), u2 = b.u, v2 = b.v;
  if ((x2 - x1) > 0 && (y2 - y1) > 0)
  {
    int uu = f2i((u2 - u1) * reciprocal[x2 - x1]);
    int vv = f2i((v2 - v1) * reciprocal[y2 - y1]);
    
    u1 = i2f(u1);
    v1 = i2f(v1);

    if (x1 < cl) {u1 += (cl - x1) * uu; x1 = cl;}
    if (y1 < ct) {v1 += (ct - y1) * vv; y1 = ct;}
    if (x2 > cr) {x2 = cr;}
    if (y2 > cb) {y2 = cb;}

    if ((x2 - x1) > 0 && (y2 - y1) > 0)
    {
      int w = sprite[0];

      mode=0;
      // switch (mode)
      // {
      //   case 0:
      //   {
          for (; y1 < y2; y1++, v1 += vv)
          {
            PIGGY_ROM u16* tb = &sprite[f2i(v1)*w+2];
            // u16* fb = &piggy->viewport.back->pixels[y1][x1];
            int i = x2 - x1, u = u1 - uu;

            u8* fb2=&(((u8*)piggy->viewport.back->pixels)[(x1*PIGGY_VIEWPORT_HEIGHT+PIGGY_VIEWPORT_HEIGHT-y1)*3]);
#define AFFINE(I) {u16 c=tb[f2i(u+=uu)];if(c)putPixel(fb2,c,I);}
          AFFINE_LOOP;
#undef  AFFINE
          }
      //     break;
      //   }
      // }
    }
  }
}
#endif

/*
** Name: stretch_sprite_di
** Desc: Draws a device independant sprite to the screen. The sprite will be
**       scaled to match the viewport.
*/
void CODE_IN_IWRAM stretch_sprite_di(
  piggy_t* piggy,
  int x1, int y1, int x2, int y2, int z,
  PIGGY_ROM sprite_t sprite,
  int u1, int v1, int u2, int v2, int mode)
{
  if (z > 4)
  {
    vertex_t a, b;

    a.x = x1; a.y = y1; a.z = z; a.u = u1; a.v = v1;
    b.x = x2; b.y = y2; b.z = z; b.u = u2; b.v = v2;

    vertex_project(&a);
    vertex_project(&b);

    draw_sprite(
      piggy, a, b,
      sprite,
      PIGGY_VIEWPORT_X1, PIGGY_VIEWPORT_Y1, PIGGY_VIEWPORT_X2, PIGGY_VIEWPORT_Y2,
      mode);
  }
}

void CODE_IN_IWRAM draw_sprite_di(
  piggy_t* piggy,
  int x1, int y1, int x2, int y2, int z,
  PIGGY_ROM sprite_t sprite, int mode)
{
  stretch_sprite_di(piggy, x1, y1, x2, y2, z, sprite, 0, 0, sprite[0], sprite[1], mode);
}

/*
** Name: draw_poly_flat
** Desc: Clears a polygon to black. Might be used to render far polygons that
**       have faded to black.
*/
#ifndef __PATCH_DRAW_FILLED_POLY__
void CODE_IN_IWRAM draw_poly_flat(vertex_t* p, int n, framebuffer_t* dst)
{
  int lt_i = 0, lt_x = 0, lt_xx = 0;
  int rt_i = 0, rt_x = 0, rt_xx = 0;
  int x1 = 0, x2 = 0, i = 0;
  int lt_length = 0, rt_length = 0;
  int y1 = 999999, y2 = -999999;

  for (i = n; i--;)
  {
    p[i].d = fixup(p[i].y);
    if (p[i].y < y1) {y1 = p[i].y; lt_i = rt_i = i;}
    if (p[i].y > y2) {y2 = p[i].y;}
  }
  
  if ((y1 = fixup(y1)) < PIGGY_VIEWPORT_Y1) y1 = PIGGY_VIEWPORT_Y1;
  if ((y2 = fixup(y2)) > PIGGY_VIEWPORT_Y2) y2 = PIGGY_VIEWPORT_Y2;

  for (; y1 < y2; y1++)
  {
    if (--lt_length <= 0)
    {
      do {
        i = lt_i--; if (lt_i < 0) lt_i = n - 1;
      } while ((lt_length = p[lt_i].d - p[i].d) <= 0);

      lt_xx = r2i((p[lt_i].x - (lt_x = p[i].x)) * reciprocal[lt_length]);
    }
    if (--rt_length <= 0)
    {
      do {
        i = rt_i++; if (rt_i >= n) rt_i = 0;
      } while ((rt_length = p[rt_i].d - p[i].d) <= 0);

      rt_xx = r2i((p[rt_i].x - (rt_x = p[i].x)) * reciprocal[rt_length]);
    }
    if ((x1 = fixup(lt_x)) < PIGGY_VIEWPORT_X1) x1 = PIGGY_VIEWPORT_X1;
    if ((x2 = fixup(rt_x)) > PIGGY_VIEWPORT_X2) x2 = PIGGY_VIEWPORT_X2;

    if ((i = x2 - x1) > 0)
    {
      // u16* fb = &dst->pixels[y1][x1];
      
    u8* fb2=&(((u8*)dst->pixels)[(x1*PIGGY_VIEWPORT_HEIGHT+PIGGY_VIEWPORT_HEIGHT-y1)*3]);
#define AFFINE(I) putPixel(fb2,0,I);
      AFFINE_LOOP;
#undef  AFFINE
    }
    lt_x += lt_xx;
    rt_x += rt_xx; 
  }
}
#endif

/*
** Name: draw_poly
** Desc: Renders a lit, texture mapped polygon to a viewport. No clipping is
**       applied. 
** Note: This is the function that Greg replaced with his poly ARM asm code.
*/
#ifndef __PATCH_DRAW_POLY__
void CODE_IN_IWRAM draw_poly(vertex_t* p, int n, PIGGY_ROM texture_t texture, framebuffer_t* dst,
  lua_t lua)
{
  int lt_i = 0, lt_x = 0, lt_xx = 0, lt_u = 0, lt_uu = 0, lt_v = 0, lt_vv = 0, lt_l = 0, lt_ll = 0;
  int rt_i = 0, rt_x = 0, rt_xx = 0, rt_u = 0, rt_uu = 0, rt_v = 0, rt_vv=  0, rt_l = 0, rt_ll = 0;
  int x1 = 0, x2 = 0, i = 0, j = 0, u = 0, uu = 0, v = 0, vv = 0, l = 0, ll = 0;
  int lt_length = 0, rt_length = 0;
  int y1 = 999999, y2 = -999999;

  for (i = n; i--;)
  {
    p[i].d = fixup(p[i].y);
    if (p[i].y < y1) {y1 = p[i].y; lt_i = rt_i = i;}
    if (p[i].y > y2) {y2 = p[i].y;}
  }

  if ((y1 = fixup(y1)) < PIGGY_VIEWPORT_Y1) y1 = PIGGY_VIEWPORT_Y1;
  if ((y2 = fixup(y2)) > PIGGY_VIEWPORT_Y2) y2 = PIGGY_VIEWPORT_Y2;

  for (; y1 < y2; y1++)
  {
    if (--lt_length <= 0)
    {
      do {
        i = lt_i--; if (lt_i < 0) lt_i = n - 1;
      } while ((lt_length = p[lt_i].d - p[i].d) <= 0);

      j = reciprocal[lt_length];

      lt_xx = r2i((p[lt_i].x - (lt_x = p[i].x)) * j);
      lt_uu = r2i((p[lt_i].u - (lt_u = p[i].u)) * j);
      lt_vv = r2i((p[lt_i].v - (lt_v = p[i].v)) * j);
      lt_ll = r2i((p[lt_i].l - (lt_l = p[i].l)) * j);
    }
    if (--rt_length <= 0)
    {
      do {
        i = rt_i++; if (rt_i >= n) rt_i = 0;
      } while ((rt_length = p[rt_i].d - p[i].d) <= 0);

      j = reciprocal[rt_length];

      rt_xx = r2i((p[rt_i].x - (rt_x = p[i].x)) * j);
      rt_uu = r2i((p[rt_i].u - (rt_u = p[i].u)) * j);
      rt_vv = r2i((p[rt_i].v - (rt_v = p[i].v)) * j);
      rt_ll = r2i((p[rt_i].l - (rt_l = p[i].l)) * j);
    }
    if ((x1 = fixup(lt_x)) < PIGGY_VIEWPORT_X1) x1 = PIGGY_VIEWPORT_X1;
    if ((x2 = fixup(rt_x)) > PIGGY_VIEWPORT_X2) x2 = PIGGY_VIEWPORT_X2;

    if ((i = x2 - x1) > 0)
    {
      // u16* fb = &dst->pixels[y1][x1];
      u8* fb2=&(((u8*)dst->pixels)[(x1*PIGGY_VIEWPORT_HEIGHT+PIGGY_VIEWPORT_HEIGHT-y1)*3]);
      
      j = reciprocal[i];
      uu = r2i((rt_u - (u = lt_u)) * j); u -= uu;
      vv = r2i((rt_v - (v = lt_v)) * j); v -= vv;
      l = lt_l - rt_l;

      if (l > PIGGY_CONSTANT_LIGHT || l < -PIGGY_CONSTANT_LIGHT)
      {
        ll = r2i((rt_l - (l = lt_l)) * j); l -= ll;

#define AFFINE(I) putPixel(fb2,lua[f2i(l+=ll)][texture[f2i((v+=vv))&63][f2i((u+=uu))&63]],I);
      AFFINE_LOOP;
#undef  AFFINE
      }
      else
      {
        u16* lut = lua[f2i((lt_l + rt_l) >> 1)];
#define AFFINE(I) putPixel(fb2,lut[texture[f2i(v+=vv)&63][f2i(u+=uu)&63]],I);
      AFFINE_LOOP;
#undef  AFFINE
      }
    }
    lt_x += lt_xx; lt_u += lt_uu; lt_v += lt_vv; lt_l += lt_ll;
    rt_x += rt_xx; rt_u += rt_uu; rt_v += rt_vv; rt_l += rt_ll; 
  }
}
#endif

/******************************************************************************/

/*
** Name: polygon_clip
** Desc: clip a polygon to pre-calculated plane distances.
*/
#ifndef __PATCH_POLYGON_CLIP__
int CODE_IN_IWRAM polygon_clip(piggy_t* piggy, vertex_t** dst, vertex_t** src, int n)
{
  vertex_t** tmp = dst;

  if (n > 2)
  {
    vertex_t* pre;

    for (pre = src[n - 1]; n--; pre = *src++)
    {
      if (pre->d >= 0) *dst++ = pre;

      if ((pre->d ^ src[0]->d) < 0)
      {
        vertex_t* dv = *dst++ = PIGGY_VERTEX(piggy);
        vertex_t* sv = src[0];
        int d = pre->d - sv->d;
        int r = f2i(d < 0 ? reciprocal[-d] * -pre->d : reciprocal[d] * pre->d);

        dv->x = fixlerp(pre->x, sv->x, r);
        dv->y = fixlerp(pre->y, sv->y, r);
        dv->z = fixlerp(pre->z, sv->z, r);
        dv->u = fixlerp(pre->u, sv->u, r);
        dv->v = fixlerp(pre->v, sv->v, r);
        dv->l = fixlerp(pre->l, sv->l, r);

        vertex_project(dv);
      }
    }
  }
  return dst - tmp;
}
#endif

/******************************************************************************/

/*
** Name: draw_clipped_poly
** Desc: Renders a clipped poly to a screen. The texture is DDA mapped to the given polygon.
*/
#ifndef __PATCH_DRAW_CLIPPED_POLY__
void CODE_IN_IWRAM draw_clipped_poly(piggy_t* piggy, polyclip_t src, int n, int tid)
{
  int i;
  polygon_t p;

  for (i = n; i--;)
  {
    p[i].x = src[i]->sx;
    p[i].y = src[i]->sy;
    p[i].u = src[i]->u;
    p[i].v = src[i]->v;
    p[i].l = src[i]->l;
  }
  if (
    tid == PIGGY_TEXTURE_SKY ||
    tid == PIGGY_TEXTURE_WINDOW ||
    tid == PIGGY_TEXTURE_WATER ||
    tid == PIGGY_TEXTURE_LAVA)
  {
    for (i = n; i--;) p[i].l = i2f(32);
  }
// NOTE: Gregs ARM Texture Mapper for the GBA swaps X & Y.
#ifdef __GBA__
#ifdef __PATCH_DRAW_POLY__
  for (i = n; i--;)
  {
    int x = p[i].x; p[i].x = p[i].y; p[i].y = x;
  }
#endif
#endif

  draw_poly(p, n, piggy->textures[tid], piggy->viewport.back, piggy->lighting[0]);
}
#endif

/******************************************************************************/

/*
** Name: draw_texture
** Desc: Renders a texture to a screen. The texture is DDA mapped to the given polygon.
*/
#ifndef __PATCH_DRAW_TEXTURE__
void CODE_IN_IWRAM draw_texture(piggy_t* piggy, polyclip_t p, int n, int tid)
{
  int i;

  for (i = n; i--;)
  {
    if (
       (p[i]->x + p[i]->z) < 0 ||
       (p[i]->z - p[i]->x) < 0 ||
       (p[i]->y + p[i]->z) < 0 ||
       (p[i]->z - p[i]->y) < 0 )
    {
      polyclip_t a, b;

      for (i = n; i--; p[i]->d = p[i]->x + p[i]->z); n = polygon_clip(piggy, a, p, n); // Left
      for (i = n; i--; a[i]->d = a[i]->z - a[i]->x); n = polygon_clip(piggy, b, a, n); // Right
      for (i = n; i--; b[i]->d = b[i]->y + b[i]->z); n = polygon_clip(piggy, a, b, n); // Top
      for (i = n; i--; a[i]->d = a[i]->z - a[i]->y); n = polygon_clip(piggy, b, a, n); // Bottom

      p = b;
      
      break;
    }
  }
  if (n > 2) draw_clipped_poly(piggy, p, n, tid);
}
#endif
/******************************************************************************/

/*
** Name: rotate_vector
** Desc: Rotates a cell vector and apply world lighting. Rotated vectors are
**       cached in map cells so they can be reused. The rotated vertex is
**       indexed by its un-rotated y position.
*/
vertex_t* CODE_IN_IWRAM rotate_vector(piggy_t* piggy, vec3_t* v)
{
  int x, y, z;
  vertex_t* rv;
  cell_t* cell = &piggy->cells[f2i(v->z)][f2i(v->x)];

  if (CELL_IS_MARKED(cell))
  {
    for (x = cell->vcache_size; x--;)
    {
      if (cell->vcache[x]->i == v->y) return cell->vcache[x];
    }
  }
  else
  {
    CELL_MARK(cell);
    cell->vcache_size = 0;
  }
  
  x = v->x - piggy->camera->x;
  y = v->y - piggy->camera->z;
  z = v->z - piggy->camera->y;

  rv    = PIGGY_VERTEX(piggy);
  rv->i = v->y;
  rv->x = f2i(piggy->m[0][0] * x + piggy->m[0][1] * y + piggy->m[0][2] * z);
  rv->y = piggy->is2d ? y : f2i(piggy->m[1][0] * x + piggy->m[1][1] * y + piggy->m[1][2] * z);
  rv->z = f2i(piggy->m[2][0] * x + piggy->m[2][1] * y + piggy->m[2][2] * z);
  rv->l = cell->lit - (rv->z >> 1);

  if (rv->l < PIGGY_LIGHT_MIN) rv->l = PIGGY_LIGHT_MIN;
  if (rv->l > PIGGY_LIGHT_MAX) rv->l = PIGGY_LIGHT_MAX;

  vertex_project(rv);

  if (cell->vcache_size < PIGGY_VCACHE_MAX)
  {
    cell->vcache[cell->vcache_size++] = rv;
  }
  return rv;
}

/*
** Name: texcoord_fixup
** Desc: Fix up texture UV's so they are within optimal range. The PSONE
**       doesn't support texture wrapping, so we have to shift the UV's
**       withing range. Plus, excessive UV's increase fixpt errors. 
*/
void CODE_IN_IWRAM texcoord_fixup(polyclip_t p, int n)
{
  int i;
  int u = p[0]->u;
  int v = p[0]->v;

  for (i = 1; i < n; i++)
  {
    if (p[i]->u < u) u = p[i]->u;
    if (p[i]->v < v) v = p[i]->v;
  }
  for (i = n; i--;)
  {
    p[i]->u -= u & ~(i2f(64) - 1);
    p[i]->v -= v & ~(i2f(64) - 1);

    if (p[i]->u == i2f(64)) p[i]->u--;
    if (p[i]->v == i2f(64)) p[i]->v--;
  }
}

/*
** Name: draw_quad
** Desc: Renders a single quad/rectangle of a cell using the given texture
**       mapping mode and texture id. The mapping mode changes depending
**       of if this is a north-south wall, east-west wall or ceiling/floor.
*/
void CODE_IN_IWRAM draw_quad(piggy_t* piggy, int tid, Quad_t quad, int texgen)
{
  int u, uu, v, vv;
  polyclip_t p;

  tid = CLAMP(tid, 0, PIGGY_TEXTURE_MAX - 1); 

  p[0] = rotate_vector(piggy, &quad[0]);
  p[1] = rotate_vector(piggy, &quad[1]);
  p[2] = rotate_vector(piggy, &quad[2]);
  p[3] = rotate_vector(piggy, &quad[3]);

  u  = piggy->surfaces[tid].xsize;
  v  = piggy->surfaces[tid].ysize;
  uu = piggy->surfaces[tid].xpan;
  vv = piggy->surfaces[tid].ypan;

  switch (texgen)
  {
    case PIGGY_TEXGEN_WALL_LR:
    {
      p[0]->u = (quad[0].z + uu) << u; p[0]->v = (quad[0].y + vv) << v;
      p[1]->u = (quad[1].z + uu) << u; p[1]->v = (quad[1].y + vv) << v;
      p[2]->u = (quad[2].z + uu) << u; p[2]->v = (quad[2].y + vv) << v;
      p[3]->u = (quad[3].z + uu) << u; p[3]->v = (quad[3].y + vv) << v;
      break;
    }
    case PIGGY_TEXGEN_WALL_FB:
    {
      p[0]->u = (quad[0].x + uu) << u; p[0]->v = (quad[0].y + vv) << v;
      p[1]->u = (quad[1].x + uu) << u; p[1]->v = (quad[1].y + vv) << v;
      p[2]->u = (quad[2].x + uu) << u; p[2]->v = (quad[2].y + vv) << v;
      p[3]->u = (quad[3].x + uu) << u; p[3]->v = (quad[3].y + vv) << v;
      break;
    }
    case PIGGY_TEXGEN_FLAT:
    {
      p[0]->u = (quad[0].x + uu) << u; p[0]->v = (quad[0].z + vv) << v;
      p[1]->u = (quad[1].x + uu) << u; p[1]->v = (quad[1].z + vv) << v;
      p[2]->u = (quad[2].x + uu) << u; p[2]->v = (quad[2].z + vv) << v;
      p[3]->u = (quad[3].x + uu) << u; p[3]->v = (quad[3].z + vv) << v;
      break;
    }
    case PIGGY_TEXGEN_BASIC:
    {
      p[0]->u = i2f(64); p[0]->v = i2f(64);
      p[1]->u = i2f( 0); p[1]->v = i2f(64);
      p[2]->u = i2f( 0); p[2]->v = i2f( 0);
      p[3]->u = i2f(64); p[3]->v = i2f( 0);
      break;
    }
  }
#ifdef __PSONE__
  texcoord_fixup(p, 4);
#endif

  draw_texture(piggy, p, 4, tid);
}

void CODE_IN_IWRAM draw_wall(piggy_t* piggy, int tid, Quad_t quad, int top, int bot, int texgen)
{
  if (bot < top)
  {
#ifdef PIGGY_LOW_QUALITY_WALLS
    quad[0].y = quad[3].y = top; quad[1].y = quad[2].y = bot;
    draw_quad(piggy, tid, quad, texgen);
#else
    int i, j;

    for (i = bot; i < top; i = j)
    {                       
      j = fixtrunc(i + FIXONE);
      quad[0].y = quad[3].y = MIN(j, top); quad[1].y = quad[2].y = i;
      draw_quad(piggy, tid, quad, texgen);
    }
#endif    
  }
}

/******************************************************************************/

/*
** Name: draw_entity_as_sprite
** Desc: Draws a entity as a 2D sprite.
*/
void CODE_IN_IWRAM draw_entity_as_sprite(entity_t* e)
{
  sprite_t sprite = (rgb555_t*)e->visual.data;
  int w = sprite_width (sprite) * e->visual.width;
  int h = sprite_height(sprite) * e->visual.height;
  
  draw_sprite_di((piggy_t *)e->piggy, e->rx - w, e->ry + h, e->rx + w, e->ry - h,
    e->rz, sprite, e->visual.mode);
   
}

/*
** Name: draw_entity_as_model
** Desc: Draws a entity 3D model at the given location.
*/
void CODE_IN_IWRAM draw_entity_as_model(entity_t* entity)
{
  s8* model = (s8*)entity->visual.data;
  piggy_t* piggy = (piggy_t*)entity->piggy;
  matrx_t m;
  polyclip_t p;
  vec3_t verts[100];
  int i, j, nvertices;

  model++;

  matrix_rotate_object(m, f2i(entity->r), f2i(entity->p), f2i(entity->t));
  
  for (nvertices = *model++, i = 0; i < nvertices; i++)
  {
    int u = *model++;
    int v = *model++;
    int w = *model++;
    int x = f2i(m[0][0] * u + m[0][1] * v + m[0][2] * w) + entity->x - piggy->camera->x;
    int y = f2i(m[1][0] * u + m[1][1] * v + m[1][2] * w) + entity->z - piggy->camera->z;
    int z = f2i(m[2][0] * u + m[2][1] * v + m[2][2] * w) + entity->y - piggy->camera->y;
    
    verts[i].x = f2i(piggy->m[0][0] * x + piggy->m[0][1] * y + piggy->m[0][2] * z);
    verts[i].y = piggy->is2d ? y : f2i(piggy->m[1][0] * x + piggy->m[1][1] * y + piggy->m[1][2] * z);
    verts[i].z = f2i(piggy->m[2][0] * x + piggy->m[2][1] * y + piggy->m[2][2] * z);
  }
  for (i = *model++; i--;)
  {
    int npts = *model++;
    for (j = 0; j < npts; j++)
    {
      p[j] = PIGGY_VERTEX(piggy);
      *((vec3_t*)&p[j][0]) = verts[*model++];
      p[j]->u = i2f(*model++);
      p[j]->v = i2f(*model++);
      p[j]->l = i2f(48);
      vertex_project(p[j]);
    }
    draw_texture(piggy, p, npts, *model++);
  }
}

/******************************************************************************/
extern u8 leftOrRight;

void CODE_IN_IWRAM piggy_draw(piggy_t* piggy)
{
  cell_t* cell;
  Quad_t p;
  entity_t* e1;
  entity_t* e2;
  int i, nrays, ncells, rayangle, raywidth;

  CELL_UNMARK_ALL();

  piggy->is2d = !piggy->camera->p && !piggy->camera->r;

  s32 ox=piggy->camera->x, oy=piggy->camera->y, oz=piggy->camera->z;

  float slider=CONFIG_3D_SLIDERSTATE;
  float interaxial=slider*5.0f;

  if(!leftOrRight)interaxial=-interaxial;

  // for 3D effect
  piggy->camera->x -= (fixsin(f2i(piggy->camera->t)) >> 6) + (+((int)(interaxial*fixcos(f2i(piggy->camera->t)))) >> 6);
  piggy->camera->y -= (fixcos(f2i(piggy->camera->t)) >> 6) + (-((int)(interaxial*fixsin(f2i(piggy->camera->t)))) >> 6);
  
  matrix_rotate_world(piggy->m,
    -f2i(piggy->camera->r),
    -f2i(piggy->camera->p),
    -f2i(piggy->camera->t));

  raywidth = 16 + (ABS(piggy->camera->p) >> 13);

  rayangle = (PIGGY_RAY_MAX >> 1) * raywidth + f2i(piggy->camera->t);
  
  for (i = PIGGY_RAY_MAX; i--; rayangle -= raywidth)
  {
    piggy->ray_vel[i].x = fixsin(rayangle) >> PIGGY_RAY_QUALITY;
    piggy->ray_vel[i].y = fixcos(rayangle) >> PIGGY_RAY_QUALITY;
    piggy->ray_pos[i].x = piggy->camera->x;
    piggy->ray_pos[i].y = piggy->camera->y;
  }
  
  for (nrays = PIGGY_RAY_MAX, ncells = 0; nrays && ncells < PIGGY_CELL_MAX;)
  {
    for (i = PIGGY_RAY_MAX; i--;)
    {    
      if (piggy->ray_pos[i].x)
      {
        cell = &piggy->cells[f2i(piggy->ray_pos[i].y)][f2i(piggy->ray_pos[i].x)];
                  
        if (CELL_IS_SOLID(cell))
        {
          nrays--;
          piggy->ray_pos[i].x = 0;
        }
        else
        {
          if (!CELL_IS_MARKED(cell))
          {
            CELL_MARK(cell);
            cell->entities = 0;
            piggy->vis[ncells].x = f2i(piggy->ray_pos[i].x);
            piggy->vis[ncells].y = f2i(piggy->ray_pos[i].y);

            if (++ncells == PIGGY_CELL_MAX) break;
          }                           
          piggy->ray_pos[i].x += piggy->ray_vel[i].x;
          piggy->ray_pos[i].y += piggy->ray_vel[i].y;
        }
      }
    }
  }
  
  // Merge the entities into the world map.
  
  for (i = piggy->nentities; i--;)
  {
    e1 = &piggy->entities[i];

    if (e1 != piggy->camera && e1->visual.data)
    {
      cell = &piggy->cells[f2i(e1->y)][f2i(e1->x)];

      if (CELL_IS_MARKED(cell))
      {
        int x = e1->x - piggy->camera->x;
        int y = e1->z - piggy->camera->z;
        int z = e1->y - piggy->camera->y;

        e1->rx = f2i(piggy->m[0][0] * x + piggy->m[0][1] * y + piggy->m[0][2] * z);
        e1->ry = piggy->is2d ? y : f2i(piggy->m[1][0] * x + piggy->m[1][1] * y + piggy->m[1][2] * z);
        e1->rz = f2i(piggy->m[2][0] * x + piggy->m[2][1] * y + piggy->m[2][2] * z);

        if (!cell->entities || e1->z < cell->entities->rz)
        {
          e1->next = cell->entities; cell->entities = e1;
        }
        else
        {
          for (e2 = cell->entities; e2->next && e1->rz < e2->next->rz; e2 = e2->next);
          e1->next = e2->next; e2->next = e1;
        }
      }
    }
  }

  CELL_UNMARK_ALL();
  
  for (i = ncells; i--;)
  {
    int x1 = piggy->vis[i].x;
    int y1 = piggy->vis[i].y;

    cell_t* c0 = &piggy->cells[y1-1][x1];
    cell_t* c1 = &piggy->cells[y1  ][x1];
    cell_t* c2 = &piggy->cells[y1+1][x1];

    int top = c1[0].top;
    int bot = c1[0].bot;

    int x2 = i2f(x1 + 1);
    int y2 = i2f(y1 + 1);
    x1 = i2f(x1);
    y1 = i2f(y1);

    // Render Right Wall

    if (piggy->camera->x < x2)
    {
      p[0].x = p[1].x = p[2].x = p[3].x = x2;
      p[0].z = p[1].z = y1; p[2].z = p[3].z = y2;

      draw_wall(piggy, c1[1].wtx, p, MIN(c1[1].bot, top), bot, PIGGY_TEXGEN_WALL_LR);
      draw_wall(piggy, c1[1].wtx, p, top, MAX(c1[1].top, bot), PIGGY_TEXGEN_WALL_LR);
    }

    // Render Left Wall

    if (piggy->camera->x > x1)
    {
      p[0].x = p[1].x = p[2].x = p[3].x = x1;
      p[0].z = p[1].z = y2; p[2].z = p[3].z = y1;

      draw_wall(piggy, c1[-1].wtx, p, MIN(c1[-1].bot, top), bot, PIGGY_TEXGEN_WALL_LR);
      draw_wall(piggy, c1[-1].wtx, p, top, MAX(c1[-1].top, bot), PIGGY_TEXGEN_WALL_LR);
    }

    // Render Front Wall

    if (piggy->camera->y < y2)
    {
      p[0].z = p[1].z = p[2].z = p[3].z = y2;
      p[0].x = p[1].x = x2; p[2].x = p[3].x = x1;

      draw_wall(piggy, c2[0].wtx, p, MIN(c2[0].bot, top), bot, PIGGY_TEXGEN_WALL_FB);
      draw_wall(piggy, c2[0].wtx, p, top, MAX(c2[0].top, bot), PIGGY_TEXGEN_WALL_FB);
    }

    // Render Back Wall

    if (piggy->camera->y > y1)
    {
      p[0].z = p[1].z = p[2].z = p[3].z = y1;
      p[0].x = p[1].x = x1; p[2].x = p[3].x = x2;

      draw_wall(piggy, c0[0].wtx, p, MIN(c0[0].bot, top), bot, PIGGY_TEXGEN_WALL_FB);
      draw_wall(piggy, c0[0].wtx, p, top, MAX(c0[0].top, bot), PIGGY_TEXGEN_WALL_FB);
    }

    // Render cell ceiling and floor
    
    if (!CELL_IS_SOLID(c1))
    {
       p[0].x = p[3].x = x1; p[1].x = p[2].x = x2;

      // Cell Bottom (Floor)

      if (piggy->camera->z > bot)
      {
        p[0].y = p[1].y = p[2].y = p[3].y = bot;
        p[0].z = p[1].z = y2; p[2].z = p[3].z = y1;

        draw_quad(piggy, c1->btx, p, PIGGY_TEXGEN_FLAT);
      }

      // Cell Top (Ceiling)

      if (piggy->camera->z < top)
      {
        p[0].y = p[1].y = p[2].y = p[3].y = top;
        p[0].z = p[1].z = y1; p[2].z = p[3].z = y2;

        draw_quad(piggy, c1->ttx, p, PIGGY_TEXGEN_FLAT);
      }
    }
    for (e1 = c1->entities; e1; e1 = e1->next)
    {
      if (e1->ondraw) e1->ondraw(e1); else draw_entity_as_sprite(e1);
    }
  }

  if (piggy->overlay)
  {
    draw_sprite_di(piggy, -i2f(1), i2f(1), i2f(1), -i2f(1), 200, piggy->overlay, 2);
  }

  piggy->camera->x=ox;
  piggy->camera->y=oy;
  piggy->camera->z=oz;
}

/******************************************************************************/

