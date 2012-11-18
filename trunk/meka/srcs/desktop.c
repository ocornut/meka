//-----------------------------------------------------------------------------
// MEKA - desktop.c
// Desktop Manager - Code
//-----------------------------------------------------------------------------
// Save position and size of windows on the desktop.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "tools/libparse.h"
#include "tools/tfile.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_desktop Desktop;

struct t_desktop_item
{
    const char *	name;
    t_gui_box *		box;
    int				pos_x;
    int				pos_y;
	int				z;
    bool			active;
    bool *			active_org;
};

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

static void     Desktop_GetStateFromBoxes (void);
void            Desktop_SetStateToBoxes (void);

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
    t_desktop_item* item;

    item        = (t_desktop_item*)Memory_Alloc (sizeof (t_desktop_item));
    item->name  = strdup(name);
    item->box   = box;  // Note: can be NULL at this stage

    return (item);
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
            box->frame.pos.x = item->pos_x;
            box->frame.pos.y = item->pos_y;
            *(item->active_org) = item->active;
            //gui_box_clip_position (b);
            //gui_box_show (b, item->active, FALSE); // FIXME: Focus
            return;
        }
    }

    // ConsolePrintf ("make new %s\n", name);
    // Make new item, retrieve current data from box (unnecessary?)
	t_desktop_item* item;
    item                = (t_desktop_item*)Desktop_Item_New(name, box);
    item->pos_x         = box->frame.pos.x;
    item->pos_y         = box->frame.pos.y;
    item->active_org    = active_org;
    item->active        = default_active;
    *(item->active_org) = default_active;
    gui_box_show(box, item->active, FALSE); // FIXME: Focus

    // Add new item to list
    list_add (&Desktop.items, item);
}

static void         Desktop_GetStateFromBoxes (void)
{
    for (t_list* list = Desktop.items; list != NULL; list = list->next)
    {
        t_desktop_item* item = (t_desktop_item*)list->elem;
        if (item->box != NULL)
        {
            // Get state for this box
            item->pos_x     = item->box->frame.pos.x;
            item->pos_y     = item->box->frame.pos.y;
            item->active    = *item->active_org;
        }
    }
}

static int	Desktop_ItemCmpByZ(void* lhs, void* rhs)
{
	return ((t_desktop_item*)rhs)->z - ((t_desktop_item*)lhs)->z;
}

void	Desktop_SetStateToBoxes(void)
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

static int	Desktop_Load_Line(char *line)
{
    char *  w;
    char    name[64+1];
    char	buf[256+1];

    if (!(w = parse_getword(name, countof(name), &line, ",", ';')))
        return (0);

    if (!(w = parse_getword(buf, countof(buf), &line, ",", ';')))
        return (0);
    const int pos_x = atoi(w);
    if (!(w = parse_getword(buf, countof(buf), &line, ",", ';')))
        return (0);
    const int pos_y = atoi(w);
    if (!(w = parse_getword(buf, countof(buf), &line, ",", ';')))
        return (0);
    const int active = atoi(w);

    t_desktop_item *item = Desktop_Item_New(name, NULL);
    item->pos_x = pos_x;
    item->pos_y = pos_y;
	item->z = list_size(Desktop.items);
    item->active = (bool)active;
    item->active_org = NULL;
    list_add_to_end(&Desktop.items, item);

    // Ok
    return (1);
}

void	Desktop_Load (void)
{
    t_tfile* tf;

    // Open and read file
    if ((tf = tfile_read (Desktop.filename)) == NULL)
        return;

    // Parse each line
    int line_cnt = 0;
    for (t_list* lines = tf->data_lines; lines; lines = lines->next)
    {
        line_cnt += 1;
        char* line = (char*)lines->elem;
        Desktop_Load_Line(line);
    }

    // Free file data
    tfile_free(tf);
}

static void    Desktop_Save_Item(t_desktop_item *item, FILE *f)
{
    fprintf(f, "%s, %i, %i, %d\n", item->name, item->pos_x, item->pos_y, item->active);
}

void	Desktop_Save(void)
{
    FILE *      f;

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

    fprintf(f, "\n;-----------------------------------------------------------------------------\n\n");

    // Close write
    fclose (f);
}

//-----------------------------------------------------------------------------
