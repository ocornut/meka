//-----------------------------------------------------------------------------
// MEKA - meka.h
// Main variables, headers, prototypes and constants
//-----------------------------------------------------------------------------

// Z80 CPU Clock (3.579540 MHz) -----------------------------------------------
// FIXME: this should be obsoleted by what is in TVTYPE.C/.H
#define Z80_DEFAULT_CPU_CLOCK   (3579540 /*3579545*/) // Emu use 60*262*228 = 3584160?

// Emulated screen resolutions ------------------------------------------------
#define SMS_RES_X               (256)
#define SMS_RES_Y               (192)
#define SMS_RES_Y_TOTAL         (224)
#define GG_RES_X                (160)
#define GG_RES_Y                (144)
#define MAX_RES_X               SMS_RES_X
#define MAX_RES_Y               SMS_RES_Y_TOTAL

// Max length of a message ----------------------------------------------------
#define MSG_MAX_LEN             (16384)

// Max tiles (in video mode 5)
#define MAX_TILES               (512)

// Fixed colors
extern ALLEGRO_COLOR			COLOR_BLACK;
extern ALLEGRO_COLOR			COLOR_WHITE;
extern ALLEGRO_COLOR			COLOR_DEBUG_BACKDROP;	// When background render is disabled

#define COLOR_BLACK16			0x0000
#define COLOR_WHITE16			0xFFFF

// Border Color
// FIXME: Unsupported
#define BORDER_COLOR            COLOR_BLACK		//((sms.VDP[7] & 15) + 16)
#define BORDER_COLOR16			COLOR_BLACK16

extern "C"	// C-style mangling
{
extern u8      RAM[0x10000];               // RAM
extern u8      SRAM[0x8000];               // Save RAM
extern u8      VRAM[0x4000];               // Video RAM
extern u8      PRAM[0x40];				   // Palette RAM
extern u8 *    ROM;                        // Emulated ROM
extern u8 *    Game_ROM;                   // Cartridge ROM
extern u8 *    Game_ROM_Computed_Page_0;   // Cartridge ROM computed first page
extern u8 *    Mem_Pages[8];               // Pointer to memory pages
};

// Flags for layer handling ---------------------------------------------------
#define LAYER_BACKGROUND		(0x01)
#define LAYER_SPRITES           (0x02)

// Main MEKA state ------------------------------------------------------------
enum t_meka_state
{
	MEKA_STATE_INIT,
	MEKA_STATE_GAME,
	MEKA_STATE_GUI,
	MEKA_STATE_SHUTDOWN,
};

// Battery Backed RAM Macros --------------------------------------------------
#define SRAM_Active             (sms.SRAM_Mapping_Register & 0x08)
#define SRAM_Page               (sms.SRAM_Mapping_Register & 0x04)

// On Board RAM Macros (currently only for Ernie Els Golf)
#define ONBOARD_RAM_EXIST       (0x20)
#define ONBOARD_RAM_ACTIVE      (0x40)

// Variables needed by one emulated SMS
// FIXME: reconceptualize those stuff, this is pure, old crap
struct SMS_TYPE
{
    // CPU State
#ifdef MARAT_Z80
    Z80   R;                              // CPU Registers (Marat Faizullin)
#elif RAZE_Z80
    void *CPU;                            // CPU Registers (Richard Mitton)
#elif MAME_Z80
    // nothing. currently implemented as global
#endif
    // Other State
    u8      VDP [16];                      // VDP Registers
    u8      __UNUSED__PRAM_Address;        // Current palette address
    // NOTE: variable below (VDP_Status) is modified from videoasm.asm, do NOT move it
    u8      VDP_Status;                    // Current VDP status
    u16     VDP_Address;                   // Current VDP address
    u8      VDP_Access_Mode;               // 0: Address Low - 1: Address High
    u8      VDP_Access_First;              // Address Low Latch
    u8      VDP_ReadLatch;                 // Read Latch
    u8      VDP_Pal;                       // Currently Reading Palette ?
    u8      Country;                       // 0: English - 1: Japanese
    int     Lines_Left;                    // Lines Left before H-Blank
    u8      Pending_HBlank;                // Pending HBL interrupt
    u8      Pending_NMI;                   // Pending NMI interrupt (for Coleco emulation)
    u8      Glasses_Register;              // 3-D Glasses Register
    u8      SRAM_Pages;                    // SRAM pages used
    u8      SRAM_Mapping_Register;         // SRAM status + mapping offset
    u8      FM_Magic;                      // FM Latch (for detection)
    u8      FM_Register;                   // FM Register
    u8      Input_Mode;   // Port 0xDE     // 0->6: Keyboard - 7: Joypad
};

