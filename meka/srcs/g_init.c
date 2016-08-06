//-----------------------------------------------------------------------------
// MEKA - g_init.c
// GUI Initialization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_about.h"
#include "app_cheatfinder.h"
#include "app_filebrowser.h"
#include "app_game.h"
#include "app_mapview.h"
#include "app_memview.h"
#include "app_palview.h"
#include "app_options.h"
#include "app_techinfo.h"
#include "app_textview.h"
#include "app_tileview.h"
#include "datadump.h"
#include "debugger.h"
#include "desktop.h"
#include "inputs_c.h"
#include "skin_bg.h"
#include "skin_fx.h"
#include "textbox.h"

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static void    GUI_InitApplets(void);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    GUI_Init()
{
    opt.GUI_Inited = TRUE;

    gui_buffer = NULL;
    gui_background = NULL;

    gui.info.screen_pad.x = 2;
    gui.info.screen_pad.y = 2;
    gui.info.bars_height = Font_Height((t_font_id)g_configuration.font_menus)+3;
    gui.info.grid_distance = 32;

    gui.boxes = NULL;
    gui.boxes_count = 0;

	gui.info.screen.x = g_configuration.video_mode_gui_res_x;
    gui.info.screen.y = g_configuration.video_mode_gui_res_y;
	GUI_CreateVideoBuffers();

    Desktop_Init();

	// Create game box (create before applets so it gets focus by default when there's no .dsk file)
    static bool active_dummy = TRUE;
    gamebox_instance = gamebox_create(163, 151);
    Desktop_Register_Box("GAME", gamebox_instance, 1, &active_dummy);

	GUI_InitApplets();
    SkinFx_Init();

    Desktop_SetStateToBoxes();     // Set all boxes state based on MEKA.DSK data
    gui_menus_init();              // Create menus (Note: need to be done after Desktop_SetStateToBoxes because it uses the 'active' flags to check items)
    gui_init_mouse();
}

void	GUI_SetupNewVideoMode()
{
	gui.info.must_redraw = TRUE;
	gui.info.screen.x = g_configuration.video_mode_gui_res_x;
    gui.info.screen.y = g_configuration.video_mode_gui_res_y;
	GUI_CreateVideoBuffers();

    Skins_StartupFadeIn();

    // Fix windows position
    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;;
        gui_box_clip_position(box);
    }
}

void	GUI_DestroyVideoBuffers()
{
    if (gui_buffer != NULL)
    {
        al_destroy_bitmap(gui_buffer);
        gui_buffer = NULL;
        assert(gui_background != NULL);
        al_destroy_bitmap(gui_background);
        gui_background = NULL;
    }

    // Recreate existing windows buffers
    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;;
		gui_box_destroy_video_buffer(box);
	}
}

void	GUI_CreateVideoBuffers()
{
    GUI_DestroyVideoBuffers();

    // Setup buffers
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_NO_PRESERVE_TEXTURE);
	al_set_new_bitmap_format(g_configuration.video_gui_format_request);
    gui_buffer = al_create_bitmap(gui.info.screen.x, gui.info.screen.x);
    al_set_target_bitmap(gui_buffer);
	al_clear_to_color(COLOR_BLACK);
	g_gui_buffer_format = al_get_bitmap_format(gui_buffer);

	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_NO_PRESERVE_TEXTURE);
	al_set_new_bitmap_format(g_configuration.video_gui_format_request);
    gui_background = al_create_bitmap(gui.info.screen.x, gui.info.screen.x);

    Skins_Background_Redraw();

    // Recreate existing windows buffers
    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;;
		gui_box_create_video_buffer(box);
	}
}

void	GUI_Close(void)
{
    // FIXME: Nice....

    TextViewer_Close(&TextViewer);
}

void    GUI_InitApplets(void)
{
    AboutBox_Init();
    TB_Message_Init();
    MemoryViewer_MainInstance = MemoryViewer_New(TRUE, -1, -1);
    TilemapViewer_MainInstance = TilemapViewer_New(TRUE);
	g_CheatFinder_MainInstance = CheatFinder_New(TRUE);

    // Text Viewer
    TextViewer_Init(&TextViewer);
    // FIXME: save current file in meka.cfg
    if (TextViewer_Open(&TextViewer, Msg_Get(MSG_Doc_BoxTitle), g_env.Paths.DocumentationMain) != MEKA_ERR_OK)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Doc_File_Error));
    TextViewer.current_file = 0; // FIXME: Remove this field

    TechInfo_Init();
    TileViewer_Init();
    PaletteViewer_Init();
    // FM_Editor_Init();
    FB_Init();
    Options_Init_Applet();
    Inputs_CFG_Init_Applet();

#ifdef SOUND_DEBUG_APPLET
	SoundDebugApp_Init();
#endif

    // Debugger
    #ifdef MEKA_Z80_DEBUGGER
    Debugger_Enable();
    Debugger_Init();
    DataDump_Init();
    #endif
}

//-----------------------------------------------------------------------------
