//-----------------------------------------------------------------------------
// MEKA - g_menu.h
// GUI Menus - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define GUI_MENUS_FONT      (F_LARGE)

#define MAX_MENUS           (256)
#define MAX_MENUS_ENTRY     (96)

#define MENU_ID_MAIN        (0)

#define MENUS_DISTANCE      (20)
#define MENUS_PADDING_X     (8)
#define MENUS_PADDING_Y     (4)

#define ITEM_NOTHING        (0)
#define ITEM_SUB_MENU       (1)
#define ITEM_EXECUTE        (2)
#define ITEM_BAR            (3)

// Definitions for Menus Attributes
#define AM_Nothing          (0)
#define AM_Active           (1)
#define AM_Checked          (2)

#define LOOK_UNKNOWN_       (-1)
#define LOOK_THIN           (0)
#define LOOK_ROUND          (1)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_init_menu (void);
void    gui_redraw_menus (void);

void    gui_draw_menu (int n_menu, int n_parent, int n_parent_entry);
void    gui_update_menu (int n_menu, int n_parent, int n_parent_entry, int generation);
void    gui_update_menus (void);

//-----------------------------------------------------------------------------
// Data (NEW)
// FIXME: Finish and obsolete old API
//-----------------------------------------------------------------------------

// WIP
/*
typedef struct
{
    t_list *            items;
    // FIXME
} t_menu;

typedef enum
{
    MENU_ITEM_TYPE_SINGLE       = 1,
    MENU_ITEM_TYPE_MENU         = 2,
    MENU_ITEM_TYPE_SEPARATOR    = 3
} t_menu_item_type;

typedef enum
{
    MENU_ITEM_ATTR_ACTIVE       = 0x0001,
    MENU_ITEM_ATTR_CHECKED      = 0x0002,
} t_menu_item_attr;

typedef struct t_menu_item
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
} t_menu_item;

typedef struct
{
	t_menu *			menu;
	t_menu_item *		menu_item;
	int					menu_item_idx;		// FIXME: Make obsolete only menu API doesn't need this crap anymore
	void *				user_data;
} t_menu_event;
*/

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct  gui_type_status
 {
  char  message [MSG_MAX_LEN];
  int   x;
  int   timeleft;
 } gui_status;

struct
 {
  int   menu;
  int   file;
  int   machine, power, country, tvtype;
  int   video, themes, blitters, layers, flickering, glasses, screenshots;
  int   inputs, rapidfire;
  int   sound, volume, rate, channels, fm, fm_emu;
  int   tools;
  int   debug, dump, dump_cfg, watch;
  int   help;
  int   languages;
  int   sound_log;
 } menus_ID;

// gui_type_menu_id menus_ID;

typedef struct
 {
  char  *label;
  char  *hotkey;
  byte  type;           // 0: nothing - 1: sub-menu - 2: execute
  byte  attr;
  byte  mouse_over;
  byte  submenu_id;     // id of submenu if (action == 1)
  void  (*event_handler)();      // pointer to function to execute if (action == 2)
  void  *user_data;
 } gui_type_menu_entry;

typedef struct
 {
  gui_type_menu_entry   *entry[MAX_MENUS_ENTRY];
  byte                  id;
  byte                  n_entry;
  int                   generation;
  int                   sx, sy, lx, ly;
 } gui_type_menu;

struct  gui_type_menus_opt
 {
  int   distance;
  int   distance_usable;
  int   c_menu, c_entry;
  int   c_somewhere;
  int   c_generation;
 } menus_opt;

gui_type_menu *menus[MAX_MENUS];

typedef struct
{
	gui_type_menu *			menu;
	int						menu_idx;			// FIXME: Make obsolete once menu API doesn't need this crap anymore
	gui_type_menu_entry *	menu_item;
	int						menu_item_idx;		// FIXME: Make obsolete once menu API doesn't need this crap anymore
	void *					user_data;
} t_menu_event;

//-----------------------------------------------------------------------------

