//-----------------------------------------------------------------------------
// MEKA - inputs_u.c
// Inputs Update - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_game.h"
#include "inputs_t.h"
#include "lightgun.h"
#include "rapidfir.h"
#include "sk1100.h"
#include "sportpad.h"
#include "tvoekaki.h"
#include "video.h"
#include "vdp.h"

// #define DEBUG_JOY

bool					g_keyboard_state[ALLEGRO_KEY_MAX];
int						g_keyboard_modifiers = 0;
ALLEGRO_EVENT_QUEUE *	g_keyboard_event_queue = NULL;
ALLEGRO_MOUSE_STATE		g_mouse_state;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void Inputs_FixUpJoypadOppositesDirections(void);
static void	Inputs_UpdateMouseRange(void);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void        Inputs_Sources_Init (void)
{
    Inputs.Sources = NULL;
    Inputs.Sources_Max = 0;

    Inputs.Peripheral [0] = INPUT_JOYPAD;
    Inputs.Peripheral [1] = INPUT_JOYPAD;

	memset(&g_keyboard_state, 0, sizeof(g_keyboard_state));
	memset(&g_mouse_state, 0, sizeof(g_mouse_state));
	g_keyboard_event_queue = al_create_event_queue();
	al_register_event_source(g_keyboard_event_queue, al_get_keyboard_event_source());
}

t_input_src *       Inputs_Sources_Add (char *name)
{
	t_input_src* Src = (t_input_src*)malloc (sizeof (t_input_src));

    Src->name                      = name;
    Src->type                      = INPUT_SRC_TYPE_KEYBOARD;
    Src->enabled                   = TRUE;
    Src->flags                     = INPUT_SRC_FLAGS_DIGITAL;
    Src->player                    = PLAYER_1;
    Src->Connection_Port           = 0;
    Src->Analog_to_Digital_FallOff = 0.8f;
    Src->Connected_and_Ready       = FALSE; // by default. need to be flagged for use

    for (int i = 0; i < INPUT_MAP_MAX; i++)
    {
        Src->Map[i].Type = 0; // key,button,..
        Src->Map[i].Idx = -1;
        Src->Map[i].Res = 0;
        Src->Map_Counters[i] = 0;
    }

	Inputs.Sources = (t_input_src**)realloc(Inputs.Sources, (Inputs.Sources_Max + 1) * sizeof (t_input_src *));
    Inputs.Sources [Inputs.Sources_Max] = Src;
    Inputs.Sources_Max ++;

    return (Src);
}

