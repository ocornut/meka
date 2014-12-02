//-----------------------------------------------------------------------------
// MEKA - app_mapview.c
// TileMap Viewer - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_mapview.h"
#include "desktop.h"
#include "g_tools.h"
#include "g_widget.h"
#include "vdp.h"
#include "video_c.h"
#include "video_m2.h"
#include "video_m5.h"
#include "palette.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TILEMAP_VIEWER_PADDING   (3)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_tilemap_viewer *  TilemapViewer_MainInstance;
t_list *            TilemapViewers;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void			TilemapViewer_SetupLayoutSizes(t_tilemap_viewer *app);
static void         TilemapViewer_Layout(t_tilemap_viewer *app, bool setup);
static void         TilemapViewer_Update(t_tilemap_viewer *app);
static void			TilemapViewer_UpdateAddresses(t_tilemap_viewer *app);
static void         TilemapViewer_UpdateInfos(t_tilemap_viewer *app);
static void         TilemapViewer_UpdateScroll(t_tilemap_viewer *app);

static void         TilemapViewer_Switch(t_widget *w);
static void         TilemapViewer_CallbackTilemapClick(t_widget *w);
static void         TilemapViewer_CallbackTilemapAddressScroll(t_widget *w);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_tilemap_viewer *  TilemapViewer_New(bool register_desktop)
{
    t_tilemap_viewer* app = (t_tilemap_viewer*)malloc(sizeof(t_tilemap_viewer));

    // Add to global list
    list_add(&TilemapViewers, app);

    // Setup members
    app->active                     = FALSE;
    app->config_bg                  = TRUE;
    app->config_fg                  = TRUE;
    app->config_hflip               = TRUE;
    app->config_vflip               = TRUE;
    app->config_scroll              = TRUE;
    app->config_scroll_raster       = TRUE;
    app->config_tilemap_addr        = 0x3800;
    app->config_tilemap_addr_auto   = TRUE;
	app->config_tilemap_addr_manual_base_addr	= 0x0000;
	app->config_tilemap_addr_manual_step_size	= 0x0800;
	app->config_tilemap_addr_manual_step_count	= 8;
    app->tile_hovered               = -1;
    app->tile_selected              = 0;

	// Start with SMS/GG layout because it is the biggest and we can't resize a gui box above its initial size
	app->layout	= TILEMAP_VIEWER_LAYOUT_SMSGG;
	TilemapViewer_SetupLayoutSizes(app);

	// Create box
    app->frame_box.pos.x        = 524;
    app->frame_box.pos.y        = 52;
    app->box = gui_box_new(&app->frame_box, "Tilemap Viewer");  // FIXME-LOCALIZATION
    app->box->user_data = app;
    app->box->destroy = (t_gui_box_destroy_handler)TilemapViewer_Delete;

    // Register to desktop (applet is disabled by default)
    if (register_desktop)
        Desktop_Register_Box("TILEMAP_VIEWER", app->box, FALSE, &app->active);

    // Layout
    TilemapViewer_Layout(app, TRUE);

    // Return new instance
    return (app);
}

void	TilemapViewer_Delete(t_tilemap_viewer *app)
{
    // Remove from global list
    list_remove(&TilemapViewers, app);
    free(app);
}

void	TilemapViewer_ChangeLayout(t_tilemap_viewer *app, t_tilemap_viewer_layout layout)
{
	app->layout = layout;
	gui_box_destroy_widgets(app->box);
	TilemapViewer_SetupLayoutSizes(app);
	gui_box_resize(app->box, app->frame_box.size.x, app->frame_box.size.y, false);
	TilemapViewer_Layout(app, true);
}

