
// from saves.h ---------------------------------------------------------------

#define MSV_VERSION (0x0C)

// 0x06 -> 0x07
// - sms.SRAM_Pages is multiplied by two
//   Now a page is 8 kb instead of 16 kb (because of NES)

// 0x07 -> 0x08
// - saved PSG data

// 0x08 -> 0x09
// - rewrote 93c46 emulation
//   93c46 data of version <= 0x07 is not loaded

// 0x09 -> 0x0A
// - upgraded Z80 core, IFF_2 flag in IFF is now 0x08 instead of 0x40
//   old save states will copy the bit

// 0x0A -> 0x0B
// - updated PSG implementation and changed saved data (other than registers)

// 0x0B -> 0x0C
// - added ROM CRC32 (not used now, but will be useful for tools in the future)

// from saves.c ---------------------------------------------------------------

//-----------------------------------------------------------------------------
// Load_Game_MSV (FILE *f)
// Load state from given file, in MEKA save state format
//-----------------------------------------------------------------------------
int         Load_Game_MSV (FILE *f)
{
    char    buf[5];
    u8      b, version;

    fread (&buf, 4, 1, f);
    buf[4] = 0;
    if (strcmp(buf, "MEKA") != 0)
    {
        if (strcmp(buf, "S8B ") == 0)
            return (Load_Game_MSD (f));
        return (3); // not a savegame
    }
    fseek (f, 5, SEEK_SET);
    fread (&version, 1, 1, f);
    if (version > MSV_VERSION)
        return (4); // unsupported version

    // Msg (MSGT_DEBUG, "Loading, version = %02X", version);

    // Read driver id
    fread (&b, 1, 1, f);
    if (version >= 0x05)
    {
        if (b != cur_drv->id)
            return (5); // not the same machine
    }
    else
    {
        // Old kind of machine identification
        if (b != drv_id_to_mode (cur_drv->id))
            return (5); // not the same machine
    }

    // CRC32, introduced in version 0x0C. Currently skip this value.
    if (version >= 0x0C)
    {
        u32 crc;
        fread (&crc, sizeof (u32), 1, f);
    }

    // 'sms' structure (including CPU stuff)
    {
        // Backup then restore debugging purpose data
        // So they're not lost when loading a state while debugging
        u16 trap = sms.R.Trap;
        u8  trace = sms.R.Trace;
        fread (&sms, sizeof (struct SMS_TYPE), 1, f);
        sms.R.Trap = trap;
        sms.R.Trace = trace;
    }

    // Read RAM & mapper specific data
    // FIXME: RAM size should be based on value got from current driver
    switch (tsms.Mapper)
    {
    case MAPPER_32kRAM:
        fread (RAM, 0x08000, 1, f);
        break;
    case MAPPER_ColecoVision:
        if (version >= 0x06)
            fread (RAM, 0x00400, 1, f);
        else
            fread (RAM, 0x02000, 1, f); // Previously saved 8 Kb instead of 1 kb
        break;
    case MAPPER_SG1000:
        if (version >= 0x06)
            fread (RAM, 0x01000, 1, f);
        else
            fread (RAM, 0x02000, 1, f); // Previously saved 8 Kb instead of 4 Kb
        break;
    case MAPPER_TVOekaki:
        fread (RAM, 0x01000, 1, f);
        fread (&TVOekaki, sizeof (TVOekaki), 1, f);
        break;
    case MAPPER_SF7000:
        fread (RAM, 0x10000, 1, f);
        fread (&SF7000, sizeof (SF7000), 1, f);
        break;
    case MAPPER_CodeMasters:
        if (OnBoard_RAM_Exist) // Ernie Els Golf Onboard RAM
            fread (RAM, cur_drv->ram + 0x2000, 1, f);
        else
            fread (RAM, cur_drv->ram, 1, f);
        break;
    default:
        fread (RAM, cur_drv->ram, 1, f);
        break;
    }

    // Read VRAM
    fread (VRAM, 0x4000, 1, f);

    // Read Palette
    switch (cur_drv->id)
    {
    case DRV_SMS:
        fread (PRAM, 32, 1, f);
        break;
    case DRV_GG:
        fread (PRAM, 64, 1, f);
        fread (&Gear_to_Gear, sizeof (t_gear_to_gear), 1, f);
        break;
    case DRV_NES:
        fread (nes, sizeof (t_nes), 1, f);
        fread (PRAM, 32, 1, f); // Isn't that pointing to VRAM ?!
        NES_Mapper_Load (f);
        break;
    }

    // Read PSG data
    if (version >= 0x08)
        PSG_Load (f, version);
    else
    {
        // ...If not available, mute PSG volumes by default
        // SN76489_Reset (opt.CPU_Clock_Current, audio_sample_rate);
        PSG_Mute ();
        // There is a reason for muting the PSG volumes instead of reseting it all
        // when the PSG data is missing: a same game is more likely to have noises 
        // setup the same way, so we want to keep current settings.
    }

    // Read FM data
    if (version >= 0x03)
        FM_Load (f);

    // Read port 3F value
    if (version >= 0x04)
        fread (&tsms.Periph_Nat, 1, 1, f);

    // Previously stored 16 kb pages instead of 8 kb pages
    if (version < 0x07)
        sms.SRAM_Pages *= 2;

    // Read backed memory unless version<0x09 & emulation 93c46
    if (!(version < 0x09 && tsms.Mapper == MAPPER_93c46))
        BMemory_Load_State (f);

    // Fix up CPU registers
    // Copy IFF2 bit for old Z80 Registers structure
    if (version < 0x0A)
        sms.R.IFF = ((sms.R.IFF & 0xB7) | ((sms.R.IFF & 0x40) ? IFF_2 : 0x00));

    return (1);
}

