//-----------------------------------------------------------------------------
// MEKA - g_box.c
// GUI Boxes - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------
// Macros (useless)
//-----------------------------------------------------------------------------

#define qblit(b1, b2, sx, sy, lx, ly) blit(b1, b2, sx, sy, sx, sy, lx, ly);

//-----------------------------------------------------------------------------
// Functions (crap, horrible, painful)
//-----------------------------------------------------------------------------

// CHECK IF USER DO SOMETHING TO A BOX ----------------------------------------
void        gui_update_boxes(void)
{
    int         i;
    bool        will_move = FALSE;
    t_gui_box * b = NULL;
    t_gui_box * b_hover = NULL;

    if ((gui_mouse.pressed_on == PRESSED_ON_DESKTOP) || (gui_mouse.pressed_on == PRESSED_ON_MENUS))
        return;

    // FIND ON WHICH BOX IS THE MOUSE CURSOR --------------------------------------
    for (i = 0; i < gui.boxes_count; i++)
    {
        int     mouse_x;
        int     mouse_y;

        b = gui.boxes_z_ordered[i];
        if (!(b->flags & GUI_BOX_FLAGS_ACTIVE)) // Skip invisible boxes
            continue;

        mouse_x = gui_mouse.x - b->frame.pos.x;
        mouse_y = gui_mouse.y - b->frame.pos.y;
        will_move = widgets_update_box(b, mouse_x, mouse_y);

        if (b_hover == NULL)
        {
            if ((gui_mouse_area(b->frame.pos.x - 2, b->frame.pos.y - 20, b->frame.pos.x + b->frame.size.x + 2, b->frame.pos.y + b->frame.size.y + 2))
                ||
                ((gui_mouse.on_box == b) && (gui_mouse.button & 1)))
            {
                b_hover = b;
                break;
            }
        }
    }
    if (b_hover == NULL)
    {
        if ((gui_mouse.button) && (gui_mouse.pressed_on == PRESSED_ON_NOTHING))
            gui_mouse.pressed_on = PRESSED_ON_DESKTOP;
        return;
    }

    if ((gui_mouse.on_box != NULL) && (gui_mouse.on_box != b_hover))
        return;

    /*
    will_move = TRUE;
    if ((b->n_widgets > 0) && (gui_mouse.pressed_on != PRESSED_ON_BOX))
        will_move = widgets_update_box(b, cx, cy);
    */

    if ((gui_mouse.button & 1) == 0)
        return;

    gui_mouse.on_box = b;
    gui_box_set_focus(b);
    b->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
    // gui.info.must_redraw = TRUE;

    //Msg(MSGT_DEBUG, "will_move=%d", will_move);

    // ELSE, MOVE THE BOX --------------------------------------------------------
    if ((will_move) && (gui_mouse.pressed_on != PRESSED_ON_WIDGET) && (gui_mouse.button & 1))
    {
        int mx, my;
        int ax1, ay1, ax2, ay2;
        int bx1, by1, bx2, by2;

        Show_Mouse_In (NULL);

        if (gui_mouse.pressed_on != PRESSED_ON_BOX)
        {
            gui_mouse.pressed_on = PRESSED_ON_BOX;
            mx = my = 0;
        }
        else
        {
            mx = gui_mouse.x - gui_mouse.px;
            my = gui_mouse.y - gui_mouse.py;
            // if ((!mx) && (!my)) continue;
        }

        if (mx >= 0)
        {
            if (my >= 0)
            { // mx > 0 - my > 0
                ax1 = b->frame.pos.x                                          -  2 ;
                ay1 = b->frame.pos.y                                          - 20 ;
                ax2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
                ay2 = b->frame.pos.y + Limit(b->frame.size.y + 22, my) + 1    - 20 ;

                bx1 = b->frame.pos.x                                          -  2 ;
                by1 = b->frame.pos.y + Limit(b->frame.size.y + 22, my) + 1    - 20 ;
                bx2 = b->frame.pos.x + Limit(b->frame.size.x, mx) + 1         +  2 ;
                by2 = b->frame.pos.y + b->frame.size.y + 1                    +  2 ;
            }
            else
            { // mx > 0 - my < 0
                ax1 = b->frame.pos.x                                          -  2 ;
                ay1 = b->frame.pos.y                                          - 20 ;
                ax2 = b->frame.pos.x + Limit(b->frame.size.x, mx) + 1         +  2 ;
                ay2 = b->frame.pos.y + b->frame.size.y - Limit(b->frame.size.y + 22, -my)  +  2 ;

                bx1 = b->frame.pos.x                                          -  2 ;
                by1 = b->frame.pos.y + b->frame.size.y - Limit(b->frame.size.y + 22, -my)  +  2 ;
                bx2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
                by2 = b->frame.pos.y + b->frame.size.y + 1                    +  2 ;
            }
        }
        else
        {
            if (my >= 0)
            { // mx < 0 - my > 0
                ax1 = b->frame.pos.x                                          -  2 ;
                ay1 = b->frame.pos.y                                          - 20 ;
                ax2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
                ay2 = b->frame.pos.y + Limit(b->frame.size.y + 22, my) + 1    - 20 ;

                bx1 = b->frame.pos.x + b->frame.size.x - Limit(b->frame.size.x, -mx)       -  2 ;
                by1 = b->frame.pos.y + Limit(b->frame.size.y + 22, my) + 1    - 20 ;
                bx2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
                by2 = b->frame.pos.y + b->frame.size.y + 1                    +  2 ;
            }
            else
            { // mx < 0 - my < 0
                ax1 = b->frame.pos.x + b->frame.size.x - Limit(b->frame.size.x, -mx)       -  2 ;
                ax2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
                ay1 = b->frame.pos.y                                          - 20 ;
                ay2 = b->frame.pos.y + b->frame.size.y - Limit(b->frame.size.y + 22, -my)  +  2 ;

                bx1 = b->frame.pos.x                                   -  2 ;
                by1 = b->frame.pos.y + b->frame.size.y - Limit(b->frame.size.y + 22, -my)  +  2 ;
                bx2 = b->frame.pos.x + b->frame.size.x + 1                       +  2 ;
                by2 = b->frame.pos.y + b->frame.size.y + 1                       +  2 ;
            }
        }

        qblit(gui_background, gui_buffer, ax1, ay1, ax2 - ax1, ay2 - ay1);
        qblit(gui_background, gui_buffer, bx1, by1, bx2 - bx1, by2 - by1);
        // qblit (color_buffer, gui_buffer, ax1, ay1, ax2 - ax1, ay2 - ay1);
        // qblit (color2_buffer, gui_buffer, bx1, by1, bx2 - bx1, by2 - by1);
        // qblit (gui_buffer, screen, 0, 0, 640, 480);

        // Update 'must_redraw' flag for other boxes
        {
            int j;
            for (j = i + 1; j < gui.boxes_count; j++)
            {
                t_gui_box *b2 = gui.boxes_z_ordered[j];
                if (((b2->frame.pos.x + b2->frame.size.x + 2 >= ax1) && (b2->frame.pos.x - 2 <= ax2) && (b2->frame.pos.y + b2->frame.size.y + 2 >= ay1) && (b2->frame.pos.y - 20 <= ay2))
                    ||
                    ((b2->frame.pos.x + b2->frame.size.x + 2 >= bx1) && (b2->frame.pos.x - 2 <= bx2) && (b2->frame.pos.y + b2->frame.size.y + 2 >= by1) && (b2->frame.pos.y - 20 <= by2)))
                {
                    b2->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
                }
            }
        }

        // Update position
        b->frame.pos.x += mx;
        b->frame.pos.y += my;
        gui_box_clip_position (b);

        Show_Mouse_In (gui_buffer);
    } // Move Box -------------------------------------------------------------
}

