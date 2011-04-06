//-----------------------------------------------------------------------------
// MEKA - capture.c
// Screen Capture - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "capture.h"
#include "vdp.h"

// Keep request enabled if we're in 'all frames' capture mode
//if (!g_Configuration.capture_all_frames)
//Capture.request = FALSE;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Capture_Init()
// Initialize screen capture system
// Needs to be called before loading the configuration file
//-----------------------------------------------------------------------------
void        Capture_Init(void)
{
	Capture.request = FALSE;
	Capture.request_all_frames = FALSE;
	Capture_Init_Game();
}

//-----------------------------------------------------------------------------
// Capture_Init_Game()
// Screen capture per-game initialization
// Called each time a new Game/ROM is loaded
//-----------------------------------------------------------------------------
void        Capture_Init_Game(void)
{
    Capture.id_number = 1;
}

//-----------------------------------------------------------------------------
// Capture_Request()
// Request a screen capture (will be actually done on next rendering)
//-----------------------------------------------------------------------------
void        Capture_Request(void)
{
    Capture.request = TRUE;
}

//-----------------------------------------------------------------------------
// Capture_FileName_Get(char *dst)
// Compute filename for next screen capture
//-----------------------------------------------------------------------------
static void	Capture_FileName_Get(char *dst)
{
    char *  game_name;
    char    s1 [FILENAME_LEN];
    char    s2 [FILENAME_LEN];

    // Create directory if necessary
    if (!file_exists(g_Env.Paths.ScreenshotDirectory, 0xFF, NULL))
        al_make_directory(g_Env.Paths.ScreenshotDirectory);

    // Figure out a base filename
    if ((machine & MACHINE_RUN) == MACHINE_RUN) // If a game is loaded & running
    {
        strcpy(s1, g_Env.Paths.MediaImageFile);
        killpath(s1);
        killext(s1);
        game_name = s1;
    }
    else
    {
        game_name = CAPTURE_DEFAULT_PREFIX;
    }
    sprintf(s2, "%%s/%s", g_Configuration.capture_filename_template);

    // Create a full filename and check if the file already exists. Loop if it is the case.
	// Note: CAPTURE_ID_MAX is 9999, for all capturing this gives us 2mn46s worth of frames at 60 FPS.
    do
    {
        sprintf(dst, s2, g_Env.Paths.ScreenshotDirectory, game_name, Capture.id_number);
        Capture.id_number++;
    }
    while (file_exists(dst, 0xFF, NULL) != 0 && Capture.id_number < CAPTURE_ID_MAX);
}

//-----------------------------------------------------------------------------
// Capture_Screen(void)
// Capture current screen to a file
//-----------------------------------------------------------------------------
static void		Capture_Screen(void)
{
    //PALETTE     pal;
    ALLEGRO_BITMAP *    bmp;
    ALLEGRO_BITMAP *    source;
    char        s1[FILENAME_LEN];
    int         x_start, x_len;
    int         y_start, y_len;

    // Get a filename
    Capture_FileName_Get(s1);
    if (Capture.id_number >= CAPTURE_ID_MAX)
    {
        Msg(MSGT_USER, Msg_Get(MSG_Capture_Error_File));
        return;
    }

	if ((Meka_State == MEKA_STATE_FULLSCREEN) || (!g_Configuration.capture_include_gui)) 
	{
		// Fullscreen
		source = screenbuffer;
		x_start = cur_drv->x_start;
		y_start = cur_drv->y_show_start;
		x_len = cur_drv->x_res;
		y_len = cur_drv->y_res;

		// Crop left column
		if (g_Configuration.capture_crop_scrolling_column)
		{
			if ((cur_drv->id == DRV_SMS) && (tsms.VDP_VideoMode > 4) && (Mask_Left_8))
			{
				x_start += 8;
				x_len -= 8;
			}
		}

		// Automatic crop on tile boundaries (for map making)
		// In total, remove 8 pixels from each axis
		if (g_Configuration.capture_crop_align_8x8)
		{
			const int scroll_x = cur_machine.VDP.scroll_x_latched;
			const int scroll_y = cur_machine.VDP.scroll_y_latched;
			x_start += scroll_x & 7;
			y_start += 8 - (scroll_y & 7);
			x_len -= 8;
			y_len -= 8;
		}
	}
	else if (Meka_State == MEKA_STATE_GUI)
	{
		// GUI mode
		x_start = 0;
		y_start = 0;
		x_len = g_Configuration.video_mode_gui_res_x;
		y_len = g_Configuration.video_mode_gui_res_y;
		source = gui_buffer;
	}
	else
	{
		// Unknown Mode
		assert(0);
		return;
	}

    acquire_bitmap(source);
    bmp = create_sub_bitmap(source, x_start, y_start, x_len, y_len);
    if (bmp == NULL)
    {
        Msg(MSGT_USER, Msg_Get(MSG_Capture_Error));
        return;
    }
    release_bitmap(source);

    //get_palette(pal);
    if (save_bitmap(s1, bmp, NULL) != 0)
    {
        Msg(MSGT_USER, Msg_Get(MSG_Capture_Error));
        al_destroy_bitmap(bmp);
        return;
    }

    al_destroy_bitmap(bmp);

    // Verbose
    killpath(s1);
    Msg(MSGT_USER, Msg_Get(MSG_Capture_Done), s1);
}

void	Capture_Update(void)
{
	if (Capture.request)
	{
		Capture_Screen();
		Capture.request = FALSE;
	}
	if (Capture.request_all_frames && !(machine & MACHINE_PAUSED))
	{
		Capture_Screen();
	}
}

//-----------------------------------------------------------------------------

void	Capture_MenuHandler_Capture(void)
{
	Capture_Request();
}

void	Capture_MenuHandler_AllFrames(void)
{
	Capture.request_all_frames = !Capture.request_all_frames;
	gui_menu_inverse_check(menus_ID.screenshots, 1);
	if (Capture.request_all_frames)
		;// FIXME-CAPTURE
}

void	Capture_MenuHandler_IncludeGui(void)
{
	g_Configuration.capture_include_gui = !g_Configuration.capture_include_gui;
	gui_menu_inverse_check(menus_ID.screenshots, 2);
}

//-----------------------------------------------------------------------------
