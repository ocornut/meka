//-----------------------------------------------------------------------------
// Meka - video_m5.c
// SMS/GG Video Mode Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fskipper.h"
#include "vdp.h"

//-----------------------------------------------------------------------------

       int      Sprites_on_Line;
       int      Do_Collision;
static byte     Sprites_Draw_Mask [SMS_RES_X + 16];

#define         Sprites_Collision_Table_Len (SMS_RES_X + 32)
int             Sprites_Collision_Table_Start[Sprites_Collision_Table_Len + 32];
int            *Sprites_Collision_Table = Sprites_Collision_Table_Start + 16;

// REDRAW A SCREEN LINE -------------------------------------------------------
void    Refresh_Line_5 (void)
{
 #ifdef CLOCK
  int clock_save = Clock[CLOCK_GFX_BACK].time;
 #endif

 // Point to current video line -----------------------------------------------
 GFX_Line = screenbuffer->line [tsms.VDP_Line];

 if (fskipper.Show_Current_Frame == YES)
    {
    #ifdef CLOCK
       Clock_Start (CLOCK_GFX_BACK);
    #endif

    // Display Background & Foreground -------------------------------------------
    if ((opt.Layer_Mask & LAYER_BACKGROUND) && Display_ON)

        // Test 'background' bit only in SMS mode. This is likely not the good
        // way to fix the infamous Ys bug happening on a GG but not a SMS (!)
        // but at least, and temporaly, this work.
        // Also note Cosmic Spacehead is not setting the bit.
        /* && (cur_drv->id != DRV_SMS || Background_ON) */
       Display_BackGround_Line_5 ();
    else // If Display set to OFF ------------------------------------------------
       {
       memset (Sprites_Draw_Mask, 0, 256);
       if (!(opt.Layer_Mask & LAYER_BACKGROUND))
            memset (GFX_Line, 95, 256);//Border_Color, 256);    // 95 is some GUI color entry, useful for ripping sprites
       else
            memset (GFX_Line, Palette_Refs[16 + (sms.VDP[7] & 15)], 256);
       //memset (GFX_Line, (cur_drv->id == DRV_SMS || cur_drv->id == DRV_GG) ? Palette_Refs[16 + (sms.VDP[7] & 15)] : Border_Color, 256);
       }

    #ifdef CLOCK
       Clock_Stop (CLOCK_GFX_BACK);
       Clock [CLOCK_GFX_BACK].time += clock_save;
       clock_save = Clock[CLOCK_GFX_SPRITES].time;
       Clock_Start (CLOCK_GFX_SPRITES);
    #endif

    Refresh_Sprites_5 (Display_ON && (opt.Layer_Mask & LAYER_SPRITES));

    #ifdef CLOCK
       Clock_Stop (CLOCK_GFX_SPRITES);
    #endif

    // Mask left columns with black if necessary
    if (Mask_Left_8)
       {
       ((int *)GFX_Line)[0] = Border_Color_x4;
       ((int *)GFX_Line)[1] = Border_Color_x4;
       }
    }
 else
    {
    // Only update collision if frame is being skipped
    Refresh_Sprites_5 (FALSE);
    }
}

