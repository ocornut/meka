//-----------------------------------------------------------------------------
// MEKA - video_m2.c
// TMS9918 Video Modes Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "vdp.h"

//-----------------------------------------------------------------------------

static byte Sprites_On_Line [192 + 32];

const RGB TMS9918_Palette [16] =
 {
   /* 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xC0, 0x20,
   0x60, 0xE0, 0x60, 0x20, 0x20, 0xE0, 0x40, 0x60, 0xE0,
   0xA0, 0x20, 0x20, 0x40, 0xC0, 0xE0, 0xE0, 0x20, 0x20,
   0xE0, 0x60, 0x60, 0xC0, 0xC0, 0x20, 0xC0, 0xC0, 0x80,
   0x20, 0x80, 0x20, 0xC0, 0x40, 0xA0, 0xA0, 0xA0, 0xA0,
   0xE0, 0xE0, 0xE0, */
   { 0x00, 0x00, 0x00, 0 }, /*  0: Transparent   */
   { 0x00, 0x00, 0x00, 0 }, /*  1: Black         */
   { 0x08, 0x30, 0x08, 0 }, /*  2: Medium Green  */
   { 0x18, 0x38, 0x18, 0 }, /*  3: Light Green   */
   { 0x08, 0x08, 0x38, 0 }, /*  4: Dark Blue     */
   { 0x10, 0x18, 0x38, 0 }, /*  5: Light Blue    */
   { 0x28, 0x08, 0x08, 0 }, /*  6: Dark Red      */
   { 0x10, 0x30, 0x38, 0 }, /*  7: Cyan          */
   { 0x38, 0x08, 0x08, 0 }, /*  8: Medium Red    */
   { 0x38, 0x18, 0x18, 0 }, /*  9: Light Red     */
   { 0x30, 0x30, 0x08, 0 }, /* 10: Dark Yellow   */
   { 0x30, 0x30, 0x20, 0 }, /* 11: Light Yellow  */
   { 0x08, 0x20, 0x08, 0 }, /* 12: Dark Green    */
   { 0x30, 0x10, 0x28, 0 }, /* 13: Magenta       */
   { 0x28, 0x28, 0x28, 0 }, /* 14: Grey          */
   { 0x38, 0x38, 0x38, 0 }  /* 15: White         */
 };

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TMS9918_Palette_Set ()
// Setup TMS9918 palette and fixed references
//-----------------------------------------------------------------------------
void    TMS9918_Palette_Set (void)
{
  int   i;

  // Set TMS9918 emulation palette
  Palette_SetColor_Emulation (0, TMS9918_Palette[sms.VDP[7] & 15]);
  for (i = 1; i < 16; i ++)
     Palette_SetColor_Emulation (i, TMS9918_Palette[i]);

  // Set all references linearly from 0 to 15
  for (i = 0; i < 16; i++)
     Palette_SetColor_Reference_Force (i, i);
}

// DISPLAY TEXT MODE 0 SCREEN -------------------------------------------------
void    Display_Text_0 (void)
{
 int    i, j, k;

 byte   *p2;
 byte   *tile_n = BACK_AREA;
 byte   *Table = Table_Mode_0 + ((sms.VDP[7] >> 4) * 2048);

 for (j = 0; j < (24 * 8); j += 8)
     {
     for (k = 0; k < 8; k ++)
         {
         memset (&screenbuffer->line [j + k] [0], Border_Color, 8);
         memset (&screenbuffer->line [j + k] [248], Border_Color, 8);
         }
     for (i = 8; i < (40 * 6) + 8; i += 6)
         {
         p2 = SG_BACK_TILE + (*tile_n * 8);
         for (k = 0; k < 8; k ++)
             {
             memcpy (&screenbuffer->line [j + k] [i], Table + (*p2 << 3), 6);
             p2 ++;
             }
         tile_n ++;
         }
     }
}

