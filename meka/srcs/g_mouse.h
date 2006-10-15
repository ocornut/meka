//-----------------------------------------------------------------------------
// MEKA - g_mouse.h
// GUI Mouse related things - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

typedef enum
{
    GUI_FOCUS_NONE          = 0,
    GUI_FOCUS_DESKTOP       = 1,
    GUI_FOCUS_BOX           = 2,
    GUI_FOCUS_MENUS         = 3,
    GUI_FOCUS_WIDGET        = 4,
} t_gui_focus;

/*
#define  PRESSED_ON_NOTHING   (-1)
#define  PRESSED_ON_DESKTOP   (0)
#define  PRESSED_ON_BOX       (1)
#define  PRESSED_ON_MENUS     (2)
#define  PRESSED_ON_WIDGET    (3)
*/

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_init_mouse          (void);
void    gui_update_mouse        (void);

void    gui_mouse_show          (BITMAP *bitmap);

int     gui_mouse_area          (int x1, int y1, int x2, int y2);
int     gui_mouse_test_area     (byte b, int x1, int y1, int x2, int y2);

//-----------------------------------------------------------------------------

