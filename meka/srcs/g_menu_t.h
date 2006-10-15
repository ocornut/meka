//-----------------------------------------------------------------------------
// MEKA - g_menu_t.h
// GUI Menus Tools - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// new names
int     menu_new (void);
int     menu_add_item (int n_menu, char *label, int attr, void (*pnt)());
int     menu_add_menu (int n_menu, char *label, int attr);

// old names
void    gui_menu_highlight              (int menu_id, int entry_id);
void    gui_menu_return_children_pos    (int p_menu, int p_entry, int *x, int *y);
void    gui_menu_return_entry_pos       (int menu_id, int n_entry, int *x1, int *y1, int *x2, int *y2);
void    gui_menu_update_size            (int menu_id);
void    gui_menus_update_size           (void);

void    gui_menu_un_check               (int menu_id);
void    gui_menu_un_check_area          (int menu_id, int start, int end);
#define gui_menu_check(menu_id,n_entry)         do { menus[(menu_id)]->entry[(n_entry)]->attr |= AM_Checked; } while (0)
#define gui_menu_un_check_one(menu_id,n_entry)  do { menus[(menu_id)]->entry[(n_entry)]->attr &= ~AM_Checked; } while (0)
void    gui_menu_inverse_check          (int menu_id, int n_entry);

void    gui_menu_active                 (int active, int menu_id, int menu_item);
void    gui_menu_active_area            (int active, int menu_id, int start, int end);

void    gui_menu_un_mouse_over          (int menu_id);

//-----------------------------------------------------------------------------

