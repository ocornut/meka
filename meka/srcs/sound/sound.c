//-----------------------------------------------------------------------------
// MEKA - sound.c
// Sound Engine - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fmunit.h"
#include "fskipper.h"
#include "psg.h"
#include "emu2413/mekaintf.h"
#include "sound_logging.h"
#include "desktop.h"
#include "g_widget.h"
#include "tvtype.h"

//-----------------------------------------------------------------------------
// FORWARD DECLARATIONS
//-----------------------------------------------------------------------------

static bool	Sound_InitEmulators();

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

enum t_sound_chip
{
	SOUND_CHIP_PSG,
	SOUND_CHIP_FM,
};

struct t_sound_stream
{
	t_sound_chip			chip;
	ALLEGRO_EVENT_QUEUE	*	event_queue;
	ALLEGRO_AUDIO_STREAM *	audio_stream;
	s16	*					audio_buffer;
	int						audio_buffer_size;
	int						audio_buffer_wpos;	// write position for chip emulator 
	int						audio_buffer_rpos;	// read position for audio system
	void					(*sample_writer)(void*,int);

	// Counters
	double					samples_leftover;
	int						samples_rendered0;
	int						samples_rendered1;
	s64						last_rendered_cycle_counter;
};

t_sound			Sound;
t_sound_stream*	g_psg_stream = NULL;
t_sound_stream*	g_ym2413_stream = NULL;

struct t_app_sound_debug
{
	t_gui_box*	box;
	bool		active;
};

static t_app_sound_debug	SoundDebugApp;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

// Initialize sound structure with its default settings
void	Sound_Init_Config(void)
{
    // General
    Sound.Enabled       = TRUE;
    Sound.Initialized   = FALSE;
    Sound.SampleRate    = 44100;                  // 44100 Hz by default
    Sound.Paused        = 0;
    Sound.MasterVolume  = 128;
	Sound.CycleCounter	= 0;

    // Sound Logging
    Sound_Log_Init();
}

// Initialize actual sound engine ---------------------------------------------
int		Sound_Init(void)
{
    // Temporarily set fake/null FM interface
    // This is to avoid crashing when using FM registers (savestates...) if sound is disabled.
    FM_Null_Active();

    // Skip if there is no need to initialize sound now
    // FIXME: Does MEKA works properly with no soundcard, is the machine updating registters properly and saving good savestates ?
    if (Sound.Enabled == FALSE)
        return (MEKA_ERR_OK);

    // Print Sound initialization message
    ConsolePrintf ("%s\n", Msg_Get(MSG_Sound_Init));
	ConsolePrintf (Msg_Get(MSG_Sound_Init_Soundcard), Sound.SampleRate);

	if (!al_install_audio())
	{
		Quit_Msg("%s", Msg_Get(MSG_Sound_Init_Error_Audio));
		return (MEKA_ERR_FAIL);
	}
	if (!al_reserve_samples(0))
	{
		Quit_Msg("%s", Msg_Get(MSG_Sound_Init_Error_Audio));
		return (MEKA_ERR_FAIL);
	}
	ConsolePrintf ("\n");

    // Initialize Sound card
    // Start in pause mode, to avoid sound update on startup (could crash, before everything is initialized)
    Sound.Paused = TRUE;

    // Initialize Sound emulators
    if (!Sound_InitEmulators())
	{
		Quit_Msg("%s", Msg_Get(MSG_Sound_Init_Error_Audio));
		return (MEKA_ERR_FAIL);
	}

	Sound_UpdateClockSpeed();

    Sound.Initialized = TRUE;
    return (MEKA_ERR_OK);
}

/* static void SawSampleWrite(s16* buf, int len)
{
	static int val = 0;
	static int pitch = 0x10000;
	for (int i = 0; i < ken; i++) 
	{
		buf[i] = ((val >> 16) & 0xff)*100;
		val += pitch;
		pitch++;
	}
}*/

