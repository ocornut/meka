//-----------------------------------------------------------------------------
// MEKA - g_update.c
// GUI Update - Code
//-----------------------------------------------------------------------------
// FIXME: the whole updating process is an hardcored mess.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_memview.h"
#include "app_palview.h"
#include "debugger.h"
#include "g_file.h"
#include "g_widget.h"

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
    if (apps.active.Voice_Rec)
        gui.box [apps.id.Voice_Rec]->update();
    if (apps.active.Tech)
        gui.box [apps.id.Tech]->update();
    //if (TB_Message.Active)
    //    TB_Update (TB_Message.TB);
    if (apps.active.FM_Editor)
        gui.box [apps.id.FM_Editor]->update();
    if (TextViewer.Active)
        TextViewer_Update_Inputs(TextViewer.TV);
    if (FB.active)
        FB_Update_Inputs();

    if (MemoryViewer.active)
        MemoryViewer_Update_Inputs();

    #ifdef MEKA_Z80_DEBUGGER
        if (Debugger.Active)
            Debugger_Update();
    #endif
}

// UPDATE GUI APPLETS, BEFORE REFRESHING SCREEN ------------------------------
void    gui_update_applets_before (void)
{
    // Tile Viewer : flag appropriate decoded VRAM tiles before emulation run
    if (apps.active.Tiles)
        gui.box[apps.id.Tiles]->update ();
    if (PaletteViewer.active)
        PaletteViewer_Update();

    if (MemoryViewer.active)
        MemoryViewer_Update();

    // Input Configuration. Need to be done before to intercept the ESCAPE key.
    // Done in INPUTS.C !
    // if (Inputs_CFG.Active)
    //   gui.box [Inputs_CFG.ID]->update ();

    // Theme effects (blood/snow/hearts) : saving data from the framebuffer
    if (Themes.special != SPECIAL_NOTHING)
        special_effects_update_before ();
}

// UPDATE GUI APPLETS, AFTER REFRESHING SCREEN --------------------------------
void    gui_update_applets_after_redraw (void)
{
    // Theme effects (blood/snow/hearts) : restoring data to the framebuffer
    if (Themes.special != SPECIAL_NOTHING)
        special_effects_update_after ();
}

//-----------------------------------------------------------------------------
// gui_update ()
// Update the GUI
//-----------------------------------------------------------------------------
void    gui_update (void)
{
    // Those two calls were once in 'gui_update_before()'
    gui_update_mouse ();
    gui_update_applets_before ();

    // ...
    gui_update_applets ();
    Themes_Update ();
    //
    gui_update_menus ();
    gui_update_boxes ();
    widgets_call_update ();
}

//-----------------------------------------------------------------------------
