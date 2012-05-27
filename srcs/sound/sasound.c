//
// SASOUND.C
// Sound control library version 0.10
// Programmed by Hiro-shi (Hiromitsu Shioya) in 1998-1999 for Raine/MEKA/MAME
// Reworked on by Bock (Omar Cornut) in 2000-2004
// It is still a big mess, thought :(
//

/*
  History:
  09/03/99  restart raine support.
            Added sound timer control
            Replaced stream write timing
  12/02/99  rename sample.c -> sasound.c
            support start Meka
  00->02    heavy cleaning and modifications (Omar)
*/

#include "shared.h"

t_sasound g_sasound;

int     sound_freerun_count;
int     sound_slice;

int  pause_sound;

SoundRec       *SndMachine, snd_entry, *nowSndRec;

int             vbover_err, vbunder_err;

/*static*/ int  reserved_channel;       // voice/channel allocator
int             sound_stream_mode;
int             stream_buffer_max;
int             buffered_stream_max;

/*******************************************************************************************/
/**** streams control (base:mame 0.34b6)                                                ****/
/*******************************************************************************************/

static t_stream Streams[MAX_STREAM_CHANNELS];

/*******************************************************************************************/
/**** osd_??? functions                                                                 ****/
/*******************************************************************************************/

/*
// UNUSED

void    osd_play_streamed_sample_16 (int channel, signed short *data, int len, int freq, int volume)
{
  if (!sound_stream_mode) saPlayStreamedSampleBase (channel, (signed char *)data, len, freq, volume, 16, 128);
  else                    saPlayBufferedStreamedSampleBase (channel, (signed char *)data, len, freq, volume, 16, 128);
}
*/

/*
// See new version below

void    osd_play_streamed_sample2_16 (int channel, signed short *data, int len, int freq, int volume, int pan)
{
  if (!sound_stream_mode) saPlayStreamedSampleBase (channel, (signed char *)data, len, freq, volume, 16, pan);
  else                    saPlayBufferedStreamedSampleBase (channel, (signed char *)data, len, freq, volume, 16, pan);
}
*/

INLINE void    osd_play_streamed_sample2_16 (int channel, t_stream *Stream, int pan)
{
  if (sound_stream_mode == SOUND_STREAM_NORMAL)
     saPlayStreamedSampleBase (channel, (signed char *)Stream->buffer, 2 * Stream->buffer_len, Stream->sample_rate, Stream->volume, 16, pan);
  else /* SOUND_STREAM_WAIT */
     saPlayBufferedStreamedSampleBase (channel, (signed char *)Stream->buffer, 2 * Stream->buffer_len, Stream->sample_rate, Stream->volume, 16, pan);
}

/************************************/
/*    start stream system           */
/************************************/
int     streams_sh_start (void)
{
  int   i;

  for (i = 0; i < MAX_STREAM_CHANNELS; i++)
      {
      Streams[i].buffer = NULL;
      }
  return 0;
}

/************************************/
/*    stop stream system            */
/************************************/
void    streams_sh_stop(void)
{
  int   i;

  for (i = 0; i < MAX_STREAM_CHANNELS; i++)
      {
      if (Streams[i].buffer)
         {
         free (Streams[i].buffer);
         Streams[i].buffer = NULL;
         }
      }
}

/************************************/
/*    update stream system          */
/************************************/
int     sound_scalebufferpos (int value)
{
  int result = (int)((double)value * saGetSoundCPUTime());

  // return (result);

  if (value >= 0)
     return ((result < value) ? result : value);
  else
     return ((result > value) ? result : value);
}

/************************************/
/*    update stream system          */
/************************************/
void        stream_update (int channel, int min_interval)
{
    int           newpos;
    int           buflen;
    t_stream     *cStream = &Streams[channel];

    if (cStream->buffer == NULL)
        return;

    // Get current position based on the timer
    newpos = sound_scalebufferpos (cStream->buffer_len);
    buflen = newpos - cStream->buffer_pos;
    //if (buflen < 0)
    //Msg (MSGT_DEBUG, "pos:%d newpos:%d len:%d buflen:%d\n", cStream->buffer_pos, newpos, buflen, cStream->buffer_len);

    // Fill samples
    if (buflen * cStream->sample_length > min_interval)
    {
        void    *buf;
        buf = &((short *)cStream->buffer)[cStream->buffer_pos];
        (*cStream->callback)(cStream->param, buf, buflen);
        cStream->buffer_pos += buflen;
    }
}

