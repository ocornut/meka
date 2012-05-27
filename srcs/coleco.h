//-----------------------------------------------------------------------------
// MEKA - coleco.h
// Coleco Vision Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// FIXME: use WRITE_FUNC from mappers.h
void    Write_Mapper_Coleco (register word Addr, register byte Value);

byte    Coleco_Port_In (word Port);
void    Coleco_Port_Out (word Port, byte Value);

word    Loop_Coleco (void);

void    Coleco_Init_Table_Inputs (void);

byte    Coleco_Inputs(word Port);
byte    Coleco_Keypad_1(void);
byte    Coleco_Keypad_2(void);

//-----------------------------------------------------------------------------


