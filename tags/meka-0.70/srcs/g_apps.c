//-----------------------------------------------------------------------------
// MEKA - g_apps.c
// GUI Applets (some) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "about.h"
#include "debugger.h"
#include "desktop.h"
#include "g_file.h"
#include "g_widget.h"
#include "memview.h"
#include "options.h"
#include "techinfo.h"
#include "tileview.h"

//-----------------------------------------------------------------------------
// gui_init_applets (void)
// Initialize default GUI applets
//-----------------------------------------------------------------------------
void    gui_init_applets (void)
{
    // About box
    About_Init ();

    // Message Log
    TB_Message_Init ();

    // Memory Viewer
    MemoryViewer_Init();

    // Text Viewer
    TextViewer_Init ();
    // FIXME: save current file in .cfg
    if (TextViewer_Open (TextViewer.TV, Msg_Get (MSG_Doc_BoxTitle), Env.Paths.DocumentationMain) != MEKA_ERR_OK)
        Msg (MSGT_USER, Msg_Get (MSG_Doc_File_Error));
    TextViewer.CurrentFile = 0;

    // Technical Information
    TechInfo_Init ();

    // Voice Recognition
    // FIXME: Is this still around ?
    apps.id.Voice_Rec = gui_box_create (10, 50, 147, 29, Msg_Get (MSG_VoiceRecognition_BoxTitle));
    apps.gfx.Voice_Rec = create_bitmap (148, 30);
    gui_set_image_box (apps.id.Voice_Rec, apps.gfx.Voice_Rec);
    gui.box [apps.id.Voice_Rec]->update = gui_applet_voice_rec;
    apps.opt.Voice.Dir = 0;
    apps.opt.Voice.Value = 0;
    apps.opt.Voice.Delay = 0;
    Desktop_Register_Box ("VOICEREC", apps.id.Voice_Rec, 0, &apps.active.Voice_Rec);

    // Tiles Viewer
    TileViewer_Init ();

    // Palette Viewer
    apps.id.Palette = gui_box_create (15, 53, 191, 49, Msg_Get (MSG_Palette_BoxTitle));
    apps.gfx.Palette = create_bitmap (192, 50);
    gui_set_image_box (apps.id.Palette, apps.gfx.Palette);
    gui_applet_palette_configure (cur_drv->colors);
    gui.box [apps.id.Palette]->update = gui_applet_palette_update;
    widget_closebox_add (apps.id.Palette, Action_Switch_Palette);
    Desktop_Register_Box ("PALETTE", apps.id.Palette, 1, &apps.active.Palette);

    // FM Instruments Editor
    FM_Editor_Init ();

    // File Browser
    FB_Init ();

    // Options
    Options_Init_Applet ();

    // Inputs Configuration
    Inputs_CFG_Init_Applet ();

    // Debugger
    #ifdef MEKA_Z80_DEBUGGER
    if (Configuration.debug_mode)
    {
        Debugger_Enable ();
        Debugger_Init ();
        DataDump_Init ();
    }
    #endif
}

// UPDATE THE VOICE RECOGNITION APPLET ----------------------------------------
void    gui_applet_voice_rec (void)
{
    if (apps.opt.Voice.Old_Value == apps.opt.Voice.Value)
        return;
    rectfill (apps.gfx.Voice_Rec, 10, 5, 10 + apps.opt.Voice.Value, 25, GUI_COL_HIGHLIGHT);
    rectfill (apps.gfx.Voice_Rec, 10 + apps.opt.Voice.Value, 5, 138, 25, GUI_COL_BORDERS);
    apps.opt.Voice.Old_Value = apps.opt.Voice.Value;
    gui.box[apps.id.Voice_Rec]->must_redraw = YES;
}

void        gui_applet_palette_configure (int size)
{
    int     i;
    int     bar_size;

    apps.opt.Palette_Size = size;
    bar_size = apps.gfx.Palette->w / size;
    for (i = 0; i < size; i++)
        rectfill (apps.gfx.Palette,
        (i * bar_size), 0, (i * bar_size) + bar_size - 1, 49,
        Palette_Refs [i]);
    gui.box[apps.id.Palette]->must_redraw = YES;
}

// CREATE PALETTE APPLET ------------------------------------------------------
// Note: has to be executed before tileviewer::update
void        gui_applet_palette_update (void)
{
    int     i;
    int     bar_size;
    int     size = apps.opt.Palette_Size;

    bar_size = apps.gfx.Palette->w / size;
    for (i = 0; i < size; i++)
        if (Palette_Refs_Dirty [i])
        {
            rectfill (apps.gfx.Palette, (
                i * bar_size), 0, (i * bar_size) + bar_size - 1, 49,
                Palette_Refs [i]);
            Palette_Refs_Dirty [i] = NO;
            gui.box[apps.id.Palette]->must_redraw = YES;
        }
}

//-----------------------------------------------------------------------------
