//-----------------------------------------------------------------------------
// MEKA - textview.h
// Text Viewer Applet - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TEXTVIEWER_PADDING              (4)
#define TEXTVIEWER_SCROLLBAR_SIZE_X     (7)
#define TEXTVIEWER_SCROLL_VELOCITY_BASE (2.5f)
#define TEXTVIEWER_SCROLL_VELOCITY_MAX  (20.0f)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_app_textviewer
{
    bool        active;
    bool        dirty;
    int         current_file;           // FIXME: REMOVE OUT OF APPLET

    t_gui_box * box;
    t_font_id   font;
    int         font_height;

    char **     text_lines;
    int         text_lines_count;
    t_frame     text_frame;
    int         text_size_y;            // == text_lines_count * font_height
    int         text_size_per_page;     // == size_y * font_height

    int         scroll_position_y;
    int         scroll_position_y_max;  // text_size_y - (text_lines_per_page * font_height)
    float       scroll_velocity_y;

    t_widget *  widget_scrollbar;

};

extern t_app_textviewer   TextViewer;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void                TextViewer_Init(t_app_textviewer* app);
void                TextViewer_Close(t_app_textviewer* app);
int                 TextViewer_Open(t_app_textviewer* app, const char *title, const char *filename);
void                TextViewer_Update(t_app_textviewer* app);
void                TextViewer_Switch(t_app_textviewer* app, const char *title, const char *filename, int current_file);

void                TextViewer_Switch_Doc_Main();
void                TextViewer_Switch_Doc_Compat();
void                TextViewer_Switch_Doc_Multiplayer_Games();
void                TextViewer_Switch_Doc_Changes();
#ifdef MEKA_Z80_DEBUGGER
void                TextViewer_Switch_Doc_Debugger();
#endif
void                TextViewer_Switch_Close();

//-----------------------------------------------------------------------------
