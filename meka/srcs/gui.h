//-----------------------------------------------------------------------------
// MEKA - gui.h
// Graphical User Interface (GUI) - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Basic type
//-----------------------------------------------------------------------------

typedef struct s_gui_box    t_gui_box;

typedef struct
{
    int   x;
    int   y;
} t_xy;

typedef struct
{
    t_xy  pos;
    t_xy  size;
} t_frame;

//-----------------------------------------------------------------------------
// Includes other GUI files
//-----------------------------------------------------------------------------

#include "g_action.h"
#include "g_applet.h"
#include "g_box.h"
#include "g_emu.h"
#include "g_tools.h"
#include "g_action.h"
#include "g_mouse.h"
#include "g_update.h"

#include "g_init.h"
#include "g_menu.h"
#include "g_menu_i.h"
#include "g_menu_t.h"

//-----------------------------------------------------------------------------
// GUI non-shared includes
// Those are listed here for reference purposes, but should be included
// manually by each file using them.
//-----------------------------------------------------------------------------

// #include "g_file.h"      // G_FILE.H     File Browser
// #include "g_widget.h"    // G_WIDGET.H   Widgets

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

#define GUI_FB_ACCESS_DIRECT        (0)
#define GUI_FB_ACCESS_BUFFERED      (1)
#define GUI_FB_ACCESS_FLIPPED       (2)

typedef enum
{
    GUI_BOX_TYPE_STANDARD           = 0,
    GUI_BOX_TYPE_GAME               = 1,
} t_gui_box_type;

typedef enum
{
    GUI_BOX_FLAGS_ACTIVE                    = 0x0001,
    GUI_BOX_FLAGS_DIRTY_REDRAW              = 0x0002,
    GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT   = 0x0004,
    GUI_BOX_FLAGS_FOCUS_INPUTS_EXCLUSIVE    = 0x0008,   // When set and the box has focus, inputs are exclusive to this box
    GUI_BOX_FLAGS_DELETE                    = 0x0010,
    GUI_BOX_FLAGS_TAB_STOP                  = 0x0020,
} t_gui_box_flags;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_redraw (void);
void    gui_redraw_everything_now_once (void);
void    Redraw_Background (void);

void    gui_relayout(void);

int     gui_box_image (byte is, int which, BITMAP *bitmap);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct s_gui_box
{
    t_frame         frame;						// Frame (position & size)
    char *          title;						// Title
    t_gui_box_type  type;                       // Type
    t_gui_box_flags flags;                      // Flags/Attributes
    BITMAP *		gfx_buffer;					// Graphics buffer holding content render
    t_list *        widgets;                    // Widgets

    // Handlers
    void            (*update)();
    void            (*destroy)(void *user_data);

    // User data
    void *          user_data;
}; // t_gui_box

typedef struct
{
  bool              must_redraw;
  int               bars_height;
  int               grid_distance;
  int               dirty_x, dirty_y;
  t_xy              screen;
  t_xy              screen_pad;
} t_gui_info;

typedef struct
{
    t_list *        boxes;
    t_gui_box *     boxes_z_ordered[GUI_BOX_MAX];
    int             boxes_count;
    t_gui_info      info;
} t_gui;

t_gui           gui;

//-----------------------------------------------------------------------------
