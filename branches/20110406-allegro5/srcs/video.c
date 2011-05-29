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
#include "inputs_i.h"
#include "inputs_t.h"
#include "palette.h"
#include "skin_bg.h"
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
    Video.page_flipflop				= 0;
    Video.clear_request				= FALSE;
    Video.game_area_x1				= 0;
	Video.game_area_x2				= 0;
	Video.game_area_y1				= 0;
	Video.game_area_y2				= 0;
    Video.driver					= 1;
	Video.refresh_rate_requested	= 0;
	Video.triple_buffering_activated= FALSE;
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

static int Video_Mode_Change(t_video_driver* driver, int w, int h, int v_w, int v_h, bool fullscreen, int refresh_rate, bool fatal)
{
    // Attempt to avoid unnecessary resolution change (on blitter change)
    static struct
    {
        t_video_driver* driver;
        int w, h, v_w, v_h;
		int fullscreen;
        int refresh_rate;
    } previous_mode = { NULL, -1, -1, -1, -1, -1, -1 };
    if (driver == previous_mode.driver && w == previous_mode.w && h == previous_mode.h && v_w == previous_mode.v_w && v_h == previous_mode.v_h && (int)fullscreen == previous_mode.fullscreen && refresh_rate == previous_mode.refresh_rate)
    {
        Video_GameMode_UpdateBounds();
        return (MEKA_ERR_OK);
    }

    previous_mode.driver = driver;
    previous_mode.w = w;
    previous_mode.h = h;
    previous_mode.v_w = v_w;
    previous_mode.v_h = v_h;
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
	if (g_env.state == MEKA_STATE_GUI)
		GUI_SetupNewVideoMode();

    return (MEKA_ERR_OK);
}

void    Video_GameMode_UpdateBounds(void)
{
    int   x_fact, y_fact;
    Blitters_Get_Factors(&x_fact, &y_fact);

    // Compute game area position to be centered on the screen
    Video.game_area_x1 = (Video.res_x - cur_drv->x_res * x_fact) / 2;
    Video.game_area_y1 = (Video.res_y - cur_drv->y_res * y_fact) / 2;
    Video.game_area_x2 = (Video.res_x - Video.game_area_x1);
    Video.game_area_y2 = (Video.res_y - Video.game_area_y1);
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
    Video.clear_request = TRUE;
}

