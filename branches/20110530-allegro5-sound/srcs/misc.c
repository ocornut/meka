//-----------------------------------------------------------------------------
// MEKA - misc.c
// Miscellaneous - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_game.h"
#include "app_memview.h"
#include "capture.h"
#include "debugger.h"
#include "inputs_i.h"
#include "palette.h"
#include "sound/sound_logging.h"
#include "video.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Change_System_Misc ()
// Called when media (ROM) changes - updates various things
//-----------------------------------------------------------------------------
void    Change_System_Misc (void)
{
    gamebox_resize_all();
    Capture_Init_Game();
    Sound_Log_Init_Game();
    MemoryViewers_MediaReload();
#ifdef MEKA_Z80_DEBUGGER
    Debugger_MediaReload();
#endif // MEKA_Z80_DEBUGGER
}

// Called when the MEKA mode changes - updates various things
void    Change_Mode_Misc(void)
{
    switch (g_env.state)
    {
    case MEKA_STATE_GAME:
        {
            Palette_Emulation_Reload();
            Video_Clear();
            break;
        }
    case MEKA_STATE_GUI:
        // .. nothing to do ..
        break;
	}
    Inputs_Peripheral_Change_Update();
}

#ifdef ARCH_UNIX
#define ESC         "\x1b"
#define FGCOLOR(n)  ESC"[3"#n"m"
#define BGCOLOR(n)  ESC"[4"#n"m"
#define RESET       ESC"[0m"
#endif

// Show the ending ASCII message
void    Show_End_Message (void)
{
	// ANSI colors codes reminder:
	//  0: Black
	//  4: Red
	//  7: White
	//  8: Dark Gray
	// 15: Bright White

#ifdef ARCH_UNIX
	printf(" ");
	printf(BGCOLOR(1));
	printf(FGCOLOR(0));
	printf ("                           ");  
	printf (RESET "  %s (c) %s\n", MEKA_NAME_VERSION, MEKA_AUTHORS_SHORT);

	printf (" ");
	printf(BGCOLOR(1));
	printf(FGCOLOR(0));
	printf (FGCOLOR(0) " ");
	printf (FGCOLOR(7) "WONDER");
	printf (FGCOLOR(0) " ");
	printf (FGCOLOR(7) "BOY");
	printf (FGCOLOR(0) " ");
	printf (FGCOLOR(7) "III");
	printf (FGCOLOR(0) "            ");
	printf (RESET"  Built on %s at %s\n", MEKA_BUILD_DATE, MEKA_BUILD_TIME);

	printf (" ");
	printf(BGCOLOR(1));
	printf(FGCOLOR(0));
	printf ("                      ");
	printf (FGCOLOR(7) "SEGA");
	printf (FGCOLOR(0) " ");
	printf (RESET "  " MEKA_HOMEPAGE "\n");

	printf (" ");
	printf ("===========================");

	printf (RESET);
	printf ("\n");
#else
	ConsolePrintf (" %s (c) %s\n", MEKA_NAME_VERSION, MEKA_AUTHORS_SHORT);
	ConsolePrintf (" Built on %s at %s\n", MEKA_BUILD_DATE, MEKA_BUILD_TIME);
	ConsolePrintf (" " MEKA_HOMEPAGE "\n");
#endif
}

// Quit the application immediately
void    Quit(void)
{
    // Set text mode if we're not already in
    if (g_env.state != MEKA_STATE_INIT && g_env.state != MEKA_STATE_SHUTDOWN)
    {
		al_destroy_display(g_display);
		g_display = NULL;
        //set_gfx_mode (GFX_TEXT, 80, 25, 80, 25);
        // g_env.state = MEKA_STATE_SHUTDOWN;
        // Video_Setup_State ();
    }

    // Return back to starting directory
    chdir (g_env.Paths.StartingDirectory);

#ifdef ARCH_WIN32
    if (g_env.state == MEKA_STATE_INIT)
    {
        ConsoleEnablePause();
        ConsoleWaitForAnswer(FALSE);
    }
#endif

    // Exit application
    exit (1);
}

// Display an error message then quit the application
void	Quit_Msg (const char *format, ...)
{
    // Set text mode if we're not already in
    if (g_env.state != MEKA_STATE_INIT && g_env.state != MEKA_STATE_SHUTDOWN)
    {
		al_destroy_display(g_display);
		g_display = NULL;
        //set_gfx_mode (GFX_TEXT, 80, 25, 80, 25);
        // g_env.state = MEKA_STATE_SHUTDOWN;
        // Video_Setup_State ();
    }

    // Return back to starting directory
    chdir (g_env.Paths.StartingDirectory);

#ifdef ARCH_WIN32
    {
        // FIXME: should redirect on console
		char buffer[512];
		va_list params;
        va_start (params, format);
        vsprintf (buffer, format, params);
        va_end   (params);

        ConsolePrint(buffer);
        ConsoleEnablePause();

        if (g_env.state == MEKA_STATE_INIT)
            ConsoleWaitForAnswer(FALSE);
        else
        {
            // Note: we don't use allegro_message() because Allegro might be unitialized here
            MessageBox (NULL, buffer, Msg_Get (MSG_Window_Title), MB_OK | MB_ICONINFORMATION);
        }
    }
#else
    {
		va_list params;
        va_start (params, format);
        vprintf  (format, params); // FIXME: use Console*
        va_end   (params);
        ConsolePrint ("\n");
    }
#endif

    // Force Allegro closing
    // Trying to fix the crash on quit.
    // Note that this call works even if Allegro has not been initialized yet 
    // (which may happens considering Quit_Msg() gets called during initialization)
    // allegro_exit();
    // remove_joystick();

    // Exit application
    exit (1);

}

//-----------------------------------------------------------------------------
