//-----------------------------------------------------------------------------
// MEKA - mappers.h
// Memory Mapper Emulation - Headers
//-----------------------------------------------------------------------------

#define LOG_READ()      \
   { Msg(MSGT_DEBUG, Msg_Get(MSG_Debug_Trap_Read), CPU_GetPC, Addr); }

#define LOG_WRITE()      \
   { Msg(MSGT_DEBUG, Msg_Get(MSG_Debug_Trap_Write), CPU_GetPC, Value, Addr); }

#ifdef DEBUG_MEM
  void    Write_Error (int Addr, byte Value);
#else
  #define Write_Error(a,v)
#endif

//#define DEBUG_UNINITIALIZED_RAM_ACCESSES

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MAPPER_Auto                             (-1)
#define MAPPER_Standard                         (0)         // Standard Sega mapper.
#define MAPPER_32kRAM                           (1)
#define MAPPER_ColecoVision                     (2)
#define MAPPER_CodeMasters                      (3)
#define MAPPER_93c46                            (4)
#define MAPPER_SG1000                           (5)         // FIXME: Emulating 4KB RAM when it should be 2KB !#@?
#define MAPPER_SMS_ActionReplay                 (6)
#define MAPPER_TVOekaki                         (7)
#define MAPPER_SF7000                           (8)
#define MAPPER_SMS_Korean_A000                  (9)         // Register at 0xA000.
#define MAPPER_SMS_DisplayUnit                  (10)
#define MAPPER_SMS_NoMapper                     (11)        // 0x0000->0xBFFF map to ROM. No mapper register.
#define MAPPER_SMS_Korean_MSX_8KB_0003          (12)        // 8KB bank-switching (4 banks)
#define MAPPER_SMS_Korean_Janggun               (13)        // 8KB bank-switching (4 banks) mixed with 16KB bank-switching
#define MAPPER_SMS_4PakAllAction                (14)
#define MAPPER_SG1000_Taiwan_MSX_Adapter_TypeA  (15)        // 8KB RAM from 0x2000->0x3FFF + regular 2KB ram in 0xC000-0xFFFF range
#define MAPPER_SMS_Korean_FFFF_HiCom            (16)        // Register at 0xFFFF to map 32 KB at 0x0000->0x8000
#define MAPPER_SC3000_Survivors_Multicart       (17)
#define MAPPER_SMS_Korean_MSX_8KB_0300          (18)        // Registers at 0x0000,0x0100,0x0200,0x0300 (Super Multi Game Super 75 in 1, Super Game World 75 etc.)
#define MAPPER_SMS_Korean_2000_xor_1F           (19)        // Register at 0x2000 (128 Hap, Game Mo-eumjip 188 Hap etc.)
#define MAPPER_SMS_Korean_BFFC                  (20)        // Register at 0xBFFC
#define MAPPER_SMS_Korean_FFFE                  (21)        // Register at 0xFFFE (Super Game 45)
#define MAPPER_SMS_Korean_FFF3_FFFC             (22)        // Registers at 0xFFF3 and 0xFFFC (Super Game 150, Super Game 270)
#define MAPPER_SMS_Korean_0000_xor_FF           (23)        // Register at 0x0000 with MSX-oriented paging
#define MAPPER_SMS_Korean_MD_FFF0               (24)        // Registers at 0xFFF0 and 0xFFFF (Mega Mode Super Game 30 [SMS-MD])
#define MAPPER_SMS_Korean_MD_FFF5               (25)        // Registers at 0xFFF5 and 0xFFFF (Jaemiissneun Game Mo-eumjip 42/65 Hap [SMS-MD], Pigu Wang Hap ~ Jaemiiss-neun Game Mo-eumjip [SMS-MD])
#define MAPPER_SMS_Korean_MD_FFFA               (26)        // Registers at 0xFFFA and 0xFFFF (Game Jiphap 30 Hap [SMS-MD])
#define MAPPER_SMS_Korean_MSX_32KB_2000         (27)        // Register at 0x2000 (2 Hap in 1 (Moai-ui bomul, David-2))
#define MAPPER_GG_Super_GG_15_BFFF_FFFF         (29)        // Registers at 0xBFFF, 0xFFFF, and 0xFFFE (Super GG 15 [Cliffhanger] [Gold & Yellow Label])

