//-----------------------------------------------------------------------------
// MEKA - sound.c
// Sound Engine - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fmunit.h"
#include "psg.h"
#include "emu2413/mekaintf.h"
#include "sound_logging.h"
#include "desktop.h"
#include "g_widget.h"
#include "tvtype.h"

#include "fskipper.h" //Allegro (Wall) output sound rate (Z80cycles/sec) determined by current output framerate

#include "circular_buffer.h"

//Note on "Samples" vs "Frames": 
//A sample is an individual sound level for one channel
//An audio frame consists of exactly one sample of audio for each channel (by default CHANNEL_COUNT=2 here)
//So four frames of  2 channel audio will consist of 8 samples; LRLRLRLR

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

#define SOUND_INTERNAL static 

//-----------------------------------------------------------------------------
// FORWARD DECLARATIONS
//-----------------------------------------------------------------------------

static bool Sound_InitEmulators();

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
    t_sound_chip            chip;
    ALLEGRO_EVENT_QUEUE *   event_queue;
    ALLEGRO_AUDIO_STREAM *  audio_stream;
  
    CBuff                   audio_buffer;         //Samples buffer (circular buffer)

    t_audio_frame_writer    audio_frame_writer;   //A function which write a requested number of audio
                                                  //frames into a sample buffer. (Each frame may have several samples)

    // Counters
    double                  audio_frames_leftover;
    s64                     last_rendered_cycle_counter;

    u32                     frames_requested;       //Counter recording the number audio frames render requests
    u32                     samples_requested;      //Counter recording the  samples render requests
    u32                     samples_rendered;       //Counter recording the number of actual samples rendered after requests
};

t_sound         Sound;
t_sound_stream* g_psg_stream = NULL;
t_sound_stream* g_ym2413_stream = NULL;

struct t_app_sound_debug
{
    t_gui_box*  box;
    bool        active;
};

static t_app_sound_debug    SoundDebugApp;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

//Returns the total number of Z80 cpu cycles per frame
SOUND_INTERNAL int GetZ80CyclesPerFrame(t_machine *machine){

  const int cycles_per_line = CPU_GetIPeriod();             //Assuming IPeriod represents cycles per scanline (time between H-blanks)
  const int lines_per_frame = machine->TV->screen_lines;    //Assuming logical lines per frame (including v-blank time)

  const int cycles_per_frame = cycles_per_line * lines_per_frame;
  return cycles_per_frame;
}


// Initialize sound structure with its default settings
void    Sound_Init_Config(void)
{
    // General
    Sound.Enabled       = TRUE;
    Sound.Initialized   = FALSE;
    Sound.SampleRate    = 44100;                  // 44100 Hz by default
    Sound.Paused        = 0;
    Sound.MasterVolume  = 128;
    Sound.CycleCounter  = 0;

    // Sound Logging
    Sound_Log_Init();
}

// Initialize actual sound engine ---------------------------------------------
int     Sound_Init(void)
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

static bool Sound_InitEmulators()
{
    PSG_Init();
    g_psg_stream = SoundStream_Create(PSG_WriteAudioFrames);
    if (!g_psg_stream)
        return false;

    FM_Digital_Init();
    FM_Digital_Active();
    g_ym2413_stream = SoundStream_Create(FM_Digital_WriteAudioFrames);
    if (!g_ym2413_stream)
        return false;

    return true;
}

void    Sound_Close()
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


void    Sound_UpdateClockSpeed()
{

  const int cycles_per_frame = GetZ80CyclesPerFrame(&g_machine);
  Sound.CpuClock = cycles_per_frame * g_machine.TV->screen_frequency; //Emulated clock rate

  SN76489_SetClock((double)Sound.CpuClock);
    // FIXME-NEWSOUND: FM?
}

