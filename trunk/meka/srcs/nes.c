//-----------------------------------------------------------------------------
// MEKA - nes.c
// Nintendo Emulation - Code
//-----------------------------------------------------------------------------
// TO DO:
//  - Joypad strobing stuff
//  - Vertical Scrolling
//  - Sprite #0 Pixel Exact emulation
//  - Fix Flickering in Super Mario Bros
//  - Fix Title screen in Super Mario Bros
//  - Fix Reversed tiles in Ice Hockey
//  - Improve speed
//-----------------------------------------------------------------------------

#include "shared.h"
#include "debugger.h"
#include "mappers.h"
#include "nes.h"
#include "palette.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

const RGB   NES_Palette[64] =
{
  { 0x80,0x80,0x80,0 }, { 0x00,0x00,0xBB,0 }, { 0x37,0x00,0xBF,0 }, { 0x84,0x00,0xA6,0 },
  { 0xBB,0x00,0x6A,0 }, { 0xB7,0x00,0x1E,0 }, { 0xB3,0x00,0x00,0 }, { 0x91,0x26,0x00,0 },
  { 0x7B,0x2B,0x00,0 }, { 0x00,0x3E,0x00,0 }, { 0x00,0x48,0x0D,0 }, { 0x00,0x3C,0x22,0 },
  { 0x00,0x2F,0x66,0 }, { 0x00,0x00,0x00,0 }, { 0x05,0x05,0x05,0 }, { 0x05,0x05,0x05,0 },

  { 0xC8,0xC8,0xC8,0 }, { 0x00,0x59,0xFF,0 }, { 0x44,0x3C,0xFF,0 }, { 0xB7,0x33,0xCC,0 },
  { 0xFF,0x33,0xAA,0 }, { 0xFF,0x37,0x5E,0 }, { 0xFF,0x37,0x1A,0 }, { 0xD5,0x4B,0x00,0 },
  { 0xC4,0x62,0x00,0 }, { 0x3C,0x7B,0x00,0 }, { 0x1E,0x84,0x15,0 }, { 0x00,0x95,0x66,0 },
  { 0x00,0x84,0xC4,0 }, { 0x11,0x11,0x11,0 }, { 0x09,0x09,0x09,0 }, { 0x09,0x09,0x09,0 },

  { 0xFF,0xFF,0xFF,0 }, { 0x00,0x95,0xFF,0 }, { 0x6F,0x84,0xFF,0 }, { 0xD5,0x6F,0xFF,0 },
  { 0xFF,0x77,0xCC,0 }, { 0xFF,0x6F,0x99,0 }, { 0xFF,0x7B,0x59,0 }, { 0xFF,0x91,0x5F,0 },
  { 0xFF,0xA2,0x33,0 }, { 0xA6,0xBF,0x00,0 }, { 0x51,0xD9,0x6A,0 }, { 0x4D,0xD5,0xAE,0 },
  { 0x00,0xD9,0xFF,0 }, { 0x66,0x66,0x66,0 }, { 0x0D,0x0D,0x0D,0 }, { 0x0D,0x0D,0x0D,0 },

  { 0xFF,0xFF,0xFF,0 }, { 0x84,0xBF,0xFF,0 }, { 0xBB,0xBB,0xFF,0 }, { 0xD0,0xBB,0xFF,0 },
  { 0xFF,0xBF,0xEA,0 }, { 0xFF,0xBF,0xCC,0 }, { 0xFF,0xC4,0xB7,0 }, { 0xFF,0xCC,0xAE,0 },
  { 0xFF,0xD9,0xA2,0 }, { 0xCC,0xE1,0x99,0 }, { 0xAE,0xEE,0xB7,0 }, { 0xAA,0xF7,0xEE,0 },
  { 0xB3,0xEE,0xFF,0 }, { 0xDD,0xDD,0xDD,0 }, { 0x11,0x11,0x11,0 }, { 0x11,0x11,0x11,0 }
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// NES Memory Map -------------------------------------------------------------
//  0000h -> 1FFFh : RAM (mirrored, blocks of 2 kb)
//  2000h -> 3FFFh : PPU
//  4000h -> 5FFFh : Mapped Peripherals
//  6000h -> 7FFFh : Battery backed Memory
//  8000h -> FFFFh : ROM (4 pages of 8 kb, mapped)
//-----------------------------------------------------------------------------

void    NES_Init (void)
{
	// if (cfg.NES_Enabled == 0)
	//    return;
	nes = Memory_Alloc (sizeof (*nes));
}

void    NES_Reset (void)
{
 int    i, m;

 //-- MAPPING -----------------------------------------------------------------
 NES_Header = ROM;
 m = NESHEAD_MAPPER(NES_Header);
 NES_Prg = ROM + 16;
 NES_Prg_Cnt = NESHEAD_PRG_COUNT(NES_Header);
 NES_Prg_Mask = (NES_Prg_Cnt * 2) - 1;
 NES_Chr_Cnt = NESHEAD_CHR_COUNT(NES_Header);
 NES_Chr_Mask = (NES_Chr_Cnt * 2) - 1;
 if (NES_Chr_Cnt > 0)
    NES_Chr = NES_Prg + (NES_Prg_Cnt * 0x4000);
 else
    NES_Chr = &VRAM[0x0000];
 //-- MAPPERS -----------------------------------------------------------------
 for (i = 0; NES_Mappers[i].id != -1; i++)
    if (NES_Mappers[i].id == m)
       {
       NES_Mapper_Set (i);
       break;
       }
 if (NES_Mappers[i].id == -1)
    {
    Msg (MSGT_USER, Msg_Get (MSG_NES_Mapper_Unknown), m);
    NES_Mapper_Set (0); // Default, NULL mapper
    }
 //-- MIRRORING ---------------------------------------------------------------
 if (NESHEAD_FOURSCREEN(NES_Header))
    nes->Mirroring = NES_MIRRORING_4S;
 else
    nes->Mirroring = (NESHEAD_MIRRORING(NES_Header)) ? NES_MIRRORING_V : NES_MIRRORING_H;
 //-- SaveRAM -----------------------------------------------------------------
 sms.SRAM_Pages = NESHEAD_SAVERAM(NES_Header) ? 1 : 0;
 if (sms.SRAM_Pages)
    Map_8k_Other (3, &SRAM [0x0000]);
 //-- VIDEO -------------------------------------------------------------------
 sms.VDP_Address = 0x2000;
 sms.VDP_Status = 0x00;
 tsms.VDP_Line = 0;
 nes->CR0 = 0x00;
 nes->CR1 = NES_CR1_BG_ON | NES_CR1_SPR_ON;
 nes->Toggle = 0;
 nes->Scroll[0] = nes->Scroll[1] = 0;
 #ifdef NEW_S0HIT
    nes->Sprite_0_Hit = -1;
 #endif
 NES_PPU_Map (0, 8, (NES_Chr_Cnt >= 1 ? NES_Chr : &VRAM[0x0000]));
 NES_PPU_Set_Mirroring (nes->Mirroring);
 // PRAM = VRAM + 0x3F00;
 //PRAM = Palette_Refs;
 memset (&nes->Object_RAM, 0, 0x100);
 NES_Decode_Tiles (); // Dirty flags are set in machine_reset
 //-- INPUT -------------------------------------------------------------------
 nes->Joy_Strobe = nes->Joy_Status = 0;
 //-- CPU ---------------------------------------------------------------------
 // Note: has to be done after ROM mapping is done
 nes->Regs.IPeriod = opt.Cur_IPeriod; // 114: 1.7897725 Mhz (NTSC) or 1.773447 (PAL)
 Reset6502 (&nes->Regs);
 nes->Regs.TrapBadOps = FALSE;
}

void    Wr6502 (register word Addr, register byte Value)
{
  // FIXME: there's a generic write-trap message, with PC..
  // Msg (MSGT_DEBUG, "[NES] Write %02X to %04X", Value, Addr);

  switch (Addr >> 13)
    {
    // RAM
    case 0:     RAM [Addr & 0x07FF] = Value; return;
    // PPU
    case 1:     NES_PPU_Write (Addr, Value); return;
    // SaveRAM
    case 3:     if (sms.SRAM_Pages)
                   Mem_Pages [3] [Addr] = Value; // == SRAM[Addr & 0x1FFF] = Value
                // else
                //   Msg (MSGT_DEBUG, "[NES] Writing to SaveRAM (%04X=%02X)", Addr, Value);
                return;
    // Mapper
    case 4:
    case 5:
    case 6:
    case 7:     NES_Mapper_Write (Addr, Value); return;
    }

  /* Sprite DMA */
  if (Addr == 0x4014)
     {
     memcpy (nes->Object_RAM, RAM + (Value << 8), 0x100);
     return;
     }

  /* Joypad #1 */
  if (Addr == 0x4016)
    {
    if (nes->Joy_Strobe && Value == 0)
       nes->Joy_Status_Read = (nes->Joy_Status & 0xFF) | 0x00010000;
                                                         //   ^ Signature
    nes->Joy_Strobe = Value;
    return;
    }

 // Msg (MSGT_DEBUG, "[NES] Write %02X to %04X", Value, Addr);
}

byte    Rd6502 (register word Addr)
{
  // Msg (MSGT_DEBUG, "[NES] Read at %04X", Addr);

  switch (Addr >> 13)
    {
    // ROM
    case 4:     return Mem_Pages [4] [Addr];
    case 5:     return Mem_Pages [5] [Addr];
    case 6:     return Mem_Pages [6] [Addr];
    case 7:     return Mem_Pages [7] [Addr];
    // RAM
    case 0:     return RAM [Addr & 0x07FF];
    // PPU
    case 1:     return NES_PPU_Read (Addr);
    // Peripherals - see below
    // case 2:
    // SaveRAM
    case 3:     // Msg (MSGT_DEBUG, "[NES] Read from SaveRAM (%04X)", Addr);
                if (sms.SRAM_Pages)
                   return Mem_Pages [3] [Addr]; // == SRAM [Addr & 0x1FFF];
                return (0x00);
    }

  /* Joypad #1 */
  if (Addr == 0x4016)
     {
     int Ret = nes->Joy_Status_Read & 1;
     nes->Joy_Status_Read >>= 1;
     return (Ret | 0x40);
     }

 // Msg (MSGT_DEBUG, "[NES] Read at %04X, %02X", Addr, NES_Prg [Addr & 0x3FFF]);
 return (0x00);
}

byte    Loop6502 (register M6502 *R)
{
    tsms.VDP_Line = (tsms.VDP_Line + 1) % 262; // opt.Cur_TV_Lines; // Break ICE Hockey

    // Debugger hook
    #ifdef MEKA_Z80_DEBUGGER
	if (Debugger.active)
		Debugger_RasterLine_Hook(tsms.VDP_Line);
	#endif

    // Update sound cycle counter
    Sound_Update_Count += opt.Cur_IPeriod; // Should be made obsolete
    Sound_CycleCounter += opt.Cur_IPeriod;

    // Screen refreshing
    if (tsms.VDP_Line < 240)
        NES_PPU_Refresh (tsms.VDP_Line);

    // Vertical blank
    if (tsms.VDP_Line == 240)
    {
        /*---[ Generic Stuff ]------------------------------------*/
        #undef Macro_Stop_CPU
        #define Macro_Stop_CPU { return (NES_INT_QUIT); }
        Interrupt_Loop_Misc_Common;
        Refresh_Screen ();
        if ((opt.Force_Quit) || (CPU_Loop_Stop))
            Macro_Stop_CPU;
        #undef Macro_Stop_CPU
        /*---[ Joystick ]-----------------------------------------*/
        nes->Joy_Status = 0x00;
        // Convert Sega 8-bit controller format to NES one
        // This is a great proof of how bad NES is integrated in MEKA
        if (!(tsms.Control[7] & 0x0001))    nes->Joy_Status |= 0x10;    // UP
        if (!(tsms.Control[7] & 0x0002))    nes->Joy_Status |= 0x20;    // DOWN
        if (!(tsms.Control[7] & 0x0004))    nes->Joy_Status |= 0x40;    // LEFT
        if (!(tsms.Control[7] & 0x0008))    nes->Joy_Status |= 0x80;    // RIGHT
        if (!(tsms.Control[7] & 0x0020))    nes->Joy_Status |= 0x01;    // 1
        if (!(tsms.Control[7] & 0x0010))    nes->Joy_Status |= 0x02;    // 2
        if (!(tsms.Control[7] & 0x0400))    nes->Joy_Status |= 0x04;    // SELECT
        if (!(tsms.Control[7] & 0x0800))    nes->Joy_Status |= 0x08;    // START
        /*---[ Scrolling ]----------------------------------------*/
        nes->Toggle = 0;
        nes->Scroll[0] = nes->Scroll[1] = 0;
        /*---[ NMI ]----------------------------------------------*/
        sms.VDP_Status |= NES_PPU_VBLANK;
        #ifdef NEW_S0HIT
            nes->Sprite_0_Hit = -1;
        #else
            sms.VDP_Status &= ~NES_PPU_S0HIT;
        #endif
        if (NES_VBlank_ON)
        {
            return (NES_INT_NMI);
        }
    }
    return (NES_INT_NONE);
}

// Planar -> Linear
void    NES_Decode_Tile (int n)
{
    int x, y;
    int b0, b1;
    int i0, i1;
    u8  *src, *dst;
    int addr = n * 16;

    src = &NES_VRAM_Banks [addr >> 10] [addr & 0x03FF];
    dst = &tgfx.Tile_Decoded [n] [0];

    // FIXME: Unroll, unroll...
    for (y = 0; y < 8; y++)
    {
        b0 = src [y + 0];
        b1 = src [y + 8];
        for (x = 0; x < 8; x++)
        {
            i0 = (b0 >> (7 - x)) & 1;
            i1 = (b1 >> (7 - x)) & 1;
            *dst++ = (i1 << 1) | i0;
        }
    }
}

void    NES_Decode_Tiles (void)
{
    int    i;

    for (i = 0; i < 512; i++)
    {
        if (tgfx.Tile_Dirty[i] & TILE_DIRTY_DECODE)
            NES_Decode_Tile (i);
        tgfx.Tile_Dirty[i] = TILE_DIRTY_REDRAW;
    }
}

//-----------------------------------------------------------------------------
// NES_Palette_Set ()
// Setup NES palette and default references
//-----------------------------------------------------------------------------
void    NES_Palette_Set (void)
{
	int   i;

	// Set NES emulation palette
	for (i = 0; i != 32; i++)
		Palette_Emulation_SetColor(i, NES_Palette[PRAM[i] & 63]);
}

//-----------------------------------------------------------------------------

