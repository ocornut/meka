//-----------------------------------------------------------------------------
// MEKA - VIDEO_SP.C
// SMS/GG Sprite Drawing - Code
//-----------------------------------------------------------------------------
// Note:
//   ** THIS FILE IS INCLUDED TWICE FROM VIDEO_M5.C **
//   It was made this way because it uses macros that enable/disable
//   certains checks depending on screen size
//-----------------------------------------------------------------------------

 int    line;
 byte * p_src;

 // Now parse the actual sprites ----------------------------------------------
 switch (sms.VDP[1] & (1 + 2))
   {
   case 0: // 8x8 sprites
     {
     for (j = Sprite_Last; j >= 0; j --)
         {
         y = SPR_AREA [j];
         #ifndef WIDE_SCREEN_28
           // if (y == 224) continue;
           if (y > 224) y -= 256;
         #endif
         line = tsms.VDP_Line - y - 1;
         if (line < 0 || line >= 8) continue;
         Addr_Sprite = 0x80 + (j * 2);
         n = SPR_AREA [Addr_Sprite + 1] | tgfx.Base_Sprite;
         x = SPR_AREA [Addr_Sprite] - Sprite_Shift_X;
         if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
            { Decode_Tile (n); tgfx.Tile_Dirty [n] = TILE_DIRTY_REDRAW; }
         p_src = &tgfx.Tile_Decoded [n] [line * 8];
         if (Do_Collision)
            Sprite_Collide_Line (p_src, x);
         if (Sprites_on_Line < 9)
            Sprite_Draw_Line (p_src, x);
         Sprites_on_Line --;
         }
     } break;
   case 1: // 8x8 sprites zoomed
     {
     for (j = Sprite_Last; j >= 0; j --)
         {
         y = SPR_AREA [j];
         #ifndef WIDE_SCREEN_28
           // if (y == 224) continue;
           if (y > 224) y -= 256;
         #endif
         line = tsms.VDP_Line - y - 1;
         if (line < 0 || line >= 16) continue;
         Addr_Sprite = 0x80 + (j * 2);
         n = SPR_AREA [Addr_Sprite + 1] | tgfx.Base_Sprite;
         x = SPR_AREA [Addr_Sprite] - Sprite_Shift_X;
         if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
            { Decode_Tile (n); tgfx.Tile_Dirty [n] = TILE_DIRTY_REDRAW; }
         p_src = &tgfx.Tile_Decoded [n] [(line / 2) * 8];
         if (Do_Collision)
            Sprite_Collide_Line_Double (p_src, x);
         if (Sprites_on_Line < 9)
            Sprite_Draw_Line_Double (p_src, x);
         Sprites_on_Line --;
         }
     } break;
   case 2: // 8x16 sprites
     {
     for (j = Sprite_Last; j >= 0; j --)
         {
         y = SPR_AREA [j];
         #ifndef WIDE_SCREEN_28
           // if (y == 224) continue;
           if (y > 224) y -= 256;
         #endif
         line = tsms.VDP_Line - y - 1;
         if (line < 0 || line >= 16) continue;
         Addr_Sprite = 0x80 + (j * 2);
         n = (SPR_AREA [Addr_Sprite + 1] & 0xFE) | tgfx.Base_Sprite;
         if (line >= 8)
            {
            n ++;
            line -= 8;
            y += 8;
            }
         x = SPR_AREA [Addr_Sprite] - Sprite_Shift_X;
         if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
            { Decode_Tile (n); tgfx.Tile_Dirty [n] = TILE_DIRTY_REDRAW; }
         p_src = &tgfx.Tile_Decoded [n] [line * 8];
         if (Do_Collision)
            Sprite_Collide_Line (p_src, x);
         if (Sprites_on_Line < 9)
            Sprite_Draw_Line (p_src, x);
         Sprites_on_Line --;
         }
     } break;
   case 3: // 8x16 sprites zoomed
     {
     for (j = Sprite_Last; j >= 0; j --)
         {
         y = SPR_AREA [j];
         #ifndef WIDE_SCREEN_28
           // if (y == 224) continue;
           if (y > 224) y -= 256;
         #endif
         line = tsms.VDP_Line - y - 1;
         if (line < 0 || line >= 32) continue;
         Addr_Sprite = 0x80 + (j * 2);
         n = (SPR_AREA [Addr_Sprite + 1] & 0xFE) | tgfx.Base_Sprite;
         if (line >= 16)
            {
            n ++;
            line -= 16;
            y += 16;
            }
         x = SPR_AREA [Addr_Sprite] - Sprite_Shift_X;
         if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
            { Decode_Tile (n); tgfx.Tile_Dirty [n] = TILE_DIRTY_REDRAW; }
         p_src = &tgfx.Tile_Decoded [n] [(line / 2) * 8];
         if (Do_Collision)
            Sprite_Collide_Line_Double (p_src, x);
         if (Sprites_on_Line < 9)
            Sprite_Draw_Line_Double (p_src, x);
         Sprites_on_Line --;
         }
     } break;
   } // end of switch

//-----------------------------------------------------------------------------