void    Sound_Update()
{
    if (!Sound.Enabled)
        return;

    static int frame_count = 0;
    if ((frame_count++ % 60) == 0)
    {
	if (SoundDebugApp.active){
            Msg(MSGT_DEBUG, "Tick, frames/samples/rendered %d %d %d",
		g_psg_stream->frames_requested, g_psg_stream->samples_requested, g_psg_stream->samples_rendered);
	}
	g_psg_stream->frames_requested = 0;
	g_psg_stream->samples_requested = 0;
	g_psg_stream->samples_rendered = 0;
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



//-----------------------------------------------------------------------------

t_sound_stream* SoundStream_Create(t_audio_frame_writer audio_frame_writer)
{
    t_sound_stream* stream = new t_sound_stream();
    stream->event_queue = al_create_event_queue();

#if SOUND_CHANNEL_COUNT == 1
    const ALLEGRO_CHANNEL_CONF channelConf = ALLEGRO_CHANNEL_CONF_1;
#elif SOUND_CHANNEL_COUNT == 2
    const ALLEGRO_CHANNEL_CONF channelConf = ALLEGRO_CHANNEL_CONF_2;
#else
    assert(0);
#endif

    //Note: Allegro's internal defintion of an audio "sample" actually an audio frame, whose number
    //of samples depends on the number of sound channels. The size of an allegro 'sample' depends both on them
    //audio bit depth and the number of sound channels.
    stream->audio_stream = al_create_audio_stream(SOUND_BUFFERS_COUNT, SOUND_BUFFERS_FRAME_COUNT, Sound.SampleRate, ALLEGRO_AUDIO_DEPTH_INT16, channelConf);
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

    const u32 SOUND_BUFFERS_SAMPLE_COUNT = SOUND_BUFFERS_FRAME_COUNT*SOUND_BUFFERS_COUNT;
    const u32 buffer_sample_capacity = SOUND_BUFFERS_SAMPLE_COUNT*SOUND_CHANNEL_COUNT;

    stream->audio_buffer = CBuff_CreateCircularBuffer(buffer_sample_capacity);
    
    stream->audio_frame_writer = audio_frame_writer;

    stream->audio_frames_leftover = 0.0f;
    stream->last_rendered_cycle_counter = 0;

    stream->frames_requested = 0;
    stream->samples_requested = 0;
    stream->samples_rendered = 0;

    return stream;
}

void SoundStream_Destroy(t_sound_stream* stream)
{
    al_destroy_audio_stream(stream->audio_stream);
    CBuff_DeleteCircularBuffer(&stream->audio_buffer);
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
	    //Note: What allegro refers to as a "sample" is in fact an audio frame of (possible multiple) samples, one sample for each channel in the frame
	    const u32 ALLEGRO_BUFFER_SIZE_FRAMES = SOUND_BUFFERS_FRAME_COUNT;
	    
	    const int frames_underrun = ALLEGRO_BUFFER_SIZE_FRAMES - SoundStream_CountReadableFrames(stream);
            if (frames_underrun > 0)
            {
		//Try to make enough audio frames to fill up the next Allegro output buffer
		SoundStream_RenderAudioFrames(stream, frames_underrun);

		stream->last_rendered_cycle_counter = Sound_GetElapsedCycleCounter();

            }

	    //Write a complete fragement's worth of audio frames into the allegro buffer
            SoundStream_PopFrames(stream, buf, ALLEGRO_BUFFER_SIZE_FRAMES);

            if (!al_set_audio_stream_fragment(stream->audio_stream, buf))
                Msg(MSGT_DEBUG, "Error in al_set_audio_stream_fragment()");
        }
    }
}


//Returns the current number of frames worth of audio stored  in the meka sound buffer
int SoundStream_CountReadableFrames(const t_sound_stream* stream)
{

  const u32 readable_samples = CBuff_Size(&stream->audio_buffer);
  const int readable_frames = readable_samples / SOUND_CHANNEL_COUNT;
  return readable_frames;

}

//Returns the current number of frames worth of audio which can be written to the meka sound buffer
int SoundStream_CountWritableFrames(const t_sound_stream* stream)
{
  //Number of samples written depends on SOUND_CHANNEL_COUNT!!!!! (In current sample logic) 

  const u32 writable_samples = CBuff_UnusedCapacity(&stream->audio_buffer);
  const u32 writable_frames = writable_samples / SOUND_CHANNEL_COUNT;
  return writable_frames;
  
}


void SoundStream_RenderAudioFrames(t_sound_stream* stream, const int frames_requested)
{

    if (frames_requested <= 0)
    {
        Msg(MSGT_DEBUG, "RenderAudioFrames() %d", frames_requested);
        return;
    }

    const u32 samples_requested = frames_requested*SOUND_CHANNEL_COUNT; //The frame contains one sample per sound channel
    
    const u32 samples_cap_remaining = CBuff_UnusedCapacity(&stream->audio_buffer);
    u32 samples_rendered = 0;
    if(samples_cap_remaining >= samples_requested){

	//Slightly awkward logic as we store samples, but the read/write requests are fundamentally in audio frames.

	//Get a split span from the circular sound buffer, giving two contiguous sample blocks, in logical sound order
	CBuff_SplitSpan write_span = CBuff_PushBackSpan(&stream->audio_buffer , samples_requested);

	//Each audio frame contains one sample per audio channel
	//(TODO: We may run into trouble if an odd number of samples is ever added)
	const u32 block1_frames = CBuff_Length(&write_span.block1)/SOUND_CHANNEL_COUNT;
	const u32 block2_frames = CBuff_Length(&write_span.block2)/SOUND_CHANNEL_COUNT;
	
	//Call the stream sample writer function to write data to each block in turn (The block lengths may be zero)
	stream->audio_frame_writer(write_span.block1.begin, block1_frames, SOUND_CHANNEL_COUNT);
	stream->audio_frame_writer(write_span.block2.begin, block2_frames, SOUND_CHANNEL_COUNT);
	
	samples_rendered = CBuff_Length(&write_span);

	if(samples_rendered != samples_requested){
	    Msg(MSGT_DEBUG, "RenderAudioFrames(): Samples Requested/Rendered: %d > %d", samples_requested, samples_rendered);
	}
	          
    } else {
      Msg(MSGT_DEBUG, "RenderAudioFrames(): CBuff overflow: %d > %d", samples_requested, samples_cap_remaining);
    }

    //Keep a record of frame/sample requests/renders
    stream->frames_requested += frames_requested;
    stream->samples_requested += samples_requested;
    stream->samples_rendered  += samples_rendered;
    
    
}

