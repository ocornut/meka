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
#include "video.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Close_Button_Callback ()
// Close button hook, called by Allegro
//-----------------------------------------------------------------------------
// FIXME-ALLEGRO5: Use ALLEGRO_EVENT_DISPLAY_CLOSE event
/*
void    Close_Button_Callback (void)
{
    if (Meka_State == MEKA_STATE_INIT || Meka_State == MEKA_STATE_SHUTDOWN)
        return;
    opt.Force_Quit = TRUE;
}
*/

//-----------------------------------------------------------------------------
// Switch_In_Callback ()
// Application switch-in hook, called by Allegro
// This is fired when the application screen reappear (eg: after an ALT-TAB)
//-----------------------------------------------------------------------------
// FIXME-ALLEGRO5: Use ALLEGRO_EVENT_DISPLAY_SWITCH_IN, ALLEGRO_EVENT_DISPLAY_SWITCH_OUT
/*
void    Switch_In_Callback (void)
{
    if (Meka_State == MEKA_STATE_INIT || Meka_State == MEKA_STATE_SHUTDOWN)
        return;
    // Msg (MSGT_USER, "Switch_In_Callback()");
    // clear_to_color (screen, Border_Color);
    Video_Clear ();
    Sound_Playback_Resume ();
}

//-----------------------------------------------------------------------------
// Switch_Out_Callback ()
// Application switch-out hook, called by Allegro
// This is fired when the application screen disappear (eg: using ALT-TAB)
//-----------------------------------------------------------------------------
void    Switch_Out_Callback (void)
{
    if (Meka_State == MEKA_STATE_INIT || Meka_State == MEKA_STATE_SHUTDOWN)
        return;
    // Msg (MSGT_USER, "Switch_Out_Callback()");
    Sound_Playback_Mute ();
}
*/

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

//-----------------------------------------------------------------------------
// Change_Mode_Misc ()
// Called when the MEKA mode changes - updates various things
//-----------------------------------------------------------------------------
void    Change_Mode_Misc (void)
{
    switch (Meka_State)
    {
    case MEKA_STATE_FULLSCREEN: // Fullscreen
        {
            Palette_Emulation_Reload();
            Video_Clear ();
            break;
        }
    case MEKA_STATE_GUI:
        // .. nothing to do ..
        break;
    }
    gui_mouse_show (NULL);
    //#ifdef ARCH_DOS
    //    Video_VGA_Set_Border_Color (Border_Color);
    //#endif
    Inputs_Peripheral_Change_Update ();
}

//-----------------------------------------------------------------------------
// Set_Mouse_Cursor(int mouse_cursor)
// Set current mouse cursor
//-----------------------------------------------------------------------------
// FIXME: Merge with Inputs_Peripheral_Change_Update() ?
//-----------------------------------------------------------------------------
void    Set_Mouse_Cursor(int mouse_cursor)
{
    if (g_Env.mouse_installed == -1)
        return;
    switch (mouse_cursor)
    {
    case MEKA_MOUSE_CURSOR_NONE: 
		// FIXME-ALLEGRO5: What to do?
        //set_mouse_sprite (NULL);
        break;
    case MEKA_MOUSE_CURSOR_STANDARD: 
		al_set_mouse_cursor(g_display, Graphics.Cursors.Main);
        break;
    case MEKA_MOUSE_CURSOR_LIGHT_PHASER: 
		al_set_mouse_cursor(g_display, Graphics.Cursors.LightPhaser);
        break;
    case MEKA_MOUSE_CURSOR_SPORTS_PAD: 
		al_set_mouse_cursor(g_display, Graphics.Cursors.SportsPad);
        break;
    case MEKA_MOUSE_CURSOR_TV_OEKAKI: 
		al_set_mouse_cursor(g_display, Graphics.Cursors.TvOekaki);
        break;
    case MEKA_MOUSE_CURSOR_WAIT: 
		al_set_mouse_cursor(g_display, Graphics.Cursors.Wait);
        break;
    }
}

#ifdef ARCH_UNIX
#define ESC         "\x1b"
#define FGCOLOR(n)  ESC"[3"#n"m"
#define BGCOLOR(n)  ESC"[4"#n"m"
#define RESET       ESC"[0m"
#endif

