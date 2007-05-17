//-----------------------------------------------------------------------------
// MEKA 0.72 (c) Omar Cornut (Bock) & MEKA team 1998-2007
// Sega Master System / Game Gear / SG-1000 / SC-3000 / SF-7000 / ColecoVision / Famicom emulator
// Sound engine by Hiromitsu Shioya (Hiroshi) in 1998-1999
// Z80 CPU core by Marat Faizullin, 1994-1998
//-----------------------------------------------------------------------------
// MEKA - meka.c
// Entry points and various initialization functions
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_options.h"
#include "app_tileview.h"
#include "bios.h"
#include "blit.h"
#include "blitintf.h"
#include "capture.h"
#include "config.h"
#include "db.h"
#include "debugger.h"
#include "desktop.h"
#include "effects.h"
#include "fdc765.h"
#include "file.h"
#include "fskipper.h"
#include "games.h"
#include "g_file.h"
#include "glasses.h"
#include "inputs_f.h"
#include "inputs_i.h"
#include "mappers.h"
#include "nes.h"
#include "palette.h"
#include "patch.h"
#include "setup.h"
#include "video.h"
#include "video_m5.h"
#include "vlfn.h"
#include "osd/timer.h"
#include "libaddon/png/loadpng.h"
#ifdef WIN32
#include <commctrl.h>
#endif

//-----------------------------------------------------------------------------
// Redefine Allegro list to space executable size
// Note: I'm not even sure it works as expected
//-----------------------------------------------------------------------------

#ifndef MACOSX
BEGIN_COLOR_DEPTH_LIST
   COLOR_DEPTH_8
   COLOR_DEPTH_15
   COLOR_DEPTH_16
   COLOR_DEPTH_24
   COLOR_DEPTH_32
END_COLOR_DEPTH_LIST

BEGIN_DIGI_DRIVER_LIST
END_DIGI_DRIVER_LIST

BEGIN_MIDI_DRIVER_LIST
END_MIDI_DRIVER_LIST
#endif // MACOSX

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Init_Emulator (void)
//-----------------------------------------------------------------------------
// FIXME: this function is pretty old and is basically a left-over or
// everything that was not moved elsewhere.
//-----------------------------------------------------------------------------
void    Init_Emulator (void)
{
    Video_Init ();

    memset(RAM, 0, 0x10000);        // RAM: 64 Kb (max=SF-7000)
    memset(SRAM, 0, 0x8000);        // SRAM: 32 Kb (max)
    memset(VRAM, 0, 0x4000);        // VRAM: 16 Kb
    PRAM = PRAM_Static;
    memset(PRAM, 0, 0x0040);        // PRAM: 64 bytes
    ROM = Game_ROM_Computed_Page_0 = Memory_Alloc (0x4000); // 16 kbytes (one page)
    memset(Game_ROM_Computed_Page_0, 0, 0x4000);
    Game_ROM = NULL;

    tsms.Pages_Count_8k = 1;
    tsms.Pages_Count_16k = 1;

    RdZ80 = RdZ80_NoHook = Read_Default;
    drv_set (cur_machine.driver_id);
}

// INITALIZE PRE-CALCULATED TABLES --------------------------------------------
void    Init_Tables (void)
{
    Coleco_Init_Table_Inputs ();
    #ifdef X86_ASM
        Decode_Tile_ASM_Init ();
    #endif
}

