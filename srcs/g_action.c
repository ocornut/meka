//-----------------------------------------------------------------------------
// MEKA - g_action.c
// Miscellaenous GUI action handlers - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "db.h"

//-----------------------------------------------------------------------------

// ACTION: QUITTING EMULATOR --------------------------------------------------
void    Action_Quit (void)
{
    Msg (MSGT_USER_INFOLINE, Msg_Get(MSG_Quit));

    // Refresh the screen now so we can see the message
    Refresh_Screen ();
    Show_Mouse_In (gui_buffer);
    // Shut up sound while fading
    Sound_Playback_Stop ();
    // Fade
    gui_fade_to_black ();
    opt.Force_Quit = YES;
}

// ACTION: SHOW OR HIDE SPRITES LAYER -----------------------------------------------
void    Action_Switch_Layer_Sprites (void)
{
    opt.Layer_Mask ^= LAYER_SPRITES;
    gui_menu_inverse_check (menus_ID.layers, 0);
    if (opt.Layer_Mask & LAYER_SPRITES)
    {
        Msg (MSGT_USER, Msg_Get (MSG_Layer_Spr_Enabled));
    }
    else
    {
        Msg (MSGT_USER, Msg_Get (MSG_Layer_Spr_Disabled));
    }
}

// ACTION: SHOW OR HIDE BACKGROUND LAYER --------------------------------------
void    Action_Switch_Layer_Background (void)
{
    opt.Layer_Mask ^= LAYER_BACKGROUND;
    gui_menu_inverse_check (menus_ID.layers, 1);
    if (opt.Layer_Mask & LAYER_BACKGROUND)
    {
        Msg (MSGT_USER, Msg_Get (MSG_Layer_BG_Enabled));
    }
    else
    {
        Msg (MSGT_USER, Msg_Get (MSG_Layer_BG_Disabled));
    }
}

// ACTION: SWITCH SPRITE FLICKERING TO 'AUTOMATIC' ----------------------------
void    Action_Switch_Flickering_Auto (void)
{
    Configuration.sprite_flickering = SPRITE_FLICKERING_AUTO;
    if (DB_CurrentEntry && (DB_CurrentEntry->flags & DB_FLAG_EMU_SPRITE_FLICKER))
        Configuration.sprite_flickering |= SPRITE_FLICKERING_ENABLED;
    gui_menu_un_check (menus_ID.flickering);
    gui_menu_check (menus_ID.flickering, 0);
    Msg (MSGT_USER, Msg_Get (MSG_Flickering_Auto));
}

// ACTION: SWITCH SPRITE FLICKERING TO 'YES' ----------------------------------
void    Action_Switch_Flickering_Yes (void)
{
    Configuration.sprite_flickering = SPRITE_FLICKERING_ENABLED;
    gui_menu_un_check (menus_ID.flickering);
    gui_menu_check (menus_ID.flickering, 1);
    Msg (MSGT_USER, Msg_Get (MSG_Flickering_Yes));
}

// ACTION: SWITCH SPRITE FLICKERING TO 'NO' -----------------------------------
void    Action_Switch_Flickering_No (void)
{
    Configuration.sprite_flickering = SPRITE_FLICKERING_NO;
    gui_menu_un_check (menus_ID.flickering);
    gui_menu_check (menus_ID.flickering, 2);
    Msg (MSGT_USER, Msg_Get (MSG_Flickering_No));
}

// ACTION: SWITCH BETWEEN FULLSCREEN AND INTERFACE MODES ----------------------
void    Action_Switch_Mode (void)
{
    switch (Meka_State)
    {
    case MEKA_STATE_FULLSCREEN: Meka_State = MEKA_STATE_GUI;        break;
    case MEKA_STATE_GUI:        Meka_State = MEKA_STATE_FULLSCREEN; break;
    default:
        // FIXME: Should not happen
        break;
    }

    Sound_Playback_Mute ();
    Video_Setup_State ();
    Sound_Playback_Resume ();
}

// ACTION: ENABLE OR DISABLE PALETTE ------------------------------------------
void    Action_Switch_Palette (void)
{
    if (apps.active.Palette ^= 1)
        Msg (MSGT_USER, Msg_Get (MSG_Palette_Enabled));
    else
        Msg (MSGT_USER, Msg_Get (MSG_Palette_Disabled));
    gui_box_show (gui.box[apps.id.Palette], apps.active.Palette, TRUE);
    gui_menu_inverse_check (menus_ID.tools, 1);
}

// ACTION: ENABLE OR DISABLE TECH INFOS ---------------------------------------
void    Action_Switch_Tech (void)
{
    if (apps.active.Tech ^= 1)
        Msg (MSGT_USER, Msg_Get (MSG_TechInfo_Enabled));
    else
        Msg (MSGT_USER, Msg_Get (MSG_TechInfo_Disabled));
    gui_box_show (gui.box[apps.id.Tech], apps.active.Tech, TRUE);
    gui_menu_inverse_check (menus_ID.tools, 4);
}

// ACTION: ENABLE OR DISABLE VOICE RECOGNITION --------------------------------
void    Action_Switch_Voice_Rec (void)
{
    if (apps.active.Voice_Rec ^= 1)
        Msg (MSGT_USER, Msg_Get (MSG_VoiceRecognition_Enabled));
    else
        Msg (MSGT_USER, Msg_Get (MSG_VoiceRecognition_Disabled));
    gui_menu_inverse_check (menus_ID.sound, 5);
    gui_box_show (gui.box[apps.id.Voice_Rec], apps.active.Voice_Rec, TRUE);
    if (apps.active.Voice_Rec)
        gui.box [apps.id.Voice_Rec]->update ();
}

//-----------------------------------------------------------------------------

