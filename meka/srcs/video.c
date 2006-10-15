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
#include "vdp.h"
#include "osd/misc.h"
#include "osd/timer.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

#ifdef DOS
extern int    _wait_for_vsync;
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Video_Init (void)
{
    // Allocate buffers
    screenbuffer_1      = create_bitmap (MAX_RES_X + 32, MAX_RES_Y + 32);
    screenbuffer_2      = create_bitmap (MAX_RES_X + 32, MAX_RES_Y + 32);
    screenbuffer        = screenbuffer_1;
    screenbuffer_next   = screenbuffer_2;
    double_buffer       = create_bitmap ((MAX_RES_X * 2) + 32, (MAX_RES_Y * 2) + 32);

    // Clear variables
    Video.res_x         = 0;
    Video.res_y         = 0;
    Video.page_flipflop = 0;
    Video.clear_need    = NO;
    Video.game_area_x1  = Video.game_area_x2 = Video.game_area_y1 = Video.game_area_y2 = 0;
    Video.driver        = 1;
    Video.refresh_rate_real = Video.refresh_rate_requested = 0;
    fs_page_0 = fs_page_1 = fs_out = NULL;
}

// CHANGE GRAPHIC MODE AND UPDATE NECESSARY VARIABLES -------------------------
static int     Video_Mode_Change (int driver, int w, int h, int v_w, int v_h, int refresh_rate, int fatal)
{
    // Attempt to avoid unnecessary resolution change (on blitter change)
    static struct
    {
        int driver;
        int w, h, v_w, v_h;
        int refresh_rate;
    } previous_mode = { -1, -1, -1, -1, -1, -1 };
    if (driver == previous_mode.driver && w == previous_mode.w && h == previous_mode.h && v_w == previous_mode.v_w && v_h == previous_mode.v_h && refresh_rate == previous_mode.refresh_rate)
    {
        Video_Mode_Update_Size ();
        return (MEKA_ERR_OK);
    }
    previous_mode.driver = driver;
    previous_mode.w = w;
    previous_mode.h = h;
    previous_mode.v_w = v_w;
    previous_mode.v_h = v_h;
    previous_mode.refresh_rate = refresh_rate;

    // Set new mode
    request_refresh_rate (refresh_rate);
    if (set_gfx_mode (driver, w, h, v_w, v_h) != 0)
    {
        if (fatal)
            Quit_Msg (Msg_Get (MSG_Error_Video_Mode), w, h, allegro_error);
        Msg (MSGT_USER, Msg_Get (MSG_Error_Video_Mode), w, h, allegro_error);
        return (MEKA_ERR_FAIL);
    }
    fs_page_0 = NULL;
    fs_page_1 = NULL;

#ifdef DOS
    // Set the Allegro vsync flag to that VGA scroll do not automatically vsync
    _wait_for_vsync = FALSE;
#endif

    Video.res_x = w;
    Video.res_y = h;
    Video.refresh_rate_requested = refresh_rate;
    Video.refresh_rate_real = get_refresh_rate();
    Video_Mode_Update_Size ();

    yield_timeslice();
    rest(100);

    return (MEKA_ERR_OK);
}

void    Video_Mode_Update_Size (void)
{
    int   x_fact, y_fact;

    Blitters_Get_Factors (&x_fact, &y_fact);

    // Compute game area position to be centered on the screen
    Video.game_area_x1 = (Video.res_x - cur_drv->x_res * x_fact) / 2;
    Video.game_area_y1 = (Video.res_y - cur_drv->y_res * y_fact) / 2;
    Video.game_area_x2 = (Video.res_x - Video.game_area_x1);
    Video.game_area_y2 = (Video.res_y - Video.game_area_y1);
}

void    Video_Clear (void)
{
    Video.clear_need = YES;
    // (clearing is done in blit.c)
}

void    Video_GUI_ChangeVideoMode (int res_x, int res_y, int depth)
{
    int i;

    assert(depth == 8); // One day...

    Show_Mouse_In (NULL);
    cfg.GUI_Res_X = res_x;
    cfg.GUI_Res_Y = res_y;
    gui_set_resolution(cfg.GUI_Res_X, cfg.GUI_Res_Y);
    if (Meka_State == MEKA_STATE_GUI)
        Video_Setup_State();
    Regenerate_Background();

    // Fix position
    for (i = 0; i < gui.box_last; i ++)
    {
        t_gui_box *box = gui.box[i];
        gui_box_clip_position(box);
        box->must_redraw = YES;
    }
}

