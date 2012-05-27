
// from saves.h

#define MSV_VERSION (0x07)

// from meka.h
// note: this structure really stinks, I avoided to tweak it
// since it was originally directly saved as is
// MARAT_Z80 is #defined in all release

struct SMS_TYPE
{
 #ifdef MARAT_Z80
  Z80 R;                                // CPU Registers (Marat Faizullin)
 #elif NEIL_Z80
  CONTEXTMZ80 CPU;                      // CPU Registers (Neil Bradley)
 #elif MDK_Z80
  Z80_Regs R;                           // CPU Registers (Marcel de Kogel)
 #elif RAZE_Z80
  void *CPU;                            // CPU Registers (Richard Mitton)
 #elif MAME_Z80
  // nothing. currently implemented as global
 #endif
 byte    VDP [16];                      // VDP Registers
 byte    PRAM_Address;                  // Current address for PRAM accesses
 byte    VDP_Status;                    // Current VDP status
 word    VDP_Address;                   // Current VDP address
 byte    VDP_Access_Mode;               // 0: Address Low - 1: Address High
 byte    VDP_Access_First;              // To save Address Low
 byte    VDP_Read;
 byte    VDP_Pal;
 byte    Country;                       // 0: English - 1: Japanese
 int     Lines_Left;
 byte    Need_HBlank;
 byte    Need_VBlank;
 byte    Glasses_Register;
 byte    SRAM_Pages;                    // SRAM pages used
 byte    Mapping_Register;              // SRAM status + mapping offset
 byte    FM_Magic;
 byte    FM_Reg;
 byte    Input_Mode;   // Port 0xDE     // 0->6: Keyboard - 7: Joypads
 byte    Pages_Reg [3];                 // Paging registers
};

// from drivers.h

#define DRV_SMS      (0)
#define DRV_GG       (1)
#define DRV_SG1000   (2)
#define DRV_SC3000   (3)
#define DRV_COLECO   (4)
#define DRV_MSX      (5)
#define DRV_NES      (6)
#define DRV_SF7000   (7)
#define DRV_MAX      (8)

#define VDP_SMSGG    (0)
#define VDP_TMS      (1)
#define VDP_NES      (2)

#define SND_SN76489  (0)
#define SND_NES      (1)

// from commport.h

typedef struct  s_gear_to_gear
{
 byte           Config;
 byte           Data_01;
 byte           Data_Direction_NMIe;
}               t_gear_to_gear;

// from tvoekaki.h

typedef struct  s_tvoekaki
{
 int            X, Y;
 byte           Infos;
}               t_tvoekaki;

// from sf7000.h

typedef struct  s_sf7000
{
 byte           Port_E4, Port_E5, Port_E6, Port_E7;
 byte           Port_E8, Port_E9;
}               t_sf7000;

// from saves.c

