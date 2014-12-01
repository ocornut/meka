//-----------------------------------------------------------------------------
// MEKA - g_menu_t.c
// GUI Menus Tools - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Functions - Old API, rewrite and make this obsolete
//-----------------------------------------------------------------------------

void    gui_menu_highlight(int menu_id, int entry_id)
{
    int    x1, x2;
    int    y1, y2;

    gui_menu_return_entry_pos (menu_id, entry_id, &x1, &y1, &x2, &y2);
	al_set_target_bitmap(gui_buffer);
    al_draw_filled_rectangle(x1 - 2, y1, x2 + 2+1, y2+1, COLOR_SKIN_MENU_SELECTION);
}

void    gui_menu_return_children_pos(int p_menu, int p_entry, int *x, int *y)
{
    int    x1, x2;
    int    y1, y2;

    gui_menu_return_entry_pos(p_menu, p_entry, &x1, &y1, &x2, &y2);
    if (p_menu == MENU_ID_MAIN)
    {
        *x = x1;
        *y = y2 + 5;
    }
    else
    {
        *x = x2 + 5;
        *y = y1 - 1;
    }
}

void    gui_menu_return_entry_pos(int menu_id, int n_entry, int *x1, int *y1, int *x2, int *y2)
{
	Font_SetCurrent(FONTID_MENUS);
	t_menu* menu = menus[menu_id];
	if (menu_id == MENU_ID_MAIN)
	{
		*y1 = 1;
		*y2 = gui.info.bars_height - 1;
		*x1 = menus_opt.spacing_render;
		for (int i = 0; i < n_entry; i ++)
		{
			t_menu_item* item = menu->entry[i];
			*x1 += Font_TextLength(FONTID_CUR, item->label) + menus_opt.spacing_render;
		}
		*x1 -= (menus_opt.spacing / 2);
		*x2 = *x1 + menus_opt.spacing + Font_TextLength(FONTID_CUR, menu->entry[n_entry]->label);
	}
	else
	{
		*x1 = menu->start_pos_x + 2;
		*x2 = menu->start_pos_x + menu->size_x - 2;
		int y = menu->start_pos_y + MENUS_PADDING_Y;
		for (int i = 0; i < n_entry; i ++)
		{
			t_menu_item* item = menu->entry[i];
			y += item->type == MENU_ITEM_TYPE_SEPARATOR ? MENUS_PADDING_Y : Font_Height()+MENUS_PADDING_Y;
		}

		t_menu_item* item = menu->entry[n_entry];
		*y1 = y;
		*y2 = y + (item->type == MENU_ITEM_TYPE_SEPARATOR ? 0 : Font_Height());
		*y1 -= 3;
	}
}

// UPDATE THE SIZE OF A MENU --------------------------------------------------
void    gui_menu_update_size(int menu_id)
{
	int size_x = 0;
	int size_y = 0;
	Font_SetCurrent(FONTID_MENUS);
	
	t_menu* menu = menus[menu_id];
	for (int i = 0; i < menu->n_entry; i ++)
	{
		t_menu_item* item = menu->entry[i];
		if (item->type == MENU_ITEM_TYPE_SEPARATOR)
		{
			size_y += MENUS_PADDING_Y;
			continue;
		}
		else
		{
			const int text_w = Font_TextLength(FONTID_CUR, item->label);
			const int shortcut_w = item->shortcut ? Font_TextLength(FONTID_MEDIUM, item->shortcut) : 0;

			size_x = MAX(size_x, text_w + MENUS_PADDING_X + shortcut_w + MENUS_PADDING_CHECK_X);
			size_y += Font_Height() + MENUS_PADDING_Y;
		}
	}
	menu->size_x = size_x + (3 * MENUS_PADDING_X);
	menu->size_y = size_y;
}

void gui_menus_update_size (void)
{
	for (int i = 0; i < MAX_MENUS; i++)
	{
		if (menus[i])
		{
			gui_menu_update_size (i);
		}
	}
}

// CREATE A NEW MENU ----------------------------------------------------------
int     menu_new (void)
{
	// Look for next un-allocated menu
	int    menu_id;
	for (menu_id = 0; menu_id < MAX_MENUS; menu_id ++)
		if (menus[menu_id] == NULL)
			break;
	if (menu_id == MAX_MENUS)
		return (0);

	// Allocate new menu and initialize it with default values
	t_menu* menu = (t_menu *)malloc(sizeof (t_menu));
	menus[menu_id] = menu;
	menu->id = menu_id;
	menu->start_pos_x = menus[menu_id]->start_pos_y = 0;
	menu->size_x = menus[menu_id]->size_y = 0;
	menu->n_entry = 0;
	menu->generation = -1;

	return (menu_id);
}

int		menu_add_menu (int menu_id, const char *label, int flags)
{
	t_menu* menu = menus[menu_id];
	if (menu->n_entry >= MAX_MENUS_ENTRY)
	{
		return (0);
	}

	const int submenu_id = menu_new();
	t_menu_item* entry = menu->entry[menu->n_entry] = (t_menu_item *)malloc(sizeof (t_menu_item));

	entry->label = strdup(label);
	entry->shortcut = NULL;
	entry->type = MENU_ITEM_TYPE_SUB_MENU;
	entry->flags = flags;
	entry->mouse_over = false;
	entry->submenu_id = submenu_id;

	menu->n_entry++;
	return (submenu_id);
}

