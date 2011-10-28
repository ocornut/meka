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

static const char*	s_value_type_names[] =
{
	"U8",
	"S8",
};

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void         CheatFinder_Layout(t_cheat_finder *app, bool setup);
static void			CheatFinder_Update(t_cheat_finder* app);

static void			CheatFinder_ResetMatches(t_cheat_finder* app);
static void			CheatFinder_ReduceMatches(t_cheat_finder* app);

static void			CheatFinder_CallbackMemtypeSelect(t_widget* w);
static void			CheatFinder_CallbackValuetypeSelect(t_widget* w);
static void			CheatFinder_CallbackReset(t_widget* w);
static void			CheatFinder_CallbackReduce(t_widget* w);
static void			CheatFinder_CallbackClose(t_widget* w);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_cheat_finder *	CheatFinder_New(bool register_desktop)
{
	t_cheat_finder* app = new t_cheat_finder();
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

	app->memtype		= MEMTYPE_RAM;
	app->valuetype		= CHEAT_FINDER_VALUE_TYPE_U8;

	app->matches.clear();
	app->reset_state	= true;

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
		widget_closebox_add(app->box, CheatFinder_CallbackClose);

	// Setup Memory sections
	FontPrinter fp(F_MIDDLE);
	DrawCursor dc(v2i(5,0), F_MIDDLE);
	dc.y_spacing += 4;

	fp.Printf(dc.pos+v2i(0,4), "Memory region:");
	dc.pos.x += 90;

	if (setup)
	{
		for (int i = 0; i != MEMTYPE_MAX_; i++)
		{
			t_memory_type memtype = (t_memory_type)i;
			t_memory_range memrange;
			MemoryRange_GetDetails(memtype, &memrange);

			t_frame frame(dc.pos, v2i(30,Font_Height(F_SMALL) + 3));
			app->w_memtype_buttons[memtype] = widget_button_add(app->box, &frame, 1, CheatFinder_CallbackMemtypeSelect, WIDGET_BUTTON_STYLE_SMALL, (const char *)memrange.name, (void*)memtype);
			dc.pos.x += frame.size.x + 2;
		}
	}
	dc.NewLine();
	dc.HorizontalSeparator();

	fp.Printf(dc.pos+v2i(0,4), "Variable type:");
	dc.pos.x += 90;
	if (setup)
	{
		for (int i = 0; i != CHEAT_FINDER_VALUE_TYPE_MAX_; i++)
		{
			t_frame frame(dc.pos, v2i(30,Font_Height(F_SMALL)+3));
			app->w_valuetype_buttons[i] = widget_button_add(app->box, &frame, 1, CheatFinder_CallbackValuetypeSelect, WIDGET_BUTTON_STYLE_SMALL, (const char *)s_value_type_names[i], (void*)i);
			dc.pos.x += frame.size.x + 2;
		}
	}
	dc.NewLine();
	dc.HorizontalSeparator();

	if (setup)
	{
		t_frame frame(dc.pos, v2i(80,Font_Height(F_SMALL)+3));
		widget_button_add(app->box, &frame, 1, (t_widget_callback)CheatFinder_CallbackReset, WIDGET_BUTTON_STYLE_SMALL, "RESET SEARCH");	// FIXME-LOCALIZATION
	}

	app->matches_frame.SetPos(92,dc.pos.y);
	app->matches_frame.SetSize(app->box->frame.size - app->matches_frame.pos);
	al_draw_line(91+0.5f,dc.pos.y-2,91+0.5f,app->box->frame.size.y+1, COLOR_SKIN_WINDOW_SEPARATORS, 0);

	dc.NewLine();
	dc.NewLine();

	fp.Printf(dc.pos+v2i(0,4), "Value:");
	dc.pos.x = 46;
	if (setup)
	{
		t_frame frame(dc.pos, v2i(40,Font_Height(F_SMALL)+3));
		app->w_custom_value = widget_inputbox_add(app->box, &frame, 6, F_MIDDLE, CheatFinder_CallbackReduce);
		widget_inputbox_set_content_type(app->w_custom_value, WIDGET_CONTENT_TYPE_DECIMAL);
	}

	dc.NewLine();
	dc.pos.y += 2;
	if (setup)
	{
		t_frame frame(dc.pos, v2i(80,Font_Height(F_SMALL)+3));
		app->w_reduce_search = widget_button_add(app->box, &frame, 1, (t_widget_callback)CheatFinder_CallbackReduce, WIDGET_BUTTON_STYLE_SMALL, "START");		// FIXME-LOCALIZATION
	}
}

void	CheatFinder_Update(t_cheat_finder* app)
{
	if (!app->active)
		return;

	al_set_target_bitmap(app->box->gfx_buffer);

	// If skin has changed, redraw everything
	if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
	{
		CheatFinder_Layout(app, FALSE);
		app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
	}

	for (int i = 0; i != MEMTYPE_MAX_; i++)
		widget_button_set_selected(app->w_memtype_buttons[i], app->memtype == i);

	for (int i = 0; i != CHEAT_FINDER_VALUE_TYPE_MAX_; i++)
		widget_button_set_selected(app->w_valuetype_buttons[i], app->valuetype == i);

	widget_button_set_label(app->w_reduce_search, app->reset_state ? "START" : "REDUCE");	// FIXME-LOCALIZATION

	// Always dirty (ok for a developer tool)
	app->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

	al_draw_filled_rectangle(app->matches_frame.pos.x, app->matches_frame.pos.y, app->matches_frame.pos.x+app->matches_frame.size.x, app->matches_frame.pos.y+app->matches_frame.size.y, COLOR_SKIN_WINDOW_BACKGROUND);

	FontPrinter fp(F_MIDDLE);
	DrawCursor dc(app->matches_frame.pos+v2i(5,4), F_MIDDLE);

	fp.Printf(dc.pos, (app->matches.size()>1) ? "%d matches" : "%d match", app->matches.size());
	dc.NewLine();
}

void CheatFinder_ResetMatches(t_cheat_finder* app)
{
	app->reset_state = true;
	app->matches.clear();
}

void CheatFinder_ReduceMatches(t_cheat_finder* app)
{
	t_memory_range memrange;
	MemoryRange_GetDetails(app->memtype, &memrange);

	if (app->reset_state)
	{
		for (int i = 0; i != memrange.size; i++)
		{
			u8 v = memrange.ReadByte(i);
			app->matches[i] = v;
		}
	}
	app->reset_state = false;
}

static void CheatFinder_CallbackMemtypeSelect(t_widget* w)
{
	t_cheat_finder* app = (t_cheat_finder*)w->box->user_data; // Get instance
	app->memtype = (t_memory_type)(int)w->user_data;
}

static void CheatFinder_CallbackValuetypeSelect(t_widget* w)
{
	t_cheat_finder* app = (t_cheat_finder*)w->box->user_data; // Get instance
	app->valuetype = (t_cheat_finder_value_type)(int)w->user_data;
}

static void CheatFinder_CallbackReset(t_widget* w)
{
	t_cheat_finder* app = (t_cheat_finder*)w->box->user_data; // Get instance
	CheatFinder_ResetMatches(app);
}

static void CheatFinder_CallbackReduce(t_widget* w)
{
	t_cheat_finder* app = (t_cheat_finder*)w->box->user_data; // Get instance
	if (w != app->w_reduce_search)
		widget_button_trigger(app->w_reduce_search);
	else
		CheatFinder_ReduceMatches(app);
}

static void	CheatFinder_CallbackClose(t_widget* w)
{
	t_cheat_finder* app = (t_cheat_finder*)w->box->user_data; // Get instance
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
