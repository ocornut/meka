//-----------------------------------------------------------------------------
// MEKA - message.c
// Messaging System, Languages, Console - Code
//-----------------------------------------------------------------------------

#define __MESSAGE_C__ // Needed to include table in Message.h
#include "shared.h"
#include "tools/libparse.h"
#include "tools/tfile.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void		Lang_Set (t_menu_event *event);

//-----------------------------------------------------------------------------

// Win32 Console
#ifdef WIN32

typedef struct
{
    HINSTANCE   hinstance;
    HWND        hwnd_parent;
    HWND        hwnd;
    HWND        hwnd_edit;
    HANDLE      thread;
    DWORD       thread_id;
    HANDLE      semaphore_init;
    HANDLE      semaphore_wait;
    bool        waiting_for_answer;
    bool        quit;
} t_console_win32;

static int          ConsoleWin32_Initialize(t_console_win32 *c, HINSTANCE hInstance, HWND hWndParent);
static void         ConsoleWin32_Close(t_console_win32 *c);
static void         ConsoleWin32_Show(t_console_win32 *c);
static void         ConsoleWin32_Hide(t_console_win32 *c);
static void         ConsoleWin32_Clear(t_console_win32 *c);
static void         ConsoleWin32_CopyToClipboard(t_console_win32 *c);
static void         ConsoleWin32_Print(t_console_win32 *c, char *s);
static bool         ConsoleWin32_WaitForAnswer(t_console_win32 *c, bool allow_run);
static int CALLBACK ConsoleWin32_DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI ConsoleWin32_Thread(LPVOID data);

// Allegro stuff
extern HINSTANCE    allegro_inst;
AL_FUNC(HWND, win_get_window, (void));

#endif

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

static bool             ConsolePause;

#ifdef WIN32
static t_console_win32  ConsoleWin32;
#endif

//-----------------------------------------------------------------------------
// LANGUAGES
//-----------------------------------------------------------------------------

t_lang *        Lang_New (char *name)
{
    int         i;
    t_lang *    lang;
    t_list *    langs;

    for (langs = Messages.Langs; langs; langs = langs->next)
        if (stricmp (name, ((t_lang *)langs->elem)->Name) == 0)
            return (langs->elem);
    lang = malloc(sizeof (t_lang));
    lang->Name = strdup (name);
    for (i = 0; i < MSG_MAX; i++)
        lang->Messages[i] = NULL;
    lang->WIP = FALSE;
    list_add_to_end (&Messages.Langs, lang);
    return (lang);
}

int             Lang_Post_Check (t_lang *lang)
{
    int         i, j;
    int         cnt;

    // Count available messages (skipping MSG_NULL)
    // and set default for when one is missing
    cnt = 0;
    for (i = 1; i < MSG_MAX; i++)
        if (lang->Messages[i])
            cnt++;
    if (cnt < MSG_MAX - 1)
    {
        // We need to display the first line even in WIP mode, if this is the
        // default language, else MEKA will screw up later, with missing strings..
        if (lang->WIP == FALSE || lang == Messages.Lang_Default)
        {
            ConsolePrintf ("Language \"%s\" is incomplete (%d/%d messages found) !\n",
                lang->Name, cnt, MSG_MAX - 1);
            ConsoleEnablePause ();
        }
        if (lang->WIP == FALSE)
        {
            ConsolePrintf ("The following messages are missing:\n");
            for (i = 1; i < MSG_MAX; i++)
                if (lang->Messages[i] == NULL)
                {
                    for (j = 0; Msg_Translation_Table[j].name; j++)
                        if (Msg_Translation_Table[j].value == i)
                            ConsolePrintf ("  %s\n", Msg_Translation_Table[j].name);
                    if (lang != Messages.Lang_Default)
                        lang->Messages[i] = Messages.Lang_Default->Messages[i];
                }
            ConsoleEnablePause ();
        }
        else
        {
            if (lang != Messages.Lang_Default)
                for (i = 1; i < MSG_MAX; i++)
                    if (lang->Messages[i] == NULL)
                        lang->Messages[i] = Messages.Lang_Default->Messages[i];
        }
        return (MEKA_ERR_INCOMPLETE);
    }
    return (MEKA_ERR_OK);
}

