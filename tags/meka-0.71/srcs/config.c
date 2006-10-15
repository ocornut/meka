//-----------------------------------------------------------------------------
// MEKA - config.h
// Configuration File Load/Save - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_tileview.h"
#include "blitintf.h"
#include "capture.h"
#include "config.h"
#include "config_v.h"
#include "debugger.h"
#include "fskipper.h"
#include "g_file.h"

//-----------------------------------------------------------------------------

static FILE *       CFG_File;
static INLINE void  CFG_Write_Line (char *str)              { fprintf (CFG_File, "%s\n", str); }
static INLINE void  CFG_Write_Int (char *name, int value)   { fprintf (CFG_File, "%s = %d\n", name, value); }
static INLINE void  CFG_Write_Str (char *name, char *str)   { fprintf (CFG_File, "%s = %s\n", name, str); }

//-----------------------------------------------------------------------------
// Configuration_Load_Line (char *variable, char *value)
// Handle a variable assignment during configuration file loading
//-----------------------------------------------------------------------------
static void     Configuration_Load_Line (char *variable, char *value)
{
 int            var_num;
 int            n;
 char *         p;
 char           tvalue [256];

 static char  *Config_File_Variables [] =
     {
     "frameskip_mode", "frameskip_auto_speed", "frameskip_normal_speed",
     "blitter",

     "sound_card", "sound_enabled", "sound_rate",
     "fm_emulator", "opl_speed",

     "gui_video_mode", "gui_video_driver", "gui_access_mode", "gui_vsync",

     "start_in_gui", "color_theme",
     "fb_width", "fb_height",
     "fb_uses_db", "fb_close_after_load", "fb_fullscreen_after_load",
     "last_directory",

     "bios_logo",
     "rapidfire",
     "country",
     "tv_type",
     "tv_snow_effect",
     "palette",
     "show_product_number",
     "show_messages_fullscreen",
     "screenshot_template",

     "3dglasses_mode", "3dglasses_com_port",

     "iperiod", "iperiod_coleco", "iperiod_sg1000_sc3000", "magic",

     "nes_sucks",
     "mario_is_a_fat_plumber",

     "sprite_flickering",

     "video_depth",
     "language",
     "screenshots_filename_template",

     "music_wav_filename_template", "musics_wav_filename_template",
     "music_vgm_filename_template", "musics_vgm_filename_template",
     "music_vgm_log_accuracy",      "musics_vgm_log_accuracy",

     "fm_enabled",

     "gui_refresh_rate",

     "tile_viewer_displayed_tiles",

     "debug_mode",
     "allow_opposite_directions",

     "debugger_console_lines",
     "debugger_disassembly_lines",
     "debugger_disassembly_display_labels",
     "debugger_log",

     NULL
     };

 strlwr (variable);
 for (var_num = 0; Config_File_Variables [var_num]; var_num++)
     if (!strcmp (variable, Config_File_Variables [var_num]))
        break;

 if (!Config_File_Variables[var_num])
   return;

 if (strcmp (Config_File_Variables[var_num], "last_directory") != 0)
   strlwr (value);

 // FIXME: parameter setting isn't done well
 // We should set limits values, and if the new value isn't in bound, the
 // variable is not modified, etc..
 switch (var_num)
    {
    //-------------------------------------------------------------------------
    // frameskip_mode
    case 0:  if (strcmp (value, "normal") == 0 || strcmp (value, "standard") == 0)
                fskipper.Mode = FRAMESKIP_MODE_STANDARD;
             else
                fskipper.Mode = FRAMESKIP_MODE_AUTO;
             break;
    // frameskip_speed
    case 1:  fskipper.Automatic_Speed = atoi (value); break;
    // frameskip_value
    case 2:  fskipper.Standard_Frameskip = atoi (value); break;
    // blitter
    case 3:  blitters.current_num = atoi (value); break;
    //-------------------------------------------------------------------------
    // sound_card
    case 4:  Sound.SoundCard = atoi (value);
             break;
    // sound_enabled
    case 5:  Sound.Enabled = (bool)atoi(value);
             break;
    // sound_rate
    case 6:  n = atoi (value);
             if (n > 0)
                Sound.SampleRate = atoi (value);
             break;
    // fm_emulator
    case 7:  if (!strcmp (value, "none"))
                Sound.FM_Emulator_Current = FM_EMULATOR_NONE;
             else
             if (!strcmp (value, "opl"))
                Sound.FM_Emulator_Current = FM_EMULATOR_YM2413HD;
             else
             if (!strcmp (value, "digital"))
                Sound.FM_Emulator_Current = FM_EMULATOR_EMU2413;
             break;
    #ifdef MEKA_OPL
    // opl_speed
    case 8:  n = atoi (value);
             if (n < 0)
                 n = 0;
             Sound.OPL_Speed = n;
             break;
    #endif
    //-------------------------------------------------------------------------
    // gui_video_mode
    case 9:  if (!(p = strchr (value, 'x')))
                break;
             strcpy (tvalue, p + 1);
             cfg.GUI_Res_Y = atoi (tvalue);
             strncpy (tvalue, value, p - value);
             tvalue [p - value] = 0;
             cfg.GUI_Res_X = atoi (tvalue);
             break;
    // gui_video_driver
    case 10: 
        cfg.GUI_Driver = VideoDriver_FindByDesc (value)->drv_id; 
        break;
    // gui_access_mode
    case 11: if (!strcmp (value, "direct"))
                { cfg.GUI_Access_Mode = GUI_FB_ACCESS_DIRECT; }
             else
             if (!strcmp(value, "flipped"))
                { cfg.GUI_Access_Mode = GUI_FB_ACCESS_FLIPPED; }
             else
                { cfg.GUI_Access_Mode = GUI_FB_ACCESS_BUFFERED; }
             break;
    // gui_vsync
    case 12: cfg.GUI_VSync = (bool)atoi(value);
             break;
    //-------------------------------------------------------------------------
    // start_in_gui
    case 13: cfg.GUI_Start_In = (bool)atoi(value);
             break;
    // color theme
    case 14: Themes.current = atoi (value);
             break;
    // fb_width
    case 15: FB.res_x = atoi (value);
             break;
    // fb_height
    case 16: FB.file_y = atoi (value);
             break;
    // fb_uses_db
    case 17: Configuration.fb_uses_DB = (bool)atoi(value);
             break;
    // fb_close_after_load
    case 18: Configuration.fb_close_after_load = (bool)atoi(value);
             break;
    // fb_fullscreen_after_load
    case 19: Configuration.fullscreen_after_load = (bool)atoi(value);
             break;
    // last_directory
    case 20: StrReplace (value, '*', ' ');
             strncpy (FB.current_directory, value, FILENAME_LEN);
             break;
    //-------------------------------------------------------------------------
    // bios_logo
    case 21: Configuration.enable_BIOS = (bool)atoi(value);
             break;
    // rapidfire
    case 22: RapidFire = atoi (value);
             break;
    // country
    case 23: if (strcmp (value, "jap") == 0)
                 Configuration.country_cfg = COUNTRY_JAP;
             else
                 Configuration.country_cfg = COUNTRY_EUR_US;
             break;
    // tv_type
    case 24: if (strcmp (value, "ntsc") == 0)
                TV_Type_User = &TV_Type_Table[TVTYPE_NTSC];
             else
             if (strcmp (value, "pal") == 0 || strcmp (value, "secam") || strcmp (value, "pal/secam"))
                TV_Type_User = &TV_Type_Table[TVTYPE_PAL_SECAM];
             TVType_Update_Values ();
             break;
    // tv_snow_effect
    case 25: effects.TV_Enabled = (bool)atoi(value);
             break;
    // palette
    case 26: if (strcmp (value, "muted") == 0)
                Configuration.palette_type = PALETTE_TYPE_MUTED;
             else
             if (strcmp (value, "bright") == 0)
                Configuration.palette_type = PALETTE_TYPE_BRIGHT;
             break;
    // show_product_number
    case 27: Configuration.show_product_number = (bool)atoi(value);
             break;
    // show_messages_fullscreen
    case 28: Configuration.show_fullscreen_messages = (bool)atoi(value);
             break;
    // screenshot_template (OBSOLETE variable name, see below)
    case 29: StrReplace (value, '*', ' ');
             Capture.filename_template = strdup (value);
             break;
    //-------------------------------------------------------------------------
    // 3dglasses_mode
    case 30: Glasses_Set_Mode (atoi (value));
             break;
    // 3dglasses_com_port
    case 31: Glasses_Set_ComPort (atoi (value));
             break;
    //-------------------------------------------------------------------------
    // iperiod
    case 32: opt.IPeriod = atoi (value);
             break;
    // iperiod_coleco
    case 33: opt.IPeriod_Coleco = atoi (value);
             break;
    // iperiod_sg1000_sc3000
    case 34: opt.IPeriod_Sg1000_Sc3000 = atoi (value);
             break;
    // magic at C000h
    case 35: opt.Magic_C000 = GetNbr (value);
             break;
    //-------------------------------------------------------------------------
    // nes_sucks
    case 36: if (GetNbr (value) < 1)
                {
                Quit_Msg ("\n%s", Msg_Get (MSG_NES_Sucks));
                }
             break;
    // mario_is_a_fat_plumber
    case 37: Configuration.enable_NES = (bool)atoi (value);
             break;
    //-------------------------------------------------------------------------
    // sprite_flickering
    case 38: if (strcmp (value, "auto") == 0)
                 Configuration.sprite_flickering = SPRITE_FLICKERING_AUTO;
             else
             if (strcmp (value, "yes") == 0)
                 Configuration.sprite_flickering = SPRITE_FLICKERING_ENABLED;
             else
             if (strcmp (value, "no") == 0)
                 Configuration.sprite_flickering = SPRITE_FLICKERING_NO;
             break;
    // video_depth
    case 39: cfg.Video_Depth = GetNbr (value);
             break;
    // language
    case 40: StrReplace (value, '*', ' ');
             Lang_Set_by_Name (value);
             break;
    // screenshots_filename_template
    case 41: StrReplace (value, '*', ' ');
             Capture.filename_template = strdup (value);
             break;
    // music[s]_wav_filename_template
    case 42:
    case 43: StrReplace (value, '*', ' ');
             Sound.LogWav_FileName_Template = strdup (value);
             break;
    // music[s]_vgm_filename_template
    case 44:
    case 45: StrReplace (value, '*', ' ');
             Sound.LogVGM_FileName_Template = strdup (value);
             break;
    // music[s]_vgm_log_accuracy
    case 46:
    case 47: if (strcmp (value, "frame") == 0)
                Sound.LogVGM_Logging_Accuracy = VGM_LOGGING_ACCURACY_FRAME;
             else
             if (strcmp (value, "sample") == 0)
                Sound.LogVGM_Logging_Accuracy = VGM_LOGGING_ACCURACY_SAMPLE;
             break;
    // fm_enabled
    case 48: if (!strcmp (value, "yes"))
                Sound.FM_Enabled = YES;
             else
             if (!strcmp (value, "no"))
                Sound.FM_Enabled = NO;
             break;
    // gui_refresh_rate
    case 49: if (!strcmp (value, "auto"))
                cfg.GUI_Refresh_Rate = 0;
             else
                cfg.GUI_Refresh_Rate = atoi (value);
             break;
    // tile_viewer_displayed_tiles
    case 50: n = atoi(value);
             if (n == 448 || n == 512)
                 TileViewer.tiles_count = n;
             break;
    // debug_mode
    case 51: Configuration.debug_mode_cfg = (bool)atoi(value);
             break;

    // allow_opposite_directions
    case 52: Configuration.allow_opposite_directions = (bool)atoi(value);
             break;

    // debugger_console_lines
    case 53:
        n = atoi(value);
        if (n >= 1)
            Configuration.debugger_console_lines = n;
        break;

    // debugger_disassembly_lines
    case 54:
        n = atoi(value);
        if (n >= 1)
            Configuration.debugger_disassembly_lines = n;
        break;

    // debugger_disassembly_display_labels
    case 55:
        Configuration.debugger_disassembly_display_labels = (bool)atoi(value);
        break;

    // debugger_log
    case 56:
        Configuration.debugger_log_enabled = (bool)atoi(value);
        break;

    default:
        Quit_Msg("Error #4785");
        break;
    }
}

