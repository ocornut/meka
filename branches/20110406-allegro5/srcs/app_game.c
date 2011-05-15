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

void        gamebox_draw (t_gui_box *box, ALLEGRO_BITMAP *game_buffer)
{
    int     x_start = cur_drv->x_start;
    int     y_start = cur_drv->y_show_start;
    int     x_len   = cur_drv->x_res;
    int     y_len   = cur_drv->y_res;
    int     x_dst   = box->frame.pos.x;
    int     y_dst   = box->frame.pos.y;

	al_set_target_bitmap(gui_buffer);
    if ((cur_drv->id == DRV_SMS) && (Mask_Left_8))
    {
        // Center screen when 8 left columns are masked
        // This not logical but looks good
        al_draw_filled_rectangle(x_dst, y_dst, x_dst + 4, y_dst + y_len, COLOR_BLACK);
        al_draw_filled_rectangle(x_dst + x_len - 4, y_dst, x_dst + x_len, y_dst + y_len, COLOR_BLACK);
        x_len -= 8;
        x_start += 8;
        x_dst += 4;
    }

    //stretch_blit(screenbuffer, fs_out, 
      //  blit_cfg.src_sx, blit_cfg.src_sy,
        //cur_drv->x_res, cur_drv->y_res,
        //0,0, Video.res_x, Video.res_y);

    al_draw_bitmap_region(game_buffer, x_start, y_start, x_len, y_len, x_dst, y_dst, 0);
}

void        gamebox_compute_size(int *x, int *y)
{
    *x = (cur_drv->x_res * g_Configuration.game_screen_scale) - 1;
    *y = (cur_drv->y_res * g_Configuration.game_screen_scale) - 1;
}

// CREATE A GAME BOX ----------------------------------------------------------
t_gui_box * gamebox_create(int x, int y)
{
    t_gui_box *box;
    t_frame frame;

    frame.pos.x = x;
    frame.pos.y = y;
    gamebox_compute_size(&frame.size.x, &frame.size.y);
    box = gui_box_new(&frame, "--");
    if (box == NULL)
        return (NULL);

    box->type = GUI_BOX_TYPE_GAME;
    box->flags |= GUI_BOX_FLAGS_TAB_STOP;

    gui_box_clip_position(box);
    gamebox_rename_all();

    return (box);
}

void        gamebox_resize_all (void)
{
    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;
        if (box->type == GUI_BOX_TYPE_GAME)
        {
            gamebox_compute_size(&box->frame.size.x, &box->frame.size.y);
            box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
        }
    }
    gui.info.must_redraw = TRUE;
}

void        gamebox_rename_all (void)
{
    const char *new_name;
    
	if (DB.current_entry)
	{
        new_name = DB_Entry_GetCurrentName(DB.current_entry);
	}
    else
    {
        if (machine & MACHINE_CART_INSERTED)
            new_name = Msg_Get (MSG_DB_Name_Default);
        else
            new_name = Msg_Get (MSG_DB_Name_NoCartridge);
    }

    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* box = (t_gui_box*)boxes->elem;
        if (box->type == GUI_BOX_TYPE_GAME)
            gui_box_set_title (box, new_name);
    }
}

//-----------------------------------------------------------------------------

