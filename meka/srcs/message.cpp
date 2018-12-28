//-----------------------------------------------------------------------------
// MEKA - message.c
// Messaging System, Languages, Console - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_game.h"
#include "libparse.h"
#include "newgui.h"
#ifdef ARCH_WIN32
#include "projects/msvc/resource.h"
#endif

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void     Lang_Set(t_menu_event *event);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_messages Messages;

struct S2I_TYPE
{
    const char *    name;
    int             value;
};

#define __MSG_ADD(ID)   { #ID, ID }
static const S2I_TYPE Msg_Translation_Table [] =
{
    __MSG_ADD(MSG_Welcome),
    __MSG_ADD(MSG_Window_Title),
    __MSG_ADD(MSG_Quit),

    __MSG_ADD(MSG_About_BoxTitle),
    __MSG_ADD(MSG_About_Line_Meka_Date),
    __MSG_ADD(MSG_About_Line_Authors),
    __MSG_ADD(MSG_About_Line_Homepage),

    __MSG_ADD(MSG_Ok),
    __MSG_ADD(MSG_Failed),
    __MSG_ADD(MSG_Error_Base),
    __MSG_ADD(MSG_Error_Error),
    __MSG_ADD(MSG_Error_Memory),
    __MSG_ADD(MSG_Error_Param),
    __MSG_ADD(MSG_Error_Syntax),

    __MSG_ADD(MSG_Error_Video_Mode),
    __MSG_ADD(MSG_Error_Video_Mode_Back_To_GUI),

    __MSG_ADD(MSG_Error_File_Not_Found),
    __MSG_ADD(MSG_Error_File_Read),
    __MSG_ADD(MSG_Error_File_Empty),

    __MSG_ADD(MSG_Error_ZIP_Not_Supported),
    __MSG_ADD(MSG_Error_ZIP_Loading),
    __MSG_ADD(MSG_Error_ZIP_Internal),

    __MSG_ADD(MSG_Error_Directory_Open),

    __MSG_ADD(MSG_Must_Reset),
    __MSG_ADD(MSG_No_ROM),

    __MSG_ADD(MSG_Init_Allegro),
    __MSG_ADD(MSG_Init_GUI),
    __MSG_ADD(MSG_Init_Completed),

    __MSG_ADD(MSG_Setup_Running),
    __MSG_ADD(MSG_Setup_Setup),
    __MSG_ADD(MSG_Setup_Video_Driver),
    __MSG_ADD(MSG_Setup_Video_DisplayMode),
    __MSG_ADD(MSG_Setup_SampleRate_Select),

    __MSG_ADD(MSG_Capture_Done),
    __MSG_ADD(MSG_Capture_Error),
    __MSG_ADD(MSG_Capture_Error_File),

    __MSG_ADD(MSG_SRAM_Loaded),
    __MSG_ADD(MSG_SRAM_Load_Unable),
    __MSG_ADD(MSG_SRAM_Wrote),
    __MSG_ADD(MSG_SRAM_Write_Unable),

    __MSG_ADD(MSG_93c46_Reset),
    __MSG_ADD(MSG_93c46_Loaded),
    __MSG_ADD(MSG_93c46_Load_Unable),
    __MSG_ADD(MSG_93c46_Wrote),
    __MSG_ADD(MSG_93c46_Write_Unable),

    __MSG_ADD(MSG_TVType_Set),
    __MSG_ADD(MSG_TVType_Info_Speed),

    __MSG_ADD(MSG_Blitters_Loading),
    __MSG_ADD(MSG_Blitters_Error_Not_Enough),
    __MSG_ADD(MSG_Blitters_Error_Not_Found),
    __MSG_ADD(MSG_Blitters_Error_Missing),
    __MSG_ADD(MSG_Blitters_Error_Unrecognized),
    __MSG_ADD(MSG_Blitters_Error_Incorrect_Value),
    __MSG_ADD(MSG_Blitters_Set),

    __MSG_ADD(MSG_NES_Activate),
    __MSG_ADD(MSG_NES_Sucks),
    __MSG_ADD(MSG_NES_Mapper_Unknown),
    __MSG_ADD(MSG_NES_Deny_Facts),

    __MSG_ADD(MSG_Debug_Init),
    __MSG_ADD(MSG_Debug_Welcome),
    __MSG_ADD(MSG_Debug_Not_Available),
    __MSG_ADD(MSG_Debug_Trap_Read),
    __MSG_ADD(MSG_Debug_Trap_Write),
    __MSG_ADD(MSG_Debug_Trap_Port_Read),
    __MSG_ADD(MSG_Debug_Trap_Port_Write),
    __MSG_ADD(MSG_Debug_Symbols_Loaded),
    __MSG_ADD(MSG_Debug_Symbols_Error),
    __MSG_ADD(MSG_Debug_Symbols_Error_Line),

    __MSG_ADD(MSG_DataDump_Mode_Ascii),
    __MSG_ADD(MSG_DataDump_Mode_Raw),
    __MSG_ADD(MSG_DataDump_Error),
    __MSG_ADD(MSG_DataDump_Error_OB_Memory),
    __MSG_ADD(MSG_DataDump_Error_Palette),
    __MSG_ADD(MSG_DataDump_Error_Sprites),
    __MSG_ADD(MSG_DataDump_Main),

    __MSG_ADD(MSG_Doc_BoxTitle),
    __MSG_ADD(MSG_Doc_File_Error),
    __MSG_ADD(MSG_Doc_Enabled),
    __MSG_ADD(MSG_Doc_Disabled),

    __MSG_ADD(MSG_Flickering_Auto),
    __MSG_ADD(MSG_Flickering_Yes),
    __MSG_ADD(MSG_Flickering_No),

    __MSG_ADD(MSG_Layer_BG_Disabled),
    __MSG_ADD(MSG_Layer_BG_Enabled),
    __MSG_ADD(MSG_Layer_Spr_Disabled),
    __MSG_ADD(MSG_Layer_Spr_Enabled),

    __MSG_ADD(MSG_FDC765_Unknown_Read),
    __MSG_ADD(MSG_FDC765_Unknown_Write),
    __MSG_ADD(MSG_FDC765_Disk_Too_Large1),
    __MSG_ADD(MSG_FDC765_Disk_Too_Large2),
    __MSG_ADD(MSG_FDC765_Disk_Too_Small1),
    __MSG_ADD(MSG_FDC765_Disk_Too_Small2),

    __MSG_ADD(MSG_Palette_BoxTitle),
    __MSG_ADD(MSG_Palette_Disabled),
    __MSG_ADD(MSG_Palette_Enabled),

    __MSG_ADD(MSG_Message_BoxTitle),

    __MSG_ADD(MSG_TechInfo_BoxTitle),
    __MSG_ADD(MSG_TechInfo_Disabled),
    __MSG_ADD(MSG_TechInfo_Enabled),

    __MSG_ADD(MSG_TilesViewer_BoxTitle),
    __MSG_ADD(MSG_TilesViewer_Disabled),
    __MSG_ADD(MSG_TilesViewer_Enabled),
    __MSG_ADD(MSG_TilesViewer_Tile),

    __MSG_ADD(MSG_MemoryEditor_BoxTitle),
    __MSG_ADD(MSG_MemoryEditor_Disabled),
    __MSG_ADD(MSG_MemoryEditor_Enabled),
    __MSG_ADD(MSG_MemoryEditor_WriteZ80_Unable),
    __MSG_ADD(MSG_MemoryEditor_Address_Out_of_Bound),

    __MSG_ADD(MSG_RapidFire_JxBx_On),
    __MSG_ADD(MSG_RapidFire_JxBx_Off),

    __MSG_ADD(MSG_FM_Enabled),
    __MSG_ADD(MSG_FM_Disabled),

    __MSG_ADD(MSG_Country_European_US),
    __MSG_ADD(MSG_Country_JAP),

    __MSG_ADD(MSG_Patch_Loading),
    __MSG_ADD(MSG_Patch_Missing),
    __MSG_ADD(MSG_Patch_Unrecognized),
    __MSG_ADD(MSG_Patch_Value_Not_a_Byte),
    __MSG_ADD(MSG_Patch_Out_of_Bound),

    __MSG_ADD(MSG_Glasses_Enabled),
    __MSG_ADD(MSG_Glasses_Disabled),
    __MSG_ADD(MSG_Glasses_Show_Both),
    __MSG_ADD(MSG_Glasses_Show_Left),
    __MSG_ADD(MSG_Glasses_Show_Right),
    __MSG_ADD(MSG_Glasses_Com_Port),
    __MSG_ADD(MSG_Glasses_Com_Port2),
    __MSG_ADD(MSG_Glasses_Com_Port_Open_Error),
    __MSG_ADD(MSG_Glasses_Unsupported),

    __MSG_ADD(MSG_Inputs_Joy_Init),
    __MSG_ADD(MSG_Inputs_Joy_Init_None),
    __MSG_ADD(MSG_Inputs_Joy_Init_Found),
    __MSG_ADD(MSG_Inputs_Joy_Calibrate_Error),

    __MSG_ADD(MSG_Inputs_Joypad),
    __MSG_ADD(MSG_Inputs_LightPhaser),
    __MSG_ADD(MSG_Inputs_PaddleControl),
    __MSG_ADD(MSG_Inputs_SportsPad),
    __MSG_ADD(MSG_Inputs_GraphicBoard),
    __MSG_ADD(MSG_Inputs_GraphicBoardV2),
    __MSG_ADD(MSG_Inputs_Play_Digital),
    __MSG_ADD(MSG_Inputs_Play_Mouse),
    __MSG_ADD(MSG_Inputs_Play_Digital_Unrecommended),
    __MSG_ADD(MSG_Inputs_Play_Pen),
    __MSG_ADD(MSG_Inputs_SK1100_Enabled),
    __MSG_ADD(MSG_Inputs_SK1100_Disabled),

    __MSG_ADD(MSG_Inputs_Config_BoxTitle),
    __MSG_ADD(MSG_Inputs_Config_Peripheral_Click),
    __MSG_ADD(MSG_Inputs_Config_Source_Enabled),
    __MSG_ADD(MSG_Inputs_Config_Source_Player),
    __MSG_ADD(MSG_Inputs_Config_Source_Emulate_Joypad),

    __MSG_ADD(MSG_Inputs_Src_Loading),
    __MSG_ADD(MSG_Inputs_Src_Not_Enough),
    __MSG_ADD(MSG_Inputs_Src_Missing),
    __MSG_ADD(MSG_Inputs_Src_Equal),
    __MSG_ADD(MSG_Inputs_Src_Unrecognized),
    __MSG_ADD(MSG_Inputs_Src_Syntax_Param),
    __MSG_ADD(MSG_Inputs_Src_Inconsistency),
    __MSG_ADD(MSG_Inputs_Src_Map_Keyboard),
    __MSG_ADD(MSG_Inputs_Src_Map_Keyboard_Ok),
    __MSG_ADD(MSG_Inputs_Src_Map_Joypad),
    __MSG_ADD(MSG_Inputs_Src_Map_Joypad_Ok_A),
    __MSG_ADD(MSG_Inputs_Src_Map_Joypad_Ok_B),
    __MSG_ADD(MSG_Inputs_Src_Map_Mouse),
    __MSG_ADD(MSG_Inputs_Src_Map_Mouse_Ok_B),
    __MSG_ADD(MSG_Inputs_Src_Map_Mouse_No_A),
    __MSG_ADD(MSG_Inputs_Src_Map_Cancelled),

    __MSG_ADD(MSG_Machine_Pause),
    __MSG_ADD(MSG_Machine_Resume),
    __MSG_ADD(MSG_Machine_Reset),

    __MSG_ADD(MSG_FDB_Loading),

    __MSG_ADD(MSG_DB_Loading),
    __MSG_ADD(MSG_DB_Name_Default),
    __MSG_ADD(MSG_DB_Name_NoCartridge),
    __MSG_ADD(MSG_DB_SyntaxError),

    __MSG_ADD(MSG_Config_Loading),

    __MSG_ADD(MSG_Datafile_Loading),

    __MSG_ADD(MSG_Driver_Unknown),

    __MSG_ADD(MSG_OverDump),

    __MSG_ADD(MSG_Sound_Init),
    __MSG_ADD(MSG_Sound_Init_Error_Audio),
    __MSG_ADD(MSG_Sound_Init_Error_Blaster),
    __MSG_ADD(MSG_Sound_Init_Error_Blaster_A),
    __MSG_ADD(MSG_Sound_Init_Error_Voices),
    __MSG_ADD(MSG_Sound_Init_Error_Voice_N),
    __MSG_ADD(MSG_Sound_Init_Soundcard),
    __MSG_ADD(MSG_Sound_Init_Soundcard_No),
    __MSG_ADD(MSG_Sound_Init_SN76496),
    __MSG_ADD(MSG_Sound_Init_YM2413_Digital),
    __MSG_ADD(MSG_Sound_Stream_Error),
    __MSG_ADD(MSG_Sound_Volume_Changed),

    __MSG_ADD(MSG_Theme_Loading),
    __MSG_ADD(MSG_Theme_Error_Not_Enough),
    __MSG_ADD(MSG_Theme_Error_Missing_Theme_Name),
    __MSG_ADD(MSG_Theme_Error_Syntax),
    __MSG_ADD(MSG_Theme_Error_Attribute_Defined),
    __MSG_ADD(MSG_Theme_Error_Out_of_Bound),
    __MSG_ADD(MSG_Theme_Error_Theme_Missing_Data),
    __MSG_ADD(MSG_Theme_Error_BG_Big),
    __MSG_ADD(MSG_Theme_Error_BG),
    __MSG_ADD(MSG_Theme_Error_BG_FileName),

    __MSG_ADD(MSG_LoadROM_Loading),
    __MSG_ADD(MSG_LoadROM_Success),
    __MSG_ADD(MSG_LoadDisk_Success),
    __MSG_ADD(MSG_LoadROM_Comment),
    __MSG_ADD(MSG_LoadROM_SMSGG_Mode_Comment),
    __MSG_ADD(MSG_LoadROM_Warning),
    __MSG_ADD(MSG_LoadROM_Bad_Dump_Long),
    __MSG_ADD(MSG_LoadROM_Bad_Dump_Short),
    __MSG_ADD(MSG_LoadROM_Product_Num),
    __MSG_ADD(MSG_LoadROM_SDSC),
    __MSG_ADD(MSG_LoadROM_SDSC_Name),
    __MSG_ADD(MSG_LoadROM_SDSC_Version),
    __MSG_ADD(MSG_LoadROM_SDSC_Date),
    __MSG_ADD(MSG_LoadROM_SDSC_Author),
    __MSG_ADD(MSG_LoadROM_SDSC_Release_Note),
    __MSG_ADD(MSG_LoadROM_SDSC_Unknown),
    __MSG_ADD(MSG_LoadROM_SDSC_Error),
    __MSG_ADD(MSG_LoadROM_Reload_Reloaded),
    __MSG_ADD(MSG_LoadROM_Reload_No_ROM),

    __MSG_ADD(MSG_FileBrowser_BoxTitle),
    __MSG_ADD(MSG_FileBrowser_Drive),
    __MSG_ADD(MSG_FileBrowser_Load),
    __MSG_ADD(MSG_FileBrowser_Close),
    __MSG_ADD(MSG_FileBrowser_LoadNames),
    __MSG_ADD(MSG_FileBrowser_ReloadDir),

    __MSG_ADD(MSG_FM_Editor_BoxTitle),
    __MSG_ADD(MSG_FM_Editor_Enabled),
    __MSG_ADD(MSG_FM_Editor_Disabled),

    __MSG_ADD(MSG_Frameskip_Auto),
    __MSG_ADD(MSG_Frameskip_Standard),
    __MSG_ADD(MSG_FPS_Counter_Enabled),
    __MSG_ADD(MSG_FPS_Counter_Disabled),

    __MSG_ADD(MSG_Log_Need_Param),
    __MSG_ADD(MSG_Log_Session_Start),

    __MSG_ADD(MSG_Load_Need_Param),
    __MSG_ADD(MSG_Load_Error),
    __MSG_ADD(MSG_Load_Not_Valid),
    __MSG_ADD(MSG_Load_Success),
    __MSG_ADD(MSG_Load_Version),
    __MSG_ADD(MSG_Load_Wrong_System),
    __MSG_ADD(MSG_Load_Massage),
    __MSG_ADD(MSG_Save_Not_in_BIOS),
    __MSG_ADD(MSG_Save_Error),
    __MSG_ADD(MSG_Save_Success),
    __MSG_ADD(MSG_Save_Slot),

    __MSG_ADD(MSG_Options_BoxTitle),
    __MSG_ADD(MSG_Options_Close),
    __MSG_ADD(MSG_Options_BIOS_Enable),
    __MSG_ADD(MSG_Options_DB_Display),
    __MSG_ADD(MSG_Options_Product_Number),
    __MSG_ADD(MSG_Options_Bright_Palette),
    __MSG_ADD(MSG_Options_Allow_Opposite_Directions),
    __MSG_ADD(MSG_Options_Load_Close),
    __MSG_ADD(MSG_Options_Load_FullScreen),
    __MSG_ADD(MSG_Options_FullScreen_Messages),
    __MSG_ADD(MSG_Options_GUI_VSync),
    __MSG_ADD(MSG_Options_Capture_Crop_Align),
    __MSG_ADD(MSG_Options_NES_Enable),
    __MSG_ADD(MSG_Options_GUI_GameWindowScale),

    __MSG_ADD(MSG_Language_Set),
    __MSG_ADD(MSG_Language_Set_Warning),

    __MSG_ADD(MSG_Sound_Dumping_Start),
    __MSG_ADD(MSG_Sound_Dumping_Stop),
    __MSG_ADD(MSG_Sound_Dumping_Error_File_1),
    __MSG_ADD(MSG_Sound_Dumping_Error_File_2),
    __MSG_ADD(MSG_Sound_Dumping_VGM_Acc_Frame),
    __MSG_ADD(MSG_Sound_Dumping_VGM_Acc_Sample),
    __MSG_ADD(MSG_Sound_Dumping_VGM_Acc_Change),

    __MSG_ADD(MSG_Menu_Main),
    __MSG_ADD(MSG_Menu_Main_LoadROM),
    __MSG_ADD(MSG_Menu_Main_FreeROM),
    __MSG_ADD(MSG_Menu_Main_SaveState_Save),
    __MSG_ADD(MSG_Menu_Main_SaveState_Load),
    __MSG_ADD(MSG_Menu_Main_SaveState_PrevSlot),
    __MSG_ADD(MSG_Menu_Main_SaveState_NextSlot),
    __MSG_ADD(MSG_Menu_Main_Options),
    __MSG_ADD(MSG_Menu_Main_Language),
    __MSG_ADD(MSG_Menu_Main_Quit),

    __MSG_ADD(MSG_Menu_Debug),
    __MSG_ADD(MSG_Menu_Debug_Enabled),
    __MSG_ADD(MSG_Menu_Debug_ReloadROM),
    __MSG_ADD(MSG_Menu_Debug_ReloadSymbols),
    __MSG_ADD(MSG_Menu_Debug_StepFrame),
    __MSG_ADD(MSG_Menu_Debug_LoadStateAndContinue),
    __MSG_ADD(MSG_Menu_Debug_Dump),

    __MSG_ADD(MSG_Menu_Machine),
    __MSG_ADD(MSG_Menu_Machine_Power),
    __MSG_ADD(MSG_Menu_Machine_Power_On),
    __MSG_ADD(MSG_Menu_Machine_Power_Off),
    __MSG_ADD(MSG_Menu_Machine_Region),
    __MSG_ADD(MSG_Menu_Machine_Region_Export),
    __MSG_ADD(MSG_Menu_Machine_Region_Japan),
    __MSG_ADD(MSG_Menu_Machine_TVType),
    __MSG_ADD(MSG_Menu_Machine_TVType_NTSC),
    __MSG_ADD(MSG_Menu_Machine_TVType_PALSECAM),
    __MSG_ADD(MSG_Menu_Machine_PauseEmulation),
    __MSG_ADD(Msg_Menu_Machine_ResetEmulation),

    __MSG_ADD(MSG_Menu_Video),
    __MSG_ADD(MSG_Menu_Video_FullScreen),
    __MSG_ADD(MSG_Menu_Video_Themes),
    __MSG_ADD(MSG_Menu_Video_Blitters),
    __MSG_ADD(MSG_Menu_Video_Layers),
    __MSG_ADD(MSG_Menu_Video_Layers_Sprites),
    __MSG_ADD(MSG_Menu_Video_Layers_Background),
    __MSG_ADD(MSG_Menu_Video_Flickering),
    __MSG_ADD(MSG_Menu_Video_Flickering_Auto),
    __MSG_ADD(MSG_Menu_Video_Flickering_Yes),
    __MSG_ADD(MSG_Menu_Video_Flickering_No),
    __MSG_ADD(MSG_Menu_Video_3DGlasses),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_Enabled),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_ShowBothSides),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_ShowLeftSide),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_ShowRightSide),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_UsesCOMPort),
    __MSG_ADD(Msg_Menu_Video_ScreenCapture),
    __MSG_ADD(Msg_Menu_Video_ScreenCapture_Capture),
    __MSG_ADD(Msg_Menu_Video_ScreenCapture_CaptureRepeat),
    __MSG_ADD(Msg_Menu_Video_ScreenCapture_IncludeGui),

    __MSG_ADD(MSG_Menu_Sound),
    __MSG_ADD(MSG_Menu_Sound_FM),
    __MSG_ADD(MSG_Menu_Sound_FM_Enabled),
    __MSG_ADD(MSG_Menu_Sound_FM_Disabled),
    __MSG_ADD(MSG_Menu_Sound_FM_Editor),
    __MSG_ADD(MSG_Menu_Sound_Volume),
    __MSG_ADD(MSG_Menu_Sound_Volume_Mute),
    __MSG_ADD(MSG_Menu_Sound_Volume_Value),
    __MSG_ADD(MSG_Menu_Sound_Rate),
    __MSG_ADD(MSG_Menu_Sound_Rate_Hz),
    __MSG_ADD(MSG_Menu_Sound_Channels),
    __MSG_ADD(MSG_Menu_Sound_Channels_Tone),
    __MSG_ADD(MSG_Menu_Sound_Channels_Noises),
    __MSG_ADD(MSG_Menu_Sound_Capture),
    __MSG_ADD(MSG_Menu_Sound_Capture_WAV_Start),
    __MSG_ADD(MSG_Menu_Sound_Capture_WAV_Stop),
    __MSG_ADD(MSG_Menu_Sound_Capture_VGM_Start),
    __MSG_ADD(MSG_Menu_Sound_Capture_VGM_Stop),
    __MSG_ADD(MSG_Menu_Sound_Capture_VGM_SampleAccurate),

    __MSG_ADD(MSG_Menu_Inputs),
    __MSG_ADD(MSG_Menu_Inputs_Joypad),
    __MSG_ADD(MSG_Menu_Inputs_LightPhaser),
    __MSG_ADD(MSG_Menu_Inputs_PaddleControl),
    __MSG_ADD(MSG_Menu_Inputs_SportsPad),
    __MSG_ADD(MSG_Menu_Inputs_GraphicBoard),
    __MSG_ADD(MSG_Menu_Inputs_GraphicBoardV2),
    __MSG_ADD(MSG_Menu_Inputs_SK1100),
    __MSG_ADD(MSG_Menu_Inputs_RapidFire),
    __MSG_ADD(MSG_Menu_Inputs_RapidFire_PxBx),
    __MSG_ADD(MSG_Menu_Inputs_Configuration),

    __MSG_ADD(MSG_Menu_Tools),
    __MSG_ADD(MSG_Menu_Tools_Messages),
    __MSG_ADD(MSG_Menu_Tools_Palette),
    __MSG_ADD(MSG_Menu_Tools_TilesViewer),
    __MSG_ADD(MSG_Menu_Tools_TilemapViewer),
    __MSG_ADD(MSG_Menu_Tools_CheatFinder),
    __MSG_ADD(MSG_Menu_Tools_TechInfo),
    __MSG_ADD(MSG_Menu_Tools_MemoryEditor),

    __MSG_ADD(MSG_Menu_Help),
    __MSG_ADD(MSG_Menu_Help_Documentation),
    __MSG_ADD(MSG_Menu_Help_Compat),
    __MSG_ADD(MSG_Menu_Help_Multiplayer_Games),
    __MSG_ADD(MSG_Menu_Help_Changes),
    __MSG_ADD(MSG_Menu_Help_Debugger),
    __MSG_ADD(MSG_Menu_Help_About),

    { NULL, MSG_NULL }
};
#undef __MSG_ADD

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Win32 Console
#ifdef ARCH_WIN32