static bool	Sound_InitEmulators(void)
{
	PSG_Init();
	g_psg_stream = SoundStream_Create(PSG_WriteSamples);
	if (!g_psg_stream)
		return false;

	FM_Digital_Init();
	FM_Digital_Active();
	g_ym2413_stream = SoundStream_Create(FM_Digital_WriteSamples);
	if (!g_ym2413_stream)
		return false;

	return true;
}

void	Sound_Close(void)
{
	FM_Digital_Close();

    if (!Sound.Initialized)
		return;

    Sound_Log_Close();
	SoundStream_Destroy(g_psg_stream);
	SoundStream_Destroy(g_ym2413_stream);
	al_uninstall_audio();
    Sound.Initialized = FALSE;
}

void	Sound_UpdateClockSpeed(void)
{
	const double throttle_scale = 1.0f;//(double)fskipper.Throttled_Speed / (double)g_machine.TV->screen_frequency;
	//Sound.CpuClock = g_machine.TV->CPU_clock;
	Sound.CpuClock = CPU_GetIPeriod() * g_machine.TV->screen_lines * g_machine.TV->screen_frequency;
	SN76489_SetClock((double)Sound.CpuClock * throttle_scale);
	// FIXME-NEWSOUND: FM?
}

void	Sound_Update(void)
{
	if (!Sound.Enabled)
		return;

	static int frame_count = 0;
	if ((frame_count++ % 60) == 0)
	{
		if (SoundDebugApp.active)
			Msg(MSGT_DEBUG, "Tick, samples rendered %d %d", g_psg_stream->samples_rendered0, g_psg_stream->samples_rendered1);
		g_psg_stream->samples_rendered0 = 0;
		g_psg_stream->samples_rendered1 = 0;
	}
	SoundStream_Update(g_psg_stream);
	SoundStream_Update(g_ym2413_stream);
}

void    Sound_Playback_Start(void)
{
    Sound.Paused = TRUE;
    Sound_Playback_Resume();
}

void    Sound_Playback_Stop(void)
{
    Sound.Paused = FALSE;
    Sound_Playback_Mute();
}

// Mute sound playback
// Increase 'Sound.Paused' counter and mute sound on >= 1
void    Sound_Playback_Mute(void)
{
    if (Sound.Paused == 0)
    {
        FM_Mute();
    }
    Sound.Paused++;
}

// Resume sound playback
// Decrease Sound.Paused counter and resume sound on zero
void    Sound_Playback_Resume(void)
{
    Sound.Paused--;
    if (Sound.Paused == 0)
    {
        FM_Resume();
    }
}

// Change Master Volume (0-128)
void    Sound_SetMasterVolume(int volume)
{
    Sound.MasterVolume = volume;
	// FIXME-NEWSOUND: Master volume
}

void Sound_ResetCycleCounter()
{
	Sound.CycleCounter = 0;
	Sound_UpdateClockSpeed();
	if (g_psg_stream)
		g_psg_stream->last_rendered_cycle_counter = 0;
	if (g_ym2413_stream)
		g_ym2413_stream->last_rendered_cycle_counter = 0;
}

s64 Sound_GetElapsedCycleCounter()
{
	// Calculate current CPU time
	const int icount = CPU_GetICount(); // - Sound_Update_Count;
	const int iperiod = CPU_GetIPeriod();

	// iperiod : 228
	// iperiod : 228.. 227.. 226.. [..] .. 3.. 2.. 1.. 0..
	// Cycle elapsed in the period : iperiod-icount
	// Cycle left in the period    : icount
	return Sound.CycleCounter + (iperiod - icount);
}

double Sound_ConvertSamplesToCycles(double samples_count)
{
	return samples_count * (double)Sound.CpuClock / (double)Sound.SampleRate;
}

//-----------------------------------------------------------------------------

t_sound_stream*	SoundStream_Create(void (*sample_writer)(void*,int))
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

	stream->audio_buffer_size = SOUND_BUFFERS_SIZE*SOUND_BUFFERS_COUNT*4;
	stream->audio_buffer = new s16[stream->audio_buffer_size];
	stream->audio_buffer_wpos = 0;
	stream->audio_buffer_rpos = 0;
	stream->sample_writer = sample_writer;

	stream->samples_leftover = 0.0f;
	stream->samples_rendered0 = 0;
	stream->samples_rendered1 = 0;
	stream->last_rendered_cycle_counter = 0;

	return stream;
}

