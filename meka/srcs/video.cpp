//-----------------------------------------------------------------------------
// MEKA - video.c
// Video / Miscellaneous - Code
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
#include "vmachine.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_video g_video;

t_video_driver  g_video_drivers[] =
{
#ifdef ARCH_WIN32
    { "directx",    "DirectX",      0,                  },  // Allegro for Win32 wants a zero here because it is "default".
#endif
    { "opengl",     "OpenGL",       ALLEGRO_OPENGL,     },
    { "opengl30",   "OpenGL 3.0",   ALLEGRO_OPENGL_3_0, },
    { NULL, }
};

t_video_driver* g_video_driver_default = &g_video_drivers[0];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Video_Init()
{
    //Video_CreateVideoBuffers();
    Video_EnumerateDisplayModes();

    // Clear variables
    g_video.res_x                       = 0;
    g_video.res_y                       = 0;
    g_video.clear_requests              = 0;
    g_video.game_area_x1                = 0;
    g_video.game_area_x2                = 0;
    g_video.game_area_y1                = 0;
    g_video.game_area_y2                = 0;
    g_video.driver                      = 1;
    g_video.refresh_rate_requested      = 0;
    g_video.display_mode_current_index  = 0;
    fs_out                              = NULL;
}

void Video_DestroyVideoBuffers()
{
    if (Screenbuffer_IsLocked())
        Screenbuffer_ReleaseLock();

    if (screenbuffer_1)
        al_destroy_bitmap(screenbuffer_1);
    if (screenbuffer_2)
        al_destroy_bitmap(screenbuffer_2);

    screenbuffer_1 = screenbuffer_2 = NULL;

    GUI_DestroyVideoBuffers();
    Data_DestroyVideoBuffers();
    Blit_DestroyVideoBuffers();
    SkinFx_DestroyVideoBuffers();
}

void Video_CreateVideoBuffers()
{
    Video_DestroyVideoBuffers();

    // Allocate buffers
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_NO_PRESERVE_TEXTURE);
    al_set_new_bitmap_format(g_config.video_game_format_request);
    screenbuffer_1      = al_create_bitmap(MAX_RES_X + 32, MAX_RES_Y + 32);
    screenbuffer_2      = al_create_bitmap(MAX_RES_X + 32, MAX_RES_Y + 32);
    screenbuffer        = screenbuffer_1;
    screenbuffer_next   = screenbuffer_2;

    // Retrieve actual video format. This will be used to compute color values.
    g_screenbuffer_format = al_get_bitmap_format(screenbuffer_1);

    assert(screenbuffer != NULL);
    assert(screenbuffer_next != NULL);

    Screenbuffer_AcquireLock();

    Blit_CreateVideoBuffers();
    Data_CreateVideoBuffers();
    SkinFx_CreateVideoBuffers();
    GUI_SetupNewVideoMode();
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
            gui_relayout_all();
        return (MEKA_ERR_OK);
    }

    previous_mode.driver = driver;
    previous_mode.w = w;
    previous_mode.h = h;
    previous_mode.fullscreen = fullscreen;
    previous_mode.refresh_rate = refresh_rate;

    Video_DestroyVideoBuffers();

    // Set new mode
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
#if ARCH_MACOSX
        display_flags |= ALLEGRO_FULLSCREEN_WINDOW; // FIXME: Expose/use FULLSCREEN_WINDOW everywhere?
#else
        display_flags |= ALLEGRO_FULLSCREEN;
#endif
    else
        display_flags |= ALLEGRO_WINDOWED;
    al_set_new_display_flags(display_flags);
    al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
    al_set_new_display_refresh_rate(g_config.video_mode_gui_refresh_rate);
    g_display = al_create_display(w, h);

    if (!g_display)
    {
        if (fatal)
            Quit_Msg(Msg_Get(MSG_Error_Video_Mode), w, h);
        Msg(MSGT_USER, Msg_Get(MSG_Error_Video_Mode), w, h);
        return (MEKA_ERR_FAIL);
    }

    al_register_event_source(g_display_event_queue, al_get_display_event_source(g_display));

    g_video.res_x = al_get_display_width(g_display);
    g_video.res_y = al_get_display_height(g_display);
    g_video.refresh_rate_requested = refresh_rate;
    Video_GameMode_UpdateBounds();

    // Window title
    al_set_window_title(g_display, Msg_Get(MSG_Window_Title));

    // Recreate all video buffers
    Video_CreateVideoBuffers();

    return (MEKA_ERR_OK);
}