struct t_console_win32
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
};

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

#endif

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

static bool             ConsolePause;

#ifdef ARCH_WIN32
static t_console_win32  ConsoleWin32;
#endif

//-----------------------------------------------------------------------------
// LANGUAGES
//-----------------------------------------------------------------------------

t_lang *    Lang_New(char *name)
{
    for (t_list* langs = Messages.Langs; langs; langs = langs->next)
    {
        t_lang* lang = (t_lang*)langs->elem;
        if (stricmp (name, lang->Name) == 0)
            return lang;
    }

    t_lang* lang = (t_lang*)malloc(sizeof (t_lang));
    lang->Name = strdup (name);
    for (int i = 0; i < MSG_MAX; i++)
        lang->Messages[i] = NULL;
    lang->WIP = FALSE;
    list_add_to_end (&Messages.Langs, lang);
    return (lang);
}

void    Lang_Delete(t_lang* lang)
{
    for (int i = 0; i < MSG_MAX; i++)
        free(lang->Messages[i]);
    free(lang->Name);
    free(lang);
}

int     Lang_Post_Check (t_lang *lang)
{
    // Count available messages (skipping MSG_NULL) and set default for when one is missing
    int cnt = 0;
    for (int i = 1; i < MSG_MAX; i++)
        if (lang->Messages[i])
            cnt++;
    if (cnt < MSG_MAX - 1)
    {
        // We need to display the first line even in WIP mode, if this is the
        // default language, else MEKA will screw up later, with missing strings..
        if (lang->WIP == FALSE || lang == Messages.Lang_Default)
        {
            ConsolePrintf ("Language \"%s\" is incomplete (%d/%d messages found) !\n", lang->Name, cnt, MSG_MAX - 1);
            ConsoleEnablePause();
        }
        if (lang->WIP == FALSE)
        {
            ConsolePrintf ("The following messages are missing:\n");
            for (int i = 1; i < MSG_MAX; i++)
                if (lang->Messages[i] == NULL)
                    for (int j = 0; Msg_Translation_Table[j].name; j++)
                        if (Msg_Translation_Table[j].value == i)
                            ConsolePrintf ("  %s\n", Msg_Translation_Table[j].name);
            ConsoleEnablePause();
        }
        return (MEKA_ERR_INCOMPLETE);
    }
    return (MEKA_ERR_OK);
}

