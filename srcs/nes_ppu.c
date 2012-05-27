//-----------------------------------------------------------------------------
// MEKA - nes_ppu.c
// Nintendo PPU Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "debugger.h"
#include "fskipper.h"
#include "nes.h"
#include "palette.h"

//-----------------------------------------------------------------------------
// Color configuration
// (Note: absolutely uses those define, so it willl be easier to switch to direct
//  24/32 output someday)
//-----------------------------------------------------------------------------

#define PIXEL_TYPE              u16
#define PIXEL_LINE_DST          GFX_Line16
#define PIXEL_PALETTE_TABLE     Palette_EmulationToHost16

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// Rendering
static u8		NES_Spr_Mask_Whole[NES_RES_X + 32];
static u8 *		NES_Spr_Mask = NES_Spr_Mask_Whole + 16;

static int		NES_NameTables[4][4] =
 {
   {      0,      1,      2,      3 }, // No Mirroring
   {      0,      0,      1,      1 }, // H Mirroring
   {      0,      1,      0,      1 }, // V Mirroring
   {      0,      1,      2,      3 }, // Four Screens (need 2kb+ of VRAM)
 };

static u16 *    GFX_Line16;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// PPU helper function to handle mapping
void    NES_PPU_Map (int page, int page_num, byte *data_start)
{
	int i, j;

	for (i = 0; i < page_num; i++, page++)
	{
		NES_VRAM_Banks[page] = data_start + (i * 0x400);
		if (page < 8)
		{
			for (j = 0; j < 64; j++)
			{
				tgfx.Tile_Dirty [page * 64 + j] |= TILE_DIRTY_DECODE;
			}
		}
	}
}

void    NES_PPU_Set_Mirroring (int Mirroring)
{
	nes->Mirroring = Mirroring;
	NES_VRAM_Banks[0x08] = NES_VRAM_Banks[0x0C] = &VRAM [0x2000 + (NES_NameTables [Mirroring] [0] * 0x400)];
	NES_VRAM_Banks[0x09] = NES_VRAM_Banks[0x0D] = &VRAM [0x2000 + (NES_NameTables [Mirroring] [1] * 0x400)];
	NES_VRAM_Banks[0x0A] = NES_VRAM_Banks[0x0E] = &VRAM [0x2000 + (NES_NameTables [Mirroring] [2] * 0x400)];
	NES_VRAM_Banks[0x0B] = NES_VRAM_Banks[0x0F] = &VRAM [0x2000 + (NES_NameTables [Mirroring] [3] * 0x400)];
}

void    NES_PPU_Write (word Addr, byte Value)
{
	switch (Addr & 0x2007)
	{
	case 0x2000: /* CR0 */
		nes->CR0 = Value;
		return;
	case 0x2001: /* CR1 */
		nes->CR1 = Value;
		return;
	case 0x2003: /* Object RAM Set Address */
		nes->Object_ADX = Value;
		return;
	case 0x2004: /* Object RAM Write */
		nes->Object_RAM [nes->Object_ADX] = Value;
		nes->Object_ADX = (nes->Object_ADX + 1) & 0xFF;
		return;
	case 0x2005: /* Scrolling Set */
		nes->Scroll [nes->Toggle] = Value;
		nes->Toggle ^= 1;
		return;
	case 0x2006: /* VRAM Set Address */
		if (nes->Toggle == 0)
		{
			sms.VDP_Access_First = Value;
		}
		else
		{
			sms.VDP_Address = ((sms.VDP_Access_First << 8) | Value) & 0x3FFF;
			nes->PPU_Read = 1;
		}
		nes->Toggle ^= 1;
		return;
	case 0x2007: /* VRAM Write */
		if (sms.VDP_Address >= 0x3F00)
		{
			//Value &= 0x3F; // Mask out the two higher bits
			if ((sms.VDP_Address & 0x0F) == 0)
			{
				// FIXME: 2007/05/08. This is probably way incorrect. Just trying to fix it to look good on test games. No care for NES emulation.
				PRAM[0x00] = PRAM[0x04] = PRAM[0x08] = PRAM[0x0C] =
				PRAM[0x10] = PRAM[0x14] = PRAM[0x18] = PRAM[0x1C] = Value;
				Palette_Emulation_SetColor(0x00, NES_Palette[Value & 63]);
				Palette_Emulation_SetColor(0x04, NES_Palette[Value & 63]);
				Palette_Emulation_SetColor(0x08, NES_Palette[Value & 63]);
				Palette_Emulation_SetColor(0x0C, NES_Palette[Value & 63]);
				Palette_Emulation_SetColor(0x10, NES_Palette[Value & 63]);
				Palette_Emulation_SetColor(0x14, NES_Palette[Value & 63]);
				Palette_Emulation_SetColor(0x18, NES_Palette[Value & 63]);
				Palette_Emulation_SetColor(0x1C, NES_Palette[Value & 63]);
			}
			else if (sms.VDP_Address & 0x03)
			{
				const int color_index = sms.VDP_Address & 0x1F;
				PRAM[color_index] = Value;
				Palette_Emulation_SetColor(color_index, NES_Palette[Value & 63]);
			}
			// Msg (MSGT_DEBUG, "VRAM [%04X] = %02X", sms.VDP_Address, Value);
		}
		else
		{
			// FIXME: write protection for VROM ?
			NES_VRAM_Banks [sms.VDP_Address >> 10] [sms.VDP_Address & 0x03FF] = Value;
			if (sms.VDP_Address < 0x2000)
				tgfx.Tile_Dirty [sms.VDP_Address / 16] |= TILE_DIRTY_DECODE;
		}
		sms.VDP_Address += ((nes->CR0 & NES_CR0_INC) ? 32 : 1);
		sms.VDP_Address &= 0x3FFF;
		return;
	}
	// Msg (MSGT_DEBUG, "[NES] PPU Write %02X to %04X", Value, Addr);
}

