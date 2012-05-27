//-----------------------------------------------------------------------------
// MEKA - eeprom.h
// EEPROM Emulation (model 93c46) - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    BMemory_93c46_Load              (FILE *f);
void    BMemory_93c46_Save              (FILE *f);
void    BMemory_93c46_Load_State        (FILE *f);
void    BMemory_93c46_Save_State        (FILE *f);
void    BMemory_93c46_Get_Infos         (void **data, int *len);

//-----------------------------------------------------------------------------

void    EEPROM_93c46_Init               (int Init);
void    EEPROM_93c46_Clear              (void);
void    EEPROM_93c46_Control            (byte v);

void    EEPROM_93c46_Set_Lines          (byte lines);
byte    EEPROM_93c46_Read               (void);

void    EEPROM_93c46_Direct_Write       (int Addr, byte Data);
byte    EEPROM_93c46_Direct_Read        (int Addr);

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define EEPROM_93C46_LINE_DATA_IN       (0x01)
#define EEPROM_93C46_LINE_CLOCK         (0x02)
#define EEPROM_93C46_LINE_CS            (0x04)
#define EEPROM_93C46_LINE_DATA_OUT      (0x08)
#define EEPROM_93C46_LINE_DATA_OUT_POS  (3)

#define EEPROM_93C46_INIT_NORMAL        (0)
#define EEPROM_93C46_INIT_ALL           (1)

#define EEPROM_93C46_DATA_SIZE          (128) /* (64 * sizeof (word)) */

#define EEPROM_93C46_STATUS_START       (0) // Waiting for Start Bit
#define EEPROM_93C46_STATUS_OPCODE      (1) // Reading Opcode + Address
#define EEPROM_93C46_STATUS_READING     (2) // Clocking out data to output
#define EEPROM_93C46_STATUS_WRITING     (3) // Clocking in data to write

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_93c46
{
  byte          Enabled;
  byte          Lines;
  byte          Status;
  byte          Read_Only;
  byte          Position;
  word          Opcode;
  word          Latch;
  word          Data [EEPROM_93C46_DATA_SIZE/2]; // Data must be at end of structure
};

extern t_93c46  EEPROM_93c46;

// Legacy structure (for loading old MEKA savestates) -------------------------

/*
struct t_93c46_LEGACY
{
  byte          Action;
  byte          Enabled;
  byte          Wait;
  byte          Pos;
  byte          Opcode;
  byte          Address;
  byte          Dummy_Zero;
  word          Data_Temp;
  word          Data [64]; // _DATA_SIZE / sizeof(_DATA_TYPE)
  byte          EW_Enable;
  byte          Return_Read;
};
*/

//-----------------------------------------------------------------------------

