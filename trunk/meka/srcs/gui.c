//-----------------------------------------------------------------------------
// MEKA - gui.h
// Graphical User Interface (GUI) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "app_game.h"
#include "g_tools.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_gui gui;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

DrawCursor::DrawCursor(v2i _pos, int font_id)
{
	this->pos = _pos;
	this->x_base = this->pos.x;
	this->y_spacing = Font_Height((t_font_id)font_id)+2;

	ALLEGRO_BITMAP* bitmap = al_get_target_bitmap();
	this->viewport_min.Set(0,0);
	this->viewport_max.Set(al_get_bitmap_width(bitmap),al_get_bitmap_height(bitmap));
}

void	DrawCursor::HorizontalSeparator()
{
	this->pos.y += 2;
	al_draw_line(viewport_min.x, this->pos.y+0.5f, viewport_max.x, this->pos.y+0.5f, COLOR_SKIN_WINDOW_SEPARATORS, 0);
	this->pos.y += 2;
}

void	DrawCursor::VerticalSeparator()
{
	this->pos.x += 2;
	al_draw_line(this->pos.x+0.5f, viewport_min.y, this->pos.x+0.5f, viewport_max.y, COLOR_SKIN_WINDOW_SEPARATORS, 0);
	this->pos.x += 2;
}

//-----------------------------------------------------------------------------

void    gui_redraw_everything_now_once (void)
{
    gui_update();
    gui_draw();
    Blit_GUI();
}

void    gui_draw_background()
{
	al_set_target_bitmap(gui_buffer);
	al_draw_bitmap(gui_background, 0, 0, 0x0000);
}

void	gui_draw()
{
    // If we were asked to redraw everything, redraw the background as well
    if (gui.info.must_redraw == TRUE)
        gui_draw_background();

	al_set_target_bitmap(gui_buffer);
    for (int i = gui.boxes_count - 1; i >= 0; i--)
    {
        t_gui_box* b = gui.boxes_z_ordered[i];
        const t_frame bb = b->frame;
		const v2i bb_min = bb.GetMin();
		const v2i bb_max = bb.GetMax();

        if (!(b->flags & GUI_BOX_FLAGS_ACTIVE))
            continue;

        // Draw widgets
        for (t_list* widgets = b->widgets; widgets != NULL; widgets = widgets->next)
        {
            t_widget *w = (t_widget *)widgets->elem;
            if (w->enabled && w->type != WIDGET_TYPE_CLOSEBOX)
                if (w->redraw_func != NULL)
                    w->redraw_func(w);
        }

        // Blit content
        switch (b->type)
        {
        case GUI_BOX_TYPE_STANDARD: 
			al_set_target_bitmap(gui_buffer);
			al_draw_bitmap_region(b->gfx_buffer, 0, 0, bb.size.x + 1, bb.size.y + 1, bb.pos.x, bb.pos.y, 0x0000);
            break;
		case GUI_BOX_TYPE_GAME: 
            gamebox_draw(b, screenbuffer);
            break;
        }

		// Draw borders
		al_set_target_bitmap(gui_buffer);
		gui_rect(LOOK_ROUND, bb.pos.x - 2, bb.pos.y - 20, bb.pos.x + bb.size.x + 2, bb.pos.y + bb.size.y + 2, COLOR_SKIN_WINDOW_BORDER);
		al_draw_line(bb.pos.x, bb.pos.y - 1.5f, bb.pos.x + bb.size.x + 1, bb.pos.y - 1.5f, COLOR_SKIN_WINDOW_BORDER, 0);
		al_draw_line(bb.pos.x, bb.pos.y - 0.5f, bb.pos.x + bb.size.x + 1, bb.pos.y - 0.5f, COLOR_SKIN_WINDOW_BORDER, 0);

		// Draw resize widget (invisible for game window)
		if (b->flags & GUI_BOX_FLAGS_ALLOW_RESIZE)
		{
			const bool is_resizing = (gui.mouse.focus == GUI_FOCUS_BOX && gui.mouse.focus_box == b && gui.mouse.focus_is_resizing);
			if (b->type != GUI_BOX_TYPE_GAME || is_resizing)
			{
				const int sz = 9; // display size is 9, interaction is 12
				const ALLEGRO_COLOR color = is_resizing ? COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT : COLOR_SKIN_WINDOW_TITLEBAR_TEXT_UNACTIVE;
				al_draw_filled_triangle(bb_max.x+2, bb_max.y+2, bb_max.x+2-sz, bb_max.y+2, bb_max.x+2, bb_max.y+2-sz, color);
			}
		}

		// Draw title bar
		{
			t_frame titlebar_frame;
			titlebar_frame.pos.x  = bb.pos.x;
			titlebar_frame.pos.y  = bb.pos.y - 18;
			titlebar_frame.size.x = bb.size.x;
			titlebar_frame.size.y = 15;
			SkinGradient_DrawHorizontal(&Skins_GetCurrentSkin()->gradient_window_titlebar, gui_buffer, &titlebar_frame);

			// Draw title bar text, with wrapping
			// Is window the focused one?
			const ALLEGRO_COLOR color = (i == 0) ? COLOR_SKIN_WINDOW_TITLEBAR_TEXT : COLOR_SKIN_WINDOW_TITLEBAR_TEXT_UNACTIVE;
			Font_SetCurrent(FONTID_LARGE);
			if (Font_TextWidth(FONTID_CUR, b->title) <= (bb.size.x - 8))
			{
				Font_Print (FONTID_CUR, b->title, bb.pos.x + 4, bb.pos.y - 17, color);
			}
			else
			{
				// FIXME-OPT: shit code.
				char title[256];
				int len = strlen(b->title);
				strcpy(title, b->title);
				while (Font_TextWidth(FONTID_CUR, title) > (bb.size.x - 17))
					title[--len] = EOSTR;
				strcat(title, "..");
				Font_Print(FONTID_CUR, title, bb.pos.x + 4, bb.pos.y - 17, color);
			}

			// Draw widgets
			for (t_list* widgets = b->widgets; widgets != NULL; widgets = widgets->next)
			{
				t_widget *w = (t_widget *)widgets->elem;
				if (w->enabled && w->type == WIDGET_TYPE_CLOSEBOX)
					if (w->redraw_func != NULL)
						w->redraw_func(w);
			}
		}
    }

    // Redraw menus on top of the desktop
    gui_redraw_menus();

    // Update applets that comes after the redraw
    gui_update_applets_after_redraw();

    // Clear global redrawing flag and makes mouse reappear
    gui.info.must_redraw = FALSE;
}

void    gui_relayout_all()
{
    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;
        box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
    }
}

//-----------------------------------------------------------------------------