int             Lang_Message_Add (t_lang *lang, char *msg_id, char *msg)
{
    int           i, n;

    // Find message number (#define) by name
    n = -1;
    for (i = 0; Msg_Translation_Table[i].name; i++)
        if (stricmp (msg_id, Msg_Translation_Table[i].name) == 0)
        {
            n = Msg_Translation_Table[i].value;
            break;
        }
    if (n == -1)
        return (MEKA_ERR_UNKNOWN);

    // Store message
    if (lang->Messages [n])
    {
        free (lang->Messages [n]);
        ConsolePrintf ("In %s: message \"%s\" redefined! Keeping new value.\n",
            lang->Name, msg_id);
    }

    // Replace_Backslash_N (msg);
    // lang->Messages [n] = strdup (msg);
    lang->Messages[n] = parse_getword(NULL, 0, &msg, "\"", 0, 0);

    // Verify that there's nothing after this line
    parse_skip_spaces(&msg);
    if (msg[0])
        return (MEKA_ERR_SYNTAX);

    return (MEKA_ERR_OK);
}

static void		Lang_Set (t_menu_event *event)
{
	Messages.Lang_Cur = (t_lang *)event->user_data;
    gui_menu_un_check (menus_ID.languages);
	gui_menu_check (menus_ID.languages, event->menu_item_idx);
    Msg (MSGT_USER, Msg_Get (MSG_Language_Set), Messages.Lang_Cur->Name);
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Language_Set_Warning));

    // Post-process
    // FIXME: Rebuild menus
    gamebox_rename_all();
    gui_relayout();
}

void            Lang_Set_by_Name (char *name)
{
    t_list *      langs;
    t_lang *      lang;

    for (langs = Messages.Langs; langs; langs = langs->next)
    {
        lang = langs->elem;
        if (stricmp (lang->Name, name) == 0)
        {
            Messages.Lang_Cur = lang;
            return;
        }
    }
}

void            Langs_Menu_Add (int menu_id)
{
    int         s;
    t_list *    langs;
    t_lang *    lang;

    s = list_size(Messages.Langs);
    if (s > 1)
    {
        menus_ID.languages = menu_add_menu (menu_id, Msg_Get(MSG_Menu_Main_Language), AM_Active);
        for (langs = Messages.Langs; langs; langs = langs->next)
        {
            lang = langs->elem;
            menu_add_item (menus_ID.languages, lang->Name, AM_Active | Is_Checked (lang == Messages.Lang_Cur), Lang_Set, lang);
        }
    }
}

//-----------------------------------------------------------------------------
// MESSAGING SYSTEM
//-----------------------------------------------------------------------------

int             Messages_Init_Parse_Line (char *line)
{
    char *      p;
    //char *      p2;
    int         ret;

    if (line[0] == '[')
    {
        line = strdup(line);    // Work on a copy
        if ((p = strchr(line, ']')) != NULL)
            *p = EOSTR;
        Messages.Lang_Cur = Lang_New(line + 1);
        if (Messages.Lang_Default == NULL)
            Messages.Lang_Default = Messages.Lang_Cur;
        free(line);
        return (MEKA_ERR_OK);
    }

    if (Messages.Lang_Cur == NULL)
        return (MEKA_ERR_MISSING);

    if (stricmp(line, MSG_LANG_WIP_STR) == 0)
    {
        Messages.Lang_Cur->WIP = TRUE;
        return (MEKA_ERR_OK);
    }

    line = strdup(line);    // Work on a copy
    if ((p = strchr(line, '=')) == NULL)
    {
        free(line);
        return (MEKA_ERR_SYNTAX);
    }
    *p = EOSTR;
    Trim (line);
    strupr (line);
    Trim (p + 1);
    if ((p = strchr (p + 1, '\"')) == NULL)
    {
        free(line);
        return (MEKA_ERR_SYNTAX);
    }
    // if ((p2 = strrchr (p + 1, '\"')) == NULL)
    //    return (MEKA_ERR_SYNTAX);
    // *p2 = EOSTR;
    ret = Lang_Message_Add(Messages.Lang_Cur, line, p + 1);
    free(line);
    return (ret);
}

