//-----------------------------------------------------------------------------
// MEKA - misc.c
// Miscellaneous - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_memview.h"
#include "capture.h"
#include "debugger.h"
#include "inputs_i.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Close_Button_Callback ()
// Close button hook, called by Allegro
//-----------------------------------------------------------------------------
void    Close_Button_Callback (void)
{
    if (Meka_State == MEKA_STATE_INIT || Meka_State == MEKA_STATE_SHUTDOWN)
        return;
    opt.Force_Quit = YES;
}

//-----------------------------------------------------------------------------
// Switch_In_Callback ()
// Application switch-in hook, called by Allegro
// This is fired when the application screen reappear (eg: after an ALT-TAB)
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Change_System_Misc ()
// Called when media (ROM) changes - updates various things
//-----------------------------------------------------------------------------
void    Change_System_Misc (void)
{
    gamebox_resize_all ();
    Capture_Init_Game ();
    Sound_Log_Init_Game ();
    MemoryViewer_LoadROM ();
    Debugger_MediaReload();
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
            RGB color_black = {  0,  0,  0, 0 };
            RGB color_white = { 63, 63, 63, 0 };
            Palette_SetColor (GUI_COL_BLACK, color_black); // These colors are
            Palette_SetColor (GUI_COL_WHITE, color_white); // used for cursors.
            Video_Clear ();
            if (!(machine & MACHINE_POWER_ON))
                Effects_TV_Init_Colors ();
            break;
        }
    case MEKA_STATE_GUI:
        // .. nothing to do ..
        break;
    }
    Show_Mouse_In (NULL);
    #ifdef DOS
        Video_VGA_Set_Border_Color (Border_Color);
    #endif
    Inputs_Peripheral_Change_Update ();
}

//-----------------------------------------------------------------------------
// Set_Mouse_Cursor (int n)
// Set mouse cursor to given one
//-----------------------------------------------------------------------------
// FIXME: Add proper enum to cursor values instead of using obscure hardcoded integers.
//-----------------------------------------------------------------------------
void    Set_Mouse_Cursor (int n)
{
    if (cfg.Mouse_Installed == -1)
        return;
    switch (n)
    {
    case 0: set_mouse_sprite (NULL);
        break;
    case 1: set_mouse_sprite (Graphics.Cursors.Main);
        set_mouse_sprite_focus (0, 0);
        break;
    case 2: set_mouse_sprite (Graphics.Cursors.LightPhaser);
        set_mouse_sprite_focus (7, 7);
        break;
    case 3: set_mouse_sprite (Graphics.Cursors.SportsPad);
        set_mouse_sprite_focus (7, 7);
        break;
    case 4: set_mouse_sprite (Graphics.Cursors.TvOekaki);
        set_mouse_sprite_focus (3, 12);
        break;
    case 5: set_mouse_sprite (Graphics.Cursors.Wait);
        set_mouse_sprite_focus (6, 2);
        break;
    }
}

#ifdef UNIX
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

#ifdef UNIX
  printf(" ");
  printf(BGCOLOR(1));
  printf(FGCOLOR(0));
  printf ("                           ");  
  printf (RESET "  %s (c) %s\n", PROG_NAME_VER, PROG_AUTHORS_SHORT);
  
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
  printf (RESET "  " PROG_HOMEPAGE "\n");

  printf (" ");
  printf ("===========================");
  
  printf (RESET);
  printf ("\n");
#elif DOS
    // Line 1 ------------------------------------------------------------------
    textbackground (4);     // Red background
    textcolor (0);          // Black text
    printf (" ");
    cprintf ("еееееееееееееееееееееееееее");
    printf ("  %s (c) %s\n", PROG_NAME_VER, PROG_AUTHORS_SHORT);
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
    printf ("  " PROG_HOMEPAGE "\n");
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
    ConsolePrintf (" %s (c) %s\n", PROG_NAME_VER, PROG_AUTHORS_SHORT);
    ConsolePrintf (" Built on %s at %s\n", MEKA_BUILD_DATE, MEKA_BUILD_TIME);
    ConsolePrintf (" " PROG_HOMEPAGE "\n");
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
        set_gfx_mode (GFX_TEXT, 80, 25, 80, 25);
        // Meka_State = MEKA_STATE_SHUTDOWN;
        // Video_Setup_State ();
    }

    // Return back to starting directory
    chdir (Env.Paths.StartingDirectory);

#ifdef WIN32
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
        set_gfx_mode (GFX_TEXT, 80, 25, 80, 25);
        // Meka_State = MEKA_STATE_SHUTDOWN;
        // Video_Setup_State ();
    }

    // Return back to starting directory
    chdir (Env.Paths.StartingDirectory);

#ifdef WIN32
    {
        // FIXME: should redirect on console
        va_start (params, format);
        vsprintf (GenericBuffer, format, params);
        va_end   (params);

        ConsolePrint(GenericBuffer);
        ConsoleEnablePause();

        if (Meka_State == MEKA_STATE_INIT)
            ConsoleWaitForAnswer(FALSE);
        else
        {
            // Note: we don't use allegro_message() because Allegro might be unitialized here
            MessageBox (NULL, GenericBuffer, Msg_Get (MSG_Window_Title), MB_OK | MB_ICONINFORMATION);
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