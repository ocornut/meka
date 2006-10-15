//-----------------------------------------------------------------------------
// MEKA - g_menu.c
// GUI Menus - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fskipper.h"

//-----------------------------------------------------------------------------

// REDRAW MENU AND STATUS BARS ------------------------------------------------
void        gui_redraw_bars (void)
{
    char    s[16];

    // Redraw status bar ------------------------------------------------------
    rectfill (gui_buffer, 0, cfg.GUI_Res_Y - gui.info.bars_height, cfg.GUI_Res_X, cfg.GUI_Res_Y, GUI_COL_BARS);
    rectfill (gui_buffer, 0, cfg.GUI_Res_Y - gui.info.bars_height - 2, cfg.GUI_Res_X, cfg.GUI_Res_Y - gui.info.bars_height - 1, GUI_COL_BORDERS);

    Font_SetCurrent (F_LARGE);

    // Show status bar message
    if (gui_status.timeleft)
    {
        Font_Print (-1, gui_buffer, gui_status.message, gui_status.x, cfg.GUI_Res_Y - 16, GUI_COL_TEXT_STATUS);
        gui_status.timeleft --;
    }

    // Show FPS counter
    if (fskipper.FPS_Display)
    {
        sprintf (s, "%d FPS", fskipper.FPS);
        Font_Print (-1, gui_buffer, s, cfg.GUI_Res_X - 100 - Font_TextLength (-1, s), cfg.GUI_Res_Y - 16, GUI_COL_TEXT_STATUS);
    }

    // Show current time
    meka_time_getf (s);
    Font_Print (-1, gui_buffer, s, cfg.GUI_Res_X - 10 - Font_TextLength (-1, s), cfg.GUI_Res_Y - 16, GUI_COL_TEXT_STATUS);
}

// UPDATE ALL MENUS -----------------------------------------------------------
void            gui_update_menu (int n_menu, int n_parent, int n_parent_entry, int generation)
{
 int            i;
 int            x1, y1, x2, y2;
 gui_type_menu  *menu = menus[n_menu];

 // if (gui_mouse.on_box != -1)
 if ((gui_mouse.pressed_on != PRESSED_ON_NOTHING) && (gui_mouse.pressed_on != PRESSED_ON_MENUS))
    {
    return;
    }

 menu->generation = generation;

 // Update each menus ---------------------------------------------------------
 for (i = 0; i < menu->n_entry; i ++)
     {
     if (!(menu->entry[i]->attr & AM_Active))
        {
        continue;
        }
     gui_menu_return_entry_pos (n_menu, i, &x1, &y1, &x2, &y2);

     // ---
     if (gui_mouse_area (x1, y1, x2, y2))
        {
        if ((!gui_mouse.button) && (gui_mouse.pbutton & 1))
           {
           if (menu->entry[i]->type == ITEM_EXECUTE)
              {
              gui_menu_un_mouse_over (menus_ID.menu);
              menu->entry[i]->func (i); // Execute the callback function
              }
           }
        if (gui_mouse.button & 1)
           {
           if (!((menus_opt.c_menu == n_menu) && (menus_opt.c_entry == i)))
              {
              if (menus_opt.c_generation > generation)
                 {
                 menu->entry[i]->mouse_over = 0;
                 }
              menus_opt.c_menu = n_menu;
              menus_opt.c_entry = i;
              menus_opt.c_somewhere = 1;
              menus_opt.c_generation = generation;
              if (menu->entry[i]->mouse_over == 0)
                 {
                 gui_menu_un_mouse_over (n_menu);
                 menu->entry[i]->mouse_over = 1;
                 }
              else
                 {
                 if (menu->entry[i]->type == ITEM_SUB_MENU)
                    {
                    gui_menu_un_mouse_over (menu->entry[i]->submenu_id);
                    }
                 menu->entry[i]->mouse_over = 0;
                 gui.info.must_redraw = 1;
                 }
              gui_mouse.pressed_on = PRESSED_ON_MENUS;
              }
           }
        }

     // Update sub-menus if necessary ----------------------------------------
     if ((menu->entry[i]->mouse_over) && (menu->entry[i]->type == ITEM_SUB_MENU))
        {
        gui_menu_return_children_pos (n_menu, i, &menus[menu->entry[i]->submenu_id]->sx,
                                                 &menus[menu->entry[i]->submenu_id]->sy);
        gui_update_menu (menu->entry[i]->submenu_id, n_menu, i, generation + 1);
        }
     }
}

