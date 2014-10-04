//-----------------------------------------------------------------------------
// MEKA - g_update.c
// GUI Update - Code
//-----------------------------------------------------------------------------
// FIXME: the whole updating process is an hardcored mess.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_about.h"
#include "app_cheatfinder.h"
#include "app_filebrowser.h"
#include "app_mapview.h"
#include "app_memview.h"
#include "app_palview.h"
#include "app_techinfo.h"
#include "app_textview.h"
#include "app_tileview.h"
#include "app_options.h"
#include "debugger.h"
#include "g_widget.h"
#include "inputs_t.h"
#include "skin_fx.h"
#include "textbox.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

void    gui_update_applets();
void    gui_update_applets_after_redraw();
void    gui_update_applets_before();

// Update the differents GUI applets
void    gui_update_applets()
{
    // Update Memory Editors first,
    // because it allows changing tile data/palette so we want that to be reflected 
    // (especially since dirty flags are being cleared each frame)
    MemoryViewers_Update();
	PROFILE_STEP("- MemoryViewers_Update");

    TilemapViewers_Update();
	PROFILE_STEP("- TilemapViewers_Update");

	CheatFinders_Update();
	PROFILE_STEP("- CheatFinders_Update");

    // Tile Viewer flag appropriate decoded VRAM tiles before emulation run
    TileViewer_Update(&TileViewer);
	PROFILE_STEP("- TileViewer_Update");

	PaletteViewer_Update();
	PROFILE_STEP("- PaletteViewer_Update");

    //if (TB_Message.Active)
    //    TB_Update (TB_Message.TB);
    //if (apps.active.FM_Editor)
    //    gui.box [apps.id.FM_Editor]->update();

    TechInfo_Update();
    FB_Update();
    Options_Update();
    AboutBox_Update();
    TB_Message_Update();
    TextViewer_Update(&TextViewer);
	PROFILE_STEP("- misc");

	#ifdef SOUND_DEBUG_APPLET
		SoundDebugApp_Update();
	#endif

    #ifdef MEKA_Z80_DEBUGGER
        Debugger_Update();
    #endif
}

void    gui_update_applets_after_redraw()
{
    // Theme effects (blood/snow/hearts) : restoring data to the framebuffer
    if (Skins_GetCurrentSkin()->effect != SKIN_EFFECT_NONE)
        special_effects_update_after();
}

void    gui_update()
{
    // Theme effects (blood/snow/hearts) : saving data from the framebuffer
    if (Skins_GetCurrentSkin()->effect != SKIN_EFFECT_NONE)
        special_effects_update_before();

    // Skins update
    Skins_Update();

    // Menus update
    // Note: must be done before updating applets
    gui_update_menus();        

    // Boxes update (move / compute dirtyness)
    gui_update_boxes();
	PROFILE_STEP("gui_update_boxes()");

    // Process box deletion
    for (t_list* boxes = gui.boxes; boxes != NULL; )
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;
        boxes = boxes->next;
        if (box->flags & GUI_BOX_FLAGS_DELETE)
            gui_box_delete(box);
    }

    // Widgets update
    widgets_call_update();
	PROFILE_STEP("widgets_call_update()");

    // Call applets handlers
    // Note: Must be done after updating widgets
    gui_update_applets();
	PROFILE_STEP("gui_update_applets()");
}

//-----------------------------------------------------------------------------
