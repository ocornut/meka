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
// gui_menus_init ()
// Initialize and create GUI menus
//-----------------------------------------------------------------------------
void        gui_menus_init (void)
{
    int     i;

    gui_status.x = 10;
#if 1
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Welcome), MEKA_NAME_VERSION, MEKA_AUTHORS);
    Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Welcome), MEKA_NAME_VERSION, MEKA_AUTHORS_SHORT);
#else
    // Arnosan lameness
    Msg (MSGT_USER_BOX, Msg_Get (MSG_Welcome), MEKA_NAME_VERSION, "Arnosan fan club");
    Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Welcome), MEKA_NAME_VERSION, "Arnosan fan club");
    Msg (MSGT_USER_BOX, " \nThis special edition is dedicated to the grand Arnosan, idol among the idols.\nArnosan had some wonderful comments to pass:\n- He is the best friend of Miyamoto.\n- He loves Famicom.\n- He is the ultimate Nintendo fan.\n- He has all Twins in all colors.\n- Nintendo died in 1996.\n \nEveryday, worship Arnosan!");
#endif
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
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_LoadROM),   AM_Active | Is_Checked (FB.active), FB_Switch, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_FreeROM),   AM_Active, Free_ROM, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_SaveState), AM_Active, Save_Game, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_LoadState), AM_Active, Load_Game, NULL);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_Options),   AM_Active | Is_Checked (Options.active), Options_Switch, NULL);
    Langs_Menu_Add    (menus_ID.file);
    menu_add_item     (menus_ID.file, Msg_Get(MSG_Menu_Main_Quit),      AM_Active, Action_Quit, NULL);

    //-------------------------------------------------------------------------
    // DEBUG
    //-------------------------------------------------------------------------
    if (g_Configuration.debug_mode)
    {
        menu_add_item (menus_ID.debug,  Msg_Get (MSG_Menu_Debug_Enabled), AM_Active | Is_Checked (Debugger.active), Debugger_Switch, NULL);
        menu_add_item (menus_ID.debug,  Msg_Get (MSG_Menu_Debug_Reload_ROM), AM_Active, Reload_ROM, NULL);
        menus_ID.dump  = menu_add_menu (menus_ID.debug, Msg_Get (MSG_Menu_Debug_Dump),       AM_Active);
        DataDump_Init_Menus (menus_ID.dump);
        //menus_ID.watch = menu_add_menu (menus_ID.debug, Msg_Get (MSG_Menu_Debug_Watch),      AM_Nothing);
    }

    //-------------------------------------------------------------------------
    // MACHINE
    //-------------------------------------------------------------------------
    menus_ID.power    = menu_add_menu (menus_ID.machine, Msg_Get (MSG_Menu_Machine_Power),  AM_Active);
    menus_ID.country  = menu_add_menu (menus_ID.machine, Msg_Get (MSG_Menu_Machine_Country),AM_Active);
    menus_ID.tvtype   = menu_add_menu (menus_ID.machine, Msg_Get (MSG_Menu_Machine_TVType), AM_Active);
    menu_add_item     (menus_ID.machine,  Msg_Get (MSG_Menu_Machine_HardPause),             AM_Active, Machine_Pause, NULL);
    menu_add_item     (menus_ID.machine,  Msg_Get (MSG_Menu_Machine_HardReset),             AM_Active, Machine_Reset, NULL);
    // MACHINE -> POWER
    menu_add_item     (menus_ID.power,    Msg_Get (MSG_Menu_Machine_Power_On),              AM_Active, Machine_ON, NULL);
    menu_add_item     (menus_ID.power,    Msg_Get (MSG_Menu_Machine_Power_Off),             AM_Active /* | AM_Checked */, Machine_OFF, NULL);
    // MACHINE -> COUNTRY
    menu_add_item     (menus_ID.country,  Msg_Get (MSG_Menu_Machine_Country_EU),            AM_Active | Is_Checked (g_Configuration.country == COUNTRY_EXPORT), Set_Country_European_US, NULL);
    menu_add_item     (menus_ID.country,  Msg_Get (MSG_Menu_Machine_Country_Jap),           AM_Active | Is_Checked (g_Configuration.country == COUNTRY_JAPAN),    Set_Country_JP, NULL);
    // MACHINE -> TV TYPE
    menu_add_item     (menus_ID.tvtype,   Msg_Get (MSG_Menu_Machine_TVType_NTSC),           AM_Active | Is_Checked (TV_Type_User->id == TVTYPE_NTSC), TVType_Set_NTSC, NULL);
    menu_add_item     (menus_ID.tvtype,   Msg_Get (MSG_Menu_Machine_TVType_PALSECAM),       AM_Active | Is_Checked (TV_Type_User->id == TVTYPE_PAL_SECAM), TVType_Set_PAL_SECAM, NULL);

    //-------------------------------------------------------------------------
    // VIDEO
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.video,    Msg_Get (MSG_Menu_Video_FullScreen),              AM_Active, Action_Switch_Mode, NULL);
    // VIDEO -> THEMES
    menus_ID.themes   = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Themes),     AM_Active);
    Skins_MenuInit (menus_ID.themes);
    // VIDEO -> BLITTERS
    menus_ID.blitters = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Blitters),   AM_Active);
    Blitters_Menu_Init(menus_ID.blitters);
    // VIDEO -> LAYERS
    menus_ID.layers   = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Layers),     AM_Active);
    menu_add_item     (menus_ID.layers,   Msg_Get (MSG_Menu_Video_Layers_Sprites),          AM_Active | AM_Checked, Action_Switch_Layer_Sprites, NULL);
    menu_add_item     (menus_ID.layers,   Msg_Get (MSG_Menu_Video_Layers_Background),       AM_Active | AM_Checked, Action_Switch_Layer_Background, NULL);
    // VIDEO -> FLICKERING
    menus_ID.flickering = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Flickering), AM_Active);
    menu_add_item     (menus_ID.flickering, Msg_Get (MSG_Menu_Video_Flickering_Auto),       AM_Active | Is_Checked (g_Configuration.sprite_flickering & SPRITE_FLICKERING_AUTO), Action_Switch_Flickering_Auto, NULL);
    menu_add_item     (menus_ID.flickering, Msg_Get (MSG_Menu_Video_Flickering_Yes),        AM_Active | Is_Checked (!(g_Configuration.sprite_flickering & SPRITE_FLICKERING_AUTO) && (g_Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED)), Action_Switch_Flickering_Yes, NULL);
    menu_add_item     (menus_ID.flickering, Msg_Get (MSG_Menu_Video_Flickering_No),         AM_Active | Is_Checked (!(g_Configuration.sprite_flickering & SPRITE_FLICKERING_AUTO) && !(g_Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED)), Action_Switch_Flickering_No, NULL);
    // VIDEO -> GLASSES (3-D)
    menus_ID.glasses  = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_3DGlasses),  AM_Active);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_Enabled),       AM_Active | Is_Checked (Glasses.Enabled), Glasses_Switch_Enable, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_ShowBothSides), Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_BOTH), Glasses_Switch_Mode_Show_Both, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_ShowLeftSide),  Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_ONLY_LEFT), Glasses_Switch_Mode_Show_Left, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_ShowRightSide), Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_SHOW_ONLY_RIGHT), Glasses_Switch_Mode_Show_Right, NULL);
    menu_add_item     (menus_ID.glasses,  Msg_Get (MSG_Menu_Video_3DGlasses_UsesCOMPort),   Is_Active (Glasses.Enabled) | Is_Checked (Glasses.Mode == GLASSES_MODE_COM_PORT), Glasses_Switch_Mode_Com_Port, NULL);
    // VIDEO -> SCREENS
    //menus_ID.screens  = menu_add_menu (menus_ID.video, Msg_Get (MSG_Menu_Video_Screens),    AM_Active);
    //menu_add_item     (menus_ID.screens,  Msg_Get (MSG_Menu_Video_Screens_New),             AM_Active, gamebox_create_on_mouse_pos, NULL);
    //menu_add_item     (menus_ID.screens,  Msg_Get (MSG_Menu_Video_Screens_KillLast),        AM_Nothing, gamebox_kill_last, NULL);
    //menu_add_item     (menus_ID.screens,  Msg_Get (MSG_Menu_Video_Screens_KillAll),         AM_Nothing, gamebox_kill_all, NULL);

    //-------------------------------------------------------------------------
    // SOUND
    //-------------------------------------------------------------------------
    // SOUND -> FM
    menus_ID.fm       = menu_add_menu (menus_ID.sound, Msg_Get (MSG_Menu_Sound_FM),        AM_Active);
    menu_add_item     (menus_ID.fm,       Msg_Get (MSG_Menu_Sound_FM_Enabled),             AM_Active | Is_Checked (Sound.FM_Enabled == TRUE),     FM_Enable, NULL);
    menu_add_item     (menus_ID.fm,       Msg_Get (MSG_Menu_Sound_FM_Disabled),            AM_Active | Is_Checked (Sound.FM_Enabled == FALSE ),     FM_Disable, NULL);
    menus_ID.fm_emu   = menu_add_menu (menus_ID.fm, Msg_Get (MSG_Menu_Sound_FM_Emulator),  AM_Active);
    // menu_add_item     (menus_ID.fm,       Msg_Get (MSG_Menu_Sound_FM_Editor),              AM_Active | Is_Checked (apps.active.FM_Editor),       FM_Editor_Switch, NULL);
    // SOUND -> FM -> EMULATOR
    menu_add_item     (menus_ID.fm_emu,   Msg_Get (MSG_Menu_Sound_FM_Emulator_OPL),        Is_Checked (Sound.FM_Emulator_Current == FM_EMULATOR_YM2413HD), FM_Emulator_OPL, NULL);
    menu_add_item     (menus_ID.fm_emu,   Msg_Get (MSG_Menu_Sound_FM_Emulator_Digital),    Is_Checked (Sound.FM_Emulator_Current == FM_EMULATOR_EMU2413),  FM_Emulator_Digital, NULL);
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
        sprintf (GenericBuffer, Msg_Get (MSG_Menu_Sound_Channels_Tone), i);
        menu_add_item  (menus_ID.channels, GenericBuffer, AM_Active | AM_Checked, Sound_Channels_Menu_Handler, (void *)(i - 1));
    }
    menu_add_item     (menus_ID.channels, Msg_Get (MSG_Menu_Sound_Channels_Noises),        AM_Active | AM_Checked, Sound_Channels_Menu_Handler, (void *)3);
    // SOUND -> LOGGING
    menus_ID.sound_log = menu_add_menu (menus_ID.sound, Msg_Get (MSG_Menu_Sound_Dump),     AM_Active);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_VGM_Start),        AM_Active, Sound_LogVGM_Start, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_VGM_Stop),         AM_Nothing, Sound_LogVGM_Stop, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_VGM_SampleAccurate), AM_Active | Is_Checked (Sound.LogVGM_Logging_Accuracy == VGM_LOGGING_ACCURACY_SAMPLE), Sound_LogVGM_Accuracy_Switch, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_WAV_Start),        AM_Active, Sound_LogWAV_Start, NULL);
    menu_add_item     (menus_ID.sound_log, Msg_Get (MSG_Menu_Sound_Dump_WAV_Stop),         AM_Nothing, Sound_LogWAV_Stop, NULL);

    //-------------------------------------------------------------------------
    // INPUTS
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_Joypad),        AM_Active | AM_Checked, Inputs_Switch_Joypad, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_LightPhaser),   AM_Active, Inputs_Switch_LightPhaser, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_PaddleControl), AM_Active, Inputs_Switch_PaddleControl, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_SportsPad),     AM_Active, Inputs_Switch_SportsPad, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_GraphicBoard),  AM_Active, Inputs_Switch_TVOekaki, NULL);
    menu_add_item     (menus_ID.inputs,   Msg_Get (MSG_Menu_Inputs_SK1100),        AM_Active, Keyboard_Switch, NULL);
    // INPUTS -> RAPID FIRE
    menus_ID.rapidfire = menu_add_menu (menus_ID.inputs, Msg_Get (MSG_Menu_Inputs_RapidFire), AM_Active);
    sprintf           (GenericBuffer, Msg_Get (MSG_Menu_Inputs_RapidFire_PxBx), 1, 1);
    menu_add_item     (menus_ID.rapidfire, GenericBuffer, AM_Active | Is_Checked (RapidFire & RAPIDFIRE_J1B1), RapidFire_Switch_J1B1, NULL);
    sprintf           (GenericBuffer, Msg_Get (MSG_Menu_Inputs_RapidFire_PxBx), 1, 2);
    menu_add_item     (menus_ID.rapidfire, GenericBuffer, AM_Active | Is_Checked (RapidFire & RAPIDFIRE_J1B2), RapidFire_Switch_J1B2, NULL);
    sprintf           (GenericBuffer, Msg_Get (MSG_Menu_Inputs_RapidFire_PxBx), 2, 1);
    menu_add_item     (menus_ID.rapidfire, GenericBuffer, AM_Active | Is_Checked (RapidFire & RAPIDFIRE_J2B1), RapidFire_Switch_J2B1, NULL);
    sprintf           (GenericBuffer, Msg_Get (MSG_Menu_Inputs_RapidFire_PxBx), 2, 2);
    menu_add_item     (menus_ID.rapidfire, GenericBuffer, AM_Active | Is_Checked (RapidFire & RAPIDFIRE_J2B2), RapidFire_Switch_J2B2, NULL);
    // INPUTS (misc)
    menu_add_item     (menus_ID.inputs,    Msg_Get (MSG_Menu_Inputs_Configuration), AM_Active | Is_Checked (Inputs_CFG.active), Inputs_CFG_Switch, NULL);

    //-------------------------------------------------------------------------
    // TOOLS
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_Messages),      AM_Active | Is_Checked (TB_Message.active),                  TB_Message_Switch,                  NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_Palette),       AM_Active | Is_Checked (PaletteViewer.active),               PaletteViewer_Switch,               NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_TilesViewer),   AM_Active | Is_Checked (TileViewer.active),                  TileViewer_Switch,                  NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_TilemapViewer), AM_Active | Is_Checked (TilemapViewer_MainInstance->active), TilemapViewer_SwitchMainInstance,   NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_MemoryEditor),  AM_Active | Is_Checked (MemoryViewer_MainInstance->active),  MemoryViewer_SwitchMainInstance,    NULL);
    menu_add_item     (menus_ID.tools,     Msg_Get (MSG_Menu_Tools_TechInfo),      AM_Active | Is_Checked (TechInfo.active),                    TechInfo_Switch,                    NULL);

    //-------------------------------------------------------------------------
    // HELP
    //-------------------------------------------------------------------------
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Documentation),  AM_Active | Is_Checked (TextViewer.active), TextViewer_Switch_Doc_Main, NULL);
    #ifdef WIN32
        menu_add_item (menus_ID.help,      Msg_Get (MSG_Menu_Help_Documentation_W),AM_Active, TextViewer_Switch_Doc_MainW, NULL);
    #endif
    #ifdef UNIX
        menu_add_item (menus_ID.help,      Msg_Get (MSG_Menu_Help_Documentation_U),AM_Active, TextViewer_Switch_Doc_MainU, NULL);
    #endif
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Compat),         AM_Active, TextViewer_Switch_Doc_Compat, NULL);
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Multiplayer_Games),AM_Active, TextViewer_Switch_Doc_Multiplayer_Games, NULL);
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Changes),        AM_Active, TextViewer_Switch_Doc_Changes, NULL);
    menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_Debugger),       AM_Active, TextViewer_Switch_Doc_Debugger, NULL);
	menu_add_item     (menus_ID.help,      Msg_Get (MSG_Menu_Help_About),          AM_Active | Is_Checked (AboutBox.active), AboutBox_Switch, NULL);

    // ...
    gui_menu_un_mouse_over (menus_ID.menu);
    gui_menus_update_size ();
}

//-----------------------------------------------------------------------------

