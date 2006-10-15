//-----------------------------------------------------------------------------
// MEKA - inputs_u.c
// Inputs Update - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "keyboard.h"
// #define DEBUG_JOY

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void    Inputs_FixUp_JoypadOppositesDirections (void);
#ifdef DOS
static void    Inputs_Update_VoiceRecognition (void);
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void        Inputs_Sources_Init (void)
{
    Inputs.Sources = NULL;
    Inputs.Sources_Max = 0;
    Inputs.Sources_Joy_Driver = JOY_TYPE_AUTODETECT;

    Inputs.Peripheral [0] = INPUT_JOYPAD;
    Inputs.Peripheral [1] = INPUT_JOYPAD;
}

t_input_src *       Inputs_Sources_Add (char *name)
{
    int             i;
    t_input_src *   Src = malloc (sizeof (t_input_src));

    Src->Name                      = name;
    Src->Type                      = INPUT_SRC_KEYBOARD;
    Src->Enabled                   = YES;
    Src->Player                    = PLAYER_1;
    Src->Connection_Port           = 0;
    Src->Driver                    = 0;
    Src->Result_Type               = DIGITAL;
    Src->Analog_to_Digital_FallOff = 0.8f;
    Src->Connected_and_Ready       = NO; // by default. need to be flagged for use

    for (i = 0; i < INPUT_MAP_MAX; i++)
    {
        Src->Map[i].Type = 0; // key,button,..
        Src->Map[i].Idx = -1;
        Src->Map[i].Res = 0;
        Src->Map_Counters[i] = 0;
    }

    Inputs.Sources = realloc (Inputs.Sources, (Inputs.Sources_Max + 1) * sizeof (t_input_src *));
    Inputs.Sources [Inputs.Sources_Max] = Src;
    Inputs.Sources_Max ++;

    return (Src);
}

void       Inputs_Sources_Close (void)
{
    int    i;

    for (i = 0; i < Inputs.Sources_Max; i++)
    {
        if (Inputs.Sources[i]->Name)
            free (Inputs.Sources[i]->Name);
        free (Inputs.Sources[i]);
    }
    free (Inputs.Sources);
    Inputs.Sources = NULL;
    Inputs.Sources_Max = 0;
}