int     Lang_Message_Add (t_lang *lang, char *msg_id, char *msg)
{
    // Find message number (#define) by name
    int n = -1;
    for (int i = 0; Msg_Translation_Table[i].name; i++)
    {
        if (stricmp (msg_id, Msg_Translation_Table[i].name) == 0)
        {
            n = Msg_Translation_Table[i].value;
            break;
        }
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

    // lang->Messages [n] = strdup (msg);
    lang->Messages[n] = parse_getword(NULL, 0, &msg, "\"", 0);

    // Verify that there's nothing after this line
    parse_skip_spaces(&msg);
    if (msg[0])
        return (MEKA_ERR_SYNTAX);

    return (MEKA_ERR_OK);
}

static void     Lang_Set (t_menu_event *event)
{
    Messages.Lang_Cur = (t_lang *)event->user_data;
    gui_menu_uncheck_all (menus_ID.languages);
    gui_menu_check (menus_ID.languages, event->menu_item_idx);
    Msg(MSGT_USER, Msg_Get(MSG_Language_Set), Messages.Lang_Cur->Name);
    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_Language_Set_Warning));

    // Post-process
    // FIXME: Rebuild menus
    gamebox_rename_all();
    gui_relayout_all();
}

void            Lang_Set_by_Name (char *name)
{
    for (t_list* langs = Messages.Langs; langs; langs = langs->next)
    {
        t_lang* lang = (t_lang*)langs->elem;
        if (stricmp (lang->Name, name) == 0)
        {
            Messages.Lang_Cur = lang;
            return;
        }
    }
}

