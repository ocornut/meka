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

#define MAPPER_Auto								(-1)
#define MAPPER_Standard							(0)			// Standard Sega mapper.
#define MAPPER_32kRAM							(1)
#define MAPPER_ColecoVision						(2)
#define MAPPER_CodeMasters						(3)
#define MAPPER_93c46							(4)
#define MAPPER_SG1000							(5)			// FIXME: Emulating 4KB RAM when it should be 2KB !#@?
#define MAPPER_SMS_ActionReplay					(6)
#define MAPPER_TVOekaki							(7)
#define MAPPER_SF7000							(8)
#define MAPPER_SMS_Korean						(9)
#define MAPPER_SMS_DisplayUnit					(10)
#define MAPPER_SMS_NoMapper						(11)		// 0x0000->0xBFFF map to ROM. No mapper register.
#define MAPPER_SMS_Korean_MSX_8KB				(12)		// 8KB bank-switching (4 banks)
#define MAPPER_SMS_Korean_Janggun				(13)		// 8KB bank-switching (4 banks) mixed with 16KB bank-switching
#define MAPPER_SMS_4PakAllAction				(14)
#define MAPPER_SG1000_Taiwan_MSX_Adapter_TypeA	(15)		// 8KB RAM from 0x2000->0x3FFF + regular 2KB ram in 0xC000-0xFFFF range
#define MAPPER_SMS_Korean_Xin1					(16)		// Mapping register at 0xFFFF to map 32 KB at 0x0000->0x8000
#define MAPPER_SC3000_Survivors_Multicart		(17)

#define READ_FUNC(_NAME)   u8 _NAME(register u16 Addr)
#define WRITE_FUNC(_NAME)  void _NAME(register u16 Addr, register u8 Value)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void	Mapper_InitializeLookupTables();
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
WRITE_FUNC (Write_Mapper_SMS_Korean);
WRITE_FUNC (Write_Mapper_SMS_DisplayUnit);
WRITE_FUNC (Write_Mapper_SMS_NoMapper);
WRITE_FUNC (Write_Mapper_SMS_Korean_MSX_8KB);
WRITE_FUNC (Write_Mapper_SMS_Korean_Janggun);
WRITE_FUNC (Write_Mapper_SMS_4PakAllAction);
WRITE_FUNC (Write_Mapper_SG1000_Taiwan_MSX_Adapter_TypeA);
WRITE_FUNC (Write_Mapper_SMS_Korean_Xin1);
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
