//-----------------------------------------------------------------------------
// MEKA - coleco.h
// Coleco Vision Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

byte   *BIOS_ROM_Coleco;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// FIXME: use WRITE_FUNC from mappers.h
void    Write_Mapper_Coleco (register word Addr, register byte Value);

byte    Coleco_Port_In (word Port);
void    Coleco_Port_Out (word Port, byte Value);

word    Loop_Coleco (void);

byte    Coleco_Joy_Table_Conv [64];
void    Coleco_Init_Table_Inputs (void);

byte    Coleco_Inputs   (word Port);
byte    Coleco_Keypad_1 (void);
byte    Coleco_Keypad_2 (void);

//-----------------------------------------------------------------------------


