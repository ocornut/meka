//
// Meka - MEKAINTF.C
// Interface to EMU2413
//

#include "shared.h"
#include "emu2413.h"
#include "mekaintf.h"
#include "sound/fmunit.h"

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

int     FM_Digital_Init()
{
    ConsolePrintf("%s ", Msg_Get(MSG_Sound_Init_YM2413_Digital));

	opll = OPLL_new(Z80_DEFAULT_CPU_CLOCK, Sound.SampleRate);
    if (opll == NULL)
    {
        ConsolePrintf("%s\n", Msg_Get(MSG_Failed));
        return (MEKA_ERR_FAIL);
    }
	// FIXME-NEWSOUND: FM init
	/*
    FM_Digital_saChannel = stream_init ("YM-2413 #0", g_sasound.audio_sample_rate, 16, 0, FM_Digital_Update);
    if (FM_Digital_saChannel == -1)
    {
        ConsolePrintf ("%s\n", Msg_Get(MSG_Failed));
        return (MEKA_ERR_FAIL); // FIXME: Error in channel creation
    }
    stream_set_volume (FM_Digital_saChannel, VOLUME_MAX);
	*/

    OPLL_reset(opll);

    ConsolePrintf("%s\n", Msg_Get(MSG_Ok));
    return (MEKA_ERR_OK);
}

void    FM_Digital_Close()
{
    OPLL_delete (opll);
}

// Active this engine as being the current FM interface to use
void    FM_Digital_Active()
{
    FM_Set_Interface(&FM_Digital_Interface, opll->reg);
}

// Reset emulated YM-2413
void    FM_Digital_Reset()
{
    OPLL_reset(opll);
}

// Port write to the YM-2413
void    FM_Digital_Write(int Register, int Value)
{
    OPLL_writeReg(opll, Register, Value);
}

void    FM_Digital_Mute()
{
}

void    FM_Digital_Resume()
{
}

// Regenerate various data from YM-2413 registers
// This is called after a state loading
void    FM_Digital_Regenerate()
{
    // Msg(MSGT_DEBUG, __FUNCTION__);
    for (int i = 0; i < YM2413_REGISTERS; i++)
    {
        FM_Digital_Write(i, FM_Regs[i]);
    }
    OPLL_forceRefresh(opll);
}

// Update audio stream
// This is periodically called by the sound engine
void    FM_Digital_WriteSamples(void *buffer, int length)
{
    // Msg(MSGT_USER, "FM_Digital_WriteSamples(%p, %d)", buffer, length);

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