byte    NES_PPU_Read (word Addr)
{
 byte ret = 0;
 switch (Addr & 0x2007)
    {
    case 0x2002:
         nes->Toggle = 0;
         ret = sms.VDP_Status;
         #ifdef NEW_S0HIT
           // ICount goes from 114 down to 0
           if (nes->Sprite_0_Hit != -1 && nes->Regs.ICount < nes->Sprite_0_Hit)
              {
              nes->Sprite_0_Hit = -1;
              ret |= NES_PPU_S0HIT;
              }
         #endif
         sms.VDP_Status &= ~NES_PPU_VBLANK; // Clear VBL flag
         return (ret);
    case 0x2007:
         ret = nes->PPU_Read_Latch;
         if (nes->PPU_Read) /* First fead from PPU is invalid */
            {
            nes->PPU_Read = FALSE;
            }
         else
            {
            sms.VDP_Address += ((nes->CR0 & NES_CR0_INC) ? 32 : 1);
            sms.VDP_Address &= 0x3FFF;
            }
         nes->PPU_Read_Latch = NES_VRAM_Banks [sms.VDP_Address >> 10] [sms.VDP_Address & 0x03FF];
         return (ret);
    }
// Msg (MSGT_DEBUG, "[NES] PPU Read %04X, returning zero", Addr);
 return (nes->PPU_Read_Latch);
}

void    NES_PPU_Refresh (int Line)
{
	if (fskipper.Show_Current_Frame)
	{
		// Point to current video line
		GFX_Line16 = (u16 *)screenbuffer->line[Line];

		// Blank line if necessary ---------------------------------------------------
		if (!(NES_Display_BG))
		{
			const u16 backdrop_color = COLOR_BLACK;
			int n;
			u16 *p = GFX_Line16;
			for (n = NES_RES_X; n != 0; n--)
				*p++ = backdrop_color;
			return;
		}

		// Draw background -----------------------------------------------------------
		if (opt.Layer_Mask & LAYER_BACKGROUND)
		{
			NES_PPU_Refresh_BgFg (Line);
		}
		else
		{
			const u16 backdrop_color = makecol16(222,222,101);
			int n;
			u16 *p = GFX_Line16;
			for (n = NES_RES_X; n != 0; n--)
				*p++ = backdrop_color;
			memset(NES_Spr_Mask, 0, NES_RES_X);
		}

		// Draw sprites --------------------------------------------------------------
		// FIXME: emulates S0HIT even if sprites are disabled
		if (opt.Layer_Mask & LAYER_SPRITES)
		{
			if (NES_Display_SPR)
				NES_PPU_Refresh_Sprites (Line);
		}
		else
		{
			NES_PPU_Refresh_Sprites_S0Hit (Line);
		}

		// Blank the leftmost column if necessary ------------------------------------
		if (NES_Mask_Left_BG)
		{
			// FIXME-BORDER
			//const u16 backdrop_color = COLOR_BLACK;
			const u16 backdrop_color = PIXEL_PALETTE_TABLE[0];
			GFX_Line16[0] = backdrop_color;
			GFX_Line16[1] = backdrop_color;
			GFX_Line16[2] = backdrop_color;
			GFX_Line16[3] = backdrop_color;
			GFX_Line16[4] = backdrop_color;
			GFX_Line16[5] = backdrop_color;
			GFX_Line16[6] = backdrop_color;
			GFX_Line16[7] = backdrop_color;
		}
	}
	else
	{
		NES_PPU_Refresh_Sprites_S0Hit (Line);
	}
}

