//
// Meka - PSG.C
// PSG SN-76496 Emulator - Code
// Based on code from Hiromitsu Shioya, 1998-1999.
// Rewrote by Omar Cornut, 1999-2001.
//

//-----------------------------------------------------------------------------
// Routines to emulate the Texas Instruments SN76489 and SN76496 programmable
// tone /noise generator.
// Noise emulation is not accurate due to lack of documentation. The noise
// generator uses a shift register with a XOR-feedback network, but the exact
// layout is unknown. It can be set for either period or white noise; again,
// the details are unknown.
//-----------------------------------------------------------------------------

#include "shared.h"

t_psg   PSG [PSG_MAX];

//-----------------------------------------------------------------------------
#define SIGNED_SAMPLES
#ifdef SIGNED_SAMPLES
  #define MAX_OUTPUT    (0x7fff)
  #define AUDIO_CONV(A) (A)
#else
  #error "Unsigned samples are not supported"
  #define MAX_OUTPUT    (0xffff)
  #define AUDIO_CONV(A) (A)
#endif

#define STEP (0x10000)
//-----------------------------------------------------------------------------

// FIXME: needs to implements correct noise generator emulation
// FIXME: implement random sequence (as found by Maxim)

// Formulas for noise generator -----------------------------------------------
// bit0 = output

// Noise feedback for White Noise mode
#define FB_WNOISE (0x12000)     /* bit15.d(16bits) = bit0(out) ^ bit2 */
// #define FB_WNOISE (0x14000)   /* bit15.d(16bits) = bit0(out) ^ bit1 */
// #define FB_WNOISE (0x28000)   /* bit16.d(17bits) = bit0(out) ^ bit2 (same to AY-3-8910) */
// #define FB_WNOISE (0x50000)   /* bit17.d(18bits) = bit0(out) ^ bit2 */

// Noise feedback for Periodic Noise mode
// (it is correct maybe - it was in the Megadrive sound manual)
// #define FB_PNOISE (0x10000)     /* 16bit rorate */
// #define FB_PNOISE (0x08000)   /* JH 981127 - fixes Do Run Run */
#define FB_PNOISE (0x10000)

// Noise generator start preset (for Periodic Noise)
// #define NG_PRESET (0x0F35)
#define NG_PRESET (0x00000)

// PSG_sh_start() / PSG_sh_stop() / PSG_sh_update() ---------------------------
// PSG interface for SaSound, although it is in fact bypassed
//-----------------------------------------------------------------------------
int     PSG_sh_start (void *userdata)
{
 int    chip;
 t_psg_interface *intf = userdata;

 for (chip = 0; chip < intf->Num; chip++)
     {
     if (PSG_Init (chip, audio_sample_rate, intf->CPU_Clock, intf->Volume[chip], intf->Gain[chip]) != MEKA_ERR_OK)
        {
        return (MEKA_ERR_FAIL);
        }
     }
 return (MEKA_ERR_OK);
}
//-----------------------------------------------------------------------------

// PSG_Init() -----------------------------------------------------------------
// Initialize a PSG
//-----------------------------------------------------------------------------
int     PSG_Init (int chip, int sample_rate, int cpu_clock, int volume, int gain)
{
 int    i;
 char   name[16];
 t_psg *psg = &PSG[chip];

 printf("%s ", Msg_Get (MSG_Sound_Init_SN76496));
 fflush(stdout);

 sprintf(name, "SN76496 #%d", chip);
 psg->saChannel = stream_init (name, sample_rate, 16, chip, PSG_Update_16);
 if (psg->saChannel == -1)
 {
    printf("%s\n", Msg_Get (MSG_Failed));
    return (MEKA_ERR_FAIL);
 }
 psg->saSampleRate = sample_rate;

 PSG_Clock_Set  (chip, cpu_clock);
 PSG_Volume_Set (chip, volume);
 PSG_Gain_Set   (chip, gain);
 PSG_Reset      (chip);

 for (i = 0; i < 4; i++)
     psg->Mute[i] = NO;

 printf("%s\n", Msg_Get (MSG_Ok));
 return (MEKA_ERR_OK);
}
//-----------------------------------------------------------------------------

