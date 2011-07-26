//-----------------------------------------------------------------------------
// MEKA - g_box.c
// GUI Boxes - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------
// Functions (crap, horrible, painful)
//-----------------------------------------------------------------------------

// FIXME: That was a stupid macro!
#define Limit(a,b)              (((b) > (a)) ? (a) : (b))

void        gui_update_boxes(void)
{
    int         i;
    bool        will_move = FALSE;
    t_gui_box * b = NULL;
    t_gui_box * b_hover = NULL;

    if (gui.mouse.focus == GUI_FOCUS_DESKTOP || gui.mouse.focus == GUI_FOCUS_MENUS)
        return;

    // Update widgets, and find on which box the mouse cursor is
    for (i = 0; i < gui.boxes_count; i++)
    {
        int mouse_x;
        int mouse_y;

        b = gui.boxes_z_ordered[i];

        // Skip invisible boxes
        if (!(b->flags & GUI_BOX_FLAGS_ACTIVE))
            continue;

        mouse_x = gui.mouse.x - b->frame.pos.x;
        mouse_y = gui.mouse.y - b->frame.pos.y;
        will_move = widgets_update_box(b, mouse_x, mouse_y);

        if (b_hover == NULL)
        {
            if ((gui_is_mouse_hovering_area(b->frame.pos.x - 2, b->frame.pos.y - 20, b->frame.pos.x + b->frame.size.x + 2, b->frame.pos.y + b->frame.size.y + 2))
                ||
                // FIXME-FOCUS
                ((gui.mouse.focus == GUI_FOCUS_BOX && gui.mouse.focus_item == b) && (gui.mouse.buttons & 1)))
            {
                b_hover = b;
                break;
            }
        }
    }
    if (b_hover == NULL)
    {
        // FIXME-FOCUS
        //if ((gui.mouse.buttons) && (gui_mouse.pressed_on == PRESSED_ON_NOTHING))
        //    gui_mouse.pressed_on = PRESSED_ON_DESKTOP;
        if ((gui.mouse.buttons) && (gui.mouse.focus == GUI_FOCUS_NONE))
        {
            gui.mouse.focus = GUI_FOCUS_DESKTOP;
            gui.mouse.focus_item = NULL;
        }
        return;
    }

    // FIXME-FOCUS
    //if ((gui_mouse.on_box != NULL) && (gui_mouse.on_box != b_hover))
    if (gui.mouse.focus == GUI_FOCUS_BOX && gui.mouse.focus_item != b_hover)
        return;

    /*
    will_move = TRUE;
    if ((b->n_widgets > 0) && (gui_mouse.pressed_on != PRESSED_ON_BOX))
        will_move = widgets_update_box(b, cx, cy);
    */

    if ((gui.mouse.buttons & 1) == 0)
        return;

    // FIXME-FOCUS
    //gui_mouse.on_box = b;
    gui_box_set_focus(b);
    b->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
    // gui.info.must_redraw = TRUE;

    //Msg(MSGT_DEBUG, "will_move=%d", will_move);

    // ELSE, MOVE THE BOX --------------------------------------------------------
    if ((will_move) && (gui.mouse.focus != GUI_FOCUS_WIDGET) && (gui.mouse.buttons & 1))
    {
        int mx, my;
        int ax1, ay1, ax2, ay2;
        int bx1, by1, bx2, by2;

        // FIXME-FOCUS
        //if (gui_mouse.pressed_on != PRESSED_ON_BOX)
        if (gui.mouse.focus != GUI_FOCUS_BOX)
        {
            // FIXME-FOCUS
            //gui_mouse.pressed_on = PRESSED_ON_BOX;
            gui.mouse.focus = GUI_FOCUS_BOX;
            gui.mouse.focus_item = b;
            mx = 0;
            my = 0;
        }
        else
        {
            mx = gui.mouse.x - gui.mouse.x_prev;
            my = gui.mouse.y - gui.mouse.y_prev;
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

		al_set_target_bitmap(gui_buffer);
		al_draw_bitmap_region(gui_background, ax1, ay1, ax2 - ax1, ay2 - ay1, ax1, ay1, 0x0000);
		al_draw_bitmap_region(gui_background, bx1, by1, bx2 - bx1, by2 - by1, bx1, by1, 0x0000);
        // blit (color_buffer, gui_buffer, ax1, ay1, ax1, ay1, ax2 - ax1, ay2 - ay1);
        // blit (color2_buffer, gui_buffer, bx1, by1, bx1, by1, bx2 - bx1, by2 - by1);
        // blit (gui_buffer, screen, 0, 0, 0, 0, 640, 480);

        // Update 'must_redraw' flag for other boxes
        {
            for (int j = i + 1; j < gui.boxes_count; j++)
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
    } // Move Box -------------------------------------------------------------
}

t_gui_box *	    gui_box_new(const t_frame *frame, const char *title)
{
	// Allocate new box
	t_gui_box* box = (t_gui_box *)malloc(sizeof (t_gui_box));
	assert(box != NULL);

    // Setup members
    box->frame      = *frame;
    box->title      = strdup(title);
    box->type       = GUI_BOX_TYPE_STANDARD;
    box->flags      = GUI_BOX_FLAGS_ACTIVE | GUI_BOX_FLAGS_DIRTY_REDRAW | GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
	box->gfx_buffer = NULL;
	gui_box_create_video_buffer(box);
    box->widgets    = NULL;
    box->user_data  = NULL;
    box->update     = NULL;
    box->destroy    = NULL;

    // Clear GFX buffer
	al_set_target_bitmap(box->gfx_buffer);
    //clear_to_color(box->gfx_buffer, COLOR_SKIN_WINDOW_BACKGROUND);
    al_clear_to_color(al_map_rgb_f(1.0f, 0.0f, 1.0f));

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

void	gui_box_create_video_buffer(t_gui_box *box)
{
	if (box->gfx_buffer)
		al_destroy_bitmap(box->gfx_buffer);

	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	al_set_new_bitmap_format(g_Configuration.video_gui_format_request);
	box->gfx_buffer = al_create_bitmap(box->frame.size.x+1, box->frame.size.y+1);

	// Redraw and layout all
	box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW | GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
}

void    gui_box_delete(t_gui_box *box)
{
    // Destroy applet callback
    if (box->destroy != NULL)
        box->destroy(box->user_data);

    // Remove from lists
    list_remove(&gui.boxes, box);
    for (int i = 0; i < gui.boxes_count; i++)
	{
        if (gui.boxes_z_ordered[i] == box)
        {
            for (; i < gui.boxes_count - 1; i++)
                gui.boxes_z_ordered[i] = gui.boxes_z_ordered[i + 1];
            break;
        }
	}
    gui.boxes_z_ordered[gui.boxes_count - 1] = NULL;
    gui.boxes_count--;

	// Delete widgets
	list_free_custom(&box->widgets, (t_list_free_handler)widget_destroy);

	// Delete
    al_destroy_bitmap(box->gfx_buffer);
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
    for (t_list* widgets = box->widgets; widgets != NULL; widgets = widgets->next)
    {
        t_widget *w = (t_widget *)widgets->elem;
        w->dirty = TRUE;
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
            for (int n = 1; n < gui.boxes_count; n++)
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

int		gui_box_find_z(const t_gui_box* box)
{
    for (int i = 0; i < gui.boxes_count; i++)
	{
        if (gui.boxes_z_ordered[i] == box)
			return i;
	}
	return -1;
}

// Set focus to given box
// FIXME: ARGH!
void	gui_box_set_focus(t_gui_box *box)
{
    if (gui_box_has_focus(box))
        return;

    gui_box_set_dirty(box);
    gui_box_set_dirty(gui.boxes_z_ordered[0]);

    // Shift
    t_gui_box* box_prev = box;
    for (int i = 0; i != gui.boxes_count; i++)
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

// Return weither given box has the focus
int     gui_box_has_focus(t_gui_box *box)
{
    return (gui.boxes_z_ordered[0] == box);
}

// Set title of given box
void    gui_box_set_title(t_gui_box *box, const char *title)
{
    free(box->title);
    box->title = strdup(title);
}

void	gui_box_resize(t_gui_box *box, int size_x, int size_y)
{
	box->frame.size.x = size_x;
	box->frame.size.y = size_y;
	box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
	gui.info.must_redraw = TRUE;
}

// Clip position of given box so that it shows on desktop.
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