void       Inputs_Sources_Close (void)
{
    for (int i = 0; i < Inputs.Sources_Max; i++)
    {
        t_input_src *input_src = Inputs.Sources[i];
        free(input_src->name);
        free(input_src);
    }
    free(Inputs.Sources);
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
    // Control[7] is the following:
    // LG2.LG1.Unused.Reset.P2B.P2A.P2R.P2L - P2D.P2U.P1B.P1A.P1R.P1L.P1D.P1U
    // Now setting all bits (active logic)
    u16* c = &tsms.Control[7];
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
    if (g_env.state == MEKA_STATE_GUI && !(g_machine_flags & MACHINE_PAUSED))
	{
        if (gui.boxes_z_ordered[0] && (gui.boxes_z_ordered[0]->flags & GUI_BOX_FLAGS_FOCUS_INPUTS_EXCLUSIVE) != 0)
        {
            // Returning from the emulation inputs update requires to take care of a few variables...
            if (tsms.Control_Start_Pause == 1) // Leave it if it is == 2
                tsms.Control_Start_Pause = 0;
            if (cur_drv->id == DRV_GG)
                tsms.Control_GG |= (0x80);
            if (Inputs.SK1100_Enabled)
                SK1100_Clear();
            return;
        }
	}

    bool pause_pressed = false;
	bool reset_pressed = false;

    // Convert input sources data to emulation inputs data
    const int players = ((cur_drv->id == DRV_GG) ? 1 : 2); // 1 player on GG, else 2
    for (int i = 0; i < players; i++)
	{
        for (int source_index = 0; source_index < Inputs.Sources_Max; source_index++)
        {
            const t_input_src *src = Inputs.Sources[source_index];
            if (src->enabled == FALSE || src->player != i)
                continue;

            // If current peripheral is digital, skip analog only inputs sources
            // If current peripheral is analog, skip digital only inputs sources
            // k = Inputs_Peripheral_Infos [Inputs.Peripheral [i]].result_type;
            // if (!(Src->Result_Type & (k | (k << 1))))
            //    continue;
            // Process peripheral dependant stuff
            switch (Inputs.Peripheral [i])
            {
            case INPUT_JOYPAD: //---------------------------- Joypad/Control Stick
                if (src->flags & INPUT_SRC_FLAGS_DIGITAL)
                {
					//Msg(MSGT_DEBUG, "Player %d Src %d LEFT=%d", i, source_index, src->Map[INPUT_MAP_DIGITAL_LEFT].Res);
                    if (src->Map[INPUT_MAP_DIGITAL_UP].Res)     *c &= (!i? ~0x0001 : ~0x0040);
                    if (src->Map[INPUT_MAP_DIGITAL_DOWN].Res)   *c &= (!i? ~0x0002 : ~0x0080);
                    if (src->Map[INPUT_MAP_DIGITAL_LEFT].Res)   *c &= (!i? ~0x0004 : ~0x0100);
                    if (src->Map[INPUT_MAP_DIGITAL_RIGHT].Res)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (src->Map[INPUT_MAP_BUTTON1].Res)        *c &= (!i? ~0x0010 : ~0x0400);
                    if (src->Map[INPUT_MAP_BUTTON2].Res)        *c &= (!i? ~0x0020 : ~0x0800);
                }
                else if (src->flags & INPUT_SRC_FLAGS_EMULATE_DIGITAL) // ANALOG
                {
                    if (src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res < 0)  *c &= (!i? ~0x0001 : ~0x0040);
                    if (src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res > 0)  *c &= (!i? ~0x0002 : ~0x0080);
                    if (src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res < 0)  *c &= (!i? ~0x0004 : ~0x0100);
                    if (src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res > 0)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (src->Map[INPUT_MAP_BUTTON1].Res)                *c &= (!i? ~0x0010 : ~0x0400);
                    if (src->Map[INPUT_MAP_BUTTON2].Res)                *c &= (!i? ~0x0020 : ~0x0800);
                }
                break;
            case INPUT_LIGHTPHASER: //------------------------------- Light Phaser
                if (src->flags & INPUT_SRC_FLAGS_ANALOG)
                    LightPhaser_Update(i, src->Map[INPUT_MAP_ANALOG_AXIS_X].Res, src->Map[INPUT_MAP_ANALOG_AXIS_Y].Res);
                if (src->Map[INPUT_MAP_BUTTON1].Res)           *c &= (!i? ~0x0010 : ~0x0400);
                if (src->Map[INPUT_MAP_BUTTON2].Res)           *c &= (!i? ~0x0020 : ~0x0800);
                break;
            case INPUT_PADDLECONTROL: //------------------------ Paddle Controller
                {
                    int x = Inputs.Paddle_X[i];
                    int dx = 0;
                    if (src->flags & INPUT_SRC_FLAGS_ANALOG)
                    {
                        // Using analogic relative movement
                        dx = src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res;
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
                        //Msg (MSGT_DEBUG, "Map_Counters[DIGITAL_LEFT] = %d, [DIGITAL_RIGHT] = %d", src->Map_Counters[INPUT_MAP_DIGITAL_LEFT], src->Map_Counters[INPUT_MAP_DIGITAL_RIGHT]);
                        if (src->Map_Counters[INPUT_MAP_DIGITAL_LEFT] && !src->Map_Counters[INPUT_MAP_DIGITAL_RIGHT])
                        {
                            dx = src->Map_Counters[INPUT_MAP_DIGITAL_LEFT];
                            dx = -dx_table[(dx > 5) ? 5 : dx];
                        }
                        if (src->Map_Counters[INPUT_MAP_DIGITAL_RIGHT] && !src->Map_Counters[INPUT_MAP_DIGITAL_LEFT])
                        {
                            dx = src->Map_Counters[INPUT_MAP_DIGITAL_RIGHT];
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
                    if (src->Map[INPUT_MAP_BUTTON1].Res)           *c &= (!i? ~0x0010 : ~0x0400);
                    break;
                }
            case INPUT_SPORTSPAD: //--------------------------------- Sports Pads
                if (src->flags & INPUT_SRC_FLAGS_ANALOG)
                    SportsPad_Update (i, src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res, src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res);
                if (src->Map[INPUT_MAP_BUTTON1].Res)           *c &= (!i? ~0x0010 : ~0x0400);
                if (src->Map[INPUT_MAP_BUTTON2].Res)           *c &= (!i? ~0x0020 : ~0x0800);
                break;
            case INPUT_TVOEKAKI: //--------------------------------- Terebi Oekaki
                if (src->flags & INPUT_SRC_FLAGS_ANALOG)
                {
                    // Create button field (this is due to old code legacy)
                    const int b_field = (src->Map[INPUT_MAP_BUTTON1].Res ? 1 : 0) | (src->Map[INPUT_MAP_BUTTON2].Res ? 2 : 0);
                    TVOekaki_Update (src->Map[INPUT_MAP_ANALOG_AXIS_X].Res, src->Map[INPUT_MAP_ANALOG_AXIS_Y].Res, b_field);
                }
                else // Support standard controller with digital inputs, because the
                {    // Terebi Oekaki is not connected to the controller port, it is free to use !
                    if (src->Map[INPUT_MAP_DIGITAL_UP].Res)     *c &= (!i? ~0x0001 : ~0x0040);
                    if (src->Map[INPUT_MAP_DIGITAL_DOWN].Res)   *c &= (!i? ~0x0002 : ~0x0080);
                    if (src->Map[INPUT_MAP_DIGITAL_LEFT].Res)   *c &= (!i? ~0x0004 : ~0x0100);
                    if (src->Map[INPUT_MAP_DIGITAL_RIGHT].Res)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (src->Map[INPUT_MAP_BUTTON1].Res)        *c &= (!i? ~0x0010 : ~0x0400);
                    if (src->Map[INPUT_MAP_BUTTON2].Res)        *c &= (!i? ~0x0020 : ~0x0800);
                }
                break;
            }
            // Process RESET and PAUSE/START buttons
            if (src->Map[INPUT_MAP_PAUSE_START].Res) { pause_pressed = TRUE; if (tsms.Control_Start_Pause == 0) tsms.Control_Start_Pause = 1; }
            if (src->Map[INPUT_MAP_RESET].Res)       { reset_pressed = TRUE; }
        }
	}

    // SK-1100 Keyboard update
    if (Inputs.SK1100_Enabled)
        SK1100_Update();

    // Handle reset and clear pause latch if necessary
    if (reset_pressed == TRUE)
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
                if (Inputs.SK1100_Enabled == FALSE)
                    Machine_Reset();
            }
        }
    }

    if (pause_pressed == FALSE)
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
    if (!g_Configuration.allow_opposite_directions)
        Inputs_FixUpJoypadOppositesDirections ();

    // Simulate Rapid Fire
    if (running)
    {
        if (RapidFire != 0)
            RapidFire_Update ();
    }
}

