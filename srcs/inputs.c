//-----------------------------------------------------------------------------
// MEKA - inputs.c
// User Inputs & Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blitintf.h"
#include "capture.h"
#include "config_v.h"
#include "db.h"
#include "debugger.h"
#include "fskipper.h"
#include "g_file.h"
#include "inputs_t.h"
#include "keyboard.h"
#include "options.h"
#include "saves.h"
#include "vdp.h"
#include "tileview.h"

//-----------------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------------

// SWITCH TO INPUT CURRENTLY DEFINED ------------------------------------------
void    Inputs_Switch_Current (void)
{
    switch (Inputs.Peripheral [PLAYER_1])
    {
    case INPUT_JOYPAD:        Inputs_Switch_Joypad        (); break;
    case INPUT_LIGHTPHASER:   Inputs_Switch_LightPhaser   (); break;
    case INPUT_PADDLECONTROL: Inputs_Switch_PaddleControl (); break;
    case INPUT_SPORTSPAD:     Inputs_Switch_SportPad      (); break;
    case INPUT_TVOEKAKI:      Inputs_Switch_TVOekaki      (); break;
    default: Msg (MSGT_USER, "Error #691: Input type not defined"); break;
    }
}

// SWITCH TO NEXT INPUT PERIPHERAL --------------------------------------------
void    Inputs_Peripheral_Next (int Player)
{
    if (Inputs.Peripheral [Player] == INPUT_SPORTSPAD) // Skip TV Oekaki
        Inputs.Peripheral [Player] = INPUT_JOYPAD;
    else
        Inputs.Peripheral [Player] = (Inputs.Peripheral [Player] + 1) % INPUT_PERIPHERAL_MAX;
    if (Player == PLAYER_1)
        Inputs_Switch_Current ();
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
    if (Inputs_CFG.Active)
        gui.box [Inputs_CFG.ID]->update ();

    switch (key_shifts & (KB_CTRL_FLAG | KB_ALT_FLAG | KB_SHIFT_FLAG))
    {
    case 0: // No modifiers
        {
            // Sprites Refresh switch
            if (Inputs_KeyPressed (KEY_F11, NO))
                Action_Switch_Layer_Sprites ();
            // Hard Pause
            if (Inputs_KeyPressed (KEY_F12, NO))
                Machine_Pause_Need_To = YES;
        }
        break;
    case KB_CTRL_FLAG:
        {
            // Easter egg: Tetris
            if (!sk1100_pressed && Inputs_KeyPressed (KEY_T, NO))
                Tetris_Start ();
            // Nintendon't - now removed because it is too violent
            #ifdef DOS
                if (!sk1100_pressed && Inputs_KeyPressed (KEY_N, NO))     
                    Quit_Msg ("Nintendont.");
            #endif
            // Hard Pause
            if (Inputs_KeyPressed (KEY_F12, NO) || (!sk1100_pressed && Inputs_KeyPressed (KEY_P, NO)))
                Machine_Pause_Need_To = YES;
            // Hard Reset
            if (!sk1100_pressed && Inputs_KeyPressed (KEY_BACKSPACE, NO))
                Machine_Reset ();
        }
        break;
   case KB_ALT_FLAG:
       {
           // SK-1100 Keyboard switch
           if (Inputs_KeyPressed (KEY_F9, NO))        
               Keyboard_Switch ();
           // Background Refresh switch
           if (Inputs_KeyPressed (KEY_F11, NO)) 
               Action_Switch_Layer_Background ();
           // Next frame (pause hack)
           if (Inputs_KeyPressed (KEY_F12, NO))
               Machine_Pause_Need_To = (machine & MACHINE_PAUSED) ? 2 : 1;

           if (!sk1100_pressed)
           {
               // FPS Counter switch
               if (Inputs_KeyPressed (KEY_F, NO))         
                   Frame_Skipper_Switch_FPS_Counter ();
               // Applets hotkeys
               if (Inputs_KeyPressed (KEY_L, NO))         FB_Switch ();
               if (Inputs_KeyPressed (KEY_O, NO))         Options_Switch ();
               if (Inputs_KeyPressed (KEY_M, NO))         TB_Message_Switch ();
               if (Inputs_KeyPressed (KEY_P, NO))         Action_Switch_Palette ();
               if (Inputs_KeyPressed (KEY_T, NO))         TileViewer_Switch ();
               if (Inputs_KeyPressed (KEY_I, NO))         Action_Switch_Tech ();
               // Quit emulator
               if (Inputs_KeyPressed (KEY_X, NO))         
                   opt.Force_Quit = YES;
               // Hard Reset
               if (Inputs_KeyPressed (KEY_BACKSPACE, NO)) 
                   Machine_Reset ();

               // GUI fullscreen/windowed
                if (Inputs_KeyPressed (KEY_ENTER, NO))
                {
                    if (Meka_State == MEKA_STATE_FULLSCREEN)
                    {
                        t_video_driver *driver = VideoDriver_FindByDriverId(blitters.current->driver);
                        if (driver && driver->drv_id_switch_fs_win)
                        {
                            // FIXME: Put that properly in blitter interface
                            // FIXME: Not saved anywhere... better than nothing anyway.
                            blitters.current->driver = driver->drv_id_switch_fs_win;
                            Blitters_Current_Update();
                            Video_Setup_State();
                        }
                    }
                    else if (Meka_State == MEKA_STATE_GUI)
                    {
                        t_video_driver *driver = VideoDriver_FindByDriverId(cfg.GUI_Driver);
                        if (driver && driver->drv_id_switch_fs_win)
                        {
                            cfg.GUI_Driver = driver->drv_id_switch_fs_win;
                            Video_GUI_ChangeVideoMode(cfg.GUI_Res_X, cfg.GUI_Res_Y, 8);
                        }
                    }
                    return;
                }

           }
       }
       break;
    }

    // Quit emulator
    if (key [Inputs.Cabinet_Mode ? KEY_ESC : KEY_F10]) 
        opt.Force_Quit = YES;

    // Debugger switch
    #ifdef MEKA_Z80_DEBUGGER
        if (!sk1100_pressed && Inputs_KeyPressed (KEY_SCRLOCK, YES))
            Debugger_Switch ();
    #endif

    // State save/load
    if (Inputs_KeyPressed (KEY_F5, NO))
        Save_Game ();
    if (Inputs_KeyPressed (KEY_F7, NO))
        Load_Game ();

    // State change slot
    /*
    if (cur_drv->id != DRV_COLECO)
        if (!gui.box[gui.box_plan[0]]->focus_inputs_exclusive) // check note in inputs_u.c::Inputs_Emulation_Update()
        {
            if (Inputs_KeyPressed (KEY_0, NO))  Save_Set_Slot (0);
            if (Inputs_KeyPressed (KEY_1, NO))  Save_Set_Slot (1);
            if (Inputs_KeyPressed (KEY_2, NO))  Save_Set_Slot (2);
            if (Inputs_KeyPressed (KEY_3, NO))  Save_Set_Slot (3);
            if (Inputs_KeyPressed (KEY_4, NO))  Save_Set_Slot (4);
            if (Inputs_KeyPressed (KEY_5, NO))  Save_Set_Slot (5);
            if (Inputs_KeyPressed (KEY_6, NO))  Save_Set_Slot (6);
            if (Inputs_KeyPressed (KEY_7, NO))  Save_Set_Slot (7);
            if (Inputs_KeyPressed (KEY_8, NO))  Save_Set_Slot (8);
            if (Inputs_KeyPressed (KEY_9, NO))  Save_Set_Slot (9);
        }
    */
    if (Inputs_KeyPressed_Repeat (KEY_F6, NO, 30, 3)) 
        Save_Set_Slot (opt.State_Current - 1);
    if (Inputs_KeyPressed_Repeat (KEY_F8, NO, 30, 3)) 
        Save_Set_Slot (opt.State_Current + 1);

    // Blitters switch
    if (Inputs_KeyPressed (KEY_F1, NO))    
        Blitters_Switch ();

    // Speed & Frame skip 
    if (Inputs_KeyPressed (KEY_F2, NO))
        Frame_Skipper_Switch ();
    if (Inputs_KeyPressed (KEY_F3, NO))
        Frame_Skipper_Configure (-1);
    if (Inputs_KeyPressed (KEY_F4, NO))
        Frame_Skipper_Configure (1);

    // Input peripheral
    if (Inputs_KeyPressed (KEY_F9, NO))
        Inputs_Peripheral_Next (PLAYER_1);

    // Screen capture
    if (Inputs_KeyPressed (KEY_PRTSCR, NO))
        Capture_Request ();

    // Switch mode (Fullscreen <-> GUI)
    if (Inputs_KeyPressed (Inputs.Cabinet_Mode ? KEY_F10 : KEY_ESC, NO))
        Action_Switch_Mode ();

    // SF-7000 Disk 21 Bomber Raid
    // if (Test_Key(KEY_W))
    //     PSG.Registers[6] = PSG.Registers[6] ^ 0x04;

    // Wonder Boy III, Current Song
    //if (key[KEY_J])
    //   Msg (0, "RAM[0xFF9] = %02X", RAM[0xFF9]);

    // Old video modes debugging
    // if (Test_Key(KEY_J)) BACK_AREA -= 0x100;
    // if (Test_Key(KEY_K)) BACK_AREA += 0x100;
    // if (Test_Key(KEY_U)) BACK_AREA -= 0x1;
    // if (Test_Key(KEY_I)) BACK_AREA += 0x1;
    // SG-1000 stuff
    // if (Test_Key(KEY_U)) SG_BACK_TILE -= 0x100;
    // if (Test_Key(KEY_I)) SG_BACK_TILE += 0x100;
    // if (Test_Key(KEY_O)) SG_BACK_TILE -= 0x2000;

    /*
    if (Test_Key(KEY_U))
        { Debug_Generic_Value ++; Msg (MSGT_DEBUG, "Debug_Generic_Value: %d -> %d", Debug_Generic_Value - 1, Debug_Generic_Value); }
    if (Test_Key(KEY_I))
        { Debug_Generic_Value --; Msg (MSGT_DEBUG, "Debug_Generic_Value: %d -> %d", Debug_Generic_Value + 1, Debug_Generic_Value); }
    */
}