void	TilemapViewer_SetupLayoutSizes(t_tilemap_viewer *app)
{
	switch (app->layout)
	{
	case TILEMAP_VIEWER_LAYOUT_SMSGG:
	case TILEMAP_VIEWER_LAYOUT_SMSGG_EXTRAHEIGHT:
		{
			app->frame_tilemap.pos.x        = TILEMAP_VIEWER_PADDING;
			app->frame_tilemap.pos.y        = TILEMAP_VIEWER_PADDING;
			app->frame_tilemap.size.x       = 256;
			app->frame_tilemap.size.y       = (app->layout == TILEMAP_VIEWER_LAYOUT_SMSGG_EXTRAHEIGHT) ? 256 : 224;

			app->frame_infos.pos.x          = TILEMAP_VIEWER_PADDING;
			app->frame_infos.pos.y          = app->frame_tilemap.pos.y + app->frame_tilemap.size.y + TILEMAP_VIEWER_PADDING;
			app->frame_infos.size.x         = 180 - TILEMAP_VIEWER_PADDING / 2;
			app->frame_infos.size.y         = 104;

			app->frame_config.pos.x         = app->frame_infos.pos.x + app->frame_infos.size.x + (TILEMAP_VIEWER_PADDING * 2);
			app->frame_config.pos.y         = app->frame_tilemap.pos.y + app->frame_tilemap.size.y + TILEMAP_VIEWER_PADDING;
			app->frame_config.size.x        = 76;
			app->frame_config.size.y        = 104;

			app->frame_tilemap_addr.pos.x   = TILEMAP_VIEWER_PADDING;
			app->frame_tilemap_addr.pos.y   = app->frame_infos.pos.y + app->frame_infos.size.y + TILEMAP_VIEWER_PADDING;
			app->frame_tilemap_addr.size.x  = 256;
			app->frame_tilemap_addr.size.y  = 16;

			// Box size (note that we don't touch pos here)
			app->frame_box.size.x			= app->frame_tilemap.size.x + (TILEMAP_VIEWER_PADDING * 2) - 1;
			app->frame_box.size.y			= app->frame_tilemap.size.y + app->frame_infos.size.y + app->frame_tilemap_addr.size.y + (TILEMAP_VIEWER_PADDING * 3) - 1;
			break;
		}
	case TILEMAP_VIEWER_LAYOUT_SGSC:
		{
			app->frame_tilemap.pos.x        = TILEMAP_VIEWER_PADDING;
			app->frame_tilemap.pos.y        = TILEMAP_VIEWER_PADDING;
			app->frame_tilemap.size.x       = 256;
			app->frame_tilemap.size.y       = 192;

			app->frame_infos.pos.x          = TILEMAP_VIEWER_PADDING;
			app->frame_infos.pos.y          = app->frame_tilemap.pos.y + app->frame_tilemap.size.y + TILEMAP_VIEWER_PADDING;
			app->frame_infos.size.x         = 180 + 76 - TILEMAP_VIEWER_PADDING / 2;
			app->frame_infos.size.y         = 90;//26;

			app->frame_config.pos.x         = app->frame_infos.pos.x + app->frame_infos.size.x + (TILEMAP_VIEWER_PADDING * 2);
			app->frame_config.pos.y         = app->frame_tilemap.pos.y + app->frame_tilemap.size.y + TILEMAP_VIEWER_PADDING;
			app->frame_config.size.x        = 0;//76;
			app->frame_config.size.y        = 90;//26;

			app->frame_tilemap_addr.pos.x   = TILEMAP_VIEWER_PADDING;
			app->frame_tilemap_addr.pos.y   = app->frame_infos.pos.y + app->frame_infos.size.y + TILEMAP_VIEWER_PADDING;
			app->frame_tilemap_addr.size.x  = 256;
			app->frame_tilemap_addr.size.y  = 16*1;

			// Box size (note that we don't touch pos here)
			app->frame_box.size.x			= app->frame_tilemap.size.x + (TILEMAP_VIEWER_PADDING * 2) - 1;
			app->frame_box.size.y			= app->frame_tilemap.size.y + app->frame_infos.size.y + app->frame_tilemap_addr.size.y + (TILEMAP_VIEWER_PADDING * 3) - 1;
			break;
		}
	default:
		assert(0);
	}
}