// Load messages from MEKA.MSG file (path given in structure)
// Return a MEKA_ERR_xxx code
int             Messages_Init (void)
{
    t_tfile *   tf;
    t_list *    lines;
    int         line_cnt;
    char *      p;

    Messages.Lang_Cur = Messages.Lang_Default = NULL;
    Messages.Langs = NULL;

    // Note: this is one of the few cases were the string has to be hardcoded.
    // That is of course because the messages/localization system is not
    // initialized as of yet..
    ConsolePrint ("Loading MEKA.MSG (messages).. ");

    // Open and read file --------------------------------------------------------
    tf = tfile_read (Messages.FileName);
    if (tf == NULL)
        Quit_Msg ("MISSING!\nTry re-installing your version of Meka.");
    ConsolePrint ("\n");

    // Parse each line -----------------------------------------------------------
    line_cnt = 0;
    for (lines = tf->data_lines; lines; lines = lines->next)
    {
        char *line = lines->elem;
        line_cnt += 1;

        // Cut Comments
        p = strchr (line, ';');
        if (p != NULL)
            *p = EOSTR;

        Trim (line);
        if (StrNull (line))
            continue;

        // Parse Line and handle errors
        // ConsolePrintf ("%d: %s--\n", line_cnt, line);
        switch (Messages_Init_Parse_Line(line))
        {
        case MEKA_ERR_MISSING:    
            ConsolePrintf ("On line %d: No language defined for storing message !", line_cnt);
            tfile_free (tf);
            Quit ();
            break;
        case MEKA_ERR_UNKNOWN:    
            ConsolePrintf ("On line %d: Unknown message \"%s\", skipping it.\n", line_cnt, line);
            // tfile_free (tf);
            // Quit ();
            break;
        case MEKA_ERR_SYNTAX:     
            ConsolePrintf ("On line %d: Syntax error.\n%s\n", line_cnt, line);
            tfile_free (tf);
            Quit ();
            break;
        }
    }

    // Free file data
    tfile_free (tf);

    // Verify language completion
    {
        t_list        *langs;
        t_lang        *lang;

        if (Messages.Lang_Cur == NULL)
            Quit_Msg ("No language defined. Try re-installing your version of Meka.");
        Messages.Lang_Cur = Messages.Lang_Default;
        for (langs = Messages.Langs; langs; langs = langs->next)
        {
            lang = langs->elem;
            if (Lang_Post_Check (lang) != MEKA_ERR_OK)
                if (lang == Messages.Lang_Default)
                    Quit_Msg ("This is the default language, so we need to abort.");
        }
    }

    // Ok
    return (MEKA_ERR_OK);
}

void            Messages_Close (void)
{
}

//-----------------------------------------------------------------------------
// ConsoleInit (void)
// Initialize text output console.
//-----------------------------------------------------------------------------
void            ConsoleInit (void)
{
    // Reset pause flag
    ConsolePause = FALSE;

    // Initialize Win32 console
    #ifdef WIN32
        ConsoleWin32_Initialize(&ConsoleWin32, allegro_inst, win_get_window());
        // ConsoleWin32_Show(&ConsoleWin32);
    #endif
}

//-----------------------------------------------------------------------------
// ConsoleClose (void)
// Close console.
//-----------------------------------------------------------------------------
void            ConsoleClose (void)
{
    // Close Win32 console
    #ifdef WIN32
        ConsoleWin32_Close(&ConsoleWin32);
    #endif
}

