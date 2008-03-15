//-----------------------------------------------------------------------------
// MEKA - s_opl.c
// OPL Access - Code
//-----------------------------------------------------------------------------

#include "shared.h"

#ifdef MEKA_OPL
//#error "OPL Access file included/compiled without MEKA_OPL defined!"
//#endif

//-----------------------------------------------------------------------------
// Quick OPL reference
//-----------------------------------------------------------------------------
//
//  Read:  Status       abc.....
//                      a: either timer expired
//                      b: timer 1 expired
//                      c: timer 2 expired
//
//  Write: Base+0       Register
//  Write: Base+1       Data
//
//  Register Map:
//      01 : Test LSI / Enable waveform control
//      02 : Timer 1 data
//      03 : Timer 2 data
//      04 : Timer control flags
//      08 : Speech synthesis mode / Keyboard split note select
//  20..35 : Amp mod / Vibrato / EG type / Key scaling / Multiple
//  40..55 : Key scaling level / Operator output level
//  60..75 : Attack rate / Decay rate
//  80..95 : Sustain level / Release rate
//  A0..A8 : Frequency (low 8 bits)
//  B0..B8 : Key on / Octave / Frequency (high 2 bits)
//      BD : AM depth / Vibrato depth / Rhythm control
//  C0..C8 : Feedback strength / Connection type
//  E0..F5 : Wave select
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Setup Port access macros
//-----------------------------------------------------------------------------

#ifdef ARCH_WIN32
    #include <conio.h>
    #define inportb       _inp
    #define outportb      _outp
#endif

//-----------------------------------------------------------------------------
// Sound_OPL_Init_Config ()
// Find and parse BLASTER environnement variable if set
// Get SoundBlaster OPL base port. For some compatibles this must be 0x388.
//-----------------------------------------------------------------------------
void    Sound_OPL_Init_Config (void)
{
    char  *p;
    char  *blaster_env;
    int    base;

    Sound.OPL_Address = 0;
    if (!(blaster_env = getenv("BLASTER")))
    {
        ConsolePrintf ("%s\n", Msg_Get (MSG_Sound_Init_Error_Blaster));
        return;
    }

    // Look for 'A'ddress setting
    p = strchr (blaster_env, 'A');
    if (p == NULL)
    {
        ConsolePrintf ("%s\n", Msg_Get (MSG_Sound_Init_Error_Blaster_A));
        return;
    }

    // Read hex number in the string
    base = 0;
    for (p++; (*p >= '0' && *p <= '9'); p++)
    {
        base = (base * 16) + (*p - '0');
    }
    Sound.OPL_Address = base;

    // DelayReg = 4;   /* Delay after an OPL register write increase it to avoid problems ,but you'll lose speed */
    // DelayData = 7;  /* same as above but after an OPL data write this usually is greater than above */
}

//-----------------------------------------------------------------------------
// Sound_OPL_Read_Status ()
// Read OPL Status
//-----------------------------------------------------------------------------
INLINE int      Sound_OPL_Read_Status (void)
{
    return inportb (Sound.OPL_Address);
}

//-----------------------------------------------------------------------------
// Sound_OPL_Write (int R, int V)
// Write value V into OPL register R
//-----------------------------------------------------------------------------
void    Sound_OPL_Write (int R, int V)
{
    int   J;

    // printf("Sound_OPL_Write(%d, %d), OPL_Address=%X\n", R, V, Sound.OPL_Address);

    if (Sound.OPL_Address == 0)
        return;

    // Wait then setup Register
    for (J = 0; J < Sound.OPL_Speed; J++)
        inportb (Sound.OPL_Address);
    outportb (Sound.OPL_Address + 0, R & 0xff);

    // Wait then setup Value
    for (J = 0; J < Sound.OPL_Speed; J++)
        inportb (Sound.OPL_Address);
    outportb (Sound.OPL_Address + 1, V & 0xff);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sound_OPL_Init ()
// Attempt to detect and initialize OPL. Return 1 on success, 0 otherwise.
//-----------------------------------------------------------------------------
int     Sound_OPL_Init (void)
{
    int   I, A1, A2;

    ConsolePrint (" - OPL: "); // FIXME: should be a MsgGet()?

    // Sound.OPL_Address = 0x388;
    if (Sound.OPL_Address == 0)
    {
        ConsolePrint ("Unused\n");
        return (MEKA_ERR_FAIL);
    }

    // Detect if adlib is present
    Sound_OPL_Write (0x01, 0x00);         /* Delete test register    */
    Sound_OPL_Write (0x04, 0x60);         /* Mask and disable timers */
    Sound_OPL_Write (0x04, 0x80);         /* Reset timers flags      */
    A1 = Sound_OPL_Read_Status () & 0xE0; /* Read status             */
    Sound_OPL_Write (0x02, 0xFF);         /* Set Timer1 to 0xFF      */
    Sound_OPL_Write (0x04, 0x21);         /* Unmask and start Timer1 */
    for (I = 0; I < 0xC8; I++)            /* Wait 80us               */
        Sound_OPL_Read_Status ();
    A2 = Sound_OPL_Read_Status () & 0xE0; /* Read status             */
    Sound_OPL_Write (0x04, 0x60);         /* Mask and disable timers */
    Sound_OPL_Write (0x04, 0x80);         /* Reset timers flags      */
    if ((A2 != 0xC0) || A1)
    {
        ConsolePrint ("Failed\n");
        return (MEKA_ERR_FAIL);
    }

    // Successfull OPL initialization
    ConsolePrint ("Ok\n");
    return (MEKA_ERR_OK);
}

//-----------------------------------------------------------------------------
// Sound_OPL_Close ()
// Close OPL
//-----------------------------------------------------------------------------
void    Sound_OPL_Close (void)
{
    if (Sound.OPL_Address != 0)
    {
        Sound_OPL_Write (0xbd, 0x00);
        Sound_OPL_Write (0x04, 0x60);  /* Mask and disable timers */
        Sound_OPL_Write (0x04, 0x80);  /* Reset timers flags      */
    }
}

//-----------------------------------------------------------------------------

#endif // MEKA_OPL

//-----------------------------------------------------------------------------