void         TilemapViewer_Layout(t_tilemap_viewer *app, bool setup)
{
    t_frame frame;

    al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    // Add closebox widget
    if (setup)
        widget_closebox_add(app->box, TilemapViewer_Switch);

    // Tilemap zone/button
    if (setup)
        app->frame_tilemap_zone = widget_button_add(app->box, &app->frame_tilemap, 1, TilemapViewer_CallbackTilemapClick, FONTID_NONE, NULL);

    // Horizontal line
    al_draw_line(0, app->frame_tilemap_addr.pos.y+0.5f, app->frame_config.pos.x + app->frame_config.size.x, app->frame_tilemap_addr.pos.y+0.5f, COLOR_SKIN_WINDOW_SEPARATORS, 0);

	if (app->layout == TILEMAP_VIEWER_LAYOUT_SMSGG || app->layout == TILEMAP_VIEWER_LAYOUT_SMSGG_EXTRAHEIGHT)
	{
		// Vertical line
		al_draw_line(app->frame_config.pos.x - TILEMAP_VIEWER_PADDING*2+1, app->frame_config.pos.y, app->frame_config.pos.x - TILEMAP_VIEWER_PADDING*2+1, app->frame_config.pos.y + app->frame_config.size.y, COLOR_SKIN_WINDOW_SEPARATORS, 0);

		// Config/Options
		frame.SetSize(10, 10);

		frame.pos.x = app->frame_config.pos.x + 2;
		frame.pos.y = app->frame_config.pos.y + 3;
		Font_Print(FONTID_MEDIUM, "Display", frame.pos.x, frame.pos.y + 2, COLOR_SKIN_WINDOW_TEXT);

		frame.pos.x  = app->frame_config.pos.x;
		frame.pos.y += frame.size.y + 3;
		if (setup)
			widget_checkbox_add(app->box, &frame, &app->config_bg, NULL); 
		frame.pos.x += frame.size.x + 6;
		Font_Print(FONTID_MEDIUM, "BG", frame.pos.x, frame.pos.y + 2, COLOR_SKIN_WINDOW_TEXT);

		frame.pos.x  = app->frame_config.pos.x;
		frame.pos.y += frame.size.y + 3;
		if (setup)
			widget_checkbox_add(app->box, &frame, &app->config_fg, NULL); 
		frame.pos.x += frame.size.x + 6;
		Font_Print(FONTID_MEDIUM, "FG", frame.pos.x, frame.pos.y + 2, COLOR_SKIN_WINDOW_TEXT);

		frame.pos.x  = app->frame_config.pos.x;
		frame.pos.y += frame.size.y + 3;
		if (setup)
			widget_checkbox_add(app->box, &frame, &app->config_hflip, NULL); 
		frame.pos.x += frame.size.x + 6;
		Font_Print(FONTID_MEDIUM, "H Flip", frame.pos.x, frame.pos.y + 2, COLOR_SKIN_WINDOW_TEXT);

		frame.pos.x  = app->frame_config.pos.x;
		frame.pos.y += frame.size.y + 3;
		if (setup)
			widget_checkbox_add(app->box, &frame, &app->config_vflip, NULL); 
		frame.pos.x += frame.size.x + 6;
		Font_Print(FONTID_MEDIUM, "V Flip", frame.pos.x, frame.pos.y + 2, COLOR_SKIN_WINDOW_TEXT);

		frame.pos.x  = app->frame_config.pos.x;
		frame.pos.y += frame.size.y + 3;
		if (setup)
			widget_checkbox_add(app->box, &frame, &app->config_scroll, NULL); 
		frame.pos.x += frame.size.x + 6;
		Font_Print(FONTID_MEDIUM, "Scroll", frame.pos.x, frame.pos.y + 2, COLOR_SKIN_WINDOW_TEXT);

		frame.pos.x  = app->frame_config.pos.x;
		frame.pos.y += frame.size.y + 3;
		if (setup)
			widget_checkbox_add(app->box, &frame, &app->config_scroll_raster, NULL); 
		frame.pos.x += frame.size.x + 6;
		Font_Print(FONTID_MEDIUM, "Raster", frame.pos.x, frame.pos.y + 2, COLOR_SKIN_WINDOW_TEXT);
	}

    // Tilemap Address - Scrollbar
    frame.SetPos(app->frame_tilemap_addr.pos.x + 136, app->frame_tilemap_addr.pos.y + 3 + 1);
    frame.SetSize(8 * 8, 8);
    if (setup)
    {
        app->widget_tilemap_addr_scrollbar_slot_cur = 0;
        app->widget_tilemap_addr_scrollbar = widget_scrollbar_add(app->box, WIDGET_SCROLLBAR_TYPE_HORIZONTAL, &frame, &app->config_tilemap_addr_manual_step_count, &app->widget_tilemap_addr_scrollbar_slot_cur, 1, TilemapViewer_CallbackTilemapAddressScroll);
    }

    // Tilemap Address - Auto Button
    frame.pos.x += frame.size.x + 8;
    frame.pos.y  = app->frame_tilemap_addr.pos.y + 3;
    frame.SetSize(10, 10);
    if (setup)
        app->widget_tilemap_addr_auto_checkbox = widget_checkbox_add(app->box, &frame, &app->config_tilemap_addr_auto, NULL); 
    frame.pos.x += frame.size.x + 6;
    Font_Print(FONTID_MEDIUM, "Auto", frame.pos.x, frame.pos.y + 2, COLOR_SKIN_WINDOW_TEXT);
}

void         TilemapViewer_CallbackTilemapClick(t_widget *w)
{
    t_tilemap_viewer *app = (t_tilemap_viewer *)w->box->user_data; // Get instance
    const int mx = app->frame_tilemap_zone->mouse_x;
    const int my = app->frame_tilemap_zone->mouse_y;
    if (app->frame_tilemap_zone->mouse_action & WIDGET_MOUSE_ACTION_HOVER)
        app->tile_selected = (mx / 8) + (my / 8) * 32;
}

