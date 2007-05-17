//-----------------------------------------------------------------------------
// MEKA - g_update.c
// GUI Update - Code
//-----------------------------------------------------------------------------
// FIXME: the whole updating process is an hardcored mess.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_about.h"
#include "app_mapview.h"
#include "app_memview.h"
#include "app_palview.h"
#include "app_techinfo.h"
#include "app_textview.h"
#include "app_tileview.h"
#include "app_options.h"
#include "debugger.h"
#include "g_file.h"
#include "g_widget.h"
#include "inputs_t.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

void    gui_update_applets (void);
void    gui_update_applets_after_redraw (void);
void    gui_update_applets_before (void);

//-----------------------------------------------------------------------------
// gui_update_applets ()
// Update the differents GUI applets
//-----------------------------------------------------------------------------
void    gui_update_applets (void)
{
    // Update Memory Editors first,
    // because it allows changing tile data/palette so we want that to be reflected 
    // (especially since dirty flags are being cleared each frame)
    MemoryViewers_Update();

    // Tilemap Viewer
    TilemapViewers_Update();

    // Tile Viewer : flag appropriate decoded VRAM tiles before emulation run
    TileViewer_Update(&TileViewer);

    // Palette Viewer
    PaletteViewer_Update();

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

    #ifdef MEKA_Z80_DEBUGGER
        Debugger_Update();
    #endif
}

// UPDATE GUI APPLETS, AFTER REFRESHING SCREEN --------------------------------
void    gui_update_applets_after_redraw (void)
{
    // Theme effects (blood/snow/hearts) : restoring data to the framebuffer
    if (Skins_GetCurrentSkin()->effect != SKIN_EFFECT_NONE)
        special_effects_update_after ();
}

//-----------------------------------------------------------------------------
// gui_update ()
// Update the GUI
//-----------------------------------------------------------------------------
void    gui_update (void)
{
    t_list *boxes;

    // Update mous data
    gui_update_mouse ();

    // Theme effects (blood/snow/hearts) : saving data from the framebuffer
    if (Skins_GetCurrentSkin()->effect != SKIN_EFFECT_NONE)
        special_effects_update_before ();

    // Skins update
    Skins_Update();

    // Menus update
    // Note: must be done before updating applets
    gui_update_menus ();        

    // Boxes update (move / compute dirtyness)
    gui_update_boxes ();

    // Process box deletion
    for (boxes = gui.boxes; boxes != NULL; )
    {
        t_gui_box *box = boxes->elem;
        boxes = boxes->next;
        if (box->flags & GUI_BOX_FLAGS_DELETE)
            gui_box_delete(box);
    }

    // Widgets update
    widgets_call_update ();

    // Call applets handlers
    // Note: Must be done after updating widgets
    gui_update_applets ();
}

//-----------------------------------------------------------------------------