void    Video_GameMode_UpdateBounds()
{
    // Compute game area position to be centered on the screen
    Blit_Fullscreen_UpdateBounds();
}

void    Video_GameMode_ScreenPosToEmulatedPos(int screen_x, int screen_y, int* pemu_x, int* pemu_y, bool clamp)
{
    if (clamp)
    {
        const int rx = LinearRemapClamp(screen_x, g_video.game_area_x1, g_video.game_area_x2, 0, g_driver->x_res);
        const int ry = LinearRemapClamp(screen_y, g_video.game_area_y1, g_video.game_area_y2, 0, g_driver->y_res);
        *pemu_x = rx;
        *pemu_y = ry;
    }
    else
    {
        const int rx = LinearRemap(screen_x, g_video.game_area_x1, g_video.game_area_x2, 0, g_driver->x_res);
        const int ry = LinearRemap(screen_y, g_video.game_area_y1, g_video.game_area_y2, 0, g_driver->y_res);
        *pemu_x = rx;
        *pemu_y = ry;
    }
}

void    Video_GameMode_EmulatedPosToScreenPos(int emu_x, int emu_y, int* pscreen_x, int* pscreen_y, bool clamp)
{
    if (clamp)
    {
        const int rx = LinearRemapClamp(emu_x, 0, g_driver->x_res, g_video.game_area_x1, g_video.game_area_x2);
        const int ry = LinearRemapClamp(emu_y, 0, g_driver->y_res, g_video.game_area_y1, g_video.game_area_y2);
        *pscreen_x = rx;
        *pscreen_y = ry;
    }
    else
    {
        const int rx = LinearRemap(emu_x, 0, g_driver->x_res, g_video.game_area_x1, g_video.game_area_x2);
        const int ry = LinearRemap(emu_y, 0, g_driver->y_res, g_video.game_area_y1, g_video.game_area_y2);
        *pscreen_x = rx;
        *pscreen_y = ry;
    }
}


void    Video_GameMode_GetScreenSize(int* pscreen_x, int* pscreen_y )
{
    *pscreen_x = al_get_display_width(g_display);
    *pscreen_y = al_get_display_height(g_display);
}

void    Video_GameMode_GetScreenBounds(int* pscreen_x1, int* pscreen_y1, int* pscreen_x2, int* pscreen_y2 )
{
    *pscreen_x1 = g_video.game_area_x1;
    *pscreen_y1 = g_video.game_area_y1;
    *pscreen_x2 = g_video.game_area_x2;
    *pscreen_y2 = g_video.game_area_y2;
}

void    Video_GameMode_GetScreenCenterPos(int* pscreen_x, int* pscreen_y)
{
    *pscreen_x = (g_video.game_area_x1 + g_video.game_area_x2) >> 1;
    *pscreen_y = (g_video.game_area_y1 + g_video.game_area_y2) >> 1;
}

void    Video_ClearScreenBackBuffer()
{
    // Note: actual clearing will be done in blit.c
    g_video.clear_requests = 3;
}