void        TilemapViewer_CallbackTilemapAddressScroll(t_widget *w)
{
    t_tilemap_viewer *app = (t_tilemap_viewer *)w->box->user_data; // Get instance

    const int new_addr = app->config_tilemap_addr_manual_base_addr + (app->widget_tilemap_addr_scrollbar_slot_cur * app->config_tilemap_addr_manual_step_size);
    const int cur_addr = app->config_tilemap_addr;

    if (new_addr != cur_addr)
    {
        app->config_tilemap_addr = new_addr;
        app->config_tilemap_addr_auto = FALSE;
    }
}

void         TilemapViewer_Switch(t_widget *w)
{
    t_tilemap_viewer *app = (t_tilemap_viewer *)w->box->user_data; // Get instance
    if (app == TilemapViewer_MainInstance)
    {
        TilemapViewer_SwitchMainInstance();
    }
    else
    {
        app->active ^= 1;
        gui_box_show(app->box, app->active, TRUE);
        if (!app->active)
        {
            // Flag GUI box for deletion
            app->box->flags |= GUI_BOX_FLAGS_DELETE;
            return;
        }
    }
}

void         TilemapViewer_SwitchMainInstance(void)
{
    t_tilemap_viewer *app = TilemapViewer_MainInstance;
    app->active ^= 1;
    // FIXME-LOCAL
    //if (app->active ^= 1)
    //    Msg(MSGT_USER, Msg_Get(MSG_MemoryEditor_Enabled));
    //else
    //    Msg(MSGT_USER, Msg_Get(MSG_MemoryEditor_Disabled));
    gui_box_show(app->box, app->active, TRUE);
    gui_menu_toggle_check(menus_ID.tools, 3);
}

