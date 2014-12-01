//-----------------------------------------------------------------------------
// MEKA - g_widget.h
// GUI Widgets - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

enum t_widget_type
{
    WIDGET_TYPE_CLOSEBOX,
    WIDGET_TYPE_BUTTON,
    WIDGET_TYPE_SCROLLBAR,
    WIDGET_TYPE_CHECKBOX,
    WIDGET_TYPE_TEXTBOX,
    WIDGET_TYPE_INPUTBOX,
};

enum t_widget_content_type
{
    WIDGET_CONTENT_TYPE_TEXT        = 0,
    WIDGET_CONTENT_TYPE_DECIMAL     = 1,
    WIDGET_CONTENT_TYPE_HEXADECIMAL = 2,
	WIDGET_CONTENT_TYPE_DEC_HEX_BIN	= 3,
};

enum t_widget_inputbox_flags
{
    WIDGET_INPUTBOX_FLAGS_DEFAULT                   = 0,
    WIDGET_INPUTBOX_FLAGS_NO_CURSOR                 = 0x0001,
    WIDGET_INPUTBOX_FLAGS_NO_MOVE_CURSOR            = 0x0002,
    WIDGET_INPUTBOX_FLAGS_NO_DELETE                 = 0x0004,
    WIDGET_INPUTBOX_FLAGS_HIGHLIGHT_CURRENT_CHAR    = 0x0008,
    WIDGET_INPUTBOX_FLAGS_COMPLETION                = 0x0010,
    WIDGET_INPUTBOX_FLAGS_HISTORY                   = 0x0020,
	WIDGET_INPUTBOX_FLAGS_NO_SELECTION              = 0x0040,
};

enum t_widget_scrollbar_type
{
    WIDGET_SCROLLBAR_TYPE_VERTICAL                  = 0,
    WIDGET_SCROLLBAR_TYPE_HORIZONTAL                = 1,
};

enum t_widget_mouse_action
{
    WIDGET_MOUSE_ACTION_NONE    = 0,
    WIDGET_MOUSE_ACTION_HOVER   = 0x0001,
    WIDGET_MOUSE_ACTION_CLICK   = 0x0002,
};

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef void (*t_widget_callback)(t_widget*);

struct t_widget
{
    t_widget_type           type;
    bool                    enabled;
	bool					highlight;
    t_gui_box *             box;
    t_frame                 frame;
    void *                  data;

    // Mouse data
    int                     mouse_x;
    int                     mouse_y;
    int                     mouse_buttons;
    int                     mouse_buttons_previous;
    int                     mouse_buttons_mask;
	int                     mouse_buttons_activation;
    int						mouse_action;				// (enum t_widget_mouse_action) // FIXME-ENUM

    // Handlers
	void					(*destroy_func)(t_widget* w);
    void                    (*redraw_func)(t_widget* w);
    void                    (*update_func)(t_widget* w);

