//-----------------------------------------------------------------------------
// MEKA - tvtype.c
// TV Types emulation (NTSC/PAL/SECAM) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "tvtype.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_tv_type * TV_Type_User = NULL;

t_tv_type TV_Type_Table [] =
{
    { TVTYPE_NTSC,      "NTSC",       262,  60,  3579540 }, // 262 lines @ 60 Hz, 3.579540 MHz
    { TVTYPE_PAL_SECAM, "PAL/SECAM",  313,  50,  3546893 }  // 313 lines @ 50 Hz, 3.546893 MHz
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TVType_Init_Values (void)
{
    TV_Type_User = &TV_Type_Table[TVTYPE_NTSC];
    g_machine.TV = TV_Type_User;
    g_machine.TV_lines = TV_Type_User->screen_lines;
    TVType_Update_Values();
}

void    TVType_Update_Values (void)
{
    // FIXME: Update VGM stuff
}

void    TVType_Set (int tv_type, bool verbose)
{
    TV_Type_User = &TV_Type_Table[tv_type];
    g_machine.TV = TV_Type_User;
    g_machine.TV_lines = TV_Type_User->screen_lines;

    // FIXME: CPU_Clock_Current is not taken into account for IPeriod in CPU emulation

    // 262 * 228 = 59736, * 60 = 3584160
    // 313 * 228 = 71364, * 50 = 3568200

    // SN76489_SetClock(opt.TV_Lines_Current * opt.Cur_IPeriod); // 59736 for NTSC
    // SN76489_SetClock(g_machine.TV->CPU_clock);
	Sound_UpdateClockSpeed();

    if (Sound.LogVGM.Logging == VGM_LOGGING_ACCURACY_SAMPLE)
        VGM_Update_Timing (&Sound.LogVGM);

    if (verbose)
    {
        // Print message & and update GUI checks
        Msg(MSGT_USER, Msg_Get(MSG_TVType_Set), TV_Type_User->name);
        Msg(MSGT_USER_BOX, Msg_Get(MSG_TVType_Info_Speed), TV_Type_User->screen_frequency);
        gui_menu_uncheck_all (menus_ID.tvtype);
        gui_menu_check (menus_ID.tvtype, tv_type);
        // Note that GUI checks are NOT updated if verbose mode is not set.
        // The reason is the parameters in MEKA.NAM can force a TV type, but we don't
        // want the user to be notified by that (unless he manually reoverride it).
    }
}

void    TVType_Set_NTSC (void)
{
    TVType_Set (TVTYPE_NTSC, TRUE);
}

void    TVType_Set_PAL_SECAM (void)
{
    TVType_Set (TVTYPE_PAL_SECAM, TRUE);
}

//-----------------------------------------------------------------------------