//-----------------------------------------------------------------------------
// Inputs_Emulation_Update ()
// Update emulation inputs, based on inputs sources data
//-----------------------------------------------------------------------------
// If 'running' is false, only update emulation state, but do not produce
// any side effect such as backspace doing an automatic HardReset in GG mode.
//-----------------------------------------------------------------------------
void        Inputs_Emulation_Update (bool running)
{
    int     i, j;
    u16 *   c;
    int     Pause_Pressed = NO, Reset_Pressed = NO;
    int     players;

    //----------------------------------------------------------------------------
    // Control[7] is the following:
    // LG2.LG1.Unused.Reset.P2B.P2A.P2R.P2L - P2D.P2U.P1B.P1A.P1R.P1L.P1D.P1U
    // Now setting all bits (active logic)
    c = &tsms.Control[7];
    *c |= 0x1FFF;

    // Now this is tricky... if we are in GUI mode, check if the focused box
    // has the exclusive inputs flag. If it has it and the machine is running 
    // (not paused or being debugged), return now.
    // The reason for doing that is that handling inputs priority is very
    // complicated with the current GUI, because applet/widget updating code
    // is mostly only run on emulation frame that are not skipped, while inputs
    // for emulation should run for all frames (including skipped ones).
    // So it is a bit complicated to handle a way for an applet to 'eat' a key, 
    // and I use an easy path, that is until rewriting the GUI.
    if (Meka_State == MEKA_STATE_GUI && !(machine & MACHINE_PAUSED))
        if (gui.box[gui.box_plan[0]]->focus_inputs_exclusive)
        {
            // Returning from the emulation inputs update requires to take care
            // of a few variables...
            if (tsms.Control_Start_Pause == 1) // Leave it if it is == 2
                tsms.Control_Start_Pause = 0;
            if (cur_drv->id == DRV_GG)
                tsms.Control_GG |= (0x80);
            if (Inputs.Keyboard_Enabled)
                Keyboard_Emulation_Clear();
            return;
        }

    // Convert input sources data to emulation inputs data
    players = ((cur_drv->id == DRV_GG) ? 1 : 2); // 1 player on GG, else 2
    for (i = 0; i < players; i++)
        for (j = 0; j < Inputs.Sources_Max; j++)
        {
            t_input_src *Src = Inputs.Sources[j];
            if (Src->Enabled == NO || Src->Player != i)
                continue;
            // Starting from here, source 'j' apply to player 'i' ------------------
            // If current peripheral is digital, skip analog only inputs sources
            // If current peripheral is analog, skip digital only inputs sources
            // k = Inputs_Peripheral_Infos [Inputs.Peripheral [i]].Result_Type;
            // if (!(Src->Result_Type & (k | (k << 1))))
            //    continue;
            // Process peripheral dependant stuff
            switch (Inputs.Peripheral [i])
            {
            case INPUT_JOYPAD: //---------------------------- Joypad/Control Stick
                if (Src->Result_Type & DIGITAL)
                {
                    if (Src->Map[INPUT_MAP_DIGITAL_UP].Res)     *c &= (!i? ~0x0001 : ~0x0040);
                    if (Src->Map[INPUT_MAP_DIGITAL_DOWN].Res)   *c &= (!i? ~0x0002 : ~0x0080);
                    if (Src->Map[INPUT_MAP_DIGITAL_LEFT].Res)   *c &= (!i? ~0x0004 : ~0x0100);
                    if (Src->Map[INPUT_MAP_DIGITAL_RIGHT].Res)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (Src->Map[INPUT_MAP_BUTTON1].Res)        *c &= (!i? ~0x0010 : ~0x0400);
                    if (Src->Map[INPUT_MAP_BUTTON2].Res)        *c &= (!i? ~0x0020 : ~0x0800);
                }
                else if (Src->Result_Type & EMULATE_DIGITAL) // ANALOG
                {
                    if (Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res < 0)  *c &= (!i? ~0x0001 : ~0x0040);
                    if (Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res > 0)  *c &= (!i? ~0x0002 : ~0x0080);
                    if (Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res < 0)  *c &= (!i? ~0x0004 : ~0x0100);
                    if (Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res > 0)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (Src->Map[INPUT_MAP_BUTTON1].Res)                *c &= (!i? ~0x0010 : ~0x0400);
                    if (Src->Map[INPUT_MAP_BUTTON2].Res)                *c &= (!i? ~0x0020 : ~0x0800);
                }
                break;
            case INPUT_LIGHTPHASER: //------------------------------- Light Phaser
                if (Src->Result_Type & ANALOG)
                    LightGun_Update (i, Src->Map[INPUT_MAP_ANALOG_AXIS_X].Res, Src->Map[INPUT_MAP_ANALOG_AXIS_Y].Res);
                if (Src->Map[INPUT_MAP_BUTTON1].Res)           *c &= (!i? ~0x0010 : ~0x0400);
                if (Src->Map[INPUT_MAP_BUTTON2].Res)           *c &= (!i? ~0x0020 : ~0x0800);
                break;
            case INPUT_PADDLECONTROL: //------------------------ Paddle Controller
                {
                    int x = Inputs.Paddle_X[i];
                    int dx = 0;
                    if (Src->Result_Type & ANALOG)
                    {
                        // Using analogic relative movement
                        dx = Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res;
                        if (dx > 0)
                            dx = (dx + 4) / 5;
                        else if (dx < 0)
                            dx = (dx - 4) / 5;
                    }
                    else
                    {
                        // Using digital
                        // Linear maxed acceleration
                        const int dx_table[1+5] = 
                        { 0, 1, 2, 3, 5, 7 };
                        //0  1  2  3  4  5, 6
                        //Msg (MSGT_DEBUG, "Map_Counters[DIGITAL_LEFT] = %d, [DIGITAL_RIGHT] = %d", Src->Map_Counters[INPUT_MAP_DIGITAL_LEFT], Src->Map_Counters[INPUT_MAP_DIGITAL_RIGHT]);
                        if (Src->Map_Counters[INPUT_MAP_DIGITAL_LEFT] && !Src->Map_Counters[INPUT_MAP_DIGITAL_RIGHT])
                        {
                            dx = Src->Map_Counters[INPUT_MAP_DIGITAL_LEFT];
                            dx = -dx_table[(dx > 5) ? 5 : dx];
                        }
                        if (Src->Map_Counters[INPUT_MAP_DIGITAL_RIGHT] && !Src->Map_Counters[INPUT_MAP_DIGITAL_LEFT])
                        {
                            dx = Src->Map_Counters[INPUT_MAP_DIGITAL_RIGHT];
                            dx = dx_table[(dx > 5) ? 5 : dx];
                        }
                    }
                    if (dx != 0)
                    {
                        x += dx;
                        if (x < 0) x = 0; else if (x > 255) x = 255;
                        Inputs.Paddle_X [i] = x;
                    }
                    // Button 1 (only one button on Paddle Control)
                    if (Src->Map[INPUT_MAP_BUTTON1].Res)           *c &= (!i? ~0x0010 : ~0x0400);
                    break;
                }
            case INPUT_SPORTSPAD: //--------------------------------- Sports Pads
                if (Src->Result_Type & ANALOG)
                    SportPad_Update (i, Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res, Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res);
                if (Src->Map[INPUT_MAP_BUTTON1].Res)           *c &= (!i? ~0x0010 : ~0x0400);
                if (Src->Map[INPUT_MAP_BUTTON2].Res)           *c &= (!i? ~0x0020 : ~0x0800);
                break;
            case INPUT_TVOEKAKI: //--------------------------------- Terebi Oekaki
                if (Src->Result_Type & ANALOG)
                {
                    // Create button field (this is due to old code legacy)
                    int b_field = (Src->Map[INPUT_MAP_BUTTON1].Res ? 1 : 0) | (Src->Map[INPUT_MAP_BUTTON2].Res ? 2 : 0);
                    TVOekaki_Update (Src->Map[INPUT_MAP_ANALOG_AXIS_X].Res, Src->Map[INPUT_MAP_ANALOG_AXIS_Y].Res, b_field);
                }
                else // Support standard controller with digital inputs, because the
                {    // Terebi Oekaki is not connected to the controller port, it is free to use !
                    if (Src->Map[INPUT_MAP_DIGITAL_UP].Res)     *c &= (!i? ~0x0001 : ~0x0040);
                    if (Src->Map[INPUT_MAP_DIGITAL_DOWN].Res)   *c &= (!i? ~0x0002 : ~0x0080);
                    if (Src->Map[INPUT_MAP_DIGITAL_LEFT].Res)   *c &= (!i? ~0x0004 : ~0x0100);
                    if (Src->Map[INPUT_MAP_DIGITAL_RIGHT].Res)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (Src->Map[INPUT_MAP_BUTTON1].Res)        *c &= (!i? ~0x0010 : ~0x0400);
                    if (Src->Map[INPUT_MAP_BUTTON2].Res)        *c &= (!i? ~0x0020 : ~0x0800);
                }
                break;
            }
            // Process RESET and PAUSE/START buttons
            if (Src->Map[INPUT_MAP_PAUSE_START].Res) { Pause_Pressed = YES; if (tsms.Control_Start_Pause == 0) tsms.Control_Start_Pause = 1; }
            if (Src->Map[INPUT_MAP_RESET].Res)       { Reset_Pressed = YES; }
        }

    // SK-1100 Keyboard update
    if (Inputs.Keyboard_Enabled)
        Keyboard_Emulation_Update ();

    // Handle reset and clear pause latch if necessary
    if (Reset_Pressed == YES)
    {
        if (cur_drv->id == DRV_SMS)
        {
            // Set the reset bit on SMS
            // FIXME: What about always setting it? (not only on SMS)
            *c &= ~0x1000;
        }
        else
        {
            if (running)
            {
                // If SK-1100 is not emulated then process with an hardware Reset
                // Note: this test is invalid in case Reset was pressed from a pad, it will cancel pressing reset from the pad
                if (Inputs.Keyboard_Enabled == NO)
                    Machine_Reset();
            }
        }
    }

    if (Pause_Pressed == NO)
        tsms.Control_Start_Pause = 0;

    // Game Gear Start button
    if (cur_drv->id == DRV_GG)
    {
        if (tsms.Control_Start_Pause == 1)
            tsms.Control_GG &= (~0x80);
        else
            tsms.Control_GG |= (0x80);
    }

    // Correct the cases where opposite directions are pressed
    if (!Configuration.allow_opposite_directions)
        Inputs_FixUp_JoypadOppositesDirections ();

    // Simulate Rapid Fire
    if (running)
    {
        if (RapidFire != 0)
            RapidFire_Update ();
    }

    // Voice Recognition
    #ifdef DOS
        Inputs_Update_VoiceRecognition ();
    #endif
}

