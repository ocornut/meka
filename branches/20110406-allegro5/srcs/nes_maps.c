//-----------------------------------------------------------------------------
// MEKA - nes_maps.c
// Nintendo Mappers - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "nes.h"
#include "mappers.h"

//-----------------------------------------------------------------------------

t_nes_mapper    NES_Mappers[] =
{
  {  0,  NES_Mapper_0_Init,     NES_Mapper_0_Write,    NULL,                   NULL                     },
  {  1,  NES_Mapper_1_Init,     NES_Mapper_1_Write,    NES_Mapper_1_Load,      NES_Mapper_1_Save        },
  {  2,  NES_Mapper_2_Init,     NES_Mapper_2_Write,    NES_Mapper_2_Load,      NES_Mapper_2_Save        },
  {  3,  NES_Mapper_3_Init,     NES_Mapper_3_Write,    NES_Mapper_3_Load,      NES_Mapper_3_Save        },
  { -1,  NULL,                  NULL,                  NULL,                   NULL                     }
};

//-----------------------------------------------------------------------------

void            NES_Mapper_Set (int i)
{
  NES_Mapper = &NES_Mappers [i];
  if (NES_Mapper->Init)
     NES_Mapper->Init ();
  NES_Mapper_Write = NES_Mappers[i].Write;
}

void            NES_Mapper_Load (FILE *f)
{
  if (NES_Mapper->Load)
     NES_Mapper->Load (f);
}

void            NES_Mapper_Save (FILE *f)
{
  if (NES_Mapper->Save)
     NES_Mapper->Save (f);
}

//-----------------------------------------------------------------------------
// MAPPER 0
//-----------------------------------------------------------------------------

void        NES_Mapper_0_Init (void)
{
    int     i;

    ROM += 16;
    for (i = 0; i < 4; i++)
        Map_8k_ROM (4 + i, i & NES_Prg_Mask);
    ROM -= 16;
}

void    NES_Mapper_0_Write (word Addr, byte Value)
{
  // Nothing to do
}

//-----------------------------------------------------------------------------
// MAPPER 1
//-----------------------------------------------------------------------------

byte    NES_Mapper_1_Regs[4];
byte    NES_Mapper_1_Val;
byte    NES_Mapper_1_Count;

void    NES_Mapper_1_Init (void)
{
  NES_Mapper_1_Regs[0] = 0x0C;
  NES_Mapper_1_Regs[1] = 0x00;
  NES_Mapper_1_Regs[2] = 0x00;
  NES_Mapper_1_Regs[3] = 0x00;
  NES_Mapper_1_Val = 0x00;
  NES_Mapper_1_Count = 0;
  ROM += 16;
  Map_16k_ROM (4, 0);
  Map_16k_ROM (6, 2 * (NES_Prg_Cnt - 1)); // 2
  ROM -= 16;
}

void    NES_Mapper_1_Write_Reg (int Reg, byte Val)
{
  Val &= 0x1F;
  NES_Mapper_1_Regs[Reg] = Val;
  // Msg (MSGT_DEBUG, "[NES] Mapper 1 Write -> Reg[%d] = %02X", Reg, NES_Mapper_1_Regs[Reg]);
  ROM += 16; // FIXME: that sucks!
  switch (Reg)
    {
    case 0: // Register 0
            // Bit 0:   Mirroring (0: Vertical, 1: Horizontal)
            // Bit 1:   Mirroring (0: One screen mode, ignore Bit 1, 1: H/V, use bit 0)
            // Bit 2:   PRGROM Area (0: switch high bank, 1: switch low bank)
            // Bit 3:   PRGROM Bank Size (0: 32 kb, ignore Bit 3, 1: 16 kb)
            // Bit 4:   CHRROM Bank Size (0: 8 kb, 1: 4 kb)
            if (Val & 2)
               NES_PPU_Set_Mirroring ((Val & 1) ? NES_MIRRORING_H : NES_MIRRORING_V);
            else
               NES_PPU_Set_Mirroring (NES_MIRRORING_NO);
            break;
    case 1: // Register 1
            if (NES_Chr_Cnt == 0)
               break;
            if (NES_Mapper_1_Regs[0] & 0x10)
               { // 4 kb banks
               NES_PPU_Map (0, 4, NES_Chr + Val * 0x1000);
               }
            else
               { // 8 kb banks
               NES_PPU_Map (0, 8, NES_Chr + (Val / 2) * 0x2000);
               }
            break;
    case 2: // Register 2
            if (NES_Chr_Cnt == 0)
               break;
            if (NES_Mapper_1_Regs[0] & 0x10)
               { // 4 kb banks
               NES_PPU_Map (4, 4, NES_Chr + Val * 0x1000);
               }
            break;
    case 3: // Register 3
            Val &= 0x0F;
            Val &= (NES_Prg_Cnt - 1);
            if (NES_Mapper_1_Regs[0] & 0x08)
               { // 16 kb banks mode
               if (NES_Mapper_1_Regs[0] & 0x04)
                  { // Switch low bank
                  Map_16k_ROM (4, 2 * Val);
                  Map_16k_ROM (6, 2 * (NES_Prg_Cnt - 1));
                  }
               else
                  { // Switch high bank
                  Map_16k_ROM (4, 2 * 0);
                  Map_16k_ROM (6, 2 * Val);
                  }
               }
            else
               { // 32 kb banks mode
               Map_16k_ROM (4, 2 * ((Val / 2) + 0));
               Map_16k_ROM (6, 2 * ((Val / 2) + 1));
               }
            break;
    }
  ROM -= 16; // FIXME: that sucks!
}

