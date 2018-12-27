//-----------------------------------------------------------------------------
// MEKA - S_MISC.C
// Sound Miscellaneous - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "psg.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void		Sound_Volume_Menu_Handler(t_menu_event *event);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

int		Sound_Rate_Default_Table[] =
{ 
	22050, 
	44100, 
	-1
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    FM_Disable (void)
{
    Sound.FM_Enabled = FALSE;
    Msg(MSGT_USER, "%s", Msg_Get(MSG_FM_Disabled));
    Msg(MSGT_USER_BOX, "%s", Msg_Get(MSG_Must_Reset));
    gui_menu_uncheck_range (menus_ID.fm, 0, 1);
    gui_menu_check (menus_ID.fm, 1);
}

void    FM_Enable (void)
{
    Sound.FM_Enabled = TRUE;
    Msg(MSGT_USER, "%s", Msg_Get(MSG_FM_Enabled));
    Msg(MSGT_USER_BOX, "%s", Msg_Get(MSG_Must_Reset));
    gui_menu_uncheck_range (menus_ID.fm, 0, 1);
    gui_menu_check (menus_ID.fm, 0);
}

// SOUND->VOLUME menu ---------------------------------------------------------

void    Sound_Volume_Menu_Init(int menu_id)
{
    const int master_volume_100 = (float)Sound.MasterVolume * ((float)100 / 128);
    for (int i = 0; i <= 100; i += 20)
    {
		char buffer[256];
        if (i == 0)
            snprintf(buffer, countof(buffer), "%s", Msg_Get(MSG_Menu_Sound_Volume_Mute));
        else
            snprintf(buffer, countof(buffer), Msg_Get(MSG_Menu_Sound_Volume_Value), i);
        menu_add_item(menu_id, buffer, NULL, Is_Checked(i - 9 < master_volume_100 && i + 9 > master_volume_100), 
			(t_menu_callback)Sound_Volume_Menu_Handler, (void *)(int)((float)i * ((float)128 / 100)));
    }
}

void    Sound_Volume_Menu_Handler(t_menu_event *event)
{
	const int volume = (long int)event->user_data;

	Sound_SetMasterVolume(volume);
    Msg(MSGT_USER /*_BOX*/, Msg_Get(MSG_Sound_Volume_Changed), volume);
    gui_menu_uncheck_all (menus_ID.volume);
	gui_menu_check (menus_ID.volume, event->menu_item_idx);
}

// SOUND->CHANNELS menu -------------------------------------------------------

void    Sound_Channels_Menu_Handler(t_menu_event *event)
{
	const int channel_idx = (long int)event->user_data;

    PSG.Channels[channel_idx].Active ^= 1;
    gui_menu_toggle_check (menus_ID.channels, channel_idx);
}