// DISPLAY A BACKGROUND LINE --------------------------------------------------
void    Display_BackGround_Line_5_C (void)
{
 byte  *DstBuf;
 byte  *SrcMap;
 byte  *Sprite_Mask;
 int    x, x_scroll, x_ignore_vscroll, y;
 int    tile_x, tile_line;

 //---- X computations --------------------------------------------------------
 x_scroll = ((Top_No_Scroll) && (tsms.VDP_Line < 16)) ? 0 : X_Scroll;
 x = x_scroll & 7;  // x = x_scroll % 8
 x_scroll >>= 3;    // x_scroll /= 8
 if (x_scroll == 0)
    x_scroll = 32;

 // Set destination address ---------------------------------------------------
 DstBuf = GFX_Line;
 Sprite_Mask = Sprites_Draw_Mask;

 // Fill first 'non existents' pixels -----------------------------------------
 if (x > 0)
    {
    // If left column is masked, no need to care about what we put there
    // and not even the Sprite_Mask (it can stay undefined!)
    if (Mask_Left_8)
       {
       DstBuf += x;
       Sprite_Mask += x;
       }
    else
       {
       int  i;
       byte color = Palette_Refs [0];
       for (i = x; i > 0; i--)
           {
           *DstBuf++ = color;
           *Sprite_Mask++ = 0;
           }
       }
    }

 //---- Y computations --------------------------------------------------------
 y = tsms.VDP_Line + Y_Scroll;
 if (Wide_Screen_28)
    {
    y &= 255; // y %= 256, Wrap at 256
    y -= 32;
    }
 else
    {
    y %= 224;
    }

 // Bit 0 of Register 2 act as a mask on the 315-5124 -------------------------
 if (cur_machine.VDP.model == VDP_MODEL_315_5124)
    if ((sms.VDP[2] & 1) == 0)
        y &= 127;

 // Calculate source address & line in tile -----------------------------------
 SrcMap = BACK_AREA + ((y & 0xFFFFFFF8) * 8) + (2 * (32 - x_scroll));
 tile_line = (y & 0x07) * 8;

 // Calculate position where vertical scrolling will be ignored ---------------
 x_ignore_vscroll = (Right_No_Scroll) ? 23 : -1;

 // Drawing loop --------------------------------------------------------------
 tile_x = 0;
 while (tile_x < 32)
    {
    // Part of Horizontal Line not refreshed in Game Gear mode ----------------
    if ((cur_drv->id != DRV_GG) || ((tile_x > 4) && (tile_x < 26)))
       {
       int   tile_n;
       byte  Attr, Color;
       byte *Pal, *Src;

       // Draw tile line ------------------------------------------------------
       Attr = *(SrcMap + 1);
       tile_n = *((word *)SrcMap) & 511;
       if (tgfx.Tile_Dirty [tile_n] & TILE_DIRTY_DECODE)
          {
          Decode_Tile (tile_n);
          tgfx.Tile_Dirty [tile_n] = TILE_DIRTY_REDRAW;
          }

       Pal = (Attr & 0x08) ? &Palette_Refs[16] : &Palette_Refs[0];
       Src = tgfx.Tile_Decoded[tile_n] + ((Attr & 0x04) ? (7 * 8) - tile_line : tile_line);

       switch (Attr & 0x12)
          {
          // 0 - Not Flipped - Background Tile
          case  0: DstBuf[0] = Pal[Src[0]];
                   DstBuf[1] = Pal[Src[1]];
                   DstBuf[2] = Pal[Src[2]];
                   DstBuf[3] = Pal[Src[3]];
                   DstBuf[4] = Pal[Src[4]];
                   DstBuf[5] = Pal[Src[5]];
                   DstBuf[6] = Pal[Src[6]];
                   DstBuf[7] = Pal[Src[7]];
                   ((int *)Sprite_Mask)[0] = 0;
                   ((int *)Sprite_Mask)[1] = 0;
                   break;
          // 1 -  X Flipped - Background Tile
          case  2: DstBuf[0] = Pal[Src[7]];
                   DstBuf[1] = Pal[Src[6]];
                   DstBuf[2] = Pal[Src[5]];
                   DstBuf[3] = Pal[Src[4]];
                   DstBuf[4] = Pal[Src[3]];
                   DstBuf[5] = Pal[Src[2]];
                   DstBuf[6] = Pal[Src[1]];
                   DstBuf[7] = Pal[Src[0]];
                   ((int *)Sprite_Mask)[0] = 0;
                   ((int *)Sprite_Mask)[1] = 0;
                   break;
          // 16 - Not Flipped - Foreground Tile
          case 16: Color = Src[0]; DstBuf[0] = Pal[Color]; Sprite_Mask[0] = (Color ? 1 : 0);
                   Color = Src[1]; DstBuf[1] = Pal[Color]; Sprite_Mask[1] = (Color ? 1 : 0);
                   Color = Src[2]; DstBuf[2] = Pal[Color]; Sprite_Mask[2] = (Color ? 1 : 0);
                   Color = Src[3]; DstBuf[3] = Pal[Color]; Sprite_Mask[3] = (Color ? 1 : 0);
                   Color = Src[4]; DstBuf[4] = Pal[Color]; Sprite_Mask[4] = (Color ? 1 : 0);
                   Color = Src[5]; DstBuf[5] = Pal[Color]; Sprite_Mask[5] = (Color ? 1 : 0);
                   Color = Src[6]; DstBuf[6] = Pal[Color]; Sprite_Mask[6] = (Color ? 1 : 0);
                   Color = Src[7]; DstBuf[7] = Pal[Color]; Sprite_Mask[7] = (Color ? 1 : 0);
                   break;
          // 18 - X Flipped - Foreground Tile
          case 18: Color = Src[7]; DstBuf[0] = Pal[Color]; Sprite_Mask[0] = (Color ? 1 : 0);
                   Color = Src[6]; DstBuf[1] = Pal[Color]; Sprite_Mask[1] = (Color ? 1 : 0);
                   Color = Src[5]; DstBuf[2] = Pal[Color]; Sprite_Mask[2] = (Color ? 1 : 0);
                   Color = Src[4]; DstBuf[3] = Pal[Color]; Sprite_Mask[3] = (Color ? 1 : 0);
                   Color = Src[3]; DstBuf[4] = Pal[Color]; Sprite_Mask[4] = (Color ? 1 : 0);
                   Color = Src[2]; DstBuf[5] = Pal[Color]; Sprite_Mask[5] = (Color ? 1 : 0);
                   Color = Src[1]; DstBuf[6] = Pal[Color]; Sprite_Mask[6] = (Color ? 1 : 0);
                   Color = Src[0]; DstBuf[7] = Pal[Color]; Sprite_Mask[7] = (Color ? 1 : 0);
                   break;
          } // switch
       }

    if (tile_x == x_ignore_vscroll)
       {
       if (Wide_Screen_28)
          {
          SrcMap = BACK_AREA + (((tsms.VDP_Line - 32) & 0xFFFFFFF8) * 8) + ((2 * (32 - x_scroll + tile_x)) & 63);
          tile_line = ((tsms.VDP_Line - 32) & 0x07) * 8;
          }
       else
          {
          SrcMap = BACK_AREA + ((tsms.VDP_Line & 0xFFFFFFF8) * 8) + ((2 * (32 - x_scroll + tile_x)) & 63);
          tile_line = (tsms.VDP_Line & 0x07) * 8;
          }
       }

    if (++tile_x == x_scroll)
       {
       SrcMap -= 62;
       }
    else
       {
       SrcMap += 2;
       }

    DstBuf += 8;
    Sprite_Mask += 8;
    // x += 8 // It is not necessary to maintain 'x' in the loop
    }
}