//-----------------------------------------------------------------------------
// Configuration_Load ()
// Load configuration file data from MEKA.CFG
//-----------------------------------------------------------------------------
void        Configuration_Load (void)
{
 char *     p;
 int        i, j;
 char       line2 [256];
 char       variable [256], value [256];
 t_tfile *  tf;
 t_list *   lines;
 char *     line;
 int        line_cnt;

 StrCpyPathRemoved(line2, Env.Paths.ConfigurationFile);
 strupr (line2);
 ConsolePrintf (Msg_Get (MSG_Config_Loading), line2);

 // Open and read file --------------------------------------------------------
 if ((tf = tfile_read (Env.Paths.ConfigurationFile)) == NULL)
    {
    ConsolePrintf ("%s\n", meka_strerror());
    return;
    }

 // Ok
 ConsolePrint ("\n");

 // Parse each line -----------------------------------------------------------
 line_cnt = 0;
 for (lines = tf->data_lines; lines; lines = lines->next)
    {
    line_cnt += 1;
    line = lines->elem;

    if (StrNull (line))
       {
       continue;
       }
    /* strlwr (line); */
    for (i = 0, j = 0; line [i] != 0; i ++)
        {
        if (line [i] != ' ')
           {
           line2 [j++] = line [i];
           }
        }
    line2 [j] = 0;
    p = strchr (line2, '=');
    if (p == NULL)
       {
       /*
       if (StrSChr(line2, "Meka") && StrSChr(line2, "-ConfigurationFile"))
          {
          cfg.CFG_File_Version_High = atoi(line2 + 4);
          p = strchr(line2 + 4, '.');
          if (
          cfg.CFG_File_Version_Low = atoi(strchr(line2 + 4, '.') + 1);
          // FIXME: poetntial crash above
          }
       */
       continue;
       }
    strcpy (value, p + 1);
    strncpy (variable, line2, p - line2);
    variable [p - line2] = 0;
    Configuration_Load_Line (variable, value);
    }

 // Free file data ------------------------------------------------------------
 tfile_free (tf);
}

