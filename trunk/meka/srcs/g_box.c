//-----------------------------------------------------------------------------
// MEKA - g_box.c
// GUI Boxes - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------
// Functions (anything in this file is particularly bad code)
//-----------------------------------------------------------------------------

void	gui_update_boxes()
{
    if (gui.mouse.focus == GUI_FOCUS_DESKTOP || gui.mouse.focus == GUI_FOCUS_MENUS)
        return;

    // Update widgets, and find on which box the mouse cursor is
    t_gui_box * b_hover = NULL;
    bool no_widgets = false;
	bool do_move = false;
	bool do_resize = false;

    for (int i = 0; i < gui.boxes_count; i++)
    {
		t_gui_box* b = gui.boxes_z_ordered[i];
        if (!(b->flags & GUI_BOX_FLAGS_ACTIVE))
            continue;

        const int mouse_x = gui.mouse.x - b->frame.pos.x;
        const int mouse_y = gui.mouse.y - b->frame.pos.y;
        no_widgets = !widgets_update_box(b, mouse_x, mouse_y);

        if (b_hover == NULL)
        {
			// FIXME-FOCUS
			// Already has focus?
			if ((gui.mouse.focus == GUI_FOCUS_BOX && gui.mouse.focus_box == b) && (gui.mouse.buttons & 1))
			{
				b_hover = b;
				do_move = gui.mouse.focus_is_resizing == false;
				do_resize = gui.mouse.focus_is_resizing == true;
				break;
			}
			
			const bool hovering_window = gui_is_mouse_hovering_area(b->frame.pos.x - 2, b->frame.pos.y - 20, b->frame.pos.x + b->frame.size.x + 2, b->frame.pos.y + b->frame.size.y + 2);
            if (hovering_window)
            {
				do_move = true;
				if ((b->flags & GUI_BOX_FLAGS_ALLOW_RESIZE) != 0)
				{
					t_frame resize_bb;
					resize_bb.pos.x = b->frame.pos.x + b->frame.size.x - 12;
					resize_bb.pos.y = b->frame.pos.y + b->frame.size.y - 12;
					resize_bb.size.x = resize_bb.size.y = 14;
					if (gui_is_mouse_hovering_area(&resize_bb))
					{
						do_move = false;
						do_resize = true;
					}
				}

				// When using light phaser, paddle control, graphic board, etc. which require
				if (b->type == GUI_BOX_TYPE_GAME && Inputs.Peripheral[0] != INPUT_JOYPAD)
					if (gui_is_mouse_hovering_area(&b->frame))
						do_move = do_resize = false;

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
            gui.mouse.focus_box = NULL;
			gui.mouse.focus_widget = NULL;
        }
        return;
    }

    // FIXME-FOCUS
    //if ((gui_mouse.on_box != NULL) && (gui_mouse.on_box != b_hover))
    if (gui.mouse.focus == GUI_FOCUS_BOX && gui.mouse.focus_box != b_hover)
        return;

    /*
    will_move = TRUE;
    if ((b->n_widgets > 0) && (gui_mouse.pressed_on != PRESSED_ON_BOX))
        will_move = !widgets_update_box(b, cx, cy);
    */

    if ((gui.mouse.buttons & 1) == 0)
        return;

    // Focus
    //gui_mouse.on_box = b;
	t_gui_box* b;
	if (gui.mouse.focus == GUI_FOCUS_BOX || gui.mouse.focus == GUI_FOCUS_WIDGET)
		b = gui.mouse.focus_box;
	else
		b = b_hover;

    gui_box_set_focus(b);

    // Move/resize
    if ((do_move || do_resize) && (gui.mouse.focus != GUI_FOCUS_WIDGET) && (gui.mouse.buttons & 1))
    {
        // FIXME-FOCUS
        //if (gui_mouse.pressed_on != PRESSED_ON_BOX)
        if (gui.mouse.focus != GUI_FOCUS_BOX)
        {
            // FIXME-FOCUS
            //gui_mouse.pressed_on = PRESSED_ON_BOX;
            gui.mouse.focus = GUI_FOCUS_BOX;
            gui.mouse.focus_box = b;
			gui.mouse.focus_widget = NULL;
			gui.mouse.focus_is_resizing = do_resize ? true : false;
			gui.mouse.focus_pivot.x = gui.mouse.x - b->frame.pos.x;
			gui.mouse.focus_pivot.y = gui.mouse.y - b->frame.pos.y;
			if (gui.mouse.focus_is_resizing)
			{
				// Lower-right pivot for resizing
				gui.mouse.focus_pivot.x -= b->frame.size.x;
				gui.mouse.focus_pivot.y -= b->frame.size.y;
			}
        }

		assert((t_gui_box*)gui.mouse.focus_box == b);

		// Mouse moves box
		// FIXME: rewrite this embarrassing 1998 code into something decent
		if (gui.mouse.focus_is_resizing)
		{
			// Resize
			const int sx = (gui.mouse.x - gui.mouse.focus_pivot.x) - b->frame.pos.x;
			const int sy = (gui.mouse.y - gui.mouse.focus_pivot.y) - b->frame.pos.y;
			gui_box_resize(b, sx, sy, true);
		}
		else
		{
			// Move
			const int mx = (gui.mouse.x - gui.mouse.focus_pivot.x) - b->frame.pos.x;
			const int my = (gui.mouse.y - gui.mouse.focus_pivot.y) - b->frame.pos.y;

			if (mx || my)
			{
				int ax1, ay1, ax2, ay2;
				int bx1, by1, bx2, by2;
				if (mx >= 0)
				{
					ax1 = b->frame.pos.x  -2;
					ay1 = b->frame.pos.y - 20;

					if (my >= 0)
					{ // mx > 0 - my > 0
						ax2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
						ay2 = b->frame.pos.y + MIN(b->frame.size.y + 22, my) + 1    - 20 ;

						bx1 = b->frame.pos.x                                          -  2 ;
						by1 = b->frame.pos.y + MIN(b->frame.size.y + 22, my) + 1    - 20 ;
						bx2 = b->frame.pos.x + MIN(b->frame.size.x, mx) + 1         +  2 ;
						by2 = b->frame.pos.y + b->frame.size.y + 1                    +  2 ;
					}
					else
					{ // mx > 0 - my < 0
						ax2 = b->frame.pos.x + MIN(b->frame.size.x, mx) + 1         +  2 ;
						ay2 = b->frame.pos.y + b->frame.size.y - MIN(b->frame.size.y + 22, -my)  +  2 ;

						bx1 = b->frame.pos.x                                          -  2 ;
						by1 = b->frame.pos.y + b->frame.size.y - MIN(b->frame.size.y + 22, -my)  +  2 ;
						bx2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
						by2 = b->frame.pos.y + b->frame.size.y + 1                    +  2 ;
					}
				}
				else
				{
					if (my >= 0)
					{ // mx < 0 - my > 0
						ax1 = b->frame.pos.x -  2 ;
						ay1 = b->frame.pos.y - 20 ;
						ax2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
						ay2 = b->frame.pos.y + MIN(b->frame.size.y + 22, my) + 1    - 20 ;

						bx1 = b->frame.pos.x + b->frame.size.x - MIN(b->frame.size.x, -mx)       -  2 ;
						by1 = b->frame.pos.y + MIN(b->frame.size.y + 22, my) + 1    - 20 ;
						bx2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
						by2 = b->frame.pos.y + b->frame.size.y + 1                    +  2 ;
					}
					else
					{ // mx < 0 - my < 0
						ax1 = b->frame.pos.x + b->frame.size.x - MIN(b->frame.size.x, -mx)       -  2 ;
						ax2 = b->frame.pos.x + b->frame.size.x + 1                    +  2 ;
						ay1 = b->frame.pos.y                                          - 20 ;
						ay2 = b->frame.pos.y + b->frame.size.y - MIN(b->frame.size.y + 22, -my)  +  2 ;

						bx1 = b->frame.pos.x                                   -  2 ;
						by1 = b->frame.pos.y + b->frame.size.y - MIN(b->frame.size.y + 22, -my)  +  2 ;
						bx2 = b->frame.pos.x + b->frame.size.x + 1                       +  2 ;
						by2 = b->frame.pos.y + b->frame.size.y + 1                       +  2 ;
					}
				}

				al_set_target_bitmap(gui_buffer);
				al_draw_bitmap_region(gui_background, ax1, ay1, ax2 - ax1, ay2 - ay1, ax1, ay1, 0x0000);
				al_draw_bitmap_region(gui_background, bx1, by1, bx2 - bx1, by2 - by1, bx1, by1, 0x0000);

				// Update position
				b->frame.pos.x += mx;
				b->frame.pos.y += my;
				gui_box_clip_position(b);
			}
		}
    }
}

t_gui_box *	gui_box_new(const t_frame *frame, const char *title)
{
	assert(frame->size.x > 0 && frame->size.y > 0);

	// Allocate new box
	t_gui_box* box = (t_gui_box *)malloc(sizeof (t_gui_box));
	assert(box != NULL);

    // Setup members
    box->frame      = *frame;
    box->title      = strdup(title);
    box->type       = GUI_BOX_TYPE_STANDARD;
    box->flags      = GUI_BOX_FLAGS_ACTIVE | GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
	box->gfx_buffer = NULL;
	gui_box_create_video_buffer(box);
    box->widgets    = NULL;
	box->size_min.Set(32,32);
	box->size_max.Set(10000,10000);
	box->size_step.Set(1,1);
	box->size_fixed_ratio = false;
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
    gui.info.must_redraw = true;

    // Set focus
    // Note: be sure to call this after gui.box_last++
    gui_box_set_focus(box);
    gui_box_clip_position(box);

    return (box);
}

void	gui_box_create_video_buffer(t_gui_box *box)
{
	int sx, sy;
	if (box->gfx_buffer)
	{
		// Reuse previous bitmap size over the size stored in box.
		// Because current resizing system keeps largest size gfx_buffer so size stored in box may be smaller
		// (as used by Tilemap Viewer, but we may switch to properly recreating the video buffer)
		sx = al_get_bitmap_width(box->gfx_buffer);
		sy = al_get_bitmap_height(box->gfx_buffer);
		al_destroy_bitmap(box->gfx_buffer);
	}
	else
	{
		sx = box->frame.size.x+1;
		sy = box->frame.size.y+1;
	}

	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_NO_PRESERVE_TEXTURE);
	al_set_new_bitmap_format(g_configuration.video_gui_format_request);
	box->gfx_buffer = al_create_bitmap(box->frame.size.x+1, box->frame.size.y+1);
	assert(box->gfx_buffer);

	// Redraw and layout all
	box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
}

