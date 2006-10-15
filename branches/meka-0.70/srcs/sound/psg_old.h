//
// Meka - PSG.H
// PSG SN-76496 Emulator - Headers
// Based on code from Hiromitsu Shioya, 1998-1999.
// Rewrote by Omar Cornut, 1999-2001.
//

#ifndef __PSG_H__
#define __PSG_H__

// Maximum number of emulated PSG
#define PSG_MAX                 (4)

// Channels
#define PSG_CHANNEL_TONE_1      (0)
#define PSG_CHANNEL_TONE_2      (1)
#define PSG_CHANNEL_TONE_3      (2)
#define PSG_CHANNEL_NOISE       (3)
#define PSG_CHANNEL_MAX         (4)

typedef struct
{
  int           Num;                //   Total number of 76496 in the machine
  int           CPU_Clock;          //   Base CPU Clock (in Hertz)
  int           Volume [PSG_MAX];   //   Base volume for each 76496
  int           Gain [PSG_MAX];     //   Gain for each 76496
} t_psg_interface;

typedef struct
{
  int           saChannel;          //   SaSound channel
  int           saSampleRate;       //   SaSound Sample Rate (in Hertz)
  int           saVolume;           //   SaSound volume
  int           Clock;              //   Base Clock (in Hertz)
  unsigned int  UpdateStep;         //   Number of steps that happens in a sample (based on CPU_Clock)
  int           VolumesTable [16];  //   Volume table
  word          Register [8];       // * Registers
  int           LastRegister;       // * Last register written
  int           Volume [4];         //   Volumes (from VolumesTable[])
  unsigned int  RNG;                // * Random Noise Generator shift register
  int           NoiseFB;            //   Noise feedback mask
  int           Period [4];         //   Half Period of the square wave
  int           Count [4];          //
  int           Output [4];         // * Current output (0 or 1)
  int           Mute [4];           //   Current mute state (set to mute channel), tested as != 0
} t_psg;

extern  t_psg   PSG [PSG_MAX];

int             PSG_sh_start (void *userdata);
// Compiling with Visual C requires that.. what is the reason ? I did not find it
//#ifndef WIN32
//t_psg_interface *intf
//#endif

int             PSG_Init (int chip, int sample_rate, int cpu_clock, int volume, int gain);
void            PSG_Gain_Set (int chip, int gain);
void            PSG_Volume_Set (int chip, int volume);
void            PSG_Clock_Set (int chip, int cpu_clock);
void            PSG_Reset (int chip);

void            PSG_Save (int chip, FILE *f);
void            PSG_Load (int chip, FILE *f);
void            PSG_Regenerate_Data (int chip);

void            PSG_Write (int chip, int data);
#define         PSG_0_Write(data) PSG_Write (0, data)
#define         PSG_1_Write(data) PSG_Write (1, data)
#define         PSG_2_Write(data) PSG_Write (2, data)
#define         PSG_3_Write(data) PSG_Write (3, data)

void            PSG_Update_16 (int chip, void *buffer, int length);

#endif /* __PSG_H__ */

