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
// Note: the Win32 code, especially initialization, is boring and messy.
// This may be improved in the future if we need more Win32 stuff.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "setup.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------
#ifdef WIN32
    static int    Setup_Interactive_Win32 (void);
    AL_FUNC(HWND, win_get_window, (void));
#else
    static int    Setup_Interactive_Console (void);
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Setup_Interactive_Init (void)
{
    if ((opt.Setup_Interactive_Execute)
        || (Sound.Enabled && Sound.SoundCard == SOUND_SOUNDCARD_SELECT))
    {
        ConsolePrintf ("%s\n", Msg_Get(MSG_Setup_Running));
        opt.Setup_Interactive_Execute = FALSE;
        Sound_Init_SEAL (); // FIXME: this is needed to enumerate sound cards
        if (Setup_Interactive () == MEKA_ERR_CANCEL)
        {
            // Note: the only reason for setting the state to SHUTDOWN is so that Quit() doesn't pause on the console.
            Meka_State = MEKA_STATE_SHUTDOWN;   
            Quit ();
        }
    }
}

int     Setup_Interactive (void)
{
    // Call appropriate setup
    #ifdef WIN32
        // Windows setup
        return (Setup_Interactive_Win32 ());
    #else
        // Console setup (DOS & UNIX)
        return (Setup_Interactive_Console ());
    #endif
}

//-----------------------------------------------------------------------------

#ifdef WIN32

extern HINSTANCE      allegro_inst;

// Win32 Dialog Procedure for interactive setup -------------------------------
static BOOL CALLBACK
Setup_Interactive_Win32_DialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG:
        {
        int i, n;
        int default_selection;
        AUDIOCAPS Audio_Caps;
        HWND combo_hwnd;
        int  combo_idx;

        // Set various localized text
        sprintf(GenericBuffer, "MEKA - %s", Msg_Get (MSG_Setup_Setup));
        SetWindowText(hDlg, GenericBuffer);
        SetWindowText(GetDlgItem(hDlg, IDC_SETUP_SOUND_TEXT_1), Msg_Get (MSG_Setup_Soundcard_Select));
        // SetWindowText(GetDlgItem(hDlg, IDC_SETUP_SOUND_TEXT_2), Msg_Get (MSG_Setup_Soundcard_Select_Tips_Win32));
        SetWindowText(GetDlgItem(hDlg, IDC_SETUP_SOUND_TEXT_3), Msg_Get (MSG_Setup_SampleRate_Select));
        SetWindowText(GetDlgItem(hDlg, IDC_SETUP_DEBUGGER_ENABLE), Msg_Get (MSG_Setup_Debugger_Enable));

        // Fill soundcards combo box
        n = AGetAudioNumDevs();
        combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_SOUNDCARDS);
        default_selection = Sound.SoundCard;
        for (i = 0; i < n; i++)
            if (AGetAudioDevCaps (i, &Audio_Caps) == AUDIO_ERROR_NONE)
               {
               combo_idx = SendMessage(combo_hwnd, CB_ADDSTRING, 0, (LPARAM)Audio_Caps.szProductName);
               SendMessage(combo_hwnd, CB_SETITEMDATA, combo_idx, (LPARAM)i);
               if (default_selection == SOUND_SOUNDCARD_SELECT)
                  if (strcmp (Audio_Caps.szProductName, "DirectSound") == 0)
                      default_selection = i;
               }
        if (default_selection != SOUND_SOUNDCARD_SELECT)
           SendMessage(combo_hwnd, CB_SETCURSEL, default_selection, 0);

        // Fill soundrates combo box
        combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_SOUNDRATE);
        default_selection = 2; // FIXME: 2= 44100 KH
        for (i = 0; Sound_Rate_Default_Table[i] != -1; i++)
           {
           sprintf (GenericBuffer, Msg_Get (MSG_Menu_Sound_Rate_Hz), Sound_Rate_Default_Table [i]);
           combo_idx = SendMessage(combo_hwnd, CB_ADDSTRING, 0, (LPARAM)GenericBuffer);
           SendMessage(combo_hwnd, CB_SETITEMDATA, combo_idx, (LPARAM)i);
           if (Sound_Rate_Default_Table[i] == Sound.SampleRate)
              default_selection = i;
           }
        if (default_selection != -1)
           SendMessage(combo_hwnd, CB_SETCURSEL, default_selection, 0);

        // Fill language combo box
        {
           t_list *langs;
           combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_LANGUAGE);
           for (langs = Messages.Langs; langs; langs = langs->next)
              {
              t_lang *lang = langs->elem;
              combo_idx = SendMessage(combo_hwnd, CB_ADDSTRING, 0, (LPARAM)lang->Name);
              SendMessage(combo_hwnd, CB_SETITEMDATA, combo_idx, (LPARAM)lang);
              // printf("lang %s %i\n", lang->Name, combo_idx);
              }
           n = SendMessage(combo_hwnd, CB_GETCOUNT, 0, 0);
           // Now set default selection
           // Note: we have to do it that way because the combo box is sorted!
           for (i = 0; i < n; i++)
              {
              t_lang *lang = (t_lang *)SendMessage(combo_hwnd, CB_GETITEMDATA, i, 0);
              if (lang == Messages.Lang_Cur)
                 {
                 SendMessage(combo_hwnd, CB_SETCURSEL, i, 0);
                 break;
                 }
              }
        }

        // Fill debugger enable box
        CheckDlgButton(hDlg, IDC_SETUP_DEBUGGER_ENABLE, (bool)Configuration.debug_mode_cfg);

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

                  // Sound Card
                  if ((n = SendMessage(GetDlgItem(hDlg, IDC_SETUP_SOUNDCARDS), CB_GETCURSEL, 0, 0)) != CB_ERR)
                     Sound.SoundCard = n;
                  // Sample Rate
                  if ((n = SendMessage(GetDlgItem(hDlg, IDC_SETUP_SOUNDRATE), CB_GETCURSEL, 0, 0)) != CB_ERR)
                     Sound.SampleRate = Sound_Rate_Default_Table[n];
                  // Language
                  combo_hwnd = GetDlgItem(hDlg, IDC_SETUP_LANGUAGE);
                  if ((n = SendMessage(combo_hwnd, CB_GETCURSEL, 0, 0)) != CB_ERR)
                     {
                     t_lang *lang = (t_lang *)SendMessage(combo_hwnd, CB_GETITEMDATA, n, 0);
                     Messages.Lang_Cur = lang; // FIXME
                     }
                  // Debugger enable
                  Configuration.debug_mode_cfg = (bool)IsDlgButtonChecked(hDlg, IDC_SETUP_DEBUGGER_ENABLE);
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
static  int     Setup_Interactive_Win32 (void)
{
    int         ret;

    ret = DialogBox(allegro_inst, MAKEINTRESOURCE(IDD_SETUP), win_get_window(), Setup_Interactive_Win32_DialogProc);
    if (ret == -1)
        return (MEKA_ERR_FAIL);
    if (ret == 2)
        return (MEKA_ERR_CANCEL);
    return (MEKA_ERR_OK);
}