// PSG_Gain_Set() -------------------------------------------------------------
// Create volumes table based on given gain
//-----------------------------------------------------------------------------
void    PSG_Gain_Set (int chip, int gain)
{
 t_psg *psg = &PSG[chip];
 int    i;
 double out;

 gain &= 0xff;

 // Increase max output basing on gain (0.2 dB per step)
 out = MAX_OUTPUT /* / 3 */;
 while (gain -- > 0)
   out *= 1.023292992; // = (10 ^ (0.2/20))

 // Build volume table (2dB per step)
 for (i = 0; i < 15; i++)
     {
     // Limit volume to avoid clipping
     if (out > MAX_OUTPUT /* / 3 */)
        psg->VolumesTable[i] = MAX_OUTPUT /* / 3 */;
     else
        psg->VolumesTable[i] = out;
     // Attenuate volume
     out /= 1.258925412; // = 10 ^ (2/20) = 2dB
     }
 psg->VolumesTable[15] = 0;
}
//-----------------------------------------------------------------------------

// PSG_Volume_Set() -----------------------------------------------------------
// Set volume in SaSound engine
//-----------------------------------------------------------------------------
void    PSG_Volume_Set (int chip, int volume)
{
 PSG[chip].saVolume = volume;
 stream_set_volume (PSG[chip].saChannel, volume);
}
//-----------------------------------------------------------------------------

// PSG_Clock_Set() ------------------------------------------------------------
// Set base CPU Clock
//-----------------------------------------------------------------------------
void    PSG_Clock_Set (int chip, int cpu_clock)
{
 // the base clock for the tone generators is the chip clock divided by 16;
 // for the noise generator, it is clock / 256.
 // Here we calculate the number of steps which happen during one sample
 // at the given sample rate. No. of events = sample rate / (clock/16).
 // STEP is a multiplier used to turn the fraction into a fixed point number.
 PSG[chip].Clock = cpu_clock;
 PSG[chip].UpdateStep = ((double)STEP * PSG[chip].saSampleRate * 16) / cpu_clock;
}
//-----------------------------------------------------------------------------

// PSG_Reset() ----------------------------------------------------------------
// Reset a PSG
//-----------------------------------------------------------------------------
void    PSG_Reset (int chip)
{
 int    i;
 t_psg *psg = &PSG[chip];

 // Registers
 psg->LastRegister = 0;
 for (i = 0; i < 8; i += 2)
     {
     psg->Register[i + 0] = (i < 6) ? 0x0000 : 0x0004; // Frequency = 0, White Noise
     psg->Register[i + 1] = 0x0F;   // Volume = F (muted)
     }

 // Others
 for (i = 0; i < 4; i++)
     {
     psg->Volume[i] = 0x0000;
     psg->Output[i] = (i < 3) ? 0 : (psg->RNG & 1);
     psg->Period[i] = psg->Count[i] = psg->UpdateStep;
     }
 psg->RNG = NG_PRESET;
}
//-----------------------------------------------------------------------------

// PSG_Save() -----------------------------------------------------------------
// Save PSG state to file
//-----------------------------------------------------------------------------
void            PSG_Save (int chip, FILE *f)
{
 byte           b;

 fwrite (&PSG[0].Register[0],  8, sizeof (word),         f);
 // fwrite (&PSG[0].LastRegister, 1, sizeof (int),       f);  // Not necessary
 fwrite (&PSG[0].RNG,          1, sizeof (unsigned int), f);
 b = (PSG[0].Output[0] & 1) | ((PSG[0].Output[1] & 1) << 1) | ((PSG[0].Output[2] & 1) << 2) | ((PSG[0].Output[3] & 1) << 3);
 fwrite (&b,                   1, sizeof (byte),         f);
}
//-----------------------------------------------------------------------------