// Temporary (not saved) data for one machine
// FIXME: re-conceptualize those stuff, this is pure, old crap
struct TSMS_TYPE
{
    u16     Control [8];                   // 0->6 = Keyboard - 7: Joypads
    u8      Control_GG;
    u8      Control_Check_GUI;
    u8      Control_Start_Pause;
    int     VDP_Line;
    int     Pages_Mask_8k,  Pages_Count_8k;
    int     Pages_Mask_16k, Pages_Count_16k;
    long    Size_ROM;
    u8      Port3F;
    int     VDP_VideoMode, VDP_New_VideoMode;
    int     VDP_Video_Change;
};

// Bits for gfx.Tile_Dirty
#define TILE_DIRTY_DECODE       (0x01)
#define TILE_DIRTY_REDRAW       (0x02)

// Variables related to graphics - not saved in savestate
struct TGFX_TYPE
{
	u8	Tile_Dirty [MAX_TILES];
	u8	Tile_Decoded [MAX_TILES] [64];
};

struct OPT_TYPE
{
    bool        GUI_Inited;
    int         IPeriod, IPeriod_Coleco, IPeriod_Sg1000_Sc3000, Cur_IPeriod;
    int         Layer_Mask;
    int         Current_Key_Pressed;
    //----
    bool        Force_Quit;                         // Set to TRUE for program to quit
    int         State_Current;                      // Current savestate number
    int         State_Load;                         // Set to != 1 to set and load a state on Machine_Reset();
    bool        Setup_Interactive_Execute;          // Set to TRUE to execute an interactive setup on startup
    int         GUI_Current_Page;
};

// Max path length
#if defined(ARCH_UNIX) || defined(ARCH_MACOSX)
#define FILENAME_LEN	(PATH_MAX)
#else
#define FILENAME_LEN	(1024)
#endif

extern "C"
{
extern OPT_TYPE   opt;
extern TGFX_TYPE  tgfx;
extern SMS_TYPE   sms;
extern TSMS_TYPE  tsms;
}

//-----------------------------------------------------------------------------
// NEW STRUCTURES
// Below are new structures which should hopefully obsolete everything above.
// Members can eventually be moved out of old structure to those new ones.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Emulated machine
//-----------------------------------------------------------------------------

// 'STATE' denote fields that should be saved in a savestate
// 'CACHE' denote fields that directly derive from emulated machine data (eg: vdp register)
// 'CONFIG' denote fields that derive from emulator configuration
struct t_machine_vdp_smsgg
{
    int                     model;								// CONFIG
    int                     sprite_shift_x;						// CACHE	// 0 or 8
	u8 *					name_table_address;					// CACHE
	u8 *					sg_color_table_address;				// CACHE	// Was g_machine.VDP.sg_color_table_address
	u8 *					sg_pattern_gen_address;				// CACHE	// Was g_machine.VDP.pattern_gen_address
	int						sprite_pattern_gen_index;			// CACHE	// 0 or 256, SMS/GG only
	u8 *					sprite_pattern_gen_address;			// CACHE
	u8 *					sprite_attribute_table;				// CACHE

    // Scrolling latches
    u8                      scroll_x_latched;					// STATE
    u8                      scroll_y_latched;					// STATE
    u8                      scroll_x_latched_table[MAX_RES_Y];	// CACHE	// For tools
};

#define MAPPER_REGS_MAX		(6)									// Janggun-ui Adeul uses up to 6

struct t_machine
{
    int                     driver_id;							// STATE	// t_machine_driver
    int                     mapper;								// STATE
	u8						mapper_regs[MAPPER_REGS_MAX];		// STATE
	int						mapper_regs_count;					// CACHE	// Derived from 'mapper'
	int						mapper_janggun_bytes_flipping_flags;// CACHE
    t_machine_vdp_smsgg     VDP;
    struct t_tv_type *      TV;									// CONFIG
    int                     TV_lines;							// CONFIG	// Copy of TV->screen_lines
};

extern t_machine   g_machine;

//-----------------------------------------------------------------------------
// Runtime environment
//-----------------------------------------------------------------------------

struct t_meka_env_paths
{
    char    EmulatorDirectory       [FILENAME_LEN];
    char    StartingDirectory       [FILENAME_LEN];
    char    ConfigurationFile       [FILENAME_LEN];
    char    DataBaseFile            [FILENAME_LEN];
    char    DataFile                [FILENAME_LEN];
    char    SkinFile                [FILENAME_LEN];
    char    ScreenshotDirectory     [FILENAME_LEN];
    char    SavegameDirectory       [FILENAME_LEN];
    char    BatteryBackedMemoryFile [FILENAME_LEN];
    char    MusicDirectory          [FILENAME_LEN];
    char    DebugDirectory          [FILENAME_LEN];

