//-----------------------------------------------------------------------------
// MEKA - g_mouse.c
// GUI Mouse related things - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_mouse_show (BITMAP *bitmap)
{
    if (Env.mouse_installed == -1)
        return;
    show_mouse(bitmap);
}

// CHECK IF THE MOUSE IS IN A CERTAIN AREA ------------------------------------
int     gui_mouse_area (int x1, int y1, int x2, int y2)
{
    if ((gui.mouse.x >= x1) && (gui.mouse.y >= y1) && (gui.mouse.x <= x2) && (gui.mouse.y <= y2))
        return (1);
    return (0);
}

// CHECK IF A MOUSE BUTTON WAS PRESSED IN A CERTAIN AREA ----------------------
int     gui_mouse_test_area (byte b, int x1, int y1, int x2, int y2)
{
    if (gui.mouse.buttons & b)
        if ((gui.mouse.x >= x1) && (gui.mouse.y >= y1) && (gui.mouse.x <= x2) && (gui.mouse.y <= y2))
            return (1);
    return (0);
}

// UPDATE MOUSE VARIABLES -----------------------------------------------------
void    gui_update_mouse (void)
{
    if (Env.mouse_installed == -1)
    {
        return;
    }

    gui.mouse.x_prev = gui.mouse.x;
    gui.mouse.y_prev = gui.mouse.y;
    gui.mouse.buttons_prev = gui.mouse.buttons;

    gui.mouse.x = mouse_x;
    gui.mouse.y = mouse_y;
    gui.mouse.buttons = mouse_b;
    // Msg (MSGT_DEBUG, "gui_mouse_button = %d", mouse_b);

    gui.mouse.z_prev = gui.mouse.z_current;
    gui.mouse.z_current = mouse_z;
    gui.mouse.z_rel = gui.mouse.z_current - gui.mouse.z_prev;

    // Uncomment to bypass Allegro 3 button emulation
    // if (gui_mouse.button == 4) gui_mouse.button = 3;

    gui.mouse.time_since_last_click ++;

    if (gui.mouse.buttons == 0)
    {
        gui.mouse.focus = GUI_FOCUS_NONE;
        gui.mouse.focus_item = NULL;

        menus_opt.c_menu = -1;
        menus_opt.c_entry = -1;
        menus_opt.c_generation = -1;
        if (gui.mouse.reset_timer)
        {
            gui.mouse.reset_timer = FALSE;
            gui.mouse.time_since_last_click = 0;
        }
    }
    else
    {
        gui.mouse.reset_timer = TRUE;
    }
}

// INITIALIZE MOUSE VARIABLES -------------------------------------------------
void    gui_init_mouse (void)
{
    gui.mouse.x = 0;
    gui.mouse.y = 0;
    gui.mouse.x_prev = 0;
    gui.mouse.y_prev = 0;
    gui.mouse.buttons = 0;
    gui.mouse.buttons_prev = 0;
    gui.mouse.z_rel = 0;
    gui.mouse.z_current = 0;
    gui.mouse.z_prev = 0;
    gui.mouse.focus = GUI_FOCUS_NONE;
    gui.mouse.focus_item = NULL;
    gui.mouse.reset_timer = TRUE;
    gui.mouse.time_since_last_click = 0;
}

//-----------------------------------------------------------------------------
