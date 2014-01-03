//-----------------------------------------------------------------------------
// MEKA - g_file.c
// GUI File Browser - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_filebrowser.h"
#include "desktop.h"
#include "g_tools.h"
#include "g_widget.h"
#include "db.h"
#include "file.h"
#include "inputs_t.h"
#include "vlfn.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_filebrowser    FB;

//-----------------------------------------------------------------------------
// Definitions (private)
//-----------------------------------------------------------------------------

#define FB_BUTTON_X             (60)
#define FB_BUTTON_Y             (27)
#define FB_MINI_BUTTON_X        (9)
#define FB_MINI_BUTTON_Y        (9)
#define FB_PAD_X                (6)
#define FB_PAD_Y                (6)
#define FB_TEXT_PAD_X           (12)
#define FB_TEXT_PAD_Y           (10)
#define FB_SCROLL_X             (10)

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void		FB_Layout               (t_filebrowser *app, bool setup);

//...

static int		FB_Return_File_Area_Y   (void);
static int		FB_Return_Res_Y         (void);

static void		FB_Draw_List            (void);
static void		FB_Draw_Infos           (void);
static void		FB_Click_List           (t_widget *w);
static void		FB_Sort_Files           (int start, int end);
static void		FB_Check_and_Repos      (void);

static bool		FB_SelectEntryByFileName	(const char* name);
static void		FB_OpenSelectedEntry		(void);
static void		FB_OpenDirectory			(const char* name);
static void		FB_OpenFile					(const char* name);
static void		FB_LoadAllNames				(void);

static t_filebrowser_entry *   FB_Entry_New        (int type, char *file_name);
static void                    FB_Entry_Delete     (t_filebrowser_entry *entry);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FB_Entry_New (int type, char *filename)
// Create a new file browser entry
//-----------------------------------------------------------------------------
t_filebrowser_entry *       FB_Entry_New (int type, char *file_name)
{
    t_filebrowser_entry* entry;
	entry = (t_filebrowser_entry*)malloc(sizeof(t_filebrowser_entry));
    entry->type             = type;
    entry->file_name        = file_name;
    entry->db_entry         = NULL;
    entry->db_entry_name    = NULL;
    return (entry);
}

//-----------------------------------------------------------------------------
// FB_Entry_Delete (t_filebrowser_entry *entry)
// Delete given file browser entry
//-----------------------------------------------------------------------------
void    FB_Entry_Delete(t_filebrowser_entry *entry)
{
    free (entry->file_name);
    free (entry);
}

//-----------------------------------------------------------------------------
// FB_Entry_FindVLFN (t_filebrowser_entry *entry)
// Find VLFN data associated to given entry file name, and associate data
//-----------------------------------------------------------------------------
void                FB_Entry_FindVLFN(t_filebrowser_entry *entry)
{
    t_vlfn_entry *  vlfn_entry;
    vlfn_entry = VLFN_FindByFileName(entry->file_name);
    if (vlfn_entry)
    {
        entry->db_entry         = vlfn_entry->db_entry;
        entry->db_entry_name    = DB_Entry_GetCurrentName (entry->db_entry);    // pre-get
    }
}

//-----------------------------------------------------------------------------

int     FB_Return_File_Area_Y()
{
    return ((FB.file_y * Font_Height(F_LARGE)) + 5);
}

int     FB_Return_Res_Y()
{
    return (FB_Return_File_Area_Y() + (3 * FB_PAD_Y) + FB_BUTTON_Y);
}

void    FB_Switch()
{
    FB.active ^= 1;
    gui_box_show(FB.box, FB.active, TRUE);
    gui_menu_toggle_check(menus_ID.file, 0);
}

void    FB_Init_Values()
{
    FB.res_x    = 340;
    FB.file_y   = 21;
}

void	FB_Init()
{
    t_frame     frame;

    frame.pos.x     = 454;
    frame.pos.y     = 54;
    frame.size.x    = FB.res_x;
    frame.size.y    = FB_Return_Res_Y();
    FB.box = gui_box_new(&frame, Msg_Get(MSG_FileBrowser_BoxTitle));
    Desktop_Register_Box("FILEBROWSER", FB.box, 0, &FB.active);

    // Set exclusive inputs flag to avoid messing with emulation
    FB.box->flags |= GUI_BOX_FLAGS_FOCUS_INPUTS_EXCLUSIVE;

    // Add close box widget
    widget_closebox_add(FB.box, (t_widget_callback)FB_Switch);

    // Layout
    FB_Layout(&FB, TRUE);
}