// ACTION: SET INPUT TO STANDARD JOYPADS --------------------------------------
void    Inputs_Switch_Joypad (void)
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_JOYPAD);
    Msg (MSGT_USER, Msg_Get (MSG_Inputs_Joypad));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Inputs_Play_Digital));
    gui_menu_un_check_area (menus_ID.inputs, 0, 4);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral [PLAYER_1]);
}

// INPUTS: SET INPUT TO LIGHT PHASER ------------------------------------------
void    Inputs_Switch_LightPhaser (void)
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_LIGHTPHASER);
    Msg (MSGT_USER, Msg_Get (MSG_Inputs_LightPhaser));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Inputs_Play_Mouse));
    gui_menu_un_check_area (menus_ID.inputs, 0, 4);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral [PLAYER_1]);
}

// INPUTS: SET INPUT TO PADDLE CONTROLLER -------------------------------------
void    Inputs_Switch_PaddleControl (void)
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_PADDLECONTROL);
    // Easter egg: Pong
    if (Inputs_KeyPressed (KEY_P, NO)) 
        Pong_Start ();
    Msg (MSGT_USER, Msg_Get (MSG_Inputs_PaddleControl));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Inputs_Play_Mouse));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Inputs_Play_Digital_Unrecommended));
    gui_menu_un_check_area (menus_ID.inputs, 0, 4);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral [PLAYER_1]);
}

