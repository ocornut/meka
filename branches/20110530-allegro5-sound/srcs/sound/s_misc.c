//-----------------------------------------------------------------------------
// MEKA - S_MISC.C
// Sound Miscellaenous - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void		Sound_Volume_Menu_Handler(t_menu_event *event);
static void		Sound_Rate_Menu_Handler(t_menu_event *event);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

int		Sound_Rate_Default_Table[4] =
{ 
	11025, 
	22050, 
	44100, 
	-1
};

// Global
int		sound_vcount = -1, sound_icount = -1;
int		Sound_Update_Count;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OLD STUFFS -----------------------------------------------------------------

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
  const int CPU_ICount = CPU_GetICount(); // - Sound_Update_Count;
  const int CPU_IPeriod = CPU_GetIPeriod();

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
    Sound.FM_Enabled = FALSE;
    Msg (MSGT_USER, Msg_Get (MSG_FM_Disabled));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Must_Reset));
    gui_menu_un_check_area (menus_ID.fm, 0, 1);
    gui_menu_check (menus_ID.fm, 1);
}

void    FM_Enable (void)
{
    Sound.FM_Enabled = TRUE;
    Msg (MSGT_USER, Msg_Get (MSG_FM_Enabled));
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Must_Reset));
    gui_menu_un_check_area (menus_ID.fm, 0, 1);
    gui_menu_check (menus_ID.fm, 0);
}

// SOUND->VOLUME menu ---------------------------------------------------------

void    Sound_Volume_Menu_Init (int menu_id)
{
    int    i;
    int    master_volume_100;
	char   buffer[256];

    master_volume_100 = (float)Sound.MasterVolume * ((float)100 / 128);
    for (i = 0; i <= 100; i += 20)
    {
        if (i == 0)
            snprintf(buffer, countof(buffer), Msg_Get(MSG_Menu_Sound_Volume_Mute));
        else
            snprintf(buffer, countof(buffer), Msg_Get(MSG_Menu_Sound_Volume_Value), i);
        menu_add_item(menu_id, buffer, AM_Nothing | Is_Checked (i - 9 < master_volume_100 && i + 9 > master_volume_100), 
			(t_menu_callback)Sound_Volume_Menu_Handler, (void *)(int)((float)i * ((float)128 / 100)));
    }
}

void    Sound_Volume_Menu_Handler (t_menu_event *event)
{
	const int volume = (int)event->user_data;

	Sound_MasterVolume_Set(volume);
    Msg (MSGT_USER /*_BOX*/, Msg_Get (MSG_Sound_Volume_Changed), volume);
    gui_menu_un_check (menus_ID.volume);
	gui_menu_check (menus_ID.volume, event->menu_item_idx);
}

// SOUND->RATE menu -----------------------------------------------------------

void    Sound_Rate_Menu_Init (int menu_id)
{
    int		i;
	char	buffer[256];

	for (i = 0; Sound_Rate_Default_Table[i] != -1; i++)
    {
        sprintf(buffer, Msg_Get(MSG_Menu_Sound_Rate_Hz), Sound_Rate_Default_Table [i]);
        menu_add_item(menus_ID.rate, buffer, AM_Active, (t_menu_callback)Sound_Rate_Menu_Handler, (void *)Sound_Rate_Default_Table[i]);
    }
    Sound_Rate_Set(Sound.SampleRate, FALSE);
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

void    Sound_Rate_Menu_Handler (t_menu_event *event)
{
	const int sound_rate = (int)event->user_data;
    Sound_Rate_Set (sound_rate, TRUE);
}

// SOUND->CHANNELS menu -------------------------------------------------------

void    Sound_Channels_Menu_Handler(t_menu_event *event)
{
	const int channel_idx = (int)event->user_data;

    PSG.Channels[channel_idx].Active ^= 1;
    gui_menu_inverse_check (menus_ID.channels, channel_idx);
}