void            Langs_Menu_Add (int menu_id)
{
    const int s = list_size(Messages.Langs);
    if (s > 1)
    {
        menus_ID.languages = menu_add_menu (menu_id, Msg_Get(MSG_Menu_Main_Language), MENU_ITEM_FLAG_ACTIVE);
        for (t_list* langs = Messages.Langs; langs; langs = langs->next)
        {
            t_lang* lang = (t_lang*)langs->elem;
            menu_add_item(menus_ID.languages, lang->Name, NULL, MENU_ITEM_FLAG_ACTIVE | Is_Checked (lang == Messages.Lang_Cur), (t_menu_callback)Lang_Set, lang);
        }
    }
}

//-----------------------------------------------------------------------------
// MESSAGING SYSTEM
//-----------------------------------------------------------------------------

int             Messages_Init_Parse_Line (char *line)
{
    char *      p;

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
    StrTrim (line);
    StrUpper(line);
    StrTrim (p + 1);
    if ((p = strchr (p + 1, '\"')) == NULL)
    {
        free(line);
        return (MEKA_ERR_SYNTAX);
    }
    // if ((p2 = strrchr (p + 1, '\"')) == NULL)
    //    return (MEKA_ERR_SYNTAX);
    // *p2 = EOSTR;
    int ret = Lang_Message_Add(Messages.Lang_Cur, line, p + 1);
    free(line);
    return (ret);
}

