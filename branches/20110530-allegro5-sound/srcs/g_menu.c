//-----------------------------------------------------------------------------
// MEKA - g_menu.c
// GUI Menus - Code
//-----------------------------------------------------------------------------
// FIXME: This API is absolute crap. Redo from stratch (see WIP in g_menu.h)
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fskipper.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_gui_status_bar	g_gui_status;
t_gui_menus_id		menus_ID;
gui_type_menus_opt	menus_opt;
t_menu *		menus[MAX_MENUS];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void        gui_redraw_bars (void)
{
    // Redraw status bar
	al_set_target_bitmap(gui_buffer);
    al_draw_filled_rectangle(0, g_Configuration.video_mode_gui_res_y - gui.info.bars_height,     g_Configuration.video_mode_gui_res_x+1, g_Configuration.video_mode_gui_res_y + 1, COLOR_SKIN_WIDGET_STATUSBAR_BACKGROUND);
    al_draw_filled_rectangle(0, g_Configuration.video_mode_gui_res_y - gui.info.bars_height - 2, g_Configuration.video_mode_gui_res_x+1, g_Configuration.video_mode_gui_res_y - gui.info.bars_height, COLOR_SKIN_WIDGET_STATUSBAR_BORDER);

    Font_SetCurrent (F_LARGE);

    // Show status bar message
    if (g_gui_status.timeleft)
    {
        Font_Print(F_CURRENT, g_gui_status.message, g_gui_status.x, g_Configuration.video_mode_gui_res_y - 16, COLOR_SKIN_WIDGET_STATUSBAR_TEXT);
        g_gui_status.timeleft --;
    }

    // Show FPS counter
    if (fskipper.FPS_Display)
    {
	    char s[16];
        sprintf (s, "%.1f FPS", fskipper.FPS);
        Font_Print(F_CURRENT, s, g_Configuration.video_mode_gui_res_x - 100 - Font_TextLength(F_CURRENT, s), g_Configuration.video_mode_gui_res_y - 16, COLOR_SKIN_WIDGET_STATUSBAR_TEXT);
    }

    // Show current time
    char s[16];
    meka_time_getf(s);
    Font_Print(F_CURRENT, s, g_Configuration.video_mode_gui_res_x - 10 - Font_TextLength(F_CURRENT, s), g_Configuration.video_mode_gui_res_y - 16, COLOR_SKIN_WIDGET_STATUSBAR_TEXT);
}