/************************************/
/*    update stream system          */
/************************************/
void    streams_slice_update (void)
{
    int   channel;

    for (channel = 0; channel < MAX_STREAM_CHANNELS; channel ++)
        stream_update (channel, 0);
}

/************************************/
/*    update stream system          */
/************************************/
void    streams_sh_update (void)
{
    int   channel;

    if (g_sasound.audio_sample_rate == 0)
        return;

// OLD

    // Update all the output buffers
    for (channel = 0; channel < MAX_STREAM_CHANNELS; channel ++)
    {
        t_stream *cStream = &Streams[channel];
        int       newpos  = cStream->buffer_len;
        int       buflen  = newpos - cStream->buffer_pos;

        if (cStream->buffer == NULL)
            continue;

        // Fill samples up to end of frame
        // FIXME: this is where the sample bug is!!!
        // Msg (MSGT_DEBUG, "completing by %d samples", buflen);
        if (buflen > 0)
        {
            void *buf;
            buf = &((short *)cStream->buffer)[cStream->buffer_pos];
            (*cStream->callback)(cStream->param, buf, buflen);
            // memset (buf, 0xff, buflen * sizeof (short));
        }
        cStream->buffer_pos = 0;
        sound_icount = 0;
        //Sound_Update_Count = 0;
    }

// NEW

    // ...
    // Msg (MSGT_DEBUG, "CycleCounter: %d", Sound_CycleCounter);

    // Reset sound cycle counter
    Sound.CycleCounter = 0;

    for (channel = 0; channel < MAX_STREAM_CHANNELS; channel ++)
    {
        t_stream *cStream = &Streams[channel];
        if (cStream->buffer)
            osd_play_streamed_sample2_16 (channel, cStream, 128); // 128=center panning
    }
}

/************************************/
/*    stream init (1 channel)       */
/************************************/
int             stream_init (const char *name, int sample_rate, int sample_bits, int param, void (*callback)(int param, void *buffer, int length))
{
  int           channel;
  t_stream *    Stream;

  // The engine does not support 8-bit playback anymore
  if (sample_bits != 16)
     return (-1);

  channel = saGetPlayChannels(1);
  Stream = &Streams[channel];

  strcpy (Stream->name, name);
  // Buffer is sized for one 1/60th of sound
  Stream->buffer_len = sample_rate / DEF_SOUND_BASE;
  // Re-adjust sample rate to make it a multiple of buffer_len
  sample_rate = Stream->buffer_len * DEF_SOUND_BASE;

  // FIXME: The * 2 is because this lameass code sometimes play after the buffer
  // due to timing problem. This isn't a fix but at least limit noises to gap.
  // The regular allocation and usage size doesn't need this *2
  if ((Stream->buffer = malloc((sample_bits / 8) * Stream->buffer_len * 2)) == 0)
     return (-1);
  memset(Stream->buffer, 0, (sample_bits / 8) * Stream->buffer_len * 2);

  Stream->sample_rate = sample_rate;
  Stream->sample_bits = sample_bits;
  Stream->volume = VOLUME_MAX;
  Stream->buffer_pos = 0;
  if (sample_rate)
     Stream->sample_length = 1000000 / sample_rate;
  else
     Stream->sample_length = 0;
  Stream->param = param;
  Stream->callback = callback;

  return (channel);
}

/************************************/
/*    stream set volume             */
/************************************/
void    stream_set_volume(int channel, int volume)
{
    /* backwards compatibility with old 0-255 volume range */
    //if (volume > 100) volume = volume * 50 / 255;
    Streams[channel].volume = volume;
}

/************************************/
/*    stream get colume             */
/************************************/
int     stream_get_volume(int channel)
{
    return Streams[channel].volume;
}

/************************************/
/*    stream get volume             */
/************************************/
INLINE const char *stream_get_name(int channel)
{
    if (Streams[channel].buffer)
        return Streams[channel].name;
    return NULL; // unused channel
}

/*******************************************************************************************/
/*  sa???Sound                                                                             */
/*******************************************************************************************/

