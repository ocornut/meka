//-----------------------------------------------------------------------------
// MEKA - inputs.c
// User Inputs & Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_filebrowser.h"
#include "app_options.h"
#include "app_palview.h"
#include "app_techinfo.h"
#include "app_tileview.h"
#include "blitintf.h"
#include "capture.h"
#include "db.h"
#include "debugger.h"
#include "fskipper.h"
#include "glasses.h"
#include "inputs_c.h"
#include "inputs_t.h"
#include "lightgun.h"
#include "saves.h"
#include "sk1100.h"
#include "textbox.h"
#include "tvoekaki.h"
#include "vdp.h"
#include "video.h"
#include "vmachine.h"
#include "sound/sound_logging.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_inputs Inputs;

//-----------------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------------

// SWITCH TO INPUT CURRENTLY DEFINED ------------------------------------------
void    Inputs_Switch_Current (void)
{
    switch (Inputs.Peripheral [PLAYER_1])
    {
    case INPUT_JOYPAD:          Inputs_Switch_Joypad       (); break;
    case INPUT_LIGHTPHASER:     Inputs_Switch_LightPhaser  (); break;
    case INPUT_PADDLECONTROL:   Inputs_Switch_PaddleControl(); break;
    case INPUT_SPORTSPAD:       Inputs_Switch_SportsPad    (); break;
    case INPUT_GRAPHICBOARD:    Inputs_Switch_GraphicBoard (); break;
    case INPUT_GRAPHICBOARD_V2: Inputs_Switch_GraphicBoardV2(); break;
    default: Msg(MSGT_USER, "Error #691: Input type not defined"); break;
    }
}

// SWITCH TO NEXT INPUT PERIPHERAL --------------------------------------------
void    Inputs_Peripheral_Next (int Player)
{
    if (Inputs.Peripheral [Player] == INPUT_SPORTSPAD) // Skip TV Oekaki
        Inputs.Peripheral [Player] = INPUT_JOYPAD;
    else
        Inputs.Peripheral [Player] = (t_input_peripheral)((Inputs.Peripheral [Player] + 1) % INPUT_PERIPHERAL_MAX);
    if (Player == PLAYER_1)
        Inputs_Switch_Current();
    else
        Inputs_CFG_Peripheral_Change (Player, Inputs.Peripheral [Player]);
}