// INITIALIZING DEFAULT VARIABLES VALUES --------------------------------------
void    Init_Default_Values (void)
{
    Debug_Print_Infos = FALSE;

    // IPeriod
    opt.IPeriod = opt.Cur_IPeriod = 228;
    opt.IPeriod_Coleco = 228; // 215
    opt.IPeriod_Sg1000_Sc3000 = 228;
    opt.IPeriod_NES = 114;

    opt.Layer_Mask = LAYER_BACKGROUND | LAYER_SPRITES;

    opt.GUI_Inited = FALSE;
    opt.Current_Key_Pressed = 0;
    opt.State_Current = 0;
    opt.State_Load = -1;
    opt.Setup_Interactive_Execute = FALSE;
    opt.Force_Quit = FALSE;

    // Machine
    cur_machine.driver_id = DRV_SMS;

    // Country
    g_Configuration.country                       = COUNTRY_EXPORT;
    g_Configuration.country_cfg                   = COUNTRY_EXPORT;
    g_Configuration.country_cl                    = COUNTRY_AUTO;

    // Debug Mode
    g_Configuration.debug_mode                    = FALSE;
    g_Configuration.debug_mode_cfg                = FALSE;
    g_Configuration.debug_mode_cl                 = FALSE;

    // Miscellaneous
    g_Configuration.sprite_flickering             = SPRITE_FLICKERING_AUTO;
    g_Configuration.slash_nirv                    = FALSE;
    g_Configuration.enable_BIOS                   = TRUE;
    g_Configuration.show_product_number           = FALSE;
    g_Configuration.show_fullscreen_messages      = TRUE;
    g_Configuration.enable_NES                    = FALSE;
    g_Configuration.allow_opposite_directions     = FALSE;
    g_Configuration.start_in_gui                  = TRUE;

    // Applet: Game Screen
    g_Configuration.game_screen_scale             = 1;// 2

    // Applet: File Browser
    g_Configuration.fb_close_after_load           = TRUE;
    g_Configuration.fb_uses_DB                    = TRUE;
    g_Configuration.fullscreen_after_load         = FALSE;

    // Applet: Debugger
    g_Configuration.debugger_console_lines        = 22;
    g_Configuration.debugger_disassembly_lines    = 14;
    g_Configuration.debugger_disassembly_display_labels = TRUE;
    g_Configuration.debugger_log_enabled          = TRUE;

    // Applet: Memory Editor
    g_Configuration.memory_editor_lines           = 16;
    g_Configuration.memory_editor_columns         = 16;

    // Video
    g_Configuration.video_mode_depth_desktop		= 0;    // Unknown yet
	g_Configuration.video_mode_game_depth			= 16;   // 16-bits
	g_Configuration.video_mode_game_depth_cfg		= 16;   // 16-bits
	g_Configuration.video_mode_game_vsync			= FALSE;
	g_Configuration.video_mode_game_triple_buffering= TRUE;
	g_Configuration.video_mode_game_page_flipping	= FALSE;
    g_Configuration.video_mode_gui_depth			= 0;    // Auto
    g_Configuration.video_mode_gui_depth_cfg		= 0;    // Auto
    g_Configuration.video_mode_gui_res_x			= 640;
    g_Configuration.video_mode_gui_res_y			= 480;
    g_Configuration.video_mode_gui_driver			= GFX_AUTODETECT_FULLSCREEN;
    g_Configuration.video_mode_gui_refresh_rate		= 0;    // Auto
    g_Configuration.video_mode_gui_access_mode		= GUI_FB_ACCESS_BUFFERED;
    g_Configuration.video_mode_gui_vsync			= FALSE;

	// Capture
#ifdef DOS
	g_Configuration.capture_filename_template	  = "%.5s-%02d.png"; // Short Filename
#else
	g_Configuration.capture_filename_template	  = "%s-%02d.png";   // Long Filename (ala SMS Power)
#endif
	g_Configuration.capture_automatic_crop_align  = FALSE;

    // Media
    // FIXME: yet not fully used
    media_ROM.type        = MEDIA_IMAGE_ROM;
    media_ROM.data        = NULL;
    media_ROM.data_size   = 0;
    media_ROM.mekacrc.v[0]= 0;
    media_ROM.mekacrc.v[1]= 0;
    media_ROM.crc32       = 0;

    Machine_Pause_Need_To = FALSE;

    Blitters_Init_Values ();
    Frame_Skipper_Init_Values ();

    strcpy (FB.current_directory, ".");
    FB_Init_Values ();

    TB_Message_Init_Values ();
    Sound_Init_Config ();
    TVType_Init_Values ();
    Glasses_Init_Values ();
    TileViewer_Init_Values ();
    Skins_Init_Values();

    #ifdef MEKA_Z80_DEBUGGER
        Debugger_Init_Values ();
    #endif
}

