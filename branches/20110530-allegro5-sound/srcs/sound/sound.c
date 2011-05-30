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

static bool	Sound_InitEmulators(void);

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

t_sound	Sound;
ALLEGRO_AUDIO_STREAM *	g_psg_audio_stream = NULL;
ALLEGRO_EVENT_QUEUE *	g_sound_event_queue = NULL;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

// Initialize sound structure with its default settings
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
    Sound_Log_Init();
}

// Initialize actual sound engine ---------------------------------------------
int		Sound_Init (void)
{
    // Set fake/null FM interface by default
    // This is to avoid crashing when using FM registers (savestates...) if sound is disabled.
    FM_Null_Active();

    // Skip if there is no need to initialize sound now
    // FIXME: Does MEKA works properly with no soundcard, is the machine updating registters properly and saving good savestates ?
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
	ConsolePrintf (Msg_Get (MSG_Sound_Init_Soundcard), Sound.SampleRate);

	if (!al_install_audio())
	{
		Quit_Msg ("%s", Msg_Get(MSG_Sound_Init_Error_Audio));
		return (MEKA_ERR_FAIL);
	}
	if (!al_reserve_samples(0))
	{
		Quit_Msg ("%s", Msg_Get(MSG_Sound_Init_Error_Audio));
		return (MEKA_ERR_FAIL);
	}

    // Initialize Sound card
    // Start in pause mode, to avoid sound update on startup (could crash, before everything is initialized)
    Sound.Paused = TRUE;

    // Initialize Sound emulators
    if (!Sound_InitEmulators())
	{
		Quit_Msg ("%s", Msg_Get(MSG_Sound_Init_Error_Audio));
		return (MEKA_ERR_FAIL);
	}

    Sound.Initialized = TRUE;
    return (MEKA_ERR_OK);
}

static bool	Sound_InitEmulators(void)
{
	// FIXME-NEWSOUND: Register chipsets

	g_sound_event_queue = al_create_event_queue();

	g_psg_audio_stream = al_create_audio_stream(4, 4096, Sound.SampleRate, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
	if (!g_psg_audio_stream)
		return false;
	if (!al_attach_audio_stream_to_mixer(g_psg_audio_stream, al_get_default_mixer()))
		return false;
	al_register_event_source(g_sound_event_queue, al_get_audio_stream_event_source(g_psg_audio_stream));

	return true;

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

void	Sound_Close (void)
{
    if (!Sound.Initialized)
		return;

    Sound_Log_Close();
	if (g_psg_audio_stream)
		al_destroy_audio_stream(g_psg_audio_stream);
	al_uninstall_audio();
    Sound.Initialized = FALSE;
}

void	Sound_Update_Frame (void)
{
    // Decrement FM usage counter
    // To save CPU, FM emulation is disabled when it reach zero
    // Msg (MSGT_DEBUG, "FM_Used = %d\n", FM_Used);
    if (FM_Used > 0)
        FM_Used --;

	ALLEGRO_EVENT sound_event;
	while (al_get_next_event(g_sound_event_queue, &sound_event))
	{
		if (sound_event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT)
		{
			s16* buf = (s16*)al_get_audio_stream_fragment(g_psg_audio_stream);
			if (!buf)
				continue;

			static int val = 0;
			static int pitch = 0x10000;
			for (int i = 0; i < 4096; i++) 
			{
				buf[i] = ((val >> 16) & 0xff)*100;
				val += pitch;
				pitch++;
			}

			if (!al_set_audio_stream_fragment(g_psg_audio_stream, buf))
				Msg(MSGT_DEBUG, "Error in al_set_audio_stream_fragment()");
        }
	}
}

void    Sound_Playback_Start (void)
{
    Sound.Paused = TRUE;
    Sound_Playback_Resume ();
}

void    Sound_Playback_Stop (void)
{
    Sound.Paused = FALSE;
    Sound_Playback_Mute ();
}

// Mute sound playback
// Increase 'Sound.Paused' counter and mute sound on >= 1
void    Sound_Playback_Mute (void)
{
    if (Sound.Paused == 0)
    {
        FM_Mute();
    }
    Sound.Paused++;
}

// Resume sound playback
// Decrease Sound.Paused counter and resume sound on zero
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
