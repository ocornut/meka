//-----------------------------------------------------------------------------
// MEKA - gui.h
// Graphical User Interface (GUI) - Headers
//-----------------------------------------------------------------------------

struct s_gui_box;
typedef struct s_gui_box t_gui_box;

//-----------------------------------------------------------------------------
// Basic type
//-----------------------------------------------------------------------------

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
#include "g_box.h"
#include "g_colors.h"
#include "g_emu.h"
#include "g_tools.h"
#include "g_action.h"
#include "g_mouse.h"
#include "g_update.h"

#include "g_apps.h"
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

#define MAX_BOX                     64

#define GUI_BOX_TYPE_NOTHING        (0)
#define GUI_BOX_TYPE_BITMAP         (1)
#define GUI_BOX_TYPE_GAME           (2)

#define GUI_FB_ACCESS_DIRECT        (0)
#define GUI_FB_ACCESS_BUFFERED      (1)
#define GUI_FB_ACCESS_FLIPPED       (2)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_redraw (void);
void    gui_redraw_everything_now_once (void);
void    Redraw_Background (void);

int     gui_box_image (byte is, int which, BITMAP *bitmap);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct s_gui_box
{
    int             stupid_id;              // Avoid using id as much as possible. It is here during migration to something better.
    t_frame         frame;                  // Frame (position & size)
    int             attr;                   // Attributes
    int             type;                   // Box type (FIXME: this is crap)
    int             must_redraw;            // Boolean. Set when the box need to be redrawn in the GUI.
    char *          title;                  // Title
    void            (*update)();
    int             n_widgets;
    t_widget **     widgets;
    int             focus_inputs_exclusive; // Boolean. When set and the box has focus, inputs are exclusive to this box
}; // t_gui_box;

typedef struct
{
  int           must_redraw;    // Boolean
  int           bars_height;
  int           grid_distance;
  int           dirty_x, dirty_y;
  t_xy          screen;
  t_xy          screen_pad;
  byte          bar_gradients, bar_gradients_unused;
  byte          menu_gradients, menu_gradients_unused;
  float         bar_gradients_ratio, menu_gradients_ratio;
} t_gui_info;

typedef struct
{
  int           Initialized;
  t_gui_box *   box [MAX_BOX];
  BITMAP *      box_image [MAX_BOX];
  t_gui_info    info;
  int           box_plan [MAX_BOX];
  int           box_last;
} t_gui;

t_gui           gui;

//-----------------------------------------------------------------------------

