//-----------------------------------------------------------------------------
// MEKA - g_menu.c
// GUI Menus - Code
//-----------------------------------------------------------------------------
// FIXME: This API is absolute crap. Redo from stratch (see WIP in g_menu.h)
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fskipper.h"

//-----------------------------------------------------------------------------

// REDRAW MENU AND STATUS BARS ------------------------------------------------
void        gui_redraw_bars (void)
{
    char    s[16];

    // Redraw status bar ------------------------------------------------------
    rectfill (gui_buffer, 0, Configuration.video_mode_gui_res_y - gui.info.bars_height,     Configuration.video_mode_gui_res_x, Configuration.video_mode_gui_res_y, COLOR_SKIN_WIDGET_STATUSBAR_BACKGROUND);
    rectfill (gui_buffer, 0, Configuration.video_mode_gui_res_y - gui.info.bars_height - 2, Configuration.video_mode_gui_res_x, Configuration.video_mode_gui_res_y - gui.info.bars_height - 1, COLOR_SKIN_WIDGET_STATUSBAR_BORDER);

    Font_SetCurrent (F_LARGE);

    // Show status bar message
    if (gui_status.timeleft)
    {
        Font_Print (-1, gui_buffer, gui_status.message, gui_status.x, Configuration.video_mode_gui_res_y - 16, COLOR_SKIN_WIDGET_STATUSBAR_TEXT);
        gui_status.timeleft --;
    }

    // Show FPS counter
    if (fskipper.FPS_Display)
    {
        sprintf (s, "%d FPS", fskipper.FPS);
        Font_Print (-1, gui_buffer, s, Configuration.video_mode_gui_res_x - 100 - Font_TextLength (-1, s), Configuration.video_mode_gui_res_y - 16, COLOR_SKIN_WIDGET_STATUSBAR_TEXT);
    }

    // Show current time
    meka_time_getf (s);
    Font_Print (-1, gui_buffer, s, Configuration.video_mode_gui_res_x - 10 - Font_TextLength (-1, s), Configuration.video_mode_gui_res_y - 16, COLOR_SKIN_WIDGET_STATUSBAR_TEXT);
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
		 gui_type_menu_entry *menu_entry = menu->entry[i];
		 if (!(menu_entry->attr & AM_Active))
		 {
			 continue;
		 }
		 gui_menu_return_entry_pos (n_menu, i, &x1, &y1, &x2, &y2);

		 // ---
		 if (gui_mouse_area (x1, y1, x2, y2))
		 {
			 if ((!gui_mouse.button) && (gui_mouse.pbutton & 1))
			 {
				 if (menu_entry->type == ITEM_EXECUTE)
				 {
					 // Setup event structure
					 t_menu_event event;
					 event.menu				= menu;
					 event.menu_idx			= n_menu;
					 event.menu_item		= menu_entry;
					 event.menu_item_idx	= i;
					 event.user_data		= menu_entry->user_data;

					 gui_menu_un_mouse_over (menus_ID.menu);

					 // Call event handler
					 menu_entry->event_handler(&event);
				 }
			 }
			 if (gui_mouse.button & 1)
			 {
				 if (!((menus_opt.c_menu == n_menu) && (menus_opt.c_entry == i)))
				 {
					 if (menus_opt.c_generation > generation)
					 {
						 menu_entry->mouse_over = 0;
					 }
					 menus_opt.c_menu = n_menu;
					 menus_opt.c_entry = i;
					 menus_opt.c_somewhere = 1;
					 menus_opt.c_generation = generation;
					 if (menu_entry->mouse_over == 0)
					 {
						 gui_menu_un_mouse_over (n_menu);
						 menu_entry->mouse_over = 1;
					 }
					 else
					 {
						 if (menu_entry->type == ITEM_SUB_MENU)
						 {
							 gui_menu_un_mouse_over (menu->entry[i]->submenu_id);
						 }
						 menu_entry->mouse_over = 0;
						 gui.info.must_redraw = 1;
					 }
					 gui_mouse.pressed_on = PRESSED_ON_MENUS;
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
    rectfill (gui_buffer, 0, 0, Configuration.video_mode_gui_res_x, gui.info.bars_height, COLOR_SKIN_MENU_BACKGROUND);
    rectfill (gui_buffer, 0, gui.info.bars_height + 1, Configuration.video_mode_gui_res_x, gui.info.bars_height + 2, COLOR_SKIN_MENU_BORDER);

    // Draw menu entrys
    x = menus_opt.distance;
    y = 3;
    for (i = 0; i < menu->n_entry; i ++)
        {
        ln = Font_TextLength (-1, menu->entry[i]->label);
        if (x + ln > Configuration.video_mode_gui_res_x)
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
               color = COLOR_SKIN_MENU_TEXT;
           }
        else
           {
               color = COLOR_SKIN_MENU_TEXT_UNACTIVE;
           }
        Font_Print (-1, gui_buffer, menu->entry[i]->label, x, y, color);
        x += ln + menus_opt.distance;
        }
    }
 else // Children menu (vertical) ---------------------------------------------
    {
    // Miscellaneous
    // gui.info.must_redraw = 1;
    ln = Font_Height (-1);
    gui_menu_return_children_pos (n_parent, n_parent_entry, &menu->sx, &menu->sy);

    // DRAW MENU BORDER -------------------------------------------------------
    // rectfill (gui_buffer, menu->sx - 2, menu->sy - 2, menu->sx + menu->lx + 2, menu->sy + menu->ly + 2, COLOR_SKIN_MENU_BORDER);
    // rect (gui_buffer, menu->sx - 1, menu->sy - 1, menu->sx + menu->lx + 1, menu->sy + menu->ly + 1, COLOR_SKIN_MENU_BORDER);
    rect (gui_buffer, menu->sx - 2, menu->sy - 1, menu->sx + menu->lx + 2, menu->sy + menu->ly + 1, COLOR_SKIN_MENU_BORDER);
    rect (gui_buffer, menu->sx - 1, menu->sy - 2, menu->sx + menu->lx + 1, menu->sy + menu->ly + 2, COLOR_SKIN_MENU_BORDER);

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
        if (y + ln > Configuration.video_mode_gui_res_y)
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
               color = COLOR_SKIN_MENU_TEXT;
           }
        else
           {
               color = COLOR_SKIN_MENU_TEXT_UNACTIVE;
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
        gui.info.must_redraw = TRUE;
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