//-----------------------------------------------------------------------------
// Read and update all inputs sources
//-----------------------------------------------------------------------------
void        Inputs_Sources_Update (void)
{
    int     i, j;
    int     mouse_mx, mouse_my;

#ifdef MEKA_JOY
    int     Joy_Polled = NO;
#endif

    // Poll mouse
    poll_mouse ();
    get_mouse_mickeys (&mouse_mx, &mouse_my);
    Inputs.MouseMickeys_X = mouse_mx;
    Inputs.MouseMickeys_Y = mouse_my;

    // Key pressed
    // FIXME: note that only a SINGLE keypressed is stored per frame
    // This is completely evil, dirty, bad.
    if (keypressed ())
    {
        Inputs.KeyPressed.ascii = ureadkey (&Inputs.KeyPressed.scancode);
    }
    else
    {
        Inputs.KeyPressed.scancode = 0;
        Inputs.KeyPressed.ascii = 0;
    }

    for (i = 0; i < Inputs.Sources_Max; i++)
    {
        t_input_src *Src = Inputs.Sources[i];
        if (Src->Enabled == NO || Src->Connected_and_Ready == NO)
            continue;
        switch (Src->Type)
        {
            // Keyboard -------------------------------------------------------------
        case INPUT_SRC_KEYBOARD:
            {
                for (j = 0; j < INPUT_MAP_MAX; j++)
                {
                    t_input_map *map = &Src->Map[j];
                    int old_res = map->Res;
                    map->Res = (map->Idx != -1 && key [map->Idx]);
                    if (old_res && map->Res)
                        Src->Map_Counters[j]++;
                    else
                        Src->Map_Counters[j] = 0;
                }
                break;
            }
#ifdef MEKA_JOY
            // Digital Joypad/Joystick ----------------------------------------------
        case INPUT_SRC_JOYPAD:
            {
                JOYSTICK_INFO *joystick = &joy[Src->Connection_Port];
                if (!Joy_Polled) 
                { 
                    poll_joystick(); 
                    Joy_Polled = YES; 
                }

#ifdef DEBUG_JOY
                {
                    int i, j;
                    char buf[512];
                    Msg (MSGT_DEBUG, "Joystick %d", Src->Connection_Port);
                    for (i = 0; i < joystick->num_sticks; i++)
                    {
                        JOYSTICK_STICK_INFO *stick = &joystick->stick[i];
                        Msg (MSGT_DEBUG, "- Stick %d (flags = %04x)\n", i, stick->flags);
                        for (j = 0; j < stick->num_axis; j++)
                        {
                            JOYSTICK_AXIS_INFO *axis = &stick->axis[j];
                            Msg (MSGT_DEBUG, "   - Axis %d (pos = %d, d1 = %d, d2 = %d)\n", j, axis->pos, axis->d1, axis->d2);
                        }
                    }
                    strcpy(buf, "- Buttons ");
                    for (i = 0; i < joystick->num_buttons; i++)
                        sprintf(buf + strlen(buf), "%d ", joystick->button[i].b);
                    Msg (MSGT_DEBUG, buf);
                }
#endif

                for (j = 0; j < INPUT_MAP_MAX; j++)
                {
                    t_input_map *map = &Src->Map[j];
                    int old_res = map->Res;
                    switch (map->Type)
                    {
                    case INPUT_MAP_TYPE_JOY_AXIS:
                        {
                            JOYSTICK_AXIS_INFO *axis = &joystick->stick [INPUT_MAP_GET_STICK (map->Idx)].axis [INPUT_MAP_GET_AXIS (map->Idx)];
                            map->Res = (INPUT_MAP_GET_DIR_LR (map->Idx) ? axis->d2 : axis->d1);
                            break;
                        }
                        // FIXME: to do.. support analogue axis
                        // case INPUT_MAP_TYPE_JOY_AXIS_ANAL:
                    case INPUT_MAP_TYPE_JOY_BUTTON:
                        {
                            map->Res = (map->Idx != -1 && joystick->button [map->Idx].b);
                            break;
                        }
                    }
                    if (old_res && map->Res)
                        Src->Map_Counters[j]++;
                    else
                        Src->Map_Counters[j] = 0;
                }
                break;
            }
#endif // #ifdef MEKA_JOY
            // Mouse ----------------------------------------------------------------
        case INPUT_SRC_MOUSE:
            {
                int x, y;
                bool disable_mouse_button = FALSE;

                if (Meka_State == MEKA_STATE_FULLSCREEN)
                {
                    Src->Map[INPUT_MAP_ANALOG_AXIS_X].Res = mouse_x;
                    Src->Map[INPUT_MAP_ANALOG_AXIS_Y].Res = mouse_y;
                }
                else
                {
                    // Compute distance to first GUI game box
                    // FIXME: this sucks
                    x = mouse_x - gui.box [apps.id_game]->frame.pos.x;
                    y = mouse_y - gui.box [apps.id_game]->frame.pos.y;
                    if (x < 0 || y < 0 || x >= gui.box[apps.id_game]->frame.size.x || y >= gui.box[apps.id_game]->frame.size.y)
                        disable_mouse_button = TRUE;
                    if (x < 0) x = 0; 
                    // if (x > 255) x = 255;
                    if (y < 0) y = 0;
                    Src->Map[INPUT_MAP_ANALOG_AXIS_X].Res = x;
                    Src->Map[INPUT_MAP_ANALOG_AXIS_Y].Res = y;
                }

                x = Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res;
                if (x > 0 && mouse_mx < x)
                { if (mouse_mx < 0) x = 0; else x *= Src->Analog_to_Digital_FallOff; }
                else if (x < 0 && mouse_mx > x)
                { if (mouse_mx > 0) x = 0; else x *= Src->Analog_to_Digital_FallOff; }
                else
                    x = mouse_mx;
                Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res = x;

                y = Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res;
                if (y > 0 && mouse_my < y)
                { if (mouse_my < 0) y = 0; else y *= Src->Analog_to_Digital_FallOff; }
                else if (y < 0 && mouse_my > y)
                { if (mouse_my > 0) y = 0; else y *= Src->Analog_to_Digital_FallOff; }
                else
                    y = mouse_my;
                Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res = y;

                // No counters for analog data
                Src->Map_Counters[INPUT_MAP_ANALOG_AXIS_X] = 0;
                Src->Map_Counters[INPUT_MAP_ANALOG_AXIS_Y] = 0;
                Src->Map_Counters[INPUT_MAP_ANALOG_AXIS_X_REL] = 0;
                Src->Map_Counters[INPUT_MAP_ANALOG_AXIS_Y_REL] = 0;

                // Buttons ---------------------------------------------------------
                for (j = 4; j < INPUT_MAP_MAX; j++)
                {
                    t_input_map *map = &Src->Map[j];
                    int old_res = map->Res;
                    const int button_mask = (1 << Src->Map[j].Idx);
                    if (disable_mouse_button)
                        Src->Map[j].Res = 0;
                    else
                        Src->Map[j].Res = (Src->Map[j].Idx != -1 && (mouse_b & button_mask));
                    if (old_res && map->Res)
                        Src->Map_Counters[j]++;
                    else
                        Src->Map_Counters[j] = 0;
                }
                break;
            }
            //-----------------------------------------------------------------------
        }
    }
}