// PSG_Load() -----------------------------------------------------------------
// Load PSG state from a file
//-----------------------------------------------------------------------------
void            PSG_Load (int chip, FILE *f)
{
 byte           b;

 fread (&PSG[0].Register[0],  8, sizeof (word),         f);
 fread (&PSG[0].RNG,          1, sizeof (unsigned int), f);
 fread (&b,                   1, sizeof (byte),         f);
 PSG[0].Output[0] = b & 1;
 PSG[0].Output[1] = (b >> 1) & 1;
 PSG[0].Output[2] = (b >> 2) & 1;
 PSG[0].Output[3] = (b >> 3) & 1;
 PSG_Regenerate_Data (chip);
}
//-----------------------------------------------------------------------------

// PSG_Regenerate_Data() ------------------------------------------------------
// Regenerate various PSG data from registers
// This is called after a PSG state loading
//-----------------------------------------------------------------------------
void            PSG_Regenerate_Data (int chip)
{
 int            i;
 t_psg         *psg = &PSG[chip];

 for (i = 0; i < 4; i++)
    {
    if (i < 3)
       { // TONE Period
       psg->Period[i] = psg->UpdateStep * psg->Register[(i*2)];
       if (psg->Period[i] == 0) psg->Period[i] = psg->UpdateStep;
       }
    else
       { // NOISE Period & Feedback
       int n = psg->Register[6] & 0x03;
       psg->Period[3] = (n == 0x03) ? 2 * psg->Period[2] : (psg->UpdateStep << (5 + n));
       psg->NoiseFB = (psg->Register[6] & 4) ? FB_WNOISE : FB_PNOISE;
       }
    psg->Volume[i] = psg->Mute[i] ? 0 : psg->VolumesTable[psg->Register[(i*2)+1] & 0x0f];
    }
}
//-----------------------------------------------------------------------------

