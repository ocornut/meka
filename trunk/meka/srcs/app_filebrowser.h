//-----------------------------------------------------------------------------
// MEKA - app_filebrowser.h
// GUI File Browser - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define FB_ENTRY_TYPE_FILE      (0)
#define FB_ENTRY_TYPE_DIRECTORY (1)
#define FB_ENTRY_TYPE_DRIVE     (2)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    FB_Switch               ();
void    FB_Init                 ();
void    FB_Init_2               ();
void    FB_Close                ();
void    FB_Update               ();

void    FB_Load_Directory       ();
void    FB_Reload_Names         ();

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_db_entry;

struct t_filebrowser_entry
{
    s16                 type;
    char *              file_name;
    const t_db_entry *	db_entry;
    const char *        db_entry_name;
};

struct t_filebrowser
{
    t_gui_box *         box;
	t_font_id			font_id;
    bool                active;
    t_filebrowser_entry **  files;
    int                 files_max;
    int                 file_pos;
    int                 file_display_first;
    int                 file_first, file_last; // Other are directories & drives
    int                 last_click;
    char                current_directory[FILENAME_LEN+1];
    int                 files_display_count;

	int					bottom_y;
    t_widget *          widget_scrollbar;
	t_widget *			widget_click_list;
	t_widget *			widget_close_button;
	t_widget *			widget_load_button;
	t_widget *			widget_load_names_button;
	t_widget *			widget_reload_dir_button;
};

extern t_filebrowser    FB;

//-----------------------------------------------------------------------------

