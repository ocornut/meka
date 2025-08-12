//-----------------------------------------------------------------------------
// MEKA - message.h
// Messaging System, Languages, Console - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Messages types
#define MSGT_USER_LOG                   (0x01)
#define MSGT_STATUS_BAR                 (0x02)
#define MSGT_ATTR_DEBUG                 (0x04)

#define MSGT_USER                       (MSGT_USER_LOG | MSGT_STATUS_BAR)
#define MSGT_DEBUG                      (MSGT_USER_LOG | MSGT_ATTR_DEBUG)

//-----------------------------------------------------------------------------

// String used to enable WIP for a language
#define MSG_LANG_WIP_STR                "WIP"

// Messages definitions
enum
{
    MSG_NULL                                    = 0,

    MSG_Welcome,
    MSG_Window_Title,
    MSG_Quit,

    MSG_About_BoxTitle,
    MSG_About_Line_Meka_Date,
    MSG_About_Line_Authors,

    MSG_Ok,
    MSG_Failed,
    MSG_Error_Base,
    MSG_Error_Error,
    MSG_Error_Memory,
    MSG_Error_Param,
    MSG_Error_Syntax,

    MSG_Error_Video_Mode,
    MSG_Error_Video_Mode_Back_To_GUI,

    MSG_Error_File_Not_Found,
    MSG_Error_File_Read,
    MSG_Error_File_Empty,

    MSG_Error_ZIP_Not_Supported,
    MSG_Error_ZIP_Loading,
    MSG_Error_ZIP_Internal,

    MSG_Error_Directory_Open,

    MSG_Must_Reset,
    MSG_No_ROM,

    MSG_Init_Allegro,
    MSG_Init_GUI,
    MSG_Init_Completed,

    MSG_Setup_Running,
    MSG_Setup_Setup,
    MSG_Setup_Video_Driver,
    MSG_Setup_Video_DisplayMode,
    MSG_Setup_SampleRate_Select,

    MSG_Capture_Done,
    MSG_Capture_Error,
    MSG_Capture_Error_File,

    MSG_SRAM_Loaded,
    MSG_SRAM_Load_Unable,
    MSG_SRAM_Wrote,
    MSG_SRAM_Write_Unable,

    MSG_93c46_Reset,
    MSG_93c46_Loaded,
    MSG_93c46_Load_Unable,
    MSG_93c46_Wrote,
    MSG_93c46_Write_Unable,

    MSG_TVType_Set,
    MSG_TVType_Info_Speed,

    MSG_Blitters_Loading,
    MSG_Blitters_Error_Not_Enough,
    MSG_Blitters_Error_Not_Found,
    MSG_Blitters_Error_Missing,
    MSG_Blitters_Error_Unrecognized,
    MSG_Blitters_Error_Incorrect_Value,
    MSG_Blitters_Set,

    MSG_NES_Activate,
    MSG_NES_Sucks,
    MSG_NES_Mapper_Unknown,
    MSG_NES_Deny_Facts,

    MSG_Debug_Init,
    MSG_Debug_Welcome,
    MSG_Debug_Not_Available,
    MSG_Debug_Trap_Read,
    MSG_Debug_Trap_Write,
    MSG_Debug_Trap_Port_Read,
    MSG_Debug_Trap_Port_Write,
    MSG_Debug_Symbols_Loaded,
    MSG_Debug_Symbols_Error,
    MSG_Debug_Symbols_Error_Line,

    MSG_DataDump_Error,
    MSG_DataDump_Error_OB_Memory,
    MSG_DataDump_Error_Palette,
    MSG_DataDump_Error_Sprites,
    MSG_DataDump_Main,

    MSG_Doc_BoxTitle,
    MSG_Doc_File_Error,
    MSG_Doc_Enabled,
    MSG_Doc_Disabled,

    MSG_Flickering_Auto,
    MSG_Flickering_Yes,
    MSG_Flickering_No,

    MSG_Layer_BG_Disabled,
    MSG_Layer_BG_Enabled,
    MSG_Layer_Spr_Disabled,
    MSG_Layer_Spr_Enabled,

    MSG_FDC765_Unknown_Read,
    MSG_FDC765_Unknown_Write,
    MSG_FDC765_Disk_Too_Large1,
    MSG_FDC765_Disk_Too_Large2,
    MSG_FDC765_Disk_Too_Small1,
    MSG_FDC765_Disk_Too_Small2,

    MSG_Palette_BoxTitle,
    MSG_Palette_Disabled,
    MSG_Palette_Enabled,

    MSG_Message_BoxTitle,

    MSG_TechInfo_BoxTitle,
    MSG_TechInfo_Disabled,
    MSG_TechInfo_Enabled,

