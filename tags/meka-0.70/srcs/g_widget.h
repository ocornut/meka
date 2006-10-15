//-----------------------------------------------------------------------------
// MEKA - g_widget.h
// GUI Widgets - Headers
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

typedef enum
{
    WIDGET_CONTENT_TYPE_TEXT        = 0,
    WIDGET_CONTENT_TYPE_DECIMAL     = 1,
    WIDGET_CONTENT_TYPE_HEXADECIMAL = 2,
} t_widget_content_type;

typedef enum
{
    WIDGET_INPUTBOX_FLAG_DEFAULT                = 0,
    WIDGET_INPUTBOX_FLAG_NO_CURSOR              = 0x0001,
    WIDGET_INPUTBOX_FLAG_NO_MOVE_CURSOR         = 0x0002,
    WIDGET_INPUTBOX_FLAG_NO_DELETE              = 0x0004,
    WIDGET_INPUTBOX_FLAG_HIGHLIGHT_CURRENT_CHAR = 0x0008,
} t_widget_inputbox_flags;

typedef enum
{
    WIDGET_MOUSE_ACTION_NONE                    = 0,
    WIDGET_MOUSE_ACTION_HOVER                   = 0x0001,
    WIDGET_MOUSE_ACTION_CLICK                   = 0x0002,
} t_widget_mouse_action;

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_widget
{
    int         enabled;
    int         id;
    t_gui_box * box;
    int         box_n;
    t_frame     frame;
    int         mx, my;
    int         mb, mb_old;
    int         mb_react;
    t_widget_mouse_action   mouse_action;
    void        (*redraw)();
    void        (*update)();
    void *      data;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Updating functions ---------------------------------------------------------
int         widgets_update_box      (t_gui_box *b, int cx, int cy);
void        widgets_call_update     (void);

// Create a new widget --------------------------------------------------------
t_widget *  widget_new              (int box_n);
void        widget_init_mb          (t_widget *w, int react);
void        widget_enable           (t_widget *w);
void        widget_disable          (t_widget *w);
//-----------------------------------------------------------------------------

// Widget: star button at the top right of a box ------------------------------
t_widget *  widget_closebox_add     (int box_n, void *callback);
void        widget_closebox_update  (t_widget *w);
void        widget_closebox_redraw  (t_widget *w, int bx, int by);
//-----------------------------------------------------------------------------

// Widget: button -------------------------------------------------------------
t_widget *  widget_button_add       (int box_n, t_frame *frame, int react, void *callback);
t_widget *  widget_button_add_draw  (int box_n, t_frame *frame, int look, int FontIdx, int react, void *when_clicked, char *s);
void        widget_button_update    (t_widget *w);
//-----------------------------------------------------------------------------

// Widget: scroll bar ---------------------------------------------------------
t_widget *  widget_scrollbar_add    (int box_n, t_frame *frame, int *v_max, int *v_start, int *v_per_page, void *callback);
void        widget_scrollbar_update (t_widget *w);
//-----------------------------------------------------------------------------

// Widget: check box ----------------------------------------------------------
t_widget *  widget_checkbox_add                 (int box_n, t_frame *frame, byte *pvalue, void *callback);
void        widget_checkbox_update              (t_widget *w);
void        widget_checkbox_redraw              (t_widget *w);
void        widget_checkbox_set_pvalue          (t_widget *w, byte *pvalue);
//-----------------------------------------------------------------------------

// Widget: text box -----------------------------------------------------------
t_widget *  widget_textbox_add                  (int box_n, t_frame *frame, int lines_max, int font_idx);
void        widget_textbox_redraw               (t_widget *w);
void        widget_textbox_clear                (t_widget *w);
void        widget_textbox_set_current_color    (t_widget *w, int current_color);
void        widget_textbox_print_scroll         (t_widget *w, int wrap, const char *line);
void        widget_textbox_printf_scroll        (t_widget *w, int wrap, const char *format, ...);
//-----------------------------------------------------------------------------

// Widget: input box ----------------------------------------------------------
t_widget *  widget_inputbox_add                 (int box_n, t_frame *frame, int length_max, int font_idx, void *callback_enter);
void        widget_inputbox_update              (t_widget *w);
void        widget_inputbox_redraw              (t_widget *w);
char *      widget_inputbox_get_value           (t_widget *w);
void        widget_inputbox_set_value           (t_widget *w, char *value);
int         widget_inputbox_get_cursor_pos      (t_widget *w);
void        widget_inputbox_set_cursor_pos      (t_widget *w, int cursor_pos);
void        widget_inputbox_set_callback_enter  (t_widget *w, void (*callback_enter)());
void        widget_inputbox_set_callback_edit   (t_widget *w, void (*callback_edit)());
void        widget_inputbox_set_flags           (t_widget *w, t_widget_inputbox_flags flags, bool enable);
void        widget_inputbox_set_content_type    (t_widget *w, t_widget_content_type content_type);
void        widget_inputbox_set_insert_mode     (t_widget *w, int insert_mode);
//-----------------------------------------------------------------------------
