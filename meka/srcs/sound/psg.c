//
// Meka - PSG.H
// PSG Emulation, by Maxim
// Tweaked for MEKA
//

#include "shared.h"
#include "fskipper.h"
#include "psg.h"
#include <limits.h>

// #define DEBUG_PSG

/*
//---- Original Header ----

  SN76489 emulation
  by Maxim in 2001 and 2002
  converted from my original Delphi implementation

  I'm a C newbie so I'm sure there are loads of stupid things
  in here which I'll come back to some day and redo

  Includes:
  - Super-high quality tone channel "oversampling" by calculating fractional positions on transitions
  - Noise output pattern reverse engineered from actual SMS output
  - Volume levels taken from actual SMS output

*/

// Constants
#define NoiseInitialState           0x4000
#define NoiseWhiteFeedback_SMSGG    0x0009  // Bits 0 and 3
#define NoiseWhiteFeedback_SGSC     0x0006  // Bits 1 and 2

#define NoiseWhiteFeedback          NoiseWhiteFeedback_SMSGG

// These values are taken from a real SMS2's output
static const unsigned short int PSGVolumeValues[16] =
{
    892, 892, 892, 760, 623, 497, 404, 323,
    257, 198, 159, 123,  96,  75,  60,   0
};

// Variables
t_psg           PSG;
static int      Active = 0;     // Set to true by SN76489_Init(), if false then all procedures exit immediately

//------------------------------------------------------------------------------
// PSG_Init()
// Initialize audio system for PSG emulation
//-----------------------------------------------------------------------------
int         PSG_Init()
{
    ConsolePrintf ("%s ", Msg_Get(MSG_Sound_Init_SN76496));

	// FIXME-NEWSOUND: PSG
	/*
    PSG_saChannel = stream_init ("SN76496 #0", g_sasound.audio_sample_rate, 16, 0, PSG_Update);
    if (PSG_saChannel == -1)
    {
        ConsolePrintf ("%s\n", Msg_Get(MSG_Failed));
        return (MEKA_ERR_FAIL);
    }
    stream_set_volume (PSG_saChannel, VOLUME_MAX);
	*/

    for (int i = 0; i < 4; i++)               // FIXME: to be done in sound.c ?
        PSG.Channels[i].Active = TRUE;
	SN76489_Reset (Z80_DEFAULT_CPU_CLOCK, Sound.SampleRate);

    ConsolePrintf ("%s\n", Msg_Get(MSG_Ok));
    return (MEKA_ERR_OK);
}

//------------------------------------------------------------------------------
// PSG_Update()
// Update audio stream
// This is periodically called by the sound engine
//-----------------------------------------------------------------------------
void        PSG_WriteSamples(void *buffer, int length)
{
    s16* buf = (s16*)buffer;
    for (int length_left = length; length_left > 0; length_left--)
    {
        // Get 1 sample from emulator
        int left;
        SN76489_GetValues (&left, NULL);

        // Clamp write to buffer
        left *= 2;
        if (left < -0x8000)
            *buf = -0x8000;
        else
            if (left > 0x7FFF)
                *buf =  0x7FFF;
            else
                *buf =  left;
        buf++;
    }

    // Write buffer to file if logging is activated within MEKA
    if (Sound.LogWav)
    {
        fwrite (buffer, length, sizeof (short), Sound.LogWav);
        Sound.LogWav_SizeData += length * sizeof (short);
    }
}

//-----------------------------------------------------------------------------
// PSG_Save()
// Save PSG state to file
//-----------------------------------------------------------------------------
void        PSG_Save (FILE *f)
{
    byte    b;
    word    w;
    int     i;

    // PSG Data
    fwrite (PSG.Registers, 8, sizeof (word), f);
    b = PSG.LatchedRegister;
    fwrite (&b,            1, sizeof (byte), f);
    b = PSG.Stereo;
    fwrite (&b,            1, sizeof (byte), f);
    w = PSG.NoiseShiftRegister;
    fwrite (&w,            1, sizeof (word), f);

    // Implementation Data
    for (i = 0; i < 4; i++)
    {
        fwrite (&PSG.Channels[i].ToneFreqVal,     1, sizeof (signed short int), f);
        fwrite (&PSG.Channels[i].ToneFreqPos,     1, sizeof (signed char),      f);
        fwrite (&PSG.Channels[i].IntermediatePos, 1, sizeof (signed long int),  f);
    }
}