void    Inputs_Switch_SportPad (void)
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_SPORTSPAD);
    Msg (MSGT_USER, Msg_Get (MSG_Inputs_SportsPad));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Inputs_Play_Mouse));
    gui_menu_un_check_area (menus_ID.inputs, 0, 4);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral [PLAYER_1]);
}

void    Inputs_Switch_TVOekaki (void)
{
    Inputs_CFG_Peripheral_Change (PLAYER_1, INPUT_TVOEKAKI);
    Msg (MSGT_USER, Msg_Get (MSG_Inputs_TVOekaki));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Inputs_Play_Pen));
    gui_menu_un_check_area (menus_ID.inputs, 0, 4);
    gui_menu_check (menus_ID.inputs, Inputs.Peripheral [PLAYER_1]);
}

void    Input_ROM_Change (void)
{
    int     input = INPUT_JOYPAD;
    bool    glasses = NO;
    if (DB_CurrentEntry)
    {
        if (DB_CurrentEntry->emu_inputs != -1)
            input = DB_CurrentEntry->emu_inputs;
        if (DB_CurrentEntry->flags & DB_FLAG_EMU_3D)
            glasses = YES;
    }
    if (Inputs.Peripheral [PLAYER_1] != input)
    {
        Inputs.Peripheral [PLAYER_1] = input;
        Inputs_Switch_Current ();
    }
    if (Glasses.Enabled != glasses)
    {
        Glasses_Switch_Enable ();
    }
}