/******************************************/
/******************************************/
int     saGetSoundRate (void)
{
    return (g_sasound.audio_sample_rate);
}

/*********************************************************************/
/* stop sound emulators: this is needed for when sample rate changes */
/*********************************************************************/
void    saStopSoundEmulators (void)
{
    int   i;

#ifdef INSTALL_SOUND_TIMER
    saRemoveSoundTimer();
#endif

    saResetPlayChannels();
    if (SndMachine == NULL)
    {
        SndMachine = &snd_entry;
    }
    else
    {
        streams_sh_stop();
        for (i = 0; i < SndMachine->control_max; i++)
            if (SndMachine->f_stop[i] != NULL)
                SndMachine->f_stop[i]();
        SndMachine->first           = 0;
        SndMachine->sound_timing    = 0;
        SndMachine->sound_count     = 0;
    }
}

/******************************************/
/*    setup sound                         */
/******************************************/
void    saSetupSound (SoundRecEntry *rec)
{
  int   i;

  saResetPlayChannels();
  if (rec == NULL)
     return;

  if (SndMachine == NULL)
     {
     SndMachine = &snd_entry;
     }
  else
     {
     streams_sh_stop();
     for (i = 0; i < SndMachine->control_max; i++)
        if (SndMachine->f_stop[i] != NULL)
           SndMachine->f_stop[i]();
     }
  /**** init sound control work ****/
  for (i = 0; i < SND_CONTROL_MAX; i++)
      {
      SndMachine->type[i]      = -1;
      SndMachine->f_init[i]    = NULL;
      SndMachine->f_update[i]  = NULL;
      SndMachine->f_stop[i]    = NULL;
      SndMachine->userdata[i]  = NULL;
     }
  // FIXME: weird.. why stopfunc isn't filled there ?
  SndMachine->sound_timing    = 0;
  SndMachine->sound_count     = 0;
  SndMachine->sound_countmax  = rec->count;
  SndMachine->sound_timingmax = rec->sync / SndMachine->sound_countmax;
  SndMachine->type[0]         = rec->type;
  SndMachine->f_init[0]       = rec->f_init;
  SndMachine->f_update[0]     = rec->f_update;
  SndMachine->f_stop[0]       = rec->f_stop;
  SndMachine->userdata[0]     = rec->userdata;
  SndMachine->first           = 0;
  SndMachine->control_max     = 1;
}

/******************************************/
/*    add sound                           */
/******************************************/
void    saAddSound (SoundRecEntry *rec)
{
  int   i;

  if (SndMachine->control_max >= SND_CONTROL_MAX)
     return;
  i = SndMachine->control_max;
  SndMachine->type[i]      = rec->type;
  SndMachine->f_init[i]    = rec->f_init;
  SndMachine->f_update[i]  = rec->f_update;
  SndMachine->f_stop[i]    = rec->f_stop;
  SndMachine->userdata[i]  = rec->userdata;
  SndMachine->control_max++;
}

/******************************************/
/*    destroy 1ch                         */
/******************************************/
void            saDestroyChannel (int ch)
// BOCK 07/24/01: shouldn't be called saDestroyVoice ?
{
    t_voice *   voice = &Sound.Voices[ch];

    if (voice->lpWave)
    {
        int i;
        free (voice->vstreambuf);
        for (i = 0; i < stream_buffer_max; i++)
            voice->vstreambuf_chunk_ready[i] = 0;
        AStopVoice (voice->hVoice);
        ADestroyAudioData (voice->lpWave);
        free (voice->lpWave);
        voice->lpWave = NULL;
        voice->playing = FALSE;
        free (voice->vpan);
        voice->vpan = NULL;
    }
}

/******************************************/
/*    destroy sound                       */
/******************************************/
void    saDestroySound (void)
{
  int   i;

#ifdef INSTALL_SOUND_TIMER
  saRemoveSoundTimer();
#endif
  for (i = 0; i < NUMVOICES; i++)
      {
      saDestroyChannel (i);
      ADestroyAudioVoice (Sound.Voices[i].hVoice);
      }
  ACloseVoices();
  ACloseAudio();
}