// DISPLAY BACKGROUND VIDEO MODE 1 --------------------------------------------
void    Display_Background_1 (void)
{
 int    i, j, j2;
 int    x, y = 0;
 byte   *p1,                           //-- Tile Data ---------//
        *p2,                           //-- Color Table -------//
        *p3,                           //-- Screen Bitmap -----//
        *tile_n = BACK_AREA;           //-- Tile Table --------//
 byte   c1, c2;

 // DRAW ALL TILES ------------------------------------------------------------
 for (i = 0; i < 24; i++)
     {
     x = 0;
     for (j = 0; j < 32; j ++)
         {
         // DRAW ONE TILE -------------------------------------------------
         p1 = SG_BACK_TILE + (*tile_n * 8);
         p2 = SG_BACK_COLOR + (*tile_n / 8);
         c1 = (*p2 >> 4);
         c2 = (*p2) & 15;
         for (j2 = 0; j2 < 8; j2 ++) //-- Draw one Tile -------------------
             {
             p3 = &screenbuffer->line [y + j2] [x];
             p3 [0] = (*p1 & 128) ? c1 : c2;
             p3 [1] = (*p1 & 64 ) ? c1 : c2;
             p3 [2] = (*p1 & 32 ) ? c1 : c2;
             p3 [3] = (*p1 & 16 ) ? c1 : c2;
             p3 [4] = (*p1 & 8  ) ? c1 : c2;
             p3 [5] = (*p1 & 4  ) ? c1 : c2;
             p3 [6] = (*p1 & 2  ) ? c1 : c2;
             p3 [7] = (*p1 & 1  ) ? c1 : c2;
             p1 ++; //-------------------- Increment pointer to Tile Data --
             }
         tile_n ++;
         x += 8;
         }
     y += 8;
     }
}

// DISPLAY BACKGROUND VIDEO MODE 2 --------------------------------------------
void    Display_Background_2 (void)
{
 int    i, j, j2, k;

 byte   *col_base;                     //-- Color Table Base --//
 byte   *tile_base;                    //-- Tile Data Base ----//
 byte   *p1,                           //-- Tile Data ---------//
        *p2,                           //-- Color Table -------//
        *p3,                           //-- Screen Bitmap -----//
        *p4 = BACK_AREA;               //-- Tile Table --------//

 register int x, y;
 register byte cc, c1, c2;

 int mask = sms.VDP [4] & 3;

 // DRAW ALL TILES ------------------------------------------------------------
 y = 0;
 for (k = 0; k < 3; k ++)
     {
     col_base = SG_BACK_COLOR + ((k & mask) * 0x800);
     tile_base = SG_BACK_TILE + ((k & mask) * 0x800);
     for (i = 0; i < 8; i++)
         {
         x = 0;
         for (j = 0; j < 32; j++)
             {
             // DRAW ONE TILE -------------------------------------------------
             p1 = tile_base + (*p4 * 8);
             p2 = col_base + (*p4++ * 8);
             for (j2 = 0; j2 < 8; j2 ++) //-- Draw one Tile -------------------
                 {
                 cc = (*p1 ++);
                 c1 = (*p2 >> 4);
                 c2 = (*p2 ++) & 15;
                 p3 = screenbuffer->line [y + j2] + x;
                 p3 [0] = (cc & 128) ? c1 : c2;
                 p3 [1] = (cc & 64 ) ? c1 : c2;
                 p3 [2] = (cc & 32 ) ? c1 : c2;
                 p3 [3] = (cc & 16 ) ? c1 : c2;
                 p3 [4] = (cc & 8  ) ? c1 : c2;
                 p3 [5] = (cc & 4  ) ? c1 : c2;
                 p3 [6] = (cc & 2  ) ? c1 : c2;
                 p3 [7] = (cc & 1  ) ? c1 : c2;
                 }
             x += 8;
             }
         y += 8;
         }
     }
}

