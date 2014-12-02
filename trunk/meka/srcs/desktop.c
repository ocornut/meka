//-----------------------------------------------------------------------------
// MEKA - desktop.c
// Desktop Manager - Code
//-----------------------------------------------------------------------------
// Save position and size of windows on the desktop.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "libparse.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_desktop Desktop;

struct t_desktop_item
{
    const char *	name;
    t_gui_box *		box;
	v2i				pos;
	v2i				size;
	int				z;
    bool			active;
    bool *			active_org;
};

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

static void     Desktop_GetStateFromBoxes();

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Desktop_Init (void)
{
    Desktop.items = NULL;
    Desktop_Load();
}

void    Desktop_Close (void)
{
    Desktop_GetStateFromBoxes();
    Desktop_Save();
}

static t_desktop_item * Desktop_Item_New(const char *name, t_gui_box *box)
{
    t_desktop_item* item = (t_desktop_item*)Memory_Alloc(sizeof(t_desktop_item));

	item->name  = strdup(name);
    item->box   = box;  // Note: can be NULL at this stage
	item->pos.x = 0;
	item->pos.y = 0;
	item->size.x = 0;
	item->size.y = 0;
	item->z		 = 0;
	item->active = true;
	item->active_org = NULL;

    return item;
}

// FIXME: Desktop_Item_Delete()

void    Desktop_Register_Box(const char *name, t_gui_box *box, int default_active, bool *active_org)
{
    for (t_list* list = Desktop.items; list != NULL; list = list->next)
    {
        t_desktop_item* item = (t_desktop_item*)list->elem;
        if (strcmp(item->name, name) == 0)
        {
            // ConsolePrintf ("found old %s\n", name);
            item->box = box;
            item->active_org = active_org;
            box->frame.pos = item->pos;
			if ((box->flags & GUI_BOX_FLAGS_ALLOW_RESIZE) && (box->type != GUI_BOX_TYPE_GAME))
				if (item->size.x > 0 && item->size.y > 0)
					box->frame.size = item->size;
            *(active_org) = item->active;
            //gui_box_clip_position (b);
            //gui_box_show (b, item->active, FALSE); // FIXME: Focus
            return;
        }
    }

    // ConsolePrintf ("make new %s\n", name);
    // Make new item, retrieve current data from box (unnecessary?)
	t_desktop_item* item = (t_desktop_item*)Desktop_Item_New(name, box);
    item->pos           = box->frame.pos;
	item->size			= box->frame.size;
    item->active_org    = active_org;
    item->active        = default_active;
    *(item->active_org) = default_active;
    gui_box_show(box, item->active, FALSE); // FIXME: Focus

    // Add new item to list
    list_add(&Desktop.items, item);
}

static void	Desktop_GetStateFromBoxes()
{
    for (t_list* list = Desktop.items; list != NULL; list = list->next)
    {
        t_desktop_item* item = (t_desktop_item*)list->elem;
        if (item->box != NULL)
        {
            item->pos = item->box->frame.pos;
			item->size = item->box->frame.size;
            item->active = *item->active_org;
        }
    }
}

static int	Desktop_ItemCmpByZ(void* lhs, void* rhs)
{
	return ((t_desktop_item*)rhs)->z - ((t_desktop_item*)lhs)->z;
}

void	Desktop_SetStateToBoxes()
{
    // Goes thru all boxes to set their position/active state
	//list_sort(&Desktop.items, (t_list_cmp_handler)Desktop_ItemCmpByZ);
    for (t_list* list = Desktop.items; list != NULL; list = list->next)
    {
        t_desktop_item* item = (t_desktop_item*)list->elem;
        if (item->box != NULL)
        {
            // Set state for this box
            t_gui_box *b = item->box;
            //b->frame.pos.x = item->pos_x;
            //b->frame.pos.y = item->pos_y;
            *item->active_org = item->active;
            gui_box_clip_position(b);
            gui_box_show(b, item->active, FALSE);
			if (item->active)
				gui_box_set_focus(b);
        }
    }
}