/******************************************/
/*    setup sound                         */
/******************************************/
void    saRemoveSound (void)
{
  int   i;

  saDestroySound ();
  saResetPlayChannels ();
  if (SndMachine == NULL)
     {
     SndMachine = &snd_entry;
     }
  else
     {
     streams_sh_stop();
     for (i = 0; i < SndMachine->control_max; i++)
         if (SndMachine->f_stop[i] != NULL)
             SndMachine->f_stop[i]();
     }
}

/******************************************/
/*    update sound                        */
/******************************************/
void    saUpdateSound (int nowclock)
{
  int   i;

  if (g_sasound.audio_sample_rate == 0) return;
  if (!SndMachine || !SndMachine->first) return; /* not sound initialize end */

  if (nowclock)
     {
     if (pause_sound == 0)
        {
        // Msg (MSGT_DEBUG, "update");
        streams_sh_update();
        for (i = 0; i < SndMachine->control_max; i++)
 	   if (SndMachine->f_update[i] != NULL)
 	       SndMachine->f_update[i]();
        }
     }
  else
     {
     if (sound_stream_mode == SOUND_STREAM_WAIT) /* MEKA */
        {
#ifndef ARCH_WIN32
        AUpdateAudio();
#endif
        saCheckPlayStream();
        }
     }
}

/******************************************/
/*    buffer check                        */
/******************************************/
int         saCheckPlayStream (void)
{
    int     i;
    DWORD   pos[NUMVOICES];
    t_voice *Voice;

    if (Sound.Paused > 0)
    {
        if (pause_sound == FALSE)
        {
            // Msg (MSGT_DEBUG, "destroying sound");
            pause_sound = TRUE;
            vbover_err = vbunder_err = 0; /* error initial */
            for (i = NUMVOICES - 1; i >= 0; i--)
                saDestroyChannel /*Voice*/ (i);
        }
        return (0);
    }

    pause_sound = FALSE;

    // Get current Position
    for (i = 0; i < NUMVOICES; i++)
    {
        Voice = &Sound.Voices[i];
        if (!Voice->playing)
            continue;
        AGetVoicePosition (Voice->hVoice, (LPLONG)&pos[i]);
    }

    // Check update position
    for (i = 0; i < NUMVOICES; i++)
    {
        Voice = &Sound.Voices[i];
        if (!Voice->playing)
            continue;

        // Checking if playback has gone too far
        if ((Voice->vchan - Voice->ventry) < 0)
        {
            vbunder_err++;
            #ifdef MEKA_SOUND
                Msg (MSGT_DEBUG, "Sound buffer under-run (ve:%08x, vc:%08x, verr:%08x)", Voice->ventry, Voice->vchan, vbunder_err);
            #endif
        }

        else // Update
        if ((Voice->vchan - Voice->ventry) > 0)
        {
            /**** buffer update check ****/
            int len = Voice->vlen;
            int ve  = Voice->ventry;
            int slens = len * (ve % stream_buffer_max);
            int rlens = len * (ve % buffered_stream_max);
            int rlene = rlens + len;
            #ifdef MEKA_SOUND
                #if 0
                    Msg (MSGT_DEBUG, "%d %d %d", pos[i], rlens, rlene);
                #endif
            #endif
            #if 0
                printf("%d(%d) %d (%d,%d{%d}) [%d]\n", pos[i] / len, Voice->pos,
                    (sound_freerun_count - Voice->vruncount) % stream_buffer_max,
                    rlens, rlene, (rlens - pos[i]) / len, (Voice->vchan - Voice->ventry) );
            #endif

            if ((int)pos[i] < rlens || (int)pos[i] >= rlene)
            {
                /**** copy stream buffer -> PCM buffer ****/
                slens *= Voice->vbits;
                rlens *= Voice->vbits;
                len *= Voice->vbits;

                ve %= stream_buffer_max;
                //if (!Voice->vstreambuf_chunk_ready[ve])
                //    Msg (MSGT_DEBUG, "Update wave, but chunk %d is not ready!", ve);
                memcpy (&Voice->lpWave->lpData[rlens], Voice->vstreambuf + slens, len);

                // Note: in original code, the call to AWriteAudioData() was commented here
                // AWriteAudioData (Voice->lpWave, slens, len);
                // Omar:
                //AWriteAudioData (Voice->lpWave, rlens, len);

                // Update panning
                if (Voice->vpan[ve] & WRITE_PAN)
                    ASetVoicePanning (Voice->hVoice, (UINT)Voice->vpan[ve] & 0xff);

                // Clear ready flag once we consumed the data
                Voice->vstreambuf_chunk_ready[ve] = 0;

                Voice->ventry++;

                /**** restart check ****/
                if (Voice->vrestart)
                {
                    Voice->vrestart = 0;
                    APlayVoice (Voice->hVoice, Voice->lpWave);
                }

                // Reset error counters
                vbover_err = vbunder_err = 0;
            }
            else
            {
// It means that we came too early... it is not a problem, just wait for the next time
#if 1
                vbover_err++;		/* error count */
                #if 0
                    Msg (MSGT_DEBUG, "ve:%08x, vc:%08x, verr:%08x", Voice->ventry, Voice->vchan, vbover_err);
                #endif
                if (vbover_err >= MODEB_ERROR_MAX)
                {
                    #ifdef MEKA_SOUND
                    if (g_env.state == MEKA_STATE_INIT)
                        printf ("%s\n", Msg_Get (MSG_Sound_Stream_Error));
                    else
                        Msg (MSGT_DEBUG, Msg_Get (MSG_Sound_Stream_Error));
                    #endif

                    /**** all buffer restart ****/
                    vbover_err = vbunder_err = 0;
                    // BOCK Note 07/24/2001:
                    // I'm a bit septic toward this code, since it modify 'i'
                    // It is intentionnaly done, then to restart the outer loop
                    // as 'i' ends with zero here ?
                    for (i = NUMVOICES - 1; i >= 0; i--)
                    {
                        Voice = &Sound.Voices[i];
                        #if 0
                            AStopVoice (Voice->hVoice);
                            Voice->ventry = 0;
                            Voice->vchan = MODEB_UPDATE_COUNT;
                            Voice->vrestart = 1;
                        #else
                            // Msg (MSGT_DEBUG, "Destroying channel %d", i);
                            saDestroyChannel (i);
                        #endif
                    }
                } /**** error max check ****/
#endif
            } /**** pos check end ****/
        }
    } /**** loop end ****/
    return (0);
}