void    FB_Init_2()
{
    chdir(FB.current_directory);
    FB_Load_Directory();
}

void	FB_Layout(t_filebrowser *app, bool setup)
{
    al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    // Setup widgets
    if (setup)
    {
        t_frame frame; 

        // Add scrollbar
        frame.pos.x = FB.res_x - FB_PAD_X - FB_SCROLL_X + 1;
        frame.pos.y = FB_PAD_Y + 2;
        frame.size.x = FB_SCROLL_X - 3;
        frame.size.y = FB_Return_File_Area_Y () - 4;
        app->widget_scrollbar = widget_scrollbar_add(FB.box, WIDGET_SCROLLBAR_TYPE_VERTICAL, &frame, &FB.files_max, &FB.file_display_first, FB.file_y, (t_widget_callback)FB_Draw_List);

        // Add an invisible 'button' to catch click on the list
        // (currently the GUI doesn't handle list/combo)
        frame.pos.x = FB_PAD_X + 2;
        frame.pos.y = FB_PAD_Y + 2;
        frame.size.x = FB.res_x - (2 * FB_PAD_X) - FB_SCROLL_X - 4;
        frame.size.y = FB_Return_File_Area_Y () - 6;
        widget_button_add(FB.box, &frame, 1, (t_widget_callback)FB_Click_List, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);

        // Add 'CLOSE' button
        //frame.pos.x = FB_BUTTON_X + 10;
        frame.pos.x = FB.res_x - FB_BUTTON_X - 10;
        frame.pos.y = FB_Return_File_Area_Y () + (2 * FB_PAD_Y);
        frame.size.x = FB_BUTTON_X;
        frame.size.y = FB_BUTTON_Y;
        widget_button_add(FB.box, &frame, 1, (t_widget_callback)FB_Switch, WIDGET_BUTTON_STYLE_LARGE, Msg_Get(MSG_FileBrowser_Close));

        // Add 'LOAD' button
        frame.pos.x -= FB_BUTTON_X + 10;
        widget_button_add(FB.box, &frame, 1, (t_widget_callback)FB_OpenSelectedEntry, WIDGET_BUTTON_STYLE_LARGE, Msg_Get(MSG_FileBrowser_Load));

        // Add small 'LOAD NAMES' button
        frame.pos.x = FB_PAD_X;
        frame.pos.y = FB_Return_File_Area_Y () + (2 * FB_PAD_Y) + Font_Height (F_MEDIUM) + 6;
        frame.size.x = 54;
        frame.size.y = Font_Height (F_SMALL) + 3;
        widget_button_add(FB.box, &frame, 1, (t_widget_callback)FB_LoadAllNames, WIDGET_BUTTON_STYLE_SMALL, Msg_Get(MSG_FileBrowser_LoadNames));

        // Add small 'RELOAD DIR' button
        frame.pos.x += frame.size.x + 1;
        widget_button_add(FB.box, &frame, 1, (t_widget_callback)FB_Load_Directory, WIDGET_BUTTON_STYLE_SMALL, Msg_Get(MSG_FileBrowser_ReloadDir));
    }

    // Outer frame
	gui_rect(LOOK_ROUND, FB_PAD_X, FB_PAD_Y, FB.res_x - FB_PAD_X, FB_PAD_Y + FB_Return_File_Area_Y (), COLOR_SKIN_WIDGET_LISTBOX_BORDER);
}

void        FB_Free_Memory(void)
{
    for (int i = 0; i < FB.files_max; i ++)
        FB_Entry_Delete(FB.files[i]);
    free(FB.files);
}

void		FB_Close()
{
	FB_Free_Memory();
}

#ifdef ARCH_WIN32

void		FB_Add_DiskDrives(void)
{
    for (int i = 2; i < 26; i ++)   // C: to Z:
    {
        if (GetLogicalDrives() & (1 << i))
        {
            // Create a new file browser entry of disk type
            t_filebrowser_entry *entry;
		    char buf[16];
            sprintf(buf, "%c:", 'A' + i);
            entry = FB_Entry_New(FB_ENTRY_TYPE_DRIVE, strdup(buf));

            // Add to list (FIXME: arghh.)
            FB.files[FB.files_max] = entry;
            FB.files_max ++;
			FB.files = (t_filebrowser_entry**)realloc(FB.files, (FB.files_max + 1) * sizeof(t_filebrowser_entry *));
        }
    }
}