void SoundStream_Destroy(t_sound_stream* stream)
{
	al_destroy_audio_stream(stream->audio_stream);
	delete [] stream->audio_buffer;
	al_destroy_event_queue(stream->event_queue);
	delete stream;
}

void SoundStream_Update(t_sound_stream* stream)
{
	ALLEGRO_EVENT sound_event;
	while (al_get_next_event(stream->event_queue, &sound_event))
	{
		if (sound_event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT)
		{
			s16* buf = (s16*)al_get_audio_stream_fragment(stream->audio_stream);
			if (!buf)
				continue;

			// Need to catch up?
			if (SoundStream_CountReadableSamples(stream) < SOUND_BUFFERS_SIZE)
			{
				SoundStream_RenderSamples(stream, SOUND_BUFFERS_SIZE);
				/*Msg(MSGT_DEBUG, "Sound catchup by %d samples", SOUND_BUFFERS_SIZE);
				Msg(MSGT_DEBUG, "%lld -> %lld, %lld + %.2f = %lld",
					stream->last_rendered_cycle_counter, Sound_GetElapsedCycleCounter(),
					stream->last_rendered_cycle_counter, (float)Sound_ConvertSamplesToCycles(SOUND_BUFFERS_SIZE),
					(s64)(stream->last_rendered_cycle_counter + (float)Sound_ConvertSamplesToCycles(SOUND_BUFFERS_SIZE)) );
					*/
				stream->last_rendered_cycle_counter = Sound_GetElapsedCycleCounter();
				//stream->last_rendered_cycle_counter += Sound_ConvertSamplesToCycles(SOUND_BUFFERS_SIZE);
			}

			SoundStream_PopSamples(stream, buf, SOUND_BUFFERS_SIZE);

			if (!al_set_audio_stream_fragment(stream->audio_stream, buf))
				Msg(MSGT_DEBUG, "Error in al_set_audio_stream_fragment()");
        }
	}
}

void SoundStream_RenderSamples(t_sound_stream* stream, int samples_count)
{
	//Msg(MSGT_DEBUG, "RenderSamples() %d", samples_count);
	if (samples_count <= 0)
	{
		Msg(MSGT_DEBUG, "RenderSamples %d", samples_count);
		return;
	}

	stream->samples_rendered0 += samples_count;

	s16* wbuf1;
	s16* wbuf2;
	int wbuf1_len;
	int wbuf2_len;
	if (SoundStream_PushSamplesRequestBufs(stream, samples_count, &wbuf1, &wbuf1_len, &wbuf2, &wbuf2_len))
	{
		if (wbuf1)
			stream->sample_writer(wbuf1, wbuf1_len);
		if (wbuf2)
			stream->sample_writer(wbuf2, wbuf2_len);
		stream->samples_rendered1 += wbuf1_len + wbuf2_len;
	}
}

void SoundStream_RenderUpToCurrentTime(t_sound_stream* stream)
{
	if (!Sound.Enabled)
		return;

	s64 current_cycle = Sound_GetElapsedCycleCounter();
	s64 elapsed_cycles = current_cycle - stream->last_rendered_cycle_counter;

	//elapsed_cycles = MIN(elapsed_cycles, 10000000);	// FIXME-NEWSOUND: how to handle pause, etc?

	// Convert elapsed cycles in 'frames' unit
	const int cpu_clock = Sound.CpuClock;
	const double elapsed_emulated_seconds = (double)((double)elapsed_cycles / (double)cpu_clock);

	const double samples_to_render = stream->samples_leftover + (double)Sound.SampleRate * elapsed_emulated_seconds;
	if ((int)samples_to_render > 0)
	{
		//Msg(MSGT_DEBUG, "RenderUpToCurrent() %d cycles -> %.2f samples", (int)elapsed_cycles, (float)samples_to_render);
		SoundStream_RenderSamples(stream, (int)samples_to_render);

		{
			//samples_to_render = samplerate * (elapsed_cycles / cpu_clock);
			//samples_to_render / (elapsed_cycles / cpu_clock) = samplerate;
			//(samples_to_render * cpu_clock) / elapsed_cycles = samplerate;
			//(samples_to_render * cpu_clock) = samplerate * elapsed_cycles;
			//(samples_to_render * cpu_clock) / samplerate = elapsed_cycles;

			//cycle = (rate / samples_to_render) * clock;
			//const float recalc_elapsed_cycles = Sound_ConvertSamplesToCycles((int)samples_to_render);
			//Msg(MSGT_DEBUG, "Elapsed %d <> %.2f", (int)elapsed_cycles, recalc_elapsed_cycles);
		}

		stream->last_rendered_cycle_counter = current_cycle;
		stream->samples_leftover = (double)samples_to_render - (int)samples_to_render;
	}
}