// PSG_Write() ----------------------------------------------------------------
// Write to a PSG
//-----------------------------------------------------------------------------
void    PSG_Write (int chip, int data)
{
  t_psg *psg = &PSG[chip];

  if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
     VGM_Data_Add_PSG (&Sound.LogVGM, (byte)data);

  // Update the output buffer before changing the registers
  stream_update (psg->saChannel, 0);

  if (data & 0x80)
     {
     int r = (data & 0x70) >> 4;
     int c = r / 2;

     psg->LastRegister = r;
     switch (r)
        {
        case 0: /* tone 0 : frequency */
        case 2: /* tone 1 : frequency */
        case 4: /* tone 2 : frequency */
           psg->Register[r] = (psg->Register[r] & 0x3f0) | (data & 0x0f);
           psg->Period[c] = psg->UpdateStep * psg->Register[r];
           if (psg->Period[c] == 0) psg->Period[c] = psg->UpdateStep;
           if (r == 4)
              {
              // Update noise shift frequency
              if ((psg->Register[6] & 0x03) == 0x03)
                 psg->Period[3] = 2 * psg->Period[2];
              }
           break;
        case 1: /* tone 0 : volume */
        case 3: /* tone 1 : volume */
        case 5: /* tone 2 : volume */
        case 7: /* noise  : volume */
           psg->Register[r] = (data & 0x0f);
           // if (psg->Volume[c] != psg->VolTable[data & 0x0f]) // Reset Counter on volume change
           //    psg->Count[c] = psg->UpdateStep;
           psg->Volume[c] = psg->Mute[c] ? 0 : psg->VolumesTable[data & 0x0f];
           /**** change hiro-shi (use SEGA Mark3/MasterSystem/GameGear) ****/
           if (r != 7) // If it's not the noise channel
              {
              psg->Period[c] = psg->UpdateStep * psg->Register[r & (~1)];
              if (psg->Period[c] == 0) psg->Period[c] = psg->UpdateStep;
              if (r == 5)
                 {
                 // Update noise shift frequency
                 if ((psg->Register[6] & 0x03) == 0x03)
                    psg->Period[3] = 2 * psg->Period[2];
                 }
              }
           /**** end ****/
           break;
        case 6: /* noise : frequency, mode */
           {
           int n = psg->Register[r] = data & 0x0f;
           psg->NoiseFB = (n & 4) ? FB_WNOISE : FB_PNOISE;
           n &= 3;
           /* N/512,N/1024,N/2048,Tone #3 output */
           psg->Period[3] = (n == 3) ? 2 * psg->Period[2] : (psg->UpdateStep << (5 + n));

           // Reset Noise Shift Register
           psg->RNG = NG_PRESET;
           psg->Output [3] = psg->RNG & 1;
           }
           break;
        }
     }
  else
     {
     int r = psg->LastRegister;
     int c = r / 2;

     switch (r)
        {
        case 0: /* tone 0 : frequency */
        case 2: /* tone 1 : frequency */
        case 4: /* tone 2 : frequency */
           psg->Register[r] = (psg->Register[r] & 0x0f) | ((data & 0x3f) << 4);
           // if (psg->Register[r] == 0)
           //    Msg (MSGT_DEBUG, "PSG Register[%d] Frequency == 0!", r);
           psg->Period[c] = psg->UpdateStep * psg->Register[r];
           if (psg->Period[c] == 0) psg->Period[c] = psg->UpdateStep;
           if (r == 4)
              {
              // Update noise shift frequency
              if ((psg->Register[6] & 0x03) == 0x03)
                 psg->Period[3] = 2 * psg->Period[2];
              }
           break;
#if 1
        case 1: /* tone 0 : volume */
        case 3: /* tone 1 : volume */
        case 5: /* tone 2 : volume */
        case 7: /* noise  : volume */
           psg->Register[r] = (data & 0x0f);
           psg->Volume[c] = psg->Mute[c] ? 0 : psg->VolumesTable[data & 0x0f];
           //Msg(MSGT_USER, "Tone %d volume Set", r);
           if (r != 7) // If it's not the noise channel
              {
              psg->Period[c] = psg->UpdateStep * psg->Register[r & (~1)];
              if (psg->Period[c] == 0) psg->Period[c] = psg->UpdateStep;
              if (r == 5)
                 {
                 // Update noise shift frequency
                 if ((psg->Register[6] & 0x03) == 0x03)
                    psg->Period[3] = 2 * psg->Period[2];
                 }
              }
           /**** end ****/
           break;
#endif
#if 1
        case 6: /* noise : frequency, mode */
           {
           int n = psg->Register[r] = data & 0x0f;
           //Msg(MSGT_USER, "Noise frequency Set");
           psg->NoiseFB = (n & 4) ? FB_WNOISE : FB_PNOISE;
           n &= 3;
           /* N/512,N/1024,N/2048,Tone #3 output */
           psg->Period[3] = (n == 3) ? 2 * psg->Period[2] : (psg->UpdateStep << (5 + n));

           // Reset Noise Shift Register
           psg->RNG = NG_PRESET;
           psg->Output [3] = psg->RNG & 1;
           }
           break;
#endif
        }
     }
}
//-----------------------------------------------------------------------------

// PSG_Update_16() ------------------------------------------------------------
// Generate PSG sound in given buffer
//-----------------------------------------------------------------------------
// 8-Bits version:
// #define DATATYPE     unsigned char
// #define DATACONV(A)  AUDIO_CONV((A) / (STEP * 256))
// 16-Bits version
#define DATATYPE        short
#define DATACONV(A)     ((A) / STEP)