#endif

static INLINE int   FB_Sort_Files_GetEntryPriority (t_filebrowser_entry *entry)
{
    const t_db_entry *    db_entry = entry->db_entry;
    if (!db_entry)
        return (-1);

    // Priority order : 
    // - no country flag
    // - country flag
    // - proto
    // - BIOS
    // - translation
    // - hacks
    // - bad
    int p = DB_Entry_SelectDisplayFlag(db_entry);
    if (db_entry->flags & DB_FLAG_BIOS)
        p += 500;
    if (db_entry->flags & DB_FLAG_PROTO)
        p += 1000;
    if (db_entry->flags & DB_FLAG_TRANS)
        p += 2000 + db_entry->trans_country;
    if (db_entry->flags & DB_FLAG_HACK)
        p += 4000;
    if (db_entry->flags & DB_FLAG_BAD)
        p += 8000;
    return (p);
}

// FIXME-OPT: bubble sort is slow
void        FB_Sort_Files(int start, int end)
{
    for (; start < end - 1; start ++)
	{
        for (int i = end - 1; i > start; i --)
        {
            t_filebrowser_entry *e1 = FB.files[i];
            t_filebrowser_entry *e2 = FB.files[i - 1];

            // Compare
            const char *e1_name = e1->db_entry_name ? e1->db_entry_name : e1->file_name;
            const char *e2_name = e2->db_entry_name ? e2->db_entry_name : e2->file_name;
            int d = stricmp (e1_name, e2_name);

            // Only if names are equal, sorting order depends on DB data (flags/bad marker, etc...)
            if (d == 0)
            {
                int e1_p = FB_Sort_Files_GetEntryPriority(e1);
                int e2_p = FB_Sort_Files_GetEntryPriority(e2);
                d = (e1_p - e2_p);
            }

            // Swap!
            if (d < 0)
            {
                FB.files [i] = e2;
                FB.files [i - 1] = e1;
            }
        }
	}
}

int                 FB_Ext_In_List (const t_list *ext_list, const char *ext)
{
    while (ext_list)
    {
        if (stricmp ((char*)ext_list->elem, ext) == 0)
            return (1);
        ext_list = ext_list->next;
    }
    return (0);
}

void                FB_Add_Entries (t_list *ext_list, int type)
{
#if defined(ARCH_UNIX) || defined(ARCH_MACOSX)
    DIR *           dir;
    struct dirent * dirent;
    struct stat	    dirent_stat;

    // Open current directory
    if ((dir = opendir(".")) == 0)
    {
        Msg(MSGT_USER, Msg_Get(MSG_Error_Directory_Open));
        return;
    }

    // Check all files
    while ((dirent = readdir(dir)))
    {
        char *name = dirent->d_name;
        if (stat (name, &dirent_stat) != 0)
            continue;

        if (type == FB_ENTRY_TYPE_DIRECTORY && (dirent_stat.st_mode & S_IFMT) == S_IFDIR)
        {
            // Skip '.'
            if (name[0] == '.' && name[1] == '\0') // if (!strcmp(name, "."))
                continue;
        }
        else if (type == FB_ENTRY_TYPE_FILE && (dirent_stat.st_mode & S_IFMT) == S_IFREG)
        {
            const char *ext = strrchr (name, '.');
            if (ext == NULL) 
                ext = "";
            if (!FB_Ext_In_List (ext_list, ext + 1))
                continue;
        }
        else
		{
            continue;
		}

        // Create a new file browser entry of given type
        t_filebrowser_entry *entry = FB_Entry_New (type, strdup (name));
        if (g_configuration.fb_uses_DB)
            FB_Entry_FindVLFN (entry);

        // Add to list (FIXME: argh)
	    FB.files[FB.files_max] = entry;
        FB.files_max ++;
        FB.files = (t_filebrowser_entry**)realloc (FB.files, (FB.files_max + 1) * sizeof (t_filebrowser_entry *));
    }

    closedir (dir);

#else // ARCH_WIN32

#ifndef FA_LABEL
#define FA_LABEL        8
#endif
#ifndef FA_DIREC
#define FA_DIREC        16
#endif

    struct _finddata_t info;
    long   handle;
	if ((handle = _findfirst ("*.*", &info)) < 0)
        return;

    do
    {
        int     attrib  = info.attrib;
        char *  name    = info.name;

        if (attrib & FA_LABEL /* 8 */)
            continue;

        if (type == FB_ENTRY_TYPE_DIRECTORY && (attrib & FA_DIREC) != 0)
        {
            // Skip '.'
            if (name[0] == '.' && name[1] == '\0') // if (!strcmp(name, "."))
                continue;
        }
        else if (type == FB_ENTRY_TYPE_FILE && (attrib & FA_DIREC) == 0)
        {
            const char *ext = strrchr (name, '.');
            if (ext == NULL)
                ext = "";
            if (!FB_Ext_In_List (ext_list, ext + 1))
                continue;
        }
        else
		{
            continue;
		}

        // Create a new file browser entry of given type
        t_filebrowser_entry *entry = FB_Entry_New (type, strdup (name));
        if (g_configuration.fb_uses_DB)
            FB_Entry_FindVLFN (entry);

        // Add to list (FIXME: argh)
        FB.files[FB.files_max] = entry;
        FB.files_max ++;
        FB.files = (t_filebrowser_entry**)realloc (FB.files, (FB.files_max + 1) * sizeof (t_filebrowser_entry *));
    }
    while (_findnext (handle, &info) == 0);

    _findclose (handle);

#endif
}