// FREE ALLOCATED MEMORY ------------------------------------------------------
void    Free_Memory (void)
{
    free (Game_ROM_Computed_Page_0);
    BIOS_Free_Roms ();
    if (Game_ROM)
    {
        free (Game_ROM);
        Game_ROM = NULL;
    }
}

// CLOSE EMULATOR -------------------------------------------------------------
void    Close_Emulator (void)
{
    Sound_Close          ();
    Desktop_Close        ();
    Fonts_Close          ();
    FDC765_Close         ();
    Palette_Close        ();
    Inputs_Sources_Close ();
    Inputs_Joystick_Close();
    gui_close            ();
    Free_Memory          ();
    FB_Free_Memory       ();
    DB_Close             ();
    Blitters_Close       ();
    Glasses_Close        ();
    Data_Close           ();
}

// Remove Allegro installed callback
// This is the first function to call on shutdown, to avoid getting called
// during the shutdown process (which sometimes makes things crash).
void    Close_Callback (void)
{
    #ifndef DOS
        set_close_button_callback (NULL);
    #endif
}

// Change to starting directory
// This function is registered in the atexit() table to be called on quit
void    Close_Emulator_Starting_Dir (void)
{
    chdir (g_Env.Paths.StartingDirectory);
}

// INITIALIZE ALLEGRO ---------------------------------------------------------
int     Init_Allegro (void)
{
    ConsolePrint (Msg_Get (MSG_Init_Allegro));

    // Initialize timer BEFORE allegro
    // OSD_Timer_Initialize ();

    set_uformat(U_ASCII);
    allegro_init();

    g_Configuration.video_mode_depth_desktop = desktop_color_depth();
    if (g_Configuration.video_mode_depth_desktop == 0)
        g_Configuration.video_mode_depth_desktop = 16;	// Default

    install_timer();

    // Keyboard
    install_keyboard();

    // Mouse
    //static char cmd[] = "emulate_three = 0\n";
    //override_config_data(cmd, sizeof (cmd));
    #ifdef DOS
        //#ifdef MOUSEDRV_POLLING
        //{
        //extern int _mouse_type;
        //if (os_type == OSTYPE_WINNT)
        //    _mouse_type = MOUSEDRV_WINNT;
        //}   
        //#endif
    #endif
    g_Env.mouse_installed = install_mouse ();

    // PNG support
    #ifdef MEKA_PNG
        loadpng_init ();
    #endif

    // Window title & callback
    #ifndef DOS
        set_window_title (Msg_Get (MSG_Window_Title));
        set_close_button_callback (Close_Button_Callback);
    #endif

    // text_mode (-1); // now obsolete
    ConsolePrint ("\n");

    return (1);
}

// INITIALIZE GRAPHICAL USER INTERFACE ----------------------------------------
void    Init_GUI (void)
{
    ConsolePrintf ("%s\n", Msg_Get (MSG_Init_GUI));
    gui_init(g_Configuration.video_mode_gui_res_x, g_Configuration.video_mode_gui_res_y, g_Configuration.video_mode_gui_depth);
}

// MAIN FUNCTION --------------------------------------------------------------
//#ifdef WIN32
// int WinMain
//#else
 int main
