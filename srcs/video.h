//-----------------------------------------------------------------------------
// MEKA - video.h
// Video / Miscellaenous - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
    int		driver;							// Current driver
    int		res_x, res_y;					// Current resolution
    int		refresh_rate_requested;			// Requested refresh rate
	int		refresh_rate_real;				// Real refresh rate
    int		clear_request;					// Set to 1 when a clear is requested
    int		page_flipflop;					// 0-1
    int		game_area_x1, game_area_y1;		// Game area frame
    int		game_area_x2, game_area_y2;
	bool	triple_buffering_activated;		// Enabled in configuration and supported by driver (eg: fullscreen)
} t_video;

t_video	Video;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Video_Init (void);
void    Video_Mode_Update_Size (void);
void    Video_Clear (void);
void    Video_Setup_State (void);

void    Video_GUI_ChangeVideoMode (int res_x, int res_y, int depth);

void    Refresh_Screen (void);                                    // redraw screen

extern INLINE void Screen_Save_to_Next_Buffer (void);
extern INLINE void Screen_Restore_from_Next_Buffer (void);

#ifdef DOS
void    Video_VGA_Set_Border_Color (byte idx);
#endif

//-----------------------------------------------------------------------------

