//-----------------------------------------------------------------------------
// MEKA - g_menu_i.c
// GUI Menus Initialization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_about.h"
#include "app_cheatfinder.h"
#include "app_filebrowser.h"
#include "app_mapview.h"
#include "app_memview.h"
#include "app_options.h"
#include "app_palview.h"
#include "app_techinfo.h"
#include "app_textview.h"
#include "app_tileview.h"
#include "blitintf.h"
#include "capture.h"
#include "datadump.h"
#include "debugger.h"
#include "file.h"
#include "sound/fmunit.h"
#include "glasses.h"
#include "inputs_c.h"
#include "rapidfir.h"
#include "saves.h"
#include "sk1100.h"
#include "textbox.h"
#include "tvtype.h"
#include "vmachine.h"
#include "sound/s_misc.h"
#include "sound/sound_logging.h"

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

static void Debug_ReloadSymbols_Callback(t_menu_event*)
{
    Debugger_Symbols_Load();
}

static void Debug_StepFrame_Callback(t_menu_event*)
{
    g_machine_pause_requests = (g_machine_flags & MACHINE_PAUSED) ? 2 : 1;
}

static void Debug_LoadStateAndContinue_Callback(t_menu_event*)
{
    
}

//-----------------------------------------------------------------------------

// Initialize and create GUI menus
void    gui_menus_init()
{
    char buffer[256];

    t_font_id font_id = (t_font_id)g_config.font_menus;

    g_gui_status.x = 10;
    Msg(MSGT_USER_LOG, Msg_Get(MSG_Welcome), MEKA_NAME_VERSION, MEKA_DATE " " MEKA_AUTHORS);
    Msg(MSGT_STATUS_BAR, Msg_Get(MSG_Welcome), MEKA_NAME_VERSION, MEKA_DATE " " MEKA_AUTHORS_SHORT);
    menus_opt.spacing = Font_TextWidth(font_id, "  ");
    menus_opt.spacing_render = gui.info.screen.x - 40;
    menus_opt.c_menu = -1;
    menus_opt.c_entry = -1;

    //-------------------------------------------------------------------------
    // <ROOT>
    //-------------------------------------------------------------------------
    menus_ID.root     = menu_new();
    menus_ID.file     = menu_add_menu (menus_ID.root, Msg_Get(MSG_Menu_Main),    MENU_ITEM_FLAG_ACTIVE);
    menus_ID.machine  = menu_add_menu (menus_ID.root, Msg_Get(MSG_Menu_Machine), MENU_ITEM_FLAG_ACTIVE);
    menus_ID.video    = menu_add_menu (menus_ID.root, Msg_Get(MSG_Menu_Video),   MENU_ITEM_FLAG_ACTIVE);
    menus_ID.sound    = menu_add_menu (menus_ID.root, Msg_Get(MSG_Menu_Sound),   MENU_ITEM_FLAG_ACTIVE);
    menus_ID.inputs   = menu_add_menu (menus_ID.root, Msg_Get(MSG_Menu_Inputs),  MENU_ITEM_FLAG_ACTIVE);
    menus_ID.tools    = menu_add_menu (menus_ID.root, Msg_Get(MSG_Menu_Tools),   MENU_ITEM_FLAG_ACTIVE);
    menus_ID.debug    = menu_add_menu (menus_ID.root, Msg_Get(MSG_Menu_Debug),   MENU_ITEM_FLAG_ACTIVE);
    menus_ID.help     = menu_add_menu (menus_ID.root, Msg_Get(MSG_Menu_Help),    MENU_ITEM_FLAG_ACTIVE);

    //-------------------------------------------------------------------------
    // MAIN
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_LoadROM),           "Alt+L",    MENU_ITEM_FLAG_ACTIVE | Is_Checked (FB.active), (t_menu_callback)FB_Switch, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_FreeROM),           NULL,       MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Free_ROM, NULL);
    menu_add_separator(menus_ID.file);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_SaveState_Save),    "F5",       MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)SaveState_Save, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_SaveState_Load),    "F7",       MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)SaveState_Load, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_SaveState_PrevSlot), "F6",      MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)SaveState_SetPrevSlot, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_SaveState_NextSlot), "F8",      MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)SaveState_SetNextSlot, NULL);
    menu_add_separator(menus_ID.file);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_Options),           "Alt+O",    MENU_ITEM_FLAG_ACTIVE | Is_Checked (Options.active), (t_menu_callback)Options_Switch, NULL);
    Langs_Menu_Add    (menus_ID.file);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_Quit),              "F10",      MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Action_Quit, NULL);

    //-------------------------------------------------------------------------
    // DEBUG
    //-------------------------------------------------------------------------
