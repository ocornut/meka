//-----------------------------------------------------------------------------
// MEKA - g_emu.c
// GUI things related with emulated machines - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "db.h"
#include "vdp.h"

//-----------------------------------------------------------------------------

// DRAW A GAME BOX ------------------------------------------------------------
void        gamebox_draw (int which, int sx, int sy, BITMAP *game_buffer)
{
    int     x_start = cur_drv->x_start;
    int     y_start = cur_drv->y_show_start;
    int     x_len   = cur_drv->x_res;
    int     y_len   = cur_drv->y_res;

    if ((cur_drv->id == DRV_SMS) && (Mask_Left_8))
    {
        rectfill (gui_buffer, sx, sy, sx + 3, sy + y_len - 1, GUI_COL_BLACK);
        rectfill (gui_buffer, sx + x_len - 4, sy, sx + x_len - 1, sy + y_len - 1, GUI_COL_BLACK);
        x_len -= 8;
        x_start += 8;
        sx += 4;
    }
    blit (game_buffer, gui_buffer, x_start, y_start, sx, sy, x_len, y_len);
}

// RETURN WIDTH OF A GAME BOX -------------------------------------------------
int     gamebox_x (void)
{
    return (cur_drv->x_res - 1);
}

// RETURN HEIGHT OF A GAME BOX -------------------------------------------------
int     gamebox_y (void)
{
    return (cur_drv->y_res - 1);
}

// CREATE A GAME BOX ----------------------------------------------------------
int     gamebox_create (int x, int y)
{
    int n = gui_box_create (x, y, gamebox_x (), gamebox_y (), "--");
    if (n == -1) 
        return (-1);
    gui.box[n]->type = GUI_BOX_TYPE_GAME;
    gui_box_clip_position (gui.box[n]);
    gamebox_rename_all ();
    return (n);
}

// CREATE A GAME BOX USING MOUSE POSITIONS ------------------------------------
void    gamebox_create_on_mouse_pos (void)
{
    gamebox_create (gui_mouse.x, gui_mouse.y);
}

void        gamebox_kill_all (void)
{
    int     n;
    int     first = 1;

    for (n = 0; n < gui.box_last; n ++)
        if (gui.box [n]->type == GUI_BOX_TYPE_GAME)
        {
            if (first)
            {
                first = 0;
            }
            else
            {
                //..kill here..
            }
        }
}

void        gamebox_kill_last (void)
{
    int     n;
    for (n = gui.box_last - 1; n >= 0; n ++)
        if (gui.box [n]->type == GUI_BOX_TYPE_GAME)
        {
            //..kill here..
            break;
        }
}

void        gamebox_resize_all (void)
{
    int     i;
    for (i = 0; i < gui.box_last; i++)
    {
        t_gui_box *box = gui.box[i];
        if (box->type == GUI_BOX_TYPE_GAME)
        {
            box->frame.size.x = gamebox_x ();
            box->frame.size.y = gamebox_y ();
            box->must_redraw = YES;
        }
    }
    gui.info.must_redraw = YES;
}

void        gamebox_rename_all (void)
{
    int     i;
    char *  new_name;
    
    if (DB_CurrentEntry)
        new_name = DB_Entry_GetCurrentName(DB_CurrentEntry);
    else
    {
        if (machine & MACHINE_CART_INSERTED)
            new_name = Msg_Get (MSG_DB_Name_Default);
        else
            new_name = Msg_Get (MSG_DB_Name_NoCartridge);
    }

    for (i = 0; i < gui.box_last; i ++)
    {
        t_gui_box *box = gui.box[i];
        if (box->type == GUI_BOX_TYPE_GAME)
            gui_box_set_title (box, new_name);
    }
}

//-----------------------------------------------------------------------------