void    NES_Mapper_1_Write (word Addr, byte Value)
{
  int   Reg = (Addr >> 13) - 4;

  // Msg (MSGT_DEBUG, "[NES] Mapper 1 Write -> %04X = %02X", Addr, Value);
  if (Value & 0x80) // Reset
     {
     NES_Mapper_1_Count = 0;
     NES_Mapper_1_Val = 0x00;
     NES_Mapper_1_Regs[0] |= (0x04 | 0x08);
     return;
     }
  NES_Mapper_1_Val |= (Value & 1) << NES_Mapper_1_Count;
  if (++NES_Mapper_1_Count < 5)
     return;
  NES_Mapper_1_Count = 0;
  NES_Mapper_1_Write_Reg (Reg, NES_Mapper_1_Val);
  NES_Mapper_1_Val = 0x00;
}

void    NES_Mapper_1_Load (FILE *f)
{
  int   i;
  for (i = 0; i < 4; i++)
      fread (&NES_Mapper_1_Regs[i], sizeof (byte), 1, f);
  fread (&NES_Mapper_1_Val, sizeof (byte), 1, f);
  fread (&NES_Mapper_1_Count, sizeof (byte), 1, f);
  for (i = 0; i < 4; i++)
      NES_Mapper_1_Write_Reg (i, NES_Mapper_1_Regs[i]);
}

void    NES_Mapper_1_Save (FILE *f)
{
  int   i;
  for (i = 0; i < 4; i++)
      fwrite (&NES_Mapper_1_Regs[i], sizeof (byte), 1, f);
  fwrite (&NES_Mapper_1_Val, sizeof (byte), 1, f);
  fwrite (&NES_Mapper_1_Count, sizeof (byte), 1, f);
}

//-----------------------------------------------------------------------------
// MAPPER 2
//-----------------------------------------------------------------------------

byte    NES_Mapper_2_Reg;

void    NES_Mapper_2_Init (void)
{
  NES_Mapper_2_Reg = 0;
  ROM += 16;
  Map_16k_ROM (4, 0);
  Map_16k_ROM (6, 2 * (NES_Prg_Cnt - 1));
  ROM -= 16;
}

void    NES_Mapper_2_Write (word Addr, byte Value)
{
  NES_Mapper_2_Reg = Value;
  ROM += 16;
  Map_16k_ROM (4, 2 * Value);
  ROM -= 16;
}

void    NES_Mapper_2_Load (FILE *f)
{
 fread (&NES_Mapper_2_Reg, sizeof (byte), 1, f);
 NES_Mapper_2_Write (0x8000, NES_Mapper_2_Reg);
}

void    NES_Mapper_2_Save (FILE *f)
{
 fwrite (&NES_Mapper_2_Reg, sizeof (byte), 1, f);
}

//-----------------------------------------------------------------------------
// MAPPER 3
//-----------------------------------------------------------------------------

byte    NES_Mapper_3_Reg;

void    NES_Mapper_3_Init (void)
{
  int   i;

  ROM += 16;
  for (i = 0; i < 4; i++)
      Map_8k_ROM (4 + i, i & NES_Prg_Mask);
  ROM -= 16;
  NES_Mapper_3_Reg = 0;
  NES_PPU_Map (0, 8, NES_Chr);
}

void    NES_Mapper_3_Write (word Addr, byte Value)
{
  NES_Mapper_3_Reg = Value;
  NES_PPU_Map (0, 8, NES_Chr + (Value & NES_Chr_Mask) * 0x2000);
}

void    NES_Mapper_3_Load (FILE *f)
{
 fread (&NES_Mapper_3_Reg, sizeof (byte), 1, f);
 NES_Mapper_3_Write (0x8000, NES_Mapper_3_Reg);
}

void    NES_Mapper_3_Save (FILE *f)
{
 fwrite (&NES_Mapper_3_Reg, sizeof (byte), 1, f);
}

//-----------------------------------------------------------------------------

