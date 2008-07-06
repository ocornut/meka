//-----------------------------------------------------------------------------
// MEKA - g_file.c
// GUI File Browser - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_file.h"
#include "g_tools.h"
#include "g_widget.h"
#include "db.h"
#include "file.h"
#include "inputs_t.h"
#include "vlfn.h"

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

void    FB_Layout               (t_filebrowser *app, bool setup);

//

int     FB_Return_File_Area_X   (void);
int     FB_Return_File_Area_Y   (void);
int     FB_Return_Res_Y         (void);

void    FB_Draw_List            (void);
void    FB_Draw_Infos           (void);
void    FB_Click_List           (t_widget *w);
void    FB_Sort_Files           (int start, int end);
void    FB_Check_and_Repos      (void);

void    FB_Open                 (void);
void    FB_Load_All_Names       (void);

t_filebrowser_entry *   FB_Entry_New        (int type, char *file_name);
void                    FB_Entry_Delete     (t_filebrowser_entry *entry);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FB_Entry_New (int type, char *filename)
// Create a new file browser entry
//-----------------------------------------------------------------------------
t_filebrowser_entry *       FB_Entry_New (int type, char *file_name)
{
    t_filebrowser_entry *   entry;
    entry = malloc (sizeof (t_filebrowser_entry));
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
void    FB_Entry_Delete (t_filebrowser_entry *entry)
{
    free (entry->file_name);
    free (entry);
}

//-----------------------------------------------------------------------------
// FB_Entry_FindVLFN (t_filebrowser_entry *entry)
// Find VLFN data associated to given entry file name, and associate data
//-----------------------------------------------------------------------------
void                FB_Entry_FindVLFN (t_filebrowser_entry *entry)
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

INLINE int     FB_Return_File_Area_X (void)
{
    return (FB.res_x - (2 * FB_PAD_X));
}

INLINE int     FB_Return_File_Area_Y (void)
{
    return ((FB.file_y * Font_Height (F_LARGE)) + 5);
}

INLINE int     FB_Return_Res_Y (void)
{
    return (FB_Return_File_Area_Y () + (3 * FB_PAD_Y) + FB_BUTTON_Y);
}

void    FB_Switch(void)
{
    FB.active ^= 1;
    gui_box_show(FB.box, FB.active, TRUE);
    gui_menu_inverse_check(menus_ID.file, 0);
}

void    FB_Init_Values(void)
{
    FB.res_x    = 320;
    FB.file_y   = 17;
}

void            FB_Init (void)
{
    t_frame     frame;

    frame.pos.x     = 310;
    frame.pos.y     = 50;
    frame.size.x    = FB.res_x;
    frame.size.y    = FB_Return_Res_Y();
    FB.box = gui_box_new(&frame, Msg_Get (MSG_FileBrowser_BoxTitle));
    FB.bmp = FB.box->gfx_buffer;
    Desktop_Register_Box("LOAD", FB.box, 0, &FB.active);

    // Set exclusive inputs flag to avoid messing with emulation
    FB.box->flags |= GUI_BOX_FLAGS_FOCUS_INPUTS_EXCLUSIVE;

    // Add close box widget
    widget_closebox_add(FB.box, FB_Switch);

    // Layout
    FB_Layout(&FB, TRUE);
}

void    FB_Init_2 (void)
{
    chdir(FB.current_directory);
    FB_Load_Directory();
}

void        FB_Layout(t_filebrowser *app, bool setup)
{
    // Clear
    clear_to_color(app->box->gfx_buffer, COLOR_SKIN_WINDOW_BACKGROUND);

    // Setup widgets
    if (setup)
    {
        t_frame frame; 

        // Add scrollbar
        frame.pos.x = FB.res_x - FB_PAD_X - FB_SCROLL_X + 1;
        frame.pos.y = FB_PAD_Y + 2;
        frame.size.x = FB_SCROLL_X - 3;
        frame.size.y = FB_Return_File_Area_Y () - 4;
        app->widget_scrollbar = widget_scrollbar_add(FB.box, WIDGET_SCROLLBAR_TYPE_VERTICAL, &frame, &FB.files_max, &FB.file_display_first, &FB.file_y, FB_Draw_List);

        // Add an invisible 'button' to catch click on the list
        // (currently the GUI doesn't handle list/combo)
        frame.pos.x = FB_PAD_X + 2;
        frame.pos.y = FB_PAD_Y + 2;
        frame.size.x = FB.res_x - (2 * FB_PAD_X) - FB_SCROLL_X - 4;
        frame.size.y = FB_Return_File_Area_Y () - 6;
        widget_button_add(FB.box, &frame, 1, FB_Click_List, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);

        // Add 'CLOSE' button
        //frame.pos.x = FB_BUTTON_X + 10;
        frame.pos.x = FB.res_x - FB_BUTTON_X - 10;
        frame.pos.y = FB_Return_File_Area_Y () + (2 * FB_PAD_Y);
        frame.size.x = FB_BUTTON_X;
        frame.size.y = FB_BUTTON_Y;
        widget_button_add(FB.box, &frame, 1, FB_Switch, WIDGET_BUTTON_STYLE_BIG, Msg_Get(MSG_FileBrowser_Close));

        // Add 'LOAD' button
        frame.pos.x -= FB_BUTTON_X + 10;
        widget_button_add(FB.box, &frame, 1, FB_Open, WIDGET_BUTTON_STYLE_BIG, Msg_Get(MSG_FileBrowser_Load));

        // Add small 'LOAD NAMES' button
        frame.pos.x = FB_PAD_X;
        frame.pos.y = FB_Return_File_Area_Y () + (2 * FB_PAD_Y) + Font_Height (F_MIDDLE) + 6;
        frame.size.x = 54;
        frame.size.y = Font_Height (F_SMALL) + 3;
        widget_button_add(FB.box, &frame, 1, FB_Load_All_Names, WIDGET_BUTTON_STYLE_SMALL, Msg_Get(MSG_FileBrowser_LoadNames));

        // Add small 'RELOAD DIR' button
        frame.pos.x += frame.size.x + 1;
        widget_button_add(FB.box, &frame, 1, FB_Load_Directory, WIDGET_BUTTON_STYLE_SMALL, Msg_Get(MSG_FileBrowser_ReloadDir));
    }

    // Additionnal drawing
    gui_rect (FB.bmp, LOOK_ROUND, FB_PAD_X, FB_PAD_Y, FB.res_x - FB_PAD_X, FB_PAD_Y + FB_Return_File_Area_Y (), COLOR_SKIN_WIDGET_LISTBOX_BORDER);
}

void        FB_Free_Memory(void)
{
    int     i;
    for (i = 0; i < FB.files_max; i ++)
        FB_Entry_Delete(FB.files[i]);
    free(FB.files);
}

#ifndef ARCH_UNIX

void            FB_Add_Drives (void)
{
    int         i;
    char        buf [256];

#ifdef ARCH_DOS
    int j = getdisk ();         // Save current disk
    for (i = 2; i < 26; i ++)   // C: to Z:
    {
        setdisk (i);
        if (getdisk () == i)
        {
            // Create a new file browser entry of disk type
            t_filebrowser_entry *entry;
            sprintf (buf, "%c:", 'A' + i);
            entry = FB_Entry_New (FB_ENTRY_TYPE_DRIVE, strdup (buf));

            // Add to list (FIXME: argh)
            FB.files [FB.files_max] = entry;
            FB.files_max ++;
            FB.files = realloc (FB.files, (FB.files_max + 1) * sizeof (t_filebrowser_entry *));
        }
    }
    setdisk (j); // Restore current disk
#else
    for (i = 2; i < 26; i ++)   // C: to Z:
    {
        if (GetLogicalDrives () & (1 << i))
        {
            // Create a new file browser entry of disk type
            t_filebrowser_entry *entry;
            sprintf (buf, "%c:", 'A' + i);
            entry = FB_Entry_New (FB_ENTRY_TYPE_DRIVE, strdup (buf));

            // Add to list (FIXME: argh)
            FB.files [FB.files_max] = entry;
            FB.files_max ++;
            FB.files = realloc (FB.files, (FB.files_max + 1) * sizeof (t_filebrowser_entry *));
        }
    }
#endif
}

#endif

static INLINE int   FB_Sort_Files_GetEntryPriority (t_filebrowser_entry *entry)
{
    int             p;

    t_db_entry *    db_entry = entry->db_entry;
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
    p = DB_Entry_SelectFlag (db_entry);
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

// FIXME: bubble sort is slow!
void        FB_Sort_Files (int start, int end)
{
    int     i;

    for (; start < end - 1; start ++)
        for (i = end - 1; i > start; i --)
        {
            t_filebrowser_entry *e1 = FB.files[i];
            t_filebrowser_entry *e2 = FB.files[i - 1];
            char *e1_name = e1->db_entry_name ? e1->db_entry_name : e1->file_name;
            char *e2_name = e2->db_entry_name ? e2->db_entry_name : e2->file_name;

            // Compare
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

int                 FB_Ext_In_List (t_list *ext_list, char *ext)
{
    while (ext_list)
    {
        if (stricmp (ext_list->elem, ext) == 0)
            return (1);
        ext_list = ext_list->next;
    }
    return (0);
}

void                FB_Add_Entries (t_list *ext_list, int type)
{
#ifdef ARCH_UNIX
    DIR *           dir;
    struct dirent * dirent;
    struct stat	    dirent_stat;

    // Open current directory
    if ((dir = opendir(".")) == 0)
    {
        Msg (MSGT_USER, Msg_Get (MSG_Error_Directory_Open));
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
            char *ext = strrchr (name, '.');
            if (ext == NULL) 
                ext = "";
            if (!FB_Ext_In_List (ext_list, ext + 1))
                continue;
        }
        else
            continue;

        {
            // Create a new file browser entry of given type
            t_filebrowser_entry *entry = FB_Entry_New (type, strdup (name));
            if (g_Configuration.fb_uses_DB)
                FB_Entry_FindVLFN (entry);

            // Add to list (FIXME: argh)
	    FB.files[FB.files_max] = entry;
            FB.files_max ++;
            FB.files = realloc (FB.files, (FB.files_max + 1) * sizeof (t_filebrowser_entry *));
        }
    }

    closedir (dir);

#else // ARCH_DOS & ARCH_WIN32

#ifdef ARCH_DOS
    struct ffblk f;
    if (findfirst ("*.*", &f, 0xFF) != 0)
        return;
#elif ARCH_WIN32
    struct _finddata_t info;
    long   handle;
    if ((handle = _findfirst (uconvert_toascii("*.*", NULL), &info)) < 0)
        return;
#endif

    do
    {
        #ifdef ARCH_DOS
            int     attrib  = f.ff_attrib;
            char *  name    = f.ff_name;
        #elif ARCH_WIN32
            int     attrib  = info.attrib;
            char *  name    = info.name;
        #endif

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
            char *ext = strrchr (name, '.');
            if (ext == NULL)
                ext = "";
            if (!FB_Ext_In_List (ext_list, ext + 1))
                continue;
        }
        else
            continue;

        {
            // Create a new file browser entry of given type
            t_filebrowser_entry *entry = FB_Entry_New (type, strdup (name));
            if (g_Configuration.fb_uses_DB)
                FB_Entry_FindVLFN (entry);

            // Add to list (FIXME: argh)
            FB.files[FB.files_max] = entry;
            FB.files_max ++;
            FB.files = realloc (FB.files, (FB.files_max + 1) * sizeof (t_filebrowser_entry *));
        }
    }

#ifdef ARCH_DOS
    while (findnext (&f) == 0);
#else // ARCH_WIN32
    while (_findnext (handle, &info) == 0);
#endif

#ifdef ARCH_WIN32
    _findclose (handle);
#endif

#endif
}

static void     FB_Load_Directory_Internal (void)
{
    int         i;
    static int  no_files = 0;
    t_list *    ext_list = 0;

    // Clear out
    FB.files_max = 0;
    FB.file_pos = 0;
    FB.file_display_first = 0;
    FB.last_click = -1;
    FB.files = malloc (sizeof (t_filebrowser_entry *));

    // First add directories and sort them
    FB_Add_Entries (NULL, FB_ENTRY_TYPE_DIRECTORY);
    i = FB.files_max;
    if (FB.files_max > 0 && (strcmp (FB.files [0]->file_name, "..") == 0))
        FB_Sort_Files (1, i);
    else
        FB_Sort_Files (0, i);

    // Then add files
    // FIXME: get the list from Drivers.[ch]
    list_add (&ext_list, "SMS");
    list_add (&ext_list, "GG");
    list_add (&ext_list, "SG");
    list_add (&ext_list, "SC");
    list_add (&ext_list, "SF7");
    list_add (&ext_list, "OMV");
    list_add (&ext_list, "COL");
    list_add (&ext_list, "BIN");
    list_add (&ext_list, "ROM");
    if (g_Configuration.enable_NES)
        list_add (&ext_list, "NES");
    #ifdef MEKA_ZIP
        list_add (&ext_list, "ZIP");
    #endif
    FB_Add_Entries (ext_list, FB_ENTRY_TYPE_FILE);

    // If no file was found, attempt to chdir to root and try again
    // FIXME: using a static there...
    if (FB.files_max == 0 && no_files == 0)
    {
        chdir("\\");
        no_files = 1;
        FB_Load_Directory_Internal ();
        return;
    }
    no_files = 0;

    // Sort files
    FB.file_first = i;
    FB.file_last = FB.files_max;
    FB_Sort_Files (FB.file_first, FB.file_last);

    // Finally add disk drivers
    #ifndef ARCH_UNIX
        FB_Add_Drives ();
    #endif
    
    // Display
    FB_Draw_List ();
}

void        FB_Draw_Infos (void)
{
    char    s[32];

    sprintf (s, "%d/%d", FB.file_pos + 1, FB.files_max);
    Font_SetCurrent (F_MIDDLE);
    rectfill (FB.bmp,
        FB_TEXT_PAD_X, FB_Return_File_Area_Y () + (2 * FB_PAD_Y) + 2,
        88, FB_Return_File_Area_Y () + (2 * FB_PAD_Y) + 2/*+ 6*/ + Font_Height(-1),
        COLOR_SKIN_WINDOW_BACKGROUND);
    Font_Print (-1, FB.bmp, s,
        (110 - Font_TextLength (-1, s)) / 2,
        FB_Return_File_Area_Y () + (2 * FB_PAD_Y) + 2,
        COLOR_SKIN_WINDOW_TEXT);
}

//-----------------------------------------------------------------------------
// FB_Draw_List ()
// Redraw file listing
//-----------------------------------------------------------------------------
void        FB_Draw_List(void)
{
    int     x = FB_TEXT_PAD_X;
    int     y = FB_TEXT_PAD_Y;
    int     n, lines_max;
    char    name_buffer [256];

    // Set window redraw flag
    FB.box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    lines_max = FB.file_display_first + FB.file_y;
    if (lines_max > FB.files_max)
        lines_max = FB.files_max;

    // Ask scrollbar to refresh
    widget_set_dirty(FB.widget_scrollbar);

    //
    rectfill (FB.bmp, FB_PAD_X + 2, FB_PAD_Y + 2, FB.res_x - FB_PAD_X - FB_SCROLL_X - 1, FB_PAD_Y + FB_Return_File_Area_Y () - 2, COLOR_SKIN_WIDGET_LISTBOX_BACKGROUND);
    line (FB.bmp, FB.res_x - FB_PAD_X - FB_SCROLL_X, FB_PAD_Y + 2, FB.res_x - FB_PAD_X - FB_SCROLL_X, FB_PAD_Y + FB_Return_File_Area_Y () - 2, COLOR_SKIN_WINDOW_SEPARATORS);

    Font_SetCurrent (F_LARGE);
    for (n = FB.file_display_first; n < lines_max; n++)
    {
        t_filebrowser_entry *entry = FB.files[n];
        int     x_max;
        int     x_usage = 0;
        int     entry_file_flag = -1;

        // Highlight the current file
        if (n == FB.file_pos)
            rectfill (FB.bmp, FB_PAD_X + 2, y /*- 1*/, FB.res_x - FB_SCROLL_X - FB_PAD_X - 2, y + Font_Height(-1) - 2, COLOR_SKIN_WIDGET_LISTBOX_SELECTION);

        // Get the name to print and additionnal width usage (icons, etc...)
        switch (entry->type)
        {
        case FB_ENTRY_TYPE_DIRECTORY:
            {
                // Name
                strcpy (name_buffer, entry->file_name);
                // Width usage for directory '>' marker
                x_usage = 8;
                break;
            }
        case FB_ENTRY_TYPE_DRIVE:
            {
                // Name
                sprintf (name_buffer, Msg_Get (MSG_FileBrowser_Drive), entry->file_name[0]);
                // Width usage for drive '>' marker
                x_usage = 8; 
                break;
            }
        case FB_ENTRY_TYPE_FILE:
            {
                // Name
                if (entry->db_entry_name)
                    strcpy (name_buffer, entry->db_entry_name);
                else
                    strcpy (name_buffer, entry->file_name);

                // Width usage for icons/flags
                x_usage = 0;
                if (entry->db_entry)
                {
                    // Translation icon = +18+18 (translation automatically add the country icon)
                    if (entry->db_entry->flags & DB_FLAG_TRANS)
                    {
                        // Translation icon + Translation flag (+ overwrites)
                        x_usage += 18 + 18;
                        entry_file_flag = DB_Entry_GetTranslationFlag (entry->db_entry);
                    }
                    else 
                    {
                        entry_file_flag = DB_Entry_SelectFlag (entry->db_entry);
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
        if (x + Font_TextLength (-1, name_buffer) > x_max)
        {
            int name_buffer_len = strlen(name_buffer);
            int x_usage_ellipse = Font_TextLength (-1, "..");
            while (x + Font_TextLength (-1, name_buffer) + x_usage_ellipse > x_max)
            {
                name_buffer_len--;
                name_buffer [name_buffer_len] = EOSTR;
            }
            strcat (name_buffer, "..");
        }

        // Print name
        Font_Print (-1, FB.bmp, name_buffer, x, y, COLOR_SKIN_WIDGET_LISTBOX_TEXT);

        // Print additionnal infos/icons
        switch (entry->type)
        {
        case FB_ENTRY_TYPE_DIRECTORY:
        case FB_ENTRY_TYPE_DRIVE:
            {
                // Directory/Drive '>' marker
                Font_Print (-1, FB.bmp, ">", FB.res_x - FB_TEXT_PAD_X - FB_SCROLL_X - 4, y, COLOR_SKIN_WIDGET_LISTBOX_TEXT);
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
                        draw_sprite (FB.bmp, Graphics.Icons.Hack, x + 1, y + 1);
                        x += 18;
                    }

                    // BAD icon
                    if (entry->db_entry->flags & DB_FLAG_BAD)
                    {
                        draw_sprite (FB.bmp, Graphics.Icons.BAD, x + 1+3, y + 1+1);
                        x += 18;
                    }

                    // Translation icon
                    if (entry->db_entry->flags & DB_FLAG_TRANS)
                    {
                        if (entry_file_flag == FLAG_UK || entry_file_flag == FLAG_US)
                            draw_sprite (FB.bmp, Graphics.Icons.Translation_JP, x + 1, y + 1);
                        else
                            draw_sprite (FB.bmp, Graphics.Icons.Translation_JP_US, x + 1, y + 1);
                        x += 18;
                        if (entry_file_flag == -1)
                            entry_file_flag = FLAG_UNKNOWN;
                        draw_sprite (FB.bmp, Graphics.Flags[entry_file_flag], x + 1, y + 1);
                    }
                    else
                    {
                        // Icons
                        if (entry->db_entry->flags & (DB_FLAG_HOMEBREW | DB_FLAG_PROTO | DB_FLAG_BIOS))
                        {
                            // HomeBrew icon
                            if (entry->db_entry->flags & DB_FLAG_HOMEBREW)
                                draw_sprite (FB.bmp, Graphics.Icons.HomeBrew, x + 1, y + 1);
                            // Proto icon
                            else if (entry->db_entry->flags & DB_FLAG_PROTO)
                                draw_sprite (FB.bmp, Graphics.Icons.Prototype, x + 1, y + 1);
                            // BIOS icon
                            else if (entry->db_entry->flags & DB_FLAG_BIOS)
                                draw_sprite (FB.bmp, Graphics.Icons.BIOS, x + 1, y + 1);
                        }

                        // Country Flag
                        if (entry_file_flag != -1)
                        {
                            if (entry->db_entry->flags & (DB_FLAG_HOMEBREW | DB_FLAG_PROTO | DB_FLAG_BIOS))
                                x += 18;
                            draw_sprite (FB.bmp, Graphics.Flags[entry_file_flag], x + 1, y + 1);
                        }
                    }
                }
                break;
            }
        }

        // Increment Y position
        y += Font_Height(-1);
    }

    FB_Draw_Infos ();
}

// ???
void            FB_Check_and_Repos (void)
{
    if (FB.file_pos >= FB.files_max) 
        FB.file_pos = FB.files_max - 1;
    if (FB.file_pos < 0) 
        FB.file_pos = 0;

    if (FB.file_pos < (FB.file_y >> 1)) 
        FB.file_display_first = 0;
    else
        if (FB.file_pos >= FB.files_max - (FB.file_y / 2)) 
            FB.file_display_first = FB.files_max - FB.file_y;
        else
            FB.file_display_first = FB.file_pos - (FB.file_y / 2);

    if (FB.file_display_first < 0) 
        FB.file_display_first = 0;
}

void            FB_Update(void)
{
    int         dirty = FALSE;

    // Skip update if not active
    if (!FB.active)
        return;

    // If skin has changed, redraw everything
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
        if (Inputs_KeyPressed (KEY_ENTER, TRUE) || Inputs_KeyPressed (KEY_ENTER_PAD, TRUE))
        {
            FB_Open ();
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed (KEY_HOME, FALSE))
        {
            FB.file_pos = 0;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed (KEY_END, FALSE))
        {
            FB.file_pos = FB.files_max - 1;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed_Repeat (KEY_DOWN, FALSE, 15, 1))
        {
            FB.file_pos ++;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed_Repeat (KEY_UP, FALSE, 15, 1))
        {
            FB.file_pos --;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed_Repeat (KEY_PGDN, FALSE, 30, 4))
        {
            FB.file_pos += FB.file_y;
            dirty = TRUE;
        }
        else if (Inputs_KeyPressed_Repeat (KEY_PGUP, FALSE, 30, 4))
        {
            FB.file_pos -= FB.file_y;
            dirty = TRUE;
        }
        // Note: we check for no key modifiers to be pressed
        // Eg: we don't want ALT-L to jump on 'L' games
        else if ((key_shifts & (KB_CTRL_FLAG | KB_ALT_FLAG | KB_SHIFT_FLAG)) == 0)
        {
            // FIXME: this function may need a rewrite. Also, avoid using strupr()!
            // FIXME: could use allegro feature of keycode->ascii conversion
            int i, j;
            for (i = 0; i < NUM_ALPHA_KEYS; i ++)
                if (Inputs_KeyPressed_Repeat (Key_Alpha_Table [i], FALSE, 20, 2))
                {
                    for (j = FB.file_pos + 1; j < FB.files_max; j ++)
                    {
                        t_filebrowser_entry *entry = FB.files[j];
                        char c = entry->db_entry_name ? entry->db_entry_name[0] : entry->file_name [0];
                        if (toupper(c) == Alpha_Table [i])
                        { 
                            FB.file_pos = j; 
                            break; 
                        }
                    }
                    if (j == FB.files_max)
                        for (j = 0; j < FB.file_pos; j ++)
                        {
                            t_filebrowser_entry *entry = FB.files[j];
                            char c = entry->db_entry_name ? entry->db_entry_name[0] : entry->file_name [0];
                            if (toupper (c) == Alpha_Table [i])
                            { 
                                FB.file_pos = j; 
                                break; 
                            }
                        }
                        break;
                }
            if (i != NUM_ALPHA_KEYS) 
                dirty = TRUE;
        }

        // Update mouse inputs (wheel)
        if (gui.mouse.z_rel != 0)
        {
            if (gui.mouse.z_rel < 0)
                //FB.file_display_first++;
                FB.file_pos++;
            if (gui.mouse.z_rel > 0)
                //FB.file_display_first--;
                FB.file_pos--;
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

void        FB_Load_Directory (void)
{
    int     i, j, k;

    // Msg (MSGT_DEBUG, "FB_Load_Directory()");

    i = FB.files_max;
    j = FB.file_pos;
    k = FB.file_display_first;
    FB_Free_Memory ();
    FB_Load_Directory_Internal ();
    if (i == FB.files_max) // nothing has changed in directory // FIXME: argh
    {
        FB.file_pos = j;
        FB.file_display_first = k;
        FB_Draw_List ();
    }
}

void        FB_Load_All_Names (void)
{
    int     i;

    // Save current battery backed memory if there's one
    // Because Load_ROM with no verbosing doesn't save it
    BMemory_Save ();

    // Msg (MSGT_DEBUG, "FB_Load_All_Names()");
    for (i = 0; i < FB.files_max; i++)
    {
        t_filebrowser_entry *entry = FB.files[i];
        if (entry->type == FB_ENTRY_TYPE_FILE)
        {
            strncpy(g_Env.Paths.MediaImageFile, entry->file_name, sizeof(g_Env.Paths.MediaImageFile));
            // Msg (MSGT_DEBUG, "Loading %d/%d, %s", i, FB.files_max, file.rom);
            FB.file_pos = i;
            FB_Check_and_Repos ();
            FB_Draw_List ();
            Load_ROM (LOAD_INTERFACE, FALSE);
            gui_redraw_everything_now_once ();
        }
    }

    // Free last loaded ROM
    Free_ROM ();

    // Reload directory and position us to first entry
    FB.file_pos = 0;
    FB.file_display_first = 0;
    FB_Load_Directory ();
    FB_Draw_List ();
}

void        FB_Open (void)
{
    t_filebrowser_entry *entry = FB.files [FB.file_pos];

    // Mute sound while processing loading or changing directory,
    // which may take time
    Sound_Playback_Mute ();

    switch (entry->type)
    {
    case FB_ENTRY_TYPE_DIRECTORY:
    case FB_ENTRY_TYPE_DRIVE:
        {
            char *dst = entry->file_name;
            // #ifdef ARCH_WIN32
            //  if (strlen(Dst) == 2 && Dst[1] == ':')
            //      _chdrive (Dst[0] - 'A');
            //  else
            // #endif
            chdir (dst);
            getcwd (FB.current_directory, FILENAME_LEN);
            FB_Free_Memory ();
            FB_Load_Directory_Internal ();
            break;
        }
    case FB_ENTRY_TYPE_FILE:
        {
            strncpy(g_Env.Paths.MediaImageFile, entry->file_name, sizeof(g_Env.Paths.MediaImageFile));
            Load_ROM (LOAD_INTERFACE, TRUE);
            FB_Reload_Names ();
            if (g_Configuration.fb_close_after_load)
            {
                FB_Switch ();
            }
            if (g_Configuration.fullscreen_after_load)
            {
                Action_Switch_Mode ();
            }
            break;
        }
    default:
        {
            Quit_Msg ("Error #1452: FB_Open() - Please send a report.");
            break;
        }
    }

    // Resume sound
    Sound_Playback_Resume ();
}

void    FB_Click_List (t_widget *w)
{
    int    i;

    i = FB.file_display_first + (w->mouse_y / Font_Height (F_LARGE));
    if ((i == FB.last_click) && (gui.mouse.time_since_last_click < DOUBLE_CLICK_SPEED))
    {
        // FIXME: double-clicks should be generically handled by GUI and supports frameskipping
        gui.mouse.reset_timer = TRUE;
        gui.mouse.time_since_last_click = DOUBLE_CLICK_SPEED + 1;
        FB_Open ();
    }
    else
    {
        FB.file_pos = i;
        FB.last_click = i;
        if (FB.file_pos >= FB.files_max) 
            FB.file_pos = FB.files_max - 1;
        FB_Draw_List ();
    }
}

//-----------------------------------------------------------------------------
// FB_Reload_Names (void)
// Reget names from DB, based on current country
//-----------------------------------------------------------------------------
void    FB_Reload_Names (void)
{
    int i;

    if (!g_Configuration.fb_uses_DB)
        return;

    // Reget all names
    for (i = 0; i < FB.files_max; i++)
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
    FB_Draw_List ();
}

//-----------------------------------------------------------------------------
