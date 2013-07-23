//-----------------------------------------------------------------------------
// MEKA - g_menu.h
// GUI Menus - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

typedef int t_menu_id;

#define MAX_MENUS           (256)
#define MAX_MENUS_ENTRY     (96)

#define MENU_ID_MAIN        ((t_menu_id)0)

#define MENUS_PADDING_X			(8)
#define MENUS_PADDING_Y			(4)
#define MENUS_PADDING_CHECK_X   (12)

enum t_menu_item_type
{
	MENU_ITEM_TYPE_UNKNOWN	= 0,
	MENU_ITEM_TYPE_SUB_MENU	= 1,
	MENU_ITEM_TYPE_CALLBACK	= 2,
	MENU_ITEM_TYPE_SEPARATOR = 3,
};

// Definitions for Menus Attributes
enum t_menu_item_flags
{
	MENU_ITEM_FLAG_ACTIVE	= 1<<0,
	MENU_ITEM_FLAG_CHECKED	= 1<<1,
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_init_menu();
void    gui_redraw_menus();

void    gui_draw_menu(int n_menu, int n_parent, int n_parent_entry);
void    gui_update_menu(int n_menu, int n_parent, int n_parent_entry, int generation);
void    gui_update_menus();

//-----------------------------------------------------------------------------
// Data (NEW)
// FIXME: Finish and obsolete old API
//-----------------------------------------------------------------------------

// WIP
/*
struct t_menu
{
    t_list *            items;
    // FIXME
};

enum t_menu_item_type
{
    MENU_ITEM_TYPE_SINGLE       = 1,
    MENU_ITEM_TYPE_MENU         = 2,
    MENU_ITEM_TYPE_SEPARATOR    = 3
};

enum t_menu_item_attr
{
    MENU_ITEM_ATTR_ACTIVE       = 0x0001,
    MENU_ITEM_ATTR_CHECKED      = 0x0002,
};

struct t_menu_item
{
    char *              label;                                      // Menu item label
    int                 label_msg_ref;                              // Reference to MSG ID For localization purpose	// FIXME: How to handle dynamic message with format strings?
    t_menu_item_type    type;                                       // Menu item type (single/submenu/separator)
    t_menu_item_attr    attributes;                                 // Attributes
    int                 group_id;                                   // Group ID (only 1 of a group can be checked)
    union
    {
        struct
        {
            void        (*callback)(struct t_menu_item *, void *, bool);   // Callback
        } data_single;
        struct
        {
            t_menu *    menu;
        } data_menu;
    };
    void *              user_data;
};

struct t_menu_event
{
	t_menu *			menu;
	t_menu_item *		menu_item;
	int					menu_item_idx;		// FIXME: Make obsolete only menu API doesn't need this crap anymore
	void *				user_data;
};
*/

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_gui_status_bar
{
	char  message[MSG_MAX_LEN];
	int   x;
	int   timeleft;
};

extern t_gui_status_bar g_gui_status;

struct t_gui_menus_id
{
	t_menu_id   root;
	t_menu_id	file;
	t_menu_id	machine, power, region, tvtype;
	t_menu_id	video, themes, blitters, layers, flickering, glasses, screenshots;
	t_menu_id   inputs, rapidfire;
	t_menu_id	sound, volume, rate, channels, fm;
	t_menu_id	tools;
	t_menu_id	debug, dump, dump_cfg, watch;
	t_menu_id	help;
	t_menu_id	languages;
	t_menu_id	sound_log;
};

extern t_gui_menus_id menus_ID;

struct t_menu_event;

typedef void (*t_menu_callback)(t_menu_event*);

struct t_menu_item
{
	char *				label;
	char *				shortcut;
	t_menu_item_type	type;
	unsigned int		flags;
	bool				mouse_over;
	t_menu_id			submenu_id;			// id of sub-menu if (action == 1)
	t_menu_callback		callback;			// pointer to function to execute if (action == 2)
	void *				user_data;
};

struct t_menu
{
	t_menu_id			id;
	t_menu_item *		entry[MAX_MENUS_ENTRY];
	int					n_entry;
	int                 generation;
	int                 start_pos_x, start_pos_y;
	int					size_x, size_y;
};

struct  gui_type_menus_opt
{
	int   distance;
	int   distance_usable;
	int   c_menu, c_entry;
	int   c_somewhere;
	int   c_generation;
};

extern gui_type_menus_opt menus_opt;
extern t_menu *menus[MAX_MENUS];

struct t_menu_event
{
	t_menu *		menu;
	int				menu_idx;			// FIXME: Make obsolete once menu API doesn't need this crap anymore
	t_menu_item *	menu_item;
	int				menu_item_idx;		// FIXME: Make obsolete once menu API doesn't need this crap anymore
	void *			user_data;
};

//-----------------------------------------------------------------------------

