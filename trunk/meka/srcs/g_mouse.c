//-----------------------------------------------------------------------------
// MEKA - g_mouse.c
// GUI Mouse related things - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

void    Show_Mouse_In (void *p)
{
    if (Env.mouse_installed != -1)
    {
        show_mouse (p);
    }
}

// CHECK IF THE MOUSE IS IN A CERTAIN AREA ------------------------------------
int     gui_mouse_area (int x1, int y1, int x2, int y2)
{
    if ((gui_mouse.x >= x1) && (gui_mouse.y >= y1) && (gui_mouse.x <= x2) && (gui_mouse.y <= y2))
        return (1);
    return (0);
}

// CHECK IF A MOUSE BUTTON WAS PRESSED IN A CERTAIN AREA ----------------------
int     gui_mouse_test_area (byte b, int x1, int y1, int x2, int y2)
{
    if (gui_mouse.button & b)
        if ((gui_mouse.x >= x1) && (gui_mouse.y >= y1) && (gui_mouse.x <= x2) && (gui_mouse.y <= y2))
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

    gui_mouse.px = gui_mouse.x;
    gui_mouse.py = gui_mouse.y;
    gui_mouse.pbutton = gui_mouse.button;

    gui_mouse.x = mouse_x;
    gui_mouse.y = mouse_y;
    gui_mouse.button = mouse_b;
    // Msg (MSGT_DEBUG, "gui_mouse_button = %d", mouse_b);

    gui_mouse.z_prev = gui_mouse.z_current;
    gui_mouse.z_current = mouse_z;
    gui_mouse.z_rel = gui_mouse.z_current - gui_mouse.z_prev;

    // Uncomment to bypass Allegro 3 button emulation
    // if (gui_mouse.button == 4) gui_mouse.button = 3;

    gui_mouse.time_since_last_click ++;

    if (gui_mouse.button == 0)
    {
        gui_mouse.pressed_on = PRESSED_ON_NOTHING;
        gui_mouse.on_box = NULL;

        menus_opt.c_menu = -1;
        menus_opt.c_entry = -1;
        menus_opt.c_generation = -1;
        if (gui_mouse.reset_timer)
        {
            gui_mouse.reset_timer = 0;
            gui_mouse.time_since_last_click = 0;
        }
    }
    else
    {
        gui_mouse.reset_timer = 1;
    }
}

// INITIALIZE MOUSE VARIABLES -------------------------------------------------
void    gui_init_mouse (void)
{
    gui_mouse.button = 0;
    gui_mouse.pbutton = 0;
    gui_mouse.x = 0;
    gui_mouse.y = 0;
    gui_mouse.z_rel = 0;
    gui_mouse.z_current = gui_mouse.z_prev = 0;
    gui_mouse.px = 0;
    gui_mouse.py = 0;
    gui_mouse.on_box = NULL;
    gui_mouse.pressed_on = PRESSED_ON_NOTHING;
    gui_mouse.reset_timer = 1;
    gui_mouse.time_since_last_click = 0;
}

//-----------------------------------------------------------------------------