//#endif
 (int argc, char **argv)
{
    int i;

    #ifdef DOS
        clrscr();
    #endif

    #ifdef WIN32
        // Need for XP manifest stuff
        InitCommonControls();
    #endif

    ConsoleInit (); // First thing to do
    #ifdef WIN32
        ConsolePrintf ("%s\n(c) %s %s\n--\n", MEKA_NAME_VERSION, MEKA_DATE, MEKA_AUTHORS);
    #else
        ConsolePrintf ("\n%s (c) %s %s\n--\n", MEKA_NAME_VERSION, MEKA_DATE, MEKA_AUTHORS);
    #endif

    // Wait for Win32 console signal
    if (!ConsoleWaitForAnswer(TRUE))
        return (0);

    // Save command line parameters
    g_Env.argc = argc;
    g_Env.argv = malloc (sizeof (char *) * (g_Env.argc + 1));
    for (i = 0; i != g_Env.argc; i++)
    {
        g_Env.argv[i] = strdup(argv [i]);
        //#ifndef UNIX
        //  strupr(g_Env.argv[i]);
        //#endif
    }
    g_Env.argv[i] = NULL;

    // FIXME: add 'init system' here

    // Initializations
    Meka_State = MEKA_STATE_INIT;
    Filenames_Init          (); // Set Filenames Values
    Messages_Init           (); // Load MEKA.MSG and init messaging system
    //Register_Init         (); // Check Registered User Key
    Init_Default_Values     (); // Set Defaults Variables
    Command_Line_Parse      (); // Parse Command Line (1)
    Init_Allegro            (); // Initialize Allegro Library
    Init_Games              (); // Initialize Hidden Games
    Capture_Init            (); // Initialize Screen capture
    Configuration_Load      (); // Load Configuration File
    atexit (Close_Emulator_Starting_Dir);
    Setup_Interactive_Init  (); // Show Interactive Setup if asked to
    Configuration_Load_PostProcess  ();
    Frame_Skipper_Init      (); // Initialize Auto Frame Skipper
    Country_Init            (); // Initialize Country
    DB_Init                 (); // Initialize and load DataBase file
    Patches_List_Init       (); // Load Patches List
    VLFN_Init               (); // Load Virtual Long Filename List
    Skins_Init              (); // Load Skin List
    Blitters_Init           (); // Load Blitter List
    Inputs_Init             (); // Initialize Inputs and load inputs sources list
    Blit_Init               (); // Initialize Blitter
    Random_Init             (); // Initialize Pifometer (Random Number Generator)
    Fonts_Init              (); // Initialize Fonts system
	Effects_TV_Init			();	// Initialize TV snow effect
    FDC765_Init             (); // Initialize Floppy Disk emulation
    Data_Init               (); // Load datafile
    Init_Emulator           (); // Initialize Emulation
    Palette_Init            (); // Initialize Palette system
    NES_Init                (); // Initialize NES emulation
    Init_Tables             (); // Initialize Tables
    Machine_Init            (); // Initialize Virtual Machine
    Init_GUI                (); // Initialize Graphical User Interface
    Sound_Init              (); // Initialize Sound
    Inputs_Joystick_Init    (); // Initialize Joysticks. 
    Machine_Reset           (); // Reset Emulated Machine (set default values)

    // Initialization complete
    ConsolePrintf ("%s\n--\n", Msg_Get (MSG_Init_Completed));

    // Load ROM from command line if necessary
    Load_ROM_Command_Line   ();

    // Wait for Win32 console signal
    if (!ConsoleWaitForAnswer(TRUE))
        return (0);
    ConsoleClose            (); // Close Console

    FB_Init_2               (); // Finish initializing the file browser

    // Setup initial state (fullscreen/GUI)
    if ((machine & MACHINE_RUN) == MACHINE_RUN && !g_Configuration.start_in_gui)
        Meka_State = MEKA_STATE_FULLSCREEN;
    else
        Meka_State = MEKA_STATE_GUI;
    Video_Setup_State ();

    // Start main program loop
    // Everything runs from there.
    // Z80_Opcodes_Usage_Reset ();
    Main_Loop               (); 
    // Z80_Opcodes_Usage_Print ();

    // Shutting down emulator...
    Meka_State = MEKA_STATE_SHUTDOWN;
    Video_Setup_State       (); // Switch back to text mode
    BMemory_Save            (); // Write Backed Memory if necessary
    Configuration_Save      (); // Write Configuration File
    Write_Inputs_Src_List   (); // Write Inputs Definition File
    VLFN_Close              (); // Write Virtual Long Filename List
    Close_Emulator          (); // Close Emulator
    Show_End_Message        (); // Show End Message

    return (0);
}
#ifndef WIN32
END_OF_MAIN ();
#else
// Allegro define END_OF_MAIN() the same as the above, with HINSTANCE
// types replaced by void *, which cause two warnings in /W3 mode.
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrev, char *Cmd, int nShow)  
{                                                                      
    return _WinMain((void *)_mangled_main, hInst, hPrev, Cmd, nShow);   
}
#endif

//-----------------------------------------------------------------------------

