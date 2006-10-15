//
// SASOUND.H
// Sound control library version 0.10
// Programmed by Hiro-shi (Hiromitsu Shioya) in 1998-1999
// Reworked on by Bock (Omar Cornut) in 2000-2001
//

#ifndef __SASOUND_H__
#define __SASOUND_H__

#include "deftypes.h"

// Omar Hack 9/04/2001
// Should be defined with default types, somewhere in MEKA includes..
#ifndef INLINE
 #ifdef WIN32
  #define INLINE __inline
 #else
  #define INLINE inline
 #endif
#endif

/* audio related stuff */
#define MAX_STREAM_CHANNELS           (16)
#define NUMVOICES                     (MAX_STREAM_CHANNELS)

#define VOLUME_MAX                    (255)

int             audio_sample_rate;
int             nominal_sample_rate;
int             change_sample_rate;

extern int      vbover_err, vbunder_err;

#define SND_CONTROL_MAX   (3)

typedef struct
{
  int  first;
  int  sound_timing;		/* 60 / sound_timing Hz */
  int  sound_timingmax;
  int  sound_count, sound_countmax;
  int  type[SND_CONTROL_MAX];
  int  (*f_init  [SND_CONTROL_MAX]) (void *userdata);
  void (*f_update[SND_CONTROL_MAX]) (void);
  void (*f_stop  [SND_CONTROL_MAX]) (void);
  void *userdata [SND_CONTROL_MAX];
  int  control_max;
} SoundRec;

typedef struct
{
  int           sync;
  int           count;
  int           type;
  int           (*f_init)   (void *userdata);
  void          (*f_update) (void);
  void          (*f_stop)   (void);
  void          *userdata;
} SoundRecEntry;

extern SoundRec      *SndMachine, snd_entry, *nowSndRec;

/**** prototype ****/

void            saUpdateSound (int nowclock);
int             saGetSoundCard (int soundcard);
BOOL            saInitSoundCard (int soundcard, int buffer_mode, int sample_rate);
void            saSetupSound (SoundRecEntry *rec);
void            saAddSound (SoundRecEntry *rec);
void            saDestroySound (void);
void            saRemoveSound (void);
void            saStopSoundEmulators (void);

int             saGetSoundRate (void); /* new!!:14/07/99 use MEKA */

void            saPlayBufferedStreamedSampleBase (int channel, signed char *data, int len, int freq, int volume, int bits, int pan);

void            saPlayStreamedSampleBase (int channel, signed char *data, int len, int freq, int volume, int bits, int pan);
void            saPlayStreamedSample16 (int channel, signed short *data, int len, int freq, int volume);
void            saPlayStreamedSample16Pan (int channel, signed short *data, int len, int freq, int volume, int pan);

void            saStopSample (t_voice *Voice);

int             saGetPlayChannels (int request);
void            saResetPlayChannels (void);

/**** streams control (base:mame 0.34b6) ****/

typedef struct
{
  //int         active;                 // Unset to disable update for this stream
  char          name[40];
  void         *buffer;
  int           buffer_len;
  int           sample_rate;
  int           sample_bits;            // Only 16 is supported now
  int           volume;
  int           buffer_pos;
  int           sample_length;          // In usec
  int           param;
  void        (*callback)(int param, void *buffer, int length);
}               t_stream;

int             streams_sh_start (void);
void            streams_sh_stop (void);
void            streams_sh_update (void);

int             stream_init (const char *name, int sample_rate, int sample_bits,int param,void (*callback)(int param, void *buffer, int length));
void            stream_update (int channel, int min_interval);   /* min_interval is in usec */
void            stream_set_volume (int channel, int volume);
int             stream_get_volume (int channel);
const char      *stream_get_name (int channel);

/**** volume/pan control (TAITO) ****/
#define  WRITE_PAN    (0x8000)

#define  VOL_PAN_MAX    (MAX_STREAM_CHANNELS)

/**** new!! sound timer system ****/
#define DEF_SOUND_BASE           (60) /* 1/60 sec */
//#define DEF_SOUND_SLICE_BASE   (300)
#define DEF_SOUND_SLICE_BASE     (1)
#define DEF_SOUND_SLICE_COUNT    (DEF_SOUND_BASE * DEF_SOUND_SLICE_BASE)

/**** work ****/
int     sound_freerun_count;
int     sound_slice;
int     sound_icount;

/**** prototype ****/
        void    saInitSoundTimer   (void);
        void    saRemoveSoundTimer (void);
        void    saSetSoundCPUClock (double (*func)(void));
        double  saGetSoundCPUTime  (void);

/****************************************************************/
/****************************************************************/

/*static*/ int  pause_sound;

SoundRec       *SndMachine, snd_entry, *nowSndRec;

/* Audio related stuff */

int             vbover_err, vbunder_err;

/*static*/ int  reserved_channel;       // voice/channel allocator
int             sound_stream_mode;
int             stream_buffer_max;
int             buffered_stream_max;

int     saCheckPlayStream (void);


#endif /* __SASOUND_H__ */

