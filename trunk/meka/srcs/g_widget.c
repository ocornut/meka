//-----------------------------------------------------------------------------
// MEKA - g_widget.c
// GUI Widgets - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "g_widget.h"
#include "inputs_t.h"

//-----------------------------------------------------------------------------
// Private Data
//-----------------------------------------------------------------------------

typedef struct
{
    int         lighted;
    void        (*callback)();
} t_widget_data_closebox;

typedef struct
{
    void        (*callback)();
} t_widget_data_button;

typedef struct
{
    int *       v_max;
    int *       v_start;
    int *       v_per_page;
    void        (*callback)();
} t_widget_data_scrollbar;

typedef struct
{
    byte *      pvalue;
    void        (*callback)();
} t_widget_data_checkbox;

typedef struct
{
    int         color;
    char *      text;
}
t_widget_data_textbox_line;

typedef struct
{
    int         lines_num;
    int         lines_max;
    int         columns_max;
    t_widget_data_textbox_line *     lines;
    int         font_idx;
    int         current_color;
    int         need_redraw;
} t_widget_data_textbox;

// FIXME: add focus parameters: update inputs on box OR widget focus
// FIXME: Do not show cursor without focus ?
typedef struct
{
    t_widget_inputbox_flags flags;
    t_widget_content_type   content_type;
    int                     insert_mode;    // Boolean
    char *                  value;
    int                     length;
    int                     length_max;
    void                    (*callback_edit)();
    void                    (*callback_enter)();
    int                     cursor_pos;
    int                     font_idx;
} t_widget_data_inputbox;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

int         widgets_update_box (t_gui_box *b, int mouse_x, int mouse_y)
{
    int     j;
    bool    wm = TRUE;

    for (j = 0; j < b->n_widgets; j ++)
    {
        t_widget *w = b->widgets [j];
        if (!w->enabled)
            continue;

        w->mx = -1;
        w->my = -1;
        w->mouse_action = WIDGET_MOUSE_ACTION_NONE;
        if ((mouse_x >= w->frame.pos.x) && (mouse_y >= w->frame.pos.y) && (mouse_x < w->frame.pos.x + w->frame.size.x) && (mouse_y < w->frame.pos.y + w->frame.size.y))
        {
            w->mx = mouse_x - w->frame.pos.x;
            w->my = mouse_y - w->frame.pos.y;
            w->mb = gui_mouse.button;
            w->mouse_action |= WIDGET_MOUSE_ACTION_HOVER;
            if (w->mb & w->mb_react)
            {
                gui_mouse.pressed_on = PRESSED_ON_WIDGET;
                w->mouse_action |= WIDGET_MOUSE_ACTION_CLICK;
                wm = FALSE;
            }
        }
    }
    return (wm);
}

