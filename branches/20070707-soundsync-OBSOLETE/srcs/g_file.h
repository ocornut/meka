//-----------------------------------------------------------------------------
// MEKA - g_file.h
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

void    FB_Switch               (void);
void    FB_Init_Values          (void);
void    FB_Init                 (void);
void    FB_Init_2               (void);
void    FB_Update               (void);
void    FB_Free_Memory          (void);

void    FB_Load_Directory       (void);
void    FB_Reload_Names         (void);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct s_db_entry;

typedef struct
{
    s16                 type;
    char *              file_name;
    struct s_db_entry * db_entry;
    char *              db_entry_name;
} t_filebrowser_entry;

typedef struct
{
    t_gui_box *         box;
    BITMAP *            bmp;
    bool                active;
    t_filebrowser_entry **  files;
    int                 files_max;
    int                 file_pos;
    int                 file_display_first;
    int                 file_first, file_last; // Other are directories & drives
    int                 last_click;
    char                current_directory [FILENAME_LEN+1];
    int                 res_x, file_y;

    t_widget *          widget_scrollbar;
} t_filebrowser;

t_filebrowser   FB;

//-----------------------------------------------------------------------------

