//-----------------------------------------------------------------------------
// MEKA - S_MISC.C
// Sound Miscellaenous - Headers
//-----------------------------------------------------------------------------

#ifndef __S_MISC_H__
#define __S_MISC_H__

double  Sound_Calc_CPU_Time (void);
int     sound_vcount, sound_icount;
int     Sound_Update_Count;

void    FM_Disable                      (void);
void    FM_Enable                       (void);
void    FM_Emulator_OPL                 (void);
void    FM_Emulator_Digital             (void);

void    Sound_Volume_Menu_Init          (int menu_id);
void    Sound_Volume_Menu_Handler       (int pos);

void    Sound_Rate_Set                  (int value, int reinit_hardware);
void    Sound_Rate_Menu_Init            (int menu_id);
void    Sound_Rate_Menu_Handler         (int pos);
int     Sound_Rate_Default_Table[4];    // FIXME

void    Sound_Channels_Menu_Handler     (int channel);

#endif /* __S_MISC_H__ */