#ifdef MEKA_Z80_DEBUGGER
    menu_add_item (menus_ID.debug,  Msg_Get(MSG_Menu_Debug_Enabled),        "Scroll Lock",  MENU_ITEM_FLAG_ACTIVE | Is_Checked (Debugger.active), (t_menu_callback)Debugger_Switch, NULL);
    menu_add_item (menus_ID.debug,  Msg_Get(MSG_Menu_Debug_ReloadROM),      NULL,           MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Reload_ROM, NULL);
    menu_add_item (menus_ID.debug,  Msg_Get(MSG_Menu_Debug_ReloadSymbols),  NULL,           MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Debug_ReloadSymbols_Callback, NULL);
    menu_add_item (menus_ID.debug,  Msg_Get(MSG_Menu_Debug_StepFrame),      "Ctrl+F12",     MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Debug_StepFrame_Callback, NULL);
    menu_add_item (menus_ID.debug,  Msg_Get(MSG_Menu_Debug_LoadStateAndContinue),"Ctrl+F7", MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Debug_LoadStateAndContinue_Callback, NULL);
    menus_ID.dump  = menu_add_menu (menus_ID.debug, Msg_Get(MSG_Menu_Debug_Dump),           MENU_ITEM_FLAG_ACTIVE);
    DataDump_Init_Menus (menus_ID.dump);
    //menus_ID.watch = menu_add_menu (menus_ID.debug, Msg_Get(MSG_Menu_Debug_Watch),      0);