void         TilemapViewer_Update(t_tilemap_viewer *app)
{
    if (!app->active)
        return;

	// Full relayout when changing video mode
	t_tilemap_viewer_layout required_layout;
	if (tsms.VDP_VideoMode >= 4)
		required_layout = (Wide_Screen_28) ? TILEMAP_VIEWER_LAYOUT_SMSGG_EXTRAHEIGHT : TILEMAP_VIEWER_LAYOUT_SMSGG;
	else
		required_layout = TILEMAP_VIEWER_LAYOUT_SGSC;
	if (app->layout != required_layout)
		TilemapViewer_ChangeLayout(app, required_layout);

    // If skin has changed, redraw everything
    if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        TilemapViewer_Layout(app, FALSE);
        app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
    }

	TilemapViewer_UpdateAddresses(app);

    // Update tilemap
	al_set_target_bitmap(app->box->gfx_buffer);

	// FIXME: Video mode numbers
	if (tsms.VDP_VideoMode == 2)
	{
        gui_frame_clear(app->box->gfx_buffer, &app->frame_tilemap, COLOR_SKIN_WINDOW_BACKGROUND);
		ALLEGRO_LOCKED_REGION* locked_region = al_lock_bitmap(app->box->gfx_buffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);

		// Draw tilemap
		// TODO: pattern name addr slider, 0x0000 -> 0x3C00, 0x0400 increments
		// TODO: vsection mask (use proper name)
		// TODO: pattern data base addr
		// TODO: color table base addr
		// TODO: show info per tile: Index, X,Y, pattern_name
		const u8* pattern_name_table = VRAM + app->config_tilemap_addr;
		const int vsection_mask = sms.VDP[4] & 3;
		int y = 0;
		for (int vsection_idx = 0; vsection_idx < 3; vsection_idx++) // screen in 3 parts
		{
			const u8* tile_base = g_machine.VDP.sg_pattern_gen_address + ((vsection_idx & vsection_mask) * 0x800);	// Pattern data base
			const u8* col_base = g_machine.VDP.sg_color_table_address + ((vsection_idx & vsection_mask) * 0x800);	// Color table base
			for (int ty = 0; ty < 8; ty++)
			{
				int x = 0;
				for (int tx = 0; tx < 32; tx++)
				{
					const u32 pattern_name = (*pattern_name_table++) * 8;
					const u8* p1 = tile_base + pattern_name;		// Pattern data
					const u8* p2 = col_base  + pattern_name;		// Color table

					// Draw one tile
					const int dst_x = app->frame_tilemap.pos.x + x;
					const int dst_y = app->frame_tilemap.pos.y + y;
					VDP_Mode0123_DrawTile(app->box->gfx_buffer, locked_region, dst_x, dst_y, p1, p2);
					p1 += 8;
					p2 += 8;
					x += 8;
				}
				y += 8;
			}
		}

		al_unlock_bitmap(app->box->gfx_buffer);
	}
    else if (tsms.VDP_VideoMode >= 4)
    {
		ALLEGRO_LOCKED_REGION* locked_region = al_lock_bitmap(app->box->gfx_buffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);

		// Draw tilemap
        const u16 *map = (u16 *)(VRAM + app->config_tilemap_addr);
        for (int j = 0; j != app->frame_tilemap.size.y/8; j++)
        {
            const int dst_y = app->frame_tilemap.pos.y + (j << 3);
            for (int i = 0; i != app->frame_tilemap.size.x/8; i++)
            {
                const int   dst_x           = app->frame_tilemap.pos.x + (i << 3);
                const u16   map_item        = *map++;
                const int   tile_index      = (map_item & 0x01FF);
                const u32 * tile_palette    = (map_item & 0x0800) ? &Palette_EmulationToHostGui[16] : &Palette_EmulationToHostGui[0];
                const u8 *  tile_pixels     = (u8 *)&tgfx.Tile_Decoded[tile_index];
                int         tile_flip       = (map_item & 0x0600);

                if (!app->config_hflip)
                    tile_flip &= ~0x0200;
                if (!app->config_vflip)
                    tile_flip &= ~0x0400;

                if ((!app->config_bg && !(map_item & 0x1000)) || (!app->config_fg && (map_item & 0x1000)))
                {
                    alx_locked_draw_filled_rectangle(locked_region, dst_x, dst_y, dst_x + 8, dst_y + 8, COLOR_DEBUG_BACKDROP);
                }
                else
                {
                    // Decode tile if necessary
                    if (tgfx.Tile_Dirty[tile_index] & TILE_DIRTY_DECODE)
                    {
                        Decode_Tile(tile_index);
                        tgfx.Tile_Dirty[tile_index] = TILE_DIRTY_REDRAW;
                    }

                    // Draw
                    VDP_Mode4_DrawTile(app->box->gfx_buffer, locked_region, tile_pixels, tile_palette, dst_x, dst_y, tile_flip);
                }
            }
        }
		al_unlock_bitmap(app->box->gfx_buffer);
    }
	else
	{
        gui_frame_clear(app->box->gfx_buffer, &app->frame_tilemap, COLOR_SKIN_WINDOW_BACKGROUND);
        char text[64];
        sprintf(text, "Unsupported video mode: %d", tsms.VDP_VideoMode);
        Font_Print(FONTID_MEDIUM, text, app->frame_tilemap.pos.x + TILEMAP_VIEWER_PADDING, app->frame_tilemap.pos.y + TILEMAP_VIEWER_PADDING, COLOR_SKIN_WINDOW_TEXT);
	}

    // Tilemap rectangle
    al_draw_rectangle(
        app->frame_tilemap.pos.x - 0.5f, app->frame_tilemap.pos.y - 0.5f, 
        app->frame_tilemap.pos.x + app->frame_tilemap.size.x + 0.5f, app->frame_tilemap.pos.y + app->frame_tilemap.size.y + 0.5f,
        COLOR_SKIN_WIDGET_GENERIC_BORDER, 1.0f);

    // Update hovered index
    {
        const int mx = app->frame_tilemap_zone->mouse_x;
        const int my = app->frame_tilemap_zone->mouse_y;
        if (app->frame_tilemap_zone->mouse_action & WIDGET_MOUSE_ACTION_HOVER)
            app->tile_hovered = (mx / 8) + (my / 8) * 32;
        else
            app->tile_hovered = -1;
    }

    // Update hovered/selected tile information
	if (app->layout == TILEMAP_VIEWER_LAYOUT_SMSGG || app->layout == TILEMAP_VIEWER_LAYOUT_SMSGG_EXTRAHEIGHT)
    {
        if (app->config_scroll)
            TilemapViewer_UpdateScroll(app);
    }
    TilemapViewer_UpdateInfos(app);
}

