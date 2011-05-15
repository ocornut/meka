//-----------------------------------------------------------------------------
// MEKA - g_menu_i.c
// GUI Menus Initialization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_about.h"
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
#include "g_file.h"
#include "glasses.h"
#include "inputs_c.h"
#include "keyboard.h"
#include "rapidfir.h"
#include "saves.h"

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// gui_menus_init ()
// Initialize and create GUI menus
//-----------------------------------------------------------------------------
void        gui_menus_init (void)
{
    int     i;
	char	buffer[256];

    gui_status.x = 10;
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Welcome), MEKA_NAME_VERSION, MEKA_DATE " " MEKA_AUTHORS);
    Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Welcome), MEKA_NAME_VERSION, MEKA_DATE " " MEKA_AUTHORS_SHORT);
    menus_opt.distance = gui.info.screen.x - 40;
    menus_opt.distance_usable = 20;
    menus_opt.c_menu = -1;
    menus_opt.c_entry = -1;

    //-------------------------------------------------------------------------
    // <ROOT>
    //-------------------------------------------------------------------------
    menus_ID.menu     = menu_new ();
    menus_ID.file     = menu_add_menu (menus_ID.menu, Msg_Get(MSG_Menu_Main),    AM_Active);
    if (g_Configuration.debug_mode)
       menus_ID.debug = menu_add_menu (menus_ID.menu, Msg_Get(MSG_Menu_Debug),   AM_Active);
    menus_ID.machine  = menu_add_menu (menus_ID.menu, Msg_Get(MSG_Menu_Machine), AM_Active);
    menus_ID.video    = menu_add_menu (menus_ID.menu, Msg_Get(MSG_Menu_Video),   AM_Active);
    menus_ID.sound    = menu_add_menu (menus_ID.menu, Msg_Get(MSG_Menu_Sound),   AM_Active);
    menus_ID.inputs   = menu_add_menu (menus_ID.menu, Msg_Get(MSG_Menu_Inputs),  AM_Active);
    menus_ID.tools    = menu_add_menu (menus_ID.menu, Msg_Get(MSG_Menu_Tools),   AM_Active);
    menus_ID.help     = menu_add_menu (menus_ID.menu, Msg_Get(MSG_Menu_Help),    AM_Active);

    //-------------------------------------------------------------------------
    // MAIN
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_LoadROM),   AM_Active | Is_Checked (FB.active), (t_menu_callback)FB_Switch, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_FreeROM),   AM_Active, (t_menu_callback)Free_ROM, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_SaveState), AM_Active, (t_menu_callback)Save_Game, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_LoadState), AM_Active, (t_menu_callback)Load_Game, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_Options),   AM_Active | Is_Checked (Options.active), (t_menu_callback)Options_Switch, NULL);
    Langs_Menu_Add    (menus_ID.file);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_Quit),      AM_Active, (t_menu_callback)Action_Quit, NULL);

    //-------------------------------------------------------------------------
    // DEBUG
    //-------------------------------------------------------------------------
#ifdef MEKA_Z80_DEBUGGER
    if (g_Configuration.debug_mode)
    {
        menu_add_item (menus_ID.debug,  Msg_Get (MSG_Menu_Debug_Enabled), AM_Active | Is_Checked (Debugger.active), (t_menu_callback)Debugger_Switch, NULL);
        menu_add_item (menus_ID.debug,  Msg_Get (MSG_Menu_Debug_Reload_ROM), AM_Active, (t_menu_callback)Reload_ROM, NULL);
        menus_ID.dump  = menu_add_menu (menus_ID.debug, Msg_Get (MSG_Menu_Debug_Dump),       AM_Active);
        DataDump_Init_Menus (menus_ID.dump);
        //menus_ID.watch = menu_add_menu (menus_ID.debug, Msg_Get (MSG_Menu_Debug_Watch),      AM_Nothing);
    }
