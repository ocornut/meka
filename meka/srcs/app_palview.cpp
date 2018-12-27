//-----------------------------------------------------------------------------
// MEKA - app_palview.c
// Palette Viewer - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_palview.h"
#include "desktop.h"
#include "g_tools.h"
#include "g_widget.h"
#include "palette.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_app_palette_viewer    PaletteViewer;

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static void    PaletteViewer_Layout(t_app_palette_viewer *app, bool setup);
static void    PaletteViewer_CallbackSelectColor(t_widget *w);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    PaletteViewer_Init()
{
	t_app_palette_viewer *pv = &PaletteViewer;

    // Setup
    pv->active          = TRUE;
    pv->dirty           = TRUE;
    pv->palette_size    = 32; // Note: PaletteViewer_SetPaletteSize() is called after setting up the window
    pv->color_displayed = -1;
    pv->color_hovered   = -1;
    pv->color_selected  = -1;

    // Create box
    t_frame frame;
    frame.pos.x     = 163;
    frame.pos.y     = 53;
    frame.size.x    = 192-1;
    frame.size.y    = 49+13;
    pv->box = gui_box_new(&frame, Msg_Get(MSG_Palette_BoxTitle));
	pv->box->flags |= GUI_BOX_FLAGS_ALLOW_RESIZE;
	pv->box->size_min.x = 32*2-1;//frame.size.x;
	pv->box->size_min.y = 24;
	pv->box->size_max.x = 32*16-1;//frame.size.x;
	pv->box->size_max.y = 88;
	pv->box->size_step.x = 32;
	pv->box->size_step.y = 1;

    // Register to desktop (applet is enabled by default)
    Desktop_Register_Box ("PALETTE", pv->box, TRUE, &pv->active);

    // Layout
    PaletteViewer_Layout(pv, TRUE);

    // Configuration
    PaletteViewer_SetPaletteSize(pv, g_driver->colors);
}

void    PaletteViewer_Layout(t_app_palette_viewer* app, bool setup)
{
    al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    if (setup)
        widget_closebox_add(app->box, (t_widget_callback)PaletteViewer_Switch);

	// Setup frames
	app->frame_palette.pos.x     = 0;
	app->frame_palette.pos.y     = 0;
	app->frame_palette.size.x    = app->box->frame.size.x;
	app->frame_palette.size.y    = app->box->frame.size.y - 13;
	app->frame_info.pos.x        = 0;
	app->frame_info.pos.y        = app->frame_palette.size.y;
	app->frame_info.size.x       = app->box->frame.size.x;
	app->frame_info.size.y       = 13;

	if (setup)
		app->frame_palette_zone      = widget_button_add(app->box, &app->frame_palette, 1, PaletteViewer_CallbackSelectColor, FONTID_NONE, NULL);
	else
		app->frame_palette_zone->frame = app->frame_palette;

    // Draw separator
    al_draw_line(app->frame_info.pos.x, app->frame_info.pos.y+0.5f, app->frame_info.pos.x + app->frame_info.size.x+1, app->frame_info.pos.y+0.5f, COLOR_SKIN_WINDOW_SEPARATORS, 0);

    // Draw current color square
	gui_rect(LOOK_THIN, 2, app->frame_info.pos.y + 1, 2 + 11, app->frame_info.pos.y + 1 + 11, COLOR_SKIN_WIDGET_GENERIC_BORDER);
}

void    PaletteViewer_Switch()
{
    t_app_palette_viewer *pv = &PaletteViewer;  // Global instance

    if (pv->active ^= 1)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Palette_Enabled));
    else
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Palette_Disabled));
    gui_box_show (pv->box, pv->active, TRUE);
    gui_menu_toggle_check (menus_ID.tools, 1);
}

