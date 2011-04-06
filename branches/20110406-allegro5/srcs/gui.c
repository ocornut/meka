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
// Functions
//-----------------------------------------------------------------------------

void    gui_redraw_everything_now_once (void)
{
    gui_update ();
    gui_redraw ();
    Blit_GUI ();
}

// REDRAW GUI BACKGROUND ------------------------------------------------------
void    Redraw_Background (void) // gui_blit() ?
{
    blit (gui_background, gui_buffer, 0, 0, 0, 0, gui.info.screen.x, gui.info.screen.y);
}

//-----------------------------------------------------------------------------
// gui_redraw ()
// Redraw the screen for this frame
//-----------------------------------------------------------------------------
void            gui_redraw (void)
{
    int         i, j;
    ALLEGRO_COLOR color;
    t_gui_box * b;
    t_frame     b_frame;

    // Makes mouse disappear
    gui_mouse_show (NULL);

    // If we were asked to redraw everything, redraw the background as well
    if (gui.info.must_redraw == TRUE)
        Redraw_Background ();

    // For each box...
    for (i = gui.boxes_count - 1; i >= 0; i--)
    {
        b = gui.boxes_z_ordered[i];
        b_frame = b->frame;

        // Check if it's showing
        if (!(b->flags & GUI_BOX_FLAGS_ACTIVE))
            continue;

        // Check if it should be redrawn
        if (!(b->flags & GUI_BOX_FLAGS_DIRTY_REDRAW) && (!gui.info.must_redraw))
            continue;
        b->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW;

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
            for (j = i - 1; j >= 0; j --)
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
        line (gui_buffer, b_frame.pos.x, b_frame.pos.y - 1, b_frame.pos.x + b_frame.size.x, b_frame.pos.y - 1, COLOR_SKIN_WINDOW_BORDER);
        line (gui_buffer, b_frame.pos.x, b_frame.pos.y - 2, b_frame.pos.x + b_frame.size.x, b_frame.pos.y - 2, COLOR_SKIN_WINDOW_BORDER);

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
            t_list *widgets;
            for (widgets = b->widgets; widgets != NULL; widgets = widgets->next)
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
			blit (b->gfx_buffer, gui_buffer, 0, 0, b_frame.pos.x, b_frame.pos.y, b_frame.size.x + 1, b_frame.size.y + 1);
            break;
        case GUI_BOX_TYPE_GAME : 
            gamebox_draw(b, screenbuffer);
            // Always set dirty redraw flag
            b->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
            break;
        }
    }

    // Redraw menus on top of the desktop
    gui_redraw_menus ();

    // Update applets that comes after the redraw
    // FIXME: ...
    gui_update_applets_after_redraw ();

    // Clear global redrawing flag and makes mouse reappear
    gui.info.must_redraw = FALSE;
    gui_mouse_show (gui_buffer);
}

void    gui_relayout(void)
{
    t_list *boxes;
    for (boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box *box = boxes->elem;
        box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
        gui_box_set_dirty(box);
    }
}

//-----------------------------------------------------------------------------