// PROCESS COLLISION FOR A SPRITE LINE ----------------------------------------
INLINE void     Sprite_Collide_Line_C (byte *p_src, int x)
{
    int *       p_collision_table;

    p_collision_table = &Sprites_Collision_Table [x];
    if (p_src[0]) { if ((p_collision_table[0])++ > 0) goto collide; }
    if (p_src[1]) { if ((p_collision_table[1])++ > 0) goto collide; }
    if (p_src[2]) { if ((p_collision_table[2])++ > 0) goto collide; }
    if (p_src[3]) { if ((p_collision_table[3])++ > 0) goto collide; }
    if (p_src[4]) { if ((p_collision_table[4])++ > 0) goto collide; }
    if (p_src[5]) { if ((p_collision_table[5])++ > 0) goto collide; }
    if (p_src[6]) { if ((p_collision_table[6])++ > 0) goto collide; }
    if (p_src[7]) { if ((p_collision_table[7])++ > 0) goto collide; }
    return;
collide:
    sms.VDP_Status |= VDP_STATUS_SpriteCollision; 
    Do_Collision = NO;
}

// PROCESS COLLISION FOR A DOUBLED SPRITE LINE --------------------------------
INLINE void     Sprite_Collide_Line_Double (byte *p_src, int x)
{
    int *       p_collision_table;

    p_collision_table = &Sprites_Collision_Table [x];
    if (p_src[0]) { if ((p_collision_table[ 0])++ > 0 || (p_collision_table[ 1])++ > 0) goto collide; }
    if (p_src[1]) { if ((p_collision_table[ 2])++ > 0 || (p_collision_table[ 3])++ > 0) goto collide; }
    if (p_src[2]) { if ((p_collision_table[ 4])++ > 0 || (p_collision_table[ 5])++ > 0) goto collide; }
    if (p_src[3]) { if ((p_collision_table[ 6])++ > 0 || (p_collision_table[ 7])++ > 0) goto collide; }
    if (p_src[4]) { if ((p_collision_table[ 8])++ > 0 || (p_collision_table[ 9])++ > 0) goto collide; }
    if (p_src[5]) { if ((p_collision_table[10])++ > 0 || (p_collision_table[11])++ > 0) goto collide; }
    if (p_src[6]) { if ((p_collision_table[12])++ > 0 || (p_collision_table[13])++ > 0) goto collide; }
    if (p_src[7]) { if ((p_collision_table[14])++ > 0 || (p_collision_table[15])++ > 0) goto collide; }
    return;
collide:
    sms.VDP_Status |= VDP_STATUS_SpriteCollision; 
    Do_Collision = NO;
}