//-----------------------------------------------------------------------------
// PSG_Load()
// Load PSG state from a file
//-----------------------------------------------------------------------------
void        PSG_Load (FILE *f, int version)
{
    int     i;

    // PSG Registers
    fread (PSG.Registers, 8, sizeof (word), f);
    if (version < 0x0B)
    {
        // Legagy Loading
        char dummy_buffer[5];
        fread (dummy_buffer, 4+1, sizeof (char), f);
        PSG.LatchedRegister = 0;
        PSG.Stereo = 0xFF;
        PSG.NoiseShiftRegister = NoiseInitialState;
        for (i = 0; i < 4; i++)
        {
            // Fixup zero frequencies (FIXME: this is implementation dependant!)
            if (i < 3)
                if (PSG.Registers[i * 2] == 0)
                    PSG.Registers[i * 2] = 1;
            // Set counters to 0
            PSG.Channels[i].ToneFreqVal = 0;
            // Set flip-flops to 1
            PSG.Channels[i].ToneFreqPos = 1;
            // Set intermediate positions to do-not-use value
            PSG.Channels[i].IntermediatePos = LONG_MIN;
        }
    }
    else
    {
        byte b;
        word w;

        // PSG Data
        fread (&b, 1, sizeof (byte), f);
        PSG.LatchedRegister = b;
        fread (&b, 1, sizeof (byte), f);
        PSG.Stereo = b;
        fread (&w, 1, sizeof (word), f);
        PSG.NoiseShiftRegister = w;
        // Implemention Data
        for (i = 0; i < 4; i++)
        {
            fread (&PSG.Channels[i].ToneFreqVal,     1, sizeof (signed short int), f);
            fread (&PSG.Channels[i].ToneFreqPos,     1, sizeof (signed char),      f);
            fread (&PSG.Channels[i].IntermediatePos, 1, sizeof (signed long int),  f);
        }
    }
    PSG_Regenerate();
}

//-----------------------------------------------------------------------------
// PSG_Regenerate()
// Regenerate various PSG data from registers
// This is called after a PSG state loading
//-----------------------------------------------------------------------------
void        PSG_Regenerate (void)
{
    // Set channels volume
    for (int i = 0; i < 4; i++)
        PSG.Channels[i].Volume = PSGVolumeValues[PSG.Registers[i*2+1] & 0x0F];

    // Set noise signal generator frequency
    PSG.NoiseFreq = 0x10 << (PSG.Registers[6] & 0x3);
}

// Mute PSG by setting all Volumes to 15
void        PSG_Mute (void)
{
    SN76489_Write (0x9F); // Channel 0 Volume 15
    SN76489_Write (0xBF); // Channel 1 Volume 15
    SN76489_Write (0xDF); // Channel 2 Volume 15
    SN76489_Write (0xFF); // Channel 3 Volume 15
}

// Initialise and reset emulated PSG, given clock and sampling rate
void        SN76489_Reset(const unsigned long PSGClockValue, const unsigned long SamplingRate)
{
	// Probably unnecessarily verbose
	Active = (PSGClockValue > 0);
	if (!Active) return;

	PSG.SamplingRate = SamplingRate;
	PSG.Clock = 0.0f;
	PSG.dClock = (float)PSGClockValue / (16 * SamplingRate);

	PSG.LatchedRegister = 0;		// Tone0 is latched on startup
	PSG.Stereo = 0xFF;

	// Setup default channels parameters
	for (int i = 0; i < 4; i++)
	{
		t_psg_channel*c = &PSG.Channels[i];

		// Set all frequencies to 1
		PSG.Registers[i * 2 + 0] = 1;
		// Set volumes to off (0xf)
		PSG.Registers[i * 2 + 1] = 0xF;
		c->Volume = 0;
		// Set counters to 0
		c->ToneFreqVal = 0;
		// Set flip-flops to 1
		c->ToneFreqPos = 1;
		// Set intermediate positions to do-not-use value
		c->IntermediatePos = LONG_MIN;
	}

	// Noise channels default parameters
	PSG.NoiseFreq = 0x10;
	PSG.NoiseShiftRegister = NoiseInitialState;
}

// Set PSG clock based on master CPU clock
void    SN76489_SetClock(const unsigned long PSGClockValue)
{
    PSG.dClock = (float)PSGClockValue / (16 * PSG.SamplingRate);
}

