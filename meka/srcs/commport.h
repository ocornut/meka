//-----------------------------------------------------------------------------
// MEKA - commport.h
// Gear-to-Gear Communication Port Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_gear_to_gear
{
 byte           Config;
 byte           Data_01;
 byte           Data_Direction_NMIe;
};

extern t_gear_to_gear  Gear_to_Gear;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Comm_Reset();

int     Comm_Read_01();
int     Comm_Read_02();
int     Comm_Read_03();
int     Comm_Read_04();
int     Comm_Read_05();

void    Comm_Write_01(int value);
void    Comm_Write_02(int value);
void    Comm_Write_03(int value);
void    Comm_Write_05(int value);

//-----------------------------------------------------------------------------