static void		TilemapViewer_UpdateAddresses(t_tilemap_viewer *app)
{
    // Update tilemap address slot
	const int old_tilemap_addr_step_count = app->config_tilemap_addr_manual_step_count;
	if (tsms.VDP_VideoMode >= 4)
	{
		if (Wide_Screen_28)
		{
			app->config_tilemap_addr_manual_base_addr = 0x0700;
			app->config_tilemap_addr_manual_step_size = 0x1000;
			app->config_tilemap_addr_manual_step_count = 4;
			if (app->config_tilemap_addr_auto)
				app->widget_tilemap_addr_scrollbar_slot_cur = (sms.VDP[2] & 0x0C) >> 2;
		}
		else
		{
			app->config_tilemap_addr_manual_base_addr = 0x0000;
			app->config_tilemap_addr_manual_step_size = 0x0800;
			app->config_tilemap_addr_manual_step_count = 8;
			if (app->config_tilemap_addr_auto)
				app->widget_tilemap_addr_scrollbar_slot_cur = (sms.VDP[2] & 0x0E) >> 1;
		}
	}
	else
	{
		app->config_tilemap_addr_manual_base_addr = 0x0000;
		app->config_tilemap_addr_manual_step_size = 0x400;
		app->config_tilemap_addr_manual_step_count = 16;
		if (app->config_tilemap_addr_auto)
			app->widget_tilemap_addr_scrollbar_slot_cur = (sms.VDP[2] & 0x0F);
	}

	if (old_tilemap_addr_step_count != app->config_tilemap_addr_manual_step_count)
	{
		// Approximately remap selector
		app->widget_tilemap_addr_scrollbar_slot_cur = LinearRemapClamp(app->widget_tilemap_addr_scrollbar_slot_cur, 0, old_tilemap_addr_step_count-1, 0, app->config_tilemap_addr_manual_step_count-1);
	}

    // Update tilemap address
    app->config_tilemap_addr = app->config_tilemap_addr_manual_base_addr + (app->widget_tilemap_addr_scrollbar_slot_cur * app->config_tilemap_addr_manual_step_size);
}

static void     TilemapViewer_UpdateInfos(t_tilemap_viewer *app)
{
	const int tile_count = (app->frame_tilemap.size.x / 8) * (app->frame_tilemap.size.y / 8);
	if (app->tile_selected >= tile_count)
		app->tile_selected = 0;

    const int  tile_current = (app->tile_hovered == -1) ? app->tile_selected : app->tile_hovered;
    const int  tile_current_x = tile_current & 31;
    const int  tile_current_y = tile_current >> 5;

	al_set_target_bitmap(app->box->gfx_buffer);
    gui_frame_clear(app->box->gfx_buffer, &app->frame_infos, COLOR_SKIN_WINDOW_BACKGROUND);

	FontPrinter fp(FONTID_MEDIUM);
	if (app->layout == TILEMAP_VIEWER_LAYOUT_SMSGG || app->layout == TILEMAP_VIEWER_LAYOUT_SMSGG_EXTRAHEIGHT)
	{
		const u16* tile_map = (u16 *)(VRAM + app->config_tilemap_addr);
		const u16  tile_map_item = tile_map[tile_current];
		char       tile_map_items_bits[2][9];
		StrWriteBitfield((tile_map_item & 0xFF), 8, tile_map_items_bits[0]);
		StrWriteBitfield(((tile_map_item >> 8) & 0xFF), 8, tile_map_items_bits[1]);

		v2i pos = app->frame_infos.pos;
		pos.x += TILEMAP_VIEWER_PADDING;
		pos.y += TILEMAP_VIEWER_PADDING;
		const v2i vh = v2i(0, Font_Height(FONTID_MEDIUM)+2);

		fp.Printf(pos, "Index:     $%03X @ VRAM $%04X", tile_current, app->config_tilemap_addr + (tile_current * 2)); pos += vh;
		fp.Printf(pos, "X:         %d", tile_current_x); pos += vh;
		fp.Printf(pos, "Y:         %d", tile_current_y); pos += vh;

		fp.Printf(pos, "Name Data: $%04X", tile_map_item); pos += vh;
		fp.Printf(pos, "           %%%s.%s", tile_map_items_bits[1], tile_map_items_bits[0]); pos += vh;

		//fp.Printf(&pos, "            ___pcvhn.nnnnnnnn"); pos += vh;

		fp.Printf(pos, "Tile No:   $%03X", tile_map_item & 0x1FF); pos += vh;

		fp.Printf(pos, "H Flip:    %d", (tile_map_item & 0x200) ? 1 : 0); pos += vh;
		fp.Printf(pos, "V Flip:    %d", (tile_map_item & 0x400) ? 1 : 0); pos += vh;

		fp.Printf(pos, "Palette:   %d", (tile_map_item & 0x800) ? 1 : 0); pos += vh;
		fp.Printf(pos, "Priority:  %s", (tile_map_item & 0x1000) ? "FG" : "BG"); pos += vh;
	}

	if (app->layout == TILEMAP_VIEWER_LAYOUT_SGSC)
	{
		v2i pos = app->frame_infos.pos;
		pos.x += TILEMAP_VIEWER_PADDING;
		pos.y += TILEMAP_VIEWER_PADDING;
		const v2i vh = v2i(0, Font_Height(FONTID_MEDIUM)+2);

		fp.Printf(pos, "Index:   $%03X", tile_current); pos += vh;
		fp.Printf(pos, "X:       %d", tile_current_x); pos += vh;
		fp.Printf(pos, "Y:       %d", tile_current_y); pos += vh;

		const int vsection_mask = sms.VDP[4] & 3;
		const int vsection_idx = tile_current_y / 8;
		const u8 char_name = VRAM[(app->config_tilemap_addr + tile_current) & 0x3FFF];
		const u32 char_name_addr = char_name * 8;
		const u8* tile_base = g_machine.VDP.sg_pattern_gen_address + ((vsection_idx & vsection_mask) * 0x800);	// Pattern data base
		const u8* col_base = g_machine.VDP.sg_color_table_address + ((vsection_idx & vsection_mask) * 0x800);	// Color table base
		const u8* char_pattern_data = tile_base + char_name_addr;
		const u8* char_color_data = col_base  + char_name_addr;

		fp.Printf(pos, "Name:    1 byte @ VRAM $%04X", (app->config_tilemap_addr + tile_current) & 0x3FFF); pos += vh;
		fp.Printf(pos, "         $%02X", char_name); pos += vh;

		fp.Printf(pos, "Pattern: 8 bytes @ VRAM $%04X", char_pattern_data - VRAM); pos += vh;
		fp.Printf(pos, "         $%02X $%02X $%02X $%02X $%02X $%02X $%02X $%02X", char_pattern_data[0], char_pattern_data[1], char_pattern_data[2], char_pattern_data[3], char_pattern_data[4], char_pattern_data[5], char_pattern_data[6], char_pattern_data[7]); pos += vh;

		fp.Printf(pos, "Color:   8 bytes @ VRAM $%04X", char_color_data - VRAM); pos += vh;
		fp.Printf(pos, "         $%02X $%02X $%02X $%02X $%02X $%02X $%02X $%02X", char_color_data[0], char_color_data[1], char_color_data[2], char_color_data[3], char_color_data[4], char_color_data[5], char_color_data[6], char_color_data[7]); pos += vh;
	}

    al_draw_rectangle(
        app->frame_tilemap.pos.x+tile_current_x*8-0.5f, app->frame_tilemap.pos.y+tile_current_y*8-0.5f, 
        app->frame_tilemap.pos.x+tile_current_x*8+8.5f, app->frame_tilemap.pos.y+tile_current_y*8+8.5f, COLOR_WHITE, 1.0f);

    // Tilemap Address
    al_draw_filled_rectangle(
        app->frame_tilemap_addr.pos.x + 1, app->frame_tilemap_addr.pos.y + 1,
        app->frame_tilemap_addr.pos.x + 128 + 1, app->frame_tilemap_addr.pos.y + app->frame_tilemap_addr.size.x + 1,
        COLOR_SKIN_WINDOW_BACKGROUND);

	v2i pos = app->frame_tilemap_addr.pos + v2i(2,3+2);
    fp.Printf(pos, "Tilemap @ VRAM $%04X", app->config_tilemap_addr);
}