#endif // MEKA_Z80_DEBUGGER
    //-------------------------------------------------------------------------
    // MACHINE
    //-------------------------------------------------------------------------
    menus_ID.power    = menu_add_menu (menus_ID.machine, Msg_Get (MSG_Menu_Machine_Power),  AM_Active);
    menus_ID.country  = menu_add_menu (menus_ID.machine, Msg_Get (MSG_Menu_Machine_Country),AM_Active);
    menus_ID.tvtype   = menu_add_menu (menus_ID.machine, Msg_Get (MSG_Menu_Machine_TVType), AM_Active);
    menu_add_item     (menus_ID.machine,  Msg_Get (MSG_Menu_Machine_HardPause),             AM_Active, (t_menu_callback)Machine_Pause, NULL);
    menu_add_item     (menus_ID.machine,  Msg_Get (MSG_Menu_Machine_HardReset),             AM_Active, (t_menu_callback)Machine_Reset, NULL);
    // MACHINE -> POWER
    menu_add_item     (menus_ID.power,    Msg_Get (MSG_Menu_Machine_Power_On),              AM_Active, (t_menu_callback)Machine_ON, NULL);
    menu_add_item     (menus_ID.power,    Msg_Get (MSG_Menu_Machine_Power_Off),             AM_Active /* | AM_Checked */, (t_menu_callback)Machine_OFF, NULL);
    // MACHINE -> COUNTRY
    menu_add_item     (menus_ID.country,  Msg_Get (MSG_Menu_Machine_Country_EU),            AM_Active | Is_Checked (g_Configuration.country == COUNTRY_EXPORT), (t_menu_callback)Set_Country_European_US, NULL);
    menu_add_item     (menus_ID.country,  Msg_Get (MSG_Menu_Machine_Country_Jap),           AM_Active | Is_Checked (g_Configuration.country == COUNTRY_JAPAN),  (t_menu_callback)Set_Country_JP, NULL);
    // MACHINE -> TV TYPE
    menu_add_item     (menus_ID.tvtype,   Msg_Get (MSG_Menu_Machine_TVType_NTSC),           AM_Active | Is_Checked (TV_Type_User->id == TVTYPE_NTSC), (t_menu_callback)TVType_Set_NTSC, NULL);
    menu_add_item     (menus_ID.tvtype,   Msg_Get (MSG_Menu_Machine_TVType_PALSECAM),       AM_Active | Is_Checked (TV_Type_User->id == TVTYPE_PAL_SECAM), (t_menu_callback)TVType_Set_PAL_SECAM, NULL);

    //-------------------------------------------------------------------------
    // VIDEO
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.video,    Msg_Get (MSG_Menu_Video_FullScreen),              AM_Active, (t_menu_callback)Action_Switch_Mode, NULL);

	// VIDEO -> CAPTURE
	menus_ID.screenshots = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Capture),	AM_Active);
	menu_add_item(menus_ID.screenshots, Msg_Get (MSG_Menu_Video_Capture_CaptureScreen),		AM_Active, (t_menu_callback)Capture_MenuHandler_Capture, NULL);
	menu_add_item(menus_ID.screenshots, Msg_Get (MSG_Menu_Video_Capture_CaptureScreenAll),	AM_Active, (t_menu_callback)Capture_MenuHandler_AllFrames, NULL);
	menu_add_item(menus_ID.screenshots, Msg_Get (MSG_Menu_Video_Capture_IncludeGui),		AM_Active | Is_Checked(g_Configuration.capture_include_gui), (t_menu_callback)Capture_MenuHandler_IncludeGui, NULL);
	// VIDEO -> THEMES
    menus_ID.themes   = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Themes),     AM_Active);
    Skins_MenuInit (menus_ID.themes);
    // VIDEO -> BLITTERS
    menus_ID.blitters = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Blitters),   AM_Active);
    Blitters_Menu_Init(menus_ID.blitters);
    // VIDEO -> LAYERS
    menus_ID.layers   = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Layers),     AM_Active);
    menu_add_item     (menus_ID.layers,   Msg_Get (MSG_Menu_Video_Layers_Sprites),          AM_Active | AM_Checked, (t_menu_callback)Action_Switch_Layer_Sprites, NULL);
    menu_add_item     (menus_ID.layers,   Msg_Get (MSG_Menu_Video_Layers_Background),       AM_Active | AM_Checked, (t_menu_callback)Action_Switch_Layer_Background, NULL);
    // VIDEO -> FLICKERING
    menus_ID.flickering = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Flickering), AM_Active);
    menu_add_item     (menus_ID.flickering, Msg_Get (MSG_Menu_Video_Flickering_Auto),       AM_Active | Is_Checked (g_Configuration.sprite_flickering & SPRITE_FLICKERING_AUTO), (t_menu_callback)Action_Switch_Flickering_Auto, NULL);
    menu_add_item     (menus_ID.flickering, Msg_Get (MSG_Menu_Video_Flickering_Yes),        AM_Active | Is_Checked (!(g_Configuration.sprite_flickering & SPRITE_FLICKERING_AUTO) && (g_Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED)), (t_menu_callback)Action_Switch_Flickering_Yes, NULL);
    menu_add_item     (menus_ID.flickering, Msg_Get (MSG_Menu_Video_Flickering_No),         AM_Active | Is_Checked (!(g_Configuration.sprite_flickering & SPRITE_FLICKERING_AUTO) && !(g_Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED)), (t_menu_callback)Action_Switch_Flickering_No, NULL);
    // VIDEO -> GLASSES (3-D)
    menus_ID.glasses  = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_3DGlasses),  AM_Active);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_Enabled),       AM_Active | Is_Checked (Glasses.Enabled), (t_menu_callback)Glasses_Switch_Enable, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_ShowBothSides), Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_BOTH), (t_menu_callback)Glasses_Switch_Mode_Show_Both, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_ShowLeftSide),  Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_ONLY_LEFT), (t_menu_callback)Glasses_Switch_Mode_Show_Left, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_ShowRightSide), Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_ONLY_RIGHT), (t_menu_callback)Glasses_Switch_Mode_Show_Right, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_UsesCOMPort),   Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_COM_PORT), (t_menu_callback)Glasses_Switch_Mode_Com_Port, NULL);

    //-------------------------------------------------------------------------
    // SOUND
    //-------------------------------------------------------------------------
    // SOUND -> FM
    menus_ID.fm       = menu_add_menu (menus_ID.sound, Msg_Get (MSG_Menu_Sound_FM),        AM_Active);
    menu_add_item     (menus_ID.fm,       Msg_Get (MSG_Menu_Sound_FM_Enabled),             AM_Active | Is_Checked (Sound.FM_Enabled == TRUE),     (t_menu_callback)FM_Enable, NULL);
    menu_add_item     (menus_ID.fm,       Msg_Get (MSG_Menu_Sound_FM_Disabled),            AM_Active | Is_Checked (Sound.FM_Enabled == FALSE ),   (t_menu_callback)FM_Disable, NULL);
    menus_ID.fm_emu   = menu_add_menu (menus_ID.fm, Msg_Get (MSG_Menu_Sound_FM_Emulator),  AM_Active);
    // menu_add_item     (menus_ID.fm,       Msg_Get (MSG_Menu_Sound_FM_Editor),              AM_Active | Is_Checked (apps.active.FM_Editor),     (t_menu_callback)FM_Editor_Switch, NULL);
    // SOUND -> FM -> EMULATOR
    menu_add_item     (menus_ID.fm_emu,   Msg_Get (MSG_Menu_Sound_FM_Emulator_OPL),        Is_Checked (Sound.FM_Emulator_Current == FM_EMULATOR_YM2413HD), (t_menu_callback)FM_Emulator_OPL, NULL);
    menu_add_item     (menus_ID.fm_emu,   Msg_Get (MSG_Menu_Sound_FM_Emulator_Digital),    Is_Checked (Sound.FM_Emulator_Current == FM_EMULATOR_EMU2413),  (t_menu_callback)FM_Emulator_Digital, NULL);
    // SOUND -> VOLUME
    menus_ID.volume   = menu_add_menu (menus_ID.sound, Msg_Get (MSG_Menu_Sound_Volume),    AM_Active);
    Sound_Volume_Menu_Init (menus_ID.volume);
    // SOUND -> RATE
    menus_ID.rate     = menu_add_menu (menus_ID.sound, Msg_Get (MSG_Menu_Sound_Rate),      AM_Active);
    Sound_Rate_Menu_Init (menus_ID.rate);
    // SOUND -> CHANNELS
    menus_ID.channels = menu_add_menu (menus_ID.sound, Msg_Get (MSG_Menu_Sound_Channels), AM_Active);
    for (i = 1; i <= 3; i += 1)
    {
        snprintf(buffer, countof(buffer), Msg_Get(MSG_Menu_Sound_Channels_Tone), i);
        menu_add_item  (menus_ID.channels, buffer, AM_Active | AM_Checked, (t_menu_callback)Sound_Channels_Menu_Handler, (void *)(i - 1));
    }
    menu_add_item     (menus_ID.channels, Msg_Get (MSG_Menu_Sound_Channels_Noises),        AM_Active | AM_Checked, (t_menu_callback)Sound_Channels_Menu_Handler, (void *)3);
    // SOUND -> LOGGING
    menus_ID.sound_log = menu_add_menu (menus_ID.sound, Msg_Get (MSG_Menu_Sound_Dump),     AM_Active);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_VGM_Start),        AM_Active, (t_menu_callback)Sound_LogVGM_Start, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_VGM_Stop),         AM_Nothing, (t_menu_callback)Sound_LogVGM_Stop, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_VGM_SampleAccurate), AM_Active | Is_Checked (Sound.LogVGM_Logging_Accuracy == VGM_LOGGING_ACCURACY_SAMPLE), (t_menu_callback)Sound_LogVGM_Accuracy_Switch, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_WAV_Start),        AM_Active, (t_menu_callback)Sound_LogWAV_Start, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_WAV_Stop),         AM_Nothing, (t_menu_callback)Sound_LogWAV_Stop, NULL);

    //-------------------------------------------------------------------------
    // INPUTS
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_Joypad),        AM_Active | AM_Checked, (t_menu_callback)Inputs_Switch_Joypad, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_LightPhaser),   AM_Active, (t_menu_callback)Inputs_Switch_LightPhaser, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_PaddleControl), AM_Active, (t_menu_callback)Inputs_Switch_PaddleControl, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_SportsPad),     AM_Active, (t_menu_callback)Inputs_Switch_SportsPad, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_GraphicBoard),  AM_Active, (t_menu_callback)Inputs_Switch_TVOekaki, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_SK1100),        AM_Active, (t_menu_callback)Keyboard_Switch, NULL);
    // INPUTS -> RAPID FIRE
    menus_ID.rapidfire = menu_add_menu (menus_ID.inputs, Msg_Get (MSG_Menu_Inputs_RapidFire), AM_Active);
    snprintf          (buffer, countof(buffer), Msg_Get (MSG_Menu_Inputs_RapidFire_PxBx), 1, 1);
    menu_add_item     (menus_ID.rapidfire, buffer, AM_Active | Is_Checked (RapidFire & RAPIDFIRE_J1B1), (t_menu_callback)RapidFire_Switch_J1B1, NULL);
    snprintf          (buffer, countof(buffer), Msg_Get (MSG_Menu_Inputs_RapidFire_PxBx), 1, 2);
    menu_add_item     (menus_ID.rapidfire, buffer, AM_Active | Is_Checked (RapidFire & RAPIDFIRE_J1B2), (t_menu_callback)RapidFire_Switch_J1B2, NULL);
    snprintf          (buffer, countof(buffer), Msg_Get (MSG_Menu_Inputs_RapidFire_PxBx), 2, 1);
    menu_add_item     (menus_ID.rapidfire, buffer, AM_Active | Is_Checked (RapidFire & RAPIDFIRE_J2B1), (t_menu_callback)RapidFire_Switch_J2B1, NULL);
    snprintf          (buffer, countof(buffer), Msg_Get (MSG_Menu_Inputs_RapidFire_PxBx), 2, 2);
    menu_add_item     (menus_ID.rapidfire, buffer, AM_Active | Is_Checked (RapidFire & RAPIDFIRE_J2B2), (t_menu_callback)RapidFire_Switch_J2B2, NULL);
    // INPUTS (misc)
    menu_add_item     (menus_ID.inputs,    Msg_Get (MSG_Menu_Inputs_Configuration), AM_Active | Is_Checked (Inputs_CFG.active), (t_menu_callback)Inputs_CFG_Switch, NULL);

    //-------------------------------------------------------------------------
    // TOOLS
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_Messages),      AM_Active | Is_Checked (TB_Message.active),                  (t_menu_callback)TB_Message_Switch,                  NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_Palette),       AM_Active | Is_Checked (PaletteViewer.active),               (t_menu_callback)PaletteViewer_Switch,               NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_TilesViewer),   AM_Active | Is_Checked (TileViewer.active),                  (t_menu_callback)TileViewer_Switch,                  NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_TilemapViewer), AM_Active | Is_Checked (TilemapViewer_MainInstance->active), (t_menu_callback)TilemapViewer_SwitchMainInstance,   NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_MemoryEditor),  AM_Active | Is_Checked (MemoryViewer_MainInstance->active),  (t_menu_callback)MemoryViewer_SwitchMainInstance,    NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_TechInfo),      AM_Active | Is_Checked (TechInfo.active),                    (t_menu_callback)TechInfo_Switch,                    NULL);

    //-------------------------------------------------------------------------
    // HELP
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Documentation),  AM_Active | Is_Checked (TextViewer.active), (t_menu_callback)TextViewer_Switch_Doc_Main, NULL);
    #ifdef ARCH_WIN32
        menu_add_item (menus_ID.help,      Msg_Get (MSG_Menu_Help_Documentation_W),AM_Active, (t_menu_callback)TextViewer_Switch_Doc_MainW, NULL);
    #endif
    #ifdef ARCH_UNIX
        menu_add_item (menus_ID.help,      Msg_Get (MSG_Menu_Help_Documentation_U),AM_Active, (t_menu_callback)TextViewer_Switch_Doc_MainU, NULL);
    #endif
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Compat),         AM_Active, (t_menu_callback)TextViewer_Switch_Doc_Compat, NULL);
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Multiplayer_Games),AM_Active, (t_menu_callback)TextViewer_Switch_Doc_Multiplayer_Games, NULL);
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Changes),        AM_Active, (t_menu_callback)TextViewer_Switch_Doc_Changes, NULL);
#ifdef MEKA_Z80_DEBUGGER
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Debugger),       AM_Active, (t_menu_callback)TextViewer_Switch_Doc_Debugger, NULL);
#endif // MEKA_Z80_DEBUGGER
	menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_About),          AM_Active | Is_Checked (AboutBox.active), (t_menu_callback)AboutBox_Switch, NULL);

    // ...
    gui_menu_un_mouse_over (menus_ID.menu);
    gui_menus_update_size ();
}

//-----------------------------------------------------------------------------