static void     FB_Load_Directory_Internal(void)
{
    static bool no_files_hack = false;

    // Clear out
    FB.files_max = 0;
    FB.file_pos = 0;
    FB.file_display_first = 0;
    FB.last_click = -1;
	FB.files = (t_filebrowser_entry**)malloc (sizeof (t_filebrowser_entry *));

    // First add directories and sort them
    FB_Add_Entries(NULL, FB_ENTRY_TYPE_DIRECTORY);
	const int sort_dir_last = FB.files_max;
	const int sort_dir_first = (sort_dir_last > 0 && (strcmp(FB.files [0]->file_name, "..") == 0)) ? 1 : 0;
    FB_Sort_Files(sort_dir_first, sort_dir_last);

    // Then add files
    // FIXME: get the list from Drivers.[ch]
    t_list* ext_list = NULL;
    list_add(&ext_list, (void*)"SMS");
    list_add(&ext_list, (void*)"GG");
    list_add(&ext_list, (void*)"SG");
    list_add(&ext_list, (void*)"SC");
    list_add(&ext_list, (void*)"SF7");
    list_add(&ext_list, (void*)"OMV");
    list_add(&ext_list, (void*)"COL");
    list_add(&ext_list, (void*)"BIN");
    list_add(&ext_list, (void*)"ROM");
    #ifdef MEKA_ZIP
        list_add(&ext_list, (void*)"ZIP");
    #endif
    FB_Add_Entries(ext_list, FB_ENTRY_TYPE_FILE);

    // If nothing was found, attempt to chdir to root and try again
    // FIXME: using a static there...
    if (FB.files_max == 0 && !no_files_hack)
	{
        chdir("\\");
        no_files_hack = true;
        FB_Load_Directory_Internal();
        return;
    }
    no_files_hack = false;

    // Sort files
    FB.file_first = sort_dir_last;
    FB.file_last = FB.files_max;
    FB_Sort_Files(FB.file_first, FB.file_last);

    // Finally add disk drives
    #ifdef ARCH_WIN32
        FB_Add_DiskDrives();
    #endif
}

void        FB_Draw_Infos (void)
{
	t_filebrowser *app = &FB;
	ALLEGRO_BITMAP* box_gfx = app->box->gfx_buffer;

	char buf[32];
    sprintf(buf, "%d/%d", FB.file_pos + 1, FB.files_max);
    Font_SetCurrent(F_MEDIUM);
	al_set_target_bitmap(box_gfx);
    al_draw_filled_rectangle(
        FB_TEXT_PAD_X, FB_Return_File_Area_Y() + (2 * FB_PAD_Y) + 2,
        88+1, FB_Return_File_Area_Y() + (2 * FB_PAD_Y) + 2+1/*+ 6*/ + Font_Height(),
        COLOR_SKIN_WINDOW_BACKGROUND);
    Font_Print(F_CURRENT, buf,
        (110 - Font_TextLength(F_CURRENT, buf)) / 2,
        FB_Return_File_Area_Y() + (2 * FB_PAD_Y) + 2,
        COLOR_SKIN_WINDOW_TEXT);
}

