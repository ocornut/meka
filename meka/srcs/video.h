//-----------------------------------------------------------------------------
// MEKA - video.h
// Video / Miscellaneous - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_video_mode
{
	int			w;
	int			h;
	//int		color_format;
	int			refresh_rate;
};

struct t_video
{
    int		driver;							// Current driver
    int		res_x, res_y;					// Current resolution
    int		refresh_rate_requested;			// Requested refresh rate
    int		clear_requests;					// Set to a value to request successive clears
    int		game_area_x1, game_area_y1;		// Game area frame
    int		game_area_x2, game_area_y2;
	std::vector<t_video_mode>	display_modes;
	int							display_mode_current_index;
};

struct t_video_driver
{
	const char* name;
	const char* desc;
	int			flags;
};
extern t_video			g_video;
extern t_video_driver	g_video_drivers[];
extern t_video_driver*	g_video_driver_default;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Video_Init();
void	Video_CreateVideoBuffers();

void    Video_ClearScreenBackBuffer();
void    Video_Setup_State();
void	Video_EnumerateDisplayModes();
int		Video_FindClosestDisplayMode(int w, int h, int refresh_rate);

void    Video_GameMode_UpdateBounds(void);
void	Video_GameMode_ScreenPosToEmulatedPos(int screen_x, int screen_y, int* pemu_x, int* pemu_y, bool clamp);
void	Video_GameMode_EmulatedPosToScreenPos(int emu_x, int emu_y, int* pscreen_x, int* pscreen_y, bool clamp);
void	Video_GameMode_GetScreenCenterPos(int* pscreen_x, int* pscreen_y);

void    Video_RefreshScreen(void);
void	Video_UpdateEvents();

void	Screenbuffer_AcquireLock(void);
void	Screenbuffer_ReleaseLock(void);
bool	Screenbuffer_IsLocked(void);

void	Screen_Save_to_Next_Buffer (void);
void	Screen_Restore_from_Next_Buffer (void);

t_video_driver*	VideoDriver_FindByName(const char* name);

//-----------------------------------------------------------------------------