void    NES_PPU_Refresh_BgFg (int Line)
{
  // For COMPUTATION ----------------------------------------------------------
  int   src_x, src_y;   // In pixel
  int   src_n;          // In tile
  int   src_nx, src_ny; // In tile
  int   src_ly_8;       // Line of tile (0->7), multiplied by 8
  byte *src_nt;         // Name Table
  int   src_tile_i;     // Tile index
  // For DRAWING --------------------------------------------------------------
  u8    src_color;      // Color
  int *	pal;            // Palette
  int   pal_i;          // Palette index
  u8 *	src;            // Source buffer (decoded tile)
  PIXEL_TYPE *dst;      // Destination buffer (video/memory bitmap)
  int   dst_x;          // Position in pixel
  byte *spr_mask;       // Sprite mask buffer
  //---------------------------------------------------------------------------

  src_x = nes->Scroll [NES_SCROLL_HORIZONTAL];
  if (nes->Scroll [NES_SCROLL_VERTICAL] >= 240)
     src_y = Line - nes->Scroll [NES_SCROLL_VERTICAL];
  else
     src_y = Line + nes->Scroll [NES_SCROLL_VERTICAL];

  src_nx = src_x / 8;
  src_ny = src_y / 8;

  // Init name table ----------------------------------------------------------
  // src_nt = VRAM + 0x2000 + 0x400 * NES_NameTables [nes->Mirroring] [nes->CR0 & NES_CR0_NT_ADDR_MASK];
  src_nt = NES_VRAM_Banks [8 + (nes->CR0 & NES_CR0_NT_ADDR_MASK)];
  if (src_ny >= 30)
     {
     src_ny -= 30;
     src_nt = VRAM + ((src_nt - VRAM) ^ 0x400);
     }

  src_n = (src_ny * 32) + src_nx;
  src_ly_8 = (src_y % 8) * 8;

  dst_x = -(src_x % 8);
  dst = &PIXEL_LINE_DST[dst_x];
  spr_mask = &NES_Spr_Mask[dst_x];

  while (dst_x < NES_RES_X)
     {
     // Get tile index from name table ----------------------------------------
     src_tile_i = src_nt [src_n];
     if (nes->CR0 & NES_CR0_BG_PAT)
        src_tile_i |= 0x100;
     //------------------------------------------------------------------------

     // Get tile palette from attribute table ---------------------------------
     pal_i = src_nt [0x03C0 + ((src_n & 0x0380) / 16) + ((src_n & 0x1F) / 4)];
     pal_i >>= ((src_n & 0x40) / 16) + (src_n & 0x02);
     pal = &PIXEL_PALETTE_TABLE[(pal_i & 3) * 4];
     //------------------------------------------------------------------------

     // Set pointer to tile, and decode it if it has changed ------------------
     if (tgfx.Tile_Dirty [src_tile_i] & TILE_DIRTY_DECODE)
        {
        NES_Decode_Tile (src_tile_i);
        tgfx.Tile_Dirty [src_tile_i] = TILE_DIRTY_REDRAW;
        }
     src = &tgfx.Tile_Decoded [src_tile_i] [src_ly_8];
     //------------------------------------------------------------------------

     // Draw the tile ---------------------------------------------------------
     src_color = *src++;  *dst++ = pal[src_color];  *spr_mask++ = (src_color ? 1 : 0);
     src_color = *src++;  *dst++ = pal[src_color];  *spr_mask++ = (src_color ? 1 : 0);
     src_color = *src++;  *dst++ = pal[src_color];  *spr_mask++ = (src_color ? 1 : 0);
     src_color = *src++;  *dst++ = pal[src_color];  *spr_mask++ = (src_color ? 1 : 0);
     src_color = *src++;  *dst++ = pal[src_color];  *spr_mask++ = (src_color ? 1 : 0);
     src_color = *src++;  *dst++ = pal[src_color];  *spr_mask++ = (src_color ? 1 : 0);
     src_color = *src++;  *dst++ = pal[src_color];  *spr_mask++ = (src_color ? 1 : 0);
     src_color = *src  ;  *dst++ = pal[src_color];  *spr_mask++ = (src_color ? 1 : 0);
     //------------------------------------------------------------------------

     // Next tile.. -----------------------------------------------------------
     src_n += 1;        // Increment source tile number by 1
     src_nx += 1;       // Increment source tile number (X) by 1
     if (src_nx == 32)
        {
        // Switch nametable
        src_n -= 32;
        src_nx = 0;
        src_nt = VRAM + ((src_nt - VRAM) ^ 0x400);
        }
     dst_x += 8;        // Increment destination pixel by 8
     //------------------------------------------------------------------------
     }
}