// Redraw file listing
void	FB_Draw_List(void)
{
	t_filebrowser *app = &FB;
	ALLEGRO_BITMAP* box_gfx = app->box->gfx_buffer;

    int     x = FB_TEXT_PAD_X;
    int     y = FB_TEXT_PAD_Y;
    int     lines_max;
    char    name_buffer [256];

    lines_max = FB.file_display_first + FB.file_y;
    if (lines_max > FB.files_max)
        lines_max = FB.files_max;

    // Ask scrollbar to refresh
	al_set_target_bitmap(box_gfx);
    al_draw_filled_rectangle(FB_PAD_X + 2, FB_PAD_Y + 2, FB.res_x - FB_PAD_X - FB_SCROLL_X, FB_PAD_Y + FB_Return_File_Area_Y() - 1, COLOR_SKIN_WIDGET_LISTBOX_BACKGROUND);
	al_draw_line(FB.res_x - FB_PAD_X - FB_SCROLL_X + 1, FB_PAD_Y + 2, FB.res_x - FB_PAD_X - FB_SCROLL_X + 1, FB_PAD_Y + FB_Return_File_Area_Y() - 1, COLOR_SKIN_WINDOW_SEPARATORS, 0);

    Font_SetCurrent (F_LARGE);
    for (int n = FB.file_display_first; n < lines_max; n++)
    {
        t_filebrowser_entry *entry = FB.files[n];
        int     x_max;
        int     x_usage = 0;
        int     entry_file_flag = -1;

        // Highlight the current file
        if (n == FB.file_pos)
            al_draw_filled_rectangle(FB_PAD_X + 2, y, FB.res_x - FB_SCROLL_X - FB_PAD_X - 1, y + Font_Height() - 1, COLOR_SKIN_WIDGET_LISTBOX_SELECTION);

        // Get the name to print and additional width usage (icons, etc...)
        switch (entry->type)
        {
        case FB_ENTRY_TYPE_DIRECTORY:
            {
                // Name
                strcpy(name_buffer, entry->file_name);
                // Width usage for directory '>' marker
                x_usage = 8;
                break;
            }
        case FB_ENTRY_TYPE_DRIVE:
            {
                // Name
                sprintf(name_buffer, Msg_Get(MSG_FileBrowser_Drive), entry->file_name[0]);
                // Width usage for drive '>' marker
                x_usage = 8; 
                break;
            }
        case FB_ENTRY_TYPE_FILE:
            {
                // Name
                if (entry->db_entry_name)
                    strcpy(name_buffer, entry->db_entry_name);
                else
                    strcpy(name_buffer, entry->file_name);

                // Width usage for icons/flags
                x_usage = 0;
                if (entry->db_entry)
                {
                    // Translation icon = +18+18 (translation automatically add the country icon)
                    if (entry->db_entry->flags & DB_FLAG_TRANS)
                    {
                        // Translation icon + Translation flag (+ overwrites)
                        x_usage += 18 + 18;
                        entry_file_flag = DB_Entry_GetTranslationFlag(entry->db_entry);
                    }
                    else 
                    {
                        entry_file_flag = DB_Entry_SelectDisplayFlag(entry->db_entry);
                        if (entry_file_flag != -1)
                        {
                            // Country flag
                            x_usage += 18; 
                        }
                    }
                    if (entry->db_entry->flags & (DB_FLAG_BAD|DB_FLAG_BIOS|DB_FLAG_HACK|DB_FLAG_HOMEBREW|DB_FLAG_PROTO))
                    {
                        // Any of BAD|BIOS|HACK|HOMEBREW|PROTO = +18 as well
                        x_usage += 18; 
                    }
                }
            }
        }

        // Compute x_max which is the maximum x position we can draw to
        x_max = FB.res_x - FB_TEXT_PAD_X - FB_SCROLL_X + 4;
        x_max -= x_usage;

        // If name doesn't fit in x_max, we have to cut it
        if (x + Font_TextLength(F_CURRENT, name_buffer) > x_max)
        {
            const int x_usage_ellipse = Font_TextLength(F_CURRENT, "..");
            int name_buffer_len = strlen(name_buffer);
            while (x + Font_TextLength(F_CURRENT, name_buffer) + x_usage_ellipse > x_max)
            {
                name_buffer_len--;
                name_buffer [name_buffer_len] = EOSTR;
            }
            strcat (name_buffer, "..");
        }

        // Print name
        Font_Print(F_CURRENT, name_buffer, x, y, COLOR_SKIN_WIDGET_LISTBOX_TEXT);

        // Print additionnal infos/icons
        switch (entry->type)
        {
        case FB_ENTRY_TYPE_DIRECTORY:
        case FB_ENTRY_TYPE_DRIVE:
            {
                // Directory/Drive '>' marker
                Font_Print(F_CURRENT, ">", FB.res_x - FB_TEXT_PAD_X - FB_SCROLL_X - 4, y, COLOR_SKIN_WIDGET_LISTBOX_TEXT);
                break;
            }
        case FB_ENTRY_TYPE_FILE:
            {
                if (entry->db_entry)
                {
                    // Start drawing at x_max
                    int x = x_max;

                    // Hack icon
                    if (entry->db_entry->flags & DB_FLAG_HACK)
                    {
						al_draw_bitmap(Graphics.Icons.Hack, x + 1, y + 1, 0);
                        x += 18;
                    }

                    // BAD icon
                    if (entry->db_entry->flags & DB_FLAG_BAD)
                    {
                        al_draw_bitmap(Graphics.Icons.BAD, x + 1+3, y + 1+1, 0);
                        x += 18;
                    }

                    // Translation icon
                    if (entry->db_entry->flags & DB_FLAG_TRANS)
                    {
                        if (entry_file_flag == FLAG_UK || entry_file_flag == FLAG_US)
                            al_draw_bitmap(Graphics.Icons.Translation_JP, x + 1, y + 1, 0);
                        else
                            al_draw_bitmap(Graphics.Icons.Translation_JP_US, x + 1, y + 1, 0);
                        x += 18;
                        if (entry_file_flag == -1)
                            entry_file_flag = FLAG_UNKNOWN;
                        al_draw_bitmap(Graphics.Flags[entry_file_flag], x + 1, y + 1, 0);
                    }
                    else
                    {
                        // Icons
                        if (entry->db_entry->flags & (DB_FLAG_HOMEBREW | DB_FLAG_PROTO | DB_FLAG_BIOS))
                        {
                            // HomeBrew icon
                            if (entry->db_entry->flags & DB_FLAG_HOMEBREW)
                                al_draw_bitmap(Graphics.Icons.HomeBrew, x + 1, y + 1, 0);
                            // Proto icon
                            else if (entry->db_entry->flags & DB_FLAG_PROTO)
                                al_draw_bitmap(Graphics.Icons.Prototype, x + 1, y + 1, 0);
                            // BIOS icon
                            else if (entry->db_entry->flags & DB_FLAG_BIOS)
                                al_draw_bitmap(Graphics.Icons.BIOS, x + 1, y + 1, 0);
                        }

                        // Country Flag
                        if (entry_file_flag != -1)
                        {
                            if (entry->db_entry->flags & (DB_FLAG_HOMEBREW | DB_FLAG_PROTO | DB_FLAG_BIOS))
                                x += 18;
                            al_draw_bitmap(Graphics.Flags[entry_file_flag], x + 1, y + 1, 0);
                        }
                    }
                }
                break;
            }
        }

        // Increment Y position
        y += Font_Height();
    }

    FB_Draw_Infos();
}

