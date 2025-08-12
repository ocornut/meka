//-----------------------------------------------------------------------------
// MEKA - machine.h
// Emulated Machines Initialization - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

extern int g_machine_pause_requests;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Machine_Pause();
void    Machine_Debug_Start();
void    Machine_Debug_Stop();
void    Machine_Reset();

void    Machine_Set_Mapper();
void    Machine_Set_Mapping();
void    Machine_Set_Handler_IO();
void    Machine_Set_Handler_Loop();
void    Machine_Set_Handler_MemRW();
void    Machine_Set_Country();
void    Machine_Set_IPeriod();
void    Machine_Set_TV_Lines();

//-----------------------------------------------------------------------------