//-----------------------------------------------------------------------------
// Configuration_Load_PostProcess ()
// Various post processing right after loading the configuration file
//-----------------------------------------------------------------------------
void    Configuration_Load_PostProcess (void)
{
    Configuration.debug_mode = (Configuration.debug_mode_cfg || Configuration.debug_mode_cl);
}

//-----------------------------------------------------------------------------
// Configuration_Save ()
// Save configuration file data to MEKA.CFG
//-----------------------------------------------------------------------------
void    Configuration_Save (void)
{
    char   s1 [256];

    if (!(CFG_File = fopen (Env.Paths.ConfigurationFile, "wt")))
        return;

    CFG_Write_Line (";");
    CFG_Write_Line ("; MEKA " VERSION " - Configuration File");
    CFG_Write_Line (";");
    CFG_Write_Line ("");

    CFG_Write_Line ( "-----< VIDEO DRIVERS >------------------------------------------------------");
    CFG_Write_Line ( "This is a list of video/graphic card drivers available in this version");
    CFG_Write_Line ( "of Meka. Those are needed to create video modes in the MEKA.BLT file and");
    CFG_Write_Line ( "if you want to set a custom driver for the graphical user interface.");
    VideoDriver_DumpAllDesc (CFG_File);
    // CFG_Write_Int  ("video_depth", cfg.Video_Depth);
    CFG_Write_Line ("");

    CFG_Write_Line ( "-----< FRAMESKIP and BLITTER >----------------------------------------------");
    if (fskipper.Mode == FRAMESKIP_MODE_AUTO)
        CFG_Write_Line  ("frameskip_mode = auto");
    else
        CFG_Write_Line  ("frameskip_mode = normal");
    CFG_Write_Int  ("frameskip_auto_speed", fskipper.Automatic_Speed);
    CFG_Write_Int  ("frameskip_normal_speed", fskipper.Standard_Frameskip);
    CFG_Write_Int  ("blitter", blitters.current_num);
    CFG_Write_Line ("(See MEKA.BLT file to configure blitters/fullscreen modes)");
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< INPUTS >--------------------------------------------------------------");
    CFG_Write_Line ("(See MEKA.INP file to configure inputs sources)");
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< SOUND AND MUSIC >-----------------------------------------------------");
    CFG_Write_Int  ("sound_enabled", Sound.Enabled);
    CFG_Write_Int  ("sound_card", Sound.SoundCard);
    CFG_Write_Int  ("sound_rate", Sound.SampleRate);
    CFG_Write_Line ("(Set sound_card to -1 to be prompted to choose your soundcard again)");
    CFG_Write_Str  ("fm_enabled", (Sound.FM_Enabled) ? "yes" : "no");
    switch (Sound.FM_Emulator_Current)
    {
    case FM_EMULATOR_NONE:       CFG_Write_Str  ("fm_emulator", "none");        break;
    case FM_EMULATOR_YM2413HD:   CFG_Write_Str  ("fm_emulator", "opl");         break;
    case FM_EMULATOR_EMU2413:    CFG_Write_Str  ("fm_emulator", "digital");     break;
    }
    CFG_Write_Line ("(Available settings are 'none', 'opl' and 'digital'.");
    CFG_Write_Line (" OPL is only available under DOS and Windows 95/98/ME if your soundcard");
    CFG_Write_Line (" has an OPL chip. Digital emulator is slower but works everywhere.)");
#ifdef MEKA_OPL
    CFG_Write_Int  ("opl_speed", Sound.OPL_Speed);
    CFG_Write_Line ("(Increase opl_speed when using opl if you feel that FM music are partly skipped)");
    CFG_Write_Line ("(Decrease opl_speed when using opl if you feel that FM music make things slow)");
#endif
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< GRAPHICAL USER INTERFACE VIDEO MODE >---------------------------------");
    CFG_Write_Line ("(See MEKA.BLT file to configure blitters/fullscreen modes)");
    sprintf        (s1, "%dx%d", cfg.GUI_Res_X, cfg.GUI_Res_Y);
    CFG_Write_Str  ("gui_video_mode", s1);
    CFG_Write_Str  ("gui_video_driver", VideoDriver_FindByDriverId(cfg.GUI_Driver)->desc);
    CFG_Write_Line ("(Available video drivers are marked at the top of this file.");
    CFG_Write_Line (" Please note that 'auto' does not always choose the faster mode!)");
    if (cfg.GUI_Refresh_Rate == 0)
        CFG_Write_Str ("gui_refresh_rate", "auto");
    else
        CFG_Write_Int ("gui_refresh_rate", cfg.GUI_Refresh_Rate);
    CFG_Write_Line ("(Video mode refresh rate. Set 'auto' for default rate. Not all");
    CFG_Write_Line (" drivers support non-default rate. Customized values then depends");
    CFG_Write_Line (" on your video card and screen. Setting to 60 (Hz) is usually a");
    CFG_Write_Line (" good thing as the screen will be refreshed at the same time as");
    CFG_Write_Line (" the emulated systems.)");
    switch (cfg.GUI_Access_Mode)
    {
    case GUI_FB_ACCESS_DIRECT:  CFG_Write_Str  ("gui_access_mode", "direct");   break;
    case GUI_FB_ACCESS_FLIPPED: CFG_Write_Str  ("gui_access_mode", "flipped");  break;
    default:                    CFG_Write_Str  ("gui_access_mode", "buffered"); break;
    }

    CFG_Write_Line ("(Available access modes are: 'buffered', 'flipped' and 'direct'. Buffered");
    CFG_Write_Line (" access is the slowest, but the others will make the screen flicker)");
    CFG_Write_Int  ("gui_vsync", cfg.GUI_VSync);
    CFG_Write_Line ("(enable vertical synchronisation for fast computers)");
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< GRAPHICAL USER INTERFACE CONFIGURATION >------------------------------");
    CFG_Write_Int  ("start_in_gui", cfg.GUI_Start_In);
    CFG_Write_Int  ("color_theme", Themes.current);
    CFG_Write_Int  ("fb_width", FB.res_x);
    CFG_Write_Line ("(File browser width, in pixel)");
    CFG_Write_Int  ("fb_height", FB.file_y);
    CFG_Write_Line ("(File browser height, in number of files shown)");
    CFG_Write_Int  ("fb_uses_db", Configuration.fb_uses_DB);
    CFG_Write_Int  ("fb_close_after_load", Configuration.fb_close_after_load);
    CFG_Write_Int  ("fb_fullscreen_after_load", Configuration.fullscreen_after_load);
    // FIXME: crap
    StrReplace     (FB.current_directory, ' ', '*');
    CFG_Write_Str  ("last_directory", FB.current_directory);
    StrReplace     (FB.current_directory, '*', ' ');
    CFG_Write_Int  ("tile_viewer_displayed_tiles", TileViewer.tiles_count);
    CFG_Write_Line ("(Number of tiles displayed in tile viewer, 448 or 512. Usually, displaying");
    CFG_Write_Line (" tiles over 448 shows garbage on SMS and GG, so the default is 448)");
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< MISCELLANEOUS OPTIONS >-----------------------------------------------");
    StrReplace     (Messages.Lang_Cur->Name, ' ', '*');
    CFG_Write_Str  ("language", Messages.Lang_Cur->Name);
    StrReplace     (Messages.Lang_Cur->Name, '*', ' ');
    CFG_Write_Int  ("bios_logo", Configuration.enable_BIOS);
    CFG_Write_Line ("(set to '0' to skip the Master System logo when loading a game)");
    CFG_Write_Int  ("rapidfire", RapidFire);
    CFG_Write_Str  ("country", (Configuration.country_cfg == COUNTRY_EUR_US) ? "us/eur" : "jap");
    CFG_Write_Line ("(emulated machine country, either 'us/eur' or 'jap'");
    if (Configuration.sprite_flickering & SPRITE_FLICKERING_AUTO)
        CFG_Write_Line ("sprite_flickering = auto");
    else
        CFG_Write_Str ("sprite_flickering", (Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) ? "yes" : "no");
    CFG_Write_Line ("(hardware sprite flickering emulator, either 'yes', 'no', or 'automatic'");
    CFG_Write_Str  ("tv_type", (TV_Type_User->id == TVTYPE_NTSC) ? "ntsc" : "pal/secam");
    CFG_Write_Line ("(emulated TV type, either 'ntsc' or 'pal/secam'");
    CFG_Write_Int  ("tv_snow_effect", effects.TV_Enabled);
    CFG_Write_Str  ("palette", (Configuration.palette_type == PALETTE_TYPE_MUTED) ? "muted" : "bright");
    CFG_Write_Line ("(palette type, either 'muted' or 'bright'");
    CFG_Write_Int  ("show_product_number", Configuration.show_product_number);
    CFG_Write_Int  ("show_messages_fullscreen", Configuration.show_fullscreen_messages);
    CFG_Write_Int  ("allow_opposite_directions", Configuration.allow_opposite_directions);
    StrReplace     (Capture.filename_template, ' ', '*');
    CFG_Write_Str  ("screenshots_filename_template", Capture.filename_template);
    StrReplace     (Capture.filename_template, '*', ' ');
    StrReplace     (Sound.LogWav_FileName_Template, ' ', '*');
    CFG_Write_Str  ("music_wav_filename_template", Sound.LogWav_FileName_Template);
    StrReplace     (Sound.LogWav_FileName_Template, '*', ' ');
    StrReplace     (Sound.LogVGM_FileName_Template, ' ', '*');
    CFG_Write_Str  ("music_vgm_filename_template", Sound.LogVGM_FileName_Template);
    CFG_Write_Line ("(see documentation for more information about templates)");
    StrReplace     (Sound.LogVGM_FileName_Template, '*', ' ');
    CFG_Write_Str  ("music_vgm_log_accuracy", (Sound.LogVGM_Logging_Accuracy == VGM_LOGGING_ACCURACY_FRAME) ? "frame" : "sample");
    CFG_Write_Line ("(either 'frame' or 'sample')");
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< 3-D GLASSES EMULATION >-----------------------------------------------");
    CFG_Write_Int  ("3dglasses_mode", Glasses.Mode);
    CFG_Write_Line ("('0' = show both sides and become blind)");
    CFG_Write_Line ("('1' = play without 3-D Glasses, show only left side)");
    CFG_Write_Line ("('2' = play without 3-D Glasses, show only right side)");
    CFG_Write_Line ("('3' = uses real 3-D Glasses connected to COM port)");
    // if (Glasses_Mode == GLASSES_MODE_COM_PORT)
    {
        CFG_Write_Int  ("3dglasses_com_port", Glasses.ComPort);
        CFG_Write_Line ("(this is on which COM port the Glasses are connected. Either 1 or 2)");
    }
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< EMULATION OPTIONS >---------------------------------------------------");
    CFG_Write_Int  ("iperiod", opt.IPeriod);
    CFG_Write_Int  ("iperiod_coleco", opt.IPeriod_Coleco);
    CFG_Write_Int  ("iperiod_sg1000_sc3000", opt.IPeriod_Sg1000_Sc3000);
    CFG_Write_Int  ("magic", opt.Magic_C000);
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< DEBUG MODE >----------------------------------------------------------");
    CFG_Write_Int  ("debug_mode", Configuration.debug_mode_cfg);
    CFG_Write_Line ("(set to 1 to permanently enable debug mode. you can also enable");
    CFG_Write_Line (" it for a single session by starting MEKA with the /DEBUG parameter)");
    CFG_Write_Int  ("debugger_console_lines", Configuration.debugger_console_lines);
    CFG_Write_Int  ("debugger_disassembly_lines", Configuration.debugger_disassembly_lines);
    CFG_Write_Int  ("debugger_disassembly_display_labels", Configuration.debugger_disassembly_display_labels);
    CFG_Write_Int  ("debugger_log", Configuration.debugger_log_enabled);
    CFG_Write_Line ("");

    CFG_Write_Line ("-----< FACTS >---------------------------------------------------------------");
    CFG_Write_Line ("nes_sucks = 1");
    if (Configuration.enable_NES)
        CFG_Write_Line ("mario_is_a_fat_plumber = 1");

    fclose (CFG_File);
}