/*******************************************************************************************/
/*******************************************************************************************/
/******************************************/
/*    play samples                        */
/******************************************/
void            saPlayBufferedStreamedSampleBase (int channel, signed char *data, int len, int freq, int volume, int bits, int pan)
{
    int         i;
    t_voice *   voice;

    if (g_sasound.audio_sample_rate == 0 || channel >= NUMVOICES || SndMachine == NULL)
        return;

    voice = &Sound.Voices[channel];
    if (voice->playing)
    {
        // Copy new samples to sound buffer
        int s_pos = voice->vchan % stream_buffer_max;
        memcpy (voice->vstreambuf + (len * s_pos), data, len);  // Copying samples data
        voice->vpan[s_pos] = pan | WRITE_PAN;
        voice->vstreambuf_chunk_ready[s_pos] = 1;               // Set ready flag
        //if (g_env.state != MEKA_STATE_INIT)
        //Msg (MSGT_DEBUG, "chunk_ready %d", s_pos);
        voice->vchan++;
    }
    else
    {
        // Msg (MSGT_DEBUG, "saPlayBufferedStreamedSampleBase() - reallocating voice %d", channel);

        // Destroy previous voice (if there's one)
        if (voice->lpWave)
            saStopSample (voice);

        // Reallocate
        if ((voice->lpWave = (LPAUDIOWAVE)malloc (sizeof (AUDIOWAVE))) == NULL)
            return;
        if ((voice->vstreambuf = (LPBYTE)malloc (stream_buffer_max * len)) == NULL)
        {
            free (voice->lpWave);
            voice->lpWave = NULL;
            return;
        }
        if ((voice->vpan = (int *)malloc (sizeof(int) * stream_buffer_max)) == NULL)
        {
            free (voice->lpWave);
            free (voice->vstreambuf);
            voice->lpWave = NULL;
            return;
        }
        voice->lpWave->wFormat     = AUDIO_FORMAT_16BITS | AUDIO_FORMAT_STEREO | AUDIO_FORMAT_LOOP;
        voice->lpWave->nSampleRate = g_sasound.nominal_sample_rate;
        voice->lpWave->dwLength    = buffered_stream_max*len;
        voice->lpWave->dwLoopStart = 0;
        voice->lpWave->dwLoopEnd   = voice->lpWave->dwLength;
        if (ACreateAudioData(voice->lpWave) != AUDIO_ERROR_NONE)
        {
            // Failed
            free (voice->lpWave);
            free (voice->vpan);
            free (voice->vstreambuf);
            voice->lpWave = NULL;
            voice->vpan = NULL;
            return;
        }
        // Clear samples
        memset (voice->lpWave->lpData, 0, voice->lpWave->dwLength);
        memset (voice->vstreambuf, 0, stream_buffer_max * len);
        APrimeVoice (voice->hVoice, voice->lpWave);
        ASetVoiceFrequency (voice->hVoice, (int)((double)freq * g_sasound.nominal_sample_rate / g_sasound.audio_sample_rate));
        ASetVoiceVolume (voice->hVoice, (Sound.MasterVolume * volume) / 512);
        ASetVoicePanning (voice->hVoice, (UINT)pan);
        voice->playing = TRUE;      /* use front surface */
        /**** make sound temp. buffer ****/
        voice->vchan = MODEB_UPDATE_COUNT;

        memcpy (voice->vstreambuf + len * voice->vchan, data, len);
        for (i = 0; i <= voice->vchan; i++)
            voice->vpan[i] = pan | WRITE_PAN;
        voice->ventry = voice->vchan;
        voice->vchan++;
        voice->vrestart = 0;
        voice->vbits = bits / 8;
        voice->vlen = len / voice->vbits;
        voice->vruncount = sound_freerun_count;

        // FIXME
        voice->vstreambuf_chunk_ready[0] = 0;               // Set ready flag
        voice->vstreambuf_chunk_ready[1] = 1;               // Set ready flag, current is vchan(4)%3 == 1
        voice->vstreambuf_chunk_ready[2] = 0;               // Set ready flag

        // Write audio data
        AWriteAudioData (voice->lpWave, 0, voice->lpWave->dwLength);

        // Play
        AStartVoice (voice->hVoice);
    }
}

