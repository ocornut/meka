//-----------------------------------------------------------------------------
// MEKA - inputs_u.c
// Inputs Update - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_game.h"
#include "inputs_t.h"
#include "lightgun.h"
#include "periph.h"
#include "rapidfir.h"
#include "sk1100.h"
#include "tvoekaki.h"
#include "video.h"
#include "vdp.h"

// #define DEBUG_JOY

float					g_keyboard_state[ALLEGRO_KEY_MAX];
int						g_keyboard_modifiers = 0;
ALLEGRO_EVENT_QUEUE *	g_keyboard_event_queue = NULL;
ALLEGRO_MOUSE_STATE		g_mouse_state;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void Inputs_FixUpJoypadOppositesDirections();
static void	Inputs_UpdateMouseRange();

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void	Inputs_Sources_Init()
{
    Inputs.Sources = NULL;
    Inputs.Sources_Max = 0;

    Inputs.Peripheral [0] = INPUT_JOYPAD;
    Inputs.Peripheral [1] = INPUT_JOYPAD;

	for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
		g_keyboard_state[i] = -1.0f;
	memset(&g_mouse_state, 0, sizeof(g_mouse_state));
	g_keyboard_event_queue = al_create_event_queue();
	al_register_event_source(g_keyboard_event_queue, al_get_keyboard_event_source());
}

t_input_src *       Inputs_Sources_Add(char *name)
{
	t_input_src* src = (t_input_src*)malloc (sizeof (t_input_src));

    src->name                      = name;
    src->type                      = INPUT_SRC_TYPE_KEYBOARD;
    src->enabled                   = TRUE;
    src->flags                     = INPUT_SRC_FLAGS_DIGITAL;
    src->player                    = PLAYER_1;
    src->Connection_Port           = 0;
    src->Analog_to_Digital_FallOff = 0.8f;
    src->Connected_and_Ready       = FALSE; // by default. need to be flagged for use

    for (int i = 0; i < INPUT_MAP_MAX; i++)
    {
		t_input_map_entry* map = &src->Map[i];
        map->type = INPUT_MAP_TYPE_KEY; // key,button,..
        map->hw_index = -1;
		map->hw_axis = 0;
		map->hw_direction = 0;
        map->current_value = 0;
        map->pressed_counter = 0;
    }

	Inputs.Sources = (t_input_src**)realloc(Inputs.Sources, (Inputs.Sources_Max + 1) * sizeof (t_input_src *));
    Inputs.Sources [Inputs.Sources_Max] = src;
    Inputs.Sources_Max ++;

    return (src);
}

void       Inputs_Sources_Close()
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