#else

// Interactive Setup (Console version) ----------------------------------------
// Let user choose his sound card driver
static  int     Setup_Interactive_Console (void)
{
    int         i, n;
    AUDIOCAPS   Audio_Caps;

    // Print setup message
    ConsolePrintf ("[%s]\n", Msg_Get (MSG_Setup_Setup));

    // Print soundcard selection message
    ConsolePrintf ("%s\n", Msg_Get (MSG_Setup_Soundcard_Select));
    #ifdef WIN32
        ConsolePrintf("%s\n", Msg_Get (MSG_Setup_Soundcard_Select_Tips_Win32));
    #elif DOS
        ConsolePrintf("%s\n", Msg_Get (MSG_Setup_Soundcard_Select_Tips_DOS));
    #endif

    // Print soundcard listing
    n = AGetAudioNumDevs();
    for (i = 0; i < n; i++)
    {
        if (AGetAudioDevCaps (i, &Audio_Caps) == AUDIO_ERROR_NONE)
            ConsolePrintf ("  %2d. %s\n", i, Audio_Caps.szProductName);
    }

    // Let user select (using Allegro keyboard handler)
    Sound.SoundCard = SOUND_SOUNDCARD_NONE;
#ifndef UNIX
    while (!key[KEY_ESC])
        for (i = 0; i < n; i ++)
            if (key[KEY_0 + i])
            {
                Sound.SoundCard = i;
                return (MEKA_ERR_OK);
            }
#else
    while (1)
    {
        int k = getc(stdin);
        if (k >= '0' && k <= '9' && (k - '0') < n)
        {
            Sound.SoundCard = (k - '0');
            return (MEKA_ERR_OK);
        }
    }
#endif

    return (MEKA_ERR_CANCEL);
}

#endif

//-----------------------------------------------------------------------------