//-----------------------------------------------------------------------------
// Inputs_Check_GUI ()
// Check for GUI related inputs
//-----------------------------------------------------------------------------
// Note: 'sk1100_pressed' tells if SK-1100 emulation has taken a key yet.
// Specific checks are being done here to avoid collision.
//-----------------------------------------------------------------------------
void        Inputs_Check_GUI (bool sk1100_pressed)
{
    // Update INPUTS configuration in priority, since it eat some keys
    Inputs_CFG_Update(&Inputs_CFG);
    //Inputs_CFG_Map_Change_Update();
    //if (Inputs_CFG.active)
    //    Inputs_CFG.Box->update();

    switch (g_keyboard_modifiers & (ALLEGRO_KEYMOD_CTRL | ALLEGRO_KEYMOD_ALT | ALLEGRO_KEYMOD_SHIFT))
    {
    case 0: // No modifiers
        {
            // State save/load
            if (Inputs_KeyPressed(ALLEGRO_KEY_F5, FALSE))
                SaveState_Save();
            if (Inputs_KeyPressed(ALLEGRO_KEY_F7, FALSE))
                SaveState_Load();

            // State change slot
            /*
            if (g_driver->id != DRV_COLECO)
                if (!gui.box_z_ordered[0]->focus_inputs_exclusive) // check note in inputs_u.c::Inputs_Emulation_Update()
                {
                    if (Inputs_KeyPressed (ALLEGRO_KEY_0, FALSE))  Save_Set_Slot (0);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_1, FALSE))  Save_Set_Slot (1);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_2, FALSE))  Save_Set_Slot (2);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_3, FALSE))  Save_Set_Slot (3);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_4, FALSE))  Save_Set_Slot (4);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_5, FALSE))  Save_Set_Slot (5);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_6, FALSE))  Save_Set_Slot (6);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_7, FALSE))  Save_Set_Slot (7);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_8, FALSE))  Save_Set_Slot (8);
                    if (Inputs_KeyPressed (ALLEGRO_KEY_9, FALSE))  Save_Set_Slot (9);
                }
            */
            if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_F6, FALSE, 30, 3)) 
                SaveState_SetPrevSlot();
            if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_F8, FALSE, 30, 3)) 
                SaveState_SetNextSlot();

            // Blitters switch
            if (Inputs_KeyPressed (ALLEGRO_KEY_F1, FALSE))    
                Blitters_SwitchNext();

            // Speed & Frame skip 
            if (Inputs_KeyPressed (ALLEGRO_KEY_F2, FALSE))
                Frame_Skipper_Switch();
            if (Inputs_KeyPressed_Repeat (ALLEGRO_KEY_F3, FALSE, 30, 3))
                Frame_Skipper_Configure (-1);
            if (Inputs_KeyPressed_Repeat (ALLEGRO_KEY_F4, FALSE, 30, 3))
                Frame_Skipper_Configure (1);

            // Input peripheral
            if (Inputs_KeyPressed (ALLEGRO_KEY_F9, FALSE))
                Inputs_Peripheral_Next (PLAYER_1);

            // Switch mode (Fullscreen <-> GUI)
            if (Inputs_KeyPressed (Inputs.Cabinet_Mode ? ALLEGRO_KEY_F10 : ALLEGRO_KEY_ESCAPE, FALSE))
                Action_Switch_Mode();

            // Sprites Display
            if (Inputs_KeyPressed (ALLEGRO_KEY_F11, FALSE))
                Action_Switch_Layer_Sprites();

            // Hard Pause
            if (Inputs_KeyPressed (ALLEGRO_KEY_F12, FALSE))
                g_machine_pause_requests = 1;
        }
        break;
    case ALLEGRO_KEYMOD_CTRL:
        {
            // Hard Reset
            if (!sk1100_pressed && Inputs_KeyPressed(ALLEGRO_KEY_BACKSPACE, TRUE)) // Note: eat backspace to avoid triggering software reset as well
                Machine_Reset();

            // Background Display
            if (Inputs_KeyPressed (ALLEGRO_KEY_F11, FALSE)) 
                Action_Switch_Layer_Background();

            // Next frame (pause hack)
            if (Inputs_KeyPressed (ALLEGRO_KEY_F12, FALSE))
                g_machine_pause_requests = (g_machine_flags & MACHINE_PAUSED) ? 2 : 1;

            // Load State & Continue
            if (Inputs_KeyPressed(ALLEGRO_KEY_F7, FALSE))
            {
                SaveState_Load();
                if (Debugger.active)
                {
                    char command[128] = "CONT";
                    Debugger_InputParseCommand(command);        // non-const input
                }
            }

            // SK-1100 Keyboard switch
            if (Inputs_KeyPressed (ALLEGRO_KEY_F9, FALSE))        
                SK1100_Switch();

            // CTRL-TAB cycle thru boxes with TAB_STOP flag
            if (Inputs_KeyPressed(ALLEGRO_KEY_TAB, FALSE))
            {
                // Remove keypress
                // FIXME-KEYPRESS
                for (t_list* keypresses = Inputs.KeyPressedQueue; keypresses != NULL; )
                {
                    t_key_press* keypress = (t_key_press*)keypresses->elem;
                    keypresses = keypresses->next;
                    if (keypress->scancode == ALLEGRO_KEY_TAB)
                        list_remove(&Inputs.KeyPressedQueue, keypress);
                }

                // Cycle focus
                for (int n = gui.boxes_count - 1; n >= 0; n--)
                {
                    t_gui_box* b = gui.boxes_z_ordered[n];
                    if ((b->flags & GUI_BOX_FLAGS_TAB_STOP) && (b->flags & GUI_BOX_FLAGS_ACTIVE))
                    {
                        gui_box_set_focus(b);
                        break;
                    }
                }
            }

        }
        break;
   case ALLEGRO_KEYMOD_ALT:
       {
           if (!sk1100_pressed)
           {
               // FPS Counter switch
               if (Inputs_KeyPressed (ALLEGRO_KEY_F, FALSE))         
                   Frame_Skipper_Switch_FPS_Counter();
               // Applets hotkeys
               if (Inputs_KeyPressed (ALLEGRO_KEY_L, FALSE)) FB_Switch();
               if (Inputs_KeyPressed (ALLEGRO_KEY_O, FALSE)) Options_Switch();
               if (Inputs_KeyPressed (ALLEGRO_KEY_M, FALSE)) TB_Message_Switch();
               if (Inputs_KeyPressed (ALLEGRO_KEY_P, FALSE)) PaletteViewer_Switch();
               if (Inputs_KeyPressed (ALLEGRO_KEY_T, FALSE)) TileViewer_Switch();
               if (Inputs_KeyPressed (ALLEGRO_KEY_I, FALSE)) TechInfo_Switch();
               if (Inputs_KeyPressed (ALLEGRO_KEY_V, FALSE))
               {
                   if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
                       Sound_LogVGM_Stop();
                   else
                       Sound_LogVGM_Start();
               }

               // Hard Reset
               if (Inputs_KeyPressed(ALLEGRO_KEY_BACKSPACE, TRUE))  // Note: eat backspace to avoid triggering software reset as well
                   Machine_Reset();

               // GUI fullscreen/windowed
                if (Inputs_KeyPressed (ALLEGRO_KEY_ENTER, FALSE))
                {
                    g_configuration.video_fullscreen ^= 1;
                    //al_toggle_display_flag(g_display, ALLEGRO_FULLSCREEN_WINDOW, g_configuration.video_fullscreen);
                    Video_Setup_State();
                    return;
                }

           }
       }
       break;
    }

    // Quit emulator
    if (Inputs_KeyDown(Inputs.Cabinet_Mode ? ALLEGRO_KEY_ESCAPE : ALLEGRO_KEY_F10))
        opt.Force_Quit = TRUE;

    // Debugger switch
    #ifdef MEKA_Z80_DEBUGGER
        // Disabled when SK-1100 is emulated because of collision in usage of ScrollLock key
        // Actually on SC-3000 it is hardwired to NMI
        if (!Inputs.SK1100_Enabled && Inputs_KeyPressed(ALLEGRO_KEY_SCROLLLOCK, TRUE))
        //if (!sk1100_pressed && Inputs_KeyPressed (ALLEGRO_KEY_SCROLLLOCK, TRUE))
            Debugger_Switch();
    #endif

    // Screen capture
    if (Inputs_KeyPressed(ALLEGRO_KEY_PRINTSCREEN, FALSE))
        Capture_Request();

    // SF-7000 Disk 21 Bomber Raid
    // if (Test_Key(KEY_W))
    //     PSG.Registers[6] = PSG.Registers[6] ^ 0x04;

    // Wonder Boy III, Current Song
    //if (key[KEY_J])
    //   Msg(0, "RAM[0xFF9] = %02X", RAM[0xFF9]);
}