//-----------------------------------------------------------------------------
// Inputs_FixUp_JoypadOppositesDirections()
// Fix up/down & left/right cases
//-----------------------------------------------------------------------------
static void    Inputs_FixUp_JoypadOppositesDirections (void)
{
    u16        joy = tsms.Control[7];
    if (!(joy & (0x0001 | 0x0002))) { joy |= (0x0001 | 0x0002); } // P1 Up & Down
    if (!(joy & (0x0004 | 0x0008))) { joy |= (0x0004 | 0x0008); } // P1 Left & Right
    if (!(joy & (0x0040 | 0x0080))) { joy |= (0x0040 | 0x0080); } // P2 Up & Down
    if (!(joy & (0x0100 | 0x0200))) { joy |= (0x0100 | 0x0200); } // P2 Left & Right
    tsms.Control[7] = joy;
}

//-----------------------------------------------------------------------------
// Inputs_Update_VoiceRecognition ()
// Update fake voice recognition system :)
//-----------------------------------------------------------------------------
// Note: MS-DOS only. I'd like to 'port' it to other systems but I need
// to find a sound input value.
//-----------------------------------------------------------------------------
#ifdef DOS
void    Inputs_Update_VoiceRecognition (void)
{
    if (!apps.active.Voice_Rec)
        return;

    // Get value from SB
    // Note: I forgot what kind of value it was :) ... I think, some sound input level...
    outp (0x22C, 0x20);
    apps.opt.Voice.Value = inp (0x22A);
    if (apps.opt.Voice.Value < 128)
        apps.opt.Voice.Value = 256 - apps.opt.Voice.Value;
    apps.opt.Voice.Value -= 128;

    switch (Random(3))
    {
        // Simulate directionnal presses
    case 0:
    case 1: if (!apps.opt.Voice.Delay)
            {
                if (apps.opt.Voice.Value > 80)
                {
                    apps.opt.Voice.Dir = Random(4);
                    apps.opt.Voice.Delay = 10 + Random(40);
                    tsms.Control[7] &= ~(1 << apps.opt.Voice.Dir);
                }
            }
            else
            {
                apps.opt.Voice.Delay --;
                tsms.Control[7] &= ~(1 << apps.opt.Voice.Dir);
            }
            break;
            // Simulate button presses
    case 2: if (apps.opt.Voice.Value > 80)
                tsms.Control[7] &= ~(1 << (Random(2) + 4));
        break;
    }
}
#endif // DOS

//-----------------------------------------------------------------------------