    MSG_TilesViewer_BoxTitle,
    MSG_TilesViewer_Disabled,
    MSG_TilesViewer_Enabled,
    MSG_TilesViewer_Tile,

    MSG_MemoryEditor_BoxTitle,
    MSG_MemoryEditor_Disabled,
    MSG_MemoryEditor_Enabled,
    MSG_MemoryEditor_WriteZ80_Unable,
    MSG_MemoryEditor_Address_Out_of_Bound,

    MSG_RapidFire_JxBx_On,
    MSG_RapidFire_JxBx_Off,

    MSG_FM_Enabled,
    MSG_FM_Disabled,

    MSG_Country_European_US,
    MSG_Country_JAP,

    MSG_Patch_Loading,
    MSG_Patch_Missing,
    MSG_Patch_Unrecognized,
    MSG_Patch_Value_Not_a_Byte,
    MSG_Patch_Out_of_Bound,

    MSG_Glasses_Enabled,
    MSG_Glasses_Disabled,
    MSG_Glasses_Show_Both,
    MSG_Glasses_Show_Left,
    MSG_Glasses_Show_Right,
    MSG_Glasses_Com_Port,
    MSG_Glasses_Com_Port2,
    MSG_Glasses_Com_Port_Open_Error,
    MSG_Glasses_Unsupported,

    MSG_Inputs_Joy_Init,
    MSG_Inputs_Joy_Init_None,
    MSG_Inputs_Joy_Init_Found,
    MSG_Inputs_Joy_Calibrate_Error,

    MSG_Inputs_Joypad,
    MSG_Inputs_LightPhaser,
    MSG_Inputs_PaddleControl,
    MSG_Inputs_SportsPad,
    MSG_Inputs_GraphicBoard,
    MSG_Inputs_GraphicBoardV2,
    MSG_Inputs_Play_Digital,
    MSG_Inputs_Play_Mouse,
    MSG_Inputs_Play_Digital_Unrecommended,
    MSG_Inputs_Play_Pen,
    MSG_Inputs_SK1100_Enabled,
    MSG_Inputs_SK1100_Disabled,

    MSG_Inputs_Config_BoxTitle,
    MSG_Inputs_Config_Peripheral_Click,
    MSG_Inputs_Config_Source_Enabled,
    MSG_Inputs_Config_Source_Player,
    MSG_Inputs_Config_Source_Emulate_Joypad,

    MSG_Inputs_Src_Loading,
    MSG_Inputs_Src_Not_Enough,
    MSG_Inputs_Src_Missing,
    MSG_Inputs_Src_Equal,
    MSG_Inputs_Src_Unrecognized,
    MSG_Inputs_Src_Syntax_Param,
    MSG_Inputs_Src_Inconsistency,
    MSG_Inputs_Src_Map_Keyboard,
    MSG_Inputs_Src_Map_Keyboard_Ok,
    MSG_Inputs_Src_Map_Joypad,
    MSG_Inputs_Src_Map_Joypad_Ok_A,
    MSG_Inputs_Src_Map_Joypad_Ok_B,
    MSG_Inputs_Src_Map_Mouse,
    MSG_Inputs_Src_Map_Mouse_Ok_B,
    MSG_Inputs_Src_Map_Mouse_No_A,
    MSG_Inputs_Src_Map_Cancelled,

    MSG_Machine_Pause,
    MSG_Machine_Resume,
    MSG_Machine_Reset,

    MSG_FDB_Loading,

    MSG_DB_Loading,
    MSG_DB_Name_Default,
    MSG_DB_Name_NoCartridge,
    MSG_DB_SyntaxError,

    MSG_Config_Loading,

    MSG_Datafile_Loading,

    MSG_Driver_Unknown,

    MSG_OverDump,

    MSG_Sound_Init,
    MSG_Sound_Init_Error_Audio,
    MSG_Sound_Init_Error_Blaster,
    MSG_Sound_Init_Error_Blaster_A,
    MSG_Sound_Init_Error_Voices,
    MSG_Sound_Init_Error_Voice_N,
    MSG_Sound_Init_Soundcard,
    MSG_Sound_Init_Soundcard_No,
    MSG_Sound_Init_SN76496,
    MSG_Sound_Init_YM2413_Digital,
    MSG_Sound_Stream_Error,
    MSG_Sound_Volume_Changed,

    MSG_Theme_Loading,
    MSG_Theme_Error_Not_Enough,
    MSG_Theme_Error_Missing_Theme_Name,
    MSG_Theme_Error_Syntax,
    MSG_Theme_Error_Attribute_Defined,
    MSG_Theme_Error_Out_of_Bound,
    MSG_Theme_Error_Theme_Missing_Data,
    MSG_Theme_Error_BG_Big,
    MSG_Theme_Error_BG,
    MSG_Theme_Error_BG_FileName,