// DISPLAY BACKGROUND VIDEO MODE 3 --------------------------------------------
void    Display_Background_3 (void)
{
 int    x, y, z;
 byte   *p_scr;
 byte   *tiles_data;
 byte   *pattern = BACK_AREA;

 for (y = 0; y < 192; y += 32)
     {
     for (x = 0; x < 256; x += 8)
         {
         tiles_data = SG_BACK_TILE + (*pattern++ * 8);
         for (z = 0; z < 8; z ++)
             {
             int c1 = *tiles_data >> 4;
             int c2 = *tiles_data & 0x0F;
             c1 |= (c1 << 8) | (c1 << 16) | (c1 << 24);
             c2 |= (c2 << 8) | (c2 << 16) | (c2 << 24);
             p_scr = screenbuffer->line [y + 0] + x;
             ((int *)p_scr)[0] = c1;
             ((int *)p_scr)[1] = c2;
             p_scr = screenbuffer->line [y + 1] + x;
             ((int *)p_scr)[0] = c1;
             ((int *)p_scr)[1] = c2;
             p_scr = screenbuffer->line [y + 2] + x;
             ((int *)p_scr)[0] = c1;
             ((int *)p_scr)[1] = c2;
             p_scr = screenbuffer->line [y + 3] + x;
             ((int *)p_scr)[0] = c1;
             ((int *)p_scr)[1] = c2;
             y += 4;
             tiles_data++;
             }
         y -= 32;
         }
     pattern += 96;
     }
}

// DRAW A MONOCHROME TILE (NO TRANSPARENCY) -----------------------------------
void    Draw_Tile_Mono (BITMAP *where, byte *p1, int x, int y, byte fcolor, byte bgcolor)
{
 int    i;
 byte   cc;
 byte   *p2;

 for (i = 0; i < 8; i ++)
     {
     cc = *p1++;
     p2 = &where->line [y ++] [x];
     *p2 ++ = (cc & 128) ? fcolor : bgcolor;
     *p2 ++ = (cc & 64)  ? fcolor : bgcolor;
     *p2 ++ = (cc & 32)  ? fcolor : bgcolor;
     *p2 ++ = (cc & 16)  ? fcolor : bgcolor;
     *p2 ++ = (cc & 8)   ? fcolor : bgcolor;
     *p2 ++ = (cc & 4)   ? fcolor : bgcolor;
     *p2 ++ = (cc & 2)   ? fcolor : bgcolor;
     *p2    = (cc & 1)   ? fcolor : bgcolor;
     }
}

