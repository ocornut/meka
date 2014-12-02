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

void *	Memory_Alloc(size_t size)
{
	u8* p = (u8*)malloc(size);
	if (p == NULL)
	{
		meka_errno = MEKA_ERR_MEMORY;
		Quit_Msg("%s", meka_strerror());
	}
	return (p);
}

// Called when media (ROM) changes
void    Change_System_Misc()
{
    gamebox_resize_all();
    Capture_Init_Game();
    Sound_Log_Init_Game();
    MemoryViewers_MediaReload();
#ifdef MEKA_Z80_DEBUGGER
    Debugger_MediaReload();
#endif // MEKA_Z80_DEBUGGER
}

bool	OSD_ClipboardSetText(const char* text, const char* text_end)
{ 
#ifdef ARCH_WIN32
	if (!OpenClipboard(NULL))
		return false;

	if (!text_end)
		text_end = text + strlen(text);

	const int buf_length = (text_end - text) + 1;
	HGLOBAL buf_handle = GlobalAlloc(GMEM_MOVEABLE, buf_length * sizeof(char)); 
	if (buf_handle == NULL)
		return false;

	char* buf_global = (char *)GlobalLock(buf_handle); 
	sprintf(buf_global, "%.*s", text_end - text, text);
	GlobalUnlock(buf_handle); 

	EmptyClipboard();
	SetClipboardData(CF_TEXT, buf_handle);
	CloseClipboard();

	return true;
#else
	return false;
#endif
}

char*	OSD_ClipboardGetText()
{
#ifdef ARCH_WIN32
	if (!OpenClipboard(NULL)) 
		return NULL;

	HANDLE buf_handle = GetClipboardData(CF_TEXT); 
	if (buf_handle == NULL)
		return NULL;

	char* buf_global = (char*)GlobalLock(buf_handle); 
	char* buf_local = buf_global ? strdup(buf_global) : NULL;
	GlobalUnlock(buf_handle); 
	CloseClipboard(); 

	return buf_local;
#else
	return NULL;
#endif
}

void    Random_Init (void)
{
	srand ((unsigned int)time (NULL));
#ifndef ARCH_WIN32
	srandom (time (NULL));
#endif
}

int		RandomInt(int max)
{
#ifndef ARCH_WIN32
	return random() % max;
#else
	return rand() % max;
#endif
}

// FIXME: shit
float	RandomFloat(float max)
{
	return max * ((float)RandomInt(65535) / 65535.0f);
}

// FIXME: shit
float	RandomFloat(float min, float max)
{
	return min + (max - min) * ((float)RandomInt(65535) / 65535.0f);
}

void	Profile_Step(const char* name)
{
	static double last_time = 0.0;
	double current_time = al_get_time();

	float delta_ms = (float)(current_time-last_time) * 1000.0f;

	Msg(MSGT_DEBUG, "PROF %6.2f - %s", delta_ms, name);

	last_time = current_time;
}

#ifdef ARCH_UNIX
#define ESC         "\x1b"
#define FGCOLOR(n)  ESC"[3"#n"m"
#define BGCOLOR(n)  ESC"[4"#n"m"
#define RESET       ESC"[0m"
#endif

// Show the ending ASCII message
void    Show_End_Message()
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
        // Video_Setup_State();
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
void	Quit_Msg(const char *format, ...)
{
    // Set text mode if we're not already in
    if (g_env.state != MEKA_STATE_INIT && g_env.state != MEKA_STATE_SHUTDOWN)
    {
		al_destroy_display(g_display);
		g_display = NULL;
        //set_gfx_mode (GFX_TEXT, 80, 25, 80, 25);
        // g_env.state = MEKA_STATE_SHUTDOWN;
        // Video_Setup_State();
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
            MessageBox (NULL, buffer, Msg_Get(MSG_Window_Title), MB_OK | MB_ICONINFORMATION);
        }
    }
#else
    {
		va_list params;
        va_start (params, format);
        vprintf  (format, params); // FIXME: use Console*
        va_end   (params);
        ConsolePrint("\n");
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
