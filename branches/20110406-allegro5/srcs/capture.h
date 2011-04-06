//-----------------------------------------------------------------------------
// MEKA - capture.h
// Screen Capture - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define CAPTURE_ID_MAX          (9999)       // Security measure for not going in infinite loop with short file name.
#define CAPTURE_DEFAULT_PREFIX  "meka"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Capture_Init					(void);
void    Capture_Init_Game				(void);
void    Capture_Request					(void);
void	Capture_Update					(void);

void	Capture_MenuHandler_Capture		(void);
void	Capture_MenuHandler_AllFrames	(void);
void	Capture_MenuHandler_IncludeGui	(void);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
	int         request;
	int			request_all_frames;
	int         id_number;
}               t_capture;

t_capture       Capture;

//-----------------------------------------------------------------------------

