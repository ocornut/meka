//-----------------------------------------------------------------------------
// MEKA - sound.h
// Sound Engine - Headers
//-----------------------------------------------------------------------------
// Hiromitsu Shioya, 1998-1999
// Omar Cornut, 1999+
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

// Include SEAL library headers
#ifndef UNIX
#include "audio.h"
#else
// Uncomment the appropriate:
#include "audio.h"      // Official SEAL distribute name
// #include "seal.h"    // Debian package header file was renamed to seal.h
// FIXME: Need to automate this of use some Makefile trickery to detect.
#endif

//-----------------------------------------------------------------------------
// DEFINITIONS
//-----------------------------------------------------------------------------

#define SOUND_SOUNDCARD_SELECT  (-1)
#define SOUND_SOUNDCARD_NONE    (0)
#define SOUND_VOICES_MAX        (16)

#define SOUND_MACHINE_SN76496   (0)
#define SOUND_MACHINE_YM2413HD  (1)
#define SOUND_MACHINE_EMU2413   (2)

#define FM_EMULATOR_NONE        (0x00)
#define FM_EMULATOR_YM2413HD    (0x01)  // Hiromitsu Shioya / through OPL
#define FM_EMULATOR_EMU2413     (0x02)  // Mitsutaka Okazaki / digital

//-----------------------------------------------------------------------------
// OPL Access
//-----------------------------------------------------------------------------

#ifndef UNIX
#define MEKA_OPL
#endif

#ifdef MEKA_OPL
#include "s_opl.h"                      // S_OPL.H      OPL
#endif

//-----------------------------------------------------------------------------
// INCLUDES (more. note the unordered mess)
//-----------------------------------------------------------------------------

#include "vgm.h"                // VGM.H : for VGM file creation
#include "wav.h"                // WAV.H : for WAV file creation

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

// SaSound voice (legacy)
typedef struct
{
    HAC         hVoice;         // SEAL voice handler
    LPAUDIOWAVE lpWave;
    LPBYTE      vstreambuf;     // use bufferedstream mode: stock buffer
    int         vstreambuf_chunk_ready[3];
    int         playing;        // YES/NO
    // Variables below are unclear...
    int         vbits;
    int         vchan;          // use bufferedstream mode : write butter counter
    int         ventry;         // use bufferedstream mode : entry SEAL buffer counter
    int         vlen;           // buffer length
    int         vruncount;      // use bufferedstream mode : stock "sound_freerun_count"
    int *       vpan;           // Pan buffer
    int         vrestart;       // Error restart flag
}               t_voice;

// t_sound, hold all variables/configurations stuff for the sound part
typedef struct
{
    // General
    int         Enabled;
    int         Initialized;
    int         SoundCard;                      // Seal SoundCard ID
    int         SampleRate;                     // In Hz
    int         Paused;                         // Paused stack. Sounds play only when this is zero
    int         MasterVolume;                   // Master Volume (0-128)

    // FM Emulation
    int         FM_Enabled;                     // FM Emulation enabled (emulated machine)
    int         FM_Emulator_Current;            // FM Emulators, currently used (host machine)
    int         FM_Emulator_Available;          // FM Emulators, mask of available type(s), zero if none

    // OPL
#ifdef MEKA_OPL
    int         OPL_Speed;
    int         OPL_Address;                    // 220h, 230h, etc... 0 if none
#endif

    // Voices (SaSound legacy...)
    t_voice *   Voices;
    int         Voices_Max;

    // Logging
    FILE *      LogWav;
    int         LogWav_SizeData;
    char *      LogWav_FileName_Template;
    int         LogWav_ID;
    t_vgm       LogVGM;
    int         LogVGM_Logging_Accuracy;
    char *      LogVGM_FileName_Template;
    int         LogVGM_ID;
}               t_sound;

t_sound         Sound;
int             Sound_CycleCounter;         // Number of cycle elapsed since last sound sync.

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void    Sound_Init_Config       (void);     // Initialize sound structure with its default values
int     Sound_Init_SEAL         (void);     // Initialize SEAL
int     Sound_Init              (void);     // Initialize sound engine
void    Sound_Close             (void);     // Close sound engine

void    Sound_Update_Frame      (void);     // Miscellaneous things to do on each frame

void    Sound_Playback_Start    (void);     // Start sound playback
void    Sound_Playback_Stop     (void);     // Stop sound playback

void    Sound_Playback_Mute     (void);     // Mute sound playback
void    Sound_Playback_Resume   (void);     // Resume sound playback

void    Sound_MasterVolume_Set  (int v);    // Change Master Volume

//-----------------------------------------------------------------------------
// All ununderstandable stuff from old engine
//-----------------------------------------------------------------------------

#define INSTALL_SOUND_TIMER
#define INSTALL_CALC_SOUNDCPU_CLOCK

//#define DEF_STREAM_BUFFER_MAXA        (3)
#define DEF_MODEB_FRAME_SIZE            (60)
#define DEF_MODEB_UPDATE_COUNT          (3)
#define DEF_STREAM_BUFFER_MAXB          ((DEF_FRAME_SIZE / DEF_MODEB_UPDATE_COUNT))
#define DEF_STREAM_UPDATE_ERROR_MAX     (16) // error wait is 16 seconds.

int     STREAM_BUFFER_MAXA;
int     STREAM_BUFFER_MAXB;
int     MODEB_UPDATE_COUNT;
int     MODEB_FRAME_SIZE;
int     MODEB_MASK;
int     MODEB_ERROR_MAX;

int     sound_stream_mode;

#define SOUND_STREAM_NORMAL             (0)
#define SOUND_STREAM_WAIT               (1)

//-----------------------------------------------------------------------------
// INCLUDES (even more. note the unordered mess)
//-----------------------------------------------------------------------------

#include "s_log.h"                      // S_LOG.H      Sound logging
#include "s_misc.h"                     // S_MISC.H     Miscellaenous

#include "psg.h"                        // PSG.H        PSG SN-76496 emulator

#include "fmunit.h"                     // FMUNIT.H     FM Unit wrapper to emulators
#include "fmeditor.h"	                // FMEDITOR.H   FM instrument editor applet
#include "ym2413hd.h"                   // YM2413HD.H   FM emulator / OPL
#include "emu2413/mekaintf.h"           // EMU2413.H... FM emulator / Digital
#include "sasound.h"                    // SASOUND.H    Sound system (by Hiroshi)

//-----------------------------------------------------------------------------