void	FB_Check_and_Repos(void)
{
    if (FB.file_pos >= FB.files_max) 
        FB.file_pos = FB.files_max - 1;
    if (FB.file_pos < 0) 
        FB.file_pos = 0;

    if (FB.file_pos < (FB.file_y >> 1)) 
        FB.file_display_first = 0;
    else if (FB.file_pos >= FB.files_max - (FB.file_y / 2)) 
		FB.file_display_first = FB.files_max - FB.file_y;
	else
		FB.file_display_first = FB.file_pos - (FB.file_y / 2);

    if (FB.file_display_first < 0) 
        FB.file_display_first = 0;
}

void            FB_Update(void)
{
    // Skip update if not active
    if (!FB.active)
        return;

    // If skin has changed, redraw everything
    int dirty = FALSE;
    if (FB.box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        FB_Layout(&FB, FALSE);
        FB.box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
        dirty = TRUE;
    }

    // Update user inputs only if focused
    if (gui_box_has_focus(FB.box))
    {
        // Update keyboard inputs
        if (Inputs_KeyPressed(ALLEGRO_KEY_ENTER, TRUE) || Inputs_KeyPressed (ALLEGRO_KEY_PAD_ENTER, TRUE))
        {
            FB_OpenSelectedEntry();
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed(ALLEGRO_KEY_HOME, FALSE))
        {
            FB.file_pos = 0;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed(ALLEGRO_KEY_END, FALSE))
        {
            FB.file_pos = FB.files_max - 1;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_DOWN, FALSE, 15, 1))
        {
            FB.file_pos ++;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_UP, FALSE, 15, 1))
        {
            FB.file_pos --;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_PGDN, FALSE, 30, 4))
        {
            FB.file_pos += FB.file_y;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_PGUP, FALSE, 30, 4))
        {
            FB.file_pos -= FB.file_y;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed(ALLEGRO_KEY_BACKSPACE, false))
        {
			FB_OpenDirectory("..");
			dirty = true;
        }
        // Note: we check for no key modifiers to be pressed
        // Eg: we don't want ALT-L to jump on 'L' games
        else if ((g_keyboard_modifiers & (ALLEGRO_KEYMOD_CTRL | ALLEGRO_KEYMOD_ALT | ALLEGRO_KEYMOD_SHIFT)) == 0)
        {
            // FIXME: this function may need a rewrite. Also, avoid using strupr()!
            // FIXME: could use allegro feature of keycode->ascii conversion
			int i;
            for (i = 0; i < NUM_ALPHA_KEYS; i ++)
			{
                if (Inputs_KeyPressed_Repeat(Key_Alpha_Table [i], FALSE, 20, 2))
                {
					int j;
                    for (j = FB.file_pos + 1; j < FB.files_max; j ++)
                    {
                        t_filebrowser_entry *entry = FB.files[j];
						if (entry->type == FB_ENTRY_TYPE_DRIVE)
							continue;
                        const char c = entry->db_entry_name ? entry->db_entry_name[0] : entry->file_name [0];
                        if (toupper(c) == Alpha_Table [i])
                        { 
                            FB.file_pos = j; 
                            break; 
                        }
                    }
                    if (j == FB.files_max)
					{
                        for (j = 0; j < FB.file_pos; j ++)
                        {
                            t_filebrowser_entry *entry = FB.files[j];
							if (entry->type == FB_ENTRY_TYPE_DRIVE)
								continue;
                            const char c = entry->db_entry_name ? entry->db_entry_name[0] : entry->file_name [0];
                            if (toupper (c) == Alpha_Table [i])
                            { 
                                FB.file_pos = j; 
                                break; 
                            }
                        }
					}
                    break;
                }
			}
            if (i != NUM_ALPHA_KEYS) 
                dirty = TRUE;
        }

        // Update mouse inputs (wheel)
        if (gui.mouse.z_rel != 0)
        {
			const int wheel_speed = (g_keyboard_modifiers & ALLEGRO_KEYMOD_CTRL) ? 10 : 1;
            if (gui.mouse.z_rel < 0)
                FB.file_pos += wheel_speed;
            if (gui.mouse.z_rel > 0)
                FB.file_pos -= wheel_speed;
            dirty = TRUE;
        }
    }

    // Fix and redraw
    if (dirty)
    {
        FB_Check_and_Repos();
        FB_Draw_List();
    }
}

