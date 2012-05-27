//-----------------------------------------------------------------------------
// MEKA - sound.h
// Sound Engine - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DEFINITIONS
//-----------------------------------------------------------------------------

#define SOUND_BUFFERS_COUNT		(4)
#define SOUND_BUFFERS_SIZE		(1024)

#define SOUND_DEBUG_APPLET		(1)

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

#include "vgm.h"

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

struct t_sound
{
    // General
    bool        Enabled;
    bool        Initialized;
    int         SampleRate;                 // In Hz
    int         Paused;                     // Paused stack. Sounds play only when this is zero
    int         MasterVolume;               // Master Volume (0-128)

    // FM Emulation
    bool        FM_Enabled;					// FM Emulation enabled (emulated machine)

    // Logging
    FILE *      LogWav;
    int         LogWav_SizeData;
    char *      LogWav_FileName_Template;
    int         LogWav_ID;
    t_vgm       LogVGM;
    int         LogVGM_Logging_Accuracy;
    char *      LogVGM_FileName_Template;
    int         LogVGM_ID;

	int			CpuClock;
	s64			CycleCounter;				// Number of cycle elapsed since last sound sync.
};

struct t_sound_stream;

extern t_sound			Sound;
extern t_sound_stream*	g_psg_stream;
extern t_sound_stream*	g_ym2413_stream;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void			Sound_Init_Config();
int				Sound_Init();
void			Sound_Close();
void			Sound_UpdateClockSpeed();
void			Sound_Update();
void			Sound_SetMasterVolume(int volume);
void			Sound_ResetCycleCounter();

s64				Sound_GetElapsedCycleCounter();
double			Sound_ConvertSamplesToCycles(double samples_count);

void			Sound_Playback_Start();
void			Sound_Playback_Stop();
void			Sound_Playback_Mute();
void			Sound_Playback_Resume();

t_sound_stream*	SoundStream_Create(void (*sample_writer)(void*,int));
void			SoundStream_Destroy(t_sound_stream* stream);
void			SoundStream_Update(t_sound_stream* stream);
void			SoundStream_RenderSamples(t_sound_stream* stream, int samples_count);
void			SoundStream_RenderUpToCurrentTime(t_sound_stream* stream);
int				SoundStream_CountReadableSamples(const t_sound_stream* stream);
int				SoundStream_CountWritableSamples(const t_sound_stream* stream);
bool			SoundStream_PushSamplesRequestBufs(t_sound_stream* stream, int samples_count, s16** wbuf1, int* wbuf1_len, s16** buf2, int* wbuf2_len);
int				SoundStream_PopSamples(t_sound_stream* stream, s16* buf, int samples_wanted);

void			SoundDebugApp_Init();
void			SoundDebugApp_InstallMenuItems(int menu_parent);
void			SoundDebugApp_Update();
void			SoundDebugApp_Switch();

//-----------------------------------------------------------------------------