// SWITCH FROM VIDEO MODES ----------------------------------------------------
void    Video_Setup_State (void)
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
    case MEKA_STATE_FULLSCREEN: // FullScreen mode ----------------------------
        {
			Video.triple_buffering_activated = FALSE;
			if (g_Configuration.video_mode_game_triple_buffering)
            {
				assert(0);	// FIXME-ALLEGRO5: triple buffering
                if (Video_Mode_Change(
						g_Configuration.video_driver,
                        Blitters.current->res_x, Blitters.current->res_y,
                    #ifdef ARCH_WIN32
                        0, 0,
                    #else
                        0, Blitters.current->res_y * 2,
                    #endif
						g_Configuration.video_mode_game_fullscreen,
                        Blitters.current->refresh_rate, FALSE) != MEKA_ERR_OK)
                {
                    g_env.state = MEKA_STATE_GUI;
                    Video_Setup_State();
                    Msg(MSGT_USER, Msg_Get (MSG_Error_Video_Mode_Back_To_GUI));
                    return;
                }
				if (fs_page_0)
				{
					al_destroy_bitmap(fs_page_0);
					fs_page_0 = NULL;
				}
				if (fs_page_1)
				{
					al_destroy_bitmap(fs_page_1);
					fs_page_1 = NULL;
				}
				if (fs_page_2)
				{
					al_destroy_bitmap(fs_page_2);
					fs_page_2 = NULL;
				}

					assert(0);
#if 0 // FIXME-ALLEGRO5: triple buffering
				if (gfx_capabilities & GFX_CAN_TRIPLE_BUFFER)
				{
					// Enable triple buffering
					fs_page_0 = create_video_bitmap (Video.res_x, Video.res_y);
					fs_page_1 = create_video_bitmap (Video.res_x, Video.res_y);
					fs_page_2 = create_video_bitmap (Video.res_x, Video.res_y);
					fs_out = fs_page_1;
					enable_triple_buffer();
					Video.page_flipflop = 0;
					al_set_target_bitmap(fs_page_0);
					al_clear_to_color(BORDER_COLOR);
					al_set_target_bitmap(fs_page_1);
					al_clear_to_color(BORDER_COLOR);
					al_set_target_bitmap(fs_page_2);
					al_clear_to_color(BORDER_COLOR);
					request_video_bitmap(fs_page_0);
					Video.triple_buffering_activated = TRUE;
				}
				else
				{
					// No triple buffering
					// FIXME: We allocated too much VRAM...
					fs_out = al_get_backbuffer(g_display);
					al_set_target_bitmap(fs_out);
					al_clear_to_color(BORDER_COLOR);
				}
#endif
            }
			else if (g_Configuration.video_mode_game_page_flipping)
            {
				assert(0);
#if 0 // FIXME-ALLEGRO5: page flipping
                if (Video_Mode_Change (driver,
                    Blitters.current->res_x, Blitters.current->res_y,
#ifdef ARCH_WIN32
                    0, 0,
#else
                    0, Blitters.current->res_y * 2,
#endif
                    Blitters.current->refresh_rate,
                    FALSE) != MEKA_ERR_OK)
                {
                    g_env.state = MEKA_STATE_GUI;
                    Video_Setup_State ();
                    Msg (MSGT_USER, Msg_Get (MSG_Error_Video_Mode_Back_To_GUI));
                    return;
                }
                if (fs_page_0)
				{
                    al_destroy_bitmap (fs_page_0);
					fs_page_0 = NULL;
				}
                if (fs_page_1)
				{
                    al_destroy_bitmap (fs_page_1);
					fs_page_1 = NULL;
				}
                if (fs_page_2)
                {
                    al_destroy_bitmap (fs_page_1);
                    fs_page_2 = NULL;
                }

                fs_page_0 = create_video_bitmap (Video.res_x, Video.res_y);
                fs_page_1 = create_video_bitmap (Video.res_x, Video.res_y);
                Video.page_flipflop = 0;
                fs_out = fs_page_1;
                clear_to_color (fs_page_0, BORDER_COLOR);
                clear_to_color (fs_page_1, BORDER_COLOR);
                show_video_bitmap (fs_page_0);
#endif // page flipping
            }
            else
            {
                if (Video_Mode_Change (
					g_Configuration.video_driver,
                    Blitters.current->res_x, Blitters.current->res_y,
                    0, 0,
					g_Configuration.video_mode_game_fullscreen,
                    Blitters.current->refresh_rate,
                    FALSE) != MEKA_ERR_OK)
                {
                    g_env.state = MEKA_STATE_GUI;
                    Video_Setup_State ();
                    Msg (MSGT_USER, Msg_Get (MSG_Error_Video_Mode_Back_To_GUI));
                    return;
                }
                fs_out = al_get_backbuffer(g_display);
            }
            Change_Mode_Misc ();
            //Palette_Sync_All ();
            // set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
        }
        break;
    case MEKA_STATE_GUI: // Interface Mode ------------------------------------
        {
			Video_Mode_Change (g_Configuration.video_driver, g_Configuration.video_mode_gui_res_x, g_Configuration.video_mode_gui_res_y, 0, 0, g_Configuration.video_mode_gui_fullscreen, g_Configuration.video_mode_gui_refresh_rate, TRUE);
            Change_Mode_Misc();
            //Palette_Sync_All();
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

//#ifdef ARCH_WIN32
//    Msg (MSGT_DEBUG, "%016I64x , %016I64x", OSD_Timer_GetCyclesCurrent(), OSD_Timer_GetCyclesPerSecond());
//#else
//    Msg (MSGT_DEBUG, "%016llx , %016llx", OSD_Timer_GetCyclesCurrent(), OSD_Timer_GetCyclesPerSecond());
//#endif

    if (fskipper.Show_Current_Frame)
    {
		Capture_Update();

        if (Machine_Pause_Need_To)
            Machine_Pause();

        if (g_env.state == MEKA_STATE_GUI) // GRAPHICAL USER INTERFACE ------------
        {
            gui_update ();

            // Check if we're switching GUI off now
            if (g_env.state != MEKA_STATE_GUI)
            {
                // release_bitmap(screen);
                return;
            }

            // Msg (MSGT_DEBUG, "calling gui_redraw(), screenbuffer=%d", (screenbuffer==screenbuffer_1)?1:2);

            gui_redraw();

            // Blit GUI screen
            Blit_GUI();
        }

        if (g_env.state == MEKA_STATE_FULLSCREEN) // FULLSCREEN ---------------------
        {
            // Show current FPS -----------------------------------------------------
            if (fskipper.FPS_Display)
            {
                int x, y;
                char s [16];
                sprintf (s, "%.1f FPS", fskipper.FPS);
                if (cur_drv->id == DRV_GG) { x = 48; y = 24; } else { x = 8; y = 6; }
                Font_Print (F_MIDDLE, screenbuffer, s, x, y, COLOR_WHITE); // In white
                gui_status.timeleft = 0; // Force disabling the current message
            }

            // Blit emulated screen in fullscreen mode ------------------------------
            Blit_Fullscreen ();
        }

        // Palette update after redraw
        Palette_UpdateAfterRedraw();

        // Clear keypress queue
        Inputs_KeyPressQueue_Clear();

    } // of: if (fskipper.Show_Current_Frame)

    // Draw next image in other buffer --------------------------------------------
    if (machine & MACHINE_PAUSED)
    {
        Screen_Restore_from_Next_Buffer ();
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

    // Ask frame-skipper weither next frame should be drawn or not
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

