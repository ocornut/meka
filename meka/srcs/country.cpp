//-----------------------------------------------------------------------------
// MEKA - country.c
// Country / Nationalization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_filebrowser.h"
#include "app_game.h"
#include "vmachine.h"

//-----------------------------------------------------------------------------

// Also see stuff in CONFIG.C
// Note that country handling is quite weird because:

// If you have country in EUR/US mode, for example, and start
// Meka with /JAP it will run in JAP mode, but will keep your
// configuration in EUR/US unless you manually click on JAP.
// This is very useful, user wise.
// And that's the reason for _CL (CommandLine) stuff, too.

//-----------------------------------------------------------------------------

void    Set_Country(t_country country)
{
    if (country == COUNTRY_EXPORT)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Country_European_US));
    else if (country == COUNTRY_JAPAN)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Country_JAP));

    if (g_machine_flags & MACHINE_POWER_ON)
        Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Must_Reset));

    // Set config + emulation country
    g_config.country = country;
    sms.Country = g_config.country;

    // Update game boxes name and file browser
    gamebox_rename_all();
    FB_Reload_Names();
}

void    Nationalize (byte *v)
{
    if (sms.Country == COUNTRY_EXPORT)
        if ((tsms.Port3F & 0xF) == 0x5)
        {
            if ((tsms.Port3F & 0xF0) == 0xF0)
                *v |= 0xC0; // set bits 6 and 7
            else
                *v &= 0x3F; // clear bits 6 and 7
        }
}

//-----------------------------------------------------------------------------

