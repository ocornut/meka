//-----------------------------------------------------------------------------
// MEKA - video.c
// Video / Miscellaenous - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "capture.h"
#include "debugger.h"
#include "fskipper.h"
#include "glasses.h"
#include "inputs_i.h"
#include "inputs_t.h"
#include "palette.h"
#include "skin_bg.h"
#include "skin_fx.h"
#include "vdp.h"
#include "video.h"
#include "osd/misc.h"
#include "osd/timer.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_video	Video;

t_video_driver	g_video_drivers[] =
{
#ifdef ARCH_WIN32
	{ "directx",	0,					},	// Allegro for Win32 wants a zero here because it is "default".
#endif
	{ "opengl",		ALLEGRO_OPENGL,		},
	{ "opengl30",	ALLEGRO_OPENGL_3_0, },
	{ NULL, }
};

t_video_driver*	g_video_driver_default = &g_video_drivers[0];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Video_Init (void)
{
	Video_CreateVideoBuffers();

    // Clear variables
    Video.res_x						= 0;
    Video.res_y						= 0;
    Video.clear_requests			= 0;
	Video.game_area_x1				= 0;
	Video.game_area_x2				= 0;
	Video.game_area_y1				= 0;
	Video.game_area_y2				= 0;
    Video.driver					= 1;
	Video.refresh_rate_requested	= 0;
	fs_page_0 = fs_page_1 = fs_out	= NULL;
}

void Video_CreateVideoBuffers()
{
	if (Screenbuffer_IsLocked())
		Screenbuffer_ReleaseLock();

	if (screenbuffer_1)
		al_destroy_bitmap(screenbuffer_1);
	if (screenbuffer_2)
		al_destroy_bitmap(screenbuffer_2);

	// Allocate buffers
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	al_set_new_bitmap_format(g_Configuration.video_game_format_request);
    screenbuffer_1      = al_create_bitmap(MAX_RES_X + 32, MAX_RES_Y + 32);
    screenbuffer_2      = al_create_bitmap(MAX_RES_X + 32, MAX_RES_Y + 32);
    screenbuffer        = screenbuffer_1;
    screenbuffer_next   = screenbuffer_2;

	// Retrieve actual video format. This will be used to compute color values.
	g_screenbuffer_format = al_get_bitmap_format(screenbuffer_1);

	Screenbuffer_AcquireLock();
}

static int Video_ChangeVideoMode(t_video_driver* driver, int w, int h, bool fullscreen, int refresh_rate, bool fatal)
{
    // Attempt to avoid unnecessary resolution change (on blitter change)
    static struct
    {
        t_video_driver* driver;
        int w, h;
		int fullscreen;
        int refresh_rate;
    } previous_mode = { NULL, -1, -1, -1, -1 };
    if (driver == previous_mode.driver && w == previous_mode.w && h == previous_mode.h && (int)fullscreen == previous_mode.fullscreen && refresh_rate == previous_mode.refresh_rate)
    {
        Video_GameMode_UpdateBounds();
		if (g_env.state == MEKA_STATE_GUI)
			GUI_RelayoutAll();
        return (MEKA_ERR_OK);
    }

    previous_mode.driver = driver;
    previous_mode.w = w;
    previous_mode.h = h;
	previous_mode.fullscreen = fullscreen;
    previous_mode.refresh_rate = refresh_rate;

    // Set new mode
	// FIXME-ALLEGRO5: Use al_set_new_display_refresh_rate()
    //request_refresh_rate (refresh_rate);

	if (g_display != NULL)
	{
#ifdef ARCH_WIN32
		// Allegro is missing keyboard events when there's no display, so as a workaround we clear the key states.
		Inputs_KeyClearAllState();
#endif
		al_unregister_event_source(g_display_event_queue, al_get_display_event_source(g_display));
		al_destroy_display(g_display);
		g_display = NULL;
	}

	// Create new display
	int display_flags = driver->flags;
	if (fullscreen)
		display_flags |= ALLEGRO_FULLSCREEN;
	else
		display_flags |= ALLEGRO_WINDOWED;
	al_set_new_display_flags(display_flags);
	al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
	g_display = al_create_display(w, h);

	if (!g_display)
    {
        if (fatal)
            Quit_Msg(Msg_Get(MSG_Error_Video_Mode), w, h);
        Msg(MSGT_USER, Msg_Get(MSG_Error_Video_Mode), w, h);
        return (MEKA_ERR_FAIL);
    }

	/*{
		const int modes = al_get_num_display_modes();
		for (int i = 0; i != modes; i++)
		{
			ALLEGRO_DISPLAY_MODE display_mode;
			if (al_get_display_mode(i, &display_mode))
			{
				Msg(MSGT_DEBUG, "Display Mode: %dx%d @ %d Hz, format %d", display_mode.width, display_mode.height, display_mode.refresh_rate, display_mode.format);
			}
		}
	}*/

	al_register_event_source(g_display_event_queue, al_get_display_event_source(g_display));
	
	fs_page_0 = NULL;
    fs_page_1 = NULL;
    fs_page_2 = NULL;

    Video.res_x = w;
    Video.res_y = h;
    Video.refresh_rate_requested = refresh_rate;
	Video_GameMode_UpdateBounds();

	// Window title
    al_set_window_title(g_display, Msg_Get(MSG_Window_Title));

	// Recreate all video buffers
	Blit_CreateVideoBuffers();
	Video_CreateVideoBuffers();
	Data_CreateVideoBuffers();
	SkinFx_CreateVideoBuffers();
	if (g_env.state == MEKA_STATE_GUI)
		GUI_SetupNewVideoMode();

    return (MEKA_ERR_OK);
}