static char Msg_Buf [MSG_MAX_LEN];

//-----------------------------------------------------------------------------
// ConsolePrintf (const char *format, ...)
// Print formatted message to console.
//-----------------------------------------------------------------------------
void            ConsolePrintf (const char *format, ...)
{
    va_list       params;

    va_start (params, format);
    vsprintf (Msg_Buf, format, params);
    va_end   (params);

    ConsolePrint(Msg_Buf);
}

//-----------------------------------------------------------------------------
// ConsolePrint (const char *msg)
// Print message to console.
//-----------------------------------------------------------------------------
void            ConsolePrint (const char *msg)
{
    // FIXME: to do
    #ifdef WIN32
        if (ConsoleWin32.hwnd != 0)
            ConsoleWin32_Print(&ConsoleWin32, (char *)msg);
        else
        {
            //printf("[CONSOLE]%s", msg);
            printf("%s", msg);
            fflush (stdout);
        }
    #else
        printf("%s", msg);
        fflush (stdout);
        // ...
    #endif
}

//-----------------------------------------------------------------------------
// ConsoleEnablePause (void)
// Enable console pausing. The Win32 console will display until user
// has choosen "Quit" or "Run".
//-----------------------------------------------------------------------------
void            ConsoleEnablePause (void)
{
    // Set pause flag
    ConsolePause = TRUE;
}

bool            ConsoleWaitForAnswer (bool allow_run)
{
#ifndef WIN32
    return TRUE;
#else
    if (!ConsolePause)
        return TRUE;

    // Else... wait for signal
    return ConsoleWin32_WaitForAnswer(&ConsoleWin32, allow_run);

#endif
}

//-----------------------------------------------------------------------------
// Msg ()
// Send a message to the user and/or debugging message
//-----------------------------------------------------------------------------
void            Msg (int attr, const char *format, ...)
{
    va_list     params;
    char *      src;
    char *      p;

    va_start (params, format);
    vsprintf (Msg_Buf, format, params);
    va_end   (params);

    #ifdef MSG_USER
        if (attr == MSG_USER) // Allegro constant!!
            Quit_Msg ("Fatal: avoid using MSG_USER, it is an Allegro constant!");
    #endif

    // Handle Bock-is-lazy-to-type-a-full-constant mode
    if (attr == 0)
        attr = MSGT_USER;

    // Split message by line (\n) and send it to the various places
    p = NULL;
    do
    {
        src = p ? p : Msg_Buf;
        p = strpbrk (src, "\n\r");
        if (p)
        {
            *p = EOSTR;
            p += 1;
        }

        // Set status line
        if (attr & MSGT_USER_INFOLINE)
        {
            strcpy (gui_status.message, src);
            gui_status.timeleft = 120;
        }

        // Add to user text box
        if (attr & MSGT_USER_BOX)
            TB_Message_Print (src);
    }
    while (p != NULL);
}

//-----------------------------------------------------------------------------
// WINDOWS CONSOLE
//-----------------------------------------------------------------------------

#ifdef WIN32

static int     ConsoleWin32_Initialize(t_console_win32 *c, HINSTANCE hInstance, HWND hWndParent)
{
    c->hinstance = hInstance;
    c->hwnd_parent = hWndParent;
    c->hwnd = 0;
    c->hwnd_edit = 0;
    c->waiting_for_answer = FALSE;
    c->quit = FALSE;
    c->semaphore_init = NULL;
    c->semaphore_wait = NULL;

#if 0
    return (0);
#else

    // Create initialization semaphore
    c->semaphore_init = CreateSemaphore(NULL, 0, 1, NULL);
    if (c->semaphore_init == 0)
    {
        meka_errno = MEKA_ERR_CONSOLE_WIN32_INIT;
        return (-1);
    }

    // Create thread
    c->thread = CreateThread(NULL, 0, ConsoleWin32_Thread, (void *)c, 0, &c->thread_id);
    if (c->thread == 0)
    {
        meka_errno = MEKA_ERR_CONSOLE_WIN32_INIT;
        return (-1);
    }
    SetThreadPriority(c->thread, THREAD_PRIORITY_LOWEST);

    // Wait for semaphore value to be set before continuing
    {
        DWORD dwWaitResult = WaitForSingleObject(c->semaphore_init, 3000L);
        if (dwWaitResult != WAIT_OBJECT_0)
        {
            TerminateThread(c->thread, 1);
            meka_errno = MEKA_ERR_CONSOLE_WIN32_INIT;
            return (-1);
        }
    }

    return (0);
#endif
}

