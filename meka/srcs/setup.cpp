//-----------------------------------------------------------------------------
// MEKA - setup.c
// Interactive Setup - Code
//-----------------------------------------------------------------------------
// Note: the interactive setup is different depending on the system.
// Different version feature different parameters to setup.
//-----------------------------------------------------------------------------
// - DOS, UNIX: Text mode. Soundcard setup.
// - WIN32:     Win32 applet. Soundcard, soundrate, language, debugger enable setup.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "config.h"
#include "setup.h"
#include "video.h"
#include "sound/s_misc.h"
#ifdef ARCH_WIN32
#include "projects/msvc/resource.h"
#endif

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------
#ifdef ARCH_WIN32
    static int    Setup_Interactive_Win32();
#else
    static int    Setup_Interactive_Console();
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void Setup_Interactive_Init()
{
    if (opt.Setup_Interactive_Execute)
    {
        ConsolePrintf ("%s\n", Msg_Get(MSG_Setup_Running));
        opt.Setup_Interactive_Execute = FALSE;
        const int ret = Setup_Interactive();
		if (ret == MEKA_ERR_CANCEL)
        {
            // Note: the only reason for setting the state to SHUTDOWN is so that Quit() doesn't pause on the console.
            g_env.state = MEKA_STATE_SHUTDOWN;   
            Quit();
        }
    }
}

int Setup_Interactive()
{
    // Call appropriate setup
    #ifdef ARCH_WIN32
        // Windows setup
        return Setup_Interactive_Win32();
    #else
        // Console setup (DOS & UNIX)
        //return Setup_Interactive_Console();
        return MEKA_ERR_OK;
    #endif
}

//-----------------------------------------------------------------------------

#ifdef ARCH_WIN32

//extern HINSTANCE      allegro_inst;