/******************************************/
/*    play samples                        */
/******************************************/
void            saPlayStreamedSampleBase (int channel, signed char *data, int len, int freq, int volume, int bits, int pan)
{
    t_voice *   voice;

    Msg (MSGT_DEBUG, "saPlayStreamedSampleBase()");
    if (g_sasound.audio_sample_rate == 0 || channel >= NUMVOICES || SndMachine == NULL)
        return;

    voice = &Sound.Voices[channel];
    if (voice->playing)
    {
        DWORD pos;
#if 0
        int nownum = voice->vchan % stream_buffer_max;
        for (;;)
        {
            AUpdateAudio();
            AGetVoicePosition(voice->hVoice, &pos);

            if (!nownum && pos >= len)
                break;
            if (nownum == (stream_buffer_max - 1))
            {
                if (pos < (len * nownum))
                    break;
            }
            else
            {
                if ((pos < len * (nownum) || pos >= (len * (nownum + 1))))
                    break;
            }
            /*
            if ((Voice->vchan % STREAM_BUFFER_MAXA) == 0 && pos >= len) break;
            if ((Voice->vchan % STREAM_BUFFER_MAXA) == 1 && (pos < len || pos >= (2*len))) break;
            if ((Voice->vchan % STREAM_BUFFER_MAXA) == 2 && pos < (2*len)) break;
            */
        }
#else
        AGetVoicePosition(voice->hVoice, (LPLONG)&pos);
        // AUpdateAudio();
#endif

        memcpy (&voice->lpWave->lpData[len * (voice->vchan % stream_buffer_max)], data, len);
        // AWriteAudioData (Voice->lpWave, len * (Voice->vchan % stream_buffer_max), len);
        voice->vchan++;
    }
    else
    {
        // Reallocate the voice if it has been destroyed
        // Msg (MSGT_DEBUG, "saPlayStreamedSampleBase() - reallocating voice %d", channel);

        if (voice->lpWave)
            saStopSample (voice);
        if ((voice->lpWave = (LPAUDIOWAVE)malloc(sizeof (AUDIOWAVE))) == NULL)
            return;

        voice->lpWave->wFormat = AUDIO_FORMAT_16BITS | AUDIO_FORMAT_STEREO | AUDIO_FORMAT_LOOP;
        voice->lpWave->nSampleRate = g_sasound.nominal_sample_rate;
        voice->lpWave->dwLength = stream_buffer_max * len;
        voice->lpWave->dwLoopStart = 0;
        voice->lpWave->dwLoopEnd = stream_buffer_max * len;
        if (ACreateAudioData(voice->lpWave) != AUDIO_ERROR_NONE)
        {
            free (voice->lpWave);
            voice->lpWave = NULL;
            return;
        }
        memset(voice->lpWave->lpData, 0, stream_buffer_max * len);
        memcpy(voice->lpWave->lpData, data, len);
        /* upload the data to the audio DRAM local memory */
        APrimeVoice(voice->hVoice, voice->lpWave);
        ASetVoiceFrequency(voice->hVoice, (int)((double)freq * g_sasound.nominal_sample_rate / g_sasound.audio_sample_rate));
        ASetVoiceVolume(voice->hVoice, (Sound.MasterVolume * volume) / 512);
        ASetVoicePanning(voice->hVoice, (UINT)pan);
        voice->playing = TRUE;      /* use front surface */
        voice->vchan = 1;
        voice->ventry = 1;
        AStartVoice (voice->hVoice);
    }
}