void            gui_update_menu (int n_menu, int n_parent, int n_parent_entry, int generation)
{
    int            i;
    int            x1, y1, x2, y2;
    t_menu * menu = menus[n_menu];

    //if ((gui_mouse.pressed_on != PRESSED_ON_NOTHING) && (gui_mouse.pressed_on != PRESSED_ON_MENUS))
    // FIXME-FOCUS
    if (gui.mouse.focus != GUI_FOCUS_NONE && gui.mouse.focus != GUI_FOCUS_MENUS)
    {
        return;
    }

    menu->generation = generation;

    for (i = 0; i < menu->n_entry; i ++)
    {
        t_menu_item *menu_entry = menu->entry[i];
        if (!(menu_entry->flags & AM_Active))
        {
            continue;
        }
        gui_menu_return_entry_pos (n_menu, i, &x1, &y1, &x2, &y2);

        // ---
        if (gui_is_mouse_hovering_area(x1, y1, x2, y2))
        {
            if ((!gui.mouse.buttons) && (gui.mouse.buttons_prev & 1))
            {
                if (menu_entry->type == ITEM_EXECUTE)
                {
                    // Setup event structure
                    t_menu_event event;
                    event.menu          = menu;
                    event.menu_idx      = n_menu;
                    event.menu_item     = menu_entry;
                    event.menu_item_idx = i;
                    event.user_data     = menu_entry->user_data;

                    gui_menu_un_mouse_over (menus_ID.menu);

                    // Call event handler
                    menu_entry->callback(&event);
                }
            }
            if (gui.mouse.buttons & 1)
            {
                if (!((menus_opt.c_menu == n_menu) && (menus_opt.c_entry == i)))
                {
                    if (menus_opt.c_generation > generation)
                    {
                        menu_entry->mouse_over = false;
                    }
                    menus_opt.c_menu = n_menu;
                    menus_opt.c_entry = i;
                    menus_opt.c_somewhere = 1;
                    menus_opt.c_generation = generation;
                    if (menu_entry->mouse_over == false)
                    {
                        gui_menu_un_mouse_over (n_menu);
                        menu_entry->mouse_over = true;
                    }
                    else
                    {
                        if (menu_entry->type == ITEM_SUB_MENU)
                        {
                            gui_menu_un_mouse_over (menu->entry[i]->submenu_id);
                        }
                        menu_entry->mouse_over = false;
                        gui.info.must_redraw = TRUE;
                    }

                    // FIXME-FOCUS
                    //gui_mouse.pressed_on = PRESSED_ON_MENUS;
                    gui.mouse.focus = GUI_FOCUS_MENUS;
                    gui.mouse.focus_item = menu;
                }
            }
        }

        // Update sub-menus if necessary ----------------------------------------
        if ((menu_entry->mouse_over) && (menu_entry->type == ITEM_SUB_MENU))
        {
            gui_menu_return_children_pos (n_menu, i, &menus[menu_entry->submenu_id]->sx,
                &menus[menu_entry->submenu_id]->sy);
            gui_update_menu (menu_entry->submenu_id, n_menu, i, generation + 1);
        }
    }
}