static void     TilemapViewer_UpdateScrollDrawLineWrap(t_tilemap_viewer *app, int y, int x1, int x2, ALLEGRO_COLOR color)
{
    x1 &= 255;
    x2 &= 255;
    if (Wide_Screen_28)
    {
		y &= 255;
    }
    else
    {
        if (y >= 224)
            y -= 224;
        else if (y < 0)
            y += 224;
    }
    if (x1 < x2)
    {
        al_draw_hline(
            app->frame_tilemap.pos.x + x1,
            app->frame_tilemap.pos.y + y,
            app->frame_tilemap.pos.x + x2,
            color);
    }
    else if (x1 > x2)
    {
        al_draw_hline(
            app->frame_tilemap.pos.x + x1,
            app->frame_tilemap.pos.y + y,
            app->frame_tilemap.pos.x + 255,
            color);
        al_draw_hline(
            app->frame_tilemap.pos.x + 0,
            app->frame_tilemap.pos.y + y,
            app->frame_tilemap.pos.x + x2,
            color);
    }
    else
    {
		// draw pixel is actually pixel than putpixel on a video memory buffer (no need to lock in/out)
        al_draw_pixel(app->frame_tilemap.pos.x + x1 + 0.5f, app->frame_tilemap.pos.y + y + 0.5f, color);
    }
}