// SWITCH FROM VIDEO MODES ----------------------------------------------------
void    Video_Setup_State (void)
{
    switch (Meka_State)
    {
    case MEKA_STATE_SHUTDOWN:
        {
            // ...
            set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
            break;
        }
    case MEKA_STATE_FULLSCREEN: // FullScreen mode ----------------------------
        {
            int driver;
            //#ifdef WIN32
            //   driver = blitters.current->driver_win;
            //#else
            driver = blitters.current->driver;
            //#endif
            if (blitters.current->flip)
            {
                if (Video_Mode_Change (driver,
                    blitters.current->res_x, blitters.current->res_y,
#ifdef WIN32
                    0, 0,
#else
                    0, blitters.current->res_y * 2,
#endif
                    blitters.current->refresh_rate,
                    NO) != MEKA_ERR_OK)
                {
                    Meka_State = MEKA_STATE_GUI;
                    Video_Setup_State ();
                    Msg (MSGT_USER, Msg_Get (MSG_Error_Video_Mode_Back_To_GUI));
                    return;
                }
                if (fs_page_0)
                    destroy_bitmap (fs_page_0);
                if (fs_page_1)
                    destroy_bitmap (fs_page_1);
                fs_page_0 = create_video_bitmap (Video.res_x, Video.res_y);
                fs_page_1 = create_video_bitmap (Video.res_x, Video.res_y);
                Video.page_flipflop = 0;
                fs_out = fs_page_1;
                clear_to_color (fs_page_0, Border_Color);
                clear_to_color (fs_page_1, Border_Color);
                show_video_bitmap (fs_page_0);
            }
            else
            {
                if (Video_Mode_Change (driver,
                    blitters.current->res_x, blitters.current->res_y,
                    0, 0,
                    blitters.current->refresh_rate,
                    NO) != MEKA_ERR_OK)
                {
                    Meka_State = MEKA_STATE_GUI;
                    Video_Setup_State ();
                    Msg (MSGT_USER, Msg_Get (MSG_Error_Video_Mode_Back_To_GUI));
                    return;
                }
                fs_out = screen;
            }
            Change_Mode_Misc ();
            Palette_Sync_All ();
            // set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
        }
        break;
    case MEKA_STATE_GUI: // Interface Mode ------------------------------------
        {
            switch (cfg.GUI_Access_Mode)
            {
            case GUI_FB_ACCESS_FLIPPED: //--------------------[ Two video pages ]---
                {
                    #ifdef WIN32
                        Video_Mode_Change (cfg.GUI_Driver, cfg.GUI_Res_X, cfg.GUI_Res_X, 0, 0, cfg.GUI_Refresh_Rate, YES);
                    #else
                        Video_Mode_Change (cfg.GUI_Driver, cfg.GUI_Res_X, cfg.GUI_Res_X, 0, cfg.GUI_Res_Y * 2, cfg.GUI_Refresh_Rate, YES);
                    #endif
                    gui_page_0 = create_sub_bitmap (screen, 0, 0,             cfg.GUI_Res_X, cfg.GUI_Res_Y);
                    gui_page_1 = create_sub_bitmap (screen, 0, cfg.GUI_Res_Y, cfg.GUI_Res_X, cfg.GUI_Res_Y);
                    opt.GUI_Current_Page = 1;
                    gui_buffer = gui_page_1;
                    scroll_screen (0, cfg.GUI_Res_Y);
                    break;
                }
            default: //---------------------------------[ One video page ]---
                {
                    Video_Mode_Change (cfg.GUI_Driver, cfg.GUI_Res_X, cfg.GUI_Res_Y, 0, 0, cfg.GUI_Refresh_Rate, YES);
                    if (cfg.GUI_Access_Mode == GUI_FB_ACCESS_DIRECT)
                    { gui_buffer = screen; }
                    break;
                }
            }
            gui_init_again ();
            Change_Mode_Misc ();

            Palette_Sync_All ();

            gui_redraw_everything_now_once ();
            if (cfg.GUI_Access_Mode == GUI_FB_ACCESS_BUFFERED)
            {
                Show_Mouse_In (gui_buffer);
            }
        }
        break;
    }

    #ifndef DOS
        set_display_switch_callback (SWITCH_IN,  Switch_In_Callback);
        set_display_switch_callback (SWITCH_OUT, Switch_Out_Callback);
    #endif

    Clock_Init ();
    Inputs_Init_Mouse (); // why? I forgot
}

void    Screen_Save_to_Next_Buffer (void)
{
    blit (screenbuffer, screenbuffer_next, 0, 0, 0, 0, screenbuffer->w, screenbuffer->h);
}

void    Screen_Restore_from_Next_Buffer (void)
{
    blit (screenbuffer_next, screenbuffer, 0, 0, 0, 0, screenbuffer_next->w, screenbuffer_next->h);
}