void    NES_PPU_Refresh_Sprites (int Line)
{
	int   		c, i;
	int *		Pal;
	u8 *		Src;
	PIXEL_TYPE *Dst;
	int			obj_x, obj_y, obj_a, obj_i, obj_l, obj_h;
	byte *		Spr_Mask;

	// Draw sprites from back to front -------------------------------------------
	for (i = 63; i >= 0; i--)
	{
		obj_y = nes->Object_RAM [(i << 2) + 0] + 1;
		obj_i = nes->Object_RAM [(i << 2) + 1];
		obj_a = nes->Object_RAM [(i << 2) + 2];
		obj_x = nes->Object_RAM [(i << 2) + 3];

		obj_h = (NES_Sprites_8x16 ? 16 : 8);
		if ((Line >= obj_y) && (Line < (obj_y + obj_h)))
		{
			if (NES_Sprites_8x16)
			{
				if (obj_i & 1)
				{
					obj_i &= 0xFE;
					obj_i |= 0x100;
				}
			}
			else
			{
				if (nes->CR0 & NES_CR0_SPR_PAT)
					obj_i |= 0x100;
			}

			obj_l = Line - obj_y;                            // Line of the sprite
			if (obj_a & NES_SPR_VFLIP)                       // Vertical Flipping
				obj_l = (obj_h - 1 - obj_l);
			if (obj_l >= 8)
			{
				obj_l -= 8;
				obj_i += 1;
			}

			Pal = &PIXEL_PALETTE_TABLE[0x10 + (obj_a & 3) * 4];     // Palette
			if (tgfx.Tile_Dirty[obj_i] & TILE_DIRTY_DECODE)
			{
				NES_Decode_Tile (obj_i);
				tgfx.Tile_Dirty [obj_i] = TILE_DIRTY_REDRAW;
			}
			Src = &tgfx.Tile_Decoded [obj_i] [obj_l * 8];    // Src (tiles)
			Dst = &PIXEL_LINE_DST[obj_x];                    // Dst (buffer)

			// Emulate Sprite #0 HIT -----------------------------------------------
#ifdef NEW_S0HIT
			if (i == 0 && nes->Sprite_0_Hit == -1)
			{
				c = 0;
				Spr_Mask = &NES_Spr_Mask [obj_x];
				if ((Src[0] && Spr_Mask[0]) || (++c && Src[1] && Spr_Mask[1]) ||
					(++c && Src[2] && Spr_Mask[2]) || (++c && Src[3] && Spr_Mask[3]) ||
					(++c && Src[4] && Spr_Mask[4]) || (++c && Src[5] && Spr_Mask[5]) ||
					(++c && Src[6] && Spr_Mask[6]) || (++c && Src[7] && Spr_Mask[7]))
					nes->Sprite_0_Hit = nes->Regs.IPeriod - ((obj_x + c) * nes->Regs.IPeriod) / NES_RES_X;
			}
#else
			if (i == 0 && !(sms.VDP_Status & NES_PPU_S0HIT))
			{
				Spr_Mask = &NES_Spr_Mask [obj_x];
				if (((int*)Src)[0] || ((int*)Src)[1])
					sms.VDP_Status |= NES_PPU_S0HIT;
			}
#endif
			// Draw tile -----------------------------------------------------------
			switch (obj_a & (NES_SPR_HFLIP | NES_SPR_PRIORITY))
			{
				// No flipping, over background -------------------------------------
			case 0:
				c = *Src++; if (c) *Dst = Pal[c]; Dst++;
				c = *Src++; if (c) *Dst = Pal[c]; Dst++;
				c = *Src++; if (c) *Dst = Pal[c]; Dst++;
				c = *Src++; if (c) *Dst = Pal[c]; Dst++;
				c = *Src++; if (c) *Dst = Pal[c]; Dst++;
				c = *Src++; if (c) *Dst = Pal[c]; Dst++;
				c = *Src++; if (c) *Dst = Pal[c]; Dst++;
				c = *Src  ; if (c) *Dst = Pal[c];
				break;
				// Horizontal flipping, over background ------------------------------
			case NES_SPR_HFLIP:
				Src += 7;
				c = *Src--; if (c) *Dst = Pal[c]; Dst++;
				c = *Src--; if (c) *Dst = Pal[c]; Dst++;
				c = *Src--; if (c) *Dst = Pal[c]; Dst++;
				c = *Src--; if (c) *Dst = Pal[c]; Dst++;
				c = *Src--; if (c) *Dst = Pal[c]; Dst++;
				c = *Src--; if (c) *Dst = Pal[c]; Dst++;
				c = *Src--; if (c) *Dst = Pal[c]; Dst++;
				c = *Src  ; if (c) *Dst = Pal[c];
				break;
				// No flipping, behind background ------------------------------------
			case NES_SPR_PRIORITY:
				Spr_Mask = &NES_Spr_Mask [obj_x];
				c = *Src++; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src++; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src++; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src++; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src++; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src++; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src++; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src  ; if (c && !*Spr_Mask) *Dst = Pal[c];
				break;
				// Horizontal flipping, behind background ----------------------------
			case (NES_SPR_HFLIP | NES_SPR_PRIORITY):
				Src += 7;
				Spr_Mask = &NES_Spr_Mask [obj_x];
				c = *Src--; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src--; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src--; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src--; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src--; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src--; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src--; if (c && !*Spr_Mask) *Dst = Pal[c]; Dst++; Spr_Mask++;
				c = *Src  ; if (c && !*Spr_Mask) *Dst = Pal[c];
				break;
			}
		}
	}
}

