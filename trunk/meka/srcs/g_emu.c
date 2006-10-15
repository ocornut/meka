//-----------------------------------------------------------------------------
// MEKA - g_emu.c
// GUI things related with emulated machines - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "db.h"
#include "vdp.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_gui_box *  gamebox_instance;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// DRAW A GAME BOX ------------------------------------------------------------
// FIXME: ??
void        gamebox_draw (int sx, int sy, BITMAP *game_buffer)
{
    int     x_start = cur_drv->x_start;
    int     y_start = cur_drv->y_show_start;
    int     x_len   = cur_drv->x_res;
    int     y_len   = cur_drv->y_res;

    if ((cur_drv->id == DRV_SMS) && (Mask_Left_8))
    {
        rectfill (gui_buffer, sx, sy, sx + 3, sy + y_len - 1, COLOR_BLACK);
        rectfill (gui_buffer, sx + x_len - 4, sy, sx + x_len - 1, sy + y_len - 1, COLOR_BLACK);
        x_len -= 8;
        x_start += 8;
        sx += 4;
    }
    blit (game_buffer, gui_buffer, x_start, y_start, sx, sy, x_len, y_len);
}

// RETURN WIDTH OF A GAME BOX -------------------------------------------------
int     gamebox_x (void)
{
    return (cur_drv->x_res) - 1;
}

// RETURN HEIGHT OF A GAME BOX -------------------------------------------------
int     gamebox_y (void)
{
    return (cur_drv->y_res) - 1;
}

// CREATE A GAME BOX ----------------------------------------------------------
t_gui_box * gamebox_create(int x, int y)
{
    t_gui_box *box;
    t_frame frame;

    frame.pos.x = x;
    frame.pos.y = y;
    frame.size.x = gamebox_x();
    frame.size.y = gamebox_y();
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
    t_list *boxes;
    for (boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box *box = boxes->elem;
        if (box->type == GUI_BOX_TYPE_GAME)
        {
            box->frame.size.x = gamebox_x ();
            box->frame.size.y = gamebox_y ();
            box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
        }
    }
    gui.info.must_redraw = TRUE;
}

void        gamebox_rename_all (void)
{
    t_list *boxes;
    const char *new_name;
    
    if (DB_CurrentEntry)
        new_name = DB_Entry_GetCurrentName(DB_CurrentEntry);
    else
    {
        if (machine & MACHINE_CART_INSERTED)
            new_name = Msg_Get (MSG_DB_Name_Default);
        else
            new_name = Msg_Get (MSG_DB_Name_NoCartridge);
    }

    for (boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box *box = boxes->elem;
        if (box->type == GUI_BOX_TYPE_GAME)
            gui_box_set_title (box, new_name);
    }
}

//-----------------------------------------------------------------------------