// REFRESH THE SCREEN ---------------------------------------------------------
// This is called when line == tsms.VDP_Line_End
void    Refresh_Screen (void)
{
    // acquire_bitmap(screen);

//#ifdef WIN32
//    Msg (MSGT_DEBUG, "%016I64x , %016I64x", OSD_Timer_GetCyclesCurrent(), OSD_Timer_GetCyclesPerSecond());
//#else
//    Msg (MSGT_DEBUG, "%016llx , %016llx", OSD_Timer_GetCyclesCurrent(), OSD_Timer_GetCyclesPerSecond());
//#endif

    if (fskipper.Show_Current_Frame)
    {
        if (Capture.request)
            Capture_Screen ();

        if (Machine_Pause_Need_To)
            Machine_Pause ();

        if (Meka_State == MEKA_STATE_GUI) // GRAPHICAL USER INTERFACE ------------
        {
            if (cfg.GUI_Access_Mode == GUI_FB_ACCESS_FLIPPED)
            {
                opt.GUI_Current_Page ^= 1;
                if (opt.GUI_Current_Page == 0)
                { gui_buffer = gui_page_0; scroll_screen (0, 0); }
                else
                { gui_buffer = gui_page_1; scroll_screen (0, cfg.GUI_Res_Y); }
            }

            Clock_Start (CLOCK_GUI_UPDATE);
            gui_update ();
            Clock_Stop (CLOCK_GUI_UPDATE);

            // Check if we're switching GUI off now
            if (Meka_State != MEKA_STATE_GUI)
            {
                // release_bitmap(screen);
                return;
            }

            // Msg (MSGT_DEBUG, "calling gui_redraw(), screenbuffer=%d", (screenbuffer==screenbuffer_1)?1:2);

            Clock_Start (CLOCK_GUI_REDRAW);
            gui_redraw ();
            Clock_Stop (CLOCK_GUI_REDRAW);

            // Blit GUI screen ------------------------------------------------------
            Blit_GUI ();

            Show_Mouse_In (NULL);
        }

        if (Meka_State == MEKA_STATE_FULLSCREEN) // FULLSCREEN ---------------------
        {
            if (opt.Fullscreen_Cursor)
                Show_Mouse_In (screenbuffer);

            // Show current FPS -----------------------------------------------------
            if (fskipper.FPS_Display)
            {
                int x, y;
                char s [16];
                sprintf (s, "%d FPS", fskipper.FPS);
                if (cur_drv->id == DRV_GG) { x = 48; y = 24; } else { x = 8; y = 6; }
                Font_Print (F_MIDDLE, screenbuffer, s, x, y, GUI_COL_WHITE); // In white
                gui_status.timeleft = 0; // Force disabling the current message
            }

            // Blit emulated screen in fullscreen mode ------------------------------
            Blit_Fullscreen ();

            // Disable LightGun cursor until next screen refresh --------------------
            if (opt.Fullscreen_Cursor)
                Show_Mouse_In (NULL);
        }

        Clock_Draw ();
    } // of: if (fskipper.Show_Current_Frame)

    // Unlock palette colors -----------------------------------------------------
    Palette_Emu_Unlock_All ();

    // Draw next image in other buffer --------------------------------------------
    if (machine & MACHINE_PAUSED)
    {
        Screen_Restore_from_Next_Buffer ();
    }
    else
    {
        // Swap buffers
        void *tmp = screenbuffer;
        screenbuffer = screenbuffer_next;
        screenbuffer_next = tmp;
        // Msg (MSGT_DEBUG, "Swap buffer. screenbuffer=%d", screenbuffer==screenbuffer_1?1:2);

        // In debugging mode, copy previously rendered buffer to new one
        // This is so the user always see the current rendering taking place over the previous one
        #ifdef MEKA_Z80_DEBUGGER
            if (Debugger.Active)
                Screen_Restore_from_Next_Buffer();
        #endif
    }

    // Ask frame-skipper weither next frame should be drawn or not
    Clock_Start (CLOCK_FRAME_SKIPPER);
    fskipper.Show_Current_Frame = Frame_Skipper ();
    //if (fskipper.Show_Current_Frame == NO)
    //   Msg (MSGT_USER, "Skip frame!");
    Clock_Stop (CLOCK_FRAME_SKIPPER);

    // Update console (under WIN32)
    // #ifdef WIN32
    //  ConsoleUpdate();
    // #endif

    // release_bitmap(screen);
}

// UPDATE LINE_START & LINE_END VARIABLES -------------------------------------
// FIXME: move to vdp.c
void    Update_Line_Start_End (void)
{
    if (cur_drv->id == DRV_GG && Wide_Screen_28)
        cur_drv->y_show_start = cur_drv->y_start + 16;
    else
        cur_drv->y_show_start = cur_drv->y_start;
    cur_drv->y_show_end = cur_drv->y_show_start + cur_drv->y_res - 1;
    if (Wide_Screen_28)
        cur_drv->y_int = 224;
    else
        cur_drv->y_int = 192;
}

// SET BORDER COLOR IN VGA MODES ----------------------------------------------
#ifdef DOS
void    Video_VGA_Set_Border_Color (u8 idx)
{
    inp  (0x3DA);
    outp (0x3C0, 0x31);
    outp (0x3C0, idx);
};
#endif

//-----------------------------------------------------------------------------