void	gui_box_destroy_widgets(t_gui_box* box)
{
	list_free_custom(&box->widgets, (t_list_free_handler)widget_destroy);
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

    // Shift plan/z buffer by one
    for (i = box_plan; i > 0; i--)
        gui.box_plan[i] = gui.box_plan[i - 1];
    gui.box_plan[0] = box->stupid_id;
    */
}

// Return whether given box has the focus
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

void	gui_box_resize(t_gui_box *box, int size_x, int size_y, bool interactive)
{
	size_x = Clamp<int>(size_x, box->size_min.x, box->size_max.x);
	size_y = Clamp<int>(size_y, box->size_min.y, box->size_max.y);

	if (box->size_step.x != 1 || box->size_step.y != 1)
	{
		float inc_ratio_x = (0.49f + (float)((size_x - (box->size_min.x-1))) / box->size_step.x);
		float inc_ratio_y = (0.49f + (float)((size_y - (box->size_min.y-1))) / box->size_step.y);

		if (box->size_fixed_ratio)
			inc_ratio_x = inc_ratio_y = MIN(inc_ratio_x, inc_ratio_y);
	
		size_x = box->size_min.x + (int)inc_ratio_x * box->size_step.x;
		size_y = box->size_min.y + (int)inc_ratio_y * box->size_step.y;
	}

	if (box->size_fixed_ratio && interactive)
	{
		const float scale = (size_x+1) / (float)g_driver->x_res;
		Msg(MSGT_STATUS_BAR, Msg_Get(MSG_Options_GUI_GameWindowScale), (int)(scale*100));
	}

	if (box->frame.size.x == size_x && box->frame.size.y == size_y)
		return;

	box->frame.size.x = size_x;
	box->frame.size.y = size_y;

	if (box->size_fixed_ratio)
		g_configuration.game_window_scale = (size_x+1) / (float)g_driver->x_res;

	gui_box_create_video_buffer(box);
	gui.info.must_redraw = true;

}

// Clip position of given box so that it shows on desktop.
void    gui_box_clip_position(t_gui_box *box)
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