void    NES_PPU_Refresh_Sprites_S0Hit (int Line)
{
 int    i;
 byte  *Src;
 int    obj_x, obj_y, obj_a, obj_i, obj_l, obj_h;

 #ifdef NEW_S0HIT
    byte  *Spr_Mask;

    if (nes->Sprite_0_Hit != -1)
       return;
 #else
    if (sms.VDP_Status & NES_PPU_S0HIT)
       return;
 #endif

 // Draw sprites from back to front -------------------------------------------
 i = 0;

 obj_y = nes->Object_RAM [(i << 2) + 0] + 1;
 obj_i = nes->Object_RAM [(i << 2) + 1];
 obj_a = nes->Object_RAM [(i << 2) + 2];
 obj_x = nes->Object_RAM [(i << 2) + 3];

 obj_h = (NES_Sprites_8x16 ? 16 : 8);
 if ((Line >= obj_y) && (Line < (obj_y + obj_h)))
    {
    if (NES_Sprites_8x16)
       {
       if (obj_i & 1)
          {
          obj_i &= 0xFE;
          obj_i |= 0x100;
          }
       }
    else
      if (nes->CR0 & NES_CR0_SPR_PAT)
         obj_i |= 0x100;

    obj_l = Line - obj_y;                            // Line of the sprite
    if (obj_a & NES_SPR_VFLIP)                       // Vertical Flipping
       obj_l = (obj_h - 1 - obj_l);
    if (obj_l >= 8)
       {
       obj_l -= 8;
       obj_i += 1;
       }

    if (tgfx.Tile_Dirty [obj_i] & TILE_DIRTY_DECODE)
       {
       NES_Decode_Tile (obj_i);
       tgfx.Tile_Dirty [obj_i] = TILE_DIRTY_REDRAW;
       }
    Src = &tgfx.Tile_Decoded [obj_i] [obj_l * 8];    // Src (tiles)

    // Emulate Sprite #0 HIT -----------------------------------------------

    #ifdef NEW_S0HIT
       {
       int c = 0;
       Spr_Mask = &NES_Spr_Mask [obj_x];
       if ((Src[0] && Spr_Mask[0]) || (++c && Src[1] && Spr_Mask[1]) ||
           (++c && Src[2] && Spr_Mask[2]) || (++c && Src[3] && Spr_Mask[3]) ||
           (++c && Src[4] && Spr_Mask[4]) || (++c && Src[5] && Spr_Mask[5]) ||
           (++c && Src[6] && Spr_Mask[6]) || (++c && Src[7] && Spr_Mask[7]))
                nes->Sprite_0_Hit = nes->Regs.IPeriod - ((obj_x + c) * nes->Regs.IPeriod) / NES_RES_X;
       }
    #else
      if (((int*)Src)[0] || ((int*)Src)[1])
         sms.VDP_Status |= NES_PPU_S0HIT;
    #endif
    }
}

//-----------------------------------------------------------------------------
