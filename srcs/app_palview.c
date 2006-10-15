//-----------------------------------------------------------------------------
// MEKA - app_palview.c
// Palette Viewer - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_palview.h"
#include "desktop.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static void    PaletteViewer_CallbackSelectColor(t_widget *w);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    PaletteViewer_Init(t_app_palette_viewer *pv)
{
    int box_id;

    // Setup
    pv->active          = TRUE;
    pv->palette_size    = 32; // Note: PaletteViewer_SetPaletteSize() is called after setting up the window
    pv->color_displayed = -1;
    pv->color_hovered   = -1;
    pv->color_selected  = -1;

    // Create box
    box_id  = gui_box_create (15, 53, 191, 49+13, Msg_Get (MSG_Palette_BoxTitle));
    pv->box = gui.box[box_id];
    //pv->box->update = PaletteViewer_Update;
    pv->box_gfx = create_bitmap (pv->box->frame.size.x + 1, pv->box->frame.size.y + 1);
    gui_set_image_box (box_id, pv->box_gfx);

    // Register to desktop (applet is enabled by default)
    Desktop_Register_Box ("PALETTE", box_id, TRUE, (byte *)&pv->active);

    // Add close Button
    widget_closebox_add (box_id, PaletteViewer_Switch);

    // Setup frames
    pv->frame_palette.pos.x     = 0;
    pv->frame_palette.pos.y     = 0;
    pv->frame_palette.size.x    = pv->box->frame.size.x;
    pv->frame_palette.size.y    = 50;
    pv->frame_info.pos.x        = 0;
    pv->frame_info.pos.y        = 50;
    pv->frame_info.size.x       = pv->box->frame.size.x;
    pv->frame_info.size.y       = 13;
    pv->frame_palette_zone      = widget_button_add(box_id, &pv->frame_palette, 1, PaletteViewer_CallbackSelectColor);

    // Clear palette area with black
    rectfill(pv->box_gfx, pv->frame_palette.pos.x, pv->frame_palette.pos.y, pv->frame_palette.pos.x + pv->frame_palette.size.x, pv->frame_palette.pos.y + pv->frame_palette.size.y, GUI_COL_BLACK);

    // Draw line
    line(pv->box_gfx, pv->frame_info.pos.x, pv->frame_info.pos.y, pv->frame_info.pos.x + pv->frame_info.size.x, pv->frame_info.pos.y, GUI_COL_BORDERS);

    // Draw current color square
    gui_rect(pv->box_gfx, LOOK_THIN, 2, pv->frame_info.pos.y + 1, 2 + 11, pv->frame_info.pos.y + 1 + 11, GUI_COL_BORDERS);

    // Configuration
    PaletteViewer_SetPaletteSize(pv, cur_drv->colors);
}

void    PaletteViewer_Switch(void)
{
    t_app_palette_viewer *pv = &PaletteViewer;  // Global instance

    if (pv->active ^= 1)
        Msg (MSGT_USER, Msg_Get (MSG_Palette_Enabled));
    else
        Msg (MSGT_USER, Msg_Get (MSG_Palette_Disabled));
    gui_box_show (pv->box, pv->active, TRUE);
    gui_menu_inverse_check (menus_ID.tools, 1);
}

// Note: has to be executed after tileviewer::update
void    PaletteViewer_Update(void)
{
    t_app_palette_viewer *pv = &PaletteViewer;  // Global instance

    int         i;
    bool        dirty = FALSE;
    const int   color_box_size = pv->box_gfx->w / pv->palette_size;
    int         color_current;
    bool        color_current_refresh;

    // Update hovered color index
    {
        int mx = pv->frame_palette_zone->mx;
        int my = pv->frame_palette_zone->my;
        if (mx != -1 && my != -1)
            pv->color_hovered = (pv->frame_palette_zone->mx / color_box_size);
        else
            pv->color_hovered = -1;
    }

    color_current = (pv->color_hovered != -1) ? pv->color_hovered : pv->color_selected;
    color_current_refresh = (color_current == -1) ? FALSE : ((color_current != pv->color_displayed) ? TRUE : FALSE);

    // Draw palette
    for (i = 0; i < pv->palette_size; i++)
    {
        if (Palette_Refs_Dirty [i])
        {
            rectfill (pv->box_gfx, 
                (i * color_box_size), 0, 
                (i * color_box_size) + color_box_size - 1, 49,
                Palette_Refs [i]);
            Palette_Refs_Dirty [i] = NO;
            dirty = TRUE;
            if (i == color_current)
                color_current_refresh = TRUE;
        }
    }

    if (color_current_refresh)
    {
        char buf[64];
        char color_bits[20];

        sprintf(buf, "Color %d (0x%02X)", color_current, color_current);

        switch (cur_drv->id)
        {
            case DRV_SMS:
                color_bits[0] = '%';
                Write_Bits_Field(PRAM[color_current], 8, color_bits + 1);
                sprintf(buf+strlen(buf), " - %s", color_bits);
                break;
            case DRV_GG:
                color_bits[0] = '%';
                Write_Bits_Field(PRAM[color_current * 2 + 1], 8, color_bits + 1);
                color_bits[9] = '.';
                Write_Bits_Field(PRAM[color_current * 2 + 0], 8, color_bits + 10);
                sprintf(buf+strlen(buf), " - %s", color_bits);
                break;
            default:
                color_bits[0] = 0;
                break;
        }
        rectfill(pv->box_gfx, 4, pv->frame_info.pos.y + 3, 4 + 7, pv->frame_info.pos.y + 3 + 7, Palette_Refs[color_current]);
        rectfill(pv->box_gfx, 16, pv->frame_info.pos.y + 1, pv->frame_info.pos.x + pv->frame_info.size.x, pv->frame_info.pos.y + pv->frame_info.size.y, GUI_COL_FILL);
        Font_Print(F_SMALL, pv->box_gfx, buf, 16, pv->frame_info.pos.y + 1, GUI_COL_TEXT_IN_BOX);
        dirty = TRUE;
        pv->color_displayed = color_current;
    }

    if (dirty)
        pv->box->must_redraw = YES;
}

void    PaletteViewer_SetPaletteSize(t_app_palette_viewer *pv, int palette_size)
{
    pv->palette_size = palette_size;
    PaletteViewer_Update();
}

void    PaletteViewer_CallbackSelectColor(t_widget *w)
{
    t_app_palette_viewer *pv = &PaletteViewer;  // Global instance
    const int color_box_size = pv->box_gfx->w / pv->palette_size;

    if (w->mx != -1 && w->my != -1)
    {
        pv->color_selected = (w->mx / color_box_size);
    }
}

//-----------------------------------------------------------------------------