static void    ConsoleWin32_Close(t_console_win32 *c)
{
    // Stop thread
    TerminateThread(c->thread, 1);

    // Destroy window
    if (c->hwnd == 0)
        return;
    DestroyWindow(c->hwnd);
    c->hwnd = 0;
    c->hwnd_edit = 0;
}

static void    ConsoleWin32_Show(t_console_win32 *c)
{
    if (c->hwnd == 0)
        return;
    ShowWindow(c->hwnd, SW_SHOWNORMAL);
}

static void    ConsoleWin32_Hide(t_console_win32 *c)
{
    if (c->hwnd == 0)
        return;
    ShowWindow(c->hwnd, SW_HIDE);
}

static bool     ConsoleWin32_WaitForAnswer(t_console_win32 *c, bool allow_run)
{
    bool        ret;

    // Create wait semaphore
    c->semaphore_wait = CreateSemaphore(NULL, 0, 1, NULL);
    if (c->semaphore_wait == 0)
        return (FALSE);

    // Send message to window handler
    PostMessage(c->hwnd, WM_USER+1, allow_run ? 1 : 0, 0);

    // Wait for semaphore value to be set before continuing
    WaitForSingleObject(c->semaphore_wait, INFINITE);

    // Close console and get result
    ret = !c->quit;
    ConsoleWin32_Close(c);
    return (ret);
}

static DWORD WINAPI ConsoleWin32_Thread(LPVOID data)
{
    MSG             msg;
    BOOL            bRet;
    t_console_win32 *c = data;

    // Create window
    c->hwnd = CreateDialog(c->hinstance, MAKEINTRESOURCE(IDD_CONSOLE), c->hwnd_parent, ConsoleWin32_DialogProc);
    if (c->hwnd == 0)
    {
        meka_errno = MEKA_ERR_CONSOLE_WIN32_INIT;
        return (DWORD)-1;
    }
    c->hwnd_edit = GetDlgItem(c->hwnd, IDC_CONSOLE_TEXT);

    // Show window
    ConsoleWin32_Show(c);

    // Release initialization semaphore
    ReleaseSemaphore(c->semaphore_init, 1, NULL);

    // Main Win32 message Loop
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            printf ("Message pump error!");
            break;
        }
        else
        {
            if (!IsDialogMessage(c->hwnd, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    // Release waiting semaphore
    ReleaseSemaphore(c->semaphore_wait, 1, NULL);

    return (DWORD)0;
}

static int CALLBACK ConsoleWin32_DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    t_console_win32 *c = &ConsoleWin32;

    switch (message)
    {
    case WM_INITDIALOG:
        {
            // dialogInit(hDlg, message, wParam, lParam);
            //sprintf(GenericBuffer, "MEKA %s\nStartup in progress...", VERSION);
            //SetWindowText(GetDlgItem(hDlg, IDC_CONSOLE_INFO), GenericBuffer);
            return 0;
        }
    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            //case IDC_CONSOLE_CLEAR:
            //    ConsoleWin32_Clear(&ConsoleWin32);
            //    break;
            case IDC_CONSOLE_COPY:
                {
                    ConsoleWin32_CopyToClipboard(c);
                    break;
                }
            case IDCANCEL:
            case IDC_CONSOLE_QUIT:
                {
                    c->quit = TRUE;
                    PostQuitMessage(0);
                    break;
                }
            case IDC_CONSOLE_RUN:
                {
                    c->quit = FALSE;
                    PostQuitMessage(0);
                    break;
                }
            }

            // ...
            return 0;
        }
    case WM_CLOSE:
        {
            if (c->waiting_for_answer)
            {
                c->quit = TRUE;
                PostQuitMessage(0);
            }
            return 0;
        }
    case WM_KEYDOWN:
        {
            if (c->waiting_for_answer && wParam == VK_ESCAPE)
            {
                c->quit = TRUE;
                PostQuitMessage(0);
            }
            return 0;
        }
    case WM_USER+1:
        {
            c->waiting_for_answer = TRUE;
            //SetFocus(hDlg);
            if (wParam != 0)
            {
                EnableWindow(GetDlgItem(hDlg, IDC_CONSOLE_QUIT), TRUE);
                EnableWindow(GetDlgItem(hDlg, IDC_CONSOLE_RUN), TRUE);
                SetFocus(GetDlgItem(hDlg, IDC_CONSOLE_RUN));
                SetWindowText(GetDlgItem(hDlg, IDC_CONSOLE_INFO), "WARNING - Continue?");
            }
            else
            {
                EnableWindow(GetDlgItem(hDlg, IDC_CONSOLE_QUIT), TRUE);
                SetFocus(GetDlgItem(hDlg, IDC_CONSOLE_QUIT));
                SetWindowText(GetDlgItem(hDlg, IDC_CONSOLE_INFO), "ERROR - Have to quit");
            }
            return 0;
        }
    }
    return 0;
    //return DefDlgProc(hDlg, message, wParam, lParam);
}