int	SoundStream_CountReadableSamples(const t_sound_stream* stream)
{
	if (stream->audio_buffer_rpos == stream->audio_buffer_wpos)
		return 0;

	// Circular buffer
	if (stream->audio_buffer_rpos < stream->audio_buffer_wpos)
		return (stream->audio_buffer_wpos - stream->audio_buffer_rpos);
	else
		return (stream->audio_buffer_size - stream->audio_buffer_rpos) + stream->audio_buffer_wpos;
}

int SoundStream_CountWritableSamples(const t_sound_stream* stream)
{
	return stream->audio_buffer_size - SoundStream_CountReadableSamples(stream);
}

bool SoundStream_PushSamplesRequestBufs(t_sound_stream* stream, int samples_count, s16** wbuf1, int* wbuf1_len, s16** wbuf2, int* wbuf2_len)
{
	*wbuf1 = *wbuf2 = NULL;
	*wbuf1_len = *wbuf2_len = 0;

	s16* buf_begin = &stream->audio_buffer[0];
	s16* buf_end = &stream->audio_buffer[stream->audio_buffer_size];

	int writable_samples = SoundStream_CountWritableSamples(stream);
	if (writable_samples < samples_count)
	{
		Msg(MSGT_DEBUG, "PushSamplesRequestBufs(): overflow %d > %d", samples_count, writable_samples);
		return false;
	}

	if (stream->audio_buffer_wpos >= stream->audio_buffer_rpos)
	{
		// Provide writable buffer for wpos->end section
		const int samples_to_write = MIN(samples_count, stream->audio_buffer_size - stream->audio_buffer_wpos);
		if (samples_to_write < 0)
			assert(0);
		*wbuf1 = &stream->audio_buffer[stream->audio_buffer_wpos];
		*wbuf1_len = samples_to_write;
		if (*wbuf1 && (*wbuf1 < buf_begin || *wbuf1+*wbuf1_len > buf_end))
			assert(0);
		samples_count -= samples_to_write;
		stream->audio_buffer_wpos = (stream->audio_buffer_wpos + samples_to_write) % stream->audio_buffer_size;
	}
	if (stream->audio_buffer_wpos < stream->audio_buffer_rpos)
	{
		// Provide writable buffer for wpos->rpos section
		const int samples_to_write = MIN(samples_count, stream->audio_buffer_rpos - stream->audio_buffer_wpos);
		if (samples_to_write < 0)
			assert(0);
		*wbuf2 = &stream->audio_buffer[stream->audio_buffer_wpos];
		*wbuf2_len = samples_to_write;
		if (*wbuf2 && (*wbuf2 < buf_begin || *wbuf2+*wbuf2_len > buf_end))
			assert(0);
		samples_count -= samples_to_write;
		stream->audio_buffer_wpos = (stream->audio_buffer_wpos + samples_to_write) % stream->audio_buffer_size;
	}

	return true;
}