void    Video_EnumerateDisplayModes()
{
    std::vector<t_video_mode>& display_modes = g_video.display_modes;
    display_modes.clear();

#ifndef ARCH_ANDROID
    const int modes = al_get_num_display_modes();
    for (int i = 0; i != modes; i++)
    {
        ALLEGRO_DISPLAY_MODE al_display_mode;
        if (al_get_display_mode(i, &al_display_mode))
        {
            //Msg(MSGT_DEBUG, "Display Mode: %dx%d @ %d Hz, format %d", display_mode.width, display_mode.height, display_mode.refresh_rate, display_mode.format);
            display_modes.resize(display_modes.size()+1);

            t_video_mode* video_mode = &display_modes.back();
            video_mode->w = al_display_mode.width;
            video_mode->h = al_display_mode.height;
            //video_mode->color_format = al_display_mode.format;
            video_mode->refresh_rate = al_display_mode.refresh_rate;
        }
    }

    // filter out color_format duplicates because we ignore it for now
    for (size_t i = 0; i < display_modes.size(); i++)
    {
        t_video_mode* video_mode = &display_modes[i];
        for (size_t j = i + 1; j < display_modes.size(); j++)
        {
            t_video_mode* video_mode_2 = &display_modes[j];
            if (video_mode->w == video_mode_2->w && video_mode->h == video_mode_2->h)
            {
                 if (video_mode->refresh_rate == video_mode_2->refresh_rate)
                 {
                     display_modes.erase(display_modes.begin()+j);
                     j--;
                 }
            }
        }
    }

    // find mode closest to current setting
    g_video.display_mode_current_index = 0;
    int closest_index = -1;
    float closest_d2 = FLT_MAX;
    for (size_t i = 0; i < display_modes.size(); i++)
    {
        t_video_mode* video_mode = &display_modes[i];

        int dx = (video_mode->w - g_config.video_mode_gui_res_x);
        int dy = (video_mode->h - g_config.video_mode_gui_res_y);
        float d2 = dx*dx + dy*dy;

        if (closest_d2 > d2)
        {
            if (closest_index != -1)
                if (video_mode->refresh_rate != 0 && g_config.video_mode_gui_refresh_rate != 0 && video_mode->refresh_rate != g_config.video_mode_gui_refresh_rate)
                    continue;
            closest_d2 = d2;
            closest_index = i;
        }
    }
    if (closest_index != -1)
        g_video.display_mode_current_index = closest_index;
#endif
}

void    Video_Setup_State()
{
    switch (g_env.state)
    {
    case MEKA_STATE_INIT:
        break;
    case MEKA_STATE_SHUTDOWN:
        {
            Video_DestroyVideoBuffers();
            if (g_display)
                al_destroy_display(g_display);
            g_display = NULL;
            break;
        }
    case MEKA_STATE_GAME: // FullScreen mode ----------------------------
        {
            const int refresh_rate = g_config.video_mode_gui_refresh_rate;
            const int game_res_x = g_config.video_mode_gui_res_x;
            const int game_res_y = g_config.video_mode_gui_res_y;
            const bool game_fullscreen = g_config.video_fullscreen;

            if (Video_ChangeVideoMode(g_config.video_driver, game_res_x, game_res_y, game_fullscreen, refresh_rate, FALSE) != MEKA_ERR_OK)
            {
                g_env.state = MEKA_STATE_GUI;
                Video_Setup_State();
                Msg(MSGT_USER, "%s", Msg_Get(MSG_Error_Video_Mode_Back_To_GUI));
                return;
            }
            fs_out = al_get_backbuffer(g_display);
            Palette_Emulation_Reload();
            Video_ClearScreenBackBuffer();
        }
        break;
    case MEKA_STATE_GUI:
        {
            const int refresh_rate = g_config.video_mode_gui_refresh_rate;
            const int gui_res_x = g_config.video_mode_gui_res_x;
            const int gui_res_y = g_config.video_mode_gui_res_y;
            Video_ChangeVideoMode(g_config.video_driver, gui_res_x, gui_res_y, g_config.video_fullscreen, refresh_rate, TRUE);
            if (opt.GUI_Inited)
                gui_redraw_everything_now_once();
        }
        break;
    }
    Inputs_Peripheral_Change_Update();
}

void    Screen_Save_to_Next_Buffer()
{
    al_set_target_bitmap(screenbuffer_next);
    al_draw_bitmap(screenbuffer, 0, 0, 0);
}

void    Screen_Restore_from_Next_Buffer()
{
    al_set_target_bitmap(screenbuffer);
    al_draw_bitmap(screenbuffer_next, 0, 0, 0);
}