// from meka.h ----------------------------------------------------------------
// note: this structure really stinks, I avoided to tweak it
// since it was originally directly saved as is
// MARAT_Z80 is #defined in all MEKA release

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
 // NOTE: variable below (VDP_Status) is modified from videoasm.asm, do NOT move it
 byte    VDP_Status;                    // Current VDP status
 word    VDP_Address;                   // Current VDP address
 byte    VDP_Access_Mode;               // 0: Address Low - 1: Address High
 byte    VDP_Access_First;              // Address Low Latch
 byte    VDP_Read;                      // Read Latch
 byte    VDP_Pal;                       // Currently Reading Palette ?
 byte    Country;                       // 0: English - 1: Japanese
 int     Lines_Left;                    // Lines Left before H-Blank
 byte    Need_HBlank;
 byte    Need_VBlank;
 byte    Glasses_Register;              // 3-D Glasses Register
 byte    SRAM_Pages;                    // SRAM pages used
 byte    Mapping_Register;              // SRAM status + mapping offset
 byte    FM_Magic;                      // FM Latch (for detection)
 byte    FM_Register;                   // FM Register
 byte    Input_Mode;   // Port 0xDE     // 0->6: Keyboard - 7: Joypads
 byte    Pages_Reg [3];                 // Paging registers
};

// from drivers.h -------------------------------------------------------------

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

// from commport.h ------------------------------------------------------------

typedef struct  s_gear_to_gear
{
 byte           Config;
 byte           Data_01;
 byte           Data_Direction_NMIe;
}               t_gear_to_gear;

// from tvoekaki.h ------------------------------------------------------------

typedef struct  s_tvoekaki
{
 int            X, Y;
 byte           Infos;
}               t_tvoekaki;

// from sf7000.h --------------------------------------------------------------

typedef struct  s_sf7000
{
 byte           Port_E4, Port_E5, Port_E6, Port_E7;
 byte           Port_E8, Port_E9;
}               t_sf7000;

// from bmemory.c

void    BMemory_Load_State (FILE *f)
{
  switch (tsms.Mapper)
     {
     case MAPPER_Standard:       BMemory_SRAM_Load_State (f);  break;
     case MAPPER_93c46:          BMemory_93c46_Load_State (f); break;
     }
}