int SoundStream_PopSamples(t_sound_stream* stream, s16* buf, int samples_wanted)
{
	//Msg(MSGT_DEBUG, "PopSamples() %d", samples_wanted);

	const int samples_avail = SoundStream_CountReadableSamples(stream);
	if (samples_avail < samples_wanted)
	{
		Msg(MSGT_DEBUG, "PopSamples(): underrun %d < %d available", samples_avail, samples_wanted);
		return 0;
	}
	//const s16* buf_start = buf;
	//const s16* buf_end = buf + samples_wanted;

	int samples_read = 0;
	while (samples_wanted > 0)
	{
		if (stream->audio_buffer_rpos == stream->audio_buffer_wpos)
		{
			Msg(MSGT_DEBUG, "SoundStream_PopSamples(): underrun in loop");
			break;
		}

		int samples_avail_contiguous;
		if (stream->audio_buffer_rpos < stream->audio_buffer_wpos)
		{
			// Read rpos->wpos
			samples_avail_contiguous = stream->audio_buffer_wpos - stream->audio_buffer_rpos;
		}
		else
		{
			// Read rpos->end
			samples_avail_contiguous = stream->audio_buffer_size - stream->audio_buffer_rpos;
		}

		const int samples_to_read = MIN(samples_wanted, samples_avail_contiguous);
		memcpy(&buf[samples_read], &stream->audio_buffer[stream->audio_buffer_rpos], samples_to_read * sizeof(s16));
		samples_read += samples_to_read;
		samples_wanted -= samples_to_read;
		stream->audio_buffer_rpos = (stream->audio_buffer_rpos + samples_to_read) % stream->audio_buffer_size;
	}

	return samples_read;
}

//-----------------------------------------------------------------------------

void SoundDebugApp_Init()
{
	t_app_sound_debug* app = &SoundDebugApp;

	t_frame frame;
    frame.pos.x = 150;
    frame.pos.y = 500;
    frame.size.x = 350;
    frame.size.y = 70;
	app->active = false;
	app->box = gui_box_new(&frame, "Sound Debug");
	app->box->user_data = app;
	app->box->update = SoundDebugApp_Update;
	widget_closebox_add(app->box, (t_widget_callback)SoundDebugApp_Switch);
    Desktop_Register_Box("SOUND_DEBUG", app->box, 0, &app->active);
}

void SoundDebugApp_InstallMenuItems(int menu_parent)
{
	t_app_sound_debug* app = &SoundDebugApp;
	menu_add_item(menu_parent, "Sound Debug..", NULL, MENU_ITEM_FLAG_ACTIVE | Is_Checked(app->active), (t_menu_callback)SoundDebugApp_Switch, app);
}

static void SoundDebugApp_Printf(int* px, int* py, const char* format, ...)
{
	t_app_sound_debug* app = &SoundDebugApp;

	char buf[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, countof(buf), format, args);
	va_end(args);
	
	al_set_target_bitmap(app->box->gfx_buffer);
	Font_Print(FONTID_MEDIUM, buf, *px, *py, COLOR_SKIN_WINDOW_TEXT);
	*py += Font_Height(FONTID_MEDIUM);
}

void SoundDebugApp_Update()
{
	t_app_sound_debug* app = &SoundDebugApp;
	if (!app->active)
		return;

	al_set_target_bitmap(app->box->gfx_buffer);
	al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

	if (!Sound.Enabled)
		return;

	int x = 4;
	int y = 0;

	static const char* stars64 = "****************************************************************";

	t_sound_stream* stream = g_psg_stream;
	int samples;

	samples = SoundStream_CountReadableSamples(stream);
	SoundDebugApp_Printf(&x, &y, "ReadableSamples: %-6d [%-32s]", samples, stars64+(64-MIN(32,samples/1024)));

	samples = SoundStream_CountWritableSamples(stream);
	SoundDebugApp_Printf(&x, &y, "WritableSamples: %-6d [%-32s]", samples, stars64+(64-MIN(32,samples/1024)));
}

// Called from closebox widget and menu handler
void SoundDebugApp_Switch()
{
	t_app_sound_debug *app = &SoundDebugApp;
	app->active ^= 1;
	gui_box_show(app->box, app->active, TRUE);
	gui_menu_toggle_check (menus_ID.sound, 4);	// FIXME-UGLY
}

//-----------------------------------------------------------------------------
