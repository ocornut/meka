//-----------------------------------------------------------------------------
// MEKA - app_cheatfinder.c
// Cheat Finder - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_cheatfinder.h"
#include "app_memview.h"
#include "desktop.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_cheat_finder *	g_CheatFinder_MainInstance;
t_list *			g_CheatFinders;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void         CheatFinder_Layout(t_cheat_finder *app, bool setup);
static void			CheatFinder_Update(t_cheat_finder* app);

static void			CheatFinder_Switch(t_widget *w);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_cheat_finder *	CheatFinder_New(bool register_desktop)
{
	t_cheat_finder* app = (t_cheat_finder*)malloc(sizeof(t_cheat_finder));
	list_add(&g_CheatFinders, app);

	// Setup
	app->active = FALSE;

	// Create box
	t_frame frame;
	frame.pos.x     = 437;
	frame.pos.y     = 102;
	frame.size.x    = 330;
	frame.size.y    = 200;
	app->box = gui_box_new(&frame, "Cheat Finder");	// FIXME-LOCALIZATION
	app->box->user_data = app;
	app->box->destroy = (t_gui_box_destroy_handler)CheatFinder_Delete;

	// Register to desktop (applet is disabled by default)
	if (register_desktop)
		Desktop_Register_Box("CHEAT_FINDER", app->box, FALSE, &app->active);

	// Layout
	CheatFinder_Layout(app, TRUE);

	// Return new instance
	return (app);
}

void	CheatFinder_Delete(t_cheat_finder *app)
{
	list_remove(&g_CheatFinders, app);
	free(app);
}

void	CheatFinder_Layout(t_cheat_finder *app, bool setup)
{
	al_set_target_bitmap(app->box->gfx_buffer);
	al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

	// Add closebox widget
	if (setup)
		widget_closebox_add(app->box, CheatFinder_Switch);
}

void	CheatFinder_Update(t_cheat_finder* app)
{
	if (!app->active)
		return;

	// If skin has changed, redraw everything
	if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
	{
		CheatFinder_Layout(app, FALSE);
		app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
	}

	// Always dirty (ok for a developer tool)
	app->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
}

void	CheatFinder_Switch(t_widget *w)
{
	t_cheat_finder * app = (t_cheat_finder *)w->box->user_data; // Get instance
	if (app == g_CheatFinder_MainInstance)
	{
		CheatFinder_SwitchMainInstance();
	}
	else
	{
		app->active ^= 1;
		gui_box_show(app->box, app->active, TRUE);
		if (!app->active)
		{
			// Flag GUI box for deletion
			app->box->flags |= GUI_BOX_FLAGS_DELETE;
			return;
		}
	}
}

void	CheatFinder_SwitchMainInstance()
{
	t_cheat_finder *app = g_CheatFinder_MainInstance;
	app->active ^= 1;
	gui_box_show(app->box, app->active, TRUE);
	gui_menu_inverse_check(menus_ID.tools, 5);
}

//-----------------------------------------------------------------------------

void	CheatFinders_Update()
{
	for (t_list* apps = g_CheatFinders; apps != NULL; apps = apps->next)
	{
		t_cheat_finder *app = (t_cheat_finder *)apps->elem;
		CheatFinder_Update(app);
	}
}

//-----------------------------------------------------------------------------