void    Video_GameMode_UpdateBounds(void)
{
    // Compute game area position to be centered on the screen
	Blit_Fullscreen_UpdateBounds();
}

void	Video_GameMode_ScreenPosToEmulatedPos(int screen_x, int screen_y, int* pemu_x, int* pemu_y, bool clamp)
{
	if (clamp)
	{
		const int rx = LinearRemapClamp(screen_x, Video.game_area_x1, Video.game_area_x2, 0, cur_drv->x_res);
		const int ry = LinearRemapClamp(screen_y, Video.game_area_y1, Video.game_area_y2, 0, cur_drv->y_res);
		*pemu_x = rx;
		*pemu_y = ry;
	}
	else
	{
		const int rx = LinearRemap(screen_x, Video.game_area_x1, Video.game_area_x2, 0, cur_drv->x_res);
		const int ry = LinearRemap(screen_y, Video.game_area_y1, Video.game_area_y2, 0, cur_drv->y_res);
		*pemu_x = rx;
		*pemu_y = ry;
	}
}

void	Video_GameMode_EmulatedPosToScreenPos(int emu_x, int emu_y, int* pscreen_x, int* pscreen_y, bool clamp)
{
	if (clamp)
	{
		const int rx = LinearRemapClamp(emu_x, 0, cur_drv->x_res, Video.game_area_x1, Video.game_area_x2);
		const int ry = LinearRemapClamp(emu_y, 0, cur_drv->y_res, Video.game_area_y1, Video.game_area_y2);
		*pscreen_x = rx;
		*pscreen_y = ry;
	}
	else
	{
		const int rx = LinearRemap(emu_x, 0, cur_drv->x_res, Video.game_area_x1, Video.game_area_x2);
		const int ry = LinearRemap(emu_y, 0, cur_drv->y_res, Video.game_area_y1, Video.game_area_y2);
		*pscreen_x = rx;
		*pscreen_y = ry;
	}
}

void    Video_Clear(void)
{
    // Note: actual clearing will be done in blit.c
    Video.clear_requests = 3;
}

