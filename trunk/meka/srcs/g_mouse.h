//-----------------------------------------------------------------------------
// MEKA - g_mouse.h
// GUI Mouse related things - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define  PRESSED_ON_NOTHING   (-1)
#define  PRESSED_ON_DESKTOP   (0)
#define  PRESSED_ON_BOX       (1)
#define  PRESSED_ON_MENUS     (2)
#define  PRESSED_ON_WIDGET    (3)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_init_mouse          (void);
void    gui_update_mouse        (void);

void    Show_Mouse_In           (void *p);

int     gui_mouse_area          (int x1, int y1, int x2, int y2);
int     gui_mouse_test_area     (byte b, int x1, int y1, int x2, int y2);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
  int x, px;
  int y, py;
  int button, pbutton;
  int pressed_on;  // -1: Nothing - 0: Desktop - 1: Menus - 2: Boxes
  t_gui_box *   on_box;
  int time_since_last_click;
  int reset_timer;


  int   z_rel;     // Z Relative
  int   z_current; // Z Current
  int   z_prev;    // Z Previous
} t_gui_mouse;

t_gui_mouse gui_mouse;

//-----------------------------------------------------------------------------