// ACTION: SET INPUT TO STANDARD JOYPADS --------------------------------------
void    Inputs_Switch_Joypad()
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_JOYPAD);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Inputs_Joypad));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Inputs_Play_Digital));
    gui_menu_uncheck_range (menus_ID.inputs, 0, 5);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral[PLAYER_1]);
}

// INPUTS: SET INPUT TO LIGHT PHASER ------------------------------------------
void    Inputs_Switch_LightPhaser()
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_LIGHTPHASER);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Inputs_LightPhaser));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Inputs_Play_Mouse));
    gui_menu_uncheck_range (menus_ID.inputs, 0, 5);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral[PLAYER_1]);
}

// INPUTS: SET INPUT TO PADDLE CONTROLLER -------------------------------------
void    Inputs_Switch_PaddleControl()
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_PADDLECONTROL);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Inputs_PaddleControl));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Inputs_Play_Mouse));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Inputs_Play_Digital_Unrecommended));
    gui_menu_uncheck_range (menus_ID.inputs, 0, 5);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral[PLAYER_1]);
}

void    Inputs_Switch_SportsPad()
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_SPORTSPAD);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Inputs_SportsPad));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Inputs_Play_Mouse));
    gui_menu_uncheck_range (menus_ID.inputs, 0, 5);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral[PLAYER_1]);
}