void	FB_Load_Directory(void)
{
    // Msg(MSGT_DEBUG, "FB_Load_Directory()");

    const int old_files_max = FB.files_max;
    const int old_file_pos = FB.file_pos;
    const int old_file_display_first = FB.file_display_first;
    FB_Free_Memory();
    FB_Load_Directory_Internal();
    if (old_files_max == FB.files_max) // nothing has changed in directory // FIXME: argh
    {
        FB.file_pos = old_file_pos;
        FB.file_display_first = old_file_display_first;
    }
	FB_Draw_List();
}

void	FB_LoadAllNames(void)
{
    // Save current battery backed memory if there's one
    // Because Load_ROM with no verbosing doesn't save it
    BMemory_Save();

    // Msg(MSGT_DEBUG, "FB_LoadAllNames()");
    for (int i = 0; i < FB.files_max; i++)
    {
        t_filebrowser_entry *entry = FB.files[i];
        if (entry->type == FB_ENTRY_TYPE_FILE)
        {
            strncpy(g_env.Paths.MediaImageFile, entry->file_name, sizeof(g_env.Paths.MediaImageFile));
            // Msg(MSGT_DEBUG, "Loading %d/%d, %s", i, FB.files_max, file.rom);
            FB.file_pos = i;
            FB_Check_and_Repos();
            FB_Draw_List();
            Load_ROM(LOAD_MODE_GUI, false);
            gui_redraw_everything_now_once();
        }
    }

    // Free last loaded ROM
    Free_ROM();

    // Reload directory and position us to first entry
    FB.file_pos = 0;
    FB.file_display_first = 0;
    FB_Load_Directory();
    FB_Draw_List();
}

