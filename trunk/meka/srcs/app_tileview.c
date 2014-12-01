//-----------------------------------------------------------------------------
// MEKA - tileview.c
// Tile Viewer - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_tileview.h"
#include "desktop.h"
#include "g_tools.h"
#include "g_widget.h"
#include "palette.h"
#include "video_c.h"
#include "video_m2.h"
#include "video_m5.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_app_tile_viewer   TileViewer;

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

void    TileViewer_Layout(t_app_tile_viewer *app, bool setup);

void    TileViewer_Change_Palette(void);
void    TileViewer_SelectedTile_Select(t_widget *w);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TileViewer_Init_Values (void)
{
    TileViewer.active               = FALSE;
    TileViewer.dirty                = TRUE;
    TileViewer.palette              = 0;
    TileViewer.palette_max          = 2;
    TileViewer.tile_displayed       = -1;
    TileViewer.tile_hovered         = -1;
    TileViewer.tile_selected        = -1;
    TileViewer.tiles_per_page       = 512;
    TileViewer.tiles_width          = -1;
    TileViewer.tiles_height         = -1;
    TileViewer.tiles_display_zone   = NULL;
}

// CREATE AND INITIALIZE TILE VIEWER APPLET ----------------------------------
void    TileViewer_Init (void)
{
    t_app_tile_viewer *app = &TileViewer; // Global instance

    // Setup members
    app->tiles_width  = 16;
    app->tiles_height = app->tiles_per_page / app->tiles_width;
    app->tiles_display_frame.SetPos(0, 13);
    app->tiles_display_frame.SetSize(app->tiles_width * 8, app->tiles_height * 8);

	app->tile_selected_frame.SetPos(2, app->tiles_display_frame.GetMax().y+1);
	app->tile_selected_frame.SetSize(8,8);

	app->vram_addr_tms9918_current = 0;

    // Create box
    t_frame frame;
    frame.pos.x     = 16;
    frame.pos.y     = 65;
    frame.size.x    = app->tiles_display_frame.size.x - 1;
    frame.size.y    = app->tiles_display_frame.size.y + 13 + 13 - 1;
    app->box = gui_box_new(&frame, Msg_Get(MSG_TilesViewer_BoxTitle));
    Desktop_Register_Box ("TILES", app->box, true, &app->active);

    // Layout
    TileViewer_Layout(&TileViewer, TRUE);
}

void    TileViewer_Layout(t_app_tile_viewer *app, bool setup)
{
    al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    if (setup)
    {
        // Add closebox widget
        widget_closebox_add(app->box, (t_widget_callback)TileViewer_Switch);

        // Create invisible buttons for hovering/selecting palette
        app->tiles_display_zone = widget_button_add(app->box, &app->tiles_display_frame, 1, TileViewer_SelectedTile_Select, FONTID_NONE, NULL);
        widget_button_add(app->box, &app->tiles_display_frame, 2, (t_widget_callback)TileViewer_Change_Palette, FONTID_NONE, NULL);
    }

	if (setup)
	{
		t_frame frame;
		frame.SetPos(100, 2);
		frame.SetSize(4*6, 7);
		static const int step_count = 4;
		app->vram_addr_tms9918_scrollbar = widget_scrollbar_add(app->box, WIDGET_SCROLLBAR_TYPE_HORIZONTAL, &frame, &step_count, &app->vram_addr_tms9918_current, 1, NULL);
	}

    // Separators
    al_draw_line(0, app->tiles_display_frame.pos.y-1+0.5f, app->tiles_display_frame.size.x, app->tiles_display_frame.pos.y-1+0.5f, COLOR_SKIN_WINDOW_SEPARATORS, 0);
    al_draw_line(0, 13+app->tiles_display_frame.size.y+0.5f, app->tiles_display_frame.size.x, 13+app->tiles_display_frame.size.y+0.5f, COLOR_SKIN_WINDOW_SEPARATORS, 0);

	// Rectangle enclosing current/selected tile
	const t_frame* fr = &app->tile_selected_frame;
    gui_rect(LOOK_THIN, fr->pos.x, fr->pos.y, fr->pos.x + 11, fr->pos.y + 11, COLOR_SKIN_WIDGET_GENERIC_BORDER);
}

