//-----------------------------------------------------------------------------
// MEKA - sound.h
// Sound Engine - Code. Initialization and main part of the mess.
//-----------------------------------------------------------------------------
// Hiromitsu Shioya, 1998-1999
// Omar Cornut, 1999+
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// FORWARD DECLARATIONS
//-----------------------------------------------------------------------------

static  int     Sound_Init_SoundCard(void);
static  void    Sound_Init_Emulators(void);

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

t_sound		Sound;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sound_Init_Config(void)
// Initialize sound structure with its default values
//-----------------------------------------------------------------------------
void	Sound_Init_Config(void)
{
    // General
    Sound.Enabled               = TRUE;
    Sound.Initialized           = FALSE;
    Sound.SoundCard             = SOUND_SOUNDCARD_SELECT; // Let user select by default
    Sound.SampleRate            = 44100;                  // 44100 Hz by default
    Sound.Paused                = FALSE; // 0
    Sound.MasterVolume          = 128;

    // Cycle counter
    Sound.CycleCounter = 0;

    // Sound Logging
    Sound_Log_Init ();
}

// Initialize actual sound engine ---------------------------------------------
int             Sound_Init (void)
{
    // Set fake/null FM interface by default
    // This is to avoid crashing when using FM registers (savestates...)
    // if sound is disabled.
    FM_Null_Active();

    // Skip if there is no need to initialize sound now
    // FIXME: will MEKA work properly with now soundcard ?
    // Are emulators functionning properly and saving good savestates ?
    if (Sound.Enabled == FALSE)
        return (MEKA_ERR_OK);
    if (Sound.SoundCard == SOUND_SOUNDCARD_NONE)
    {
        // Quit_Msg ("%s", Msg_Get (MSG_Sound_Init_Soundcard_No));
        // return (MEKA_ERR_FAIL);
        return (MEKA_ERR_OK);
    }

    // Print Sound initialization message
    ConsolePrintf ("%s\n", Msg_Get (MSG_Sound_Init));

    // Disable sound if user selected 'no soundcard'
    if (Sound.SoundCard == SOUND_SOUNDCARD_NONE)
    {
        // Sound.Enabled = FALSE;
        return (MEKA_ERR_OK);
    }

    // Initialize Sound card
    // Start in pause mode, to avoid sound update on startup (could crash, before everything is initialized)
    Sound.Paused = TRUE;
    Sound_Init_SoundCard();

    // Initialize Sound emulators
    Sound_Init_Emulators();

    Sound.Initialized = TRUE;
    return (MEKA_ERR_OK);
}

static  int     Sound_Init_SoundCard (void)
{
	ConsolePrintf (Msg_Get (MSG_Sound_Init_Soundcard), Sound.SampleRate);
	ConsolePrint ("\n");

	// FIXME-NEWSOUND
	// Quit_Msg (Msg_Get (MSG_Sound_Init_Error_Voice_N), i);

	return (MEKA_ERR_OK);
}

// Initialize Sound Emulators -------------------------------------------------
static  void    Sound_Init_Emulators (void)
{
	// FIXME-NEWSOUND: Register chipsets
	/*
    SoundRecEntry  rec;

    // Add SN76496 (PSG) emulator
    rec.sync       = 60;
    rec.count      = 1;
    rec.type       = SOUND_MACHINE_SN76496;
    rec.f_init     = PSG_Init;
    rec.f_update   = NULL; // PSG_Update_16 // Will be registered on initialisation
    rec.f_stop     = NULL;
    rec.userdata   = NULL;
    saSetupSound   (&rec);

    // Add EMU2413 (FM) emulator
    rec.type       = SOUND_MACHINE_EMU2413;
    rec.f_init     = FM_Digital_Init;
    rec.f_update   = NULL; // FM_Digital_Update; // will be registered on initialisation
    rec.f_stop     = FM_Digital_Close;
    rec.userdata   = NULL;
    saAddSound       (&rec);
	*/
}

// Close sound engine
void	Sound_Close (void)
{
    if (Sound.Initialized)
    {
        Sound_Log_Close();
        Sound.Initialized = FALSE;
    }
}

//-----------------------------------------------------------------------------
// Sound_Update_Frame ()
// Miscellaneous things to do on each frame
//-----------------------------------------------------------------------------
void	Sound_Update_Frame (void)
{
    // Decrement FM usage counter
    // To save CPU, FM emulation is disabled if it gets to zero
    // Msg (MSGT_DEBUG, "FM_Used = %d\n", FM_Used);
    if (FM_Used > 0)
        FM_Used --;
}

//-----------------------------------------------------------------------------
// Sound_Playback_Start ()
// Start sound playback
//-----------------------------------------------------------------------------
void    Sound_Playback_Start (void)
{
    Sound.Paused = TRUE;
    Sound_Playback_Resume ();
}

//-----------------------------------------------------------------------------
// Sound_Playback_Stop ()
// Stop sound playback
//-----------------------------------------------------------------------------
void    Sound_Playback_Stop (void)
{
    Sound.Paused = FALSE;
    Sound_Playback_Mute ();
}

//-----------------------------------------------------------------------------
// Sound_Playback_Mute ()
// Mute sound playback
// Increase 'Sound.Paused' counter and mute sound on 1 or more
//-----------------------------------------------------------------------------
void    Sound_Playback_Mute (void)
{
    if (Sound.Paused == 0)
    {
        FM_Mute();
    }
    Sound.Paused++;
}

//-----------------------------------------------------------------------------
// Sound_Playback_Resume ()
// Resume sound playback
// Decrease Sound.Paused counter and resume sound on zero
//-----------------------------------------------------------------------------
void    Sound_Playback_Resume (void)
{
    Sound.Paused--;
    if (Sound.Paused == 0)
    {
        FM_Resume ();
    }
}

// Change Master Volume (0-128)
void    Sound_MasterVolume_Set (int v)
{
    Sound.MasterVolume = v;
	// FIXME-NEWSOUND: Master volume
}
