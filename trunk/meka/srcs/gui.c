//-----------------------------------------------------------------------------
// MEKA - gui.h
// Graphical User Interface (GUI) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "g_widget.h"

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
    int         color, which;
    t_gui_box * b;
    t_frame     b_frame;

    // Makes mouse disappear
    Show_Mouse_In (NULL);

    // If we were asked to redraw everything, redraw the background as well
    if (gui.info.must_redraw == YES)
        Redraw_Background ();

    // For each box...
    for (i = gui.box_last - 1; i >= 0; i --)
    {
        which = gui.box_plan [i];
        b = gui.box [which];
        b_frame = b->frame;

        // Check if it's showing
        if (!(b->attr & A_Show))
            continue;

        // Check if it should be redrawn
        if (b->must_redraw == NO && gui.info.must_redraw == NO)
            continue;
        b->must_redraw = NO;

        if (i == 0)
        {
            // Active/focused box
            color = GUI_COL_TEXT_ACTIVE;
        }
        else
        {
            // Non-active/focused box
            color = GUI_COL_TEXT_N_ACTIVE;

            // Check if it overlaps by other windows
            // FIXME: why isn't this check done for the active window ??
            for (j = i - 1; j >= 0; j --)
            {
                t_gui_box *b2 = gui.box[gui.box_plan[j]];
                if ((b2->frame.pos.x + b2->frame.size.x + 2  >=  b_frame.pos.x - 2)
                    && (b2->frame.pos.x - 2                     <=  b_frame.pos.x + b->frame.size.x + 2)
                    && (b2->frame.pos.y + b2->frame.size.y + 2  >=  b_frame.pos.y - 20)
                    && (b2->frame.pos.y - 20                    <=  b_frame.pos.y + b->frame.size.y + 2))
                {
                    b2->must_redraw = YES;
                }
            }
        }

        // Draw borders
        gui_rect (gui_buffer, LOOK_ROUND, b_frame.pos.x - 2, b_frame.pos.y - 20, b_frame.pos.x + b_frame.size.x + 2, b_frame.pos.y + b_frame.size.y + 2, GUI_COL_BORDERS);
        line (gui_buffer, b_frame.pos.x, b_frame.pos.y - 1, b_frame.pos.x + b_frame.size.x, b_frame.pos.y - 1, GUI_COL_BORDERS);
        line (gui_buffer, b_frame.pos.x, b_frame.pos.y - 2, b_frame.pos.x + b_frame.size.x, b_frame.pos.y - 2, GUI_COL_BORDERS);

        // Draw title bar...
        if (gui.info.bar_gradients)
        {
            // ...with gradients
            int t1 = (b_frame.size.x / gui.info.bar_gradients_ratio);
            int t2 = t1 / (GUI_COL_THEME_GRADIENTS_NUM - gui.info.bar_gradients_unused);
            int t3 = b_frame.pos.x + b_frame.size.x;
            for (j = GUI_COL_THEME_GRADIENTS_NUM - gui.info.bar_gradients_unused - 1; j > 0; j --, t3 -= t2)
            {
                rectfill (gui_buffer, t3 - t2, b_frame.pos.y - 18, t3, b_frame.pos.y - 3, GUI_COL_BARS + j);
            }
            rectfill (gui_buffer, b_frame.pos.x, b_frame.pos.y - 18, t3, b_frame.pos.y - 3, GUI_COL_BARS);
        }
        else
        {
            // ...without gradients
            rectfill (gui_buffer, b_frame.pos.x, b_frame.pos.y - 18, b_frame.pos.x + b_frame.size.x, b_frame.pos.y - 3, GUI_COL_BARS);
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

        // Ask for widgets to redraw
        if (b->n_widgets > 0)
        {
            for (j = 0; j < b->n_widgets; j ++)
            {
                t_widget *w = b->widgets[j];
                if (w->redraw && w->enabled)
                    w->redraw (w, b_frame.pos.x, b_frame.pos.y);
            }
        }

        // Blit content
        switch (gui.box [which]->type)
        {
        case GUI_BOX_TYPE_BITMAP : 
            blit (gui.box_image[which], gui_buffer, 0, 0, b_frame.pos.x, b_frame.pos.y, b_frame.size.x + 1, b_frame.size.y + 1);
            break;
        case GUI_BOX_TYPE_GAME : 
            gamebox_draw (which, b_frame.pos.x, b_frame.pos.y, screenbuffer);
            b->must_redraw = YES; // Always reset 'must_redraw' flag
            break;
        }
    }

    // Redraw menus on top of the desktop
    gui_redraw_menus ();

    // Update applets that comes after the redraw
    // FIXME: ...
    gui_update_applets_after_redraw ();

    // Clear global redrawing flag and makes mouse reappear
    gui.info.must_redraw = NO;
    Show_Mouse_In (gui_buffer);
}

//-----------------------------------------------------------------------------