// Load messages from MEKA.MSG file (path given in structure)
// Return a MEKA_ERR_xxx code
int     Messages_Init()
{
    Messages.Lang_Cur = Messages.Lang_Default = NULL;
    Messages.Langs = NULL;

    // Note: this is one of the few cases were the string has to be hardcoded.
    // That is of course because the messages/localization system is not
    // initialized as of yet..
    ConsolePrint("Loading MEKA.MSG (messages).. ");

    // Open and read file --------------------------------------------------------
    t_tfile* tf = tfile_read (Messages.FileName);
    if (tf == NULL)
        Quit_Msg("MISSING!\nTry re-installing your version of Meka.");
    ConsolePrint("\n");

    // Parse each line -----------------------------------------------------------
    int line_cnt = 0;
    for (t_list* lines = tf->data_lines; lines; lines = lines->next)
    {
        char *line = (char*)lines->elem;
        line_cnt += 1;

        // Cut Comments
        char* p = strchr (line, ';');
        if (p != NULL)
            *p = EOSTR;

        StrTrim (line);
        if (StrIsNull (line))
            continue;

        // Parse Line and handle errors
        // ConsolePrintf ("%d: %s--\n", line_cnt, line);
        switch (Messages_Init_Parse_Line(line))
        {
        case MEKA_ERR_MISSING:
            ConsolePrintf ("On line %d: No language defined for storing message !", line_cnt);
            tfile_free(tf);
            Quit();
            break;
        case MEKA_ERR_UNKNOWN:
            ConsolePrintf ("On line %d: Unknown message \"%s\", skipping it.\n", line_cnt, line);
            // tfile_free(tf);
            // Quit();
            break;
        case MEKA_ERR_SYNTAX:
            ConsolePrintf ("On line %d: Syntax error.\n%s\n", line_cnt, line);
            tfile_free(tf);
            Quit();
            break;
        }
    }

    // Free file data
    tfile_free(tf);

    // Verify language completion
    {
        if (Messages.Lang_Cur == NULL)
            Quit_Msg("No language defined. Try re-installing your version of Meka.");
        Messages.Lang_Cur = Messages.Lang_Default;
        for (t_list* langs = Messages.Langs; langs; langs = langs->next)
        {
            t_lang* lang = (t_lang*)langs->elem;
            if (Lang_Post_Check (lang) != MEKA_ERR_OK)
                if (lang == Messages.Lang_Default)
                    Quit_Msg("This is the default language, so we need to abort.");
        }
    }

    // Ok
    return (MEKA_ERR_OK);
}

