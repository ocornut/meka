//-----------------------------------------------------------------------------
// MEKA - inputs_t.c
// Inputs Tools - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "inputs_t.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

static int  typematic_repeating = 0;
static int  typematic_repeat_counter = 0;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void	Inputs_KeyClearAllState()
{
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
		g_keyboard_state[i] = -1.0f;
}

void    Inputs_KeyPressQueue_Remove(t_key_press *keypress)
{
    Inputs_KeyEat(keypress->scancode); // FIXME

    list_remove(&Inputs.KeyPressedQueue, keypress);
    free(keypress);
}

void    Inputs_KeyPressQueue_Clear(void)
{
    list_free(&Inputs.KeyPressedQueue);
}

bool	Inputs_KeyDown(int keycode)
{
	return g_keyboard_state[keycode] >= 0.0f;
	//return al_key_down(&g_keyboard_state, keycode);
}

// Eat given key by removing the corresponding flag in the global key[] table
void    Inputs_KeyEat(int keycode)
{
	g_keyboard_state[keycode] = -1.0f;
	//g_keyboard_state.__key_down__internal__[keycode / 32] &= ~(1 << (keycode & 31));
}

//-----------------------------------------------------------------------------
// Inputs_KeyPressed (int keycode, bool eat)
// Return whether given key was just pressed, then eat the key if asked for
//-----------------------------------------------------------------------------
bool    Inputs_KeyPressed(int keycode, bool eat)
{
    // Check if requested key was just pressed
    if (Inputs_KeyDown(keycode) && opt.Current_Key_Pressed == 0)
    {
        opt.Current_Key_Pressed = keycode;
        typematic_repeating = FALSE;
        typematic_repeat_counter = 0;
        if (eat)
			Inputs_KeyEat(keycode);
        return true;
    }
    // Check if previously pressed key was released
    // FIXME: shouldn't be done in this function, but rather in a single inputs-update
    if (opt.Current_Key_Pressed != 0 && Inputs_KeyDown(opt.Current_Key_Pressed) == false)
        opt.Current_Key_Pressed = 0;
    return false;
}

//-----------------------------------------------------------------------------
// Inputs_KeyPressed_Repeat (int keycode, bool eat, int delay, int rate)
// Return whether given key was pressed, handing repetition,
// then eat the key if asked for.
//-----------------------------------------------------------------------------
// FIXME: this function is broken since it relies on static global data. 
// Repeating two keys should mess the whole thing ?
//-----------------------------------------------------------------------------
bool    Inputs_KeyPressed_Repeat(int keycode, bool eat, int delay, int rate)
{
    // hmm...
    Inputs_KeyPressed(keycode, eat);
    if (opt.Current_Key_Pressed != keycode)
	{
		//Msg(MSGT_DEBUG, "%d != %d", opt.Current_Key_Pressed, keycode);
        return false;
	}

    // Increment counter
    typematic_repeat_counter++;

    // Delay
    if (typematic_repeating == FALSE)
    {
        // Return TRUE on first press
        if (typematic_repeat_counter == 1)
            return true;
        // Then wait for given delay
        if (typematic_repeat_counter == delay)
        {
            typematic_repeating = TRUE;
            typematic_repeat_counter = 0;
            return true;
        }
    }
    else
    {
        // Repeat
        if (typematic_repeat_counter == rate)
        {
            typematic_repeat_counter = 0;
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