//-----------------------------------------------------------------------------

static void     Param_Check (int *current, char *msg)
{
    if ((*current) + 1 >= params_c)
        Quit_Msg (msg);
    (*current)++;
}

// PARSE COMMAND LINE ---------------------------------------------------------
void    Command_Line_Parse (void)
{
 int    i, j;
 char  *Params[] =
     {
     "EURO", "US", "JAP", "JP", "JPN", "NIRV", "HELP", "?",
     "SOUND", "NOELEPHANT", "DEBUG", "LOG", "LOAD",
     "SETUP",
     "_DEBUG_PALETTE", "_DEBUG_STEP", "_DEBUG_INFOS",
     NULL
     };

 for (i = 1; i < params_c; i ++)
     {
     char *s = params_v [i];
     if (s[0] == '-'
#ifndef UNIX
        || s[0] == '/'
#endif
     )
        {
        for (j = 0; Params[j]; j++)
            if (!stricmp (s + 1, Params[j]))
                break;
        switch (j)
           {
           case 0: case 1: // EURO/US
                   Configuration.country_cl = COUNTRY_EUR_US;
                   break;
           case 2: case 3: case 4: // JAP
                   Configuration.country_cl = COUNTRY_JAP;
                   break;
           case 5: // SLASH_NIRV
                   Configuration.slash_nirv = YES;
                   break;
           case 6: // HELP
           case 7: Command_Line_Help ();
                   break;
           case 8: // SOUND
           case 9: // NOELEPHANT
                   break;
           case 10: // DEBUG
                    #ifndef MEKA_Z80_DEBUGGER
                      Quit_Msg (Msg_Get (MSG_Debug_Not_Available));
                    #else
                      Configuration.debug_mode_cl = YES;
                    #endif
                    break;
           case 11: // LOG
                    Param_Check (&i, Msg_Get (MSG_Log_Need_Param));
                    TB_Message.log_filename = strdup (params_v[i]);
                    break;
           case 12: // LOAD
                    Param_Check (&i, Msg_Get (MSG_Load_Need_Param));
                    opt.State_Load = atoi (params_v[i]);
                    break;
           case 13: // SETUP
                    opt.Setup_Interactive_Execute = YES;
                    break;
           // Private Usage
           case 14: // _DEBUG_PALETTE
                    Palette_Debug = YES;
                    break;
           case 15: // _DEBUG_STEP
                    opt.Debug_Step = 1;
                    break;
           case 16: // _DEBUG_INFOS
                    Debug_Print_Infos = YES;
                    if (TB_Message.log_filename == NULL)
                        TB_Message.log_filename = strdup("debuglog.txt");
                    break;
           default:
                   ConsolePrintf (Msg_Get (MSG_Error_Param), s);
                   ConsolePrint ("--\n");
                   Command_Line_Help ();
                   return;
           }
        }
     else
        {
        // FIXME: specifying more than one ROM ?
        strcpy (file.rom, s);
        //MessageBox(NULL, s, s, 0);
        }
     }
}

void    Command_Line_Help (void)
{
    // Note: this help screen is not localized.
    Quit_Msg(
        #ifdef WIN32
        "Syntax: MEKAW [rom] [options] ...\n"
        #else
        "Syntax: MEKA [rom] [option1] ...\n"
        #endif
        ""                                                                   "\n" \
        "Where [rom] is a valid rom image to load. Available options are:"   "\n" \
        ""                                                                   "\n" \
        "  -HELP -?         : Display command line help"                     "\n" \
        "  -SETUP           : Start with the setup screen"                   "\n" \
        "  -EURO -US        : Emulate an European/US system for this session""\n" \
        "  -JAP -JP -JPN    : Emulate a Japanese system for this session"    "\n" \
        "  -DEBUG           : Enable debugging features"                     "\n" \
        "  -LOAD <n>        : Load savestate <n> on startup"                 "\n" \
        "  -LOG <file>      : Log message to file <file> (appending it)"     "\n" \
        "  -NOELEPHANT      : Just what it says"                             "\n"
        );
    //   "  -NIRV            : Speed up emulation dramatically"          "\n"
}

//-----------------------------------------------------------------------------


