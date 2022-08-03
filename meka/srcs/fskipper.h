//-----------------------------------------------------------------------------
// MEKA - fskipper.h
// Frame Skipper - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define FRAMESKIP_MODE_THROTTLED    (0)
#define FRAMESKIP_MODE_UNTHROTTLED  (1)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

bool    Frame_Skipper (void);
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
    int             Mode;                       // Automatic (sync) or standard
    int             Throttled_Speed;            // The requested framerate
    volatile int    Throttled_Frame_Elapsed;
    int             Unthrottled_Frameskip;
    int             Unthrottled_Counter;
    bool            Show_Current_Frame;

  // FPS Counter
  float           FPS;                        // The measured/estimated framerate
  bool            FPS_Display;                // Whether or not to display the FPS rate on screen
  int             FPS_SecondsElapsed;         // How many seconds have elapsed since FPS was last measured
  int             FPS_FrameCountAccumulator;  // Number of frames rendered since FPS was last measured
};

extern t_fskipper   fskipper;

//-----------------------------------------------------------------------------

