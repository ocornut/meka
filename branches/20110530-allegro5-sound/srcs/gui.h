//-----------------------------------------------------------------------------
// MEKA - gui.h
// Graphical User Interface (GUI) - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Basic type
//-----------------------------------------------------------------------------

struct t_gui_box;

struct t_xy
{
    int   x;
    int   y;
};

struct t_frame
{
    t_xy  pos;
    t_xy  size;
};

//-----------------------------------------------------------------------------
// Includes other GUI files
//-----------------------------------------------------------------------------

#include "g_action.h"
#include "g_box.h"
#include "g_action.h"
#include "g_mouse.h"
#include "g_update.h"

#include "g_init.h"
#include "g_menu.h"
#include "g_menu_i.h"
#include "g_menu_t.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define DOUBLE_CLICK_SPEED          (20)    // FIXME: this is time dependent rendered-frames!!

// Frames
#define GUI_LOOK_FRAME_PAD_HEAD1_X  (6)
#define GUI_LOOK_FRAME_PAD_HEAD2_X  (2)
#define GUI_LOOK_FRAME_PAD_X        (6)
#define GUI_LOOK_FRAME_PAD1_Y       (8)
#define GUI_LOOK_FRAME_PAD2_Y       (2)
#define GUI_LOOK_FRAME_SPACING_X    (7)
#define GUI_LOOK_FRAME_SPACING_Y    (6)
// Texts
#define GUI_LOOK_LINES_SPACING_Y    (0) /* 1 or  2 */

//-----------------------------------------------------------------------------

#define GUI_BOX_MAX                 (128)

enum t_gui_box_type
{
    GUI_BOX_TYPE_STANDARD           = 0,
    GUI_BOX_TYPE_GAME               = 1,
};

enum t_gui_box_flags
{
    GUI_BOX_FLAGS_ACTIVE                    = 0x0001,
    GUI_BOX_FLAGS_DIRTY_REDRAW              = 0x0002,
    GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT   = 0x0004,
    GUI_BOX_FLAGS_FOCUS_INPUTS_EXCLUSIVE    = 0x0008,   // When set and the box has focus, inputs are exclusive to this box
    GUI_BOX_FLAGS_DELETE                    = 0x0010,
    GUI_BOX_FLAGS_TAB_STOP                  = 0x0020,
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_redraw (void);
void    gui_redraw_everything_now_once (void);

void    GUI_DrawBackground(void);
void	GUI_SetDirtyAll(void);
void    GUI_RelayoutAll(void);

int     gui_box_image (byte is, int which, ALLEGRO_BITMAP *bitmap);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef void (*t_gui_box_update_handler)();
typedef void (*t_gui_box_destroy_handler)(void *);

struct t_gui_box
{
    t_frame         frame;						// Frame (position & size)
    char *          title;						// Title
    t_gui_box_type  type;                       // Type
    int				flags;                      // Flags (t_gui_box_flags) // FIXME-ENUM
    ALLEGRO_BITMAP *gfx_buffer;					// Graphics buffer holding content render
    t_list *        widgets;                    // Widgets

    // Handlers
    void            (*update)();
    void            (*destroy)(void *user_data);

    // User data
    void *          user_data;
};

struct t_gui_info
{
  bool              must_redraw;
  int               bars_height;
  int               grid_distance;
  int               dirty_x, dirty_y;
  t_xy              screen;
  t_xy              screen_pad;
};

struct t_gui_mouse
{
    int             x;
    int             x_prev;
    int             y;
    int             y_prev;
    int             buttons;
    int             buttons_prev;
    int             time_since_last_click;
    bool            reset_timer;

    t_gui_focus     focus;
    void *          focus_item;

    int             z_rel;      // Z Relative
    int             z_current;  // Z Current
    int             z_prev;     // Z Previous
};

struct t_gui
{
    t_list *        boxes;
    t_gui_box *     boxes_z_ordered[GUI_BOX_MAX];
    int             boxes_count;
    t_gui_info      info;
    t_gui_mouse     mouse;
};

extern t_gui        gui;

//-----------------------------------------------------------------------------