static void     TilemapViewer_UpdateScroll(t_tilemap_viewer *app)
{
    const ALLEGRO_COLOR color_1 = COLOR_BLACK;//makecol(0x20, 0xFF, 0x40);
    const ALLEGRO_COLOR color_2 = COLOR_WHITE;//makecol(0x00, 0xC0, 0x00);
    const int drv_x1  = g_driver->x_start + ((g_driver->id == DRV_SMS && Mask_Left_8) ? 8 : 0);
    const int drv_x2  = g_driver->x_end;
    const int drv_y1  = g_driver->y_show_start;
    const int drv_y2  = g_driver->y_show_end;
    const int y_start = g_machine.VDP.scroll_y_latched;// + (Wide_Screen_28 ? -32 : 0);
    int i;

	al_set_target_bitmap(app->box->gfx_buffer);
    if (app->config_scroll_raster)
    {
        TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + drv_y1,     -g_machine.VDP.scroll_x_latched_table[drv_y1] + drv_x1, -g_machine.VDP.scroll_x_latched_table[drv_y1] + drv_x2, color_1);
        TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + drv_y1 + 1, -g_machine.VDP.scroll_x_latched_table[drv_y1] + drv_x1, -g_machine.VDP.scroll_x_latched_table[drv_y1] + drv_x2, color_2);
        TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + drv_y2 - 1, -g_machine.VDP.scroll_x_latched_table[drv_y2] + drv_x1, -g_machine.VDP.scroll_x_latched_table[drv_y2] + drv_x2, color_2);
        TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + drv_y2,     -g_machine.VDP.scroll_x_latched_table[drv_y2] + drv_x1, -g_machine.VDP.scroll_x_latched_table[drv_y2] + drv_x2, color_1);
        for (i = drv_y1+1; i < drv_y2; i++)
        {
            /*
            const int x1a = -g_machine.VDP.scroll_x_latched_table[i-1] + drv_x1;
            const int x1b = -g_machine.VDP.scroll_x_latched_table[i]   + drv_x1;
            const int x2a = -g_machine.VDP.scroll_x_latched_table[i-1] + drv_x2;
            const int x2b = -g_machine.VDP.scroll_x_latched_table[i]   + drv_x2;
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, MAX(x1a,x1b),   MAX(x1a,x1b),   color_1);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, MAX(x1a,x1b)+1, MAX(x1a,x1b)+1, color_2);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, MIN(x2a,x2b)-1, MIN(x2a,x2b)-1, color_2);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, MIN(x2a,x2b),   MIN(x2a,x2b),   color_1);
            */
            const int x1 = -g_machine.VDP.scroll_x_latched_table[i] + drv_x1;
            const int x2 = -g_machine.VDP.scroll_x_latched_table[i] + drv_x2;
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, x1,   x1,   color_1);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, x1+1, x1+1, color_2);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, x2-1, x2-1, color_2);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, x2,   x2,   color_1);
        }
    }
    else
    {
        TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + drv_y1,     ((Top_No_Scroll && drv_y1 < 16) ? 0 : -g_machine.VDP.scroll_x_latched) + drv_x1, ((Top_No_Scroll && drv_y1 < 16) ? 0 : -g_machine.VDP.scroll_x_latched) + drv_x2, color_1);
        TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + drv_y1 + 1, ((Top_No_Scroll && drv_y1 < 16) ? 0 : -g_machine.VDP.scroll_x_latched) + drv_x1, ((Top_No_Scroll && drv_y1 < 16) ? 0 : -g_machine.VDP.scroll_x_latched) + drv_x2, color_2);
        TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + drv_y2 - 1, -g_machine.VDP.scroll_x_latched + drv_x1, -g_machine.VDP.scroll_x_latched + drv_x2, color_2);
        TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + drv_y2,     -g_machine.VDP.scroll_x_latched + drv_x1, -g_machine.VDP.scroll_x_latched + drv_x2, color_1);
        for (i = drv_y1+1; i < drv_y2; i++)
        {
            const int x1 = ((Top_No_Scroll && i < 16) ? 0 : -g_machine.VDP.scroll_x_latched) + drv_x1;
            const int x2 = ((Top_No_Scroll && i < 16) ? 0 : -g_machine.VDP.scroll_x_latched) + drv_x2;
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, x1,   x1,   color_1);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, x1+1, x1+1, color_2);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, x2-1, x2-1, color_2);
            TilemapViewer_UpdateScrollDrawLineWrap(app, y_start + i, x2,   x2,   color_1);
        }
    }
}

//-----------------------------------------------------------------------------

void	TilemapViewers_Update(void)
{
    t_list *apps;
    for (apps = TilemapViewers; apps != NULL; apps = apps->next)
    {
        t_tilemap_viewer *app = (t_tilemap_viewer *)apps->elem;
        TilemapViewer_Update(app);
    }
}

//-----------------------------------------------------------------------------