t_gui_box *	    gui_box_new(const t_frame *frame, const char *title)
{
	t_gui_box * box;

	// Allocate new box
	box = (t_gui_box *)malloc(sizeof (t_gui_box));
	assert(box != NULL);

    // Setup members
    box->frame      = *frame;
    box->title      = strdup(title);
    box->type       = GUI_BOX_TYPE_STANDARD;
    box->flags      = GUI_BOX_FLAGS_ACTIVE | GUI_BOX_FLAGS_DIRTY_REDRAW | GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
	box->gfx_buffer = create_bitmap(box->frame.size.x+1, box->frame.size.y+1);
    box->widgets    = NULL;
    box->user_data  = NULL;
    box->update     = NULL;
    box->destroy    = NULL;

    // Clear GFX buffer
    //clear_to_color(box->gfx_buffer, COLOR_SKIN_WINDOW_BACKGROUND);
    clear_to_color(box->gfx_buffer, makecol(0xFF, 0x00, 0xFF));

    // Setup GUI global data
    list_add(&gui.boxes, box);
    gui.boxes_z_ordered[gui.boxes_count] = box;
    gui.boxes_count++;
    gui.info.must_redraw = TRUE;

    // Set focus
    // Note: be sure to call this after gui.box_last++
    gui_box_set_focus(box);
    gui_box_clip_position(box);

    return (box);
}

void    gui_box_delete(t_gui_box *box)
{
    int i;

    // Destroy applet callback
    if (box->destroy != NULL)
        box->destroy(box->user_data);

    // Remove from lists
    list_remove(&gui.boxes, box);
    for (i = 0; i < gui.boxes_count; i++)
        if (gui.boxes_z_ordered[i] == box)
        {
            for (; i < gui.boxes_count - 1; i++)
                gui.boxes_z_ordered[i] = gui.boxes_z_ordered[i + 1];
            break;
        }
    gui.boxes_z_ordered[gui.boxes_count - 1] = NULL;
    gui.boxes_count--;

    // Delete
    // FIXME: Delete widgets, memory leak here
    destroy_bitmap(box->gfx_buffer);
    box->gfx_buffer = NULL;
    free(box->title);
    box->title = NULL;
    free(box);
}

