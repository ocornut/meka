//-----------------------------------------------------------------------------
// MEKA - inputs_i.c
// Inputs Initialization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "inputs_i.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

void    Inputs_Init_Mouse(void);
void    Inputs_Init_Joystick(void);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Inputs_Init (void)
{
    // Mouse
    Inputs.MouseSpeed_X = 2;
    Inputs.MouseSpeed_Y = 2;
    Inputs.MouseMickeys_X = 0;
    Inputs.MouseMickeys_Y = 0;

    // Keyboard
    Inputs.KeyPressedQueue = NULL;

    // Sources
    Inputs_Sources_Init ();

    // Peripheral
    Inputs.Paddle_X [PLAYER_1] = Inputs.Paddle_X [PLAYER_2] = 0;
    LightGun_Init ();
    SportPad_Init ();
    RapidFire_Init ();

    // Load Inputs Sources List
    Load_Inputs_Src_List ();

    // Update Mouse speed
    Inputs_Init_Mouse ();
}

void    Inputs_Init_Mouse (void)
{
    set_mouse_speed (Inputs.MouseSpeed_X, Inputs.MouseSpeed_Y);
}

#ifdef MEKA_JOY

void    Inputs_Joystick_Init(void)
{
    int    i;
    int    NumJoy;
    byte   Found = NO;

    if (Inputs.Sources_Joy_Driver == JOY_TYPE_NONE)
        return;
    for (i = 0; i < Inputs.Sources_Max; i++)
    {
        t_input_src *src = Inputs.Sources[i];
        if (src->Type == INPUT_SRC_JOYPAD)
        {
            Found = YES;
            break;
        }
    }
    if (Found == NO)
        return;

    // There is at least one joypad so we'll launch initialization
    ConsolePrint (Msg_Get (MSG_Inputs_Joy_Init));

    if (install_joystick (Inputs.Sources_Joy_Driver)
        || ((NumJoy = num_joysticks) == 0))
    {
        ConsolePrint (Msg_Get (MSG_Inputs_Joy_Init_None));
        return;
    }
    ConsolePrintf (Msg_Get (MSG_Inputs_Joy_Init_Found), NumJoy);

    for (i = 0; i < NumJoy; i++)
    {
        int first = NO;
        JOYSTICK_INFO *joystick = &joy[i];
        // Msg (MSGT_DEBUG, "joystick %d flags %04X", i, joystick->flags);
        while (joystick->flags & JOYFLAG_CALIB_DIGITAL)
        {
            char *msg = (char *)calibrate_joystick_name(i);
            if (first == NO)
            {
                ConsolePrintf (" - Calibrating joystick %d:\n", i);
                first = YES;
            }
            ConsolePrintf ("   - %s, and press a key\n", msg);
            readkey ();
            if (calibrate_joystick(i) != 0)
            {
                Quit_Msg (Msg_Get (MSG_Inputs_Joy_Calibrate_Error));
            }
        }
    }

    // Flag available devices "connected and ready"
    for (i = 0; i < Inputs.Sources_Max; i++)
    {
        t_input_src *src = Inputs.Sources[i];
        if (src->Type == INPUT_SRC_JOYPAD)
            if (src->Connection_Port < NumJoy)
                src->Connected_and_Ready = YES;
    }
}

void    Inputs_Joystick_Close(void)
{
    remove_joystick();
}

#endif // MEKA_JOY

//-----------------------------------------------------------------------------

