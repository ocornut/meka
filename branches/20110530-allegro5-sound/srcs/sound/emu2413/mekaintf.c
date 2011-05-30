//
// Meka - MEKAINTF.C
// Interface to EMU2413
//

#include "shared.h"
#include "emu2413.h"

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

// Handler to OPLL EMU2413
OPLL *opll;

// saSound channel
int   FM_Digital_saChannel;

t_fm_unit_interface     FM_Digital_Interface =
{
    "YM-2413 Digital Emulator",
    "Mitsutaka Okazaki",
    FM_Digital_Reset,
    FM_Digital_Write,
    FM_Digital_Mute,
    FM_Digital_Resume,
    FM_Digital_Regenerate
};

//-----------------------------------------------------------------------------
// FM_Digital_Init ()
// Initialize emulation
//-----------------------------------------------------------------------------
int     FM_Digital_Init (void *userdata /* unused */)
{
    ConsolePrintf ("%s ", Msg_Get (MSG_Sound_Init_YM2413_Digital));

    opll = OPLL_new (Z80_DEFAULT_CPU_CLOCK, g_sasound.audio_sample_rate);
    if (opll == NULL)
    {
        ConsolePrintf ("%s\n", Msg_Get (MSG_Failed));
        return (MEKA_ERR_FAIL);
    }
    FM_Digital_saChannel = stream_init ("YM-2413 #0", g_sasound.audio_sample_rate, 16, 0, FM_Digital_Update);
    if (FM_Digital_saChannel == -1)
    {
        ConsolePrintf ("%s\n", Msg_Get (MSG_Failed));
        return (MEKA_ERR_FAIL); // FIXME: Error in channel creation
    }
    stream_set_volume (FM_Digital_saChannel, VOLUME_MAX);
    OPLL_reset (opll);

    ConsolePrintf ("%s\n", Msg_Get (MSG_Ok));
    return (MEKA_ERR_OK);
}

//-----------------------------------------------------------------------------
// FM_Digital_Close()
// Close emulation
//-----------------------------------------------------------------------------
void    FM_Digital_Close (void)
{
    OPLL_delete (opll);
}

//-----------------------------------------------------------------------------
// FM_Digital_Active ()
// Active this engine as being the current FM interface to use
//-----------------------------------------------------------------------------
void    FM_Digital_Active   (void)
{
    FM_Set_Interface (&FM_Digital_Interface, opll->reg);
}

//-----------------------------------------------------------------------------
// FM_Digital_Reset()
// Reset emulated YM-2413
//-----------------------------------------------------------------------------
void    FM_Digital_Reset (void)
{
    OPLL_reset (opll);
}

//-----------------------------------------------------------------------------
// FM_Digital_Write()
// Port write to the YM-2413
//-----------------------------------------------------------------------------
void    FM_Digital_Write (int Register, int Value)
{
    OPLL_writeReg (opll, Register, Value);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void    FM_Digital_Mute         (void)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void    FM_Digital_Resume       (void)
{
}

//-----------------------------------------------------------------------------
// FM_Digital_Regenerate()
// Regenerate various data from YM-2413 registers
// This is called after a state loading
//-----------------------------------------------------------------------------
void    FM_Digital_Regenerate (void)
{
    int    i;

    // Msg (MSGT_DEBUG, __FUNCTION__);
    for (i = 0; i < YM2413_REGISTERS; i++)
    {
        FM_Digital_Write (i, FM_Regs[i]);
    }
    OPLL_forceRefresh (opll);
}

//-----------------------------------------------------------------------------
// FM_Digital_Update()
// Update audio stream
// This is periodically called by the sound engine
//-----------------------------------------------------------------------------
void    FM_Digital_Update       (int chip, void *buffer, int length)
{
    // Msg (MSGT_USER, "FM_Digital_Update(%d, %p, %d)", chip, buffer, length);

    // printf("\n[%s]\n", __FUNCTION__);

	s16* buf = (s16*)buffer;
	while (length--)
	{
		int val = OPLL_calc(opll) * 2;
		if (val < -0x8000)
			*buf = -0x8000;
		else
			if (val > 0x7FFF)
				*buf = 0x7FFF;
			else
				*buf =  val;
		buf++;
		// printf("%d, ", buf[-1]);
		// printf("%d, ", val);
	}
}