byte    Input_Port_DC (void)
{
    byte   v;
    static int paddle_flip_flop = 0;

    if (Inputs.Keyboard_Enabled && (sms.Input_Mode & 7) != 7)
    {
        // Msg (MSGT_USER, "Keyboard read %d", sms.Input_Mode & 7);
        return (tsms.Control [sms.Input_Mode & 7] & 0xFF);
    }
    v = tsms.Control[7] & 0xFF;

    if (Inputs.Peripheral [PLAYER_1] == INPUT_PADDLECONTROL
        || Inputs.Peripheral [PLAYER_1] == INPUT_SPORTSPAD
        || Inputs.Peripheral [PLAYER_2] == INPUT_PADDLECONTROL
        || Inputs.Peripheral [PLAYER_2] == INPUT_SPORTSPAD)
        switch (tsms.Periph_Nat)
    {
        // SPORT PAD -----------------------------------------------------------
        case 0x0D: // 00001101: Sportpad 1 bits 4-8
            v &= 0xF0;
            v |= 0x0F & (Inputs.SportPad_XY [PLAYER_1] [Inputs.SportPad_Latch [PLAYER_1] & 1] >> 4);
            break;
        case 0x2D: // 00101101: Sportpad 1 bits 0-3
            v &= 0xF0;
            v |= 0x0F & (Inputs.SportPad_XY [PLAYER_1] [Inputs.SportPad_Latch [PLAYER_1] & 1]);
            break;
        case 0x07: // 00001111: Sportpad 2 bits 4-5
            v &= 0x3F;
            v |= 0xC0 & (Inputs.SportPad_XY [PLAYER_2] [Inputs.SportPad_Latch [PLAYER_2] & 1] << 2);
            break;
        case 0x87: // 10001111: Sportpad 2 bits 0-1
            v &= 0x3F;
            v |= 0xC0 & (Inputs.SportPad_XY [PLAYER_2] [Inputs.SportPad_Latch [PLAYER_2] & 1] << 6);
            break;

            // PADDLE CONTROL ------------------------------------------------------
            // Japanese "Flip-Flop" mode
        case 0x55: // 01010101
            if (paddle_flip_flop) // Paddle 1 bits 0-3, Paddle 2 bits 0-1
                v &= (Inputs.Paddle_X [PLAYER_1] & 0xF) | ((Inputs.Paddle_X [PLAYER_2] << 6) & 0xC0) | 0x10;
            else                  // Paddle 1 bits 4-7, Paddle 2 bits 4-5
                v &= ((Inputs.Paddle_X [PLAYER_1] >> 4) & 0xF) | ((Inputs.Paddle_X [PLAYER_2] << 2) & 0xC0) | 0x10;
            break;
            // Export "Select" mode
        case 0xDD: // 11011101: Paddle 1 bits 0-3, Paddle 2 bits 0-1
            v &= (Inputs.Paddle_X [PLAYER_1] & 0xF) | ((Inputs.Paddle_X [PLAYER_2] << 6) & 0xC0) | 0x10;
            break;
        case 0xFD: // 11111101: Paddle 1 bits 4-7, Paddle 2 bits 4-5
            v &= ((Inputs.Paddle_X [PLAYER_1] >> 4) & 0xF) | ((Inputs.Paddle_X [PLAYER_2] << 2) & 0xC0) | 0x10;
            break;
        case 0xDF: // 11011111: (Paddle 2 bits 0-1 ?)
            break;
        case 0xFF: // 11111111: (Paddle 2 bits 4-5 ?)
            break;
            // default: Msg (MSGT_DEBUG, "schmilblik error #4444, %02X", tsms.Periph_Nat);
    }

    if (Inputs.Peripheral [PLAYER_1] == INPUT_PADDLECONTROL)
    {
        paddle_flip_flop ^= 1;
        if (paddle_flip_flop)
            v |= 0x20;
        else
            v &= ~0x20;
    }

    // Msg (MSGT_DEBUG, "AT PC=%04X: IN 0xDC (0x%02X) while Port_3Fh=%02X", CPU_GetPC, v, tsms.Periph_Nat);
    return (v);
}