void    Video_Setup_State(void)
{
    switch (g_env.state)
    {
    case MEKA_STATE_SHUTDOWN:
        {
			al_destroy_display(g_display);
			g_display = NULL;
            //set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
            break;
        }
    case MEKA_STATE_GAME: // FullScreen mode ----------------------------
        {
			//const int game_res_x = Blitters.current->res_x;
			//const int game_res_y = Blitters.current->res_y;
			const int game_res_x = g_Configuration.video_mode_gui_res_x;
			const int game_res_y = g_Configuration.video_mode_gui_res_y;
			const bool game_fullscreen = g_Configuration.video_fullscreen;

            if (Video_ChangeVideoMode(g_Configuration.video_driver, game_res_x, game_res_y, game_fullscreen, Blitters.current->refresh_rate, FALSE) != MEKA_ERR_OK)
            {
                g_env.state = MEKA_STATE_GUI;
                Video_Setup_State();
                Msg (MSGT_USER, Msg_Get (MSG_Error_Video_Mode_Back_To_GUI));
                return;
            }
            fs_out = al_get_backbuffer(g_display);
            Change_Mode_Misc();
            // set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        }
        break;
    case MEKA_STATE_GUI: // Interface Mode ------------------------------------
        {
			const int gui_res_x = g_Configuration.video_mode_gui_res_x;
			const int gui_res_y = g_Configuration.video_mode_gui_res_y;
			Video_ChangeVideoMode(g_Configuration.video_driver, gui_res_x, gui_res_y, g_Configuration.video_fullscreen, g_Configuration.video_mode_gui_refresh_rate, TRUE);
            Change_Mode_Misc();
            gui_redraw_everything_now_once();
        }
        break;
    }
}

void    Screen_Save_to_Next_Buffer(void)
{
	al_set_target_bitmap(screenbuffer_next);
	al_draw_bitmap(screenbuffer, 0, 0, 0);
}

void    Screen_Restore_from_Next_Buffer(void)
{
	al_set_target_bitmap(screenbuffer);
	al_draw_bitmap(screenbuffer_next, 0, 0, 0);
}

void	Screenbuffer_AcquireLock(void)
{
	assert(g_screenbuffer_locked_region == NULL);
	g_screenbuffer_locked_region = al_lock_bitmap(screenbuffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
}

void	Screenbuffer_ReleaseLock(void)
{
	assert(g_screenbuffer_locked_region != NULL);
	al_unlock_bitmap(screenbuffer);
	g_screenbuffer_locked_region = NULL;
}

bool	Screenbuffer_IsLocked(void)
{
	return g_screenbuffer_locked_region != NULL;
}

void	Video_UpdateEvents()
{
	ALLEGRO_EVENT key_event;
	while (al_get_next_event(g_display_event_queue, &key_event))
	{
		switch (key_event.type)
		{
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			if (g_env.state == MEKA_STATE_INIT || g_env.state == MEKA_STATE_SHUTDOWN)
				break;
			opt.Force_Quit = TRUE;
			break;
		case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
			//if (g_env.state == MEKA_STATE_INIT || g_env.state == MEKA_STATE_SHUTDOWN)
			//	return;
			//// Msg (MSGT_USER, "Switch_In_Callback()");
			//// clear_to_color (screen, BORDER_COLOR);
			//Video_Clear ();
			//Sound_Playback_Resume ();
			break;
		case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
			//if (g_env.state == MEKA_STATE_INIT || g_env.state == MEKA_STATE_SHUTDOWN)
			//	break;
			//// Msg (MSGT_USER, "Switch_Out_Callback()");
			//Sound_Playback_Mute ();
			break;
		}
	}
}

// This is called when line == tsms.VDP_Line_End
void    Video_RefreshScreen(void)
{
	Screenbuffer_ReleaseLock();

	Video_UpdateEvents();

	// 3-D glasses emulation cancel out one render out of two
	if (Glasses.Enabled && Glasses_Must_Skip_Frame())
		Screen_Restore_from_Next_Buffer();

    if (fskipper.Show_Current_Frame)
    {
		Capture_Update();

        if (Machine_Pause_Need_To)
            Machine_Pause();

        if (g_env.state == MEKA_STATE_GUI)
        {
            gui_update ();

            // Check if we're switching GUI off now
            if (g_env.state != MEKA_STATE_GUI)
            {
				Screenbuffer_AcquireLock();
                return;
            }

            gui_redraw();
            Blit_GUI();
        }

        if (g_env.state == MEKA_STATE_GAME)
        {
            // Show current FPS
            if (fskipper.FPS_Display)
            {
                char buf[16];
                sprintf(buf, "%.1f FPS", fskipper.FPS);
				int x, y;
                if (cur_drv->id == DRV_GG) { x = 48; y = 24; } else { x = 8; y = 6; }
				al_set_target_bitmap(screenbuffer);
                Font_Print(F_MIDDLE, buf, x, y, COLOR_WHITE); // In white
                //g_gui_status.timeleft = 0; // Force disabling the current message because it is slow to display
            }

            // Blit emulated screen in fullscreen mode
            Blit_Fullscreen();
        }

        // Palette update after redraw
        Palette_UpdateAfterRedraw();

        // Clear keypress queue
        Inputs_KeyPressQueue_Clear();

    } // of: if (fskipper.Show_Current_Frame)

    // Draw next image in other buffer --------------------------------------------
    if (g_machine_flags & MACHINE_PAUSED)
    {
        Screen_Restore_from_Next_Buffer();
    }
    else
    {
        // Swap buffers
        ALLEGRO_BITMAP *tmp = screenbuffer;
        screenbuffer = screenbuffer_next;
        screenbuffer_next = tmp;
        // Msg (MSGT_DEBUG, "Swap buffer. screenbuffer=%d", screenbuffer==screenbuffer_1?1:2);

        // In debugging mode, copy previously rendered buffer to new one
        // This is so the user always see the current rendering taking place over the previous one
        #ifdef MEKA_Z80_DEBUGGER
            if (Debugger.active)
                Screen_Restore_from_Next_Buffer();
        #endif
    }

    // Ask frame-skipper whether next frame should be drawn or not
    fskipper.Show_Current_Frame = Frame_Skipper();

	Screenbuffer_AcquireLock();
}

t_video_driver*	VideoDriver_FindByName(const char* name)
{
	t_video_driver* driver = &g_video_drivers[0];
	while (driver->name)
	{
		if (stricmp(name, driver->name) == 0)
			return driver;
		driver++;
	}

	// Silently return default
	return g_video_driver_default;
}


//-----------------------------------------------------------------------------

