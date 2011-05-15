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

static  int     Sound_Init_SoundCard (void);
#ifdef MEKA_OPL
static  int     Sound_Init_OPL (void);
#endif
static  int     Sound_Init_Engine (int buffer_mode);
static  void    Sound_Init_Emulators (void);

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

t_sound		Sound;

int STREAM_BUFFER_MAXA;
int STREAM_BUFFER_MAXB;
int MODEB_UPDATE_COUNT;
int MODEB_FRAME_SIZE;
int MODEB_ERROR_MAX;
int MODEB_MASK;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sound_Init_Config(void)
// Initialize sound structure with its default values
//-----------------------------------------------------------------------------
void            Sound_Init_Config(void)
{
    // General
    Sound.Enabled               = TRUE;
    Sound.Initialized           = FALSE;
    Sound.SoundCard             = SOUND_SOUNDCARD_SELECT; // Let user select by default
    Sound.SampleRate            = 44100;                  // 44100 Hz by default
    Sound.Paused                = FALSE; // 0
    Sound.MasterVolume          = 128;

    // FM Emulation
    Sound.FM_Emulator_Current   = FM_EMULATOR_NONE;
    Sound.FM_Emulator_Available = FM_EMULATOR_NONE;

    // OPL
#ifdef MEKA_OPL
    Sound.OPL_Speed             = 4;
    Sound.OPL_Address           = 0x000;
#endif

    // Voices & and other legacy stuff
    Sound.Voices        = NULL;
    Sound.Voices_Max    = SOUND_VOICES_MAX;
    SndMachine          = NULL;
    reserved_channel    = 0;
    sound_stream_mode   = SOUND_STREAM_WAIT;

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

    // Initialize SEAL
    Sound_Init_SEAL();

    // Disable sound if user selected 'no soundcard'
    if (Sound.SoundCard == SOUND_SOUNDCARD_NONE)
    {
        // Sound.Enabled = FALSE;
        return (MEKA_ERR_OK);
    }

    // Initialize Sound Card, SEAL side
    // Start in pause mode, to avoid sound update on startup (could crash, before everything is initialized)
    Sound.Paused = TRUE;
    Sound_Init_SoundCard();

    // Initialize OPL (if available)
#ifdef MEKA_OPL
    Sound_Init_OPL();
#endif

    // Initialize Sound emulators
    Sound_Init_Emulators();

    // Initialize Sound Engine (SEAL)
    if (Sound_Init_Engine(SOUND_STREAM_WAIT) != MEKA_ERR_OK)
        Quit ();

    // FIXME: CRAP! Legacy stuff again
    Sound.SampleRate = saGetSoundRate ();
    sound_icount = 0;
    Sound_Update_Count = 0;
    saSetSoundCPUClock (Sound_Calc_CPU_Time);
    // fm_delay_size = 6; // 0

    // Setup checks in GUI
    // Note: this GUI sucks :(
    Sound.FM_Emulator_Current &= Sound.FM_Emulator_Available;
    gui_menu_un_check_area (menus_ID.fm_emu, 0, 1);
    gui_menu_active_area (FALSE, menus_ID.fm_emu, 0, 1);

    // Select FM emulator
    if (Sound.FM_Emulator_Available & FM_EMULATOR_YM2413HD)
    {
        gui_menu_active (AM_Active, menus_ID.fm_emu, 0);
        gui_menu_active (AM_Active, menus_ID.fm, 3);
        if (Sound.FM_Emulator_Current == FM_EMULATOR_NONE)
            Sound.FM_Emulator_Current = FM_EMULATOR_YM2413HD;
    }
    if (Sound.FM_Emulator_Available & FM_EMULATOR_EMU2413)
    {
        gui_menu_active (AM_Active, menus_ID.fm_emu, 1);
        if (Sound.FM_Emulator_Current == FM_EMULATOR_NONE)
            Sound.FM_Emulator_Current = FM_EMULATOR_EMU2413;
    }

    // Activate current FM emulator
    switch (Sound.FM_Emulator_Current)
    {
    #ifdef MEKA_OPL
     case FM_EMULATOR_YM2413HD:
         FM_OPL_Active ();
         gui_menu_check (menus_ID.fm_emu, 0);
         break;
    #endif
     case FM_EMULATOR_EMU2413:
         FM_Digital_Active ();
         gui_menu_check (menus_ID.fm_emu, 1);
         break;
     default:
         FM_Null_Active ();
         break;
    }

    // Ok
    Sound.Initialized = TRUE;
    return (MEKA_ERR_OK);
}