int	menu_add_item(int menu_id, const char* label, const char* shortcut, int flags, t_menu_callback callback, void *user_data)
{
	t_menu* menu = menus [menu_id];
	if (menu->n_entry >= MAX_MENUS_ENTRY)
	{
		return (0);
	}

	t_menu_item* entry = menu->entry[menu->n_entry] = (t_menu_item *)malloc(sizeof (t_menu_item));
	entry->label = strdup(label);
	entry->shortcut = shortcut ? strdup(shortcut) : NULL;
	entry->type = MENU_ITEM_TYPE_CALLBACK;
	entry->flags = flags;
	entry->mouse_over = false;
	entry->callback = (t_menu_callback)callback;
	entry->user_data = user_data;
	return (menu->n_entry ++);
}

int	menu_add_separator(int menu_id)
{
	t_menu* menu = menus [menu_id];
	if (menu->n_entry >= MAX_MENUS_ENTRY)
	{
		return (0);
	}

	t_menu_item* entry = menu->entry[menu->n_entry] = (t_menu_item *)malloc(sizeof (t_menu_item));
	entry->label = NULL;
	entry->shortcut = NULL;
	entry->type = MENU_ITEM_TYPE_SEPARATOR;
	entry->flags = 0;
	entry->mouse_over = false;
	entry->callback = NULL;
	entry->user_data = NULL;
	return (menu->n_entry ++);
}

// SET ALL "MOUSE_OVER" VARIABLE TO ZERO, RECURSIVELY -------------------------
// FIXME: Make obsolete
void	gui_menu_un_mouse_over (int menu_id)
{
	t_menu  *menu = menus [menu_id];
	for (int i = 0; i < menu->n_entry; i ++)
	{
		if (menu->entry[i]->mouse_over)
		{
			gui.info.must_redraw = TRUE;
			menu->entry[i]->mouse_over = false;
			if ((menu->entry[i]->type == MENU_ITEM_TYPE_SUB_MENU) && (menu->entry[i]->flags & MENU_ITEM_FLAG_ACTIVE))
			{
				gui_menu_un_mouse_over (menu->entry[i]->submenu_id);
			}
		}
	}
}

// SET ALL "CHECKED" ATTRIBUTES TO ZERO, RECURSIVELY --------------------------
// FIXME: Make obsolete
void	gui_menu_uncheck_all(int menu_id)
{
	t_menu  *menu = menus [menu_id];
	for (int i = 0; i < menu->n_entry; i ++)
	{
		menu->entry[i]->flags &= (~MENU_ITEM_FLAG_CHECKED);
		if ((menu->entry[i]->type == MENU_ITEM_TYPE_SUB_MENU) && (menu->entry[i]->flags & MENU_ITEM_FLAG_ACTIVE))
		{
			gui_menu_uncheck_all (menu->entry[i]->submenu_id);
		}
	}
}

// SET ALL "CHECKED" ATTRIBUTES TO ZERO, RECURSIVELY --------------------------
// FIXME: Make obsolete
void	gui_menu_uncheck_range(int menu_id, int start, int end)
{
	t_menu  *menu = menus [menu_id];
	for (int i = start; i <= end; i ++)
	{
		menu->entry[i]->flags &= (~MENU_ITEM_FLAG_CHECKED);
		if ((menu->entry[i]->type == MENU_ITEM_TYPE_SUB_MENU) && (menu->entry[i]->flags & MENU_ITEM_FLAG_ACTIVE))
		{
			gui_menu_uncheck_all (menu->entry[i]->submenu_id);
		}
	}
}

// FIXME: Make obsolete
void	gui_menu_active(int active, int menu_id, int menu_item)
{
    t_menu  *menu = menus [menu_id];

    assert(menu_item >= 0 && menu_item < menu->n_entry);
    if (active)
        menu->entry[menu_item]->flags |= (MENU_ITEM_FLAG_ACTIVE);
    else
        menu->entry[menu_item]->flags &= (~MENU_ITEM_FLAG_ACTIVE);
}

// FIXME: Make obsolete
void	gui_menu_active_range(int active, int menu_id, int start, int end)
{
	t_menu  *menu = menus [menu_id];
	for (int i = start; i <= end; i ++)
	{
		if (active)
			menu->entry[i]->flags |= (MENU_ITEM_FLAG_ACTIVE);
		else
			menu->entry[i]->flags &= (~MENU_ITEM_FLAG_ACTIVE);
	}
}

void    gui_menu_check(int menu_id, int n_entry)
{
	menus [menu_id]->entry [n_entry]->flags |= MENU_ITEM_FLAG_CHECKED;
}

// FIXME: Make obsolete
void    gui_menu_toggle_check (int menu_id, int n_entry)
{
	menus [menu_id]->entry [n_entry]->flags ^= MENU_ITEM_FLAG_CHECKED;
}

//-----------------------------------------------------------------------------

