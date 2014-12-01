//-----------------------------------------------------------------------------
// MEKA - g_mouse.c
// GUI Mouse related things - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_init_mouse (void)
{
    gui.mouse.x = 0;
    gui.mouse.y = 0;
    gui.mouse.x_prev = 0;
    gui.mouse.y_prev = 0;
    gui.mouse.buttons = 0;
    gui.mouse.buttons_prev = 0;
	gui.mouse.double_clicked = 0;
    gui.mouse.wheel_rel = 0;
    gui.mouse.wheel_abs = 0;
    gui.mouse.focus = GUI_FOCUS_NONE;
    gui.mouse.focus_box = NULL;
	gui.mouse.focus_widget = NULL;
	gui.mouse.focus_is_resizing = false;
    gui.mouse.last_click_button = 0;
	gui.mouse.last_click_time_elapsed = 9999.0f;
}

bool	gui_is_mouse_hovering_area(int x1, int y1, int x2, int y2)
{
    return ((gui.mouse.x >= x1) && (gui.mouse.y >= y1) && (gui.mouse.x <= x2) && (gui.mouse.y <= y2));
}

bool	gui_is_mouse_hovering_area(const t_frame* frame)
{
    return ((gui.mouse.x >= frame->pos.x) && (gui.mouse.y >= frame->pos.y) && (gui.mouse.x <= frame->pos.x+frame->size.x) && (gui.mouse.y <= frame->pos.y+frame->size.y));
}


void    gui_update_mouse()
{
    if (g_env.mouse_installed == -1)
        return;

    gui.mouse.x_prev = gui.mouse.x;
    gui.mouse.y_prev = gui.mouse.y;
    gui.mouse.buttons_prev = gui.mouse.buttons;

    gui.mouse.x = g_mouse_state.x;
    gui.mouse.y = g_mouse_state.y;
	gui.mouse.buttons = g_mouse_state.buttons;
    // Msg(MSGT_DEBUG, "gui_mouse_button = %d", mouse_b);

    gui.mouse.wheel_rel = g_mouse_state.z - gui.mouse.wheel_abs;
	gui.mouse.wheel_abs = g_mouse_state.z;

    // Uncomment to bypass Allegro 3 button emulation
    // if (gui_mouse.button == 4) gui_mouse.button = 3;

	for (size_t i = 0; i < 3; i++)
	{
		if ((gui.mouse.buttons & (1<<i)) && !(gui.mouse.buttons_prev & (1<<i)))
		{
			if (gui.mouse.last_click_button == (1<<i) && gui.mouse.last_click_time_elapsed < DOUBLE_CLICK_SPEED)
			{
				gui.mouse.double_clicked |= (1<<i);
				gui.mouse.last_click_time_elapsed = 9999.0f;	// So that 3rd click doesn't become a double-click
			}
			else
			{
				gui.mouse.last_click_time_elapsed = 0;
			}
			gui.mouse.last_click_button = (1<<i);
		}
		else
		{
			gui.mouse.double_clicked &= ~(1<<i);
		}
	}
	if (gui.mouse.last_click_button)
	{
		gui.mouse.last_click_time_elapsed++;
	}

    if (gui.mouse.buttons == 0)
    {
        gui.mouse.focus = GUI_FOCUS_NONE;
        gui.mouse.focus_box = NULL;
		gui.mouse.focus_widget = NULL;

        menus_opt.c_menu = -1;
        menus_opt.c_entry = -1;
        menus_opt.c_generation = -1;
    }
}

//-----------------------------------------------------------------------------