// Win32 Dialog Procedure for interactive setup -------------------------------
static BOOL CALLBACK	Setup_Interactive_Win32_DialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			// Set various localized text
			char buffer[256];
			snprintf(buffer, countof(buffer), "MEKA - %s", Msg_Get(MSG_Setup_Setup));
			SetWindowText(hDlg, buffer);
			SetWindowText(GetDlgItem(hDlg, IDC_SETUP_VIDEO_DRIVER_TEXT), Msg_Get(MSG_Setup_Video_Driver));
			SetWindowText(GetDlgItem(hDlg, IDC_SETUP_VIDEO_RESOLUTION_TEXT), Msg_Get(MSG_Setup_Video_DisplayMode));
			SetWindowText(GetDlgItem(hDlg, IDC_SETUP_SOUND_SAMPLERATE_TEXT), Msg_Get(MSG_Setup_SampleRate_Select));

			HWND combo_hwnd;
			int default_selection;

			// Fill video driver combo box
			combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_VIDEO_DRIVER);
			default_selection = g_configuration.video_driver - &g_video_drivers[0];
			t_video_driver* driver = &g_video_drivers[0];
			while (driver->name)
			{
				int driver_idx = driver - &g_video_drivers[0];
				const int combo_idx = SendMessage(combo_hwnd, CB_ADDSTRING, 0, (LPARAM)driver->desc);
				SendMessage(combo_hwnd, CB_SETITEMDATA, combo_idx, (LPARAM)driver_idx);
				driver++;
			}
			if (default_selection != -1)
				SendMessage(combo_hwnd, CB_SETCURSEL, default_selection, 0);

			// Fill screen format/resolution box
			{
				Video_EnumerateDisplayModes();
				combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_VIDEO_DISPLAY_MODE);
				std::vector<t_video_mode>& display_modes = g_video.display_modes;
				for (size_t i = 0; i != display_modes.size(); i++)
				{
					t_video_mode* display_mode = &display_modes[i];
					char buf[256];
					sprintf(buf, "%dx%d, %d Hz", display_mode->w, display_mode->h, /*al_get_pixel_format_bits(display_mode->color_format),*/ display_mode->refresh_rate);

					const int combo_idx = SendMessage(combo_hwnd, CB_ADDSTRING, 0, (LPARAM)buf);
					SendMessage(combo_hwnd, CB_SETITEMDATA, combo_idx, (LPARAM)display_mode);
				}
				default_selection = g_video.display_mode_current_index;
				if (default_selection != -1)
					SendMessage(combo_hwnd, CB_SETCURSEL, default_selection, 0);
			}

			// Fill sound rate combo box
			combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_SOUND_SAMPLERATE);
			default_selection = 1; // FIXME: 1= 44100 KH
			for (int i = 0; Sound_Rate_Default_Table[i] != -1; i++)
			{
				char buffer[256];
				snprintf(buffer, countof(buffer), Msg_Get(MSG_Menu_Sound_Rate_Hz), Sound_Rate_Default_Table[i]);
				const int combo_idx = SendMessage(combo_hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
				SendMessage(combo_hwnd, CB_SETITEMDATA, combo_idx, (LPARAM)i);
				if (Sound_Rate_Default_Table[i] == Sound.SampleRate)
					default_selection = i;
			}
			if (default_selection != -1)
				SendMessage(combo_hwnd, CB_SETCURSEL, default_selection, 0);

			// Fill language combo box
			{
				combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_LANGUAGE);
				for (t_list* langs = Messages.Langs; langs; langs = langs->next)
				{
					t_lang* lang = (t_lang*)langs->elem;
					const int combo_idx = SendMessage(combo_hwnd, CB_ADDSTRING, 0, (LPARAM)lang->Name);
					SendMessage(combo_hwnd, CB_SETITEMDATA, combo_idx, (LPARAM)lang);
					// printf("lang %s %i\n", lang->Name, combo_idx);
				}
				int n = SendMessage(combo_hwnd, CB_GETCOUNT, 0, 0);
				// Now set default selection
				// Note: we have to do it that way because the combo box is sorted!
				for (int i = 0; i < n; i++)
				{
					t_lang *lang = (t_lang *)SendMessage(combo_hwnd, CB_GETITEMDATA, i, 0);
					if (lang == Messages.Lang_Cur)
					{
						SendMessage(combo_hwnd, CB_SETCURSEL, i, 0);
						break;
					}
				}
			}

			// Move to foreground, seems to be needed
			SetForegroundWindow(hDlg);

			return FALSE;
		}
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
				{
					if (HIWORD(wParam) == BN_CLICKED)
					{
						int  n;
						HWND combo_hwnd;

						// Video Driver
						if ((n = SendMessage(GetDlgItem(hDlg, IDC_SETUP_VIDEO_DRIVER), CB_GETCURSEL, 0, 0)) != CB_ERR)
							g_configuration.video_driver = &g_video_drivers[n];

						// Video Display Mode
						combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_VIDEO_DISPLAY_MODE);
						if ((n = SendMessage(combo_hwnd, CB_GETCURSEL, 0, 0)) != CB_ERR)
						{
							t_video_mode* display_mode = (t_video_mode*)SendMessage(combo_hwnd, CB_GETITEMDATA, n, 0);
							g_configuration.video_mode_gui_res_x = display_mode->w;
							g_configuration.video_mode_gui_res_y = display_mode->h;
							g_configuration.video_mode_gui_refresh_rate = display_mode->refresh_rate;
						}

						// Sample Rate
						if ((n = SendMessage(GetDlgItem(hDlg, IDC_SETUP_SOUND_SAMPLERATE), CB_GETCURSEL, 0, 0)) != CB_ERR)
							Sound.SampleRate = Sound_Rate_Default_Table[n];

						// Language
						combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_LANGUAGE);
						if ((n = SendMessage(combo_hwnd, CB_GETCURSEL, 0, 0)) != CB_ERR)
						{
							t_lang *lang = (t_lang *)SendMessage(combo_hwnd, CB_GETITEMDATA, n, 0);
							Messages.Lang_Cur = lang; // FIXME
						}

						// Debugger enable
						EndDialog(hDlg, 0);
					}
					return TRUE;
				}
			case IDCLOSE:
				{
					if (HIWORD(wParam) == BN_CLICKED)
						EndDialog(hDlg, 2);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

// Interactive Setup (Win32 version) ------------------------------------------
// Let user choose his sound card driver, sound rate and language
static int Setup_Interactive_Win32()
{
	const HINSTANCE hInstance = GetModuleHandle(NULL);
	const HWND hWndParent = 0; // win_get_window()
	const int ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_SETUP), hWndParent, Setup_Interactive_Win32_DialogProc);
    if (ret == -1)
        return (MEKA_ERR_FAIL);
    if (ret == 2)
        return (MEKA_ERR_CANCEL);
    return (MEKA_ERR_OK);
}

#else

// Interactive Setup (Console version) ----------------------------------------
// Let user choose his sound card driver
static int Setup_Interactive_Console()
{
    // Print setup message
    ConsolePrintf ("[%s]\n", Msg_Get(MSG_Setup_Setup));

    // Print sound card selection message
//    ConsolePrintf ("%s\n", Msg_Get(MSG_Setup_Soundcard_Select));
//#ifdef ARCH_WIN32
//	ConsolePrintf("%s\n", Msg_Get(MSG_Setup_Soundcard_Select_Tips_Win32));
//#endif
	
	return MEKA_ERR_OK;
}

#endif

//-----------------------------------------------------------------------------
