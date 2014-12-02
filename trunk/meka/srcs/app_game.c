//-----------------------------------------------------------------------------
// MEKA - app_game.c
// Game screen applet - Code
//-----------------------------------------------------------------------------
// FIXME: very old code.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_game.h"
#include "db.h"
#include "vdp.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_gui_box *  gamebox_instance;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void        gamebox_draw(t_gui_box *box, ALLEGRO_BITMAP *game_buffer)
{
	const float scale = g_configuration.game_window_scale;

	int     x_start = g_driver->x_start;
    int     y_start = g_driver->y_show_start;
    int     x_len   = g_driver->x_res;
    int     y_len   = g_driver->y_res;
    int     x_dst   = box->frame.pos.x;
    int     y_dst   = box->frame.pos.y;

    if ((g_driver->id == DRV_SMS) && (Mask_Left_8))
    {
        // Center screen when 8 left columns are masked
        // This not logical but looks good
        al_draw_filled_rectangle(x_dst, y_dst, x_dst + 4*scale, y_dst + y_len*scale, COLOR_BLACK);
        al_draw_filled_rectangle(x_dst + (x_len - 4)*scale, y_dst, x_dst + x_len*scale, y_dst + y_len*scale, COLOR_BLACK);
        x_len -= 8;
        x_start += 8;
        x_dst += 4*scale;
    }

	if (fabs(scale-1.0f) < 0.001f)
	    al_draw_bitmap_region(game_buffer, x_start, y_start, x_len, y_len, x_dst, y_dst, 0x0000);
	else
		al_draw_scaled_bitmap(game_buffer, x_start, y_start, x_len, y_len, x_dst, y_dst, x_len*scale, y_len*scale, 0x0000);
}

void        gamebox_compute_size(int *x, int *y)
{
    *x = (g_driver->x_res * g_configuration.game_window_scale) - 1;
    *y = (g_driver->y_res * g_configuration.game_window_scale) - 1;
}

t_gui_box * gamebox_create(int x, int y)
{
    t_frame frame;
    frame.pos.x = x;
    frame.pos.y = y;
    gamebox_compute_size(&frame.size.x, &frame.size.y);

	t_gui_box *box = gui_box_new(&frame, "--");
    if (box == NULL)
        return (NULL);

    box->type = GUI_BOX_TYPE_GAME;
    box->flags |= GUI_BOX_FLAGS_TAB_STOP;
	box->flags |= GUI_BOX_FLAGS_ALLOW_RESIZE;
	box->size_min.x = (g_driver->x_res/2)-1;
	box->size_min.y = (g_driver->y_res/2)-1;
	box->size_step.x = g_driver->x_res/4;
	box->size_step.y = g_driver->y_res/4;
	box->size_fixed_ratio = true;

    gui_box_clip_position(box);
    gamebox_rename_all();

    return (box);
}

void	gamebox_resize_all()
{
    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;
        if (box->type == GUI_BOX_TYPE_GAME)
        {
			box->size_min.x = (g_driver->x_res/2)-1;
			box->size_min.y = (g_driver->y_res/2)-1;
			box->size_step.x = g_driver->x_res/4;
			box->size_step.y = g_driver->y_res/4;
			int sx, sy;
            gamebox_compute_size(&sx, &sy);
			gui_box_resize(box, sx, sy, false);
        }
    }
    gui.info.must_redraw = TRUE;
}

void	gamebox_rename_all()
{
    const char *new_name;
    
	if (DB.current_entry)
	{
        new_name = DB_Entry_GetCurrentName(DB.current_entry);
	}
    else
    {
        if (g_machine_flags & MACHINE_CART_INSERTED)
            new_name = Msg_Get(MSG_DB_Name_Default);
        else
            new_name = Msg_Get(MSG_DB_Name_NoCartridge);
    }

    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;
        if (box->type == GUI_BOX_TYPE_GAME)
            gui_box_set_title(box, new_name);
    }
}

//-----------------------------------------------------------------------------