void            gui_draw_menu (int n_menu, int n_parent, int n_parent_entry)
{
    int            i;
    int            x, y;
    ALLEGRO_COLOR  color;
    t_menu  *menu = menus [n_menu];

    Font_SetCurrent (GUI_MENUS_FONT);

    if (n_menu == MENU_ID_MAIN) // Main menu (horizontal) ------------------------
    {
        // Draw menu background
		al_set_target_bitmap(gui_buffer);
        al_draw_filled_rectangle(0, 0, g_Configuration.video_mode_gui_res_x+1, gui.info.bars_height+1, COLOR_SKIN_MENU_BACKGROUND);
        al_draw_filled_rectangle(0, gui.info.bars_height + 1, g_Configuration.video_mode_gui_res_x+1, gui.info.bars_height + 2+1, COLOR_SKIN_MENU_BORDER);

        // Draw menu entrys
        x = menus_opt.distance;
        y = 3;
        for (i = 0; i < menu->n_entry; i ++)
        {
            const int ln = Font_TextLength(F_CURRENT, menu->entry[i]->label);
            if (x + ln > g_Configuration.video_mode_gui_res_x)
            {
                break;
            }
            if ((menu->entry[i]->mouse_over) && (menu->entry[i]->flags & AM_Active))
            {
                gui_menu_highlight (n_menu, i);
                if (menu->entry[i]->type == ITEM_SUB_MENU)
                {
                    gui_draw_menu (menu->entry[i]->submenu_id, n_menu, i);
                }
            }
            if (menu->entry[i]->flags & AM_Active)
            {
                color = COLOR_SKIN_MENU_TEXT;
            }
            else
            {
                color = COLOR_SKIN_MENU_TEXT_UNACTIVE;
            }
            Font_Print(F_CURRENT, menu->entry[i]->label, x, y, color);
            x += ln + menus_opt.distance;
        }
    }
    else // Children menu (vertical) ---------------------------------------------
    {
        // Miscellaneous
        // gui.info.must_redraw = TRUE;
        const int ln = Font_Height();
        gui_menu_return_children_pos(n_parent, n_parent_entry, &menu->sx, &menu->sy);

        // DRAW MENU BORDER -------------------------------------------------------
        // rectfill (gui_buffer, menu->sx - 2, menu->sy - 2, menu->sx + menu->lx + 2, menu->sy + menu->ly + 2, COLOR_SKIN_MENU_BORDER);
        // rect (gui_buffer, menu->sx - 1, menu->sy - 1, menu->sx + menu->lx + 1, menu->sy + menu->ly + 1, COLOR_SKIN_MENU_BORDER);
		al_set_target_bitmap(gui_buffer);
        al_draw_rectangle(menu->sx - 1.5f, menu->sy - 0.5f, menu->sx + menu->lx + 2.5f, menu->sy + menu->ly + 1.5f, COLOR_SKIN_MENU_BORDER, 1.0f);
        al_draw_rectangle(menu->sx - 0.5f, menu->sy - 1.5f, menu->sx + menu->lx + 1.5f, menu->sy + menu->ly + 2.5f, COLOR_SKIN_MENU_BORDER, 1.0f);

        // DRAW MENU BACKGROUND WITH/WITHOUT GRADIENTS ----------------------------
        {
            t_frame menu_frame;
            menu_frame.pos.x = menu->sx;
            menu_frame.pos.y = menu->sy;
            menu_frame.size.x = menu->lx;
            menu_frame.size.y = menu->ly;
            if (n_parent == 0)
            {
                menu_frame.pos.y -= 2;
                menu_frame.size.y += 2;
            }
            SkinGradient_DrawVertical(&Skins_GetCurrentSkin()->gradient_menu, gui_buffer, &menu_frame);
        }

        // Draw menu entrys -------------------------------------------------------
        x = menu->sx + MENUS_PADDING_X;
        y = menu->sy + MENUS_PADDING_Y;
        for (i = 0; i < menu->n_entry; i ++)
        {
            if (y + ln > g_Configuration.video_mode_gui_res_y)
            {
                break;
            }
            if ((menu->entry[i]->mouse_over) && (menu->entry[i]->flags & AM_Active))
            {
                gui_menu_highlight (n_menu, i);
                if (menu->entry[i]->type == ITEM_SUB_MENU)
                {
                    gui_draw_menu (menu->entry[i]->submenu_id, n_menu, i);
                }
            }
            if (menu->entry[i]->flags & AM_Active)
            {
                color = COLOR_SKIN_MENU_TEXT;
            }
            else
            {
                color = COLOR_SKIN_MENU_TEXT_UNACTIVE;
            }
            Font_Print(F_CURRENT, menu->entry[i]->label, x, y, color);
            switch (menu->entry[i]->type)
            {
            case ITEM_SUB_MENU:
                Font_Print(F_CURRENT, MEKA_FONT_STR_ARROW, menu->sx + menu->lx - MENUS_PADDING_X, y, color);
                break;
            case ITEM_EXECUTE:
                if (menu->entry[i]->flags & AM_Checked)
                {
                    Font_Print(F_CURRENT, MEKA_FONT_STR_CHECKED, menu->sx + menu->lx - MENUS_PADDING_X - 1, y, color);
                }
                break;
            }
            y += ln + MENUS_PADDING_Y;
        }
    }
}

void    gui_redraw_menus (void)
{
    gui_redraw_bars ();

	// initial panning animation
    if (menus_opt.distance > MENUS_DISTANCE)
    {
        menus_opt.distance -= 14;
        if (menus_opt.distance < MENUS_DISTANCE)
            menus_opt.distance = MENUS_DISTANCE;
        gui.info.must_redraw = TRUE;
    }

    gui_draw_menu (menus_ID.menu, -1, -1);
}

void    gui_update_menus (void)
{
    menus_opt.c_somewhere = 0;
    gui_update_menu (menus_ID.menu, 0, 0, 0);
    if ((gui.mouse.buttons & 1) && (menus_opt.c_somewhere == 0)
        && (menus_opt.c_menu == -1)
        && (menus_opt.c_entry == -1)
        && (menus_opt.c_generation == -1))
    {
        gui_menu_un_mouse_over (0);
    }
}

//-----------------------------------------------------------------------------