void    Inputs_Switch_GraphicBoard()
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_GRAPHICBOARD);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Inputs_GraphicBoard));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Inputs_Play_Pen));
    gui_menu_uncheck_range (menus_ID.inputs, 0, 5);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral[PLAYER_1]);
}

void    Inputs_Switch_GraphicBoardV2()
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_GRAPHICBOARD_V2);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Inputs_GraphicBoardV2));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Inputs_Play_Pen));
    gui_menu_uncheck_range (menus_ID.inputs, 0, 5);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral[PLAYER_1]);
}

void    Input_ROM_Change()
{
    bool glasses = FALSE;
    t_input_peripheral peripheral = INPUT_JOYPAD;

    if (DB.current_entry)
    {
        if (DB.current_entry->emu_inputs != -1)
            peripheral = (t_input_peripheral)DB.current_entry->emu_inputs;
        if (DB.current_entry->flags & DB_FLAG_EMU_3D)
            glasses = TRUE;
    }
    if (Inputs.Peripheral [PLAYER_1] != peripheral)
    {
        Inputs.Peripheral [PLAYER_1] = peripheral;
        Inputs_Switch_Current();
    }
    if (Glasses.Enabled != glasses)
    {
        Glasses_Switch_Enable();
    }
}

u8  Input_Port_DC (void)
{
    static int paddle_flip_flop = 0;

    if (Inputs.SK1100_Enabled && (sms.Input_Mode & 7) != 7)
    {
        // Msg(MSGT_USER, "Keyboard read %d", sms.Input_Mode & 7);
        return (tsms.Control [sms.Input_Mode & 7] & 0xFF);
    }
    u8 v = tsms.Control[7] & 0xFF;

    // FIXME: Rewrite all this broken shit properly.
    if (Inputs.Peripheral[PLAYER_1] == INPUT_GRAPHICBOARD_V2)
    {
        const t_peripheral_graphic_board_v2* p = &Inputs.GraphicBoardV2[0];
        v &= 0xF0;
        if (tsms.Port3F & 0x10)
        {
            v &= ~0x1F;
        }
        else
        {
            switch (p->read_index & 7)
            {
            case 0: v &= ~0x10; v |= p->buttons ^ 0x0f; break; // board always ready
            case 1: v |= p->unknown >> 4; break;
            case 2: v |= p->unknown & 0x0f; break;
            case 3: v |= p->x >> 4; break;
            case 4: v |= p->x & 0x0f; break;
            case 5: v |= p->y >> 4; break;
            case 6: v |= p->y & 0x0f; break;
            }
        }
        //Msg(MSGT_DEBUG, "in $dc read_index = %d --> %02x", Inputs.GraphicBoardV2[0].read_index, v);
    }

    if (Inputs.Peripheral [PLAYER_1] == INPUT_PADDLECONTROL || Inputs.Peripheral [PLAYER_1] == INPUT_SPORTSPAD
     || Inputs.Peripheral [PLAYER_2] == INPUT_PADDLECONTROL || Inputs.Peripheral [PLAYER_2] == INPUT_SPORTSPAD)
    {
        const t_peripheral_paddle* paddle_1 = &Inputs.Paddle[0];
        const t_peripheral_paddle* paddle_2 = &Inputs.Paddle[1];
        const t_peripheral_sportspad* sportspad_1 = &Inputs.SportsPad[0];
        const t_peripheral_sportspad* sportspad_2 = &Inputs.SportsPad[1];
        switch (tsms.Port3F)
        {
            // SPORT PAD -----------------------------------------------------------
        case 0x0D: // 00001101: SportsPad 1 bits 4-8
            v &= 0xF0;
            v |= 0x0F & (((sportspad_1->latch & 1) ? sportspad_1->y : sportspad_1->x) >> 4);
            break;
        case 0x2D: // 00101101: SportsPad 1 bits 0-3
            v &= 0xF0;
            v |= 0x0F & (((sportspad_1->latch & 1) ? sportspad_1->y : sportspad_1->x)     );
            break;
        case 0x07: // 00001111: SportsPad 2 bits 4-5
            v &= 0x3F;
            v |= 0xC0 & (((sportspad_2->latch & 1) ? sportspad_2->y : sportspad_2->x) << 2);
            break;
        case 0x87: // 10001111: SportsPad 2 bits 0-1
            v &= 0x3F;
            v |= 0xC0 & (((sportspad_2->latch & 1) ? sportspad_2->y : sportspad_2->x) << 6);
            break;

            // PADDLE CONTROL ------------------------------------------------------
            // Japanese "Flip-Flop" mode
        case 0x55: // 01010101
            if (paddle_flip_flop) // Paddle 1 bits 0-3, Paddle 2 bits 0-1
                v &= (paddle_1->x & 0xF) | ((paddle_2->x << 6) & 0xC0) | 0x10;
            else                  // Paddle 1 bits 4-7, Paddle 2 bits 4-5
                v &= ((paddle_1->x >> 4) & 0xF) | ((paddle_2->x << 2) & 0xC0) | 0x10;
            break;
            // Export "Select" mode
        case 0xDD: // 11011101: Paddle 1 bits 0-3, Paddle 2 bits 0-1
            v &= (paddle_1->x & 0xF) | ((paddle_2->x << 6) & 0xC0) | 0x10;
            break;
        case 0xFD: // 11111101: Paddle 1 bits 4-7, Paddle 2 bits 4-5
            v &= ((paddle_1->x >> 4) & 0xF) | ((paddle_2->x << 2) & 0xC0) | 0x10;
            break;
        case 0xDF: // 11011111: (Paddle 2 bits 0-1 ?)
            break;
        case 0xFF: // 11111111: (Paddle 2 bits 4-5 ?)
            break;
            // default: Msg(MSGT_DEBUG, "schmilblik error #4444, %02X", tsms.Port3F);
        }
    }

    if (Inputs.Peripheral [PLAYER_1] == INPUT_PADDLECONTROL)
    {
        paddle_flip_flop ^= 1;
        if (paddle_flip_flop)
            v |= 0x20;
        else
            v &= ~0x20;
    }

    // Msg(MSGT_DEBUG, "AT PC=%04X: IN 0xDC (0x%02X) while Port_3Fh=%02X", CPU_GetPC, v, tsms.Port3F);
    return (v);
}

