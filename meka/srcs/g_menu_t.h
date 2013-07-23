//-----------------------------------------------------------------------------
// MEKA - g_menu_t.h
// GUI Menus Tools - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// new names
int     menu_new();
int     menu_add_menu(int n_menu, const char *label, int attr);
int     menu_add_item(int n_menu, const char *label, const char* hotkey, int attr, t_menu_callback callback, void *user_data);
int     menu_add_separator(int n_menu);

// old names
void    gui_menu_highlight              (int menu_id, int entry_id);
void    gui_menu_return_children_pos    (int p_menu, int p_entry, int *x, int *y);
void    gui_menu_return_entry_pos       (int menu_id, int n_entry, int *x1, int *y1, int *x2, int *y2);
void    gui_menu_update_size            (int menu_id);
void    gui_menus_update_size           ();

void    gui_menu_uncheck_all            (int menu_id);
void    gui_menu_uncheck_range          (int menu_id, int start, int end);
void	gui_menu_check					(int menu_id, int n_entry);
void    gui_menu_toggle_check           (int menu_id, int n_entry);

void    gui_menu_active                 (int active, int menu_id, int menu_item);
void    gui_menu_active_range           (int active, int menu_id, int start, int end);

void    gui_menu_un_mouse_over          (int menu_id);

//-----------------------------------------------------------------------------

