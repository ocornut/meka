//-----------------------------------------------------------------------------
// MEKA - textview.h
// Text Viewer Applet - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TEXTVIEWER_LINES                (44)
#define TEXTVIEWER_COLUMNS              (60)
#define TEXTVIEWER_PADDING_X            (4)
#define TEXTVIEWER_SCROLLBAR_SIZE_X     (7)
#define TEXTVIEWER_SCROLL_VELOCITY_BASE (6)         // 8
#define TEXTVIEWER_SCROLL_VELOCITY_MAX  (5678)      // ???

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_app_textviewer
{
    bool        active;
    bool        dirty;
    int         current_file;           // FIXME: REMOVE OUT OF APPLET

    t_gui_box * box;
    int         font;
    int         font_height;

    int         size_x;                 // in columns
    int         size_y;                 // in lines

    char **     text_lines;
    int         text_lines_count;
    t_frame     text_frame;
    int         text_size_y;            // == text_lines_count * font_height
    int         text_size_per_page;     // == size_y * font_height

    int         scroll_position_y;
    int         scroll_position_y_max;  // text_size_y - (text_lines_per_page * font_height)
    int         scroll_velocity_y;

    t_widget *  widget_scrollbar;

};

extern t_app_textviewer   TextViewer;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void                TextViewer_Init(t_app_textviewer *tv);
int                 TextViewer_Open(t_app_textviewer *tv, const char *title, const char *filename);
void                TextViewer_Update(t_app_textviewer *tv);
void                TextViewer_Switch(t_app_textviewer *tv, const char *title, const char *filename, int current_file);

void                TextViewer_Switch_Doc_Main(void);
#ifdef ARCH_WIN32
void                TextViewer_Switch_Doc_MainW(void);
#endif
#ifdef ARCH_UNIX
void                TextViewer_Switch_Doc_MainU(void);
#endif
void                TextViewer_Switch_Doc_Compat(void);
void                TextViewer_Switch_Doc_Multiplayer_Games(void);
void                TextViewer_Switch_Doc_Changes(void);
#ifdef MEKA_Z80_DEBUGGER
void                TextViewer_Switch_Doc_Debugger(void);
#endif
void                TextViewer_Switch_Close(void);

//-----------------------------------------------------------------------------