// Initialize SEAL library ----------------------------------------------------
int     Sound_Init_SEAL (void)
{
    if (AInitialize() != AUDIO_ERROR_NONE)
    {
        Quit_Msg (Msg_Get (MSG_Sound_Init_Error_SEAL));
        return (MEKA_ERR_FAIL);
    }
    return (MEKA_ERR_OK);
}

// Initialize Sound Card ------------------------------------------------------
static  int     Sound_Init_SoundCard (void)
{
  int            i;
  AUDIOINFO      Audio_Infos;

  ConsolePrintf (Msg_Get (MSG_Sound_Init_Soundcard), Sound.SampleRate);
  ConsolePrint ("\n");

  Audio_Infos.nDeviceId = Sound.SoundCard;
  Audio_Infos.wFormat = AUDIO_FORMAT_16BITS | AUDIO_FORMAT_STEREO; // FIXME: Stereo ?
  Audio_Infos.nSampleRate = g_sasound.audio_sample_rate = Sound.SampleRate;

  if (AOpenAudio(&Audio_Infos) != AUDIO_ERROR_NONE)
     {
     Quit_Msg ("%s", Msg_Get (MSG_Sound_Init_Error_Audio));
     return (MEKA_ERR_FAIL);
     }
  // FIXME: original sound engine was trying different sample rate on failure

  // Unused
  // Maybe it was intended to check out number of channels there ?
  // AGetAudioCaps (Audio_Infos.nDeviceId, &Audio_Caps);

  // Open voices
  if (AOpenVoices(Sound.Voices_Max) != AUDIO_ERROR_NONE)
     {
     Quit_Msg ("%s", Msg_Get (MSG_Sound_Init_Error_Voices));
     return (MEKA_ERR_FAIL);
     }

  ASetAudioMixerValue (AUDIO_MIXER_MASTER_VOLUME, 256);

  // Allocate voices and waveforms
  Sound.Voices = (t_voice*)Memory_Alloc(sizeof (t_voice) * Sound.Voices_Max);
  for (i = 0; i < Sound.Voices_Max; i++)
     {
     if (ACreateAudioVoice(&Sound.Voices[i].hVoice) != AUDIO_ERROR_NONE)
        {
        Quit_Msg (Msg_Get (MSG_Sound_Init_Error_Voice_N), i);
        return (MEKA_ERR_FAIL);
        }
     ASetVoicePanning(Sound.Voices[i].hVoice, 128); // Center voice
     Sound.Voices[i].lpWave  = NULL;
     Sound.Voices[i].playing = FALSE;
     }

  // FIXME: is this needed ?
  AUpdateAudio();

  // FIXME: is this needed ?
  // Check frame sample rate
  g_sasound.audio_sample_rate = g_sasound.nominal_sample_rate = Audio_Infos.nSampleRate;

  return (MEKA_ERR_OK);
}

// Find and Initialize OPL if needed ------------------------------------------
#ifdef MEKA_OPL
static  int     Sound_Init_OPL (void)
{
    // OPL is used whenever a "BLASTER Axxx" environment variable is found
    // This allows using OPL emulators (eg: VDMS) under NT based systems.
    /*
    // Attempt to find OPL only on systems supporting direct port accesses
    // FIXME: Should let the user force OPL enabling, because of potential OPL
    // emulators for recent Windows platforms.
    if (os_type == OSTYPE_UNKNOWN || os_type == OSTYPE_WIN3
        || os_type == OSTYPE_WIN95   || os_type == OSTYPE_WIN98
        || os_type == OSTYPE_WINME)
    {
        Sound_OPL_Init_Config ();
    }
    */

    if (Sound.OPL_Address != 0x000)
    {
        if (Sound_OPL_Init () == MEKA_ERR_OK)
            return (MEKA_ERR_OK);
        Sound.OPL_Address = 0x000;
    }
    return (MEKA_ERR_FAIL);
}
#endif