// Note: has to be executed after tileviewer::update
void    PaletteViewer_Update()
{
    t_app_palette_viewer *pv = &PaletteViewer;  // Global instance
	ALLEGRO_BITMAP* box_gfx = pv->box->gfx_buffer;

    // Skip update if not active
    if (!pv->active)
        return;

	int         i;
    bool        dirty = FALSE;
    const int   color_box_size = al_get_bitmap_width(box_gfx) / pv->palette_size;
    int         color_current;
    bool        color_current_refresh;

    // If skin has changed, redraw everything
    if (pv->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        PaletteViewer_Layout(pv, FALSE);
        pv->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
        pv->dirty = TRUE;
    }

    // Update hovered color index
    {
        const int mx = pv->frame_palette_zone->mouse_x;
        //const int my = pv->frame_palette_zone->mouse_y;
        if (pv->frame_palette_zone->mouse_action & WIDGET_MOUSE_ACTION_HOVER)
            pv->color_hovered = (mx / color_box_size);
        else
            pv->color_hovered = -1;
    }

    color_current = (pv->color_hovered != -1) ? pv->color_hovered : pv->color_selected;
    color_current_refresh = /*(color_current == -1) ? TRUE :*/ ((color_current != pv->color_displayed) ? TRUE : FALSE);

    // Draw palette
	al_set_target_bitmap(box_gfx);
    for (i = 0; i != pv->palette_size; i++)
    {
        if (pv->dirty || Palette_EmulationFlags[i] & PALETTE_EMULATION_FLAGS_DIRTY)
        {
            al_draw_filled_rectangle(
                (i * color_box_size), 0, 
                (i * color_box_size) + color_box_size, pv->frame_palette.size.y,
                Palette_Emulation[i]);
            Palette_EmulationFlags[i] &= ~PALETTE_EMULATION_FLAGS_DIRTY;
            dirty = TRUE;
            if (i == color_current)
                color_current_refresh = TRUE;
        }
    }

    if (pv->dirty || color_current_refresh)
    {
        al_draw_filled_rectangle(16, pv->frame_info.pos.y + 1, pv->frame_info.pos.x + pv->frame_info.size.x + 1, pv->frame_info.pos.y + pv->frame_info.size.y + 1, COLOR_SKIN_WINDOW_BACKGROUND);
        dirty = TRUE;

        if (color_current != -1)
        {
            char buf[64];
            char color_bits[20];

            // Color square
			al_draw_filled_rectangle(4, pv->frame_info.pos.y + 3, 4 + 8, pv->frame_info.pos.y + 3 + 8, Palette_Emulation[color_current]);
    
            // Description
            sprintf(buf, "Color %d ($%02X)", color_current, color_current);
            switch (g_driver->id)
            {
                case DRV_SMS:
                    color_bits[0] = '%';
                    StrWriteBitfield(PRAM[color_current], 8, color_bits + 1);
                    sprintf(buf+strlen(buf), " - %s", color_bits);
                    break;
                case DRV_GG:
                    color_bits[0] = '%';
                    StrWriteBitfield(PRAM[color_current * 2 + 1], 8, color_bits + 1);
                    color_bits[9] = '.';
                    StrWriteBitfield(PRAM[color_current * 2 + 0], 8, color_bits + 10);
                    sprintf(buf+strlen(buf), " - %s", color_bits);
                    break;
                default:
                    color_bits[0] = 0;
                    break;
            }
            Font_Print(FONTID_SMALL, buf, 16, pv->frame_info.pos.y + 1, COLOR_SKIN_WINDOW_TEXT);
            dirty = TRUE;
            pv->color_displayed = color_current;
        }
        else
        {
            // Fill with black
            al_draw_filled_rectangle(4, pv->frame_info.pos.y + 3, 4 + 8, pv->frame_info.pos.y + 3 + 8, COLOR_BLACK);
        }
    }

    if (pv->dirty || dirty)
        pv->dirty = FALSE;
}

void    PaletteViewer_SetPaletteSize(t_app_palette_viewer *pv, int palette_size)
{
    pv->palette_size = palette_size;
}

void    PaletteViewer_CallbackSelectColor(t_widget *w)
{
    t_app_palette_viewer *pv = &PaletteViewer;  // Global instance
    const int color_box_size = al_get_bitmap_width(pv->box->gfx_buffer) / pv->palette_size;

    if (w->mouse_action & WIDGET_MOUSE_ACTION_HOVER)
        pv->color_selected = (w->mouse_x / color_box_size);
}

//-----------------------------------------------------------------------------
