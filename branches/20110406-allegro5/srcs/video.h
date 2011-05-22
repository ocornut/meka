//-----------------------------------------------------------------------------
// MEKA - video.h
// Video / Miscellaenous - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_video
{
    int		driver;							// Current driver
    int		res_x, res_y;					// Current resolution
    int		refresh_rate_requested;			// Requested refresh rate
    int		clear_request;					// Set to 1 when a clear is requested
    int		page_flipflop;					// 0-1
    int		game_area_x1, game_area_y1;		// Game area frame
    int		game_area_x2, game_area_y2;
	bool	triple_buffering_activated;		// Enabled in configuration and supported by driver (eg: fullscreen)
};

extern t_video	Video;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Video_Init(void);
void	Video_CreateVideoBuffers();

void    Video_Mode_Update_Size(void);
void    Video_Clear(void);
void    Video_Setup_State(void);

void    Refresh_Screen(void);                                    // redraw screen

void	Screenbuffer_AcquireLock(void);
void	Screenbuffer_ReleaseLock(void);
bool	Screenbuffer_IsLocked(void);

extern INLINE void Screen_Save_to_Next_Buffer (void);
extern INLINE void Screen_Restore_from_Next_Buffer (void);

//-----------------------------------------------------------------------------

