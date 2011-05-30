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

struct t_sound_stream
{
	ALLEGRO_EVENT_QUEUE	*	event_queue;
	ALLEGRO_AUDIO_STREAM *	audio_stream;
	s16	*					audio_buffer;
	int						audio_buffer_size;
	int						audio_buffer_wpos;	// write position for chip emulator 
	int						audio_buffer_rpos;	// read position for audio system
};

t_sound				Sound;
t_sound_stream*		g_psg_stream;
t_sound_stream*		g_ym2413_stream;

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
    // Temporarily set fake/null FM interface
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

t_sound_stream*	Sound_CreateStream()
{
	t_sound_stream* stream = new t_sound_stream();
	stream->event_queue = al_create_event_queue();
	stream->audio_stream = al_create_audio_stream(SOUND_BUFFERS_COUNT, SOUND_BUFFERS_SIZE, Sound.SampleRate, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
	if (!stream->audio_stream)
	{
		delete stream;
		return NULL;
	}
	if (!al_attach_audio_stream_to_mixer(stream->audio_stream, al_get_default_mixer()))
	{
		delete stream;
		return NULL;
	}
	al_register_event_source(stream->event_queue, al_get_audio_stream_event_source(stream->audio_stream));

	stream->audio_buffer_size = SOUND_BUFFERS_COUNT*SOUND_BUFFERS_SIZE;
	stream->audio_buffer = new s16[stream->audio_buffer_size];
	stream->audio_buffer_wpos = 0;
	stream->audio_buffer_rpos = 0;

	return stream;
}

void Sound_DestroyStream(t_sound_stream* stream)
{
	al_destroy_audio_stream(stream->audio_stream);
	delete [] stream->audio_buffer;
	delete stream;
}

void Sound_UpdateStream(t_sound_stream* stream, void (*sample_writer)(void*,int))
{
	ALLEGRO_EVENT sound_event;
	while (al_get_next_event(stream->event_queue, &sound_event))
	{
		if (sound_event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT)
		{
			s16* buf = (s16*)al_get_audio_stream_fragment(stream->audio_stream);
			if (!buf)
				continue;

			sample_writer(buf, SOUND_BUFFERS_SIZE);
			/*
			static int val = 0;
			static int pitch = 0x10000;
			for (int i = 0; i < 4096; i++) 
			{
				buf[i] = ((val >> 16) & 0xff)*100;
				val += pitch;
				pitch++;
			}
			*/

			if (!al_set_audio_stream_fragment(stream->audio_stream, buf))
				Msg(MSGT_DEBUG, "Error in al_set_audio_stream_fragment()");
        }
	}
}

static bool	Sound_InitEmulators(void)
{
	PSG_Init();
	g_psg_stream = Sound_CreateStream();
	if (!g_psg_stream)
		return false;

	FM_Digital_Init();
	FM_Digital_Active();
	g_ym2413_stream = Sound_CreateStream();
	if (!g_ym2413_stream)
		return false;

	return true;
}

void	Sound_Close (void)
{
	FM_Digital_Close();

    if (!Sound.Initialized)
		return;

    Sound_Log_Close();
	Sound_DestroyStream(g_psg_stream);
	Sound_DestroyStream(g_ym2413_stream);
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

	Sound_UpdateStream(g_psg_stream, PSG_WriteSamples);
	Sound_UpdateStream(g_ym2413_stream, FM_Digital_WriteSamples);
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