    MSG_LoadROM_Loading,
    MSG_LoadROM_Success,
    MSG_LoadDisk_Success,
    MSG_LoadROM_Comment,
    MSG_LoadROM_SMSGG_Mode_Comment,
    MSG_LoadROM_Warning,
    MSG_LoadROM_Bad_Dump_Long,
    MSG_LoadROM_Bad_Dump_Short,
    MSG_LoadROM_Product_Num,
    MSG_LoadROM_SDSC,
    MSG_LoadROM_SDSC_Name,
    MSG_LoadROM_SDSC_Version,
    MSG_LoadROM_SDSC_Date,
    MSG_LoadROM_SDSC_Author,
    MSG_LoadROM_SDSC_Release_Note,
    MSG_LoadROM_SDSC_Unknown,
    MSG_LoadROM_SDSC_Error,
    MSG_LoadROM_Reload_Reloaded,
    MSG_LoadROM_Reload_No_ROM,

    MSG_FileBrowser_BoxTitle,
    MSG_FileBrowser_Drive,
    MSG_FileBrowser_Load,
    MSG_FileBrowser_Close,
    MSG_FileBrowser_LoadNames,
    MSG_FileBrowser_ReloadDir,

    MSG_FM_Editor_BoxTitle,
    MSG_FM_Editor_Enabled,
    MSG_FM_Editor_Disabled,

    MSG_Frameskip_Auto,
    MSG_Frameskip_Standard,
    MSG_FPS_Counter_Enabled,
    MSG_FPS_Counter_Disabled,

    MSG_Log_Need_Param,
    MSG_Log_Session_Start,

    MSG_Load_Need_Param,
    MSG_Load_Error,
    MSG_Load_Not_Valid,
    MSG_Load_Success,
    MSG_Load_Version,
    MSG_Load_Wrong_System,
    MSG_Load_Massage,
    MSG_Save_Not_in_BIOS,
    MSG_Save_Error,
    MSG_Save_Success,
    MSG_Save_Slot,

    MSG_Options_BoxTitle,
    MSG_Options_Close,
    MSG_Options_BIOS_Enable,
    MSG_Options_DB_Display,
    MSG_Options_Product_Number,
    MSG_Options_Bright_Palette,
    MSG_Options_Allow_Opposite_Directions,
    MSG_Options_Load_Close,
    MSG_Options_Load_FullScreen,
    MSG_Options_FullScreen_Messages,
    MSG_Options_GUI_VSync,
    MSG_Options_Capture_Crop_Align,
    MSG_Options_NES_Enable,
    MSG_Options_GUI_GameWindowScale,

    MSG_Language_Set,
    MSG_Language_Set_Warning,

    MSG_Sound_Dumping_Start,
    MSG_Sound_Dumping_Stop,
    MSG_Sound_Dumping_Error_File_1,
    MSG_Sound_Dumping_Error_File_2,
    MSG_Sound_Dumping_VGM_Acc_Frame,
    MSG_Sound_Dumping_VGM_Acc_Sample,
    MSG_Sound_Dumping_VGM_Acc_Change,

    MSG_Menu_Main,
    MSG_Menu_Main_LoadROM,
    MSG_Menu_Main_FreeROM,
    MSG_Menu_Main_SaveState_Save,
    MSG_Menu_Main_SaveState_Load,
    MSG_Menu_Main_SaveState_PrevSlot,
    MSG_Menu_Main_SaveState_NextSlot,
    MSG_Menu_Main_Options,
    MSG_Menu_Main_Language,
    MSG_Menu_Main_Quit,

    MSG_Menu_Debug,
    MSG_Menu_Debug_Enabled,
    MSG_Menu_Debug_ReloadROM,
    MSG_Menu_Debug_ReloadSymbols,
    MSG_Menu_Debug_StepFrame,
    MSG_Menu_Debug_LoadStateAndContinue,
    MSG_Menu_Debug_Dump,

    MSG_Menu_Machine,
    MSG_Menu_Machine_Power,
    MSG_Menu_Machine_Power_On,
    MSG_Menu_Machine_Power_Off,
    MSG_Menu_Machine_Region,
    MSG_Menu_Machine_Region_Export,
    MSG_Menu_Machine_Region_Japan,
    MSG_Menu_Machine_TVType,
    MSG_Menu_Machine_TVType_NTSC,
    MSG_Menu_Machine_TVType_PALSECAM,
    MSG_Menu_Machine_PauseEmulation,
    Msg_Menu_Machine_ResetEmulation,

