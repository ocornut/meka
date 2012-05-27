//-----------------------------------------------------------------------------
// MEKA - wav.c
// WAV File Creation - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM (0x0001)
#endif

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_wav_format
{
 word           Category;
 word           Channels;
 dword          Samples_per_Second;
 dword          Avg_Bytes_per_Second;
 word           Block_Align;
 word           Bits_per_Sample; // PCM only
};

struct t_wav_header
{
 char           RIFF_Tag[4];
 dword          RIFF_Len;
 char           WAVE_Tag[4];
 char           Format_Tag[4];
 dword          Format_Len;
 t_wav_format   Format;
 char           Data_Tag[4];
 int            Data_Len;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    WAV_Header_Init(t_wav_header *h, int Samples_per_Second, int Bits_per_Sample, int Channels)
{
	memcpy(h->RIFF_Tag, "RIFF", 4);
	h->RIFF_Len = 0; // Unknown as of yet
	memcpy(h->WAVE_Tag, "WAVE", 4);
	memcpy(h->Format_Tag, "fmt ", 4);
	h->Format_Len = sizeof (t_wav_format);
	h->Format.Category = WAVE_FORMAT_PCM;
	h->Format.Channels = Channels;
	h->Format.Samples_per_Second = Samples_per_Second; // Get the *current* rate, which is not updated if user change it with menu, only on restart -> OK
	h->Format.Bits_per_Sample = Bits_per_Sample; // Always 16 in MEKA
	h->Format.Avg_Bytes_per_Second = h->Format.Channels * h->Format.Samples_per_Second * (h->Format.Bits_per_Sample / 8); // See comment below
	h->Format.Block_Align = h->Format.Channels * (h->Format.Bits_per_Sample / 8); // It's 2 in this situation, but theorically should be rounded up to the next whole number
	memcpy(h->Data_Tag, "data", 4);
	h->Data_Len = 0; // Unknown as of yet
}

FILE*	WAV_Start(char *FileName)
{
	FILE* f = fopen(FileName, "wb");
	if (f == NULL)
		return (NULL);

	t_wav_header h;
	WAV_Header_Init(&h, Sound.SampleRate, 16, 1);
	fwrite (&h, sizeof (h), 1, f);
	// ..
	return (f);
}

void	WAV_Close(FILE *f, int SizeData)
{
	t_wav_header h;

	h.RIFF_Len = (4) + (8+16) + (8 + SizeData);
	int Offset = (int)((char *)&h.RIFF_Len - (char *)&h);
	fseek (f, Offset, SEEK_SET);
	fwrite (&h.RIFF_Len, sizeof (h.RIFF_Len), 1, f);

	h.Data_Len = (SizeData);
	Offset = (int)((char *)&h.Data_Len - (char *)&h);
	fseek (f, Offset, SEEK_SET);
	fwrite (&h.Data_Len, sizeof (h.Data_Len), 1, f);

	fclose(f);
}

//-----------------------------------------------------------------------------