u8  Input_Port_DD (void)
{
    static int paddle_flip_flop = 0;    // FIXME

    // SK-1100
    if (Inputs.SK1100_Enabled && (sms.Input_Mode & 7) != 7)
        return (tsms.Control [sms.Input_Mode & 7]  >> 8);

    // Controllers
    u8 v = (tsms.Control[7]) >> 8;

    // FIXME: Rewrite all this broken shit properly.
    const t_peripheral_paddle* paddle_1 = &Inputs.Paddle[0];
    const t_peripheral_paddle* paddle_2 = &Inputs.Paddle[1];
    const t_peripheral_sportspad* sportspad_1 = &Inputs.SportsPad[0];
    const t_peripheral_sportspad* sportspad_2 = &Inputs.SportsPad[1];

    if (Inputs.Peripheral [PLAYER_2] == INPUT_PADDLECONTROL || Inputs.Peripheral [PLAYER_2] == INPUT_SPORTSPAD)
        switch (tsms.Port3F)
    {
        // SPORTS PAD ---------------------------------------------------------
        case 0x07: // 00001111: SportsPad 2 bits 6-7
            v &= 0xFC;
            v |= 0x03 & (((sportspad_2->latch & 1) ? sportspad_2->y : sportspad_2->x) >> 4);
            break;
        case 0x87: // 10001111: SportsPad 2 bits 2-3
            v &= 0xFC;
            v |= 0x03 & (((sportspad_2->latch & 1) ? sportspad_2->y : sportspad_2->x) >> 2);
            break;

            // PADDLE CONTROL ------------------------------------------------------
            // Japanese "Flip-Flop" mode
        case 0x55:
            if (paddle_flip_flop) // Paddle 2 bits 2-3
                v &= 0xFC | ((paddle_2->x >> 2) & 0x3);
            else                  // Paddle 2 bits 6-7
                v &= 0xFC | ((paddle_2->x >> 6) & 0x3);
            break;
            // Export "Select" mode
        case 0xDD: // 11011101: Paddle 2 bits 2-3
            v &= 0xFC | ((paddle_2->x >> 2) & 0x3);
            break;
        case 0xFD: // 11111101: Paddle 2 bits 6-7
            v &= 0xFC | ((paddle_2->x >> 6) & 0x3);
            break;
            // default: Msg(MSGT_DEBUG, "schmilblik error #6666, %02X", tsms.Port3F);
    }

    if (Inputs.Peripheral [PLAYER_2] == INPUT_PADDLECONTROL)
    {
        paddle_flip_flop ^= 1;
        // Disabled because paddle 2 is not simulated (input-wise) properly
        // and until that, Super Racing read buttons of the second port as
        // one of the main button, making menus to skip when a paddle is
        // emulated there.
        if (paddle_flip_flop)
            v |= 0x08;
        else
            v &= ~0x08;
    }

    // Light Gun -----------------------------------------------------------------
    if (Inputs.Peripheral [PLAYER_1] == INPUT_LIGHTPHASER)
        LightPhaser_Sync(PLAYER_1, &v);
    if (Inputs.Peripheral [PLAYER_2] == INPUT_LIGHTPHASER)
        LightPhaser_Sync(PLAYER_2, &v);
    // Nationalisation -----------------------------------------------------------
    Nationalize(&v);

    // Msg(MSGT_DEBUG, "AT PC=%04X: IN 0xDD (0x%02X) while Port_3Fh=%02X", CPU_GetPC, v, tsms.Port3F);
    return (v);
}