#define READ_FUNC(_NAME)   u8 _NAME(register u16 Addr)
#define WRITE_FUNC(_NAME)  void _NAME(register u16 Addr, register u8 Value)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Mapper_InitializeLookupTables();
void    Mapper_Get_RAM_Infos(int *plen, int *pstart_addr);
int     Mapper_Autodetect();

// Memory Handler -------------------------------------------------------------
READ_FUNC  (Read_Default);
READ_FUNC  (Read_Mapper_93c46);
READ_FUNC  (Read_Mapper_TVOekaki);
READ_FUNC  (Read_Mapper_SMS_DisplayUnit);
READ_FUNC  (Read_Mapper_SMS_Korean_Janggun);
READ_FUNC  (Read_Mapper_SG1000_Taiwan_MSX_Adapter_TypeA);
//-----------------------------------------------------------------------------
WRITE_FUNC (Write_Default);
WRITE_FUNC (Write_Mapper_SG1000);
WRITE_FUNC (Write_Mapper_32kRAM);
WRITE_FUNC (Write_Mapper_CodeMasters);
WRITE_FUNC (Write_Mapper_93c46);
WRITE_FUNC (Write_Mapper_SMS_ActionReplay);
WRITE_FUNC (Write_Mapper_TVOekaki);
WRITE_FUNC (Write_Mapper_SF7000);
WRITE_FUNC (Write_Mapper_SMS_Korean_A000);
WRITE_FUNC (Write_Mapper_SMS_Korean_BFFC);
WRITE_FUNC (Write_Mapper_SMS_DisplayUnit);
WRITE_FUNC (Write_Mapper_SMS_NoMapper);
WRITE_FUNC (Write_Mapper_SMS_Korean_MSX_8KB_0003);
WRITE_FUNC (Write_Mapper_SMS_Korean_MSX_8KB_0300);
WRITE_FUNC (Write_Mapper_SMS_Korean_Janggun);
WRITE_FUNC (Write_Mapper_SMS_4PakAllAction);
WRITE_FUNC (Write_Mapper_SG1000_Taiwan_MSX_Adapter_TypeA);
WRITE_FUNC (Write_Mapper_SMS_Korean_FFFF_HiCom);
WRITE_FUNC (Write_Mapper_SMS_Korean_2000_xor_1F);
WRITE_FUNC (Write_Mapper_SMS_Korean_FFFE);
WRITE_FUNC (Write_Mapper_SMS_Korean_FFF3_FFFC);
WRITE_FUNC (Write_Mapper_SMS_Korean_0000_xor_FF);
WRITE_FUNC (Write_Mapper_SMS_Korean_MD_FFF0);
WRITE_FUNC (Write_Mapper_SMS_Korean_MD_FFF5);
WRITE_FUNC (Write_Mapper_SMS_Korean_MD_FFFA);
WRITE_FUNC (Write_Mapper_SMS_Korean_MSX_32KB_2000);
WRITE_FUNC (Write_Mapper_GG_Super_GG_15_BFFF_FFFF);
//-----------------------------------------------------------------------------
void Out_SC3000_SurvivorsMulticarts_DataWrite(u8 v);

extern void Map_8k_RAM    (int page, int ram_page);
extern void Map_8k_ROM    (int page, int rom_page);
extern void Map_8k_Other  (int page, void *);

extern void Map_16k_ROM   (int page, int rom_page);
extern void Map_16k_Other (int page, void *);

//-----------------------------------------------------------------------------

#ifdef DEBUG_UNINITIALIZED_RAM_ACCESSES
extern u8 RAM_IsUninitialized[0x2000];
#endif

//-----------------------------------------------------------------------------
