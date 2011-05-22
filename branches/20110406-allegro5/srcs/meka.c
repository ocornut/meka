//-----------------------------------------------------------------------------
// MEKA 0.72 (c) Omar Cornut (Bock) & MEKA team 1998-2008
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
#ifdef ARCH_WIN32
#include <commctrl.h>
#endif
#include <allegro5/allegro_image.h>

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

extern "C" // C-style mangling for ASM linkage
{
OPT_TYPE				opt;
TGFX_TYPE				tgfx;
SMS_TYPE				sms;
TSMS_TYPE				tsms;
}

t_machine				cur_machine;
t_meka_env				g_env;
t_media_image			media_ROM;
t_meka_configuration	g_Configuration;

extern "C"	// C-style mangling for ASM linkage
{
u8      RAM[0x10000];               // RAM
u8      SRAM[0x8000];               // Save RAM
u8      VRAM[0x4000];               // Video RAM
u8      PRAM[0x40];                 // Palette RAM
u8 *    ROM;                        // Emulated ROM
u8 *    Game_ROM;                   // Cartridge ROM
u8 *    Game_ROM_Computed_Page_0;   // Cartridge ROM computed first page
u8 *    Mem_Pages [8];              // Pointer to memory pages
u8 *	sprite_attribute_table;
}

u8 *    BACK_AREA = NULL;
u8 *    SG_BACK_TILE = NULL;
u8 *    SG_BACK_COLOR = NULL;

ALLEGRO_DISPLAY*		g_display = NULL;
ALLEGRO_LOCKED_REGION*	g_screenbuffer_locked_region = NULL;

ALLEGRO_BITMAP *screenbuffer = NULL, *screenbuffer_next = NULL;
ALLEGRO_BITMAP *screenbuffer_1 = NULL, *screenbuffer_2 = NULL;
ALLEGRO_BITMAP *fs_out = NULL;
ALLEGRO_BITMAP *fs_page_0 = NULL, *fs_page_1 = NULL, *fs_page_2 = NULL;
ALLEGRO_BITMAP *gui_buffer = NULL;
ALLEGRO_BITMAP *gui_background = NULL;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Init_Emulator (void)
//-----------------------------------------------------------------------------
// FIXME: this function is pretty old and is basically a left-over or
// everything that was not moved elsewhere.
//-----------------------------------------------------------------------------
static void Init_Emulator (void)
{
    Video_Init ();

    memset(RAM, 0, 0x10000);        // RAM: 64 Kb (max=SF-7000)
    memset(SRAM, 0, 0x8000);        // SRAM: 32 Kb (max)
    memset(VRAM, 0, 0x4000);        // VRAM: 16 Kb
    memset(PRAM, 0, 0x0040);        // PRAM: 64 bytes
    ROM = Game_ROM_Computed_Page_0 = (u8*)Memory_Alloc(0x4000); // 16 kbytes (one page)
    memset(Game_ROM_Computed_Page_0, 0, 0x4000);
    Game_ROM = NULL;

    tsms.Pages_Count_8k = 1;
    tsms.Pages_Count_16k = 1;

    RdZ80 = RdZ80_NoHook = Read_Default;
    drv_set (cur_machine.driver_id);
}

static void Init_LookUpTables (void)
{
    Coleco_Init_Table_Inputs ();
    #ifdef X86_ASM
        Decode_Tile_ASM_Init ();
    #endif
}