void    TileViewer_Update(t_app_tile_viewer *app)
{
    ALLEGRO_BITMAP *bmp = app->box->gfx_buffer;

    // Skip update if not active
    if (!app->active)
        return;

    // If skin has changed, redraw everything
    if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        TileViewer_Layout(app, FALSE);
        app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
        app->dirty = TRUE;
    }

    bool dirty_all = app->dirty || Palette_EmulationDirtyAny;
    bool dirty = dirty_all;

    // Update hovered tile index
    {
        const int mx = app->tiles_display_zone->mouse_x;
        const int my = app->tiles_display_zone->mouse_y;
        // Msg(MSGT_USER, "mx = %d, my = %d", mx, my);
        if (app->tiles_display_zone->mouse_action & WIDGET_MOUSE_ACTION_HOVER)
            app->tile_hovered = ((my / 8) * 16) + mx / 8;
        else
            app->tile_hovered = -1;
    }

    // Compute the tile that is to display in the bottom info line
    int tile_current = (app->tile_hovered != -1) ? app->tile_hovered : app->tile_selected;
    bool tile_current_refresh = /*(tile_current == -1) ? FALSE : */ (((tile_current != app->tile_displayed) || dirty_all || tgfx.Tile_Dirty [tile_current]));
    int tile_current_addr = -1;
	
	const v2i tiles_frame_pos = app->tiles_display_frame.pos;
	const v2i tile_selected_pos = v2i(app->tile_selected_frame.pos.x + 2, app->tile_selected_frame.pos.y + 2);

	int vram_addr_min = 0x0000;
	int vram_addr_size = 0;
	int vram_tile_size = 1;

    // Then redraw all tiles
	ALLEGRO_LOCKED_REGION* locked_region = al_lock_bitmap(app->box->gfx_buffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    switch (g_driver->vdp)
    {
    case VDP_SMSGG:
        {
			widget_set_enabled(app->vram_addr_tms9918_scrollbar, false);
			vram_addr_min = 0;
			vram_addr_size = 0x4000;
			vram_tile_size = 32;

            int n = 0;
            const u8 *    nd = &tgfx.Tile_Decoded[0][0];
            const u32 *   palette_host = app->palette ? &Palette_EmulationToHostGui[16] : &Palette_EmulationToHostGui[0];
            for (int y = 0; y != app->tiles_height; y++)
			{
                for (int x = 0; x != app->tiles_width; x++)
                {
                    if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
                        Decode_Tile (n);
                    if (dirty_all || tgfx.Tile_Dirty [n])
                    {
                        VDP_Mode4_DrawTile(app->box->gfx_buffer, locked_region, nd, palette_host, tiles_frame_pos.x+(x * 8), tiles_frame_pos.y+(y * 8), 0);
                        tgfx.Tile_Dirty [n] = 0;
                        dirty = TRUE;
                    }
                    if (n == tile_current)
					{
                        tile_current_addr = vram_addr_min + (n * 32);
						VDP_Mode4_DrawTile(app->box->gfx_buffer, locked_region, nd, palette_host, tile_selected_pos.x, tile_selected_pos.y, 0);
					}
                    n ++;
                    nd += 64;
                }
			}
            break;
        }
    case VDP_TMS9918:
        {
			widget_set_enabled(app->vram_addr_tms9918_scrollbar, true);
			vram_addr_min = 0x0000 + app->vram_addr_tms9918_current*0x1000;
			vram_addr_size = 0x1000;
			vram_tile_size = 8;

			const int fg_color = Palette_EmulationToHostGui[app->palette + 1];
            const int bg_color = Palette_EmulationToHostGui[(app->palette != 0) ? 1 : 15];
            const u8 * addr = VRAM + vram_addr_min;
			//VRAM = g_machine.VDP.sg_pattern_gen_address;
            // addr = &VRAM[apps.opt.Tiles_Base];
           
            int n = 0;
            for (int y = 0; y != app->tiles_height; y ++)
			{
                for (int x = 0; x != app->tiles_width; x ++)
                {
                    if ((addr - VRAM) > 0x4000)
                        break;
                    VDP_Mode0123_DrawTile(bmp, locked_region, tiles_frame_pos.x+(x * 8), tiles_frame_pos.y+(y * 8), addr, fg_color, bg_color);
                    if (n == tile_current)
					{
                        tile_current_addr = vram_addr_min + (n * 8);
						VDP_Mode0123_DrawTile(bmp, locked_region, tile_selected_pos.x, tile_selected_pos.y, addr, fg_color, bg_color);
					}

                    n++;
                    addr += 8;
                }
			}
            dirty = TRUE; // to be replaced later
            break;
        }
    }
	al_unlock_bitmap(app->box->gfx_buffer);

	// Refresh top status line (address range)
	al_set_target_bitmap(bmp);
	{
		// FIXME-OPT
		const int y = -1;
		al_draw_filled_rectangle(0, y + 1, app->vram_addr_tms9918_scrollbar->enabled ? app->vram_addr_tms9918_scrollbar->frame.pos.x-1 : 128-1, y + 11+1, COLOR_SKIN_WINDOW_BACKGROUND);

		char buf[64];
		sprintf(buf, "Range: $%04X-$%04X", vram_addr_min, vram_addr_min+vram_addr_size-1);
		Font_Print(FONTID_SMALL, buf, 0, y + 1, COLOR_SKIN_WINDOW_TEXT);
		dirty = true;
	}

    // Refresh bottom status line (selected tile)
    if (dirty_all || tile_current_refresh)
    {
		const int y = app->tiles_display_frame.GetMax().y;

        al_draw_filled_rectangle(16, y + 1, 127+1, y + 11+1, COLOR_SKIN_WINDOW_BACKGROUND);
        dirty = TRUE;

        if (tile_current != -1)
        {
            // Description
            char addr[16];
            if (tile_current_addr != -1)
                sprintf(addr, "$%04X", tile_current_addr);
            else
                sprintf(addr, "????");

			char buf[128];
			const int tile_index = tile_current_addr / vram_tile_size;
            sprintf(buf, Msg_Get(MSG_TilesViewer_Tile), tile_index, tile_index, addr);
            Font_Print(FONTID_SMALL, buf, 16, y + 1, COLOR_SKIN_WINDOW_TEXT);
            app->tile_displayed = tile_current;
        }
        else
        {
            // Fill tile with black
			const t_frame* fr = &app->tile_selected_frame;
            al_draw_filled_rectangle(fr->pos.x+2, fr->pos.y+2, fr->pos.x+2+8, fr->pos.y+2+8, COLOR_BLACK);
        }
    }

    if (dirty_all || dirty)
        app->dirty = FALSE;
}

