//-----------------------------------------------------------------------------
// MEKA - g_mouse.h
// GUI Mouse related things - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

enum t_gui_focus
{
    GUI_FOCUS_NONE          = 0,
    GUI_FOCUS_DESKTOP       = 1,
    GUI_FOCUS_BOX           = 2,
    GUI_FOCUS_MENUS         = 3,
    GUI_FOCUS_WIDGET        = 4,
};

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

void    gui_init_mouse(void);
void    gui_update_mouse(void);
bool    gui_is_mouse_hovering_area(int x1, int y1, int x2, int y2);
bool    gui_is_mouse_hovering_area(const t_frame* frame);

//-----------------------------------------------------------------------------