// Initialize default configuration settings
static void Init_Default_Values (void)
{
    g_env.debug_dump_infos = FALSE;

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
    g_Configuration.debugger_console_lines        = 24;
    g_Configuration.debugger_disassembly_lines    = 15;
    g_Configuration.debugger_disassembly_display_labels = TRUE;
    g_Configuration.debugger_log_enabled          = TRUE;

    // Applet: Memory Editor
    g_Configuration.memory_editor_lines           = 16;
    g_Configuration.memory_editor_columns         = 16;

    // Video
    g_Configuration.video_mode_depth_desktop		= 0;    // Unknown yet
	g_Configuration.video_mode_game_fullscreen		= FALSE;
	g_Configuration.video_mode_game_depth			= 16;   // 16-bits
	g_Configuration.video_mode_game_depth_cfg		= 16;   // 16-bits
	g_Configuration.video_mode_game_vsync			= FALSE;
	g_Configuration.video_mode_game_triple_buffering= TRUE;
	g_Configuration.video_mode_game_page_flipping	= FALSE;
	g_Configuration.video_mode_gui_fullscreen		= FALSE;
    g_Configuration.video_mode_gui_depth			= 0;    // Auto
    g_Configuration.video_mode_gui_depth_cfg		= 0;    // Auto
    g_Configuration.video_mode_gui_res_x			= 640;
    g_Configuration.video_mode_gui_res_y			= 480;
    // FIXME-ALLEGRO5: no video driver
	//g_Configuration.video_mode_gui_driver			= GFX_AUTODETECT_FULLSCREEN;
    g_Configuration.video_mode_gui_refresh_rate		= 0;    // Auto
    g_Configuration.video_mode_gui_vsync			= FALSE;

	// Capture
	g_Configuration.capture_filename_template		= "%s-%02d.png";
	g_Configuration.capture_crop_scrolling_column	= TRUE;
	g_Configuration.capture_crop_align_8x8			= FALSE;
	g_Configuration.capture_include_gui				= TRUE;

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

static void Free_Memory (void)
{
    free (Game_ROM_Computed_Page_0);
    BIOS_Free_Roms ();
    if (Game_ROM)
    {
        free (Game_ROM);
        Game_ROM = NULL;
    }
}

static void Close_Emulator (void)
{
    Sound_Close          ();
    Desktop_Close        ();
    Fonts_Close          ();
    FDC765_Close         ();
    Palette_Close        ();
    Inputs_Sources_Close ();
#ifdef MEKA_JOY
	Inputs_Joystick_Close();
#endif
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
/*
static void Close_Callback (void)
{
    al_set_close_button_callback(NULL);
}
*/

// Change to starting directory
// This function is registered in the atexit() table to be called on quit
static void Close_Emulator_Starting_Dir (void)
{
    chdir (g_env.Paths.StartingDirectory);
}

static int Init_Allegro (void)
{
    ConsolePrint (Msg_Get (MSG_Init_Allegro));

    // Initialize timer BEFORE allegro
    // OSD_Timer_Initialize ();

    //set_uformat(U_ASCII);	// FIXME-ALLEGRO5: need an equivalent?
    al_init();
	al_init_font_addon();
	al_init_image_addon();
	al_init_primitives_addon();

	// FIXME-ALLEGRO5: Default display format/depth?
    //g_Configuration.video_mode_depth_desktop = desktop_color_depth();
	g_Configuration.video_mode_depth_desktop = 0;
    if (g_Configuration.video_mode_depth_desktop == 0)
        g_Configuration.video_mode_depth_desktop = 32;	// Default

    //install_timer();

    // Keyboard, mouse
    al_install_keyboard();
    g_env.mouse_installed = al_install_mouse();

    // text_mode (-1); // now obsolete
    //ConsolePrint ("\n");

	// Get Allegro version and print it in console
	{
		const unsigned int allegro_version = al_get_allegro_version();
		char buf[256];
		sprintf(buf, "%d.%d.%d (release %d)", 
				(allegro_version >> 24),
				(allegro_version >> 16) & 0xFF,
				(allegro_version >> 8) & 0xFF,
				(allegro_version & 0xFF));
		ConsolePrintf(" version %s\n", buf);
	}

    return (1);
}

static void Init_GUI (void)
{
    ConsolePrintf ("%s\n", Msg_Get (MSG_Init_GUI));
    gui_init(g_Configuration.video_mode_gui_res_x, g_Configuration.video_mode_gui_res_y, g_Configuration.video_mode_gui_depth);
}

// MAIN FUNCTION --------------------------------------------------------------
int main(int argc, char **argv)
{
    #ifdef ARCH_WIN32
        // Need for XP manifest stuff
        InitCommonControls();
    #endif

    ConsoleInit (); // First thing to do
    #ifdef ARCH_WIN32
        ConsolePrintf ("%s (built %s %s)\n(c) %s %s\n--\n", MEKA_NAME_VERSION, MEKA_BUILD_DATE, MEKA_BUILD_TIME, MEKA_DATE, MEKA_AUTHORS);
    #else
        ConsolePrintf ("\n%s (c) %s %s\n--\n", MEKA_NAME_VERSION, MEKA_DATE, MEKA_AUTHORS);
    #endif

    // Wait for Win32 console signal
    if (!ConsoleWaitForAnswer(TRUE))
        return (0);

    // Save command line parameters
    g_env.argc = argc;
    g_env.argv = (char**)malloc (sizeof(char *) * (g_env.argc + 1));
	int i;
    for (i = 0; i != g_env.argc; i++)
    {
        g_env.argv[i] = strdup(argv[i]);
        //#ifndef ARCH_UNIX
        //  strupr(g_env.argv[i]);
        //#endif
    }
    g_env.argv[i] = NULL;

    // FIXME: add 'init system' here

    // Initializations
    g_env.state = MEKA_STATE_INIT;
    Filenames_Init          (); // Set Filenames Values
    Messages_Init           (); // Load MEKA.MSG and init messaging system
    //Register_Init         (); // Check Registered User Key
    Init_Default_Values     (); // Set Defaults Variables
    Command_Line_Parse      (); // Parse Command Line (1)
    Init_Allegro            (); // Initialize Allegro Library
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

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL);
	g_display = al_create_display(800, 600);	// FIXME-ALLEGRO5: fixed size

    // Window title & callback
    al_set_window_title(g_display, Msg_Get(MSG_Window_Title));
    //al_set_close_button_callback(Close_Button_Callback);

    Blit_Init               (); // Initialize Blitter
    Random_Init             (); // Initialize Random Number Generator
    Fonts_Init              (); // Initialize Fonts system
	Effects_TV_Init			();	// Initialize TV snow effect
    FDC765_Init             (); // Initialize Floppy Disk emulation
    Data_Init               (); // Load datafile
    Init_Emulator           (); // Initialize Emulation
    Palette_Init            (); // Initialize Palette system
    NES_Init                (); // Initialize NES emulation
    Init_LookUpTables		(); // Initialize Look-up tables
    Machine_Init            (); // Initialize Virtual Machine
    Init_GUI                (); // Initialize Graphical User Interface
    Sound_Init              (); // Initialize Sound
#ifdef MEKA_JOY
    Inputs_Joystick_Init    (); // Initialize Joysticks. 
#endif
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
        g_env.state = MEKA_STATE_FULLSCREEN;
    else
        g_env.state = MEKA_STATE_GUI;
    Video_Setup_State ();

    // Start main program loop
    // Everything runs from there.
    // Z80_Opcodes_Usage_Reset ();
    Main_Loop               (); 
    // Z80_Opcodes_Usage_Print ();

    // Shutting down emulator...
    g_env.state = MEKA_STATE_SHUTDOWN;
    Video_Setup_State       (); // Switch back to text mode
    BMemory_Save            (); // Write Backed Memory if necessary
    Configuration_Save      (); // Write Configuration File
    Write_Inputs_Src_List   (); // Write Inputs Definition File
    VLFN_Close              (); // Write Virtual Long Filename List
    Close_Emulator          (); // Close Emulator
    Show_End_Message        (); // Show End Message

    return (0);
}

//-----------------------------------------------------------------------------