static t_desktop_item*	item_current = NULL;

static int	Desktop_Load_Line(char *line)
{
	if (line[0] == '[')
	{
		line++;

		char name[256];
		if (!parse_getword(name, sizeof(name), &line, "]", ';', PARSE_FLAGS_DONT_EAT_SEPARATORS))
			return MEKA_ERR_SYNTAX;
		if (*line != ']')
			return MEKA_ERR_SYNTAX;

		// Create new skin
		t_desktop_item* item = Desktop_Item_New(name, NULL);
		list_add_to_end(&Desktop.items, item);
		item_current = item;
		return MEKA_ERR_OK;
	}
	else
	{
		t_desktop_item* item = item_current;

		// Read line
		char var[256];
		if (!parse_getword(var, sizeof(var), &line, "=", ';', PARSE_FLAGS_NONE))
			return MEKA_ERR_OK;
		parse_skip_spaces(&line);

		if (!item)
			return MEKA_ERR_MISSING;

		if (strcmp(var, "active") == 0)
		{
			int a;
			if (sscanf(line, "%d,%d", &a) == 1)
				item->active = a != 0;
			return MEKA_ERR_OK;
		}
		if (strcmp(var, "pos") == 0)
		{
			int x, y;
			if (sscanf(line, "%d,%d", &x, &y) == 2)
			{
				item->pos.x = x;
				item->pos.y = y;
			}
			return MEKA_ERR_OK;
		}
		if (strcmp(var, "size") == 0)
		{
			int x, y;
			if (sscanf(line, "%d,%d", &x, &y) == 2)
			{
				item->size.x = x;
				item->size.y = y;
			}
			return MEKA_ERR_OK;
		}
	}

    return MEKA_ERR_OK;
}

void	Desktop_Load()
{
    // Open and read file
	t_tfile* tf;
    if ((tf = tfile_read(Desktop.filename)) == NULL)
        return;

    // Parse each line
    int line_cnt = 0;
	item_current = NULL;
    for (t_list* lines = tf->data_lines; lines; lines = lines->next)
    {
        line_cnt += 1;
        char* line = (char*)lines->elem;
        Desktop_Load_Line(line);
    }
	item_current = NULL;

    // Free file data
    tfile_free(tf);
}

static void    Desktop_Save_Item(t_desktop_item *item, FILE *f)
{
	fprintf(f, "[%s]\n", item->name);
	fprintf(f, "active=%d\n", item->active);
	fprintf(f, "pos=%d,%d\n", item->pos.x, item->pos.y);
	if (item->box->flags & GUI_BOX_FLAGS_ALLOW_RESIZE)
		fprintf(f, "size=%d,%d\n", item->size.x, item->size.y);
	fprintf(f, "\n");
}

void	Desktop_Save()
{
	FILE * f;
    if ((f = fopen(Desktop.filename, "wt")) == 0)
        return; // FIXME: report that somewhere ?

    // Write header
    fprintf(f, ";-----------------------------------------------------------------------------\n");
    fprintf(f, "; " MEKA_NAME " " MEKA_VERSION " - Desktop State\n");
    fprintf(f, "; This file is automatically updated and rewritten by the emulator\n");
    fprintf(f, ";-----------------------------------------------------------------------------\n\n");

    // Write all entries
    for (t_list* list = Desktop.items; list != NULL; list = list->next)
    {
        t_desktop_item* item = (t_desktop_item*)list->elem;
		item->z = gui_box_find_z(item->box);
	}
	list_sort(&Desktop.items, (t_list_cmp_handler)Desktop_ItemCmpByZ);
    for (t_list* list = Desktop.items; list != NULL; list = list->next)
		Desktop_Save_Item((t_desktop_item*)list->elem, f);

    fprintf(f, ";-----------------------------------------------------------------------------\n\n");

    // Close write
    fclose (f);
}

//-----------------------------------------------------------------------------