void SoundStream_RenderUpToCurrentTime(t_sound_stream* stream)
{
    if (!Sound.Enabled)
        return;

    s64 current_cycle = Sound_GetElapsedCycleCounter();
    s64 elapsed_cycles = current_cycle - stream->last_rendered_cycle_counter;

    //elapsed_cycles = MIN(elapsed_cycles, 10000000);   // FIXME-NEWSOUND: how to handle pause, etc?

    
    //The OUTPUT (Wall) sound clock (in Z80cycles/sec) is different from the
    //emulated sound clock (in Z80cycles/sec) as we may be running at a non-standard FPS (e.g. 10Hz or 200Hz).
    //We are generating audi frame for the OUPUT sound buffers, so we need to base our clock rate and rendered frame
    //count on the output FPS rate (otherwise we will over/underflow the actual output sound)
    
    //We can base the output FPS rate on either fskipper.Throttled_Speed (requested) or fskipper.FPS (measured)
    //Throttled speed appears to avoid most overflows, but the actually measured FPS rate does better at 10Hz (and might work for unthrottled fps)
    const double output_FPS = (double) fskipper.Throttled_Speed;
    const double output_cycles_per_sec = GetZ80CyclesPerFrame(&g_machine) * output_FPS;
    const double elasped_output_seconds = (double)((double)elapsed_cycles / (double)output_cycles_per_sec);
    //Note: elapsed_emulated_seconds = elapsed_cycles * Sound.CpuClock;
    
    // TL;DR We need enough audio frames for the elapsed output (Wall) time, not the elapsed emulated time.

    const int single_channel_sound_rate = Sound.SampleRate;
    const int audio_frame_rate = single_channel_sound_rate; //The audio frame rate is the same as all single channel sound rates
    
    const double audio_frames_to_render = stream->audio_frames_leftover + (double)audio_frame_rate * elasped_output_seconds;
    
    if ((int)audio_frames_to_render > 0)
    {

        SoundStream_RenderAudioFrames(stream, (int)audio_frames_to_render);

        stream->last_rendered_cycle_counter = current_cycle;
        stream->audio_frames_leftover = (double)audio_frames_to_render - (int)audio_frames_to_render;
    }
}


//Try to read enough audio samples for the requested number of audio frames from the internal sound buffer. The number of samples actually read is returned. 
u32 SoundStream_PopFrames(t_sound_stream* stream, s16* buf, const int frames_wanted)
{

    const int frames_avail = SoundStream_CountReadableFrames(stream);
    if (frames_avail < frames_wanted)
    {
        Msg(MSGT_DEBUG, "PopFrames(): underrun %d < %d available", frames_avail, frames_wanted);
        return 0;
    }

    //The audio buffers stores raw audio samples, one sample per channel in the audio frame
    const int samples_wanted = frames_wanted*SOUND_CHANNEL_COUNT; 

    //Get a split span from the circular sound buffer, giving two contiguous blocks, in logical sound order
    CBuff_SplitSpan read_span = CBuff_PopFrontSpan(&stream->audio_buffer, samples_wanted);

    const CBuff_Block block1 = read_span.block1;
    const CBuff_Block block2 = read_span.block2;
    const u32 block1_samples = CBuff_Length(&block1);
    const u32 block2_samples = CBuff_Length(&block2);

    //Copy each block in turn into the provided buffer (the block lengths can be zero
    memcpy(buf, block1.begin, block1_samples * sizeof(s16));
    memcpy(buf+block1_samples , block2.begin, block2_samples * sizeof(s16));

    const u32 samples_read = CBuff_Length(&read_span);

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


    const int readable_frames = SoundStream_CountReadableFrames(stream);
    SoundDebugApp_Printf(&x, &y, "Readable Audio Frames: %-6d [%-32s]", readable_frames, stars64+(64-MIN(32,readable_frames/1024)));

    const u32 writable_frames = SoundStream_CountWritableFrames(stream);
    SoundDebugApp_Printf(&x, &y, "Writable Audio Frames: %-6d [%-32s]", writable_frames, stars64+(64-MIN(32,writable_frames/1024)));
}

// Called from closebox widget and menu handler
void SoundDebugApp_Switch()
{
    t_app_sound_debug *app = &SoundDebugApp;
    app->active ^= 1;
    gui_box_show(app->box, app->active, TRUE);
    gui_menu_toggle_check (menus_ID.sound, 4);  // FIXME-UGLY
}

//-----------------------------------------------------------------------------