void    TileViewer_Change_Palette (void)
{
    //int   i;
    TileViewer.palette = (TileViewer.palette + 1) % TileViewer.palette_max;
    //for (i = 0; i < MAX_TILES; i++)
    //    tgfx.Tile_Dirty [i] |= TILE_DIRTY_REDRAW;
    TileViewer.dirty = TRUE;
}

void        TileViewer_Configure_PaletteMax (int palette_max)
{
    if (palette_max == TileViewer.palette_max)
        return;
    TileViewer.palette_max = palette_max;
    if (TileViewer.palette >= palette_max)
        TileViewer.palette %= palette_max;
    TileViewer.dirty = TRUE;
}

void    TileViewer_SelectedTile_Select (t_widget *w)
{
    if (w->mouse_action & WIDGET_MOUSE_ACTION_HOVER)
        TileViewer.tile_selected = ((w->mouse_y / 8) * 16) + (w->mouse_x / 8);
}

void    TileViewer_Switch (void)
{
    if (TileViewer.active ^= 1)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_TilesViewer_Enabled));
    else
        Msg(MSGT_USER, "%s", Msg_Get(MSG_TilesViewer_Disabled));
    gui_box_show (TileViewer.box, TileViewer.active, TRUE);
    gui_menu_toggle_check (menus_ID.tools, 2);
}

//-----------------------------------------------------------------------------
