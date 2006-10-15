/*
 * $Id: audio.h 1.17 1996/09/25 17:13:02 chasan released $
 *              1.18 1998/10/12 23:54:08 chasan released
 *              1.19 1998/10/24 18:20:52 chasan released
 *              1.20 1999/06/27 17:49:49 chasan released
 *
 * SEAL Synthetic Audio Library API Interface
 *
 * Copyright (C) 1995, 1996, 1997, 1998, 1999 Carlos Hasan
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __AUDIO_H
#define __AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#define AIAPI
#else
#define AIAPI __stdcall
#endif

#ifndef WINAPI

/* atomic data types definitions */
    typedef void            VOID;
    typedef char            CHAR;
    typedef int             INT;
    typedef long            LONG;
    typedef int             BOOL;

    typedef unsigned char   BYTE;
    typedef unsigned short  WORD;
    typedef unsigned int    UINT;
    typedef unsigned long   DWORD;

    typedef VOID*           LPVOID;
    typedef CHAR*           LPCHAR;
    typedef INT*            LPINT;
    typedef LONG*           LPLONG;
    typedef BOOL*           LPBOOL;
    typedef BYTE*           LPBYTE;
    typedef WORD*           LPWORD;
    typedef UINT*           LPUINT;
    typedef DWORD*          LPDWORD;
    typedef CHAR*           LPSTR;
    typedef DWORD           HANDLE;

/* helper macros */
#define LOBYTE(s)       ((BYTE)(s))
#define HIBYTE(s)       ((BYTE)((WORD)(s)>>8))
#define LOWORD(l)       ((WORD)(l))
#define HIWORD(l)       ((WORD)((DWORD)(l)>>16))
#define MAKEWORD(l,h)   ((WORD)(((BYTE)(l))|(((WORD)((BYTE)(h)))<<8)))
#define MAKELONG(l,h)   ((DWORD)(((WORD)(l))|(((DWORD)((WORD)(h)))<<16)))

#endif


/* audio system version number */
#define AUDIO_SYSTEM_VERSION            0x0106

/* audio capabilities bit fields definitions */
#define AUDIO_FORMAT_1M08               0x00000001
#define AUDIO_FORMAT_1S08               0x00000002
#define AUDIO_FORMAT_1M16               0x00000004
#define AUDIO_FORMAT_1S16               0x00000008
#define AUDIO_FORMAT_2M08               0x00000010
#define AUDIO_FORMAT_2S08               0x00000020
#define AUDIO_FORMAT_2M16               0x00000040
#define AUDIO_FORMAT_2S16               0x00000080
#define AUDIO_FORMAT_4M08               0x00000100
#define AUDIO_FORMAT_4S08               0x00000200
#define AUDIO_FORMAT_4M16               0x00000400
#define AUDIO_FORMAT_4S16               0x00000800

/* audio format bit fields defines for devices and waveforms */
#define AUDIO_FORMAT_8BITS              0x0000
#define AUDIO_FORMAT_16BITS             0x0001
#define AUDIO_FORMAT_LOOP               0x0010
#define AUDIO_FORMAT_BIDILOOP           0x0020
#define AUDIO_FORMAT_REVERSE            0x0080
#define AUDIO_FORMAT_MONO               0x0000
#define AUDIO_FORMAT_STEREO             0x0100
#define AUDIO_FORMAT_FILTER             0x8000

/*JB 2000-02-25*/
#define AUDIO_FORMAT_RAW_SAMPLE         0x4000
/*JB END*/


/* audio resource limits defines */
#define AUDIO_MAX_VOICES                32
#define AUDIO_MAX_SAMPLES               16
#define AUDIO_MAX_PATCHES               128
#define AUDIO_MAX_PATTERNS              256
#define AUDIO_MAX_ORDERS                256
#define AUDIO_MAX_NOTES                 96
#define AUDIO_MAX_POINTS                12
#define AUDIO_MIN_PERIOD                1
#define AUDIO_MAX_PERIOD                31999
#define AUDIO_MIN_VOLUME                0x00
#define AUDIO_MAX_VOLUME                0x40
#define AUDIO_MIN_PANNING               0x00
#define AUDIO_MAX_PANNING               0xFF
#define AUDIO_MIN_POSITION              0x00000000L
#define AUDIO_MAX_POSITION              0x00100000L
#define AUDIO_MIN_FREQUENCY             0x00000200L
#define AUDIO_MAX_FREQUENCY             0x00080000L

