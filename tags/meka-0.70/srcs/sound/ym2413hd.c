/*-----------------------------------*/
/* YM-2413 emulator using OPL        */
/* (c) by Hiromitsu Shioya           */
/* Modified by Omar Cornut           */
/*-----------------------------------*/

#include "shared.h"

//-----------------------------------------------------------------------------
// YM-2413 Instruments Table
//-----------------------------------------------------------------------------
// FIXME: Currently placed outside of the MEKA_OPL test, as it is being
// used by the FM Editor.
//-----------------------------------------------------------------------------

FM_OPL_Patch    FM_OPL_Patchs[YM2413_INSTRUMENTS] =
{
  /*KSL | MUL | AR  | SL  | EG  | DR  | RR  | TL  | KSR | WAVE| FB/CN  */
  { 1, 0, 1, 1,15, 8, 9, 8, 0, 0, 1, 4,13,14,50,63, 0, 1, 0, 0, 4, 0,}, /*  0: User voice       */
  { 0, 0, 1, 2,15,15,14,15, 1, 1, 0, 0, 8, 7,35,63, 1, 0, 0, 0, 7, 0,}, /*  1: Violin           */
  { 1, 0, 3, 1,15, 7, 7, 9, 0, 1, 2, 4, 9,12,40,63, 0, 1, 0, 0, 5, 0,}, /*  2: Guitar           */
  { 1, 0, 1, 1,15,13, 7, 7, 0, 0, 2, 2, 7,11,50,63, 0, 1, 0, 0, 4, 0,}, /*  3: Piano            */
  { 1, 0, 4, 1,12,10,15,14, 1, 1, 0, 1, 8, 8,27,63, 0, 0, 0, 0, 5, 0,}, /*  4: Flute            */
  { 1, 0, 4, 1,13,10,10,14, 1, 1, 2, 1, 7, 8,44,63, 0, 0, 0, 0, 3, 0,}, /*  5: Clarinet         */
  { 2, 0, 1, 2,15,15,15,14, 1, 1, 1, 1, 4,14,58,63, 1, 0, 0, 0, 1, 0,}, /*  6: Oboe             */
  { 0, 0, 1, 1,15,15,12, 9, 1, 1, 1, 1,14,11,40,63, 0, 0, 0, 0, 6, 0,}, /*  7: Trumpet          */
  { 1, 0, 5, 1,15,15,15,15, 1, 1, 0, 0,15,13,32,63, 0, 0, 0, 0, 0, 0,}, /*  8: Organ            */
  { 0, 0, 0, 1, 8,15, 7, 9, 1, 1, 5, 1, 9,13,43,63, 0, 0, 0, 0, 6, 0,}, /*  9: Tube             */
  { 2, 0, 1, 1, 5, 6, 5,10, 1, 1, 3, 2,14,15,63,63, 1, 0, 1, 0, 2, 0,}, /* 10: Synthesizer      */
  { 1, 0, 3, 1,13,10, 9,11, 0, 1, 4, 3, 4,14,48,63, 0, 0, 1, 0, 7, 0,}, /* 11: Harpsicode       */
  { 2, 0,11, 1,15, 9,11,10, 1, 1, 6, 3,13,12,36,63, 0, 0, 0, 0, 7, 0,}, /* 12: Vibraphone       */
  { 2, 0, 1, 3, 7,15,12,14, 1, 1, 7, 1, 7, 8,45,63, 0, 0, 0, 0, 7, 0,}, /* 13: Synth bass       */
  { 0, 0, 1, 1,15,14,11,13, 1, 1, 3, 2, 6, 9,42,63, 0, 0, 0, 0, 3, 0,}, /* 14: Wood bass        */
  { 0, 0, 1, 3,15,14,11,13, 1, 1, 3, 2, 6, 9,55,63, 0, 0, 0, 0, 3, 0,}, /* 15: Electric bass    */
};

