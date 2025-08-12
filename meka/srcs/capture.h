//-----------------------------------------------------------------------------
// MEKA - capture.h
// Screen Capture - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define CAPTURE_ID_MAX          (99999)      // Security measure for not going in infinite loop with short file name.
#define CAPTURE_DEFAULT_PREFIX  "meka"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Capture_Init();
void    Capture_Init_Game();
void    Capture_Request();
void    Capture_Update();

void    Capture_MenuHandler_Capture();
void    Capture_MenuHandler_AllFrames();

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_capture
{
    int         request;
    int         request_all_frames;
    int         id_number;
};

extern t_capture    Capture;

//-----------------------------------------------------------------------------