/* audio error code defines */
#define AUDIO_ERROR_NONE                0x0000
#define AUDIO_ERROR_INVALHANDLE         0x0001
#define AUDIO_ERROR_INVALPARAM          0x0002
#define AUDIO_ERROR_NOTSUPPORTED        0x0003
#define AUDIO_ERROR_BADDEVICEID         0x0004
#define AUDIO_ERROR_NODEVICE            0x0005
#define AUDIO_ERROR_DEVICEBUSY          0x0006
#define AUDIO_ERROR_BADFORMAT           0x0007
#define AUDIO_ERROR_NOMEMORY            0x0008
#define AUDIO_ERROR_NODRAMMEMORY        0x0009
#define AUDIO_ERROR_FILENOTFOUND        0x000A
#define AUDIO_ERROR_BADFILEFORMAT       0x000B
#define AUDIO_LAST_ERROR                0x000B

/* audio device identifiers */
#define AUDIO_DEVICE_NONE               0x0000
#define AUDIO_DEVICE_MAPPER             0xFFFF

/* audio product identifiers */
#define AUDIO_PRODUCT_NONE              0x0000
#define AUDIO_PRODUCT_SB                0x0001
#define AUDIO_PRODUCT_SB15              0x0002
#define AUDIO_PRODUCT_SB20              0x0003
#define AUDIO_PRODUCT_SBPRO             0x0004
#define AUDIO_PRODUCT_SB16              0x0005
#define AUDIO_PRODUCT_AWE32             0x0006
#define AUDIO_PRODUCT_WSS               0x0007
#define AUDIO_PRODUCT_ESS               0x0008
#define AUDIO_PRODUCT_GUS               0x0009
#define AUDIO_PRODUCT_GUSDB             0x000A
#define AUDIO_PRODUCT_GUSMAX            0x000B
#define AUDIO_PRODUCT_IWAVE             0x000C
#define AUDIO_PRODUCT_PAS               0x000D
#define AUDIO_PRODUCT_PAS16             0x000E
#define AUDIO_PRODUCT_ARIA              0x000F
#define AUDIO_PRODUCT_WINDOWS           0x0100
#define AUDIO_PRODUCT_LINUX             0x0101
#define AUDIO_PRODUCT_SPARC             0x0102
#define AUDIO_PRODUCT_SGI               0x0103
#define AUDIO_PRODUCT_DSOUND            0x0104
#define AUDIO_PRODUCT_OS2MMPM           0x0105
#define AUDIO_PRODUCT_OS2DART           0x0106
#define AUDIO_PRODUCT_BEOSR3            0x0107
#define AUDIO_PRODUCT_BEOS              0x0108
#define AUDIO_PRODUCT_QNX               0x0109

/* audio mixer channels */
#define AUDIO_MIXER_MASTER_VOLUME       0x0001
#define AUDIO_MIXER_TREBLE              0x0002
#define AUDIO_MIXER_BASS                0x0003
#define AUDIO_MIXER_CHORUS              0x0004
#define AUDIO_MIXER_REVERB              0x0005

/* audio envelope bit fields */
#define AUDIO_ENVELOPE_ON               0x0001
#define AUDIO_ENVELOPE_SUSTAIN          0x0002
#define AUDIO_ENVELOPE_LOOP             0x0004

/* audio pattern bit fields */
#define AUDIO_PATTERN_PACKED            0x0080
#define AUDIO_PATTERN_NOTE              0x0001
#define AUDIO_PATTERN_SAMPLE            0x0002
#define AUDIO_PATTERN_VOLUME            0x0004
#define AUDIO_PATTERN_COMMAND           0x0008
#define AUDIO_PATTERN_PARAMS            0x0010

/* audio module bit fields */
#define AUDIO_MODULE_AMIGA              0x0000
#define AUDIO_MODULE_LINEAR             0x0001
#define AUDIO_MODULE_PANNING            0x8000

#pragma pack(1)

/* audio capabilities structure */
    typedef struct {
	WORD    wProductId;                         /* product identifier */
	CHAR    szProductName[30];                  /* product name */
	DWORD   dwFormats;                          /* formats supported */
    } AUDIOCAPS, *LPAUDIOCAPS;

/* audio format structure */
    typedef struct {
	UINT    nDeviceId;                          /* device identifier */
	WORD    wFormat;                            /* playback format */
	WORD    nSampleRate;                        /* sampling frequency */
    } AUDIOINFO, *LPAUDIOINFO;

/* audio waveform structure */
    typedef struct {
	LPBYTE  lpData;                             /* data pointer */
	DWORD   dwHandle;                           /* waveform handle */
	DWORD   dwLength;                           /* waveform length */
	DWORD   dwLoopStart;                        /* loop start point */
	DWORD   dwLoopEnd;                          /* loop end point */
	WORD    nSampleRate;                        /* sampling rate */
	WORD    wFormat;                            /* format bits */
    } AUDIOWAVE, *LPAUDIOWAVE;