// DRAW A SPRITE LINE ---------------------------------------------------------
INLINE void     Sprite_Draw_Line (byte *p_src, int x)
{
  int           color;
  byte          *p_dst, *p_mask;

  p_dst  = &GFX_Line [x];
  p_mask = &Sprites_Draw_Mask [x];

  color = p_src[0]; if (!p_mask[0] && color) p_dst[0] = Palette_Refs[16 + color]; // Note: Palette_Refs is a table (not a pointer) so the +16 is free
  color = p_src[1]; if (!p_mask[1] && color) p_dst[1] = Palette_Refs[16 + color];
  color = p_src[2]; if (!p_mask[2] && color) p_dst[2] = Palette_Refs[16 + color];
  color = p_src[3]; if (!p_mask[3] && color) p_dst[3] = Palette_Refs[16 + color];
  color = p_src[4]; if (!p_mask[4] && color) p_dst[4] = Palette_Refs[16 + color];
  color = p_src[5]; if (!p_mask[5] && color) p_dst[5] = Palette_Refs[16 + color];
  color = p_src[6]; if (!p_mask[6] && color) p_dst[6] = Palette_Refs[16 + color];
  color = p_src[7]; if (!p_mask[7] && color) p_dst[7] = Palette_Refs[16 + color];
}

// DRAW A DOUBLED SPRITE LINE -------------------------------------------------
INLINE void     Sprite_Draw_Line_Double (byte *p_src, int x)
{
  int           color;
  byte          *p_dst, *p_mask;

  p_dst  = &GFX_Line [x];
  p_mask = &Sprites_Draw_Mask [x];

  color = p_src[0]; if (color) { color = Palette_Refs[16 + color]; if (!p_mask[ 0]) p_dst[ 0] = color; if (!p_mask[ 1]) p_dst[ 1] = color; }
  color = p_src[1]; if (color) { color = Palette_Refs[16 + color]; if (!p_mask[ 2]) p_dst[ 2] = color; if (!p_mask[ 3]) p_dst[ 3] = color; }
  color = p_src[2]; if (color) { color = Palette_Refs[16 + color]; if (!p_mask[ 4]) p_dst[ 4] = color; if (!p_mask[ 5]) p_dst[ 5] = color; }
  color = p_src[3]; if (color) { color = Palette_Refs[16 + color]; if (!p_mask[ 6]) p_dst[ 6] = color; if (!p_mask[ 7]) p_dst[ 7] = color; }
  color = p_src[4]; if (color) { color = Palette_Refs[16 + color]; if (!p_mask[ 8]) p_dst[ 8] = color; if (!p_mask[ 9]) p_dst[ 9] = color; }
  color = p_src[5]; if (color) { color = Palette_Refs[16 + color]; if (!p_mask[10]) p_dst[10] = color; if (!p_mask[11]) p_dst[11] = color; }
  color = p_src[6]; if (color) { color = Palette_Refs[16 + color]; if (!p_mask[12]) p_dst[12] = color; if (!p_mask[13]) p_dst[13] = color; }
  color = p_src[7]; if (color) { color = Palette_Refs[16 + color]; if (!p_mask[14]) p_dst[14] = color; if (!p_mask[15]) p_dst[15] = color; }
}