//------------------------------------------------------------------------------
// SN76489_Write()
// IO Write to PSG
//------------------------------------------------------------------------------
void        SN76489_Write(const unsigned char data)
{
    // VGM Logging
    if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
        VGM_Data_Add_PSG (&Sound.LogVGM, (byte)data);

    if (!Active) return;

    #ifdef DEBUG_PSG
    {
        char bf[9];
        StrWriteBitfield (data, 8, bf);
        //Msg(MSGT_DEBUG, "%04X [%d] @ PSG Write %02X / %s (Latched = %d)", CPU_GetPC, sms.Pages_Reg [1], data, bf, PSG.LatchedRegister);
        Msg(MSGT_DEBUG, "%04X @ PSG Write %02X / %s (Latched = %d)", CPU_GetPC, data, bf, PSG.LatchedRegister);
    }
    #endif

    // Update the output buffer before changing the registers
	SoundStream_RenderUpToCurrentTime(g_psg_stream);

    if (data & 0x80)
    {
        // Latch/data byte '1cctdddd'
        PSG.LatchedRegister = ((data >> 4) & 0x07);
        // Replace low 4 bits in registers with new data
        PSG.Registers[PSG.LatchedRegister] = (PSG.Registers[PSG.LatchedRegister] & 0x3F0) | (data & 0x0F);
        if (PSG.LatchedRegister & 1)
            PSG.Channels[PSG.LatchedRegister >> 1].Volume = PSGVolumeValues[data & 0x0F];
        #ifdef DEBUG_PSG
            Msg(MSGT_DEBUG, "%04X @ PSG.Regs[%d] = %04X", CPU_GetPC, PSG.LatchedRegister, PSG.Registers[PSG.LatchedRegister]);
        #endif
    }
    else
    {
        // Data byte '0-dddddd'
        switch (PSG.LatchedRegister)
        {
        case 0:
        case 2: // Tone Frequency Registers
        case 4: // Replace high 6 bits with data
            PSG.Registers[PSG.LatchedRegister] = (PSG.Registers[PSG.LatchedRegister] & 0x00F) | ((data & 0x3F) << 4);
            break;
        case 6: // Noise Frequency / Settings
            PSG.Registers[PSG.LatchedRegister] = data & 0x0F;
            break;
        case 1:
        case 3:
        case 5:
        case 7: // Volumes
            PSG.Registers[PSG.LatchedRegister] = data & 0x0F;
            PSG.Channels[PSG.LatchedRegister >> 1].Volume = PSGVolumeValues[data & 0x0F];
            break;
        }
        #ifdef DEBUG_PSG
            Msg(MSGT_DEBUG, "%04X @ PSG.Regs[%d] = %04X", CPU_GetPC, PSG.LatchedRegister, PSG.Registers[PSG.LatchedRegister]);
        #endif
    }

    switch (PSG.LatchedRegister)
    {
    case 0:
    case 2:
    case 4: // Tone Channels
        if (PSG.Registers[PSG.LatchedRegister] == 0)
            PSG.Registers[PSG.LatchedRegister] = 1;         // Zero frequency changed to 1 to avoid div/0 // FIXME
        break;
    case 6: // Noise
        PSG.NoiseShiftRegister = NoiseInitialState;        // Reset shift register
        PSG.NoiseFreq = 0x10 << (PSG.Registers[6] & 0x3);  // Set noise signal generator frequency
        break;
    }

    /*
    Msg(MSGT_DEBUG, " Now Latched = %d", PSG.LatchedRegister);
    */
};

//------------------------------------------------------------------------------
// SN76489_StereoWrite()
// Write to the stereo register (as used on the Game Gear)
//------------------------------------------------------------------------------
void    SN76489_StereoWrite(const unsigned char data)
{
	if (!Active) return;
	PSG.Stereo = data;
};