// Update emulation inputs, based on inputs sources data
// If 'running' is false, only update emulation state, but do not produce
// any side effect such as backspace doing an automatic HardReset in GG mode.
void	Inputs_Emulation_Update(bool running)
{
    // Control[7] is the following:
    // LG2.LG1.Unused.Reset.P2B.P2A.P2R.P2L - P2D.P2U.P1B.P1A.P1R.P1L.P1D.P1U
    // Now setting all bits (active logic)
    u16* c = &tsms.Control[7];
    *c |= 0x1FFF;

    // If we are in GUI mode, check if the focused box has the exclusive inputs flag. 
	// If it has it and the machine is running (not paused or being debugged), return.
    // The reason for doing that is that handling inputs priority is a mess with the 
    // current GUI system, because applet/widget updating code is mostly only run on 
	// emulation frame that are not skipped, while inputs for emulation should run for
	// all frames (including skipped ones).
	// So it is a bit complicated to handle a way for an applet to 'eat' a key, 
    // and I use an easy path.
    if (g_env.state == MEKA_STATE_GUI && !(g_machine_flags & MACHINE_PAUSED))
	{
        if (gui.boxes_z_ordered[0] && (gui.boxes_z_ordered[0]->flags & GUI_BOX_FLAGS_FOCUS_INPUTS_EXCLUSIVE) != 0)
        {
            // Returning from the emulation inputs update requires to take care of a few variables...
            if (tsms.Control_Start_Pause == 1) // Leave it if it is == 2
                tsms.Control_Start_Pause = 0;
            if (g_driver->id == DRV_GG)
                tsms.Control_GG |= (0x80);
            if (Inputs.SK1100_Enabled)
                SK1100_Clear();
            return;
        }
	}

    bool pause_pressed = false;
	bool reset_pressed = false;

    // Convert input sources data to emulation inputs data
    const int players = ((g_driver->id == DRV_GG) ? 1 : 2); // 1 player on GG, else 2
    for (int i = 0; i < players; i++)
	{
        for (int src_index = 0; src_index < Inputs.Sources_Max; src_index++)
        {
            const t_input_src* src = Inputs.Sources[src_index];
            if (src->enabled == FALSE || src->player != i)
                continue;

            // If current peripheral is digital, skip analog only inputs sources
            // If current peripheral is analog, skip digital only inputs sources
            // k = Inputs_Peripheral_Infos [Inputs.Peripheral [i]].result_type;
            // if (!(Src->Result_Type & (k | (k << 1))))
            //    continue;
            // Process peripheral dependent stuff
            switch (Inputs.Peripheral[i])
            {
            case INPUT_JOYPAD: //---------------------------- Joypad/Control Stick
                if (src->flags & INPUT_SRC_FLAGS_DIGITAL)
                {
					//Msg(MSGT_DEBUG, "Player %d Src %d LEFT=%d", i, source_index, src->Map[INPUT_MAP_DIGITAL_LEFT].Res);
                    if (src->Map[INPUT_MAP_DIGITAL_UP].current_value)     *c &= (!i? ~0x0001 : ~0x0040);
                    if (src->Map[INPUT_MAP_DIGITAL_DOWN].current_value)   *c &= (!i? ~0x0002 : ~0x0080);
                    if (src->Map[INPUT_MAP_DIGITAL_LEFT].current_value)   *c &= (!i? ~0x0004 : ~0x0100);
                    if (src->Map[INPUT_MAP_DIGITAL_RIGHT].current_value)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (src->Map[INPUT_MAP_BUTTON1].current_value)        *c &= (!i? ~0x0010 : ~0x0400);
                    if (src->Map[INPUT_MAP_BUTTON2].current_value)        *c &= (!i? ~0x0020 : ~0x0800);
                }
                else if (src->flags & INPUT_SRC_FLAGS_EMULATE_DIGITAL) // ANALOG
                {
                    if (src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].current_value < 0)  *c &= (!i? ~0x0001 : ~0x0040);
                    if (src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].current_value > 0)  *c &= (!i? ~0x0002 : ~0x0080);
                    if (src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].current_value < 0)  *c &= (!i? ~0x0004 : ~0x0100);
                    if (src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].current_value > 0)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (src->Map[INPUT_MAP_BUTTON1].current_value)                *c &= (!i? ~0x0010 : ~0x0400);
                    if (src->Map[INPUT_MAP_BUTTON2].current_value)                *c &= (!i? ~0x0020 : ~0x0800);
                }
                break;
            case INPUT_LIGHTPHASER: //------------------------------- Light Phaser
                if (src->flags & INPUT_SRC_FLAGS_ANALOG)
                    LightPhaser_Update(i, src->Map[INPUT_MAP_ANALOG_AXIS_X].current_value, src->Map[INPUT_MAP_ANALOG_AXIS_Y].current_value);
                if (src->Map[INPUT_MAP_BUTTON1].current_value)           *c &= (!i? ~0x0010 : ~0x0400);
                if (src->Map[INPUT_MAP_BUTTON2].current_value)           *c &= (!i? ~0x0020 : ~0x0800);
                break;
            case INPUT_PADDLECONTROL: //------------------------ Paddle Controller
                {
                    int x = Inputs.Paddle[i].x;
                    int dx = 0;
                    if (src->flags & INPUT_SRC_FLAGS_ANALOG)
                    {
                        // Using analog relative movement
                        dx = src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].current_value;
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
                        //Msg(MSGT_DEBUG, "Map[DIGITAL_LEFT].pressed_counter = %d, [DIGITAL_RIGHT] = %d", src->Map[INPUT_MAP_DIGITAL_LEFT].pressed_counter, src->Map[INPUT_MAP_DIGITAL_RIGHT].pressed_counter);
                        if (src->Map[INPUT_MAP_DIGITAL_LEFT].pressed_counter > 0 && src->Map[INPUT_MAP_DIGITAL_RIGHT].pressed_counter == 0)
                        {
                            dx = src->Map[INPUT_MAP_DIGITAL_LEFT].pressed_counter;
                            dx = -dx_table[(dx > 5) ? 5 : dx];
                        }
                        if (src->Map[INPUT_MAP_DIGITAL_RIGHT].pressed_counter > 0 && src->Map[INPUT_MAP_DIGITAL_LEFT].pressed_counter == 0)
                        {
                            dx = src->Map[INPUT_MAP_DIGITAL_RIGHT].pressed_counter;
                            dx = dx_table[(dx > 5) ? 5 : dx];
                        }
                    }
                    if (dx != 0)
                    {
                        x += dx;
                        if (x < 0) x = 0; else if (x > 255) x = 255;
                        Inputs.Paddle[i].x = x;
                    }
                    // Button 1 (only one button on Paddle Control)
                    if (src->Map[INPUT_MAP_BUTTON1].current_value)           *c &= (!i? ~0x0010 : ~0x0400);
                    break;
                }
            case INPUT_SPORTSPAD: //--------------------------------- Sports Pads
                if (src->flags & INPUT_SRC_FLAGS_ANALOG)
                    Peripherals_SportsPad_Update(i, src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].current_value, src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].current_value);
                if (src->Map[INPUT_MAP_BUTTON1].current_value)           *c &= (!i? ~0x0010 : ~0x0400);
                if (src->Map[INPUT_MAP_BUTTON2].current_value)           *c &= (!i? ~0x0020 : ~0x0800);
                break;
            case INPUT_GRAPHICBOARD: //----------------------------- Terebi Oekaki
                if (src->flags & INPUT_SRC_FLAGS_ANALOG)
                {
                    // Create button field (this is due to old code legacy)
                    const int b_field = (src->Map[INPUT_MAP_BUTTON1].current_value ? 1 : 0) | (src->Map[INPUT_MAP_BUTTON2].current_value ? 2 : 0);
                    TVOekaki_Update (src->Map[INPUT_MAP_ANALOG_AXIS_X].current_value, src->Map[INPUT_MAP_ANALOG_AXIS_Y].current_value, b_field);
                }
                else // Support standard controller with digital inputs, because the
                {    // Terebi Oekaki is not connected to the controller port, it is free to use !
                    if (src->Map[INPUT_MAP_DIGITAL_UP].current_value)     *c &= (!i? ~0x0001 : ~0x0040);
                    if (src->Map[INPUT_MAP_DIGITAL_DOWN].current_value)   *c &= (!i? ~0x0002 : ~0x0080);
                    if (src->Map[INPUT_MAP_DIGITAL_LEFT].current_value)   *c &= (!i? ~0x0004 : ~0x0100);
                    if (src->Map[INPUT_MAP_DIGITAL_RIGHT].current_value)  *c &= (!i? ~0x0008 : ~0x0200);
                    if (src->Map[INPUT_MAP_BUTTON1].current_value)        *c &= (!i? ~0x0010 : ~0x0400);
                    if (src->Map[INPUT_MAP_BUTTON2].current_value)        *c &= (!i? ~0x0020 : ~0x0800);
                }
				break;
			case INPUT_GRAPHICBOARD_V2:
                if (src->flags & INPUT_SRC_FLAGS_ANALOG)
				{
					const int buttons = (src->Map[INPUT_MAP_BUTTON1].current_value ? 2 : 0) | (src->Map[INPUT_MAP_BUTTON2].current_value ? 4 : 0) | (src->Map[INPUT_MAP_PAUSE_START].current_value ? 1 : 0);
					Peripherals_GraphicBoardV2_Update(i, src->Map[INPUT_MAP_ANALOG_AXIS_X].current_value, src->Map[INPUT_MAP_ANALOG_AXIS_Y].current_value, buttons);
				}
				break;
            }
            // Process RESET and PAUSE/START buttons
            if (src->Map[INPUT_MAP_PAUSE_START].current_value) { pause_pressed = TRUE; if (tsms.Control_Start_Pause == 0) tsms.Control_Start_Pause = 1; }
            if (src->Map[INPUT_MAP_RESET].current_value)       { reset_pressed = TRUE; }
        }
	}

    // SK-1100 Keyboard update
    if (Inputs.SK1100_Enabled)
        SK1100_Update();

    // Handle reset and clear pause latch if necessary
    if (reset_pressed == TRUE)
    {
        if (g_driver->id == DRV_SMS)
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
    if (g_driver->id == DRV_GG)
    {
        if (tsms.Control_Start_Pause == 1)
            tsms.Control_GG &= (~0x80);
        else
            tsms.Control_GG |= (0x80);
    }

    // Correct the cases where opposite directions are pressed
    if (!g_configuration.allow_opposite_directions)
        Inputs_FixUpJoypadOppositesDirections();

    // Simulate Rapid Fire
    if (running)
    {
        if (RapidFire != 0)
            RapidFire_Update();
    }
}