void    Screenbuffer_AcquireLock()
{
    assert(g_screenbuffer_locked_region == NULL && g_screenbuffer_locked_buffer == NULL);
    g_screenbuffer_locked_region = al_lock_bitmap(screenbuffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    g_screenbuffer_locked_buffer = screenbuffer;

    assert(g_screenbuffer_locked_region != NULL);
    assert(g_screenbuffer_locked_buffer != NULL);
}

void    Screenbuffer_ReleaseLock()
{
    assert(g_screenbuffer_locked_region != NULL);
    assert(g_screenbuffer_locked_buffer != NULL);
    assert(g_screenbuffer_locked_buffer == screenbuffer);
    al_unlock_bitmap(screenbuffer);
    g_screenbuffer_locked_region = NULL;
    g_screenbuffer_locked_buffer = NULL;
}

bool    Screenbuffer_IsLocked()
{
    return g_screenbuffer_locked_region != NULL;
}

void    Video_UpdateEvents()
{
    bool resume = false;
    bool pause = false;
    
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
        
        case ALLEGRO_EVENT_DISPLAY_LOST:
        case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
            pause = true;
            /* fallthrough */
        case ALLEGRO_EVENT_DISPLAY_EXPOSE:
        case ALLEGRO_EVENT_DISPLAY_FOUND:
        case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
            Inputs_Sources_ClearOutOfFocus();
            break;
            
            // Resize
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            al_acknowledge_resize(g_display);
            /* fallthrough */
            
         // Screen rotated
        case ALLEGRO_EVENT_DISPLAY_ORIENTATION:
            g_video.res_x = al_get_display_width(g_display);
            g_video.res_y = al_get_display_height(g_display);
            Video_GameMode_UpdateBounds();
            break;
            
            // Android leave app
        case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
            al_acknowledge_drawing_halt(g_display);
            break;
            
            // Android return to app
        case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
            al_acknowledge_drawing_resume(g_display);
            resume = true;
            break;
        }
    }

    // Android sends resume events multiple times, triggering this multiple times.
    if ( resume ) {

        Video_CreateVideoBuffers();
        Screenbuffer_ReleaseLock();
    }

#ifdef ARCH_ANDROID
    // Under android, easier to sit here and wait for resume events
    while( pause ) {

        al_wait_for_event(g_display_event_queue, &key_event);
        
        switch (key_event.type ) {
            case ALLEGRO_EVENT_DISPLAY_EXPOSE:
            case ALLEGRO_EVENT_DISPLAY_FOUND:
            case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
                pause = false;
                break;
            case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
                al_acknowledge_drawing_halt(g_display);
                break;
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                al_acknowledge_resize(g_display);
                break;
            case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
                al_acknowledge_drawing_resume(g_display);
                break;
        }
    }
#endif
}

// This is called when line == tsms.VDP_Line_End
void    Video_RefreshScreen()
{
    PROFILE_STEP("Video_RefreshScreen()");

    Screenbuffer_ReleaseLock();
    PROFILE_STEP("Screenbuffer_ReleaseLock()");

    Video_UpdateEvents();
    PROFILE_STEP("Video_UpdateEvents()");

    // 3-D glasses emulation cancel out one render out of two
    if (Glasses.Enabled && Glasses_Must_Skip_Frame())
        Screen_Restore_from_Next_Buffer();

    if (fskipper.Show_Current_Frame)
    {
        Capture_Update();

        if (g_machine_pause_requests > 0)
            Machine_Pause();

        gui_update_mouse();
        if (g_env.state == MEKA_STATE_GUI)
        {
            gui_update();
            PROFILE_STEP("gui_update()");

            // Check if we're switching GUI off now
            if (g_env.state != MEKA_STATE_GUI)
            {
                Screenbuffer_AcquireLock();
                return;
            }

            gui_draw();
            PROFILE_STEP("gui_draw()");

            Blit_GUI();
            PROFILE_STEP("Blit_GUI()");
        }

        if (g_env.state == MEKA_STATE_GAME)
        {
            // Show current FPS
            if (fskipper.FPS_Display)
            {
                char buf[16];
                sprintf(buf, "%.1f FPS", fskipper.FPS);
                int x, y;
                if (g_driver->id == DRV_GG) { x = 48; y = 24; } else { x = 8; y = 6; }
                al_set_target_bitmap(screenbuffer);
                Font_Print(FONTID_MEDIUM, buf, x, y, COLOR_WHITE); // In white
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

    // Draw next image in other buffer
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
        // Msg(MSGT_DEBUG, "Swap buffer. screenbuffer=%d", screenbuffer==screenbuffer_1?1:2);

        // In debugging mode, copy previously rendered buffer to new one
        // This is so the user always see the current rendering taking place over the previous one
        #ifdef MEKA_Z80_DEBUGGER
            if (Debugger.active)
                Screen_Restore_from_Next_Buffer();
        #endif
    }

    // Ask frame-skipper whether next frame should be drawn or not
    fskipper.Show_Current_Frame = Frame_Skipper();
    PROFILE_STEP("Frame_Skipper()");

    Screenbuffer_AcquireLock();
    PROFILE_STEP("Screenbuffer_AcquireLock()");
}

t_video_driver* VideoDriver_FindByName(const char* name)
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

