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

void    Machine_Pause               (void);
void    Machine_Debug_Start         (void);
void    Machine_Debug_Stop          (void);
void    Machine_Reset               (void);

void    Machine_Set_Mapper          (void);
void    Machine_Set_Mapping         (void);
void    Machine_Set_Handler_IO      (void);
void    Machine_Set_Handler_Loop    (void);
void    Machine_Set_Handler_Read    (void);
void    Machine_Set_Handler_Write   (void);
void    Machine_Set_Country         (void);
void    Machine_Set_IPeriod         (void);
void    Machine_Set_TV_Lines        (void);

//-----------------------------------------------------------------------------