// Read and update all inputs sources
void	Inputs_Sources_Update()
{
	float dt = 1.0f/60.0f;		// FIXME: Delta time

	for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
		if (g_keyboard_state[i] >= 0.0f)
			g_keyboard_state[i] += dt;

	// Process keyboard events
	ALLEGRO_EVENT key_event;
	while (al_get_next_event(g_keyboard_event_queue, &key_event))
	{
		switch (key_event.type)
		{
		case ALLEGRO_EVENT_KEY_DOWN:
			//Msg(MSGT_DEBUG, "ALLEGRO_EVENT_KEY_DOWN %d", key_event.keyboard.keycode);
			if (g_keyboard_state[key_event.keyboard.keycode] < 0.0f)
				g_keyboard_state[key_event.keyboard.keycode] = 0.0f;
			break;
		case ALLEGRO_EVENT_KEY_UP:
			//Msg(MSGT_DEBUG, "ALLEGRO_EVENT_KEY_UP %d", key_event.keyboard.keycode);
			g_keyboard_state[key_event.keyboard.keycode] = -1.0f;
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
	if (GetAsyncKeyState(VK_SNAPSHOT))
		g_keyboard_state[ALLEGRO_KEY_PRINTSCREEN] = g_keyboard_state[ALLEGRO_KEY_PRINTSCREEN] < 0.0f ? 0.0f : g_keyboard_state[ALLEGRO_KEY_PRINTSCREEN] + dt;
	else
		g_keyboard_state[ALLEGRO_KEY_PRINTSCREEN] = -1.0f;
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
	al_get_mouse_state(&g_mouse_state);

	// FIXME-ALLEGRO5: Used to be provided by Allegro 4 as mouse_mx, mouse_my (mickeys?) - check SVN log
	//int screen_center_x, screen_center_y;
	//Video_GameMode_GetScreenCenterPos(&screen_center_x, &screen_center_y);
	//const int mouse_mx_unbounded = g_mouse_state.x - screen_center_x;
	//const int mouse_my_unbounded = g_mouse_state.y - screen_center_y;
	const int mouse_mx_unbounded = g_mouse_state.x - gui.mouse.x_prev;
	const int mouse_my_unbounded = g_mouse_state.y - gui.mouse.y_prev;

	// Recenter and clamp in range
	Inputs_UpdateMouseRange();

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
                    t_input_map_entry *map = &Src->Map[j];
                    const int old_res = map->current_value;
                    map->current_value = (map->hw_index != -1 && Inputs_KeyDown(map->hw_index));
                    if (old_res && map->current_value)
					{
                        Src->Map[j].pressed_counter++;
						//Msg(MSGT_DEBUG, "Map %d on", j);
					}
                    else
					{
                        Src->Map[j].pressed_counter = 0;
					}
                }
                break;
            }
#ifdef MEKA_JOYPAD
            // Digital Joypad/Joystick ----------------------------------------------
        case INPUT_SRC_TYPE_JOYPAD:
            {
       			ALLEGRO_JOYSTICK *joystick = al_get_joystick(Src->Connection_Port);
				ALLEGRO_JOYSTICK_STATE state;
				al_get_joystick_state(joystick, &state);

#ifdef DEBUG_JOY
                {
					const int num_sticks = al_get_joystick_num_sticks(joystick);
					const int num_buttons = al_get_joystick_num_buttons(joystick);
					
                    Msg(MSGT_DEBUG, "Joystick %d", Src->Connection_Port);
                    for (int i = 0; i < num_sticks; i++)
                    {
                        const int num_axes = al_get_joystick_num_axes(joystick, i);
						Msg(MSGT_DEBUG, "- Stick %d\n", i);
                        for (int j = 0; j < num_axes; j++)
                        {
							Msg(MSGT_DEBUG, "   - Axis %d (pos = %f)\n", j, state.stick[i].axis[j]);
                        }
                    }

                    char buf[512];
                    strcpy(buf, "- Buttons ");
                    for (int i = 0; i < num_buttons; i++)
						sprintf(buf + strlen(buf), "%d ", state.button[i]);
                    Msg(MSGT_DEBUG, buf);
                }
#endif

                for (int j = 0; j != INPUT_MAP_MAX; j++)
                {
                    t_input_map_entry *map = &Src->Map[j];
                    int old_res = map->current_value;
                    switch (map->type)
                    {
                    case INPUT_MAP_TYPE_JOY_AXIS:
                        {
							const float v = state.stick[map->hw_index].axis[map->hw_axis];
							map->current_value = (map->hw_direction ? v > INPUT_JOY_DEADZONE : v < -INPUT_JOY_DEADZONE);
                            break;
                        }
                    case INPUT_MAP_TYPE_JOY_BUTTON:
                        {
							map->current_value = (map->hw_index != (-1) && state.button[map->hw_index]);
                            break;
                        }
                    }
                    if (old_res && map->current_value)
                        Src->Map[j].pressed_counter++;
                    else
                        Src->Map[j].pressed_counter = 0;
                }
                break;
            }
#endif // #ifdef MEKA_JOYPAD
            // Mouse ----------------------------------------------------------------
        case INPUT_SRC_TYPE_MOUSE:
            {
                bool disable_mouse_button = false;

                if (g_env.state == MEKA_STATE_GAME)
                {
					int mx, my;
					Video_GameMode_ScreenPosToEmulatedPos(g_mouse_state.x, g_mouse_state.y, &mx, &my, true);
					//Msg(MSGT_USER, "%d %d", mx, my);
                    Src->Map[INPUT_MAP_ANALOG_AXIS_X].current_value = mx;
					Src->Map[INPUT_MAP_ANALOG_AXIS_Y].current_value = my;
                }
                else
                {
                    // Compute distance to first GUI game box
                    // FIXME: this sucks
                    int x = g_mouse_state.x - gamebox_instance->frame.pos.x;
                    int y = g_mouse_state.y - gamebox_instance->frame.pos.y;
                    if (x < 0 || y < 0 || x >= gamebox_instance->frame.size.x || y >= gamebox_instance->frame.size.y)
                        disable_mouse_button = true;
                    if (x < 0) x = 0; 
                    // if (x > 255) x = 255;
                    if (y < 0) y = 0;

					const float sx = (float)g_driver->x_res / (gamebox_instance->frame.size.x + 1);
					const float sy = (float)g_driver->y_res / (gamebox_instance->frame.size.y + 1);
					x *= sx;
					y *= sy;
				    if ((g_driver->id == DRV_SMS) && (Mask_Left_8))
						x += 4;

                    Src->Map[INPUT_MAP_ANALOG_AXIS_X].current_value = x;
                    Src->Map[INPUT_MAP_ANALOG_AXIS_Y].current_value = y;
                }

                int x = Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].current_value;
				//Msg(MSGT_DEBUG, "x = %d, %d - %d", x, g_mouse_state.x,gui.mouse.x_prev);
				const int mx = mouse_mx_unbounded;
                if (x > 0 && mx < x)
                { if (mx < 0) x = 0; else x *= Src->Analog_to_Digital_FallOff; }
                else if (x < 0 && mx > x)
                { if (mx > 0) x = 0; else x *= Src->Analog_to_Digital_FallOff; }
                else
				{ x = mx; }
                Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].current_value = x;

                int y = Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].current_value;
                const int my = mouse_my_unbounded;
				if (y > 0 && my < y)
                { if (my < 0) y = 0; else y *= Src->Analog_to_Digital_FallOff; }
                else if (y < 0 && my > y)
                { if (my > 0) y = 0; else y *= Src->Analog_to_Digital_FallOff; }
                else
				{ y = my; }
                Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].current_value = y;

                // No counters for analog data
                Src->Map[INPUT_MAP_ANALOG_AXIS_X].pressed_counter = 0;
                Src->Map[INPUT_MAP_ANALOG_AXIS_Y].pressed_counter = 0;
                Src->Map[INPUT_MAP_ANALOG_AXIS_X_REL].pressed_counter = 0;
                Src->Map[INPUT_MAP_ANALOG_AXIS_Y_REL].pressed_counter = 0;

                // Buttons
                for (int j = 4; j < INPUT_MAP_MAX; j++)
                {
                    t_input_map_entry *map = &Src->Map[j];
                    const int old_res = map->current_value;
                    const int button_mask = (1 << Src->Map[j].hw_index);
                    if (disable_mouse_button)
                        Src->Map[j].current_value = 0;
                    else
						Src->Map[j].current_value = (Src->Map[j].hw_index != -1 && (g_mouse_state.buttons & button_mask));
                    if (old_res && map->current_value)
                        Src->Map[j].pressed_counter++;
                    else
                        Src->Map[j].pressed_counter = 0;
                }
                break;
            }
            //-----------------------------------------------------------------------
        }
    }
}