byte    Input_Port_DD (void)
{
    byte   v;
    static int paddle_flip_flop = 0;

    // SK-1100
    if (Inputs.Keyboard_Enabled && (sms.Input_Mode & 7) != 7)
        return (tsms.Control [sms.Input_Mode & 7]  >> 8);

    // Controllers
    v = (tsms.Control[7]) >> 8;

    if (Inputs.Peripheral [PLAYER_2] == INPUT_PADDLECONTROL
        || Inputs.Peripheral [PLAYER_2] == INPUT_SPORTSPAD)
        switch (tsms.Periph_Nat)
    {
        // SPORTS PAD ---------------------------------------------------------
        case 0x07: // 00001111: Sportpad 2 bits 6-7
            v &= 0xFC;
            v |= 0x03 & (Inputs.SportPad_XY [PLAYER_2] [Inputs.SportPad_Latch [PLAYER_2] & 1] >> 6);
            break;
        case 0x87: // 10001111: Sportpad 2 bits 2-3
            v &= 0xFC;
            v |= 0x03 & (Inputs.SportPad_XY [PLAYER_2] [Inputs.SportPad_Latch [PLAYER_2] & 1] >> 2);
            break;

            // PADDLE CONTROL ------------------------------------------------------
            // Japanese "Flip-Flop" mode
        case 0x55:
            if (paddle_flip_flop) // Paddle 2 bits 2-3
                v &= 0xFC | ((Inputs.Paddle_X [PLAYER_2] >> 2) & 0x3);
            else                  // Paddle 2 bits 6-7
                v &= 0xFC | ((Inputs.Paddle_X [PLAYER_2] >> 6) & 0x3);
            break;
            // Export "Select" mode
        case 0xDD: // 11011101: Paddle 2 bits 2-3
            v &= 0xFC | ((Inputs.Paddle_X [PLAYER_2] >> 2) & 0x3);
            break;
        case 0xFD: // 11111101: Paddle 2 bits 6-7
            v &= 0xFC | ((Inputs.Paddle_X [PLAYER_2] >> 6) & 0x3);
            break;
            // default: Msg (MSGT_DEBUG, "schmilblik error #6666, %02X", tsms.Periph_Nat);
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
        LightGun_Sync (PLAYER_1, &v);
    if (Inputs.Peripheral [PLAYER_2] == INPUT_LIGHTPHASER)
        LightGun_Sync (PLAYER_2, &v);
    // Nationalisation -----------------------------------------------------------
    Nationalize (&v);

    // Msg (MSGT_DEBUG, "AT PC=%04X: IN 0xDD (0x%02X) while Port_3Fh=%02X", CPU_GetPC, v, tsms.Periph_Nat);
    return (v);
}

// UPDATE WHAT IS NECESSARY AFTER A PERIPHERAL CHANGE -------------------------
void    Inputs_Peripheral_Change_Update (void)
{
    int    Cursor, Player;

    // Update LightGun.Enabled quick access flag
    LightGun.Enabled = (Inputs.Peripheral [0] == INPUT_LIGHTPHASER || Inputs.Peripheral [1] == INPUT_LIGHTPHASER);

    if (cfg.Mouse_Installed == -1)
        return;

    Cursor = 1; // Default GUI cursor
    Player = PLAYER_1;
    opt.Fullscreen_Cursor = NO;

    // Note: Player 1 has priority over Player 2 for cursor
    if (Inputs.Peripheral [PLAYER_2] == INPUT_LIGHTPHASER)    { Cursor = 2; Player = PLAYER_2; }
    else if (Inputs.Peripheral [PLAYER_2] == INPUT_SPORTSPAD) { Cursor = 3; Player = PLAYER_2; }
    else if (Inputs.Peripheral [PLAYER_2] == INPUT_TVOEKAKI)  { Cursor = 4; Player = PLAYER_2; }

    if (Inputs.Peripheral [PLAYER_1] == INPUT_LIGHTPHASER)    { Cursor = 2; Player = PLAYER_1; }
    else if (Inputs.Peripheral [PLAYER_1] == INPUT_SPORTSPAD) { Cursor = 3; Player = PLAYER_1; }
    else if (Inputs.Peripheral [PLAYER_1] == INPUT_TVOEKAKI)  { Cursor = 4; Player = PLAYER_1; }

    // Msg(MSGT_DEBUG, "Meka_State=%d, Cursor=%d, Mask_Left=%d", Meka_State, Cursor, Mask_Left_8);

    switch (Meka_State)
    {
    case MEKA_STATE_FULLSCREEN:
        if (Cursor == 2) // Light Phaser
        {
            Set_Mouse_Cursor (Cursor);
            LightGun_Mouse_Range (Mask_Left_8);
            position_mouse (LightGun.X [Player], LightGun.Y [Player]);
            show_mouse (screenbuffer);
            opt.Fullscreen_Cursor = YES;
        }
        else
            if (Cursor == 4) // Terebi Oekaki
            {
                Set_Mouse_Cursor (Cursor);
                TVOekaki_Mouse_Range ();
                show_mouse (screenbuffer);
                opt.Fullscreen_Cursor = YES;
            }
            else
            {
                Set_Mouse_Cursor (0);
                show_mouse (NULL);
            }
            break;
    case MEKA_STATE_GUI:
        if (Cursor == 2) // Light Phaser
        {
            Set_Mouse_Cursor (Cursor);
            position_mouse (LightGun.X [Player] + gui.box [apps.id_game]->frame.pos.x, LightGun.Y [Player] + gui.box [apps.id_game]->frame.pos.y);
        }
        else
            if (Cursor == 3) // Sport Pad
                Set_Mouse_Cursor (Cursor);
            else
                if (Cursor == 4) // Terebi Oekaki
                    Set_Mouse_Cursor (Cursor);
                else
                    Set_Mouse_Cursor (1);
        show_mouse (gui_buffer);
        break;
    }
}

//----------------------------------------------------------

t_input_peripheral_info Inputs_Peripheral_Infos [INPUT_PERIPHERAL_MAX] =
{
    { "Joypad",           DIGITAL         },
    { "Light Phaser",     ANALOG          },
    { "Paddle Control",   ANALOG          },
    { "Sports Pad",       ANALOG          },
    { "Terebi Oekaki",    ANALOG          }
};

char    *Inputs_Get_MapName (int Type, int MapIdx)
{
    switch (Type)
    {
    case INPUT_SRC_KEYBOARD:
    case INPUT_SRC_JOYPAD:
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
    case INPUT_SRC_MOUSE:
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