void            PSG_Update_16 (int chip, void *buffer, int length)
{
 int           i, length_left;
 DATATYPE     *buf = (DATATYPE *)buffer;
 t_psg        *psg = &PSG[chip];

 // If the volume is 0, increase the counter directly -------------------------
 // FIXME ?
 for (i = 0; i < 4; i++)
     {
     if (psg->Volume[i] == 0)
        {
        // note that I do count += length, NOT count = length + 1. You might
        // think it's the same since the volume is 0, but doing the latter
        // could cause interferencies when the program is rapidly modulating the volume.
        if (psg->Count[i] <= (length * STEP))
            psg->Count[i] += (length * STEP);
        }
     }

 // Process with wave synthesis and fill buffer -------------------------------
 for (length_left = length; length_left > 0; length_left --)
     {
     int          vol[4];
     int          left;
     int          out;

     // vol[] keeps track of how long each square wave stays
     // in the 1 position during the sample period.
     vol[0] = vol[1] = vol[2] = -0x8000;
     vol[3] = 0;

     // Processing tone channels ----------------------------------------------
     for (i = 0; i < 3; i ++)
         {
         // Add what's left from the old count
         if (psg->Output[i])
            vol[i] += psg->Count[i];

         // Period[i] is the half period of the square wave. Here, in each
         // loop I add Period[i] twice, so that at the end of the loop the
         // square wave is in the same status (0 or 1) it was at the start.
         // vol[i] is also incremented by Period[i], since the wave has been 1
         // exactly half of the time, regardless of the initial position.
         // If we exit the loop in the middle, Output[i] has to be inverted
         // and vol[i] incremented only if the exit status of the square
         // wave is 1.

         /* if (psg->Period[i] <= 4)
            {
            vol[i] += psg->Count[i];
            }
         else */
            {
            psg->Count[i] -= STEP;
            while (psg->Count[i] <= 0)
               {
               psg->Count[i] += psg->Period[i];
               if (psg->Count[i] > 0)
                  {
                  psg->Output[i] ^= 1;
                  if (psg->Output[i])
                     vol[i] += psg->Period[i];
                  break;
                  }
               psg->Count[i] += psg->Period[i];
               vol[i] += psg->Period[i];
               }
            if (psg->Output[i])
               vol[i] -= psg->Count[i];
            }

         // if (vol[i] < 0) vol[i] = -0x8000; else vol[i] = 0x7FFF;
         }

     // Processing noise channel ----------------------------------------------
     left = STEP;
     if (!psg->Mute[3])
        do
        {
        int nextevent;

        if (psg->Count[3] < left)
           nextevent = psg->Count[3];
        else
           nextevent = left;

        if (psg->Output[3])
           vol[3] += psg->Count[3];
        psg->Count[3] -= nextevent;
        if (psg->Count[3] <= 0)
           {
           if ((psg->RNG == 0) || (psg->RNG & 1))
              psg->RNG ^= psg->NoiseFB;
           psg->RNG >>= 1;

           psg->Output[3] = psg->RNG & 1;
           psg->Count[3] += psg->Period[3];
           if (psg->Output[3])
              vol[3] += psg->Period[3];
           }
        if (psg->Output[3])
           vol[3] -= psg->Count[3];

        left -= nextevent;
        }
        while (left > 0);

     //     0 -- -0x8000
     //  5000 --  0x0000
     // 10000 --  0x7FFF

     // Mix channels ----------------------------------------------------------
     out = vol[0] * psg->Volume[0] + vol[1] * psg->Volume[1] +
           vol[2] * psg->Volume[2] + vol[3] * psg->Volume[3];
     out = DATACONV(out);

     // Limit output ----------------------------------------------------------
     if (out > MAX_OUTPUT)
         out = MAX_OUTPUT;
     if (out < -MAX_OUTPUT)
         out = -MAX_OUTPUT;

     // Write to stream -------------------------------------------------------
     *(buf++) = out;
     }

 // Write buffer to file if it's activated within MEKA
 if (Sound.LogWav)
    {
    fwrite (buffer, length, sizeof (DATATYPE), Sound.LogWav);
    Sound.LogWav_SizeData += length * sizeof (DATATYPE);
    }
}
#undef  DATATYPE
#undef  DATACONV
//-----------------------------------------------------------------------------