#endif // MEKA_Z80_DEBUGGER

    //-------------------------------------------------------------------------
    // MACHINE
    //-------------------------------------------------------------------------
    menus_ID.power    = menu_add_menu (menus_ID.machine, Msg_Get(MSG_Menu_Machine_Power),  MENU_ITEM_FLAG_ACTIVE);
    menus_ID.region   = menu_add_menu (menus_ID.machine, Msg_Get(MSG_Menu_Machine_Region), MENU_ITEM_FLAG_ACTIVE);
    menus_ID.tvtype   = menu_add_menu (menus_ID.machine, Msg_Get(MSG_Menu_Machine_TVType), MENU_ITEM_FLAG_ACTIVE);
    menu_add_item     (menus_ID.machine,  Msg_Get(MSG_Menu_Machine_PauseEmulation),        "F12",           MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Machine_Pause, NULL);
    menu_add_item     (menus_ID.machine,  Msg_Get(Msg_Menu_Machine_ResetEmulation),        "Alt+Backspace", MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Machine_Reset, NULL);
    // MACHINE -> POWER
    menu_add_item     (menus_ID.power,    Msg_Get(MSG_Menu_Machine_Power_On),              NULL,            MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Machine_ON, NULL);
    menu_add_item     (menus_ID.power,    Msg_Get(MSG_Menu_Machine_Power_Off),             NULL,            MENU_ITEM_FLAG_ACTIVE /* | AM_Checked */, (t_menu_callback)Machine_OFF, NULL);
    // MACHINE -> COUNTRY
    menu_add_item     (menus_ID.region,   Msg_Get(MSG_Menu_Machine_Region_Export),         NULL,            MENU_ITEM_FLAG_ACTIVE | Is_Checked (g_config.country == COUNTRY_EXPORT), (t_menu_callback)Set_Country_Export, NULL);
    menu_add_item     (menus_ID.region,   Msg_Get(MSG_Menu_Machine_Region_Japan),          NULL,            MENU_ITEM_FLAG_ACTIVE | Is_Checked (g_config.country == COUNTRY_JAPAN),  (t_menu_callback)Set_Country_Japan, NULL);
    // MACHINE -> TV TYPE
    menu_add_item     (menus_ID.tvtype,   Msg_Get(MSG_Menu_Machine_TVType_NTSC),           NULL,            MENU_ITEM_FLAG_ACTIVE | Is_Checked (TV_Type_User->id == TVTYPE_NTSC), (t_menu_callback)TVType_Set_NTSC, NULL);
    menu_add_item     (menus_ID.tvtype,   Msg_Get(MSG_Menu_Machine_TVType_PALSECAM),       NULL,            MENU_ITEM_FLAG_ACTIVE | Is_Checked (TV_Type_User->id == TVTYPE_PAL_SECAM), (t_menu_callback)TVType_Set_PAL_SECAM, NULL);

    //-------------------------------------------------------------------------
    // VIDEO
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.video,    Msg_Get(MSG_Menu_Video_FullScreen),              "Escape",        MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Action_Switch_Mode, NULL);

    // VIDEO -> CAPTURE
    menus_ID.screenshots = menu_add_menu (menus_ID.video, Msg_Get(Msg_Menu_Video_ScreenCapture),    MENU_ITEM_FLAG_ACTIVE);
    menu_add_item(menus_ID.screenshots, Msg_Get(Msg_Menu_Video_ScreenCapture_Capture),      "PrintScreen",  MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Capture_MenuHandler_Capture, NULL);
    menu_add_item(menus_ID.screenshots, Msg_Get(Msg_Menu_Video_ScreenCapture_CaptureRepeat),NULL,           MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Capture_MenuHandler_AllFrames, NULL);
    menu_add_item(menus_ID.screenshots, Msg_Get(Msg_Menu_Video_ScreenCapture_IncludeGui),   NULL,           MENU_ITEM_FLAG_ACTIVE | Is_Checked(g_config.capture_include_gui), (t_menu_callback)Capture_MenuHandler_IncludeGui, NULL);
    // VIDEO -> THEMES
    menus_ID.themes   = menu_add_menu (menus_ID.video, Msg_Get(MSG_Menu_Video_Themes),     MENU_ITEM_FLAG_ACTIVE);
    Skins_MenuInit (menus_ID.themes);
    // VIDEO -> BLITTERS
    menus_ID.blitters = menu_add_menu (menus_ID.video, Msg_Get(MSG_Menu_Video_Blitters),   MENU_ITEM_FLAG_ACTIVE);
    Blitters_Menu_Init(menus_ID.blitters);
    // VIDEO -> LAYERS
    menus_ID.layers   = menu_add_menu (menus_ID.video, Msg_Get(MSG_Menu_Video_Layers),     MENU_ITEM_FLAG_ACTIVE);
    menu_add_item     (menus_ID.layers,   Msg_Get(MSG_Menu_Video_Layers_Sprites),          "F11",           MENU_ITEM_FLAG_ACTIVE | MENU_ITEM_FLAG_CHECKED, (t_menu_callback)Action_Switch_Layer_Sprites, NULL);
    menu_add_item     (menus_ID.layers,   Msg_Get(MSG_Menu_Video_Layers_Background),       "Ctrl+F11",      MENU_ITEM_FLAG_ACTIVE | MENU_ITEM_FLAG_CHECKED, (t_menu_callback)Action_Switch_Layer_Background, NULL);
    // VIDEO -> FLICKERING
    menus_ID.flickering = menu_add_menu (menus_ID.video, Msg_Get(MSG_Menu_Video_Flickering), MENU_ITEM_FLAG_ACTIVE);
    menu_add_item     (menus_ID.flickering, Msg_Get(MSG_Menu_Video_Flickering_Auto),       NULL,            MENU_ITEM_FLAG_ACTIVE | Is_Checked (g_config.sprite_flickering & SPRITE_FLICKERING_AUTO), (t_menu_callback)Action_Switch_Flickering_Auto, NULL);
    menu_add_item     (menus_ID.flickering, Msg_Get(MSG_Menu_Video_Flickering_Yes),        NULL,            MENU_ITEM_FLAG_ACTIVE | Is_Checked (!(g_config.sprite_flickering & SPRITE_FLICKERING_AUTO) && (g_config.sprite_flickering & SPRITE_FLICKERING_ENABLED)), (t_menu_callback)Action_Switch_Flickering_Yes, NULL);
    menu_add_item     (menus_ID.flickering, Msg_Get(MSG_Menu_Video_Flickering_No),         NULL,            MENU_ITEM_FLAG_ACTIVE | Is_Checked (!(g_config.sprite_flickering & SPRITE_FLICKERING_AUTO) && !(g_config.sprite_flickering & SPRITE_FLICKERING_ENABLED)), (t_menu_callback)Action_Switch_Flickering_No, NULL);
    // VIDEO -> GLASSES (3-D)
    menus_ID.glasses  = menu_add_menu (menus_ID.video, Msg_Get(MSG_Menu_Video_3DGlasses),  MENU_ITEM_FLAG_ACTIVE);
    menu_add_item     (menus_ID.glasses,  Msg_Get(MSG_Menu_Video_3DGlasses_Enabled),       NULL,            MENU_ITEM_FLAG_ACTIVE | Is_Checked (Glasses.Enabled), (t_menu_callback)Glasses_Switch_Enable, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get(MSG_Menu_Video_3DGlasses_ShowBothSides), NULL,            Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_BOTH), (t_menu_callback)Glasses_Switch_Mode_Show_Both, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get(MSG_Menu_Video_3DGlasses_ShowLeftSide),  NULL,            Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_ONLY_LEFT), (t_menu_callback)Glasses_Switch_Mode_Show_Left, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get(MSG_Menu_Video_3DGlasses_ShowRightSide), NULL,            Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_ONLY_RIGHT), (t_menu_callback)Glasses_Switch_Mode_Show_Right, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get(MSG_Menu_Video_3DGlasses_UsesCOMPort),   NULL,            Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_COM_PORT), (t_menu_callback)Glasses_Switch_Mode_Com_Port, NULL);

    //-------------------------------------------------------------------------
    // SOUND
    //-------------------------------------------------------------------------
    // SOUND -> FM
    menus_ID.fm       = menu_add_menu (menus_ID.sound, Msg_Get(MSG_Menu_Sound_FM),        MENU_ITEM_FLAG_ACTIVE);
    menu_add_item     (menus_ID.fm,       Msg_Get(MSG_Menu_Sound_FM_Enabled),             NULL,         MENU_ITEM_FLAG_ACTIVE | Is_Checked (Sound.FM_Enabled == TRUE),     (t_menu_callback)FM_Enable, NULL);
    menu_add_item     (menus_ID.fm,       Msg_Get(MSG_Menu_Sound_FM_Disabled),            NULL,         MENU_ITEM_FLAG_ACTIVE | Is_Checked (Sound.FM_Enabled == FALSE ),   (t_menu_callback)FM_Disable, NULL);
    // menu_add_item  (menus_ID.fm,       Msg_Get(MSG_Menu_Sound_FM_Editor),              AM_Active | Is_Checked (apps.active.FM_Editor),     (t_menu_callback)FM_Editor_Switch, NULL);
    // SOUND -> VOLUME
    menus_ID.volume   = menu_add_menu (menus_ID.sound, Msg_Get(MSG_Menu_Sound_Volume),    MENU_ITEM_FLAG_ACTIVE);
    Sound_Volume_Menu_Init(menus_ID.volume);
    // SOUND -> CHANNELS
    menus_ID.channels = menu_add_menu (menus_ID.sound, Msg_Get(MSG_Menu_Sound_Channels), MENU_ITEM_FLAG_ACTIVE);
    for (int i = 0; i < 3; i++)
    {
        snprintf(buffer, countof(buffer), Msg_Get(MSG_Menu_Sound_Channels_Tone), i+1);
        menu_add_item  (menus_ID.channels, buffer, NULL, MENU_ITEM_FLAG_ACTIVE | MENU_ITEM_FLAG_CHECKED, (t_menu_callback)Sound_Channels_Menu_Handler, (void *)i);
    }
    menu_add_item     (menus_ID.channels, Msg_Get(MSG_Menu_Sound_Channels_Noises),        NULL, MENU_ITEM_FLAG_ACTIVE | MENU_ITEM_FLAG_CHECKED, (t_menu_callback)Sound_Channels_Menu_Handler, (void *)3);
    // SOUND -> LOGGING
    menus_ID.sound_log = menu_add_menu (menus_ID.sound, Msg_Get(MSG_Menu_Sound_Capture),        MENU_ITEM_FLAG_ACTIVE);
    menu_add_item     (menus_ID.sound_log, Msg_Get(MSG_Menu_Sound_Capture_VGM_Start),           "Alt+V",    MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Sound_LogVGM_Start, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get(MSG_Menu_Sound_Capture_VGM_Stop),            "Alt+V",    0, (t_menu_callback)Sound_LogVGM_Stop, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get(MSG_Menu_Sound_Capture_VGM_SampleAccurate), NULL,    MENU_ITEM_FLAG_ACTIVE | Is_Checked (Sound.LogVGM_Logging_Accuracy == VGM_LOGGING_ACCURACY_SAMPLE), (t_menu_callback)Sound_LogVGM_Accuracy_Switch, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get(MSG_Menu_Sound_Capture_WAV_Start),           NULL,   MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Sound_LogWAV_Start, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get(MSG_Menu_Sound_Capture_WAV_Stop),            NULL,   0, (t_menu_callback)Sound_LogWAV_Stop, NULL);

    SoundDebugApp_InstallMenuItems(menus_ID.sound);

    //-------------------------------------------------------------------------
    // INPUTS
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.inputs,   Msg_Get(MSG_Menu_Inputs_Joypad),          "F9",       MENU_ITEM_FLAG_ACTIVE | MENU_ITEM_FLAG_CHECKED, (t_menu_callback)Inputs_Switch_Joypad, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get(MSG_Menu_Inputs_LightPhaser),     NULL,       MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Inputs_Switch_LightPhaser, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get(MSG_Menu_Inputs_PaddleControl),   NULL,       MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Inputs_Switch_PaddleControl, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get(MSG_Menu_Inputs_SportsPad),       NULL,       MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Inputs_Switch_SportsPad, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get(MSG_Menu_Inputs_GraphicBoard),    NULL,       MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Inputs_Switch_GraphicBoard, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get(MSG_Menu_Inputs_GraphicBoardV2),  NULL,       MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)Inputs_Switch_GraphicBoardV2, NULL);
    menu_add_separator(menus_ID.inputs);
    menu_add_item     (menus_ID.inputs,   Msg_Get(MSG_Menu_Inputs_SK1100),          "CTRL+F9",  MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)SK1100_Switch, NULL);
    menu_add_separator(menus_ID.inputs);
    // INPUTS -> RAPID FIRE
    menus_ID.rapidfire = menu_add_menu (menus_ID.inputs, Msg_Get(MSG_Menu_Inputs_RapidFire), MENU_ITEM_FLAG_ACTIVE);
    snprintf          (buffer, countof(buffer), Msg_Get(MSG_Menu_Inputs_RapidFire_PxBx), 1, 1);
    menu_add_item     (menus_ID.rapidfire, buffer, NULL, MENU_ITEM_FLAG_ACTIVE | Is_Checked (RapidFire & RAPIDFIRE_J1B1), (t_menu_callback)RapidFire_Switch_J1B1, NULL);
    snprintf          (buffer, countof(buffer), Msg_Get(MSG_Menu_Inputs_RapidFire_PxBx), 1, 2);
    menu_add_item     (menus_ID.rapidfire, buffer, NULL, MENU_ITEM_FLAG_ACTIVE | Is_Checked (RapidFire & RAPIDFIRE_J1B2), (t_menu_callback)RapidFire_Switch_J1B2, NULL);
    snprintf          (buffer, countof(buffer), Msg_Get(MSG_Menu_Inputs_RapidFire_PxBx), 2, 1);
    menu_add_item     (menus_ID.rapidfire, buffer, NULL, MENU_ITEM_FLAG_ACTIVE | Is_Checked (RapidFire & RAPIDFIRE_J2B1), (t_menu_callback)RapidFire_Switch_J2B1, NULL);
    snprintf          (buffer, countof(buffer), Msg_Get(MSG_Menu_Inputs_RapidFire_PxBx), 2, 2);
    menu_add_item     (menus_ID.rapidfire, buffer, NULL, MENU_ITEM_FLAG_ACTIVE | Is_Checked (RapidFire & RAPIDFIRE_J2B2), (t_menu_callback)RapidFire_Switch_J2B2, NULL);
    // INPUTS (misc)
    menu_add_item     (menus_ID.inputs,    Msg_Get(MSG_Menu_Inputs_Configuration), NULL,    MENU_ITEM_FLAG_ACTIVE | Is_Checked (Inputs_CFG.active), (t_menu_callback)Inputs_CFG_Switch, NULL);

    //-------------------------------------------------------------------------
    // TOOLS
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.tools,     Msg_Get(MSG_Menu_Tools_Messages),      "Alt+M",  MENU_ITEM_FLAG_ACTIVE | Is_Checked (TB_Message.active),                  (t_menu_callback)TB_Message_Switch,                  NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get(MSG_Menu_Tools_Palette),       "Alt+P",  MENU_ITEM_FLAG_ACTIVE | Is_Checked (PaletteViewer.active),               (t_menu_callback)PaletteViewer_Switch,               NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get(MSG_Menu_Tools_TilesViewer),   "Alt+T",  MENU_ITEM_FLAG_ACTIVE | Is_Checked (TileViewer.active),                  (t_menu_callback)TileViewer_Switch,                  NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get(MSG_Menu_Tools_TilemapViewer), NULL,     MENU_ITEM_FLAG_ACTIVE | Is_Checked (TilemapViewer_MainInstance->active), (t_menu_callback)TilemapViewer_SwitchMainInstance,   NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get(MSG_Menu_Tools_MemoryEditor),  NULL,     MENU_ITEM_FLAG_ACTIVE | Is_Checked (MemoryViewer_MainInstance->active),  (t_menu_callback)MemoryViewer_SwitchMainInstance,    NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get(MSG_Menu_Tools_CheatFinder),   NULL,     MENU_ITEM_FLAG_ACTIVE | Is_Checked (g_CheatFinder_MainInstance->active), (t_menu_callback)CheatFinder_SwitchMainInstance,    NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get(MSG_Menu_Tools_TechInfo),      "Alt+I",  MENU_ITEM_FLAG_ACTIVE | Is_Checked (TechInfo.active),                    (t_menu_callback)TechInfo_Switch,                    NULL);

    //-------------------------------------------------------------------------
    // HELP
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.help,      Msg_Get(MSG_Menu_Help_Documentation),  NULL,     MENU_ITEM_FLAG_ACTIVE | Is_Checked (TextViewer.active), (t_menu_callback)TextViewer_Switch_Doc_Main, NULL);
    menu_add_item     (menus_ID.help,      Msg_Get(MSG_Menu_Help_Compat),         NULL,     MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)TextViewer_Switch_Doc_Compat, NULL);
    menu_add_item     (menus_ID.help,      Msg_Get(MSG_Menu_Help_Multiplayer_Games),NULL,   MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)TextViewer_Switch_Doc_Multiplayer_Games, NULL);
    menu_add_item     (menus_ID.help,      Msg_Get(MSG_Menu_Help_Changes),        NULL,     MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)TextViewer_Switch_Doc_Changes, NULL);
#ifdef MEKA_Z80_DEBUGGER
    menu_add_item     (menus_ID.help,      Msg_Get(MSG_Menu_Help_Debugger),       NULL,     MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)TextViewer_Switch_Doc_Debugger, NULL);
#endif // MEKA_Z80_DEBUGGER
    menu_add_item     (menus_ID.help,      Msg_Get(MSG_Menu_Help_About),          NULL,     MENU_ITEM_FLAG_ACTIVE | Is_Checked (AboutBox.active), (t_menu_callback)AboutBox_Switch, NULL);

    // ...
    gui_menu_un_mouse_over(menus_ID.root);
    gui_menus_update_size();
}

//-----------------------------------------------------------------------------

