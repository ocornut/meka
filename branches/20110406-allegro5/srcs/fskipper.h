//-----------------------------------------------------------------------------
// MEKA - fskipper.h
// Frame Skipper - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define FRAMESKIP_MODE_AUTO     (0)
#define FRAMESKIP_MODE_STANDARD (1)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

bool    Frame_Skipper (void);
void    Frame_Skipper_Auto_Adjust_Handler (void);
void    Frame_Skipper_Auto_Install_Handler (void);
void    Frame_Skipper_Auto_Reinstall_Handler (void);
void    Frame_Skipper_Configure (int v);
void    Frame_Skipper_Switch (void);
void    Frame_Skipper_Switch_FPS_Counter (void);
void    Frame_Skipper_Show (void);
void    Frame_Skipper_Init (void);
void    Frame_Skipper_Init_Values (void);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_fskipper
{
    // Frame skipper    
    int             Mode;						// Automatic (sync) or standard
    int             Automatic_Speed;
    volatile int    Automatic_Frame_Elapsed;
    int             Standard_Frameskip;
    int             Standard_Counter;
    bool            Show_Current_Frame;

    // FPS Counter
    float			FPS;
    bool            FPS_Display;
	int				FPS_SecondsElapsed;
    int             FPS_FrameCountAccumulator;	// Number of frames rendered (this second)
};

extern t_fskipper	fskipper;

//-----------------------------------------------------------------------------

