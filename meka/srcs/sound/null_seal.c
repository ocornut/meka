//-----------------------------------------------------------------------------
// MEKA - null_seal.h
// NULL libseal driver - Headers
//-----------------------------------------------------------------------------

#include "audio.h"

UINT AIAPI AGetAudioDevCaps(UINT nDeviceId, LPAUDIOCAPS lpCaps)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AGetAudioNumDevs(VOID)
{
  return 0;
}

UINT AIAPI ACreateAudioVoice(LPHAC lphVoice)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AInitialize(VOID)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AOpenAudio(LPAUDIOINFO lpInfo)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AOpenVoices(UINT nVoices)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ASetAudioMixerValue(UINT nChannel, UINT nValue)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ASetVoicePanning(HAC hVoice, UINT nPanning)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AUpdateAudio(VOID)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ACloseAudio(VOID)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ACloseVoices(VOID)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ACreateAudioData(LPAUDIOWAVE lpWave)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ADestroyAudioData(LPAUDIOWAVE lpWave)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ADestroyAudioVoice(HAC hVoice)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AGetVoicePosition(HAC hVoice, LPLONG lpdwPosition)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI APlayVoice(HAC hVoice, LPAUDIOWAVE lpWave)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI APrimeVoice(HAC hVoice, LPAUDIOWAVE lpWave)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ASetVoiceFrequency(HAC hVoice, LONG dwFrequency)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI ASetVoiceVolume(HAC hVoice, UINT nVolume)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AStartVoice(HAC hVoice)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AStopVoice(HAC hVoice)
{
  return AUDIO_ERROR_NONE;
}

UINT AIAPI AWriteAudioData(LPAUDIOWAVE lpWave, DWORD dwOffset, UINT nCount)
{
  return AUDIO_ERROR_NONE;
}

