//-----------------------------------------------------------------------------
// MEKA - glasses.h
// 3D Glasses Support and Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define GLASSES_MODE_SHOW_BOTH          (0)
#define GLASSES_MODE_SHOW_ONLY_LEFT     (1)
#define GLASSES_MODE_SHOW_ONLY_RIGHT    (2)
#define GLASSES_MODE_COM_PORT           (3)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_glasses
{
    bool        Enabled;
    int         Mode;
    int         ComPort;
#ifdef ARCH_WIN32
    HANDLE      ComHandle;
#endif
};

extern t_glasses   Glasses;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Glasses_Init_Values();
void    Glasses_Close();
void    Glasses_Set_Mode(int mode);
void    Glasses_Set_ComPort(int port);
void    Glasses_Write(int LeftEnable, int RightEnable);
void    Glasses_Update();

int     Glasses_Must_Skip_Frame();

void    Glasses_Switch_Enable();
void    Glasses_Switch_Mode_Show_Both();
void    Glasses_Switch_Mode_Show_Left();
void    Glasses_Switch_Mode_Show_Right();
void    Glasses_Switch_Mode_Com_Port();

//-----------------------------------------------------------------------------

