//-----------------------------------------------------------------------------
// MEKA - desktop.c
// Desktop Manager - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "tools/libparse.h"
#include "tools/tfile.h"

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

typedef struct
{
    char *      name;
    t_gui_box * box;
    int         pos_x;
    int         pos_y;
    bool        active;
    bool *      active_org;
} t_desktop_item;

//-----------------------------------------------------------------------------
// FORWARD DECLARATION
//-----------------------------------------------------------------------------

static void     Desktop_GetStateFromBoxes (void);
void            Desktop_SetStateToBoxes (void);

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void    Desktop_Init (void)
{
    Desktop.items = NULL;
    Desktop_Load ();
}

void    Desktop_Close (void)
{
    Desktop_GetStateFromBoxes ();
    Desktop_Save ();
}

static t_desktop_item * Desktop_Item_New(const char *name, t_gui_box *box)
{
    t_desktop_item *item;

    item        = Memory_Alloc (sizeof (t_desktop_item));
    item->name  = strdup(name);
    item->box   = box;  // Note: can be NULL at this stage

    return (item);
}

// FIXME: Desktop_Item_Delete()

void    Desktop_Register_Box(const char *name, t_gui_box *box, int default_active, bool *active_org)
{
    t_list *list;
    t_desktop_item *item;

    for (list = Desktop.items; list != NULL; list = list->next)
    {
        item = list->elem;
        if (strcmp (item->name, name) == 0)
        {
            // ConsolePrintf ("found old %s\n", name);
            item->box = box;
            item->active_org = active_org;
            // Set will be set later
            //b->frame.pos.x = item->pos_x;
            //b->frame.pos.y = item->pos_y;
            *(item->active_org) = item->active;
            //gui_box_clip_position (b);
            //gui_box_show (b, item->active, FALSE); // FIXME: Focus
            return;
        }
    }

    // ConsolePrintf ("make new %s\n", name);
    // Make new item, retrieve current data from box (unnecessary?)
    item                = Desktop_Item_New(name, box);
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
    t_list *        list;

    for (list = Desktop.items; list != NULL; list = list->next)
    {
        t_desktop_item *item = list->elem;
        if (item->box != NULL)
        {
            // Get state for this box
            item->pos_x     = item->box->frame.pos.x;
            item->pos_y     = item->box->frame.pos.y;
            item->active    = *item->active_org;
        }
    }
}

void                Desktop_SetStateToBoxes (void)
{
    t_list *        list;

    // Goes thru all boxes to set their position/active state
    for (list = Desktop.items; list != NULL; list = list->next)
    {
        t_desktop_item *item = list->elem;
        if (item->box != NULL)
        {
            // Set state for this box
            t_gui_box *b = item->box;
            b->frame.pos.x = item->pos_x;
            b->frame.pos.y = item->pos_y;
            *item->active_org = item->active;
            gui_box_clip_position (b);
            gui_box_show (b, item->active, FALSE); // FIXME: Focus
        }
    }
}

static int      Desktop_Load_Line (char *line)
{
    t_desktop_item *item;
    char *          w;
    char            name[64+1];
    char            buf[1024+1];

    if (!(w = parse_getword(name, 64, &line, ",", ';', 0)))
        return (0);

    {
        int pos_x, pos_y;
        int active;
        if (!(w = parse_getword(buf, 1024, &line, ",", ';', 0)))
            return (0);
        pos_x = atoi(w);
        if (!(w = parse_getword(buf, 1024, &line, ",", ';', 0)))
            return (0);
        pos_y = atoi(w);
        if (!(w = parse_getword(buf, 1024, &line, ",", ';', 0)))
            return (0);
        active = atoi(w);

        item = Desktop_Item_New(name, NULL);
        item->pos_x = pos_x;
        item->pos_y = pos_y;
        item->active = (bool)active;
        item->active_org = NULL;
        list_add_to_end (&Desktop.items, item);
    }

    // Ok
    return (1);
}

void                Desktop_Load (void)
{
    t_tfile *       tf;
    t_list *        lines;
    char *          line;
    int             line_cnt;

    // Open and read file
    if ((tf = tfile_read (Desktop.filename)) == NULL)
        return;

    // Parse each line
    line_cnt = 0;
    for (lines = tf->data_lines; lines; lines = lines->next)
    {
        line_cnt += 1;
        line = lines->elem;
        Desktop_Load_Line(line);
    }

    // Free file data
    tfile_free (tf);
}

void    Desktop_Save_Item (FILE *f, t_desktop_item *item)
{
    fprintf (f, "%s, %i, %i, %d\n", item->name, item->pos_x, item->pos_y, item->active);
}

void            Desktop_Save (void)
{
    FILE *      f;
    t_list *    list;

    if ((f = fopen (Desktop.filename, "wt")) == 0)
        return; // FIXME: report that somewhere ?

    // Write header
    fprintf (f, ";-----------------------------------------------------------------------------\n");
    fprintf (f, "; MEKA " VERSION " - Desktop State\n");
    fprintf (f, "; This file is automatically updated and rewritten by the emulator\n");
    fprintf (f, ";-----------------------------------------------------------------------------\n\n");

    // Write all entries
    for (list = Desktop.items; list != NULL; list = list->next)
        Desktop_Save_Item (f, list->elem);

    fprintf (f, "\n;-----------------------------------------------------------------------------\n\n");

    // Close write
    fclose (f);
}

//-----------------------------------------------------------------------------