    // User data
    void *                  user_data;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Updating functions ---------------------------------------------------------
bool        widgets_update_box              (t_gui_box *box, int cx, int cy);
void        widgets_call_update             ();

// Widget: generics -----------------------------------------------------------
void        widget_destroy                  (t_widget* w);
void        widget_set_enabled              (t_widget* w, bool v);
void        widget_set_highlight            (t_widget* w, bool v);
void        widget_set_mouse_buttons_mask   (t_widget* w, int mouse_buttons_mask);
void *      widget_get_user_data            (t_widget* w);
void        widget_set_user_data            (t_widget* w, void *user_data);
//-----------------------------------------------------------------------------

// Widget: star button at the top right of a box ------------------------------
t_widget *  widget_closebox_add             (t_gui_box *box, t_widget_callback callback);
void        widget_closebox_update          (t_widget* w);
void        widget_closebox_redraw          (t_widget* w);
//-----------------------------------------------------------------------------

// Widget: button -------------------------------------------------------------
t_widget *  widget_button_add               (t_gui_box* box, const t_frame* frame, int mouse_buttons_mask, t_widget_callback callback, t_font_id style, const char* label, void* user_data = NULL);
void        widget_button_update            (t_widget* w);
void        widget_button_redraw            (t_widget* w);
void        widget_button_set_grayed_out    (t_widget* w, bool grayed_out);
void		widget_button_set_label			(t_widget* w, const char* label);
void		widget_button_trigger			(t_widget* w);
//-----------------------------------------------------------------------------

// Widget: scroll bar ---------------------------------------------------------
t_widget *  widget_scrollbar_add            (t_gui_box *box, t_widget_scrollbar_type scrollbar_type, const t_frame *frame, const int *v_max, int *v_start, int v_step, t_widget_callback callback);
void        widget_scrollbar_update         (t_widget* w);
void        widget_scrollbar_redraw			(t_widget* w);
void		widget_scrollbar_set_page_step	(t_widget* w, int page_step);
//-----------------------------------------------------------------------------

// Widget: check box ----------------------------------------------------------
t_widget *  widget_checkbox_add                     (t_gui_box *box, const t_frame *frame, bool *pvalue, t_widget_callback callback);
void        widget_checkbox_update                  (t_widget* w);
void        widget_checkbox_redraw                  (t_widget* w);
void        widget_checkbox_set_pvalue              (t_widget* w, bool *pvalue);
//-----------------------------------------------------------------------------

// Widget: text box -----------------------------------------------------------
t_widget *  widget_textbox_add                      (t_gui_box *box, const t_frame *frame, t_font_id font_id);
void        widget_textbox_redraw                   (t_widget* w);
void        widget_textbox_clear                    (t_widget* w);
void        widget_textbox_set_current_color        (t_widget* w, const ALLEGRO_COLOR *pcurrent_color);
void        widget_textbox_print_scroll             (t_widget* w, bool wrap, const char *line);
void        widget_textbox_printf_scroll            (t_widget* w, bool wrap, const char *format, ...);
//-----------------------------------------------------------------------------

// Widget: input box ----------------------------------------------------------
t_widget *  widget_inputbox_add                     (t_gui_box *box, const t_frame *frame, int length_max, t_font_id font_id, t_widget_callback callback_enter);
void        widget_inputbox_update                  (t_widget* w);
void        widget_inputbox_redraw                  (t_widget* w);
const char *widget_inputbox_get_value               (t_widget* w);
int         widget_inputbox_get_value_length        (t_widget* w);
void        widget_inputbox_set_value               (t_widget* w, const char *value);
int         widget_inputbox_get_cursor_pos          (t_widget* w);
void        widget_inputbox_set_cursor_pos          (t_widget* w, int cursor_pos);
bool		widget_inputbox_has_selection			(t_widget* w);
void        widget_inputbox_set_selection			(t_widget* w, int sel_begin, int sel_end);
void        widget_inputbox_set_selection_end		(t_widget* w, int sel_end);
void        widget_inputbox_set_callback_enter      (t_widget* w, void (*callback_enter)(t_widget *));
void        widget_inputbox_set_callback_edit       (t_widget* w, void (*callback_edit)(t_widget *));
void        widget_inputbox_set_flags               (t_widget* w, int /*t_widget_inputbox_flags*/ flags, bool enable);	// FIXME-ENUM
void        widget_inputbox_set_content_type        (t_widget* w, t_widget_content_type content_type);
void        widget_inputbox_set_overwrite_mode		(t_widget* w, bool overwrite_mode);
void        widget_inputbox_set_callback_completion (t_widget* w, bool (*callback_completion)(t_widget *));
void        widget_inputbox_set_callback_history    (t_widget* w, bool (*callback_history)(t_widget *, int level));
bool        widget_inputbox_insert_chars			(t_widget* w, const char *str);
void        widget_inputbox_delete_selection		(t_widget* w);
void        widget_inputbox_delete_current_char     (t_widget* w);
//-----------------------------------------------------------------------------