// DRAW A MAGNIFIED MONOCHROME SPRITE TILE ------------------------------------
void    Draw_Sprite_Mono_Double (BITMAP *where, byte *p1, int x, int y, byte fcolor)
{
 int    j;
 byte   *p2;

 if (fcolor & 128) x -= 32;
 fcolor &= 15;
 if (fcolor == 0) return;

 for (j = 0; j < 8; j ++, p1 ++)
     {
     if (y < 0 || y > 190)
        {
        y += 2;
        continue;
        }
     if (!(Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
        {
        p2 = &where->line [y] [x]; // First line
        if  (*p1 & 128) { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 64)  { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 32)  { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 16)  { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 8)   { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 4)   { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 2)   { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 1)   { *p2++ = fcolor; *p2   = fcolor; }
        }
     // if (Sprites_On_Line [y] == 5) { }
     y += 1;
     if (!(Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
        {
        p2 = &where->line [y] [x]; // First line
        if  (*p1 & 128) { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 64)  { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 32)  { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 16)  { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 8)   { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 4)   { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 2)   { *p2++ = fcolor; *p2++ = fcolor; } else { p2 += 2; }
        if  (*p1 & 1)   { *p2++ = fcolor; *p2   = fcolor; }
        }
     // if (Sprites_On_Line [y] == 5) { }
     y += 1;
     }
}

// DRAW A MONOCHROME SPRITE TILE ----------------------------------------------
void    Draw_Sprite_Mono (BITMAP *where, byte *p1, int x, int y, byte fcolor)
{
 int    j;
 byte   *p2;

 if (fcolor & 128) x -= 32;
 fcolor &= 15;
 if (fcolor == 0) return;

 for (j = 0; j < 8; j ++, y ++, p1 ++)
     {
     if (y < 0 || y > 191)
        continue;
     if (!(Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
        {
        p2 = &where->line [y] [x];
        if  (*p1 & 128) *p2  =  fcolor;  p2++;
        if  (*p1 & 64)  *p2  =  fcolor;  p2++;
        if  (*p1 & 32)  *p2  =  fcolor;  p2++;
        if  (*p1 & 16)  *p2  =  fcolor;  p2++;
        if  (*p1 & 8)   *p2  =  fcolor;  p2++;
        if  (*p1 & 4)   *p2  =  fcolor;  p2++;
        if  (*p1 & 2)   *p2  =  fcolor;  p2++;
        if  (*p1 & 1)   *p2  =  fcolor;
        }
     // if (Sprites_On_Line [y] == 5) { }
     }
}

static const int Table_Height [4] = { 8, 16, 16, 32 };
static const int Table_Mask [4] =   { 0xFF, 0xFF, 0xFC, 0xFC };

// DISPLAY SPRITES IN VIDEO MODE 1/2/3 ----------------------------------------
void    Display_Sprites_1_2_3 (void)
{
 byte   *k;
 int    i, j;
 int    x, y;
 int    Sprite_Mode = Sprites_Double | Sprites_16x16;
 int    Mask = Table_Mask [Sprite_Mode];
 int    Height = Table_Height [Sprite_Mode];

 // No sprites in Video Mode 0 (Text Mode)
 if (tsms.VDP_VideoMode == 0)
    return;

 memset (Sprites_On_Line, 0, 192 + 32);
 for (i = 0; i < 32 * 4; i += 4)
     {
     y = SPR_AREA [i];
     if ((y ++) == 0xD0) break;
     if (y > 0xD0) y -= 0xFF;
     for (j = y; j < y + Height; j++)
         if (j >= 0)
            Sprites_On_Line [j]++;
     }
 i -= 4;

 // Display sprites -----------------------------------------------------------
 while (i >= 0)
    {
    // Calculate vertical position and handle special meanings ----------------
    y = SPR_AREA [i];
    if ((y ++) == 0xD0) break;
    if (y > 0xD0) y -= 0x100;
    // Calculate horizontal position ------------------------------------------
    x = SPR_AREA [i + 1];
    // Calculate tile starting address in VRAM --------------------------------
    k = ((SPR_AREA[i + 2] & Mask) * 8) + SPR_TILE;
    switch (Sprite_Mode)
      {
      // 8x8 (used in: Sokouban)
      case 0: //--------- video buffer -- address -- x position -- y position -- color
              Draw_Sprite_Mono (screenbuffer, k, x, y, SPR_AREA[i + 3]);
              break;
      // 16x16 - 8x8 Doubled (used in: ?)
      case 1: //--------- video buffer -- address -- x position -- y position -- color
              Draw_Sprite_Mono_Double (screenbuffer, k, x, y, SPR_AREA[i + 3]);
              break;
      // 16x16 (used in most games)
      case 2: //-------------- video buffer - address -- x position --- y position --- color -----
              Draw_Sprite_Mono (screenbuffer, k,      x,     y,     SPR_AREA[i + 3]);
              Draw_Sprite_Mono (screenbuffer, k + 8,  x,     y + 8, SPR_AREA[i + 3]);
              Draw_Sprite_Mono (screenbuffer, k + 16, x + 8, y,     SPR_AREA[i + 3]);
              Draw_Sprite_Mono (screenbuffer, k + 24, x + 8, y + 8, SPR_AREA[i + 3]);
              break;
      case 3: //------------------- video buffer -- address ---- x position ---- y position --- color ----
              Draw_Sprite_Mono_Double (screenbuffer, k,      x,      y,      SPR_AREA[i + 3]);
              Draw_Sprite_Mono_Double (screenbuffer, k + 8,  x,      y + 16, SPR_AREA[i + 3]);
              Draw_Sprite_Mono_Double (screenbuffer, k + 16, x + 16, y,      SPR_AREA[i + 3]);
              Draw_Sprite_Mono_Double (screenbuffer, k + 24, x + 16, y + 16, SPR_AREA[i + 3]);
              break;
      }
    i -= 4;
    // Decrease Sprites_On_Line values ----------------------------------------
    for (j = y; j < y + Height; j++) if (j >= 0) Sprites_On_Line [j]--;
    }
}

void    Refresh_Modes_0_1_2_3 (void)
{
    // Display Background
    if (opt.Layer_Mask & LAYER_BACKGROUND)
    {
        if (Display_ON)
        {
            switch (tsms.VDP_VideoMode)
            {
            case 0: Display_Text_0 (); break;
            case 1: Display_Background_1 (); break;
            case 2: Display_Background_2 (); break;
            case 3: Display_Background_3 (); break;
            }
        }
        else
        {
            // Clear screen
            clear_to_color (screenbuffer, Border_Color);
        }
    }
    else
    {
        // Clear screen with yellow-ish color
        clear_to_color (screenbuffer, 95);  // see video_m5.c [20050403] For sprite ripping
    }
    // Display Sprites
    if ((opt.Layer_Mask & LAYER_SPRITES) && Display_ON)
    {
        Display_Sprites_1_2_3 ();
    }
}

/*
**     _____
**    |     |
**  1 |   __|__
**    |__|__|  |
**       |     | 2
**       |_____|
**
*/
// FIXME: Zoomed sprites are actually not handled well in the collision tests
/*
void    Check_Sprites_Collision_Modes_1_2_3 (void)
{
 int    Mask = Table_Mask [Sprites_Double | Sprites_16x16];
 int    Size = Table_Height [Sprites_Double | Sprites_16x16];
 byte   Sprite_Last;
 byte   n1, n2, dx, dy;
 byte   *SprSrc, *SprDst;
 byte   *TileSrc, *TileDst, *TileTmp;

 Sprite_Last = 0;
 SprSrc = SPR_AREA;
 while (Sprite_Last < 32 && SprSrc[0] != 208)
    { Sprite_Last++; SprSrc += 4; }

 for (n1 = 0, SprSrc = SPR_AREA; n1 < Sprite_Last; n1++, SprSrc += 4)
     {
     // Skip if this sprite is not drawn
     if ((SprSrc[3] & 0x0F) == 0)
        continue;

     for (n2 = n1 + 1, SprDst = SprSrc + 4; n2 < Sprite_Last; n2++, SprDst += 4)
        {
        // Skip if this sprite is not drawn
        if ((SprDst[3] & 0x0F) == 0)
           continue;

        // Compute delta-y between the two sprites, skip if they cannot overlap
        dy = SprSrc[0] - SprDst[0];
        if ((dy >= Size) && (dy <= 256 - Size))
           continue;

        // Compute delta-x between the two sprites, skip if they cannot overlap
        dx = SprSrc[1] - SprDst[1];
        if ((dx >= Size) && (dx <= 256 - Size))
           continue;

        // Prepare pointers to the first tile line of each sprite
        TileSrc = SPR_TILE + ((long)(SprSrc[2] & Mask) << 3);
        TileDst = SPR_TILE + ((long)(SprDst[2] & Mask) << 3);

        if (dy < Size)
           {
           TileDst += dy;
           }
        else
           {
           dy = 256 - dy;
           TileSrc += dy;
           }
        if (dx > 256 - Size)
           {
           dx = 256 - dx;
           TileTmp = TileSrc; TileSrc = TileDst; TileDst = TileTmp;
           }

        // Finally, attempt to compare actual sprites pixels
        while (dy < Size)
           {
           if (Size == 8)
              {
              if (*TileDst & (*TileSrc >> dx))
                 break;
              }
           else
              {
              if ( (((word)*TileDst << 8) + *(TileDst + 16))
               & ((((word)*TileSrc << 8) + *(TileSrc + 16)) >> dx) )
                 break;
              }
           dy++;
           TileSrc++;
           TileDst++;
           }

        if (dy < Size)
           {
           //Msg (MSGT_USER, "Sprites %d & %d Collide", n1, n2);
           sms.VDP_Status |= VDP_STATUS_SpriteCollision; // Set VDP Status Collision Bit
           return;
           }
        }
    }
}
*/

// FIXME: Zoomed sprites are actually not handled well in the collision tests
void    Check_Sprites_Collision_Modes_1_2_3_Line (int line)
{
  int   mask = Table_Mask [Sprites_Double | Sprites_16x16];
  int   size = Table_Height [Sprites_Double | Sprites_16x16];

  int   src_n,     dst_n;
  int   src_y,     dst_y;
  byte *src_spr,  *dst_spr;
  byte *src_tile, *dst_tile;

  int   delta_x;
  int   delta_y;

  for (src_n = 0, src_spr = SPR_AREA, src_y = src_spr[0];
       src_n < 31 && src_y != 208;
       src_n++, src_spr += 4, src_y = src_spr[0])
     {
     // Skip if this sprite does not cover current line
     if (line < src_y || line >= src_y + size)
        continue;

     // Skip if this sprite is not shown (color 0)
     if ((src_spr[3] & 0x0F) == 0)
        continue;

     // Now for all other sprites to compare with...
     for (dst_n = src_n + 1, dst_spr = src_spr + 4, dst_y = dst_spr[0];
          dst_n < 32 && dst_y != 208;
          dst_n++, dst_spr += 4, dst_y = dst_spr[0])
        {
        // Skip if this sprite does not cover current line
        if (line < dst_y || line >= dst_y + size)
           continue;

        // Skip if this sprite is not shown (color 0)
        if ((dst_spr[3] & 0x0F) == 0)
           continue;

        // Compare delta_y
        delta_y = src_y - dst_y;
        if (delta_y >= size || delta_y <= -size)
           Msg (MSGT_USER, "delta_y = %d", delta_y);

        // Compute delta_x, skip if the sprites cannot overlap
        delta_x = src_spr[1] - dst_spr[1];
        if (delta_x >= size || delta_x <= -size)
           continue;

        // Prepare pointers to the first tile of each sprite
        src_tile = SPR_TILE + ((long)(src_spr[2] & mask) << 3);
        dst_tile = SPR_TILE + ((long)(dst_spr[2] & mask) << 3);

        // Offset those pointers to the first tile line
        if (delta_y > 0)
           dst_tile += delta_y;
        else
        if (delta_y < 0)
        {
           delta_y = -delta_y;
           src_tile += delta_y;
        }

        // Inverse sprites if delta_x < 0 for the purpose of the comparaison
        if (delta_x < 0)
           {
           byte *tmp = src_tile;
           src_tile  = dst_tile;
           dst_tile  = tmp;
           delta_x = -delta_x;
           }

        // Finally compare actual sprites pixels
        if (size == 8)
           {
           while (delta_y < 8)
              {
              if (*dst_tile++ & (*src_tile++ >> delta_x))
                 break;
              delta_y++;
              }
           }
        else
           {
           while (delta_y < size)
              {
              if ( (((word)*dst_tile << 8) | *(dst_tile + 16))
                & ((((word)*src_tile << 8) | *(src_tile + 16)) >> delta_x) )
                 break;
              delta_y++;
              dst_tile++;
              src_tile++;
              }
           }

        if (delta_y < size)
           {
           // Msg (MSGT_USER, "Sprites %d & %d Collide", src_n, dst_n);
           sms.VDP_Status |= VDP_STATUS_SpriteCollision; // Set VDP Status Collision Bit
           return;
           }

        }
     }
}

//-----------------------------------------------------------------------------