// Initialize Sound Engine ----------------------------------------------------
// FIXME: This is mostly legacy stuff :(
static  int     Sound_Init_Engine (int buffer_mode)
{
    // Stream Buffer Mode
    //STREAM_BUFFER_MAXA = DEF_STREAM_BUFFER_MAXA;
    MODEB_FRAME_SIZE   = DEF_MODEB_FRAME_SIZE;
    MODEB_UPDATE_COUNT = DEF_MODEB_UPDATE_COUNT;
    MODEB_ERROR_MAX    = DEF_STREAM_UPDATE_ERROR_MAX;
    STREAM_BUFFER_MAXB = MODEB_FRAME_SIZE;
    MODEB_MASK         = MODEB_FRAME_SIZE / MODEB_UPDATE_COUNT;

    sound_stream_mode  = buffer_mode; /* SOUND_STREAM_WAIT in MEKA */
    //stream_buffer_max  = (sound_stream_mode == SOUND_STREAM_NORMAL) ? STREAM_BUFFER_MAXA : STREAM_BUFFER_MAXB;
#ifdef INSTALL_SOUND_TIMER
    buffered_stream_max = stream_buffer_max = 3; // audio_buffer_max_size
    MODEB_UPDATE_COUNT = 1;
#else
    stream_buffer_max = ((stream_buffer_max / 6) / MODEB_UPDATE_COUNT) * MODEB_UPDATE_COUNT;
    // buffered_stream_max = stream_buffer_max;   // audio_buffer_max_size
    buffered_stream_max = MODEB_UPDATE_COUNT * 2; // audio_buffer_max_size
#endif

    /**** timer work init ****/
    sound_freerun_count = 0;
    sound_slice = 0;

    if (g_sasound.change_sample_rate)
    { 
		// Sample rate has changed, so all emulators must be restarted!
        g_sasound.change_sample_rate = FALSE;
        saStopSoundEmulators();
    }
    ConsolePrint (" - SEAL: Ok\n"); // FIXME: should be a message ?

#ifdef INSTALL_SOUND_TIMER
    saInitSoundTimer();
#endif

    if (SndMachine != NULL)
    {
        if (!SndMachine->first)
        {
            int i;
            SndMachine->first = 1;           /* first flag clear */
            streams_sh_start();              /* streaming system initialize & start */
            pause_sound = 0;                 /* pause flag off */
            vbover_err = vbunder_err = 0;    /* error initial */
            for (i = 0; i < SndMachine->control_max; i++)
            {
                if (SndMachine->f_init[i])
                {
                    if (SndMachine->f_init[i] (SndMachine->userdata[i]) != MEKA_ERR_OK)
                    {
                        SndMachine = NULL;
                        return (MEKA_ERR_FAIL);
                    }
                }
            }
        }
    }
    return (MEKA_ERR_OK);
}

// Initialize Sound Emulators -------------------------------------------------
static  void    Sound_Init_Emulators (void)
{
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

    // Add YM-2413HD (FM) emulator if we have an OPL
#ifdef MEKA_OPL
    if (Sound.OPL_Address)
    {
        rec.type     = SOUND_MACHINE_YM2413HD;
        rec.f_init   = FM_OPL_Init;
        rec.f_update = FM_OPL_Update;
        rec.f_stop   = FM_OPL_Close;
        rec.userdata = NULL;
        saAddSound     (&rec);
        Sound.FM_Emulator_Available |= FM_EMULATOR_YM2413HD;
    }
#endif

    // Add EMU2413 (FM) emulator
    rec.type       = SOUND_MACHINE_EMU2413;
    rec.f_init     = FM_Digital_Init;
    rec.f_update   = NULL; // FM_Digital_Update; // will be registered on initialisation
    rec.f_stop     = FM_Digital_Close;
    rec.userdata   = NULL;
    saAddSound       (&rec);
    Sound.FM_Emulator_Available |= FM_EMULATOR_EMU2413;
}

//-----------------------------------------------------------------------------
// Sound_Close ()
// Close sound engine
//-----------------------------------------------------------------------------
void            Sound_Close (void)
{
    if (Sound.Initialized == TRUE)
    {
        saRemoveSound ();
        #ifdef MEKA_OPL
            if (Sound.OPL_Address)
                Sound_OPL_Close ();
        #endif
        Sound_Log_Close ();
        Sound.Initialized = FALSE;
    }
}

//-----------------------------------------------------------------------------
// Sound_Update_Frame ()
// Miscellaneous things to do on each frame
//-----------------------------------------------------------------------------
void            Sound_Update_Frame (void)
{
    // Decrement FM usage counter
    // To save CPU, FM emulation is disabled if it gets to zero
    // Msg (MSGT_DEBUG, "FM_Used = %d\n", FM_Used);
    if (FM_Used > 0)
        FM_Used --;

    //saSoundTimerCallback();
    //streams_sh_update();
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

//-----------------------------------------------------------------------------
// Sound_MasterVolume_Set ()
// Change Master Volume (0-128)
//-----------------------------------------------------------------------------
void    Sound_MasterVolume_Set (int v)
{
    int   i;

    Sound.MasterVolume = v;
    for (i = 0; i < Sound.Voices_Max; i++)
    {
        // FIXME: need volume
        // ASetVoiceVolume (Sound.Voices[i].hVoice, (Sound.MasterVolume * volume) / 512);
    }
}