void    widgets_call_update (void)
{
    int    i, j;

    for (i = 0; i < gui.box_last; i ++)
    {
        if (gui.box[i]->n_widgets > 0)
        {
            for (j = 0; j < gui.box[i]->n_widgets; j ++)
            {
                t_widget *w = gui.box[i]->widgets [j];
                if (w->enabled && w->update != NULL)
                    w->update (w);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// widget_new ()
// Create a new widget in given box
// FIXME: additionnal widget data should be allocated on the same time...
//-----------------------------------------------------------------------------
t_widget *  widget_new (int box_n)
{
    t_gui_box *box = gui.box [box_n];
    t_widget *w;

    // Create and initialize widget
    w = (t_widget *) malloc(sizeof (t_widget));
    w->enabled      = YES; // Enabled by default
    w->id           = box->n_widgets;
    w->box_n        = box_n;
    w->box          = box;
    w->frame.pos.x  = 0;
    w->frame.pos.y  = 0;
    w->frame.size.x = 0;
    w->frame.size.y = 0;
    w->mx           = -1;
    w->my           = -1;
    w->mb           = 0;
    w->mb_old       = 0;
    w->mb_react     = 0;
    w->redraw       = NULL;
    w->update       = NULL;
    w->data         = NULL;

    // Add to box
    if (box->n_widgets == 0)
        box->widgets = malloc (sizeof (t_widget *));
    else
        box->widgets = realloc (box->widgets, (box->n_widgets + 1) * sizeof (t_widget *));
    box->widgets [box->n_widgets++] = w;
    return (w);
}

void        widget_enable (t_widget *w)
{
    w->enabled = YES;
}

void        widget_disable (t_widget *w)
{
    w->enabled = NO;
}

void        widget_init_mb (t_widget *w, int react)
{
    w->mb = 0;
    w->mb_old = 0;
    w->mb_react = react;
}

//-----------------------------------------------------------------------------

t_widget *  widget_closebox_add (int box_n, void *callback)
{
    t_widget *w;
    t_widget_data_closebox *wd;
    t_gui_box *box = gui.box[box_n];

    w = widget_new (box->stupid_id);
    widget_init_mb (w, 1);
    w->frame.pos.x = box->frame.size.x - 10;
    w->frame.pos.y = -15;
    w->frame.size.x = 7;
    w->frame.size.y = 6;

    w->redraw = widget_closebox_redraw;
    w->update = widget_closebox_update;

    w->data = malloc (sizeof (t_widget_data_closebox));
    wd = w->data;
    wd->lighted = NO;
    wd->callback = callback;

    return w;
}

void        widget_closebox_redraw (t_widget *w, int bx, int by)
{
    int    color;
    t_widget_data_closebox *wd = w->data;

    // Get appropriate color
    color = (wd->lighted) ? GUI_COL_TEXT_ACTIVE : GUI_COL_TEXT_N_ACTIVE;

    // Draw box-closing star using the LARGE font
    Font_Print (F_LARGE, gui_buffer, MEKA_FONT_STR_STAR, bx + w->frame.pos.x, by + w->frame.pos.y - 4, color);
}

void        widget_closebox_update (t_widget *w)
{
    t_widget_data_closebox *wd = w->data;
    if (!wd->lighted)
    {
        if ((w->mouse_action & WIDGET_MOUSE_ACTION_CLICK) && (w->mb_old == 0) && (w->mb == 1))
        {
            wd->lighted = YES;
        }
    }
    else
        if ((wd->lighted) && (w->mb_old == 1) && (w->mb == 0))
        {
            wd->lighted = NO;
            if ((w->mx >= 0) && (w->my >= 0) && (w->mx <= w->frame.size.x) && (w->my <= w->frame.size.y))
            {
                wd->callback ();
            }
        }
        w->mb_old = w->mb;
        w->mb = 0;
}

//-----------------------------------------------------------------------------

t_widget *  widget_button_add_draw (int box_n, t_frame *frame, int look, int FontIdx, int react, void *when_clicked, char *s)
{
    t_widget *w;
    w = widget_button_add (box_n, frame, react, when_clicked);
    rectfill (gui.box_image [box_n], frame->pos.x + 2, frame->pos.y + 2, frame->pos.x + frame->size.x - 2, frame->pos.y + frame->size.y - 2, GUI_COL_BUTTONS);
    gui_rect (gui.box_image [box_n], look, frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x, frame->pos.y + frame->size.y, GUI_COL_BORDERS);
    Font_Print (FontIdx, gui.box_image [box_n], s, frame->pos.x + ((frame->size.x - Font_TextLength (FontIdx, s)) / 2), frame->pos.y + ((frame->size.y - Font_Height(FontIdx)) / 2) + 1, GUI_COL_TEXT_ACTIVE);
    return w;
}

t_widget *  widget_button_add (int box_n, t_frame *frame, int react, void *callback)
{
    t_widget *w;
    t_widget_data_button *wd;

    w = widget_new (box_n);
    widget_init_mb (w, react);
    w->frame = *frame;

    w->redraw = NULL;
    w->update = widget_button_update;

    w->data = malloc (sizeof (t_widget_data_button));
    wd = w->data;
    wd->callback = callback;

    return w;
}

void        widget_button_update (t_widget *w)
{
    int     call;
    t_widget_data_button *wd = w->data;

    // Check if we need to fire the callback
    call = ((w->mouse_action & WIDGET_MOUSE_ACTION_CLICK) && (w->mb & w->mb_react) && (w->mb_old == 0));

    // Update mouse buttons
    w->mb_old = w->mb;
    w->mb = 0;

    // Fire the callback. Note that it is done AFTER updating the moue buttons, this
    // is to avoid recursive calls if the callback ask for a GUI update (something
    // which the file browser does when clicking on "Load Names").
    if (call)
        wd->callback (w);
}

//-----------------------------------------------------------------------------

t_widget *  widget_scrollbar_add (int box_n, t_frame *frame, int *v_max, int *v_start, int *v_per_page, void *callback)
{
    t_widget *w;
    t_widget_data_scrollbar *wd;

    w = widget_new (box_n);
    widget_init_mb (w, 1);
    w->frame = *frame;

    w->redraw = NULL;
    w->update = widget_scrollbar_update;

    w->data = (t_widget_data_scrollbar *)malloc (sizeof (t_widget_data_scrollbar));
    wd = w->data;
    wd->v_max = v_max;
    wd->v_start = v_start;
    wd->v_per_page = v_per_page;
    wd->callback = callback;

    return w;
}

void        widget_scrollbar_update (t_widget *w)
{
    int    a = 0;
    float  pos, h, max;
    t_widget_data_scrollbar *wd = w->data;

    if ((w->mb & w->mb_react) /*&& (w->mb_old == 0)*/ && (w->mouse_action & WIDGET_MOUSE_ACTION_CLICK))
    {
        a = 1;
        *wd->v_start = ((w->my * *wd->v_max) / w->frame.size.y) - (*wd->v_per_page / 2);
    }
    w->mb_old = w->mb;
    w->mb = 0;

    if (*wd->v_start > *wd->v_max - *wd->v_per_page) 
        *wd->v_start = *wd->v_max - *wd->v_per_page;
    if (*wd->v_start < 0) 
        *wd->v_start = 0;

    // Clear bar
    rectfill (gui.box_image [w->box_n], w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, GUI_COL_FILL);

    // Draw position box
    max = *wd->v_max;
    if (max < *wd->v_per_page) 
        max = *wd->v_per_page;
    pos = w->frame.pos.y + ((*wd->v_start * w->frame.size.y) / max);
    h = (*wd->v_per_page * w->frame.size.y) / max;
    rectfill (gui.box_image [w->box_n], w->frame.pos.x, pos, w->frame.pos.x + w->frame.size.x, pos + h, GUI_COL_HIGHLIGHT);

    if (a == 1)
    {
        wd->callback (w);
    }
}

//-----------------------------------------------------------------------------

t_widget *  widget_checkbox_add (int box_n, t_frame *frame, byte *pvalue, void *callback)
{
    t_widget *w;
    t_widget_data_checkbox *wd;

    w = widget_new (box_n);
    widget_init_mb (w, 1);
    w->frame = *frame;

    w->redraw = NULL; // widget_checkbox_redraw;
    w->update = widget_checkbox_update;

    w->data = (t_widget_data_checkbox *)malloc (sizeof (t_widget_data_checkbox));
    wd = w->data;
    wd->pvalue = NULL;
    widget_checkbox_redraw (w);
    wd->pvalue = pvalue;
    widget_checkbox_redraw (w);
    wd->callback = callback;

    return w;
}

// FIXME: potential bug there, if wd->pvalue is NULL..
void        widget_checkbox_update (t_widget *w)
{
    t_widget_data_checkbox *wd;

    if ((w->mb & w->mb_react) && (w->mb_old == 0) && (w->mouse_action & WIDGET_MOUSE_ACTION_CLICK))
    {
        wd = w->data;
        *(wd->pvalue) ^= 1; // inverse flag
        if (wd->callback)
            wd->callback ();
        widget_checkbox_redraw (w);
    }
    w->mb_old = w->mb;
    w->mb = 0;
}

void        widget_checkbox_redraw (t_widget *w)
{
    t_widget_data_checkbox *wd = w->data;

    rect (gui.box_image [w->box_n], w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, GUI_COL_BORDERS);
    rectfill (gui.box_image [w->box_n], w->frame.pos.x + 1, w->frame.pos.y + 1, w->frame.pos.x + w->frame.size.x - 1, w->frame.pos.y + w->frame.size.y - 1, GUI_COL_BUTTONS);
    if (wd->pvalue && *(wd->pvalue))
    {
        line (gui.box_image [w->box_n], w->frame.pos.x + 2, w->frame.pos.y + 2, w->frame.pos.x + w->frame.size.x - 2, w->frame.pos.y + w->frame.size.y - 2, GUI_COL_TEXT_ACTIVE);
        line (gui.box_image [w->box_n], w->frame.pos.x + w->frame.size.x - 2, w->frame.pos.y + 2, w->frame.pos.x + 2, w->frame.pos.y + w->frame.size.y - 2, GUI_COL_TEXT_ACTIVE);
    }
}

void        widget_checkbox_set_pvalue (t_widget *w, byte *pvalue)
{
    t_widget_data_checkbox *wd = w->data;
    wd->pvalue = pvalue;
}

//-----------------------------------------------------------------------------

t_widget *      widget_textbox_add(int box_n, t_frame *frame, int lines_max, int font_idx)
{
    int         i;
    t_widget *  w;
    t_widget_data_textbox *wd;

    // Create widget
    w = widget_new (box_n);
    widget_init_mb (w, 0);
    w->frame.pos.x = frame->pos.x;
    w->frame.pos.y = frame->pos.y;
    w->frame.size.x = frame->size.x;
    w->frame.size.y = frame->size.y;

    w->redraw = widget_textbox_redraw;
    w->update = NULL;

    // Setup values & parameters
    w->data = malloc (sizeof (t_widget_data_textbox));
    wd = w->data;
    wd->lines_num       = 0;
    wd->lines_max       = lines_max;
    wd->font_idx        = font_idx;
    wd->current_color   = GUI_COL_TEXT_IN_BOX;
    wd->need_redraw     = YES;

    // FIXME: To compute 'columns_max', we assume that 'M' is the widest character 
    // of a font. Which may or may not be true, and should be fixed. 
    wd->columns_max = w->frame.size.x / Font_TextLength (font_idx, "M");
    wd->lines       = malloc (sizeof (t_widget_data_textbox_line) * lines_max);
    for (i = 0; i < lines_max; i++)
    {
        wd->lines[i].color = wd->current_color;
        wd->lines[i].text = malloc (sizeof (char) * (wd->columns_max + 1));
        wd->lines[i].text[0] = EOSTR;
    }

    // Return newly created widget
    return (w);
}

void        widget_textbox_redraw(t_widget *w)
{
    t_widget_data_textbox *wd = w->data;

    if (wd->need_redraw)
    {
        int fh = Font_Height (wd->font_idx);
        int x = w->frame.pos.x;
        int y = w->frame.pos.y + w->frame.size.y - fh;
        int i;
        BITMAP *bmp = gui.box_image[w->box_n];

        /*
        {
            static cnt = 0;
            Msg (MSGT_USER_INFOLINE, "widget_textbox_redraw() %d", cnt++);
        }
        // rect (gui.box_image [w->box_n], w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, GUI_COL_BORDERS);
        */

        rectfill (bmp, w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, GUI_COL_FILL);
        for (i = 0; i < wd->lines_num; i++)
        {
            if (wd->lines[i].text[0] != EOSTR)
                Font_Print (wd->font_idx, bmp, wd->lines[i].text, x, y, wd->lines[i].color);
            y -= fh;
        }

        // Clear our own flag now
        wd->need_redraw = NO;
    }
}

void        widget_textbox_clear(t_widget *w)
{
    t_widget_data_textbox *wd = w->data;
    int i;
    for (i = 0; i < wd->lines_num; i++)
        wd->lines[i].text[0] = EOSTR;
    wd->lines_num = 0;

    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    w->box->must_redraw = YES;
    wd->need_redraw = YES;
}

void        widget_textbox_set_current_color(t_widget *w, int current_color)
{
    t_widget_data_textbox *wd = w->data;
    wd->current_color = current_color;
}

static void widget_textbox_print_scroll_nowrap (t_widget *w, const char *line)
{
    t_widget_data_textbox *wd = w->data;

    // Shift all lines by one position
    // FIXME: should replace this by a circular queue
    if (wd->lines_num > 0)
    {
        t_widget_data_textbox_line wrapped_line = wd->lines[wd->lines_max - 1];
        int n;
        for (n = wd->lines_max - 1; n > 0; n--)
            wd->lines[n] = wd->lines[n - 1];
        wd->lines[0] = wrapped_line;
    }

    // Increment number of lines counter
    if (wd->lines_num < wd->lines_max)
        wd->lines_num++;

    // Copy new line
    strcpy (wd->lines[0].text, line);
    wd->lines[0].color = wd->current_color;
  
    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    w->box->must_redraw = YES;
    wd->need_redraw = YES;
}

void        widget_textbox_print_scroll (t_widget *w, int wrap, const char *line)
{
    t_widget_data_textbox *wd = w->data;
    int     pos;
    char    buf [MSG_MAX_LEN];
    const char *    src;

    if (!wrap || line[0] == EOSTR || Font_TextLength (wd->font_idx, line) <= w->frame.size.x)
    {
        widget_textbox_print_scroll_nowrap (w, line);
        return;
    }

    pos = 0;
    src = line;
    while (*src != EOSTR)
    {
        // Add one character
        buf[pos] = *src;
        buf[pos+1] = EOSTR;

        // Compute length
        // FIXME: this algorythm sucks
        if (Font_TextLength (wd->font_idx, buf) > w->frame.size.x)
        {
            char *blank = strrchr (buf, ' ');
            if (blank != NULL)
            {
                int d = buf + pos - blank;
                src -= d; // rewind
                pos -= d;
                buf[pos] = EOSTR;
            }
            else
            {
                // No blank... cut a current character
                buf[pos] = EOSTR;
                src--; // Rewind by one to display the character later
                pos--;
            }
            widget_textbox_print_scroll_nowrap (w, buf);
            pos = 0;
        }
        else
        {
            pos++;
        }
        src++;
    }

    // Some text left, print it
    if (pos != 0)
        widget_textbox_print_scroll_nowrap (w, buf);
}

void        widget_textbox_printf_scroll(t_widget *w, int wrap, const char *format, ...)
{
    // FIXME: yet unsupported
    assert(0);
}

//-----------------------------------------------------------------------------

t_widget *  widget_inputbox_add(int box_n, t_frame *frame, int length_max, int font_idx, void *callback_enter)
{
    t_widget *w;
    t_widget_data_inputbox *wd;

    // Get and verify size
    int size_x = frame->size.x;
    int size_y = frame->size.y;
    assert(size_x != -1);
    if (size_y == -1)
       size_y = Font_Height (font_idx) + 4;

    // Create widget
    w = widget_new (box_n);
    widget_init_mb (w, 1);
    w->frame.pos.x = frame->pos.x;
    w->frame.pos.y = frame->pos.y;
    w->frame.size.x = size_x;
    w->frame.size.y = size_y;

    w->redraw = widget_inputbox_redraw;
    w->update = widget_inputbox_update;

    // Setup values & parameters
    w->data = malloc (sizeof (t_widget_data_inputbox));
    wd = w->data;
    wd->flags           = WIDGET_INPUTBOX_FLAG_DEFAULT;
    wd->content_type    = WIDGET_CONTENT_TYPE_TEXT;
    wd->insert_mode     = FALSE;
    wd->length_max      = length_max;
    assert(length_max != -1); // Currently, length must be fixed
    wd->value           = malloc (sizeof (char) * (length_max + 1));
    strcpy (wd->value, "");
    wd->cursor_pos      = wd->length = 0;
    wd->callback_enter  = callback_enter;
    wd->callback_edit   = NULL;
    wd->font_idx        = font_idx;

    // Return newly created widget
    return (w);
}

//-----------------------------------------------------------------------------
// widget_inputbox_update (t_widget *)
// Update input box based on user keyboard inputs
//-----------------------------------------------------------------------------
// // Note: the global key[] table is altered whenever a key is read here.
// // The reason is to avoid savestate change/reseting or other tricky side-effects
// // of having the typed keys handled elsewhere. 
// // Of course, this way of doing things isn't correct, and we'll have to switch
// // to correct focus and input handling later.
//-----------------------------------------------------------------------------
void        widget_inputbox_update(t_widget *w)
{
    t_widget_data_inputbox *wd;
    const int tm_delay = 15;
    const int tm_rate = 1;
    bool edited = FALSE;

    // Check if we have focus
    // FIXME: This is completely a hack since it checks BOX focus (and not Widget focus,
    // which is not possible right now). So if there's more than one inputbox in the same
    // box, currently both will be updated.
    if (!gui_box_has_focus (w->box))
        return;

    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    w->box->must_redraw = YES;

    // Get pointer to inputbox specific data
    wd = w->data;

    // Msg (MSGT_DEBUG, "cascii = %c, cscan = %04x", Inputs.KeyPressed.ascii, Inputs.KeyPressed.scancode);

    // Mouse click set position
    if (!(wd->flags & WIDGET_INPUTBOX_FLAG_NO_MOVE_CURSOR))
        if ((w->mb & w->mb_react) && (w->mb_old == 0) && (w->mouse_action & WIDGET_MOUSE_ACTION_CLICK))
        {
            int mx = w->mx -= (2 + 3); // 2+3=start of text entry, see _Redraw() // FIXME
            if (mx <= 0)
                widget_inputbox_set_cursor_pos (w, 0);
            else
            {
                int i;
                char s[2] = { EOSTR, EOSTR };
                for (i = 0; i < wd->length; i++)
                {
                    s[0] = wd->value[i];
                    mx -= Font_TextLength (wd->font_idx, s);
                    if (mx <= 0)
                        break;
                }
                widget_inputbox_set_cursor_pos (w, i);
            }
        }

    // Check for printable input keys
    if (((wd->length < wd->length_max) || (wd->insert_mode == TRUE && wd->cursor_pos < wd->length)) && 
        (Inputs.KeyPressed.scancode != 0))
    {
        t_key_info *ki = KeyInfo_FindByScancode(Inputs.KeyPressed.scancode);

        if (ki != NULL)
            if (ki->flags & KEY_INFO_PRINTABLE)
            {
                char c = Inputs.KeyPressed.ascii; // ki->printable_char
                if (wd->content_type == WIDGET_CONTENT_TYPE_DECIMAL)
                {
                    if (!(c >= '0' && c <= '9'))
                        c = 0;
                }
                else if (wd->content_type == WIDGET_CONTENT_TYPE_HEXADECIMAL)
                {
                    if (c >= 'a' && c <= 'f')
                        c += 'A' - 'a';
                    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
                        c = 0;
                }
                if (c != 0)
                {
                    // Shift everything that is after cursor pos
                    if (wd->insert_mode == FALSE && wd->cursor_pos < wd->length)
                    {
                        int i;
                        for (i = wd->length; i > wd->cursor_pos; i--)
                            wd->value[i] = wd->value[i-1];
                    }

                    // Insert at cursor pos
                    wd->value[wd->cursor_pos] = c;
                    wd->cursor_pos++;
                    if (wd->length < wd->length_max)
                    {
                        wd->length++;
                        wd->value[wd->length] = '\0';
                    }

                    // Edit callback
                    if (wd->callback_edit)
                        wd->callback_edit();

                    // Eat flag in key[]
                    Inputs_Key_Eat(Inputs.KeyPressed.scancode);
                    return;
                }
            }
    }

    // Backspace, Delete
    if (!(wd->flags & WIDGET_INPUTBOX_FLAG_NO_DELETE))
        if (Inputs_KeyPressed_Repeat (KEY_BACKSPACE, NO, tm_delay, tm_rate) || Inputs_KeyPressed_Repeat (KEY_DEL, YES, tm_delay, tm_rate))
        {
            if (wd->cursor_pos > 0)
            {
                // Shift everything that is after cursor
                int i;
                for (i = wd->cursor_pos; i < wd->length; i++)
                    wd->value[i-1] = wd->value[i];
                wd->cursor_pos--;
                wd->length--;
                wd->value[wd->length] = '\0';
                edited = TRUE;
            }

            // HACK: This avoid resetting while backspacing in the widget
            // if (key[KEY_BACKSPACE])
            ///    Inputs_Key_Eat(KEY_BACKSPACE);
        }

    if (!(wd->flags & WIDGET_INPUTBOX_FLAG_NO_MOVE_CURSOR))
    {
        // Left/Right arrows: move cursor
        if (Inputs_KeyPressed_Repeat (KEY_LEFT, NO, tm_delay, tm_rate))
            if (wd->cursor_pos > 0)
                wd->cursor_pos--;
        if (Inputs_KeyPressed_Repeat (KEY_RIGHT, NO, tm_delay, tm_rate))
            if (wd->cursor_pos < wd->length)
                wd->cursor_pos++;

        // Home/End: set cursor to beginning/end of input box
        if (Inputs_KeyPressed (KEY_HOME, NO))
            wd->cursor_pos = 0;
        if (Inputs_KeyPressed (KEY_END, NO))
            wd->cursor_pos = wd->length;
    }

    // Edit callback
    if (edited && wd->callback_edit)
        wd->callback_edit();

    // Enter: validate
    if (Inputs_KeyPressed_Repeat (KEY_ENTER, NO, 30, 3))
        if (wd->callback_enter)
            wd->callback_enter();
}

void        widget_inputbox_redraw(t_widget *w)
{
    t_widget_data_inputbox *wd = w->data;
    const bool draw_cursor = !(wd->flags & WIDGET_INPUTBOX_FLAG_NO_CURSOR);
    const bool highlight_all = !(wd->flags & WIDGET_INPUTBOX_FLAG_HIGHLIGHT_CURRENT_CHAR);

    // Draw border & fill text area
    rect (gui.box_image [w->box_n], w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, GUI_COL_BORDERS);
    rectfill (gui.box_image [w->box_n], w->frame.pos.x + 1, w->frame.pos.y + 1, w->frame.pos.x + w->frame.size.x - 1, w->frame.pos.y + w->frame.size.y - 1, GUI_COL_BUTTONS);

    // Draw text & cursor
    {
        int x = w->frame.pos.x + 2 + 3; // Note: +3 to center font
        int y = w->frame.pos.y + (w->frame.size.y - Font_Height (wd->font_idx)) / 2 + 1;
        int i;
        for (i = 0; i < wd->length + 1; i++)
        {
            if (draw_cursor && i == wd->cursor_pos)
            {
                // Draw cursor line
                int cursor_y1 = w->frame.pos.y + 2;
                int cursor_y2 = cursor_y1 + w->frame.size.y - 2*2;
                vline (gui.box_image [w->box_n], x, cursor_y1, cursor_y2, GUI_COL_TEXT_ACTIVE);
            }
            if (i < wd->length)
            {
                // Draw one character
                int color = (highlight_all || i == wd->cursor_pos) ? GUI_COL_TEXT_ACTIVE : GUI_COL_TEXT_N_ACTIVE;
                char ch[2];
                ch[0] = wd->value[i];
                ch[1] = '\0';
                Font_Print (wd->font_idx, gui.box_image [w->box_n], ch, x, y, color);
                x += Font_TextLength (wd->font_idx, ch); // A bit slow
            }
        }
    }
}

char *      widget_inputbox_get_value(t_widget *w)
{
    t_widget_data_inputbox *wd = w->data;
    return (wd->value);
}

void        widget_inputbox_set_value(t_widget *w, char *value)
{
    t_widget_data_inputbox *wd = w->data;
    int     len;
    
    // Copy new value
    len = strlen(value);
    if (len < wd->length_max)
    {
        strcpy (wd->value, value);
        wd->length = len;
    }
    else
    {
        strncpy (wd->value, value, wd->length_max);
        wd->value[wd->length_max] = '\0';
        wd->length = wd->length_max;
    }

    // Set cursor to end of text
    // Eventually we'll add some parameters to avoid or tweak that behavior...
    wd->cursor_pos = wd->length;
    
    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    w->box->must_redraw = YES;
}

int         widget_inputbox_get_cursor_pos(t_widget *w)
{
    t_widget_data_inputbox *wd = w->data;
    return (wd->cursor_pos);
}

void        widget_inputbox_set_cursor_pos(t_widget *w, int cursor_pos)
{
    t_widget_data_inputbox *wd = w->data;

    if (cursor_pos < 0)
        return;
    if (cursor_pos > wd->length_max)
        return;

    // Set cursor position to given position
    // Eventually we'll add some parameters to avoid or tweak that behavior...
    wd->cursor_pos = cursor_pos;
    
    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    w->box->must_redraw = YES;
}

void        widget_inputbox_set_callback_enter(t_widget *w, void (*callback_enter)())
{
    t_widget_data_inputbox *wd = w->data;
    wd->callback_enter = callback_enter;
}

void        widget_inputbox_set_callback_edit(t_widget *w, void (*callback_edit)())
{
    t_widget_data_inputbox *wd = w->data;
    wd->callback_edit = callback_edit;
}

void        widget_inputbox_set_flags(t_widget *w, t_widget_inputbox_flags flags, bool enable)
{
    t_widget_data_inputbox *wd = w->data;
    if (enable)
        wd->flags |= flags;
    else
        wd->flags &= ~flags;
    // FIXME: changing NO_CURSOR dynamically won't refresh
}

void        widget_inputbox_set_content_type(t_widget *w, t_widget_content_type content_type)
{
    t_widget_data_inputbox *wd = w->data;
    wd->content_type = content_type;
}

void        widget_inputbox_set_insert_mode(t_widget *w, int insert_mode)
{
    t_widget_data_inputbox *wd = w->data;
    wd->insert_mode = insert_mode;
}


//-----------------------------------------------------------------------------
