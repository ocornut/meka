//-----------------------------------------------------------------------------
// MEKA - S_MISC.C
// Sound Miscellaneous - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "psg.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

int     Sound_Rate_Default_Table[] =
{
    22050,
    44100,
    -1
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    FM_Disable()
{
    Sound.FM_Enabled = FALSE;
    Msg(MSGT_USER, "%s", Msg_Get(MSG_FM_Disabled));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Must_Reset));
}

void    FM_Enable()
{
    Sound.FM_Enabled = TRUE;
    Msg(MSGT_USER, "%s", Msg_Get(MSG_FM_Enabled));
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Must_Reset));
}