int     vcref[9] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
int     vlref[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
word    fref[9];

//-----------------------------------------------------------------------------

#ifdef MEKA_OPL

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

int                     FM_OPL_Initialized = NO;
t_fm_unit_interface     FM_OPL_Interface =
{
  "YM-2413 OPL Emulator",
  "Hiromitsu Shioya & Omar Cornut",
  FM_OPL_Reset,
  FM_OPL_Write,
  FM_OPL_Mute,
  FM_OPL_Resume,
  FM_OPL_Regenerate
};

//-----------------------------------------------------------------------------
// Delayed writes stuff
//-----------------------------------------------------------------------------

#define DELAY_BUFFER_MAX        (60*3) /* max 3second */
#define DELAY_STOCK_MAX         (DELAY_BUFFER_MAX * 64)

typedef struct delay_rec
{
  BYTE  reg;
  BYTE  data;
} DelayRec;

unsigned int    fm_delay_size = 6;
unsigned int    fm_write_d, fm_update_d;
unsigned int    delay_point[DELAY_BUFFER_MAX];
unsigned int    w_delay;
DelayRec        delay_chip[DELAY_STOCK_MAX];

int     fmVol[YM2413_VOLUME_STEPS] =
{
#if 0
  0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x08, 0x0a,
  0x0c, 0x10, 0x14, 0x1c, 0x24, 0x2c, 0x34, 0x3f
#else
  0x00, 0x03, 0x06, 0x03, 0x09, 0x0c, 0x0f, 0x12,
  0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27, 0x2a
#endif
};

//-----------------------------------------------------------------------------
// FM_OPL_Init ()
// Initialize emulation (FIXME: not really necessary this way ?)
//-----------------------------------------------------------------------------
int     FM_OPL_Init (void *userdata /* unused */)
{
  ConsolePrintf ("%s ", Msg_Get (MSG_Sound_Init_YM2413_OPL));

  FM_OPL_Reset ();
  FM_OPL_Initialized = YES;

  ConsolePrintf ("%s\n", Msg_Get (MSG_Ok));
  return (MEKA_ERR_OK);
}

//-----------------------------------------------------------------------------
// FM_OPL_Close()
// Close emulation (actually only mute)
//-----------------------------------------------------------------------------
void    FM_OPL_Close (void)
{
  if (FM_OPL_Initialized)
     {
     FM_OPL_Mute ();
     // FM_OPL_Reset ();
     }
}

//-----------------------------------------------------------------------------
// FM_OPL_Active()
// Active this engine as being the current FM interface to use
//-----------------------------------------------------------------------------
void    FM_OPL_Active (void)
{
  FM_Set_Interface (&FM_OPL_Interface, FM_OPL_Regs);
}

//-----------------------------------------------------------------------------
// FM_OPL_Reset()
// Reset emulated YM-2413
//-----------------------------------------------------------------------------
void    FM_OPL_Reset (void)
{
  int    i;

  // printf("...OPL addr= %x\n", Sound.OPL_Address);

  // Clear all OPL registers
  for (i = 0; i < 255; i++)
      Sound_OPL_Write (i, 0x00);
  Sound_OPL_Write (0x01, 0x20);
  // Sound_OPL_Write (0xBD, 0x28);

  // Set all YM-2413 registers to zero
  for (i = 0; i < YM2413_REGISTERS; i++)
      FM_OPL_Regs[i] = 0x00;

  // Initialize delayed update system
  fm_write_d = fm_update_d = 0;
  w_delay = 0;
  for (i = 0; i < DELAY_BUFFER_MAX; i++)
      delay_point[i] = 0;

  // Initialize volume & voices
  for (i = 0; i < 9; i++)
      {
      vcref[i] = 0xff;
      vlref[i] = 0;
      }

  Sound_OPL_Write (0xbd, 0x00);
  for (i = 0; i < 3; i++)
      {
      Sound_OPL_Write(0x20 + i, 0x01);    Sound_OPL_Write(0x23 + i, 0x01);
      Sound_OPL_Write(0x40 + i, 0x3f);    Sound_OPL_Write(0x43 + i, 0x3f);
      Sound_OPL_Write(0x60 + i, 0xf0);    Sound_OPL_Write(0x63 + i, 0xf0);
      Sound_OPL_Write(0x80 + i, 0xff);    Sound_OPL_Write(0x83 + i, 0xff);
      Sound_OPL_Write(0xc0 + i, 0x00);

      Sound_OPL_Write(0x28 + i, 0x01);    Sound_OPL_Write(0x2b + i, 0x01);
      Sound_OPL_Write(0x48 + i, 0x3f);    Sound_OPL_Write(0x4b + i, 0x3f);
      Sound_OPL_Write(0x68 + i, 0xf0);    Sound_OPL_Write(0x6b + i, 0xf0);
      Sound_OPL_Write(0x88 + i, 0xff);    Sound_OPL_Write(0x8b + i, 0xff);
      Sound_OPL_Write(0xc3 + i, 0x00);

      Sound_OPL_Write(0x30 + i, 0x21);    Sound_OPL_Write(0x33 + i, 0x21);
      Sound_OPL_Write(0x50 + i, 0x3f);    Sound_OPL_Write(0x53 + i, 0x3f);
      Sound_OPL_Write(0x70 + i, 0xf0);    Sound_OPL_Write(0x73 + i, 0xf0);
      Sound_OPL_Write(0x90 + i, 0xf0);    Sound_OPL_Write(0x93 + i, 0xf0);
      Sound_OPL_Write(0xc6 + i, 0x00);
      }

  // Sound_OPL_Write (0x01, 0x20);
}

//-----------------------------------------------------------------------------
// FM_OPL_Mute()
// Mute FM Sound by setting all OPL volumes to zero
//-----------------------------------------------------------------------------
void    FM_OPL_Mute (void)
{
  int    i;

  // Msg (MSGT_DEBUG, __FUNCTION__);
  for (i = 0; i < 9; i++)
      {
      // Clear bit 5 of all channels (makes voice silent)
      Sound_OPL_Write (0xb0 + i, 0x00);
      /*
      int vl = vlref[i];
      FM_OPL_Set_Voice (i, 0xff, 0x3f); // All sounds off.
      vlref[i] = vl;
      */
      }
}

//-----------------------------------------------------------------------------
// FM_OPL_Resume()
// Resume FM Sound AFTER muting
//-----------------------------------------------------------------------------
void    FM_OPL_Resume (void)
{
  int    i, oldv;
  int    n_channels;

  // 6 or 9 channels -----------------------------------------------------------
  n_channels = (FM_OPL_Rhythm_Mode ? 6 : 9);
  for (i = 0; i < n_channels; i++)
      {
      oldv = vcref[i];
      vcref[i] = 0xff;
      FM_OPL_Set_Voice (i, oldv, vlref[i]);
      Sound_OPL_Write (0xa0 + i, fref[i] & 0xff);
      Sound_OPL_Write (0xb0 + i, ((fref[i] >> 8) & 0x1f) | ((FM_OPL_Regs[0x20 + i] & 0x10) << 1));
      }
  // 3 Rythmic channels (if enabled) -------------------------------------------
  if (FM_OPL_Rhythm_Mode)
     {
     for (i = 6; i < 9; i++)
         {
         Sound_OPL_Write (0xa0 + i, fref[i] & 0xff);
         Sound_OPL_Write (0xb0 + i, (fref[i] >> 8) & 0x1f);
         }
     Sound_OPL_Write (0x53, fmVol[FM_OPL_Regs[0x36] & 0x0f]);        // Bass drum
     Sound_OPL_Write (0x51, fmVol[(FM_OPL_Regs[0x37] >> 4) & 0x0f]); // Hi-hat
     Sound_OPL_Write (0x54, fmVol[FM_OPL_Regs[0x37] & 0x0f]);        // Snare
     Sound_OPL_Write (0x52, fmVol[(FM_OPL_Regs[0x38] >> 4) & 0x0f]); // Tomtom
     Sound_OPL_Write (0x55, fmVol[FM_OPL_Regs[0x38] & 0x0f]);        // Top Cymbal
     }
}

//-----------------------------------------------------------------------------
// FM_OPL_Regenerate()
// Regenerate various data from YM-2413 registers
// This is called after a state loading
//-----------------------------------------------------------------------------
void    FM_OPL_Regenerate (void)
{
  int    i;

  // Initialize delayed update system
  fm_write_d = fm_update_d = 0;
  w_delay = 0;
  for (i = 0; i < DELAY_BUFFER_MAX; i++)
      delay_point[i] = 0;

  // Rewrite all registers
  for (i = 0; i < YM2413_REGISTERS; i++)
     {
     FM_OPL_Write (i, FM_OPL_Regs[i]);
     }
}

/*******************************************************************/
/*    FM voice set                                                 */
/*    note : YM3812 register map                                   */
/*                                                                 */
/*   9 voices map                                                  */
/*   | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  |                */
/*   ----------------------------------------------                */
/*   | 00 | 01 | 02 | 08 | 09 | 0a | 10 | 11 | 12 |                */
/*   | 03 | 04 | 05 | 0b | 0c | 0d | 13 | 14 | 15 |                */
/*                                                                 */
/*   6 voices & 5 rhythm map                                       */
/*   | 0  | 1  | 2  | 3  | 4  | 5  || BD | HH | TOM| SD | CYM|     */
/*   ---------------------------------------------------------     */
/*   | 00 | 01 | 02 | 08 | 09 | 0a || 10 | 11 | 12 | 14 | 15 |     */
/*   | 03 | 04 | 05 | 0b | 0c | 0d || 13 |    |    |    |    |     */
/*                                                                 */
/*   BD=Bass drum, HH=HiHat, TOM=Tomtom, SD=Snare drum             */
/*   CYM=Cymbal                                                    */
/*******************************************************************/
void            FM_OPL_Set_Voice (int R, int V, int VL)
{
  int           R2;
  FM_OPL_Patch *patch;

  R2 = R;
  R = (R % 3) + ((R / 3) * 0x08);
  V &= 0xff;
  patch = &FM_OPL_Patchs[vcref[R2]];
  vlref[R2] = VL & 0x3f;
  if (V != 0xff)
     {
     if ((V == 0) || (vcref[R2] != V))
        {
        vcref[R2] = V;
        patch = &FM_OPL_Patchs[V];
        Sound_OPL_Write(0x20 + R, (patch->MS <<5) | (patch->MEV<<4) | patch->MML);
        Sound_OPL_Write(0x60 + R, (patch->MA <<4) | patch->MD);
        Sound_OPL_Write(0x80 + R, (patch->MSL<<4) | patch->MR);
        Sound_OPL_Write(0xe0 + R, patch->MW);
        Sound_OPL_Write(0x23 + R, (patch->CS <<5) | (patch->CEV<<4) | patch->CML);
        Sound_OPL_Write(0x63 + R, (patch->CA <<4) | patch->CD);
        Sound_OPL_Write(0x83 + R, (patch->CSL<<4) | patch->CR);
        Sound_OPL_Write(0xe3 + R, patch->CW);
        Sound_OPL_Write(0xc0 + R2, (patch->FB<<1) | patch->CON);
        Sound_OPL_Write(0x40 + R, (0x3f - patch->MTL) | (patch->MKS<<6));
        }
     }
  Sound_OPL_Write(0x43 + R, (patch->CKS << 6) | vlref[R2]);
}

/***********************************************/
/* check use user-voice & set                  */
/***********************************************/
void    FM_OPL_Set_User_Voice (void)
{
  int   c, lpmax;

  lpmax = (FM_OPL_Rhythm_Mode) ? 6 : 9;
  for (c = 0; c < lpmax; c ++)
      {
      if (!vcref[c])
         FM_OPL_Set_Voice (c, 0, vlref[c]);
      // if( !vcref[c] )  vcref[c] = 0xff; /* set direct write */
      }
}

//-----------------------------------------------------------------------------
// FM_OPL_Update()
// Update audio stream with the delayed writes
// This is periodically (~1/60th second) called by the sound engine
//-----------------------------------------------------------------------------
void    FM_OPL_Update (void)
{
  unsigned int i;
  unsigned int start, end, stock;

  if (fm_delay_size)
     {
     i = fm_write_d - fm_update_d;
     fm_write_d++;
     if (i < fm_delay_size)
        {
        delay_point[(fm_write_d+1) % DELAY_BUFFER_MAX] = delay_point[fm_write_d];
        return;
        }
     start = delay_point[fm_update_d % DELAY_BUFFER_MAX];
     fm_update_d++;
     end   = delay_point[fm_update_d % DELAY_BUFFER_MAX];
     stock = fm_delay_size;
     fm_delay_size = 0;
     // Message( MESSAGE_DEBUG, " now delay write %d %d (%d)[%d]", start, end, end - start, fm_update_d );
     // printf( " now delay write %d %d (%d)[%d]\n", start, end, end - start, fm_update_d );
     if (end >= start)
        {
        for (i = start; i < end; i++)
            FM_OPL_Write (delay_chip[i % DELAY_STOCK_MAX].reg & 0x3f, delay_chip[i % DELAY_STOCK_MAX].data & 0xff);
        }
     #if 0
     else
        {
        printf(" ??? %d %d (%d)[%d]\n", start, end, end - start, fm_update_d);
        }
     #endif
     fm_delay_size = stock;
     fm_write_d  %= DELAY_BUFFER_MAX;
     fm_update_d %= DELAY_BUFFER_MAX;
     delay_point[(fm_write_d + 1) % DELAY_BUFFER_MAX] = delay_point[fm_write_d];
     }
}

//-----------------------------------------------------------------------------
// FM_OPL_Write()
// Port write to the YM-2413
//-----------------------------------------------------------------------------
void    FM_OPL_Write (int R, int V)
{
  int   Freq, c;
  int   F;
  int   Previous_Register;
  FM_OPL_Patch *patch = &FM_OPL_Patchs[0];

  // FIXME: is the first test necessary ?
  // Note: the second is necessary, when an FM_OPL_Update() is called by the
  // sound engine, while the FM emulator has already been switched to another!
  if (FM_OPL_Initialized == NO || Sound.FM_Emulator_Current != FM_EMULATOR_YM2413HD)
     {
     FM_OPL_Regs[R] = V;
     return;
     }

  if (fm_delay_size)
     {
     delay_chip[w_delay].reg  = (BYTE)R;
     delay_chip[w_delay].data = (BYTE)V;
     w_delay = (w_delay + 1) % DELAY_STOCK_MAX;
     delay_point[(fm_write_d+1) % DELAY_BUFFER_MAX] = w_delay;
     // printf( "%d = %d\n", fm_write_d+1, delay_point[(fm_write_d+1)%DELAY_BUFFER_MAX] );
     return;
     }

  Previous_Register = FM_OPL_Regs[R];
  FM_OPL_Regs[R] = V;

  switch (R)
     {
     /**** user voice set ****/
     case 0x00:
       patch->MML = V&0x0f;    patch->MS  = (V>>5)&0x01;    patch->MEV = (V>>4)&0x01;
       FM_OPL_Set_User_Voice ();
       return;
     case 0x01:
       patch->CML = V&0x0f;    patch->CS  = (V>>5)&0x01;    patch->CEV = (V>>4)&0x01;
       FM_OPL_Set_User_Voice ();
       return;
     case 0x02:
       patch->MTL = 0x3f - (V&0x3f);    patch->MKS = (V>>6)&0x03;
       FM_OPL_Set_User_Voice ();
       return;
     case 0x03:
       patch->FB  = V&0x07;             patch->MW  = (V>>4)&0x01;
       patch->CW  = (V>>3)&0x01;        patch->MKS = (V>>6)&0x03;
       FM_OPL_Set_User_Voice ();
       return;
     case 0x04:
       patch->MA = (V>>4)&0x0f;         patch->MD = V&0x0f;
       FM_OPL_Set_User_Voice ();
       return;
     case 0x05:
       patch->CA = (V>>4)&0x0f;         patch->CD = V&0x0f;
       FM_OPL_Set_User_Voice ();
       return;
     case 0x06:
       patch->MSL = (V>>4)&0x0f;        patch->MR  = V&0x0f;
       FM_OPL_Set_User_Voice ();
       return;
     case 0x07:
       patch->CSL = (V>>4)&0x0f;        patch->CR  = V&0x0f;
       FM_OPL_Set_User_Voice ();
       return;
     case 0x0e:
       /**** rhythm select ****/
       if (FM_OPL_Rhythm_Mode)
          {
          // If it was previously desactived, setup OPL instruments
          if ((Previous_Register & 0x20) == 0x00)
             {
             for (c = 0; c < 3; c++)
                 {
                 Sound_OPL_Write (0xf0 + c, 0x00);    Sound_OPL_Write (0xf3 + c, 0x00);
                 Sound_OPL_Write (0xc6 + c, 0x00);
                 }
             Sound_OPL_Write (0x70, 0xf0);    Sound_OPL_Write (0x73, 0xf0);
             Sound_OPL_Write (0x71, 0xf0);    Sound_OPL_Write (0x74, 0xf0);
             Sound_OPL_Write (0x72, 0xf0);    Sound_OPL_Write (0x75, 0xf0);

             Sound_OPL_Write (0x30, 0x00);    Sound_OPL_Write (0x33, 0x01); /* bass */
             Sound_OPL_Write (0x31, 0x01);    Sound_OPL_Write (0x34, 0x01); /* hi-hat/snare */
             Sound_OPL_Write (0x32, 0x01);    Sound_OPL_Write (0x35, 0x01); /* tom/cymbal */

             if (!(FM_OPL_Regs[0x26] & 0x20))
                {
                Sound_OPL_Write (0x90, 0x07);    Sound_OPL_Write (0x93, 0x07);
                }
             else
                {
                Sound_OPL_Write (0x90, 0x04);    Sound_OPL_Write (0x93, 0x04);
                }
             if (!(FM_OPL_Regs[0x27] & 0x20))
                {
                Sound_OPL_Write (0x91, 0x07);    Sound_OPL_Write (0x94, 0x07);
                }
             else
                {
                Sound_OPL_Write (0x91, 0x04);    Sound_OPL_Write (0x94, 0x04);
                }
             if (!(FM_OPL_Regs[0x28] & 0x20))
                {
                Sound_OPL_Write (0x92, 0x06);    Sound_OPL_Write (0x95, 0x06);
                }
             else
                {
                Sound_OPL_Write (0x92, 0x04);    Sound_OPL_Write (0x95, 0x04);
                }
             Sound_OPL_Write (0x50, 0x0f);                                  Sound_OPL_Write (0x53, fmVol[FM_OPL_Regs[0x36]&0x0f] >> 1);
             Sound_OPL_Write (0x51, fmVol[(FM_OPL_Regs[0x37]>>4)&0x0f]);    Sound_OPL_Write (0x54, fmVol[FM_OPL_Regs[0x37]&0x0f]);
             Sound_OPL_Write (0x52, fmVol[(FM_OPL_Regs[0x38]>>4)&0x0f]);    Sound_OPL_Write (0x55, fmVol[FM_OPL_Regs[0x38]&0x0f]);

             Sound_OPL_Write (0xa6, fref[6]&0xff);    Sound_OPL_Write (0xb6, (fref[6]>>8) & 0x1f);
             Sound_OPL_Write (0xa7, fref[7]&0xff);    Sound_OPL_Write (0xb7, (fref[7]>>8) & 0x1f);
             Sound_OPL_Write (0xa8, fref[8]&0xff);    Sound_OPL_Write (0xb8, (fref[8]>>8) & 0x1f);
             }
          }
       Sound_OPL_Write (0xbd, V);
       return;
     }

  if (R >= 0x10 && R <= 0x28)
     {
     /**** Freq. set ****/
     F = R & 0x0f;
     Freq = (((int)FM_OPL_Regs[0x10+F] & 0x00ff) | (((int)FM_OPL_Regs[0x20+F] & 0x01) << 8)) << 1;
     c = (FM_OPL_Regs[0x20+F] >> 1) & 0x0007;
     if (FM_OPL_Regs[0x20+F] & 0x10) fref[F] = Freq | (c << 10) | 0x2000;
     else                            fref[F] = Freq | (c << 10);
     Sound_OPL_Write (0xa0+F, fref[F] & 0xff);
     Sound_OPL_Write (0xb0+F, (fref[F] >> 8) & 0xff);
     return;
     }

  if (R >= 0x30 && R < 0x36)
     {
     /**** set voice&volume (ch0-ch5) ****/
     FM_OPL_Set_Voice (R & 0x0f, (V >> 4) & 0x0f, fmVol[V & 0x0f]);
     return;
     }

  if (R >= 0x36 && R <= 0x38)
     {
     /**** set voice & volume (ch6-8 or rhythm) ****/
     if (!FM_OPL_Rhythm_Mode)
        {
        FM_OPL_Set_Voice (R & 0x0f, (V >> 4) & 0x0f, fmVol[V & 0x0f]);
        return;
        }
     switch (R & 0x0f)
       {
       case 6:
         Sound_OPL_Write (0x53, fmVol[V&0x0f]>>1);           /* bass drum */
         break;
       case 7:
         Sound_OPL_Write (0x51, fmVol[(V>>4)&0x0f]);         /* hi-hat */
         Sound_OPL_Write (0x54, fmVol[V&0x0f]);              /* snare  */
         break;
       case 8:
         Sound_OPL_Write (0x52, fmVol[(V>>4)&0x0f]);         /* tomtom */
         Sound_OPL_Write (0x55, fmVol[V&0x0f]);              /* top cymbal */
         break;
       }
     }
}

//-----------------------------------------------------------------------------

#endif // MEKA_OPL

