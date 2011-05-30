//-----------------------------------------------------------------------------
// MEKA - g_menu_t.c
// GUI Menus Tools - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Functions - Old API, rewrite and make this obsolete
//-----------------------------------------------------------------------------

// HIGHLIGHT A MENU ENTRY -----------------------------------------------------
void    gui_menu_highlight (int menu_id, int entry_id)
{
    int    x1, x2;
    int    y1, y2;

    gui_menu_return_entry_pos (menu_id, entry_id, &x1, &y1, &x2, &y2);
	al_set_target_bitmap(gui_buffer);
    al_draw_filled_rectangle(x1 - 2, y1, x2 + 2+1, y2+1, COLOR_SKIN_MENU_SELECTION);
}

// RETURN COORDINATE OF CHILDREN MENU -----------------------------------------
void    gui_menu_return_children_pos (int p_menu, int p_entry, int *x, int *y)
{
    int    x1, x2;
    int    y1, y2;

    gui_menu_return_entry_pos (p_menu, p_entry, &x1, &y1, &x2, &y2);
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

// RETURN COORDINATES OF A MENU ENTRY -----------------------------------------
void    gui_menu_return_entry_pos (int menu_id, int n_entry, int *x1, int *y1, int *x2, int *y2)
{
 int    i;

 Font_SetCurrent (GUI_MENUS_FONT);
 if (menu_id == MENU_ID_MAIN)
    {
    *y1 = 1;
    *y2 = gui.info.bars_height - 1;
    *x1 = menus_opt.distance;
    for (i = 0; i < n_entry; i ++)
        {
        *x1 += Font_TextLength (-1, menus[menu_id]->entry[i]->label) + menus_opt.distance;
        }
    *x1 -= (menus_opt.distance_usable / 2);
    *x2 = *x1 + menus_opt.distance_usable + Font_TextLength (-1, menus[menu_id]->entry[n_entry]->label);
    }
 else
    {
    *x1 = menus[menu_id]->sx + 2;
    *x2 = menus[menu_id]->sx + menus[menu_id]->lx - 2;
    *y1 = menus[menu_id]->sy + (n_entry * (Font_Height(-1) + MENUS_PADDING_Y)) + MENUS_PADDING_Y;
    *y2 = *y1 + Font_Height(-1);
    *y1 -= 3;
    }
}

// UPDATE THE SIZE OF A MENU --------------------------------------------------
void    gui_menu_update_size (int menu_id)
{
 int    i, j;
 int    lx_max;

 lx_max = 0;
 Font_SetCurrent (GUI_MENUS_FONT);
 for (i = 0; i < menus[menu_id]->n_entry; i ++)
     {
     j = Font_TextLength (-1, menus[menu_id]->entry[i]->label);
     if (j > lx_max)
        {
        lx_max = j;
        }
     }
 menus [menu_id]->lx = lx_max + (3 * MENUS_PADDING_X);
 menus [menu_id]->ly = ((Font_Height(-1) + MENUS_PADDING_Y) * menus[menu_id]->n_entry);
}

// UPDATE THE SIZE OF ALL MENUS -----------------------------------------------
void gui_menus_update_size (void)
{
 int i;
 for (i = 0; i < MAX_MENUS; i++)
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
 int    menu_id;

 // Look for next un-allocated menu
 for (menu_id = 0; menu_id < MAX_MENUS; menu_id ++)
     if (menus[menu_id] == NULL)
        break;
 if (menu_id == MAX_MENUS)
    return (0);

 // Allocate new menu and initialize it with default values
 menus[menu_id] = (gui_type_menu *)malloc(sizeof (gui_type_menu));
 menus[menu_id]->id = menu_id;
 menus[menu_id]->sx = menus[menu_id]->sy = 0;
 menus[menu_id]->lx = menus[menu_id]->ly = 0;
 menus[menu_id]->n_entry = 0;
 menus[menu_id]->generation = -1;

 return (menu_id);
}

// ADD A MENU ITEM ------------------------------------------------------------
int                     menu_add_menu (int menu_id, const char *label, int attr)
{
 gui_type_menu          *menu;
 gui_type_menu_entry    *entry;
 int                    submenu_id;

 menu = menus[menu_id];
 if (menu->n_entry >= MAX_MENUS_ENTRY)
    {
    return (0);
    }

 submenu_id = menu_new ();
 entry = menu->entry[menu->n_entry] = (gui_type_menu_entry *)malloc(sizeof (gui_type_menu_entry));

 entry->label = strdup(label);
 entry->hotkey = NULL;
 entry->type = ITEM_SUB_MENU;
 entry->attr = attr;
 entry->mouse_over = 0;
 entry->submenu_id = submenu_id;

 menu->n_entry++;
 return (submenu_id);
}

// ADD A MENU SUBMENU ---------------------------------------------------------
int                     menu_add_item (int menu_id, const char *label, int attr, t_menu_callback callback, void *user_data)
{
 gui_type_menu *		menu;
 gui_type_menu_entry *	entry;

 menu = menus [menu_id];
 if (menu->n_entry >= MAX_MENUS_ENTRY)
    {
    return (0);
    }

 entry = menu->entry[menu->n_entry] = (gui_type_menu_entry *)malloc(sizeof (gui_type_menu_entry));
 entry->label = strdup(label);
 entry->hotkey = NULL;
 entry->type = ITEM_EXECUTE;
 entry->attr = attr;
 entry->mouse_over = 0;
 entry->callback = (t_menu_callback)callback;
 entry->user_data = user_data;
 return (menu->n_entry ++);
}

// SET ALL "MOUSE_OVER" VARIABLE TO ZERO, RECURSIVELY -------------------------
// FIXME: Make obsolete
void            gui_menu_un_mouse_over (int menu_id)
{
 int            i;
 gui_type_menu  *menu = menus [menu_id];

 for (i = 0; i < menu->n_entry; i ++)
     {
     if (menu->entry[i]->mouse_over)
        {
        gui.info.must_redraw = TRUE;
        menu->entry[i]->mouse_over = 0;
        if ((menu->entry[i]->type == ITEM_SUB_MENU) && (menu->entry[i]->attr & AM_Active))
           {
           gui_menu_un_mouse_over (menu->entry[i]->submenu_id);
           }
        }
     }
}

// SET ALL "CHECKED" ATTRIBUTES TO ZERO, RECURSIVELY --------------------------
// FIXME: Make obsolete
void            gui_menu_un_check (int menu_id)
{
 int            i;
 gui_type_menu  *menu = menus [menu_id];

 for (i = 0; i < menu->n_entry; i ++)
     {
     menu->entry[i]->attr &= (~AM_Checked);
     if ((menu->entry[i]->type == ITEM_SUB_MENU) && (menu->entry[i]->attr & AM_Active))
        {
        gui_menu_un_check (menu->entry[i]->submenu_id);
        }
     }
}

// SET ALL "CHECKED" ATTRIBUTES TO ZERO, RECURSIVELY --------------------------
// FIXME: Make obsolete
void            gui_menu_un_check_area (int menu_id, int start, int end)
{
 int            i;
 gui_type_menu  *menu = menus [menu_id];

 for (i = start; i <= end; i ++)
     {
     menu->entry[i]->attr &= (~AM_Checked);
     if ((menu->entry[i]->type == ITEM_SUB_MENU) && (menu->entry[i]->attr & AM_Active))
        {
        gui_menu_un_check (menu->entry[i]->submenu_id);
        }
     }
}

// FIXME: Make obsolete
void            gui_menu_active (int active, int menu_id, int menu_item)
{
    gui_type_menu  *menu = menus [menu_id];

    assert(menu_item >= 0 && menu_item < menu->n_entry);
    if (active)
        menu->entry[menu_item]->attr |= (AM_Active);
    else
        menu->entry[menu_item]->attr &= (~AM_Active);
}

// FIXME: Make obsolete
void            gui_menu_active_area (int active, int menu_id, int start, int end)
{
 int            i;
 gui_type_menu  *menu = menus [menu_id];

 for (i = start; i <= end; i ++)
     {
     if (active)
        menu->entry[i]->attr |= (AM_Active);
     else
        menu->entry[i]->attr &= (~AM_Active);
     }
}

// FIXME: Make obsolete
// INVERSE CHECK ATTRIBUTE OF A CERTAIN ENTRY ---------------------------------
void    gui_menu_inverse_check (int menu_id, int n_entry)
{
 menus [menu_id]->entry [n_entry]->attr ^= AM_Checked;
}

//-----------------------------------------------------------------------------

