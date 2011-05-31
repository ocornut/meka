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

void    gui_redraw_everything_now_once (void)
{
    gui_update ();
    gui_redraw ();
    Blit_GUI ();
}

void    GUI_DrawBackground(void)
{
	al_set_target_bitmap(gui_buffer);
	al_draw_bitmap(gui_background, 0, 0, 0x0000);
}

// Redraw the screen for this frame
void	gui_redraw(void)
{
    // If we were asked to redraw everything, redraw the background as well
    if (gui.info.must_redraw == TRUE)
        GUI_DrawBackground();

    // For each box...
    for (int i = gui.boxes_count - 1; i >= 0; i--)
    {
        t_gui_box* b = gui.boxes_z_ordered[i];
        t_frame b_frame = b->frame;

        // Check if it's showing
        if (!(b->flags & GUI_BOX_FLAGS_ACTIVE))
            continue;

        // Check if it should be redrawn
        if (!(b->flags & GUI_BOX_FLAGS_DIRTY_REDRAW) && (!gui.info.must_redraw))
            continue;
        b->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW;

	    ALLEGRO_COLOR color;
        if (i == 0)
        {
            // Active/focused box
            color = COLOR_SKIN_WINDOW_TITLEBAR_TEXT;
        }
        else
        {
            // Non-active/focused box
            color = COLOR_SKIN_WINDOW_TITLEBAR_TEXT_UNACTIVE;

            // Check if it overlaps by other windows
            // FIXME: why isn't this check done for the active window ??
            for (int j = i - 1; j >= 0; j --)
            {
                t_gui_box *b2 = gui.boxes_z_ordered[j];
                if ((b2->frame.pos.x + b2->frame.size.x + 2  >=  b_frame.pos.x - 2)
                    && (b2->frame.pos.x - 2                     <=  b_frame.pos.x + b->frame.size.x + 2)
                    && (b2->frame.pos.y + b2->frame.size.y + 2  >=  b_frame.pos.y - 20)
                    && (b2->frame.pos.y - 20                    <=  b_frame.pos.y + b->frame.size.y + 2))
                {
                    b2->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
                }
            }
        }

        // Draw borders
        gui_rect (gui_buffer, LOOK_ROUND, b_frame.pos.x - 2, b_frame.pos.y - 20, b_frame.pos.x + b_frame.size.x + 2, b_frame.pos.y + b_frame.size.y + 2, COLOR_SKIN_WINDOW_BORDER);
		al_set_target_bitmap(gui_buffer);
        al_draw_line(b_frame.pos.x, b_frame.pos.y - 1.5f, b_frame.pos.x + b_frame.size.x + 1, b_frame.pos.y - 1.5f, COLOR_SKIN_WINDOW_BORDER, 0);
        al_draw_line(b_frame.pos.x, b_frame.pos.y - 0.5f, b_frame.pos.x + b_frame.size.x + 1, b_frame.pos.y - 0.5f, COLOR_SKIN_WINDOW_BORDER, 0);

        // Draw title bar.
		{
			t_frame titlebar_frame;
			titlebar_frame.pos.x  = b_frame.pos.x;
			titlebar_frame.pos.y  = b_frame.pos.y - 18;
			titlebar_frame.size.x = b_frame.size.x;
			titlebar_frame.size.y = 15;
			SkinGradient_DrawHorizontal(&Skins_GetCurrentSkin()->gradient_window_titlebar, gui_buffer, &titlebar_frame);
		}

        // Draw title bar text, with wrapping
        // FIXME: again, the algorythm below sucks. Drawn label should be precomputed anyway.
        Font_SetCurrent (F_LARGE);
        if (Font_TextLength (-1, b->title) <= (b_frame.size.x - 8))
        {
            Font_Print (-1, gui_buffer, b->title, b_frame.pos.x + 4, b_frame.pos.y - 17, color);
        }
        else
        {
            char title[256];
            int  len = strlen (b->title);
            strcpy (title, b->title);
            while (Font_TextLength (-1, title) > (b_frame.size.x - 17))
                title[--len] = EOSTR;
            strcat (title, "..");
            Font_Print (-1, gui_buffer, title, b_frame.pos.x + 4, b_frame.pos.y - 17, color);
        }

        // Redraw widgets
        if (b->widgets != NULL)
        {
            for (t_list* widgets = b->widgets; widgets != NULL; widgets = widgets->next)
            {
                t_widget *w = (t_widget *)widgets->elem;
                if (w->enabled)
                {
                    if (w->dirty)
                    {
                        w->dirty = FALSE;
                        if (w->redraw != NULL)
                            w->redraw(w);
                    }
                }
            }
        }

        // Blit content
        switch (b->type)
        {
        case GUI_BOX_TYPE_STANDARD: 
			al_set_target_bitmap(gui_buffer);
			al_draw_bitmap_region(b->gfx_buffer, 0, 0, b_frame.size.x + 1, b_frame.size.y + 1, b_frame.pos.x, b_frame.pos.y, 0x0000);
            break;
        case GUI_BOX_TYPE_GAME : 
            gamebox_draw(b, screenbuffer);
            // Always set dirty redraw flag
            b->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
            break;
        }
    }

    // Redraw menus on top of the desktop
    gui_redraw_menus();

    // Update applets that comes after the redraw
    // FIXME: ...
    gui_update_applets_after_redraw();

    // Clear global redrawing flag and makes mouse reappear
    gui.info.must_redraw = FALSE;
}

void	GUI_SetDirtyAll(void)
{
	for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
	{
		t_gui_box* box = (t_gui_box*)boxes->elem;
		gui_box_set_dirty(box);
	}
}

void    GUI_RelayoutAll(void)
{
    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;
        box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
        gui_box_set_dirty(box);
    }
}

//-----------------------------------------------------------------------------