void	FB_OpenSelectedEntry(void)
{
    t_filebrowser_entry *entry = FB.files [FB.file_pos];
    switch (entry->type)
    {
    case FB_ENTRY_TYPE_DIRECTORY:
    case FB_ENTRY_TYPE_DRIVE:
        {
			FB_OpenDirectory(entry->file_name);
            break;
        }
    case FB_ENTRY_TYPE_FILE:
        {
			FB_OpenFile(entry->file_name);
            break;
        }
    default:
        {
            Quit_Msg("Error #1452: FB_Open() - Please send a report.");
            break;
        }
    }
}

void	FB_OpenDirectory(const char* name)
{
    // Mute sound while processing loading or changing directory, which may take time
    Sound_Playback_Mute();

	// Change directory
	chdir(name);

	char previous_directory[FILENAME_LEN];
	if (!strcmp(name, ".."))
	{
		// If going up the hierarchy we want to position cursor on the orevious directory
		// Abusively use StrPath_RemoveDirectory() since we know current_directory doesn't end with /
		StrPath_RemoveDirectory(previous_directory, FB.current_directory);
	}
	else
	{
		previous_directory[0] = EOSTR;
	}

	getcwd(FB.current_directory, FILENAME_LEN);
	//printf("curdir %s¥n", FB.current_directory);
	FB_Free_Memory();
	FB_Load_Directory_Internal();

	if (previous_directory[0] != EOSTR)
	{
		FB_SelectEntryByFileName(previous_directory);
	}

	FB_Draw_List();

	// Resume sound
    Sound_Playback_Resume();
}

void	FB_OpenFile(const char* name)
{
    // Mute sound while processing loading or changing directory, which may take time
    Sound_Playback_Mute();

	strncpy(g_env.Paths.MediaImageFile, name, sizeof(g_env.Paths.MediaImageFile));
	Load_ROM(LOAD_MODE_GUI, true);
	FB_Reload_Names();
	if (g_configuration.fb_close_after_load)
	{
		FB_Switch();
	}
	if (g_configuration.fullscreen_after_load)
	{
		Action_Switch_Mode();
	}

	// Resume sound
    Sound_Playback_Resume();
}

bool    FB_SelectEntryByFileName(const char* file_name)
{
    for (int i = 0; i < FB.files_max; i++)
    {
        const t_filebrowser_entry *entry = FB.files[i];
		if (strcmp(entry->file_name, file_name) == 0)
		{
            FB.file_pos = i;
            FB_Check_and_Repos();
			return true;
		}
	}
	return false;
}

void    FB_Click_List (t_widget *w)
{
    const int i = FB.file_display_first + (w->mouse_y / Font_Height (F_LARGE));
    if ((i == FB.last_click) && (gui.mouse.time_since_last_click < DOUBLE_CLICK_SPEED))
    {
        // FIXME: double-clicks should be generically handled by GUI and supports frameskipping
        gui.mouse.reset_timer = TRUE;
        gui.mouse.time_since_last_click = DOUBLE_CLICK_SPEED + 1;
        FB_OpenSelectedEntry();
    }
    else
    {
        FB.file_pos = i;
        FB.last_click = i;
        if (FB.file_pos >= FB.files_max) 
            FB.file_pos = FB.files_max - 1;
        FB_Draw_List();
    }
}

//-----------------------------------------------------------------------------
// FB_Reload_Names (void)
// Reget names from DB, based on current country
//-----------------------------------------------------------------------------
void    FB_Reload_Names (void)
{
    if (!g_configuration.fb_uses_DB)
        return;

    // Get all names
    for (int i = 0; i < FB.files_max; i++)
    {
        t_filebrowser_entry *entry = FB.files[i];
        if (entry->type == FB_ENTRY_TYPE_FILE)
        {
            if (entry->db_entry)
                entry->db_entry_name = DB_Entry_GetCurrentName (entry->db_entry);
            else
                FB_Entry_FindVLFN (entry);
        }
    }

    // Resort files only
    FB_Sort_Files (FB.file_first, FB.file_last);

    // Display
    FB_Draw_List();
}

//-----------------------------------------------------------------------------