    char    MediaImageFile          [FILENAME_LEN];     // FIXME: abstract media (per type/slot)

    char    DocumentationMain       [FILENAME_LEN];
    char    DocumentationCompat     [FILENAME_LEN];
    char    DocumentationMulti      [FILENAME_LEN];
    char    DocumentationChanges    [FILENAME_LEN];
    char    DocumentationDebugger   [FILENAME_LEN];
    // FIXME: add and use TECH.TXT ?
};

struct t_meka_env
{
    t_meka_env_paths    Paths;
    int                 mouse_installed;
	int					argc;
	char **				argv;

	t_meka_state		state;
	bool				debug_dump_infos;
};

extern t_meka_env  g_env;

//-----------------------------------------------------------------------------
// Configuration
// All MEKA configuration options
// Note: this is dependant of runtime emulation configuration, which can
// be affected by various factors.
// This structure should basically reflect the content of MEKA.CFG
//-----------------------------------------------------------------------------

// Values for g_configuration.sprite_flickering
#define SPRITE_FLICKERING_NO        (0)
#define SPRITE_FLICKERING_ENABLED   (1)
#define SPRITE_FLICKERING_AUTO      (2) // Default

struct t_video_driver;

struct t_meka_configuration
{
	bool	loaded_configuration_file;

    // Country
    int     country;                    // Country to use
    int     sprite_flickering;          // Set to emulate sprite flickering.
    bool    allow_opposite_directions;  // Allows pressing of LEFT-RIGHT / UP-DOWN simultaneously.
    bool    enable_BIOS;
    bool    enable_NES;             
    bool    show_fullscreen_messages;
    bool    show_product_number;
    bool    start_in_gui;

    // Applet: Game Screen
    float   game_window_scale;

    // Applet: File Browser
    bool    fb_close_after_load;
    bool    fb_uses_DB;
    bool    fullscreen_after_load;

    // Applet: Debugger
    bool    debugger_disassembly_display_labels;
    bool    debugger_log_enabled;

    // Applet: Memory Editor
    int				memory_editor_lines;
    int				memory_editor_columns;

    // Video
	t_video_driver*	video_driver;
	bool			video_fullscreen;
	int				video_game_format_request;
	int				video_gui_format_request;

	bool			video_mode_game_vsync;
    int				video_mode_gui_res_x;
    int				video_mode_gui_res_y;
    bool			video_mode_gui_vsync;
    int				video_mode_gui_refresh_rate;

	// Capture
	const char *	capture_filename_template;
	bool			capture_crop_scrolling_column;
	bool			capture_crop_align_8x8;
	bool			capture_include_gui;

	// Fonts (t_font_id)
	int				font_menus;
	int				font_messages;
	int				font_options;
	int				font_debugger;
	int				font_documentation;
	int				font_techinfo;
	int				font_filebrowser;
	int				font_about;
};

extern t_meka_configuration    g_configuration;

//-----------------------------------------------------------------------------
// Media image
// Old image of a loaded media (ROM, disk, etc...).
//-----------------------------------------------------------------------------

struct t_meka_crc
{
    u32         v[2];
};

#define MEDIA_IMAGE_ROM     (0)

struct t_media_image
{
    int         type;
    u8 *        data;
    int         data_size;
    t_meka_crc  mekacrc;
    u32         crc32;
    // char *   filename ?
};

// Currently a global to hold ROM infos.
// Note that the structure is currently only half used and supported.
// We only use the 'meka_checksum' and 'crc32' fields yet.
extern t_media_image   g_media_rom;

//-----------------------------------------------------------------------------
// Data (video buffers)
//-----------------------------------------------------------------------------

extern ALLEGRO_DISPLAY*			g_display;
extern ALLEGRO_EVENT_QUEUE*		g_display_event_queue;
extern ALLEGRO_LOCKED_REGION*	g_screenbuffer_locked_region;
extern int						g_screenbuffer_format;
extern int						g_gui_buffer_format;

// Video buffers
extern ALLEGRO_BITMAP *screenbuffer, *screenbuffer_next;  // Pointers to screen memory buffers
extern ALLEGRO_BITMAP *screenbuffer_1, *screenbuffer_2;   // Screen memory buffers
extern ALLEGRO_BITMAP *fs_out;                            // Fullscreen video buffer
extern ALLEGRO_BITMAP *gui_buffer;                        // GUI memory buffer
extern ALLEGRO_BITMAP *gui_background;                    // GUI Background

//-----------------------------------------------------------------------------