void    Messages_Close()
{
    for (t_list* langs = Messages.Langs; langs; langs = langs->next)
    {
        t_lang* lang = (t_lang*)langs->elem;
        Lang_Delete(lang);
    }
    list_free_no_elem(&Messages.Langs);
}

// Initialize text output console.
void            ConsoleInit()
{
    // Reset pause flag
    ConsolePause = FALSE;

    // Initialize Win32 console
    #ifdef ARCH_WIN32
        const HINSTANCE hInstance = GetModuleHandle(NULL);
        const HWND hWndParent = 0; // win_get_window()
        ConsoleWin32_Initialize(&ConsoleWin32, hInstance, hWndParent);
        // ConsoleWin32_Show(&ConsoleWin32);
    #endif
}

// Close console.
void            ConsoleClose()
{
    // Close Win32 console
    #ifdef ARCH_WIN32
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
// ConsolePrint(const char *msg)
// Print message to console.
//-----------------------------------------------------------------------------
void            ConsolePrint(const char *msg)
{
    // FIXME: to do
    #ifdef ARCH_WIN32
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

// Enable console pausing. The Win32 console will display until user has chosen "Quit" or "Run".
void            ConsoleEnablePause()
{
    // Set pause flag
    ConsolePause = TRUE;
}

bool            ConsoleWaitForAnswer(bool allow_run)
{
#ifndef ARCH_WIN32
    return TRUE;
#else
    if (!ConsolePause)
        return TRUE;

    // Else... wait for signal
    return ConsoleWin32_WaitForAnswer(&ConsoleWin32, allow_run);

#endif
}

const char*     Msg_Get(int n)
{
    const char* msg = Messages.Lang_Cur->Messages[n];
    if (msg)
        return msg;
    msg = Messages.Lang_Default->Messages[n];
    return msg;
}

// Send a message to the user and/or debugging message
void            Msg(int attr, const char *format, ...)
{
    va_list     params;
    char *      src;
    char *      p;

    va_start (params, format);
    vsprintf (Msg_Buf, format, params);
    va_end   (params);

    #ifdef MSG_USER
        if (attr == MSG_USER) // Allegro constant!!
            Quit_Msg("Fatal: avoid using MSG_USER, it is an Allegro constant!");
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
        if (attr & MSGT_STATUS_BAR)
        {
            strcpy(g_gui_status.message, src);
            g_gui_status.timeleft = 120;
        }

        // Add to user text box
        if (attr & MSGT_USER_LOG)
            NewGui_LogAddTextLine(src);

#ifdef WIN32
        if (attr & MSGT_ATTR_DEBUG)
        {
            OutputDebugString(src);
            OutputDebugString("\n");
        }
#endif
    }
    while (p != NULL);
}

//-----------------------------------------------------------------------------
// WINDOWS CONSOLE
//-----------------------------------------------------------------------------

#ifdef ARCH_WIN32

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
        return false;

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
    t_console_win32* c = (t_console_win32*)data;

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
            // char buffer[256];
            // dialogInit(hDlg, message, wParam, lParam);
            //sprintf(buffer, "MEKA %s\nStartup in progress...", MEKA_VERSION);
            //SetWindowText(GetDlgItem(hDlg, IDC_CONSOLE_INFO), buffer);
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
    text = (char*)Memory_Alloc(strlen(s) + (newlines_counter * sizeof(char)) + 2 + 1);
    {
        char* dst = text;
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

#endif // ARCH_WIN32

//-----------------------------------------------------------------------------