//------------------------------------------------------------------------------
// SN76489_GetValues()
// Write synthetised values for left and right channels to given addresses
// result_right can be NULL if stereo is unwanted
// FIXME: This may not be the task of the synthetiser to take care of Stereo
//------------------------------------------------------------------------------
void    SN76489_GetValues(int *result_left, int *result_right)
{
	signed short int Channels[4]; // Value of each channel, before stereo is applied

	if (!Active) return;

	// Tone channels
	t_psg_channel* c = &PSG.Channels[0];
	for (int i = 0; i <= 2; i++, c++)
	{
		if (c->Active == FALSE)
		{
			Channels[i] = 0;
			continue;
		}
		if (c->IntermediatePos != LONG_MIN)
			Channels[i] = (c->Volume * c->IntermediatePos) >> 16;
		else
			Channels[i] = c->Volume * c->ToneFreqPos;
	}
	// Noise channel
	if (c->Active == FALSE)
		Channels[3] = 0;
	else
		Channels[3] = (short)(2 * c->Volume * (PSG.NoiseShiftRegister & 0x1));

	// Mix and output result
	if (result_right == NULL)
	{
		*result_left = Channels[0] + Channels[1] + Channels[2] + Channels[3];
	}
	else
	{
		int mix = 0;
		if (PSG.Stereo & 0x10)     mix += Channels[0];
		if (PSG.Stereo & 0x20)     mix += Channels[1];
		if (PSG.Stereo & 0x40)     mix += Channels[2];
		if (PSG.Stereo & 0x80)     mix += Channels[3];
		*result_left = mix;
		if (((PSG.Stereo & 0xF0) >> 4) == (PSG.Stereo & 0x0F))
		{
			*result_right = mix;
		}
		else
		{
			mix = 0;
			if (PSG.Stereo & 0x01)  mix += Channels[0];
			if (PSG.Stereo & 0x02)  mix += Channels[1];
			if (PSG.Stereo & 0x04)  mix += Channels[2];
			if (PSG.Stereo & 0x08)  mix += Channels[3];
			*result_right = mix;
		}
	}

	// Update Clock
	PSG.Clock += PSG.dClock;
	PSG.NumClocksForSample = PSG.Clock;           // Truncates
	PSG.Clock -= PSG.NumClocksForSample;          // Remove integer part

	// Decrement tone channel counters
	for (int i = 0; i <= 2; i++)
		PSG.Channels[i].ToneFreqVal -= PSG.NumClocksForSample;

	// Noise channel: match to tone2 or decrement its counter
	if (PSG.NoiseFreq == 0x80)
		PSG.Channels[3].ToneFreqVal = PSG.Channels[2].ToneFreqVal;
	else
		PSG.Channels[3].ToneFreqVal -= PSG.NumClocksForSample;

	// Value below which PSG does not output
#define PSG_CUTOFF 0x6

	// Tone channels:
	c = &PSG.Channels[0];
	for (int i = 0; i <= 2; i++, c++)
	{
		if (c->ToneFreqVal <= 0)  // If it gets below 0...
		{
			int ToneFreq = PSG.Registers[i * 2 + 0];

			// FIXME [Omar] This is because asynchronous calls of this while PSG is being
			// acessed on the main thread sometimes screw up this.
			if (ToneFreq == 0)
				ToneFreq = 1;

			if (ToneFreq > PSG_CUTOFF)
			{
				// Calculate how much of the sample is + and how much is -
				// Go to floating point and include the clock fraction for extreme accuracy :D
				// Store as long int, maybe it's faster? I'm not very good at this
				c->IntermediatePos = (long)
					(
					(PSG.NumClocksForSample - PSG.Clock + 2 * c->ToneFreqVal)
					* c->ToneFreqPos / (PSG.NumClocksForSample + PSG.Clock)
					* 65636
					);
				c->ToneFreqPos = -c->ToneFreqPos;   // Flip the flip-flop
			}
			else
			{
				c->ToneFreqPos = 1;                 // Stuck value
				c->IntermediatePos = LONG_MIN;
			};
			c->ToneFreqVal += ToneFreq * (PSG.NumClocksForSample / ToneFreq+1);
		}
		else
		{
			c->IntermediatePos = LONG_MIN;
		}
	};

	// Noise channel
	if (c->ToneFreqVal <= 0)              // If it gets below 0...
	{
		c->ToneFreqPos = -c->ToneFreqPos;  // Flip the flip-flop
		if (PSG.NoiseFreq != 0x80)         // If not matching tone2, decrement counter
			c->ToneFreqVal += PSG.NoiseFreq * (PSG.NumClocksForSample / PSG.NoiseFreq+1);
		if (c->ToneFreqPos == 1)
		{
			// Only once per cycle...
			// General method:
			/*
			int Feedback = 0;
			if (PSG.Registers[6] & 0x04)
			{ // For white noise:
			int i;            // Calculate the XOR of the tapped bits for feedback
			unsigned short int tapped = PSG.NoiseShiftRegister & NoiseWhiteFeedback;
			for (i = 0; i < 16; i++)
			Feedback += (tapped >> i) & 1;
			Feedback &= 1;
			}
			else
			{
			Feedback = PSG.NoiseShiftRegister & 1; // For periodic: feedback=output
			PSG.NoiseShiftRegister = (PSG.NoiseShiftRegister >> 1) | (Feedback << 15);
			}
			*/

			// SMS-only method, probably a bit faster:
			int Feedback = 0;
			if (PSG.Registers[6] & 0x04)  // White Noise
				Feedback = ((PSG.NoiseShiftRegister & 0x9) && ((PSG.NoiseShiftRegister & 0x9) ^ 0x9));
			else    // Periodic Noise
				Feedback = PSG.NoiseShiftRegister & 1;    // For periodic: feedback=output
			PSG.NoiseShiftRegister = (PSG.NoiseShiftRegister >> 1) | (Feedback << 15);
		}
	};
};