static void    ConsoleWin32_Clear(t_console_win32 *c)
{
    SetDlgItemText(c->hwnd, IDC_CONSOLE_TEXT, "");
}

static void    ConsoleWin32_CopyToClipboard(t_console_win32 *c)
{
    if (OpenClipboard(c->hwnd))
    {
        int     text_length;
        HGLOBAL clipbuffer;
        char *  buffer;

        // First empty the clipboard
        EmptyClipboard();

        // Allocate memory in global scope and read current text into it
        text_length = GetWindowTextLength(c->hwnd_edit);
        clipbuffer = GlobalAlloc(GMEM_DDESHARE, text_length + 1);
        buffer = (char*)GlobalLock(clipbuffer);
        GetDlgItemText(c->hwnd, IDC_CONSOLE_TEXT, buffer, text_length + 1);
        GlobalUnlock(clipbuffer);

        // Set clipboard to our data
        SetClipboardData(CF_TEXT, clipbuffer);
        CloseClipboard();
    }
}

static void        ConsoleWin32_Print(t_console_win32 *c, char *s)
{
    char *  text;
    int     text_length;
    int     newlines_counter;

    if (c->hwnd == 0)
        return;

    // Count '\n' in new text
    newlines_counter = 0;
    for (text = s; *text != EOSTR; text++)
        if (*text == '\n')
            newlines_counter++;

    // Fill text buffer
    // Replace all occurences single "\n" by "\r\n" since windows edit box wants that
    text = Memory_Alloc(strlen(s) + (newlines_counter * sizeof(char)) + 2 + 1);
    {
        char *dst = text;
        while (*s != EOSTR)
        {
            if (*s == '\n')
                *dst++ = '\r';
            *dst++ = *s++;
        }
        *dst = EOSTR;
        //  sprintf(text + text_length, "%s", s);
    }

    // Set new text
    // Tips: set an empty selection at the end then replace selection, to avoid flickering (better than a WM_SETTEXT)
    text_length = GetWindowTextLength(c->hwnd_edit);
    SendMessage(c->hwnd_edit, EM_SETSEL, text_length, text_length);
    SendMessage(c->hwnd_edit, EM_REPLACESEL, FALSE, (LPARAM)text);
    free(text);
}

#endif // WIN32

//-----------------------------------------------------------------------------