// UPDATE WHAT IS NECESSARY AFTER A PERIPHERAL CHANGE -------------------------
void    Inputs_Peripheral_Change_Update()
{
    // Update LightGun.Enabled quick access flag
    LightPhaser.Enabled = (Inputs.Peripheral[0] == INPUT_LIGHTPHASER || Inputs.Peripheral[1] == INPUT_LIGHTPHASER);

    if (g_env.mouse_installed == -1)
        return;

    t_mouse_cursor cursor = MEKA_MOUSE_CURSOR_STANDARD; 
    int player = PLAYER_1;

    // Note: Player 1 has priority over Player 2 for cursor
    if (Inputs.Peripheral[PLAYER_2] == INPUT_LIGHTPHASER)       { cursor = MEKA_MOUSE_CURSOR_LIGHT_PHASER; player = PLAYER_2; }
    else if (Inputs.Peripheral[PLAYER_2] == INPUT_SPORTSPAD)    { cursor = MEKA_MOUSE_CURSOR_SPORTS_PAD; player = PLAYER_2; }
    else if (Inputs.Peripheral[PLAYER_2] == INPUT_GRAPHICBOARD || Inputs.Peripheral[PLAYER_2] == INPUT_GRAPHICBOARD_V2) { cursor = MEKA_MOUSE_CURSOR_TV_OEKAKI; player = PLAYER_2; }

    if (Inputs.Peripheral[PLAYER_1] == INPUT_LIGHTPHASER)       { cursor = MEKA_MOUSE_CURSOR_LIGHT_PHASER; player = PLAYER_1; }
    else if (Inputs.Peripheral[PLAYER_1] == INPUT_SPORTSPAD)    { cursor = MEKA_MOUSE_CURSOR_SPORTS_PAD; player = PLAYER_1; }
    else if (Inputs.Peripheral[PLAYER_1] == INPUT_GRAPHICBOARD || Inputs.Peripheral[PLAYER_1] == INPUT_GRAPHICBOARD_V2) { cursor = MEKA_MOUSE_CURSOR_TV_OEKAKI; player = PLAYER_1; }

    // Msg(MSGT_DEBUG, "g_env.state=%d, Cursor=%d, Mask_Left=%d", g_env.state, Cursor, Mask_Left_8);

    switch (g_env.state)
    {
    case MEKA_STATE_GAME:
        if (cursor == MEKA_MOUSE_CURSOR_LIGHT_PHASER)
        {
            Inputs_SetMouseCursor(cursor);
            int sx, sy;
            Video_GameMode_EmulatedPosToScreenPos(LightPhaser.X[player], LightPhaser.Y[player], &sx, &sy, false);
            al_set_mouse_xy(g_display, sx, sy);
        }
        else if (cursor == MEKA_MOUSE_CURSOR_TV_OEKAKI)
        {
            Inputs_SetMouseCursor(cursor);
        }
        else
        {
            Inputs_SetMouseCursor(MEKA_MOUSE_CURSOR_NONE);
        }
        break;
    case MEKA_STATE_GUI:
        if (cursor == MEKA_MOUSE_CURSOR_LIGHT_PHASER)
        {
            Inputs_SetMouseCursor(cursor);
            // Note: do not reposition mouse in GUI, this is annoying!
            // position_mouse (LightGun.X [Player] + gamebox_instance->frame.pos.x, LightGun.Y [Player] + gamebox_instance->frame.pos.y);
        }
        else if (cursor == MEKA_MOUSE_CURSOR_SPORTS_PAD || cursor == MEKA_MOUSE_CURSOR_TV_OEKAKI)
        {
            Inputs_SetMouseCursor(cursor);
        }
        else
        {
            Inputs_SetMouseCursor(MEKA_MOUSE_CURSOR_STANDARD);
        }
        break;
    }
}