void    BMemory_SRAM_Load_State (FILE *f)
{
  fread (SRAM, sms.SRAM_Pages * 0x2000, 1, f);
  if (sms.SRAM_Pages < 4)
     memset (SRAM + sms.SRAM_Pages * 0x2000, 0, (4 - sms.SRAM_Pages) * 0x2000);
}

// from eeprom.c --------------------------------------------------------------

void    BMemory_93c46_Load_State (FILE *f)
{
  fread (&EEPROM_93c46, sizeof (EEPROM_93c46), 1, f);
}

// from eeprom.h --------------------------------------------------------------

typedef struct  s_93c46
{
  byte          Enabled;
  byte          Lines;
  byte          Status;
  byte          Read_Only;
  byte          Position;
  word          Opcode;
  word          Latch;
  word          Data [EEPROM_93C46_DATA_SIZE/2]; // Data must be at end of structure
}               t_93c46;

// from nes.h -----------------------------------------------------------------
// (scrapped)
// (nobody wants to read or write MEKA NES savestates)

// from psg.h -----------------------------------------------------------------

typedef struct
{
    signed short int    ToneFreqVal;            // Frequency register values (counters)
    signed       char   ToneFreqPos;            // Frequency channel flip-flops
    signed long  int    IntermediatePos;        // Intermediate values used at boundaries between + and -
  unsigned short int	Volume;                 // Current channel volume (0-900+...)
                 int    Active;                 // Set to 0 to mute
}                       t_psg_channel;

typedef struct
{
  t_psg_channel         Channels[4];            //
  unsigned short int	Registers[8];           //
                 int    LatchedRegister;        //
  unsigned       char   Stereo;                 //
  unsigned short int    NoiseShiftRegister;     //
    signed short int	NoiseFreq;              // regenerate
                 float  Clock;
                 float  dClock;
  unsigned       int    NumClocksForSample;
                 int    SamplingRate;           // fixed
}                       t_psg;

extern t_psg            PSG;
// from psg.c -----------------------------------------------------------------

//-----------------------------------------------------------------------------
// PSG_Load()
// Load PSG state from a file
//-----------------------------------------------------------------------------
void        PSG_Load (FILE *f, int version)
{
    int     i;

    // PSG Registers
    fread (PSG.Registers, 8, sizeof (word), f);
    if (version < 0x0B)
    {
        // Legagy Loading
        char dummy_buffer[5];
        fread (dummy_buffer, 4+1, sizeof (char), f);
        PSG.LatchedRegister = 0;
        PSG.Stereo = 0xFF;
        PSG.NoiseShiftRegister = NoiseInitialState;
        for (i = 0; i < 4; i++)
        {
            // Fixup zero frequencies (FIXME: this is implementation dependant!)
            if (i < 3)
                if (PSG.Registers[i * 2] == 0)
                    PSG.Registers[i * 2] = 1;
            // Set counters to 0
            PSG.Channels[i].ToneFreqVal = 0;
            // Set flip-flops to 1
            PSG.Channels[i].ToneFreqPos = 1;
            // Set intermediate positions to do-not-use value
            PSG.Channels[i].IntermediatePos = LONG_MIN;
        }
    }
    else
    {
        byte b;
        word w;

        // PSG Data
        fread (&b, 1, sizeof (byte), f);
        PSG.LatchedRegister = b;
        fread (&b, 1, sizeof (byte), f);
        PSG.Stereo = b;
        fread (&w, 1, sizeof (word), f);
        PSG.NoiseShiftRegister = w;
        // Implemention Data
        for (i = 0; i < 4; i++)
        {
            fread (&PSG.Channels[i].ToneFreqVal,     1, sizeof (signed short int), f);
            fread (&PSG.Channels[i].ToneFreqPos,     1, sizeof (signed char),      f);
            fread (&PSG.Channels[i].IntermediatePos, 1, sizeof (signed long int),  f);
        }
    }
    PSG_Regenerate ();
}