// REDRAW A MENU AND HIS CHILDREN ---------------------------------------------
void            gui_draw_menu (int n_menu, int n_parent, int n_parent_entry)
{
 int            i;
 int            x, y;
 int            color, ln;
 gui_type_menu  *menu = menus [n_menu];

 Font_SetCurrent (GUI_MENUS_FONT);

 if (n_menu == MENU_ID_MAIN) // Main menu (horizontal) ------------------------
    {
    // Draw menu background
    rectfill (gui_buffer, 0, 0, cfg.GUI_Res_X, gui.info.bars_height, GUI_COL_BARS);
    rectfill (gui_buffer, 0, gui.info.bars_height + 1, cfg.GUI_Res_X, gui.info.bars_height + 2, GUI_COL_BORDERS);

    // Draw menu entrys
    x = menus_opt.distance;
    y = 3;
    for (i = 0; i < menu->n_entry; i ++)
        {
        ln = Font_TextLength (-1, menu->entry[i]->label);
        if (x + ln > cfg.GUI_Res_X)
           {
           break;
           }
        if ((menu->entry[i]->mouse_over) && (menu->entry[i]->attr & AM_Active))
           {
           gui_menu_highlight (n_menu, i);
           if (menu->entry[i]->type == ITEM_SUB_MENU)
              {
              gui_draw_menu (menu->entry[i]->submenu_id, n_menu, i);
              }
           }
        if (menu->entry[i]->attr & AM_Active)
           {
           color = GUI_COL_TEXT_ACTIVE;
           }
        else
           {
           color = GUI_COL_TEXT_N_ACTIVE;
           }
        Font_Print (-1, gui_buffer, menu->entry[i]->label, x, y, color);
        x += ln + menus_opt.distance;
        }
    }
 else // Children menu (vertical) ---------------------------------------------
    {
    int t2;

    // Miscellaneous
    // gui.info.must_redraw = 1;
    ln = Font_Height (-1);
    gui_menu_return_children_pos (n_parent, n_parent_entry, &menu->sx, &menu->sy);

    // DRAW MENU BORDER -------------------------------------------------------
    // rectfill (gui_buffer, menu->sx - 2, menu->sy - 2, menu->sx + menu->lx + 2, menu->sy + menu->ly + 2, GUI_COL_BORDERS);
    // rect (gui_buffer, menu->sx - 1, menu->sy - 1, menu->sx + menu->lx + 1, menu->sy + menu->ly + 1, GUI_COL_BORDERS);
    rect (gui_buffer, menu->sx - 2, menu->sy - 1, menu->sx + menu->lx + 2, menu->sy + menu->ly + 1, GUI_COL_BORDERS);
    rect (gui_buffer, menu->sx - 1, menu->sy - 2, menu->sx + menu->lx + 1, menu->sy + menu->ly + 2, GUI_COL_BORDERS);

    // DRAW MENU BACKGROUND WITH/WITHOUT GRADIENTS ----------------------------
    t2 = menu->sy + menu->ly;
    if (gui.info.menu_gradients)
       {
       int j;
       int t1 = (menu->ly / gui.info.menu_gradients_ratio) / (GUI_COL_THEME_GRADIENTS_NUM - gui.info.menu_gradients_unused);
       for (j = GUI_COL_THEME_GRADIENTS_NUM - gui.info.menu_gradients_unused - 1; j > 0; j --, t2 -= t1)
           {
           rectfill (gui_buffer, menu->sx, t2 - t1, menu->sx + menu->lx, t2, GUI_COL_BARS + j);
           }
       }
    if (n_parent != 0)
       {
       rectfill (gui_buffer, menu->sx, menu->sy, menu->sx + menu->lx, t2, GUI_COL_BARS);
       }
    else
       {
       rectfill (gui_buffer, menu->sx, menu->sy - 2, menu->sx + menu->lx, t2, GUI_COL_BARS);
       }

    // Draw menu entrys -------------------------------------------------------
    x = menu->sx + MENUS_PADDING_X;
    y = menu->sy + MENUS_PADDING_Y;
    for (i = 0; i < menu->n_entry; i ++)
        {
        if (y + ln > cfg.GUI_Res_Y)
           {
           break;
           }
        if ((menu->entry[i]->mouse_over) && (menu->entry[i]->attr & AM_Active))
           {
           gui_menu_highlight (n_menu, i);
           if (menu->entry[i]->type == ITEM_SUB_MENU)
              {
              gui_draw_menu (menu->entry[i]->submenu_id, n_menu, i);
              }
           }
        if (menu->entry[i]->attr & AM_Active)
           {
           color = GUI_COL_TEXT_ACTIVE;
           }
        else
           {
           color = GUI_COL_TEXT_N_ACTIVE;
           }
        Font_Print (-1, gui_buffer, menu->entry[i]->label, x, y, color);
        switch (menu->entry[i]->type)
           {
           case ITEM_SUB_MENU:
                Font_Print (-1, gui_buffer, MEKA_FONT_STR_ARROW, menu->sx + menu->lx - MENUS_PADDING_X, y, color);
                break;
           case ITEM_EXECUTE:
                if (menu->entry[i]->attr & AM_Checked)
                   {
                   Font_Print (-1, gui_buffer, MEKA_FONT_STR_CHECKED, menu->sx + menu->lx - MENUS_PADDING_X - 1, y, color);
                   }
                break;
          }
        y += ln + MENUS_PADDING_Y;
        }
    }
}

// REDRAW ALL MENUS -----------------------------------------------------------
void    gui_redraw_menus (void)
{
    gui_redraw_bars ();
    if (menus_opt.distance > MENUS_DISTANCE)
    {
        menus_opt.distance -= 14;
        if (menus_opt.distance < MENUS_DISTANCE)
            menus_opt.distance = MENUS_DISTANCE;
        gui.info.must_redraw = YES;
    }
    gui_draw_menu (menus_ID.menu, -1, -1);
}

// UPDATE ALL MENUS -----------------------------------------------------------
void    gui_update_menus (void)
{
    menus_opt.c_somewhere = 0;
    gui_update_menu (menus_ID.menu, 0, 0, 0);
    if ((gui_mouse.button & 1) && (menus_opt.c_somewhere == 0)
        && (menus_opt.c_menu == -1)
        && (menus_opt.c_entry == -1)
        && (menus_opt.c_generation == -1))
    {
        gui_menu_un_mouse_over (0);
    }
}

//-----------------------------------------------------------------------------

