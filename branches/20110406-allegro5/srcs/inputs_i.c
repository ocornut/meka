//-----------------------------------------------------------------------------
// MEKA - inputs_i.c
// Inputs Initialization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "inputs_i.h"
#include "inputs_f.h"
#include "lightgun.h"
#include "sportpad.h"
#include "rapidfir.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Inputs_Init (void)
{
    // Mouse
    Inputs.MouseSpeed_X = 2;
    Inputs.MouseSpeed_Y = 2;

    // Keyboard
    Inputs.KeyPressedQueue = NULL;

    // Sources
    Inputs_Sources_Init ();

    // Peripheral
    Inputs.Paddle_X [PLAYER_1] = Inputs.Paddle_X [PLAYER_2] = 0;
    LightPhaser_Init ();
    SportsPad_Init ();
    RapidFire_Init ();

    // Load Inputs Sources List
    Load_Inputs_Src_List ();

    // Update Mouse speed
    Inputs_Init_Mouse ();
}

void    Inputs_Init_Mouse (void)
{
	// FIXME-ALLEGRO5: Feature missing?
    //set_mouse_speed (Inputs.MouseSpeed_X, Inputs.MouseSpeed_Y);
}

#ifdef MEKA_JOY

void    Inputs_Joystick_Init(void)
{
    int     i;
    int     num_joy;
    bool    found = FALSE;

    for (i = 0; i < Inputs.Sources_Max; i++)
    {
        t_input_src *src = Inputs.Sources[i];
        if (src->type == INPUT_SRC_TYPE_JOYPAD)
        {
            found = TRUE;
            break;
        }
    }
    if (found == FALSE)
        return;

    // There is at least one joypad so we'll launch initialization
    ConsolePrint (Msg_Get (MSG_Inputs_Joy_Init));

    if (!al_install_joystick() || ((num_joy = al_get_num_joysticks()) == 0))
    {
        ConsolePrint (Msg_Get (MSG_Inputs_Joy_Init_None));
		ConsolePrint ("\n");
        return;
    }
    ConsolePrintf (Msg_Get (MSG_Inputs_Joy_Init_Found), num_joy);
    ConsolePrint ("\n");

    // Flag available devices "connected and ready"
    for (i = 0; i < Inputs.Sources_Max; i++)
    {
        t_input_src *src = Inputs.Sources[i];
        if (src->type == INPUT_SRC_TYPE_JOYPAD)
            if (src->Connection_Port < num_joy)
                src->Connected_and_Ready = TRUE;
    }
}

void    Inputs_Joystick_Close(void)
{
    al_uninstall_joystick();
}

#endif // MEKA_JOY

//-----------------------------------------------------------------------------