void Inputs_Sources_ClearOutOfFocus()
{
	for (size_t i = 0; i < ALLEGRO_KEY_MAX; i++)
		g_keyboard_state[i] = -1.0f;
	g_keyboard_modifiers = 0;
}

void Inputs_UpdateMouseRange()
{
	if (g_env.state != MEKA_STATE_GAME)
		return;

	int sx_org;
	int sy_org;
	Video_GameMode_ScreenPosToEmulatedPos(g_mouse_state.x, g_mouse_state.y, &sx_org, &sy_org, false);

	if (Inputs.mouse_cursor == MEKA_MOUSE_CURSOR_LIGHT_PHASER || Inputs.mouse_cursor == MEKA_MOUSE_CURSOR_TV_OEKAKI)
	{ 
		const int sx = Clamp<int>(sx_org, (Mask_Left_8) ? 8 : 0, g_driver->x_res);
		const int sy = Clamp<int>(sy_org, 0, g_driver->y_res);
		if (sx != sx_org || sy != sy_org)
		{
			int mx, my;
			Video_GameMode_EmulatedPosToScreenPos(sx, sy, &mx, &my, false);
			al_set_mouse_xy(g_display, mx, my);
		}
	}
	else if (Inputs.Peripheral[PLAYER_1] == INPUT_SPORTSPAD || Inputs.Peripheral[PLAYER_2] == INPUT_SPORTSPAD)
	{
		// Recenter so we can keep moving
		int sx, sy;
		Video_GameMode_GetScreenCenterPos(&sx, &sy);
		al_set_mouse_xy(g_display, sx, sy);
	}

	//Msg(MSGT_USER, "xy %d %d -> %d %d", sx_org, sy_org, sx, sy);
}

// Fix up/down & left/right cases
static void    Inputs_FixUpJoypadOppositesDirections()
{
    u16        joy = tsms.Control[7];
    if (!(joy & (0x0001 | 0x0002))) { joy |= (0x0001 | 0x0002); } // P1 Up & Down
    if (!(joy & (0x0004 | 0x0008))) { joy |= (0x0004 | 0x0008); } // P1 Left & Right
    if (!(joy & (0x0040 | 0x0080))) { joy |= (0x0040 | 0x0080); } // P2 Up & Down
    if (!(joy & (0x0100 | 0x0200))) { joy |= (0x0100 | 0x0200); } // P2 Left & Right
    tsms.Control[7] = joy;
}

//-----------------------------------------------------------------------------