/******************************************/
/*    stop samples                        */
/******************************************/
void            saStopSample(t_voice *Voice)
{
    AStopVoice (Voice->hVoice);
    ADestroyAudioData (Voice->lpWave);
    free (Voice->lpWave);
    Voice->lpWave = NULL;
}

/*******************************************************************************************/
/*******************************************************************************************/
/******************************************/
/*    get play channel                    */
/******************************************/
int     saGetPlayChannels (int request)
{
    int ret_value = reserved_channel;
    reserved_channel += request;
    return ret_value;
}

void    saResetPlayChannels (void)
{
    reserved_channel = 0;
}

/*******************************************************************************************/
/****    sound timer system                                                             ****/
/*******************************************************************************************/
/************************************/
/*    sound timer callback          */
/************************************/
void    saSoundTimerCallback (void)
{
  sound_slice++;
  if (sound_stream_mode == SOUND_STREAM_WAIT)
     {
     // Note: DEF_SOUND_SLICE_BASE is currently defined as 1
     if (sound_slice >= DEF_SOUND_SLICE_BASE)
        {
        sound_slice = 0;
        //sound_icount = 0;		/* sound interval counter clear */
        saUpdateSound (0);              /* check update stream buffer */
        sound_freerun_count++;
        saUpdateSound (60);
        }
     }
  else
     {
     saUpdateSound (60);                /* default callback */
     }
}
//END_OF_FUNCTION (saSoundTimerCallback);

/************************************/
/*    install timer                 */
/************************************/
void    saInitSoundTimer (void)
{
#if 0 // FIXME-ALLEGRO5: sound timers
  LOCK_VARIABLE (sound_icount);
  LOCK_VARIABLE (sound_freerun_count);
  LOCK_VARIABLE (sound_slice);
  LOCK_FUNCTION (saSoundTimerCallback);
  install_int_ex (saSoundTimerCallback, BPS_TO_TIMER(DEF_SOUND_SLICE_COUNT));
#endif
  sound_freerun_count = 0;
  sound_slice = 0;
  sound_icount = 0;
}

/************************************/
/*    remove timer                  */
/************************************/
void    saRemoveSoundTimer (void)
{
#if 0 // FIXME-ALLEGRO5: sound timers
  remove_int (saSoundTimerCallback);
#endif
}

/*******************************************************************************************/
/****    stream update system                                                           ****/
/*******************************************************************************************/
/*******************************************/
/*    set Sound CPU base time (1/60sec)    */
/*******************************************/
static double (*calc_time)(void);
void    saSetSoundCPUClock (double (*func)(void))
{
    calc_time = func;
}
/*******************************************/
/*    get Sound CPU time                   */
/*******************************************/
double  saGetSoundCPUTime (void)
{
    return (calc_time != NULL) ? (double)calc_time() : (double)0;
}

/* EOF */

