//-----------------------------------------------------------------------------
// MEKA - app_cheatfinder.h
// Cheat Finder - Headers
//-----------------------------------------------------------------------------

#include "app_memview.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

enum t_cheat_finder_value_type
{
	CHEAT_FINDER_VALUE_TYPE_S8,
	CHEAT_FINDER_VALUE_TYPE_U8,
	CHEAT_FINDER_VALUE_TYPE_MAX_,
};

struct t_cheat_finder
{
	bool						active;
	t_gui_box*					box;

	t_memory_type				memtype;
	t_cheat_finder_value_type	valuetype;

	t_widget*					w_memtype_buttons[MEMTYPE_MAX_];
	t_widget*					w_valuetype_buttons[CHEAT_FINDER_VALUE_TYPE_MAX_];
	t_widget*					w_custom_value;
	t_widget*					w_reduce_search;

	bool						reset_state;
	std::map<int,u32>			matches;

	t_frame						matches_frame;
};

extern t_cheat_finder *		g_CheatFinder_MainInstance;
extern t_list *				g_CheatFinders;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_cheat_finder *			CheatFinder_New(bool register_desktop);
void                        CheatFinder_Delete(t_cheat_finder* app);
void                        CheatFinder_SwitchMainInstance(void);

void                        CheatFinders_Update(void);

//-----------------------------------------------------------------------------
