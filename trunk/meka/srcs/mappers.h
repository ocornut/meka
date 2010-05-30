//-----------------------------------------------------------------------------
// MEKA - mappers.h
// Memory Mapper Emulation - Headers
//-----------------------------------------------------------------------------

#define LOG_READ()      \
   { Msg (MSGT_DEBUG, Msg_Get (MSG_Debug_Trap_Read), CPU_GetPC, Addr); }

#define LOG_WRITE()      \
   { Msg (MSGT_DEBUG, Msg_Get (MSG_Debug_Trap_Write), CPU_GetPC, Value, Addr); }

#ifdef DEBUG_MEM
  void    Write_Error (int Addr, byte Value);
#else
  #define Write_Error(a,v)
#endif

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MAPPER_Auto                     (-1)
#define MAPPER_Standard                 (0)			// Standard Sega mapper.
#define MAPPER_32kRAM                   (1)
#define MAPPER_ColecoVision             (2)
#define MAPPER_CodeMasters              (3)
#define MAPPER_93c46                    (4)
#define MAPPER_SG1000                   (5)
#define MAPPER_SMS_ActionReplay         (6)
#define MAPPER_TVOekaki                 (7)
#define MAPPER_SF7000                   (8)
#define MAPPER_SMS_Korean               (9)
#define MAPPER_SMS_DisplayUnit          (10)
#define MAPPER_SMS_NoMapper				(11)		// 0x0000->0xBFFF map to ROM. No mapper register.
#define MAPPER_SMS_Korean_MSX_Ascii_8   (12)

#ifdef MAME_Z80
 #define READ_FUNC(a)   int  a  (int Addr)
 #define WRITE_FUNC(a)  void a  (int Addr, int Value)
#else
 #define READ_FUNC(a)   u8 a    (register word Addr)
 #define WRITE_FUNC(a)  void a  (register word Addr, register u8 Value)
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Mapper_Get_RAM_Infos (int *plen, int *pstart_addr);
int     Mapper_Autodetect (void);

// Memory Handler -------------------------------------------------------------
READ_FUNC  (Read_Default);
READ_FUNC  (Read_Mapper_93c46);
READ_FUNC  (Read_Mapper_TVOekaki);
READ_FUNC  (Read_Mapper_SMS_DisplayUnit);
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
WRITE_FUNC (Write_Mapper_SMS_Korean_MSX_Ascii_8);
//-----------------------------------------------------------------------------

extern INLINE void Map_8k_RAM    (int page, int ram_page);
extern INLINE void Map_8k_ROM    (int page, int rom_page);
extern INLINE void Map_8k_Other  (int page, void *);

extern INLINE void Map_16k_ROM   (int page, int rom_page);
extern INLINE void Map_16k_Other (int page, void *);

//-----------------------------------------------------------------------------
