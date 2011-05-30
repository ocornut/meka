//-----------------------------------------------------------------------------
// MEKA - sound.h
// Sound Engine - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DEFINITIONS
//-----------------------------------------------------------------------------

#define SOUND_SOUNDCARD_SELECT  (-1)
#define SOUND_SOUNDCARD_NONE    (0)

#define SOUND_BUFFERS_COUNT		(4)
#define SOUND_BUFFERS_SIZE		(2048)

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

#include "vgm.h"                // VGM.H : for VGM file creation
#include "wav.h"                // WAV.H : for WAV file creation

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

struct t_sound
{
    // General
    int         Enabled;
    int         Initialized;
    int         SoundCard;                      // Soundcard driver
    int         SampleRate;                     // In Hz
    int         Paused;                         // Paused stack. Sounds play only when this is zero
    int         MasterVolume;                   // Master Volume (0-128)

    // FM Emulation
    int         FM_Enabled;                     // FM Emulation enabled (emulated machine)

    // Logging
    FILE *      LogWav;
    int         LogWav_SizeData;
    char *      LogWav_FileName_Template;
    int         LogWav_ID;
    t_vgm       LogVGM;
    int         LogVGM_Logging_Accuracy;
    char *      LogVGM_FileName_Template;
    int         LogVGM_ID;

	int			CycleCounter;				// Number of cycle elapsed since last sound sync.
};

extern t_sound  Sound;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void    Sound_Init_Config       (void);     // Initialize sound structure with its default values
int     Sound_Init              (void);     // Initialize sound engine
void    Sound_Close             (void);     // Close sound engine

void    Sound_Update			(void);

void    Sound_Playback_Start    (void);     // Start sound playback
void    Sound_Playback_Stop     (void);     // Stop sound playback

void    Sound_Playback_Mute     (void);     // Mute sound playback
void    Sound_Playback_Resume   (void);     // Resume sound playback

void    Sound_MasterVolume_Set  (int v);    // Change Master Volume

//-----------------------------------------------------------------------------
// INCLUDES (even more. note the unordered mess)
//-----------------------------------------------------------------------------

#include "s_log.h"                      // S_LOG.H      Sound logging
#include "s_misc.h"                     // S_MISC.H     Miscellaenous

#include "psg.h"                        // PSG.H        PSG SN-76496 emulator

#include "fmunit.h"                     // FMUNIT.H     FM Unit wrapper to emulators
#include "fmeditor.h"	                // FMEDITOR.H   FM instrument editor applet
#include "emu2413/mekaintf.h"           // EMU2413.H... FM emulator / Digital

//-----------------------------------------------------------------------------