// REFRESH SPRITES ------------------------------------------------------------
// FIXME: should emulate 9th Sprite bit
void        Refresh_Sprites_5 (int Draw)
{
    // 1. Count the number of sprites to render on this line
    {
        // Calculate Sprite Height
        int Height = 8;
        if (Sprites_Double) Height <<= 1;
        if (Sprites_8x16)   Height <<= 1;

        Sprite_Last = 0;
        Sprites_on_Line = 0;
        Find_Last_Sprite (Wide_Screen_28, Height, tsms.VDP_Line);

        // Return if there's no sprite on this line
        if (Sprites_on_Line == 0)
            return;
    }

    // Check if we have or not to process sprite collisions
    Do_Collision = (!(sms.VDP_Status & VDP_STATUS_SpriteCollision)
        && (Sprites_on_Line > 1));

    // If sprites do not have to be drawn, only update collisions
    if (Draw == NO)
    {
        // If we don't have to process collisions, there's no point to continue
        if (Do_Collision == NO)
            return;
        // Setting value to 64+9 will never draw sprites (only update collisions)
        // since sprites are drawn only if Sprites_On_Line < 9
        Sprites_on_Line = 64 + 9;
    }
    // Draw all sprites on line if flickering is not enabled
    else if (!(Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED))
    {
        Sprites_on_Line = 0;
    }

    // Process all sprites in 224 (28*8) lines mode
    // if (Wide_Screen_28)
    //    Sprite_Last = 63;

    // Clear Sprite Collision Table
    if (Do_Collision)
        memset (Sprites_Collision_Table_Start, 0, sizeof (int) * Sprites_Collision_Table_Len);

    // 3. Draw sprites
    {
        int     x, y, n;
        byte *  p_src;
        int     j;
        byte *  spr_map_xn = &SPR_AREA[0x80];
        int     spr_map_xn_offset;
        int     spr_map_n_mask = 0x01FF;

        // Bit 0 of Register 5 and Bits 0-1 of Register 6 act as masks on the 315-5124
        if (cur_machine.VDP.model == VDP_MODEL_315_5124)
        {
            if ((sms.VDP[5] & 1) == 0)
                spr_map_xn = &SPR_AREA[0x00];
            if ((sms.VDP[6] & 1) == 0)
                spr_map_n_mask &= ~0x0080;
            if ((sms.VDP[6] & 2) == 0)
                spr_map_n_mask &= ~0x0040;
        }

        // Now process actual sprites
        switch (sms.VDP[1] & (1 + 2))
        {
        case 0: // 8x8 sprites
            {
                for (j = Sprite_Last; j >= 0; j --)
                {
                    // Fetch Y & clip
                    y = SPR_AREA [j];
                    // if (y == 224) continue;
                    if (y > 224) y -= 256;

                    // Now Y will contains the sprite line to render
                    y = tsms.VDP_Line - y - 1;
                    if (y < 0 || y >= 8) continue;

                    // Fetch N & X
                    spr_map_xn_offset = j << 1;
                    n = ((int)spr_map_xn [spr_map_xn_offset + 1] | tgfx.Base_Sprite) & spr_map_n_mask;
                    x = spr_map_xn [spr_map_xn_offset] - Sprite_Shift_X;

                    // Decode tile if it isn't decoded yet
                    if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
                    { Decode_Tile (n); tgfx.Tile_Dirty [n] = TILE_DIRTY_REDRAW; }
                    p_src = &tgfx.Tile_Decoded [n] [y << 3];

                    // Process collision & draw
                    if (Do_Collision)
                        Sprite_Collide_Line (p_src, x);
                    if (Sprites_on_Line-- < 9)
                        Sprite_Draw_Line (p_src, x);
                }
            } break;
        case 1: // 8x8 sprites zoomed
            {
                for (j = Sprite_Last; j >= 0; j --)
                {
                    // Fetch Y & clip
                    y = SPR_AREA [j];
                    // if (y == 224) continue;
                    if (y > 224) y -= 256;

                    // Now Y will contains the sprite line to render
                    y = tsms.VDP_Line - y - 1;
                    if (y < 0 || y >= 16) continue;

                    // Fetch N & X
                    spr_map_xn_offset = j << 1;
                    n = ((int)spr_map_xn [spr_map_xn_offset + 1] | tgfx.Base_Sprite) & spr_map_n_mask;
                    x = spr_map_xn [spr_map_xn_offset] - Sprite_Shift_X;

                    // Decode tile if it isn't decoded yet
                    if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
                    { Decode_Tile (n); tgfx.Tile_Dirty [n] = TILE_DIRTY_REDRAW; }
                    p_src = &tgfx.Tile_Decoded [n] [(y >> 1) << 3];

                    // Process collision & draw
                    if (Do_Collision)
                        Sprite_Collide_Line_Double (p_src, x);
                    if (Sprites_on_Line-- < 9)
                        Sprite_Draw_Line_Double (p_src, x);
                }
            } break;
        case 2: // 8x16 sprites
            {
                spr_map_n_mask &= ~0x01;
                for (j = Sprite_Last; j >= 0; j --)
                {
                    // Fetch Y & clip
                    y = SPR_AREA [j];
                    // if (y == 224) continue;
                    if (y > 224) y -= 256;

                    // Now Y will contains the sprite line to render
                    y = tsms.VDP_Line - y - 1;
                    if (y < 0 || y >= 16) continue;

                    // Fetch N & X
                    // Increase N on the sprite second tile
                    spr_map_xn_offset = j << 1;
                    n = ((int)spr_map_xn [spr_map_xn_offset + 1] | tgfx.Base_Sprite) & spr_map_n_mask;
                    if (y & 8) // >= 8
                    {
                        n ++;
                        y &= 7;
                    }
                    x = spr_map_xn [spr_map_xn_offset] - Sprite_Shift_X;

                    // Decode tile if it isn't decoded yet
                    if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
                    { Decode_Tile (n); tgfx.Tile_Dirty [n] = TILE_DIRTY_REDRAW; }
                    p_src = &tgfx.Tile_Decoded [n] [y << 3];

                    // Process collision & draw
                    if (Do_Collision)
                        Sprite_Collide_Line (p_src, x);
                    if (Sprites_on_Line-- < 9)
                        Sprite_Draw_Line (p_src, x);
                }
            } break;
        case 3: // 8x16 sprites zoomed
            {
                spr_map_n_mask &= ~0x01;
                for (j = Sprite_Last; j >= 0; j --)
                {
                    // Fetch Y & clip
                    y = SPR_AREA [j];
                    // if (y == 224) continue;
                    if (y > 224) y -= 256;

                    // Now Y will contains the sprite line to render
                    y = tsms.VDP_Line - y - 1;
                    if (y < 0 || y >= 32) continue;

                    // Fetch N & X
                    // Increase N on the sprite second tile
                    spr_map_xn_offset = j << 1;
                    n = ((int)spr_map_xn [spr_map_xn_offset + 1] | tgfx.Base_Sprite) & spr_map_n_mask;
                    if (y & 16) // >= 16
                    {
                        n ++;
                        y &= 15;
                    }
                    x = spr_map_xn [spr_map_xn_offset] - Sprite_Shift_X;

                    // Decode tile if it isn't decoded yet
                    if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
                    { Decode_Tile (n); tgfx.Tile_Dirty [n] = TILE_DIRTY_REDRAW; }
                    p_src = &tgfx.Tile_Decoded [n] [(y >> 1) << 3];

                    // Process collision & draw
                    if (Do_Collision)
                        Sprite_Collide_Line_Double (p_src, x);
                    if (Sprites_on_Line-- < 9)
                        Sprite_Draw_Line_Double (p_src, x);
                }
            } break;
        } // end of switch

    }
}

//-----------------------------------------------------------------------------