// FIXME: Merge with Inputs_Peripheral_Change_Update() ?
void    Inputs_SetMouseCursor(t_mouse_cursor mouse_cursor)
{
    Inputs.mouse_cursor = mouse_cursor;
    if (g_env.mouse_installed == -1)
        return;
    switch (mouse_cursor)
    {
    case MEKA_MOUSE_CURSOR_NONE: 
        al_hide_mouse_cursor(g_display);
        break;
    case MEKA_MOUSE_CURSOR_STANDARD: 
        al_set_mouse_cursor(g_display, Graphics.Cursors.Main);
        al_show_mouse_cursor(g_display);
        break;
    case MEKA_MOUSE_CURSOR_LIGHT_PHASER: 
        al_set_mouse_cursor(g_display, Graphics.Cursors.LightPhaser);
        al_show_mouse_cursor(g_display);
        break;
    case MEKA_MOUSE_CURSOR_SPORTS_PAD: 
        al_set_mouse_cursor(g_display, Graphics.Cursors.SportsPad);
        al_show_mouse_cursor(g_display);
        break;
    case MEKA_MOUSE_CURSOR_TV_OEKAKI: 
        al_set_mouse_cursor(g_display, Graphics.Cursors.TvOekaki);
        al_show_mouse_cursor(g_display);
        break;
    case MEKA_MOUSE_CURSOR_WAIT: 
        al_set_mouse_cursor(g_display, Graphics.Cursors.Wait);
        al_show_mouse_cursor(g_display);
        break;
    }
}

//----------------------------------------------------------

// Short names for UI
const t_input_peripheral_info   Inputs_Peripheral_Infos[INPUT_PERIPHERAL_MAX] =
{
    { "Joypad"          },
    { "Light Phaser"    },
    { "Paddle Control"  },
    { "Sports Pad"      },
    { "Terebi Oekaki"   },
    { "Graphic Board"   },
};

const char* Inputs_Get_MapName (int Type, int MapIdx)
{
    switch (Type)
    {
    case INPUT_SRC_TYPE_KEYBOARD:
    case INPUT_SRC_TYPE_JOYPAD:
        switch (MapIdx)
        {
        case INPUT_MAP_DIGITAL_UP:      return "Up";
        case INPUT_MAP_DIGITAL_DOWN:    return "Down";
        case INPUT_MAP_DIGITAL_LEFT:    return "Left";
        case INPUT_MAP_DIGITAL_RIGHT:   return "Right";
        case INPUT_MAP_BUTTON1:         return "Button 1";
        case INPUT_MAP_BUTTON2:         return "Button 2";
        case INPUT_MAP_PAUSE_START:     return "Pause/Start";
        case INPUT_MAP_RESET:           return "Reset";
        }
        break;
    case INPUT_SRC_TYPE_MOUSE:
        switch (MapIdx)
        {
        case INPUT_MAP_ANALOG_AXIS_X:   return "Axis X";
        case INPUT_MAP_ANALOG_AXIS_Y:   return "Axis Y";
        case INPUT_MAP_BUTTON1:         return "Button 1";
        case INPUT_MAP_BUTTON2:         return "Button 2";
        case INPUT_MAP_PAUSE_START:     return "Pause/Start";
        case INPUT_MAP_RESET:           return "Reset";
        }
        break;
    }
    return NULL;
}

//-----------------------------------------------------------------------------

