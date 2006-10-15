//-----------------------------------------------------------------------------
// MEKA - g_init.c
// GUI Initialization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"

//-----------------------------------------------------------------------------

void    gui_init (int res_x, int res_y)
{
    opt.GUI_Inited = YES;

    gui_buffer = NULL;
    gui_background = NULL;
    gui_set_resolution(res_x, res_y);

    gui.info.screen_pad.x = 2;
    gui.info.screen_pad.y = 2;
    gui.info.bars_height = 19;
    gui.info.grid_distance = 32;

    gui.box_last = 0;
    Desktop_Init ();
    gui_init_applets ();
    special_effects_init ();
    gui_init_default_box ();
    Desktop_SetStateToBoxes ();     // Set all boxes state based on MEKA.DSK data
    gui_menus_init ();              // Create menus (Note: need to be done after Desktop_SetStateToBoxes because it uses the 'active' flags to check items)
    gui_init_mouse ();
}

//-----------------------------------------------------------------------------
// gui_set_resolution (int res_x, int res_y)
// Set GUI desktop resolution
//-----------------------------------------------------------------------------
// Note: this cannot be naively called from anywhere. 
// Some things needs to be updated accordingly: background redrawn, 
// actual video mode changed, etc.
//-----------------------------------------------------------------------------
void    gui_set_resolution (int res_x, int res_y)
{
    gui.info.screen.x = res_x;
    gui.info.screen.y = res_y;

    // Destroy existing buffers (if any)
    if (gui_buffer != NULL)
    {
        destroy_bitmap(gui_buffer);
        gui_buffer = NULL;
        assert(gui_background != NULL);
        destroy_bitmap(gui_background);
        gui_background = NULL;
    }

    // Setup buffers
    switch (cfg.GUI_Access_Mode)
    {
    case GUI_FB_ACCESS_DIRECT:
        // Direct accesses to video memory
        // ... no initialization here ...
        break;
    case GUI_FB_ACCESS_BUFFERED:
        // Buffered accesses to video memory (default, the only good one now)
        gui_buffer = create_bitmap (res_x, res_y);
        clear_to_color (gui_buffer, 0);
        break;
    case GUI_FB_ACCESS_FLIPPED:
        // Direct accesses with page flipping
        // Unworking because the GUI doesn't refresh everything everytime,
        // so the two pages are not in sync.
        // ... no initialization here ...
        break;
    }
    gui_background = create_bitmap (gui.info.screen.x, gui.info.screen.x);
}

void    gui_init_again (void)
{
    gui_init_colors ();
    gui.info.must_redraw = YES;
}

// CLOSE GUI / FREE (SOME) ALLOCATED MEMORY -----------------------------------
void        gui_close (void)
{
    int     i;

    // FIXME: doing only that and nothing else is admitting the pure 
    // lameness of this code.
    for (i = 0; i < gui.box_last; i ++)
        free (gui.box [i]);
}

//-----------------------------------------------------------------------------
