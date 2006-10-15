//-----------------------------------------------------------------------------
// MEKA - S_MISC.C
// Sound Miscellaenous - Code
//-----------------------------------------------------------------------------

#include "shared.h"

// OLD STUFFS -----------------------------------------------------------------

int  Sound_Rate_Default_Table[4] =
{ 11025, 22050, 44100, -1 };

//
// Note from OMAR:
// IPeriod is added to Sound_Update_Count every line
// --------------------------------------------
// Some games using voices...
// --------------------------------------------
// AFTBURNR.SMS : After Burner
// AKIDD-LS.SMS : Alex Kidd The Lost Stars
// ARCADHIT.SMS : Arcade Smash Hit
// IMPOMISS.SMS : Impossible Mission
// MK2     .SMS : Mortal Kombat 2
// SAILORM .GG  : Sailor Moon S
// SEGACHES.SMS : Sega Chess
// SONICBLA.GG  : Sonic Blast
// SONICCHA.GG  : Sonic Chaos
// SHARRIER.SMS : Space Harrier
// SFIGHTR2.SMS : Street Fighter 2'
// TENNISAC.SMS : Tennis Ace
// --------------------------------------------
//

double  Sound_Calc_CPU_Time (void)
{
  int CPU_ICount = Get_ICount; // - Sound_Update_Count;
  int CPU_IPeriod = Get_IPeriod;

  // IPeriod : 228
  // ICount  : 228.. 227.. 226.. [..] .. 3.. 2.. 1.. 0..
  // Cycle elapsed in the period : IPeriod-ICount
  // Cycle left in the period    : ICount

/*
    {
    int now;
    sound_vcount = opt.Cur_IPeriod * opt.Cur_TV_Lines; //CPU_CLOCK/60;
    now = ((tsms.VDP_Line + 1) * CPU_IPeriod - CPU_ICount);
    now %= sound_vcount;
    return ((double)now / (double)sound_vcount);
    }
*/
    {
    int ic = CPU_IPeriod - CPU_ICount; // + Sound_Update_Count;

    // Number of cycle for a vertical refresh
    // sound_vcount = opt.TV_Lines_Current * opt.Cur_IPeriod;
    sound_vcount = 262 * opt.Cur_IPeriod; // FIXME
    // sound_vcount = opt.CPU_Clock_Current / 60;
    sound_icount = sound_icount + ic + Sound_Update_Count;
    sound_icount %= sound_vcount;
    Sound_Update_Count = -ic;
    return ((double)sound_icount / (double)sound_vcount);
    }
}

void    FM_Disable (void)
{
    Sound.FM_Enabled = NO;
    Msg (MSGT_USER, Msg_Get (MSG_FM_Disabled));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Must_Reset));
    gui_menu_un_check_area (menus_ID.fm, 0, 1);
    gui_menu_check (menus_ID.fm, 1);
}

void    FM_Enable (void)
{
    Sound.FM_Enabled = YES;
    Msg (MSGT_USER, Msg_Get (MSG_FM_Enabled));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Must_Reset));
    gui_menu_un_check_area (menus_ID.fm, 0, 1);
    gui_menu_check (menus_ID.fm, 0);
}

void    FM_Emulator_OPL (void)
{
#ifdef MEKA_OPL
    Sound.FM_Emulator_Current = FM_EMULATOR_YM2413HD;
    FM_OPL_Active ();
    // FIXME: message
    gui_menu_un_check_area (menus_ID.fm_emu, 0, 1);
    gui_menu_check (menus_ID.fm_emu, 0);
#else
    Msg (MSGT_DEBUG, "FM_Emulator_OPL() called while MEKA_OPL is not defined!");
#endif
}

void    FM_Emulator_Digital (void)
{
    Sound.FM_Emulator_Current = FM_EMULATOR_EMU2413;
    FM_Digital_Active ();
    // FIXME: message
    gui_menu_un_check_area (menus_ID.fm_emu, 0, 1);
    gui_menu_check (menus_ID.fm_emu, 1);
}

// SOUND->VOLUME menu ---------------------------------------------------------

void    Sound_Volume_Menu_Init (int menu_id)
{
    int    i;
    int    master_volume_100;

    master_volume_100 = (float)Sound.MasterVolume * ((float)100 / 128);
    for (i = 0; i <= 100; i += 20)
    {
        if (i == 0)
            sprintf (GenericBuffer, Msg_Get (MSG_Menu_Sound_Volume_Mute));
        else
            sprintf (GenericBuffer, Msg_Get (MSG_Menu_Sound_Volume_Value), i);
        menu_add_item  (menu_id, GenericBuffer, AM_Nothing | Is_Checked (i - 9 < master_volume_100 && i + 9 > master_volume_100), Sound_Volume_Menu_Handler);
    }
}

void    Sound_Volume_Menu_Handler (int pos)
{
    Sound_MasterVolume_Set ((float)(pos * 20) * ((float)128 / 100));
    Msg (MSGT_USER /*_BOX*/, Msg_Get (MSG_Sound_Volume_Changed), pos * 20);
    gui_menu_un_check (menus_ID.volume);
    gui_menu_check (menus_ID.volume, pos);
}

// SOUND->RATE menu -----------------------------------------------------------

void    Sound_Rate_Menu_Init (int menu_id)
{
    int    i;
    int    found;

    found = NO;
    for (i = 0; Sound_Rate_Default_Table[i] != -1; i++)
    {
        sprintf (GenericBuffer, Msg_Get (MSG_Menu_Sound_Rate_Hz), Sound_Rate_Default_Table [i]);
        menu_add_item (menus_ID.rate, GenericBuffer, AM_Active, Sound_Rate_Menu_Handler);
        if (Sound_Rate_Default_Table[i] == Sound.SampleRate)
            found = YES;
    }
    Sound_Rate_Set (Sound.SampleRate, NO);
}

void    Sound_Rate_Set (int value, int reinit_hardware)
{
    int    i;

    if (reinit_hardware)
    {
        Sound.SampleRate = value;
        Msg (MSGT_USER /*_BOX*/, Msg_Get (MSG_Sound_Rate_Changed), Sound.SampleRate);
    }
    gui_menu_un_check(menus_ID.rate);
    for (i = 0; Sound_Rate_Default_Table[i] != -1; i++)
        if (value == Sound_Rate_Default_Table[i])
        {
            gui_menu_check (menus_ID.rate, i);
            break;
        }
}

void    Sound_Rate_Menu_Handler (int pos)
{
    int    i;

    for (i = 0; Sound_Rate_Default_Table[i] != -1; i++)
        if (i == pos)
        {
            Sound_Rate_Set (Sound_Rate_Default_Table[i], YES);
            return;
        }
        Msg (MSGT_DEBUG, "Error #3731 - Please contact me!");
}

// SOUND->CHANNELS menu -------------------------------------------------------

void    Sound_Channels_Menu_Handler (int channel)
{
    PSG.Channels[channel].Active ^= 1;
    gui_menu_inverse_check (menus_ID.channels, channel);
}