// LOAD CURRENT GAME IN MEKA FORMAT -------------------------------------------
int     Load_Game_MSV (FILE *f)
{
 char   str[12];
 byte   i, version;

 fread (&str, 4, 1, f);
 str[4] = 0;
 if (StrCmp(str, "MEKA") != 0)
    {
    if (StrCmp(str, "S8B ") == 0)
       return (Load_Game_MSD (f));
    return (3); // not a savegame
    }
 fseek (f, 5, SEEK_SET);
 fread (&version, 1, 1, f);
 if (version > MSV_VERSION)
    return (4); // version not known

 fread (&i, 1, 1, f);
 if (version >= 0x05)
    {
    if (i != cur_drv->id)
        return (5); // not the same machine
    }
 else
    {
    // Old kind of machine identification
    if (i != drv_id_to_mode (cur_drv->id))
       return (5); // not the same machine
    }

 fread (&sms, sizeof (struct SMS_TYPE), 1, f);
 // FIXME: RAM size should be based on value got from current driver
 switch (tsms.Mapper)
    {
    case MAPPER_32kRAM:       fread (RAM, 0x08000, 1, f);
                              break;
    case MAPPER_ColecoVision: if (version >= 0x06)
                                 fread (RAM, 0x00400, 1, f);
                              else
                                 fread (RAM, 0x02000, 1, f);
                              break;
    case MAPPER_SG1000:       if (version >= 0x06)
                                 fread (RAM, 0x01000, 1, f);
                              else
                                 fread (RAM, 0x02000, 1, f);
                              break;
    case MAPPER_TVOekaki:     fread (RAM, 0x01000, 1, f);
                              fread (&TVOekaki, sizeof (TVOekaki), 1, f);
                              break;
    case MAPPER_SF7000:       fread (RAM, 0x10000, 1, f);
                              fread (&SF7000, sizeof (SF7000), 1, f);
                              break;
    default:                  fread (RAM, cur_drv->ram, 1, f);
                              break;
    }
 fread (VRAM, 0x4000, 1, f);
 switch (cur_drv->id)
    {
    case DRV_SMS:
         fread (PRAM, 32, 1, f);
         break;
    case DRV_GG:
         fread (PRAM, 64, 1, f);
         fread (&Gear_to_Gear, sizeof (struct G2G_TYPE), 1, f);
         break;
    case DRV_NES:
         fread (nes, sizeof (struct NES_TYPE), 1, f);
         fread (PRAM, 32, 1, f); // Isn't that pointing to VRAM ?!
         NES_Mapper_Load (f);
         break;
    }
 if (version >= 0x03)
    {
    fread (FM_Regs, 0x40, 1, f);
    }
 if (version >= 0x04)
    {
    fread (&tsms.Periph_Nat, 1, 1, f);
    }
 if (version <= 0x06)
    {
    sms.SRAM_Pages *= 2;
    }
 BMemory_Load_Append (f);
 return (1);
}

// from bmemory.c

void    BMemory_Load_Append (FILE *f)
{
 switch (tsms.Mapper)
    {
    case MAPPER_Standard:       BMemory_SRAM_Load_Append (f);  break;
    case MAPPER_93c46:          BMemory_93c46_Load_Append (f); break;
    }
}

void    BMemory_SRAM_Load_Append (FILE *f)
{
 fread (SRAM, sms.SRAM_Pages * 0x2000, 1, f);
 if (sms.SRAM_Pages < 4)
    memset (SRAM + sms.SRAM_Pages * 0x2000, 0, (4 - sms.SRAM_Pages) * 0x2000);
}

// from eeprom.c

void BMemory_93c46_Load_Append (FILE *f)
{
 fread (BM_93c46.Data, BM_93C46_DATA_SIZE, 1, f);
}

// from eeprom.h

typedef struct  s_93c46
{
  byte          Action;
  byte          Enabled;
  byte          Wait;
  byte          Pos;
  byte          Opcode;
  byte          Address;
  byte          Dummy_Zero;
  word          Data_Temp;
  word          Data [64]; // _DATA_SIZE
  byte          EW_Enable;
  byte          Return_Read;
}               t_93c46;

// from nes.h

typedef struct  s_nes
 {
   M6502        Regs;
   byte         Joy_Strobe;
   byte         Joy_Status;
   int          Joy_Status_Read;
   byte         Object_RAM [256];
   byte         Object_ADX;
   byte         PPU_Read;
   byte         PPU_Read_Latch;
   byte         CR0, CR1;
   byte         Toggle;
   byte         Scroll [2];
   int          Mirroring;
   #ifdef NEW_S0HIT
      int       Sprite_0_Hit;
   #endif
 }              t_nes;

// from nes_maps.c

void            NES_Mapper_Load (FILE *f)
{
  if (NES_Mapper->Load)
     NES_Mapper->Load (f);
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

void    NES_Mapper_2_Load (FILE *f)
{
 fread (&NES_Mapper_2_Reg, sizeof (byte), 1, f);
 NES_Mapper_2_Write (0x8000, NES_Mapper_2_Reg);
}

void    NES_Mapper_3_Load (FILE *f)
{
 fread (&NES_Mapper_3_Reg, sizeof (byte), 1, f);
 NES_Mapper_3_Write (0x8000, NES_Mapper_3_Reg);
}