/* audio envelope point structure */
    typedef struct {
	WORD    nFrame;                             /* envelope frame */
	WORD    nValue;                             /* envelope value */
    } AUDIOPOINT, *LPAUDIOPOINT;

/* audio envelope structure */
    typedef struct {
	AUDIOPOINT aEnvelope[AUDIO_MAX_POINTS];     /* envelope points */
	BYTE    nPoints;                            /* number of points */
	BYTE    nSustain;                           /* sustain point */
	BYTE    nLoopStart;                         /* loop start point */
	BYTE    nLoopEnd;                           /* loop end point */
	WORD    wFlags;                             /* envelope flags */
	WORD    nSpeed;                             /* envelope speed */
    } AUDIOENVELOPE, *LPAUDIOENVELOPE;

/* audio sample structure */
    typedef struct {
	CHAR    szSampleName[32];                   /* sample name */
	BYTE    nVolume;                            /* default volume */
	BYTE    nPanning;                           /* default panning */
	BYTE    nRelativeNote;                      /* relative note */
	BYTE    nFinetune;                          /* finetune */
	AUDIOWAVE Wave;                             /* waveform handle */
    } AUDIOSAMPLE, *LPAUDIOSAMPLE;

/* audio patch structure */
    typedef struct {
	CHAR    szPatchName[32];                    /* patch name */
	BYTE    aSampleNumber[AUDIO_MAX_NOTES];     /* multi-sample table */
	WORD    nSamples;                           /* number of samples */
	BYTE    nVibratoType;                       /* vibrato type */
	BYTE    nVibratoSweep;                      /* vibrato sweep */
	BYTE    nVibratoDepth;                      /* vibrato depth */
	BYTE    nVibratoRate;                       /* vibrato rate */
	WORD    nVolumeFadeout;                     /* volume fadeout */
	AUDIOENVELOPE Volume;                       /* volume envelope */
	AUDIOENVELOPE Panning;                      /* panning envelope */
	LPAUDIOSAMPLE aSampleTable;                 /* sample table */
    } AUDIOPATCH, *LPAUDIOPATCH;

/* audio pattern structure */
    typedef struct {
	WORD    nPacking;                           /* packing type */
	WORD    nTracks;                            /* number of tracks */
	WORD    nRows;                              /* number of rows */
	WORD    nSize;                              /* data size */
	LPBYTE  lpData;                             /* data pointer */
    } AUDIOPATTERN, *LPAUDIOPATTERN;

/* audio module structure */
    typedef struct {
	CHAR    szModuleName[32];                   /* module name */
	WORD    wFlags;                             /* module flags */
	WORD    nOrders;                            /* number of orders */
	WORD    nRestart;                           /* restart position */
	WORD    nTracks;                            /* number of tracks */
	WORD    nPatterns;                          /* number of patterns */
	WORD    nPatches;                           /* number of patches */
	WORD    nTempo;                             /* initial tempo */
	WORD    nBPM;                               /* initial BPM */
	BYTE    aOrderTable[AUDIO_MAX_ORDERS];      /* order table */
	BYTE    aPanningTable[AUDIO_MAX_VOICES];    /* panning table */
	LPAUDIOPATTERN aPatternTable;               /* pattern table */
	LPAUDIOPATCH aPatchTable;                   /* patch table */
    } AUDIOMODULE, *LPAUDIOMODULE;

/* audio music track structure */
    typedef struct {
	BYTE   nNote;                              /* note index */
	BYTE   nPatch;                             /* patch number */
	BYTE   nSample;                            /* sample number */
	BYTE   nCommand;                           /* effect command */
	BYTE   bParams;                            /* effect params */
	BYTE   nVolumeCmd;                         /* volume command */
	BYTE   nVolume;                            /* volume level */
	BYTE   nPanning;                           /* stereo panning */
	LONG   dwFrequency;                        /* note frequency */
	WORD   wPeriod;                            /* note period */
    } AUDIOTRACK, *LPAUDIOTRACK;

/* audio callback function defines */
    typedef VOID (AIAPI* LPFNAUDIOWAVE)(LPBYTE, UINT);
    typedef VOID (AIAPI* LPFNAUDIOTIMER)(VOID);
    typedef VOID (AIAPI* LPFNAUDIOCALLBACK)(BYTE, UINT, UINT);

/* audio handle defines */
    typedef HANDLE  HAC;
    typedef HAC*    LPHAC;