void    gui_box_set_dirty(t_gui_box *box)
{
    // Set main dirty flag
    box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    // Set all widgets as dirty
    {
        t_list *widgets;
        for (widgets = box->widgets; widgets != NULL; widgets = widgets->next)
        {
            t_widget *w = (t_widget *)widgets->elem;
            w->dirty = TRUE;
        }
    }
}

//-----------------------------------------------------------------------------
// gui_box_show (t_gui_box *box, bool enable, bool focus)
// Enable/disable given box
//-----------------------------------------------------------------------------
void            gui_box_show(t_gui_box *box, bool enable, bool focus)
{
    if (enable)
    {
        // Show box
        box->flags |= GUI_BOX_FLAGS_ACTIVE;

        // Set dirty
        gui_box_set_dirty(box);

        // Set focus
        if (focus)
            gui_box_set_focus(box);
    }
    else
    {
        // Hide box
        box->flags &= ~GUI_BOX_FLAGS_ACTIVE;

        // If this box had the focus, let give focus to the following one
        if (focus && gui_box_has_focus(box))
        {
            int n;
            for (n = 1; n < gui.boxes_count; n++)
            {
                t_gui_box * box_behind = gui.boxes_z_ordered[n];
                if (box_behind->flags & GUI_BOX_FLAGS_ACTIVE)
                {
                    gui_box_set_focus(box_behind);
                    break;
                }
            }
        }
    }

    // Set global redraw flag
    gui.info.must_redraw = TRUE;
}

//-----------------------------------------------------------------------------
// gui_box_set_focus (t_gui_box *)
// Set focus to given box
//-----------------------------------------------------------------------------
// FIXME: ARGH!
//-----------------------------------------------------------------------------
void            gui_box_set_focus (t_gui_box *box)
{
    int         i;
    t_gui_box * box_prev;

    if (gui_box_has_focus(box))
        return;

    gui_box_set_dirty(box);
    gui_box_set_dirty(gui.boxes_z_ordered[0]);

    // Shift
    box_prev = box;
    for (i = 0; i != gui.boxes_count; i++)
    {
        t_gui_box *box_curr = gui.boxes_z_ordered[i];
        gui.boxes_z_ordered[i] = box_prev;
        if (box_curr == box)
            break;
        box_prev = box_curr;
    }

    /*
    t_gui_box * box_current_focus;
    int         i;
    int         box_plan;

    box_plan = -1;
    for (i = 0; i < gui.box_last; i++)
        if (gui.box[gui.box_plan[i]] == box)
        {
            box_plan = i;
            break;
        }

    box_plan = gui_box_find_plan (box);
    box_current_focus = gui.box_z_ordered[0];
    if (box_current_focus == box)
        return;

    // Set redraw flag for old focused box
    box_current_focus->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    // Shift plan/z buffer by one
    for (i = box_plan; i > 0; i--)
        gui.box_plan[i] = gui.box_plan[i - 1];
    gui.box_plan[0] = box->stupid_id;
    */
}

//-----------------------------------------------------------------------------
// gui_box_has_focus(t_gui_box *)
// Return wether given box has the focus
//-----------------------------------------------------------------------------
int     gui_box_has_focus(t_gui_box *box)
{
    return (gui.boxes_z_ordered[0] == box);
}

//-----------------------------------------------------------------------------
// gui_box_set_title(t_gui_box *, char *)
// Set title of given box
//-----------------------------------------------------------------------------
void    gui_box_set_title(t_gui_box *box, const char *title)
{
    free(box->title);
    box->title = strdup(title);
}

//-----------------------------------------------------------------------------
// gui_box_clip_position (t_gui_box *box)
// Clip position of given box so that it shows on desktop.
//-----------------------------------------------------------------------------
void    gui_box_clip_position (t_gui_box *box)
{
    if (box->frame.pos.x < gui.info.screen_pad.x - box->frame.size.x)
        box->frame.pos.x = (gui.info.screen_pad.x - box->frame.size.x);
    if (box->frame.pos.x > gui.info.screen.x - gui.info.screen_pad.x)
        box->frame.pos.x = (gui.info.screen.x - gui.info.screen_pad.x);
    if (box->frame.pos.y < gui.info.screen_pad.y - box->frame.size.y + gui.info.bars_height)
        box->frame.pos.y = (gui.info.screen_pad.y - box->frame.size.y + gui.info.bars_height);
    if (box->frame.pos.y > gui.info.screen.y - gui.info.screen_pad.y - gui.info.bars_height)
        box->frame.pos.y = (gui.info.screen.y - gui.info.screen_pad.y - gui.info.bars_height);
}

//-----------------------------------------------------------------------------