    MSG_Menu_Video,
    MSG_Menu_Video_FullScreen,
    MSG_Menu_Video_Themes,
    MSG_Menu_Video_Blitters,
    MSG_Menu_Video_Layers,
    MSG_Menu_Video_Layers_Sprites,
    MSG_Menu_Video_Layers_Background,
    MSG_Menu_Video_Flickering,
    MSG_Menu_Video_Flickering_Auto,
    MSG_Menu_Video_Flickering_Yes,
    MSG_Menu_Video_Flickering_No,
    MSG_Menu_Video_3DGlasses,
    MSG_Menu_Video_3DGlasses_Enabled,
    MSG_Menu_Video_3DGlasses_ShowBothSides,
    MSG_Menu_Video_3DGlasses_ShowLeftSide,
    MSG_Menu_Video_3DGlasses_ShowRightSide,
    MSG_Menu_Video_3DGlasses_UsesCOMPort,
    Msg_Menu_Video_ScreenCapture,
    Msg_Menu_Video_ScreenCapture_Capture,
    Msg_Menu_Video_ScreenCapture_CaptureRepeat,
    Msg_Menu_Video_ScreenCapture_IncludeGui,

    MSG_Menu_Sound,
    MSG_Menu_Sound_FM,
    MSG_Menu_Sound_FM_Enabled,
    MSG_Menu_Sound_FM_Disabled,
    MSG_Menu_Sound_FM_Editor,
    MSG_Menu_Sound_Volume,
    MSG_Menu_Sound_Volume_Mute,
    MSG_Menu_Sound_Volume_Value,
    MSG_Menu_Sound_Rate,
    MSG_Menu_Sound_Rate_Hz,
    MSG_Menu_Sound_Channels,
    MSG_Menu_Sound_Channels_Tone,
    MSG_Menu_Sound_Channels_Noises,
    MSG_Menu_Sound_Capture,
    MSG_Menu_Sound_Capture_WAV_Start,
    MSG_Menu_Sound_Capture_WAV_Stop,
    MSG_Menu_Sound_Capture_VGM_Start,
    MSG_Menu_Sound_Capture_VGM_Stop,
    MSG_Menu_Sound_Capture_VGM_SampleAccurate,

    MSG_Menu_Inputs,
    MSG_Menu_Inputs_Joypad,
    MSG_Menu_Inputs_LightPhaser,
    MSG_Menu_Inputs_PaddleControl,
    MSG_Menu_Inputs_SportsPad,
    MSG_Menu_Inputs_GraphicBoard,
    MSG_Menu_Inputs_GraphicBoardV2,
    MSG_Menu_Inputs_SK1100,
    MSG_Menu_Inputs_RapidFire,
    MSG_Menu_Inputs_RapidFire_PxBx,
    MSG_Menu_Inputs_Configuration,

    MSG_Menu_Tools,
    MSG_Menu_Tools_Messages,
    MSG_Menu_Tools_Palette,
    MSG_Menu_Tools_TilesViewer,
    MSG_Menu_Tools_TilemapViewer,
    MSG_Menu_Tools_CheatFinder,
    MSG_Menu_Tools_TechInfo,
    MSG_Menu_Tools_MemoryEditor,

    MSG_Menu_Help,
    MSG_Menu_Help_Documentation,
    MSG_Menu_Help_Compat,
    MSG_Menu_Help_Multiplayer_Games,
    MSG_Menu_Help_Changes,
    MSG_Menu_Help_Debugger,
    MSG_Menu_Help_About,

    // Number of messages
    MSG_MAX,
};

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// Structure storing messages for one supported language
struct t_lang
{
    char *      Name;
    char *      Messages[MSG_MAX];
    int         WIP;
};

//-----------------------------------------------------------------------------

void            Lang_Set_by_Name(const char* name);

//-----------------------------------------------------------------------------

// Structure storing localization basis stuff and all supported languages
struct t_messages
{
    char        FileName [FILENAME_LEN];      // path to the MEKA.MSG file
    t_lang *    Lang_Cur;
    t_lang *    Lang_Default;
    t_list *    Langs;
};

extern t_messages Messages;

//-----------------------------------------------------------------------------
// Functions - Message
//-----------------------------------------------------------------------------

// Load messages from MEKA.MSG file (path given in structure)
// Return a MEKA_ERR_xxx code
int     Messages_Init();
void    Messages_Close();

// Get specified message string
const char* Msg_Get(int n);

// Send a message to the user and/or debugging message
void        Msg(int attr, const char *format, ...) FORMAT_PRINTF (2);

//-----------------------------------------------------------------------------
// Functions - Console
//-----------------------------------------------------------------------------

void    ConsoleInit();
void    ConsoleClose();
void    ConsolePrintf(const char *format, ...) FORMAT_PRINTF (1);
void    ConsolePrint(const char *msg);
void    ConsoleEnablePause();
bool    ConsoleWaitForAnswer(bool allow_run);

//-----------------------------------------------------------------------------