#pragma pack()

/* audio interface API prototypes */
    UINT AIAPI AInitialize(VOID);
    UINT AIAPI AGetVersion(VOID);
    UINT AIAPI AGetAudioNumDevs(VOID);
    UINT AIAPI AGetAudioDevCaps(UINT nDeviceId, LPAUDIOCAPS lpCaps);
    UINT AIAPI AGetErrorText(UINT nErrorCode, LPSTR lpText, UINT nSize);

    UINT AIAPI APingAudio(LPUINT lpnDeviceId);
    UINT AIAPI AOpenAudio(LPAUDIOINFO lpInfo);
    UINT AIAPI ACloseAudio(VOID);
    UINT AIAPI AUpdateAudio(VOID);
    UINT AIAPI AUpdateAudioEx(UINT nFrames);

    UINT AIAPI ASetAudioMixerValue(UINT nChannel, UINT nValue);

    UINT AIAPI AOpenVoices(UINT nVoices);
    UINT AIAPI ACloseVoices(VOID);

    UINT AIAPI ASetAudioCallback(LPFNAUDIOWAVE lpfnAudioWave);
    UINT AIAPI ASetAudioTimerProc(LPFNAUDIOTIMER lpfnAudioTimer);
    UINT AIAPI ASetAudioTimerRate(UINT nTimerRate);

    LONG AIAPI AGetAudioDataAvail(VOID);
    UINT AIAPI ACreateAudioData(LPAUDIOWAVE lpWave);
    UINT AIAPI ADestroyAudioData(LPAUDIOWAVE lpWave);
    UINT AIAPI AWriteAudioData(LPAUDIOWAVE lpWave, DWORD dwOffset, UINT nCount);

    UINT AIAPI ACreateAudioVoice(LPHAC lphVoice);
    UINT AIAPI ADestroyAudioVoice(HAC hVoice);

    UINT AIAPI APlayVoice(HAC hVoice, LPAUDIOWAVE lpWave);
    UINT AIAPI APrimeVoice(HAC hVoice, LPAUDIOWAVE lpWave);
    UINT AIAPI AStartVoice(HAC hVoice);
    UINT AIAPI AStopVoice(HAC hVoice);

    UINT AIAPI ASetVoicePosition(HAC hVoice, LONG dwPosition);
    UINT AIAPI ASetVoiceFrequency(HAC hVoice, LONG dwFrequency);
    UINT AIAPI ASetVoiceVolume(HAC hVoice, UINT nVolume);
    UINT AIAPI ASetVoicePanning(HAC hVoice, UINT nPanning);

    UINT AIAPI AGetVoicePosition(HAC hVoice, LPLONG lpdwPosition);
    UINT AIAPI AGetVoiceFrequency(HAC hVoice, LPLONG lpdwFrequency);
    UINT AIAPI AGetVoiceVolume(HAC hVoice, LPUINT lpnVolume);
    UINT AIAPI AGetVoicePanning(HAC hVoice, LPUINT lpnPanning);
    UINT AIAPI AGetVoiceStatus(HAC hVoice, LPBOOL lpnStatus);

    UINT AIAPI APlayModule(LPAUDIOMODULE lpModule);
    UINT AIAPI AStopModule(VOID);
    UINT AIAPI APauseModule(VOID);
    UINT AIAPI AResumeModule(VOID);
    UINT AIAPI ASetModuleVolume(UINT nVolume);
    UINT AIAPI ASetModulePosition(UINT nOrder, UINT nRow);
    UINT AIAPI AGetModuleVolume(LPUINT lpnVolume);
    UINT AIAPI AGetModulePosition(LPUINT pnOrder, LPUINT lpnRow);
    UINT AIAPI AGetModuleStatus(LPBOOL lpnStatus);
    UINT AIAPI ASetModuleCallback(LPFNAUDIOCALLBACK lpfnAudioCallback);

    UINT AIAPI ALoadModuleFile(LPSTR lpszFileName, 
			       LPAUDIOMODULE* lplpModule, DWORD dwFileOffset);
    UINT AIAPI AFreeModuleFile(LPAUDIOMODULE lpModule);

    UINT AIAPI ALoadWaveFile(LPSTR lpszFileName, 
			     LPAUDIOWAVE* lplpWave, DWORD dwFileOffset);
    UINT AIAPI AFreeWaveFile(LPAUDIOWAVE lpWave);

    UINT AIAPI AGetModuleTrack(UINT nTrack, LPAUDIOTRACK lpTrack);

#ifdef __cplusplus
};
#endif

#endif
