//-----------------------------------------------------------------------------
// MEKA - g_widget.h
// GUI Widgets - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

typedef enum
{
    WIDGET_TYPE_CLOSEBOX,
    WIDGET_TYPE_BUTTON,
    WIDGET_TYPE_SCROLLBAR,
    WIDGET_TYPE_CHECKBOX,
    WIDGET_TYPE_TEXTBOX,
    WIDGET_TYPE_INPUTBOX,
} t_widget_type;

typedef enum
{
    WIDGET_CONTENT_TYPE_TEXT        = 0,
    WIDGET_CONTENT_TYPE_DECIMAL     = 1,
    WIDGET_CONTENT_TYPE_HEXADECIMAL = 2,
} t_widget_content_type;

typedef enum
{
    WIDGET_INPUTBOX_FLAGS_DEFAULT                   = 0,
    WIDGET_INPUTBOX_FLAGS_NO_CURSOR                 = 0x0001,
    WIDGET_INPUTBOX_FLAGS_NO_MOVE_CURSOR            = 0x0002,
    WIDGET_INPUTBOX_FLAGS_NO_DELETE                 = 0x0004,
    WIDGET_INPUTBOX_FLAGS_HIGHLIGHT_CURRENT_CHAR    = 0x0008,
    WIDGET_INPUTBOX_FLAGS_COMPLETION                = 0x0010,
    WIDGET_INPUTBOX_FLAGS_HISTORY                   = 0x0020,
} t_widget_inputbox_flags;

typedef enum
{
    WIDGET_SCROLLBAR_TYPE_VERTICAL                  = 0,
    WIDGET_SCROLLBAR_TYPE_HORIZONTAL                = 1,
} t_widget_scrollbar_type;

typedef enum
{
    WIDGET_MOUSE_ACTION_NONE    = 0,
    WIDGET_MOUSE_ACTION_HOVER   = 0x0001,
    WIDGET_MOUSE_ACTION_CLICK   = 0x0002,
} t_widget_mouse_action;

typedef enum
{
    WIDGET_BUTTON_STYLE_INVISIBLE   = 0,
    WIDGET_BUTTON_STYLE_SMALL       = 1,
    WIDGET_BUTTON_STYLE_BIG         = 2,
} t_widget_button_style;

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_widget
{
    t_widget_type           type;
    int                     enabled;
    t_gui_box *             box;
    t_frame                 frame;
    bool                    dirty;
    void *                  data;

    // Mouse data
    int                     mouse_x;
    int                     mouse_y;
    int                     mouse_buttons;
    int                     mouse_buttons_previous;
    int                     mouse_buttons_mask;
	int                     mouse_buttons_activation;
    t_widget_mouse_action   mouse_action;

    // Handlers
    void                    (*redraw)();
    void                    (*update)();

    // User data
    void *                  user_data;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Updating functions ---------------------------------------------------------
int         widgets_update_box              (t_gui_box *box, int cx, int cy);
void        widgets_call_update             (void);

// Widget: generics -----------------------------------------------------------
void        widget_delete                   (t_widget *w);
void        widget_enable                   (t_widget *w);
void        widget_disable                  (t_widget *w);
void        widget_set_dirty                (t_widget *w);
void        widget_set_mouse_buttons_mask   (t_widget *w, int mouse_buttons_mask);
void *      widget_get_user_data            (t_widget *w);
void        widget_set_user_data            (t_widget *w, void *user_data);
//-----------------------------------------------------------------------------

// Widget: star button at the top right of a box ------------------------------
t_widget *  widget_closebox_add             (t_gui_box *box, void (*callback)());
void        widget_closebox_update          (t_widget *w);
void        widget_closebox_redraw          (t_widget *w);
//-----------------------------------------------------------------------------

// Widget: button -------------------------------------------------------------
t_widget *  widget_button_add               (t_gui_box *box, const t_frame *frame, int mouse_buttons_mask, void (*callback)(), t_widget_button_style style, const char *label);
void        widget_button_update            (t_widget *w);
void        widget_button_redraw            (t_widget *w);
void        widget_button_set_selected      (t_widget *w, bool selected);
//-----------------------------------------------------------------------------

// Widget: scroll bar ---------------------------------------------------------
t_widget *  widget_scrollbar_add            (t_gui_box *box, t_widget_scrollbar_type scrollbar_type, const t_frame *frame, int *v_max, int *v_start, int *v_per_page, void (*callback)());
void        widget_scrollbar_update         (t_widget *w);
void        widget_scrollbar_redraw			(t_widget *w);
//-----------------------------------------------------------------------------

// Widget: check box ----------------------------------------------------------
t_widget *  widget_checkbox_add                     (t_gui_box *box, const t_frame *frame, bool *pvalue, void (*callback)());
void        widget_checkbox_update                  (t_widget *w);
void        widget_checkbox_redraw                  (t_widget *w);
void        widget_checkbox_set_pvalue              (t_widget *w, bool *pvalue);
//-----------------------------------------------------------------------------

// Widget: text box -----------------------------------------------------------
t_widget *  widget_textbox_add                      (t_gui_box *box, const t_frame *frame, int lines_max, int font_idx);
void        widget_textbox_redraw                   (t_widget *w);
void        widget_textbox_clear                    (t_widget *w);
void        widget_textbox_set_current_color        (t_widget *w, int *pcurrent_color);
void        widget_textbox_print_scroll             (t_widget *w, int wrap, const char *line);
void        widget_textbox_printf_scroll            (t_widget *w, int wrap, const char *format, ...);
//-----------------------------------------------------------------------------

// Widget: input box ----------------------------------------------------------
t_widget *  widget_inputbox_add                     (t_gui_box *box, const t_frame *frame, int length_max, int font_idx, void (*callback_enter)(t_widget *));
void        widget_inputbox_update                  (t_widget *w);
void        widget_inputbox_redraw                  (t_widget *w);
const char *widget_inputbox_get_value               (t_widget *w);
int         widget_inputbox_get_value_length        (t_widget *w);
void        widget_inputbox_set_value               (t_widget *w, const char *value);
int         widget_inputbox_get_cursor_pos          (t_widget *w);
void        widget_inputbox_set_cursor_pos          (t_widget *w, int cursor_pos);
void        widget_inputbox_set_callback_enter      (t_widget *w, void (*callback_enter)(t_widget *));
void        widget_inputbox_set_callback_edit       (t_widget *w, void (*callback_edit)(t_widget *));
void        widget_inputbox_set_flags               (t_widget *w, t_widget_inputbox_flags flags, bool enable);
void        widget_inputbox_set_content_type        (t_widget *w, t_widget_content_type content_type);
void        widget_inputbox_set_insert_mode         (t_widget *w, int insert_mode);
void        widget_inputbox_set_callback_completion (t_widget *w, bool (*callback_completion)(t_widget *));
void        widget_inputbox_set_callback_history    (t_widget *w, bool (*callback_history)(t_widget *, int level));
bool        widget_inputbox_insert_char             (t_widget *w, char c);
bool        widget_inputbox_insert_string           (t_widget *w, const char *str);
bool        widget_inputbox_delete_current_char     (t_widget *w);
//-----------------------------------------------------------------------------