//-----------------------------------------------------------------------------
// Show_End_Message ()
// Show the ending ASCII message
//-----------------------------------------------------------------------------
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
#elif ARCH_DOS
    // Line 1 ------------------------------------------------------------------
    textbackground (4);     // Red background
    textcolor (0);          // Black text
    printf (" ");
    cprintf ("еееееееееееееееееееееееееее");
    printf ("  %s (c) %s\n", MEKA_NAME_VERSION, MEKA_AUTHORS_SHORT);
    // Line 2 ------------------------------------------------------------------
    printf (" ");
    cprintf ("е");
    textcolor (15); cprintf ("WONDER");
    textcolor (0);  cprintf ("е");
    textcolor (15); cprintf ("BOY");
    textcolor (0);  cprintf ("е");
    textcolor (15); cprintf ("III");
    textcolor (0);  cprintf ("ееееееееееее");
    printf ("  Built on %s at %s\n", MEKA_BUILD_DATE, MEKA_BUILD_TIME);
    // Line 3 ------------------------------------------------------------------
    printf (" ");
    cprintf ("ееееееееееееееееееееее");
    textcolor (15); cprintf ("SEGA");
    textcolor (0);  cprintf ("е");
    printf ("  " MEKA_HOMEPAGE "\n");
    // Line 4 ------------------------------------------------------------------
    textbackground (0); // Black background
    textcolor (8);      // Dark Gray text
    printf (" ");
    cprintf ("ммммммммммммммммммммммммммм");
    textcolor (7);      // Standard gray text
    /*
    if (!registered.is)
    {
        int i;
        char message [] = "  *Unregistered Version*";
        for (i = 0; i < strlen (message); i ++)
        {
            // FIXME: code a special case for Windows
            printf ("%c", message [i]);
            fflush (stdout);
            rest (60);
        }
    }
    */
    printf ("\n");
#else
    ConsolePrintf (" %s (c) %s\n", MEKA_NAME_VERSION, MEKA_AUTHORS_SHORT);
    ConsolePrintf (" Built on %s at %s\n", MEKA_BUILD_DATE, MEKA_BUILD_TIME);
    ConsolePrintf (" " MEKA_HOMEPAGE "\n");
#endif

    // Print registered info line
    // if (registered.is)
    //     ConsolePrintf ("This program is registered to:\n%s\n", registered.user_name);
}

//-----------------------------------------------------------------------------
// Quit ()
// Quit the application immediately
//-----------------------------------------------------------------------------
void    Quit (void)
{
    // Set text mode if we're not already in
    if (Meka_State != MEKA_STATE_INIT && Meka_State != MEKA_STATE_SHUTDOWN)
    {
		al_destroy_display(g_display);
		g_display = NULL;
        //set_gfx_mode (GFX_TEXT, 80, 25, 80, 25);
        // Meka_State = MEKA_STATE_SHUTDOWN;
        // Video_Setup_State ();
    }

    // Return back to starting directory
    chdir (g_Env.Paths.StartingDirectory);

#ifdef ARCH_WIN32
    if (Meka_State == MEKA_STATE_INIT)
    {
        ConsoleEnablePause();
        ConsoleWaitForAnswer(FALSE);
    }
#endif

    // Exit application
    exit (1);
}

//-----------------------------------------------------------------------------
// Quit_Msg ()
// Display an error message then quit the application
//-----------------------------------------------------------------------------
void            Quit_Msg (const char *format, ...)
{
    va_list       params;

    // Set text mode if we're not already in
    if (Meka_State != MEKA_STATE_INIT && Meka_State != MEKA_STATE_SHUTDOWN)
    {
		al_destroy_display(g_display);
		g_display = NULL;
        //set_gfx_mode (GFX_TEXT, 80, 25, 80, 25);
        // Meka_State = MEKA_STATE_SHUTDOWN;
        // Video_Setup_State ();
    }

    // Return back to starting directory
    chdir (g_Env.Paths.StartingDirectory);

#ifdef ARCH_WIN32
    {
        // FIXME: should redirect on console
		char buffer[512];
        va_start (params, format);
        vsprintf (buffer, format, params);
        va_end   (params);

        ConsolePrint(buffer);
        ConsoleEnablePause();

        if (Meka_State == MEKA_STATE_INIT)
            ConsoleWaitForAnswer(FALSE);
        else
        {
            // Note: we don't use allegro_message() because Allegro might be unitialized here
            MessageBox (NULL, buffer, Msg_Get (MSG_Window_Title), MB_OK | MB_ICONINFORMATION);
        }
    }
#else
    {
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
