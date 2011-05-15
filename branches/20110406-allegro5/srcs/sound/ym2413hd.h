/*-----------------------------------*/
/* YM-2413 emulator using OPL        */
/* (c) by Hiromitsu Shioya           */
/* Modified by Omar Cornut           */
/*-----------------------------------*/

#ifndef __YM2413HD_H__
#define __YM2413HD_H__

//-----------------------------------------------------------------------------
// Instrument Data
//-----------------------------------------------------------------------------
// FIXME: Currently placed outside of the MEKA_OPL test, as it is being
// used by the FM Editor.
//-----------------------------------------------------------------------------

struct FM_OPL_Patch
{
  unsigned char MKS, CKS;       /* KSL                   */
  unsigned char MML, CML;       /* MULTIPLE              */
  unsigned char MA,  CA;        /* ATTACK RATE           */
  unsigned char MSL, CSL;       /* SUSTAIN LEVEL         */
  unsigned char MS,  CS;        /* EG                    */
  unsigned char MD,  CD;        /* DECAY RATE            */
  unsigned char MR,  CR;        /* RELEASE RATE          */
  unsigned char MTL, CTL;       /* TOTAL LEVEL           */
  unsigned char MEV, CEV;       /* KSR                   */
  unsigned char MW,  CW;        /* WAVE FORM             */
  unsigned char FB,  CON;       /* FEEDBACK / Connection */
};

//-----------------------------------------------------------------------------

#ifdef MEKA_OPL

//-----------------------------------------------------------------------------

// Registers
extern byte    FM_OPL_Regs [YM2413_REGISTERS];
#define FM_OPL_Rhythm_Mode (FM_OPL_Regs [0x0E] & 0x20)

// Functions
int     FM_OPL_Init             (void *userdata);
void    FM_OPL_Close            (void);
void    FM_OPL_Active           (void);
void    FM_OPL_Update           (void);
void    FM_OPL_Set_Voice        (int R, int V, int VL);
void    FM_OPL_Set_User_Voice   (void);

// Interface (see FMUNIT.C/.H)
void    FM_OPL_Reset            (void);
void    FM_OPL_Write            (int Register, int Value);
void    FM_OPL_Mute             (void);
void    FM_OPL_Resume           (void);
void    FM_OPL_Regenerate       (void);

//-----------------------------------------------------------------------------

#else

// A fake set of registers is created as sound/fmunit.c reference it.
// FIXME: This sucks.
extern byte    FM_OPL_Regs [YM2413_REGISTERS];

#endif

#endif /* !__YM2413HD_H__ */