//-----------------------------------------------------------------------------
// Read and update all inputs sources
//-----------------------------------------------------------------------------
void        Inputs_Sources_Update (void)
{
	// Process keyboard events
	ALLEGRO_EVENT key_event;
	while (al_get_next_event(g_keyboard_event_queue, &key_event))
	{
		switch (key_event.type)
		{
		case ALLEGRO_EVENT_KEY_DOWN:
			g_keyboard_state[key_event.keyboard.keycode] = true;
			break;
		case ALLEGRO_EVENT_KEY_UP:
			g_keyboard_state[key_event.keyboard.keycode] = false;
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
			// Process 'character' keypresses
			// Those are transformed (given keyboard state & locale) into printable character
			// Equivalent to using ToUnicode() in the Win32 API.
			// Note: Allegro is handling repeat for us here.
			if (key_event.keyboard.unichar > 0 && (key_event.keyboard.unichar & ~0xFF) == 0)
			{
				//Msg(MSGT_DEBUG, "%i %04x", key_event.keyboard.keycode, key_event.keyboard.unichar);
				t_key_press* key_press = (t_key_press*)malloc(sizeof(*key_press));
				key_press->scancode = key_event.keyboard.keycode;
				key_press->ascii = key_event.keyboard.unichar & 0xFF;
				list_add_to_end(&Inputs.KeyPressedQueue, key_press);
			}
			break;
		}
	}
	
	// Allegro 5 doesn't receive PrintScreen under Windows because of the high-level API it is using.
#ifdef ARCH_WIN32
	g_keyboard_state[ALLEGRO_KEY_PRINTSCREEN] = (GetAsyncKeyState(VK_SNAPSHOT) != 0);
	// g_keyboard_state.__key_down__internal__[ALLEGRO_KEY_PRINTSCREEN/32] |= (1 << (ALLEGRO_KEY_PRINTSCREEN & 31));
#endif

	// Update keyboard modifiers flags
	g_keyboard_modifiers = 0;
	if (Inputs_KeyDown(ALLEGRO_KEY_LCTRL) || Inputs_KeyDown(ALLEGRO_KEY_RCTRL))
		g_keyboard_modifiers |= ALLEGRO_KEYMOD_CTRL;
	if (Inputs_KeyDown(ALLEGRO_KEY_ALT) || Inputs_KeyDown(ALLEGRO_KEY_ALTGR))
		g_keyboard_modifiers |= ALLEGRO_KEYMOD_ALT;
	if (Inputs_KeyDown(ALLEGRO_KEY_LSHIFT) || Inputs_KeyDown(ALLEGRO_KEY_RSHIFT))
		g_keyboard_modifiers |= ALLEGRO_KEYMOD_SHIFT;

	// Keyboard debugging
#if 0
	u8 win32_keyboard_state[256];
	memset(&win32_keyboard_state[0], 0, sizeof(win32_keyboard_state));
	bool ret = GetKeyboardState(&win32_keyboard_state[0]);
	for (int i = 0; i != 256; i++)
		if (win32_keyboard_state[i])
			Msg(MSGT_DEBUG, "[%d Win32 pressed %d\n", ret, i);
		//if (GetAsyncKeyState(i))
		//	Msg( MSGT_DEBUG, "[%d] Win32 Pressed key %d, %08x", ret, i, GetAsyncKeyState(i));

	//for (int i = 0; i != ALLEGRO_KEY_MAX; i++)
	//	if (al_key_down(&g_keyboard_state, i))
	//		Msg( MSGT_DEBUG, "Pressed key %d", i);
#endif

    // Poll mouse
	const int mouse_x_prev = g_mouse_state.x;
	const int mouse_y_prev = g_mouse_state.y;
	al_get_mouse_state(&g_mouse_state);
	Inputs_UpdateMouseRange();

	// FIXME-ALLEGRO5: Used to be provided by Allegro 4 as mouse_mx, mouse_my (mickeys?) - check SVN log
	const int mouse_mx = g_mouse_state.x - mouse_x_prev;
	const int mouse_my = g_mouse_state.y - mouse_y_prev;

    for (int i = 0; i < Inputs.Sources_Max; i++)
    {
        t_input_src *Src = Inputs.Sources[i];
        if (!Src->enabled || Src->Connected_and_Ready == FALSE)
            continue;
        switch (Src->type)
        {
            // Keyboard -------------------------------------------------------------
        case INPUT_SRC_TYPE_KEYBOARD:
            {
                for (int j = 0; j != INPUT_MAP_MAX; j++)
                {
                    t_input_map *map = &Src->Map[j];
                    const int old_res = map->Res;
                    map->Res = (map->Idx != -1 && Inputs_KeyDown(map->Idx));
                    if (old_res && map->Res)
					{
                        Src->Map_Counters[j]++;
						//Msg(MSGT_DEBUG, "Map %d on", j);
					}
                    else
					{
                        Src->Map_Counters[j] = 0;
					}
                }
                break;
            }
#ifdef MEKA_JOY
            // Digital Joypad/Joystick ----------------------------------------------
        case INPUT_SRC_TYPE_JOYPAD:
            {
       			ALLEGRO_JOYSTICK *joystick = al_get_joystick(Src->Connection_Port);
				ALLEGRO_JOYSTICK_STATE state;
				al_get_joystick_state(joystick, &state);

				const int num_sticks = al_get_joystick_num_sticks(joystick);
				const int num_buttons = al_get_joystick_num_buttons(joystick);

#ifdef DEBUG_JOY
                {
                    Msg (MSGT_DEBUG, "Joystick %d", Src->Connection_Port);
                    for (int i = 0; i < num_sticks; i++)
                    {
                        const int num_axes = al_get_joystick_num_axes(joystick, i);
						Msg (MSGT_DEBUG, "- Stick %d\n", i);
                        for (int j = 0; j < num_axes; j++)
                        {
							Msg (MSGT_DEBUG, "   - Axis %d (pos = %f)\n", j, state.stick[i].axis[j]);
                        }
                    }

                    char buf[512];
                    strcpy(buf, "- Buttons ");
                    for (int i = 0; i < num_buttons; i++)
						sprintf(buf + strlen(buf), "%d ", state.button[i]);
                    Msg (MSGT_DEBUG, buf);
                }
#endif

                for (int j = 0; j != INPUT_MAP_MAX; j++)
                {
                    t_input_map *map = &Src->Map[j];
                    int old_res = map->Res;
                    switch (map->Type)
                    {
                    case INPUT_MAP_TYPE_JOY_AXIS:
                        {
							const float axis = state.stick[INPUT_MAP_GET_STICK(map->Idx)].axis[INPUT_MAP_GET_AXIS(map->Idx)];
							map->Res = (INPUT_MAP_GET_DIR_LR(map->Idx) ? axis > INPUT_JOY_DEADZONE : axis < -INPUT_JOY_DEADZONE);
                            break;
                        }
                    case INPUT_MAP_TYPE_JOY_BUTTON:
                        {
							map->Res = (map->Idx != (-1) && state.button[map->Idx]);
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
        case INPUT_SRC_TYPE_MOUSE:
            {
                bool disable_mouse_button = FALSE;

                if (g_env.state == MEKA_STATE_GAME)
                {
					int mx, my;
					Video_GameMode_ScreenPosToEmulatedPos(g_mouse_state.x, g_mouse_state.y, &mx, &my, true);
                    Src->Map[INPUT_MAP_ANALOG_AXIS_X].Res = mx;
					Src->Map[INPUT_MAP_ANALOG_AXIS_Y].Res = my;
                }
                else
                {
                    // Compute distance to first GUI game box
                    // FIXME: this sucks
                    int x = g_mouse_state.x - gamebox_instance->frame.pos.x;
                    int y = g_mouse_state.y - gamebox_instance->frame.pos.y;
                    if (x < 0 || y < 0 || x >= gamebox_instance->frame.size.x || y >= gamebox_instance->frame.size.y)
                        disable_mouse_button = TRUE;
                    if (x < 0) x = 0; 
                    // if (x > 255) x = 255;
                    if (y < 0) y = 0;
                    Src->Map[INPUT_MAP_ANALOG_AXIS_X].Res = x;
                    Src->Map[INPUT_MAP_ANALOG_AXIS_Y].Res = y;
                }

                int x = Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res;
                if (x > 0 && mouse_mx < x)
                { if (mouse_mx < 0) x = 0; else x *= Src->Analog_to_Digital_FallOff; }
                else if (x < 0 && mouse_mx > x)
                { if (mouse_mx > 0) x = 0; else x *= Src->Analog_to_Digital_FallOff; }
                else
                    x = mouse_mx;
                Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].Res = x;

                int y = Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].Res;
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
                for (int j = 4; j < INPUT_MAP_MAX; j++)
                {
                    t_input_map *map = &Src->Map[j];
                    int old_res = map->Res;
                    const int button_mask = (1 << Src->Map[j].Idx);
                    if (disable_mouse_button)
                        Src->Map[j].Res = 0;
                    else
						Src->Map[j].Res = (Src->Map[j].Idx != -1 && (g_mouse_state.buttons & button_mask));
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

void Inputs_UpdateMouseRange()
{
	if (g_env.state != MEKA_STATE_GAME)
		return;

	int sx_org;
	int sy_org;
	Video_GameMode_ScreenPosToEmulatedPos(g_mouse_state.x, g_mouse_state.y, &sx_org, &sy_org, false);
	int sx = sx_org;
	int sy = sy_org;

	if (Inputs.mouse_cursor == MEKA_MOUSE_CURSOR_LIGHT_PHASER || Inputs.mouse_cursor == MEKA_MOUSE_CURSOR_TV_OEKAKI)
	{ 
		sx = Clamp<int>(sx, (Mask_Left_8) ? 8 : 0, cur_drv->x_res);
		sy = Clamp<int>(sy, 0, cur_drv->y_res);
	}

	//Msg(MSGT_USER, "xy %d %d -> %d %d", sx_org, sy_org, sx, sy);

	if (sx != sx_org || sy != sy_org)
	{
		Video_GameMode_EmulatedPosToScreenPos(sx, sy, &g_mouse_state.x, &g_mouse_state.y, false);
		al_set_mouse_xy(g_display, g_mouse_state.x, g_mouse_state.y);
		//Msg(MSGT_USER, "%d", ret);
	}
}

// Fix up/down & left/right cases
static void    Inputs_FixUpJoypadOppositesDirections (void)
{
    u16        joy = tsms.Control[7];
    if (!(joy & (0x0001 | 0x0002))) { joy |= (0x0001 | 0x0002); } // P1 Up & Down
    if (!(joy & (0x0004 | 0x0008))) { joy |= (0x0004 | 0x0008); } // P1 Left & Right
    if (!(joy & (0x0040 | 0x0080))) { joy |= (0x0040 | 0x0080); } // P2 Up & Down
    if (!(joy & (0x0100 | 0x0200))) { joy |= (0x0100 | 0x0200); } // P2 Left & Right
    tsms.Control[7] = joy;
}

//-----------------------------------------------------------------------------

