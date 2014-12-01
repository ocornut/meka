//-----------------------------------------------------------------------------
// MEKA - g_action.c
// Miscellaneous GUI action handlers - Code
//-----------------------------------------------------------------------------
// FIXME: Make this code/file obsolete eventually.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "db.h"
#include "video.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// ACTION: QUITTING EMULATOR --------------------------------------------------
// FIXME-DEPTH: Ressources (machines, icons) not faded out
void    Action_Quit()
{
    Msg(MSGT_STATUS_BAR, "%s", Msg_Get(MSG_Quit));

    // Shut up sound while fading
    Sound_Playback_Stop();

    // Redraw last time, so message appears on screen
    gui_redraw_everything_now_once();

    /*
    // Software, naive, slow fade
    // Only 32-bits supported
    int depth = bitmap_color_depth(gui_buffer);
    switch (depth)
    {
    case 32:
        {
            while (TRUE)
            {
                bool more = FALSE;
                int  y;
                u32 **ppixels = (u32 **)&gui_buffer->line;
                for (y = gui_buffer->h; y != 0; y--)
                {
                    u32 *pixels = *ppixels++;
                    int  x;
                    for (x = gui_buffer->w; x != 0; x--)
                    {
                        u32 c = *pixels;
                        int r = c & 0x000000FF;
                        int g = c & 0x0000FF00;
                        int b = c & 0x00FF0000;
                        int loop;
                        for (loop = 3; loop != 0; loop--)
                        {
                            if (r != 0) r -= 0x000001;
                            if (g != 0) g -= 0x000100;
                            if (b != 0) b -= 0x010000;
                        }
                        c = r | g | b;
                        if (c != 0)
                            more = TRUE;
                        *pixels++ = c;
                    }
                }
                Blit_GUI();
                if (!more)
                    break;
            }
        } // 32
    }
    */

    // Switch to full black skin
    //Skins_Select(Skins_GetSystemSkinBlack(), TRUE);
    //Skins_QuitAfterFade();
    opt.Force_Quit = TRUE;
}

// ACTION: SHOW OR HIDE SPRITES LAYER -----------------------------------------------
void    Action_Switch_Layer_Sprites (void)
{
    opt.Layer_Mask ^= LAYER_SPRITES;
    gui_menu_toggle_check (menus_ID.layers, 0);
    if (opt.Layer_Mask & LAYER_SPRITES)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Layer_Spr_Enabled));
    }
    else
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Layer_Spr_Disabled));
    }
}

// ACTION: SHOW OR HIDE BACKGROUND LAYER --------------------------------------
void    Action_Switch_Layer_Background (void)
{
    opt.Layer_Mask ^= LAYER_BACKGROUND;
    gui_menu_toggle_check (menus_ID.layers, 1);
    if (opt.Layer_Mask & LAYER_BACKGROUND)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Layer_BG_Enabled));
    }
    else
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Layer_BG_Disabled));
    }
}

// ACTION: SWITCH SPRITE FLICKERING TO 'AUTOMATIC' ----------------------------
void    Action_Switch_Flickering_Auto (void)
{
    g_configuration.sprite_flickering = SPRITE_FLICKERING_AUTO;
    if (DB.current_entry && (DB.current_entry->flags & DB_FLAG_EMU_SPRITE_FLICKER))
        g_configuration.sprite_flickering |= SPRITE_FLICKERING_ENABLED;
    gui_menu_uncheck_all (menus_ID.flickering);
    gui_menu_check (menus_ID.flickering, 0);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Flickering_Auto));
}

// ACTION: SWITCH SPRITE FLICKERING TO 'TRUE' ----------------------------------
void    Action_Switch_Flickering_Yes (void)
{
    g_configuration.sprite_flickering = SPRITE_FLICKERING_ENABLED;
    gui_menu_uncheck_all (menus_ID.flickering);
    gui_menu_check (menus_ID.flickering, 1);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Flickering_Yes));
}

// ACTION: SWITCH SPRITE FLICKERING TO 'FALSE' -----------------------------------
void    Action_Switch_Flickering_No (void)
{
    g_configuration.sprite_flickering = SPRITE_FLICKERING_NO;
    gui_menu_uncheck_all (menus_ID.flickering);
    gui_menu_check (menus_ID.flickering, 2);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Flickering_No));
}

// ACTION: SWITCH BETWEEN FULLSCREEN AND INTERFACE MODES ----------------------
void    Action_Switch_Mode(void)
{
    switch (g_env.state)
    {
    case MEKA_STATE_GAME: g_env.state = MEKA_STATE_GUI;  break;
    case MEKA_STATE_GUI:  g_env.state = MEKA_STATE_GAME; break;
    default:
        // FIXME: Should not happen
        break;
    }

    Sound_Playback_Mute();
    Video_Setup_State();
    Sound_Playback_Resume();
}

//-----------------------------------------------------------------------------

