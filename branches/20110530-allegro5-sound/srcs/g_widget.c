//-----------------------------------------------------------------------------
// MEKA - g_widget.c
// GUI Widgets - Code
//-----------------------------------------------------------------------------
// FIXME: Static inheritance would be more proper than dynamic allocating 'data'
//-----------------------------------------------------------------------------

#include "shared.h"
#include "g_tools.h"
#include "g_widget.h"
#include "inputs_t.h"
#include "keyinfo.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define WIDGET_SCROLLBAR_MOUSE_TOLERANCE        (20)		// In pixels

//-----------------------------------------------------------------------------
// Private Data
//-----------------------------------------------------------------------------

struct t_widget_data_closebox
{
    int                         lighted;
    void                        (*callback)(t_widget*);
};

struct t_widget_data_button
{
    const char *                label;
    t_widget_button_style       style;
    bool                        selected;
    void                        (*callback)(t_widget*);
};

struct t_widget_data_scrollbar
{
    t_widget_scrollbar_type     scrollbar_type;
    int *                       v_max;
    int *                       v_start;
    int *                       v_per_page;
    void                        (*callback)(t_widget*);
};

struct t_widget_data_checkbox
{
    bool *                      pvalue;
    void                        (*callback)(t_widget*);
} ;

struct t_widget_data_textbox_line
{
    const ALLEGRO_COLOR *       pcolor;
    char *                      text;
};

struct t_widget_data_textbox
{
    int                         lines_num;
    int                         lines_max;
    int                         columns_max;
    t_widget_data_textbox_line *lines;
    t_font_id                   font_id;
    const ALLEGRO_COLOR *       pcurrent_color;
};

// FIXME: add focus parameters: update inputs on box OR widget focus
// FIXME: Do not show cursor without focus ?
struct t_widget_data_inputbox
{
    int							flags;			// enum t_widget_inputbox_flags // FIXME-ENUM
    t_widget_content_type       content_type;
    int                         insert_mode;    // Boolean
    char *                      value;
    int                         length;
    int                         length_max;
    int                         cursor_pos;
    t_font_id                   font_id;
    void                        (*callback_edit)(t_widget *inputbox);
    void                        (*callback_enter)(t_widget *inputbox);
    bool                        (*callback_completion)(t_widget *inputbox);
    bool                        (*callback_history)(t_widget *inputbox, int level);
};

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static t_widget *  widget_new(t_gui_box *box, t_widget_type type, const t_frame *frame);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

int         widgets_update_box(t_gui_box *b, int mouse_x, int mouse_y)
{
    bool    wm = TRUE;
    t_list *widgets;

    for (widgets = b->widgets; widgets != NULL; widgets = widgets->next)
    {
        t_widget *w = (t_widget *)widgets->elem;
        if (!w->enabled)
            continue;

        w->mouse_x = (mouse_x - w->frame.pos.x);
        w->mouse_y = (mouse_y - w->frame.pos.y);
        w->mouse_action = WIDGET_MOUSE_ACTION_NONE;
        w->mouse_buttons = gui.mouse.buttons;
		w->mouse_buttons_previous = gui.mouse.buttons_prev;
        if ((w->mouse_x >= 0) && (w->mouse_y >= 0) && (w->mouse_x < w->frame.size.x) && (w->mouse_y < w->frame.size.y))
        {
            w->mouse_action |= WIDGET_MOUSE_ACTION_HOVER;

            // FIXME-FOCUS
            //if (gui_mouse.pressed_on == PRESSED_ON_NOTHING || gui_mouse.pressed_on == PRESSED_ON_WIDGET)
            if (gui.mouse.focus == GUI_FOCUS_NONE || (gui.mouse.focus == GUI_FOCUS_WIDGET)) // && gui.mouse.focus_item == w;
            {
                if (w->mouse_buttons & w->mouse_buttons_mask)
                {
                    // FIXME-FOCUS
                    //gui_mouse.pressed_on = PRESSED_ON_WIDGET;
                    gui.mouse.focus = GUI_FOCUS_WIDGET;
                    gui.mouse.focus_item = w;
                    w->mouse_action |= WIDGET_MOUSE_ACTION_CLICK;
                    wm = FALSE;
                }
            }
        }
    }

    return (wm);
}

void    widgets_call_update(void)
{
    for (t_list* boxes = gui.boxes; boxes != NULL; boxes = boxes->next)
    {
        t_gui_box* b = (t_gui_box*)boxes->elem;
        for (t_list* widgets = b->widgets; widgets != NULL; widgets = widgets->next)
        {
            t_widget* w = (t_widget *)widgets->elem;
            if (w->enabled && w->update_func != NULL)
                w->update_func(w);
        }
    }
}

// Create a new widget in given box
t_widget *  widget_new(t_gui_box *box, t_widget_type type, const t_frame *frame)
{
    t_widget *w;

    // Create widget
    w = (t_widget *)malloc(sizeof (t_widget));

    // Setup base members and clear all others
    w->type                     = type;
    w->enabled                  = TRUE;     // Enabled by default
    w->box                      = box;
    w->frame                    = *frame;
    w->mouse_x                  = -1;
    w->mouse_y                  = -1;
    w->mouse_buttons            = 0;
    w->mouse_buttons_previous   = 0;
    w->mouse_buttons_mask       = 0x00;
	w->mouse_buttons_activation = 0;
    w->dirty                    = TRUE;
	w->destroy_func				= NULL;
    w->redraw_func              = NULL;
    w->update_func              = NULL;
    w->data                     = NULL;
    w->user_data                = NULL;

    // Add to box
    list_add_to_end(&box->widgets, w);

    return (w);
}

// Note: caller is responsible for removing from list
void        widget_destroy(t_widget *w)
{
	if (w->destroy_func != NULL)
		w->destroy_func(w);
    if (w->data != NULL)
        free(w->data);
    free(w);
}

void        widget_enable(t_widget *w)
{
    w->enabled = TRUE;
}

void        widget_disable(t_widget *w)
{
    w->enabled = FALSE;
}

void        widget_set_dirty(t_widget *w)
{
    w->dirty = TRUE;
}

void        widget_set_mouse_buttons_mask(t_widget *w, int mouse_buttons_mask)
{
    w->mouse_buttons_mask = mouse_buttons_mask;
}

void *      widget_get_user_data(t_widget *w)
{
    // Check parameters
    assert(w != NULL);

    // Get user data
    return (w->user_data);
}

void        widget_set_user_data(t_widget *w, void *user_data)
{
    // Check parameters
    assert(w != NULL);

    // Set user data
    w->user_data = user_data;
}

//-----------------------------------------------------------------------------

t_widget *  widget_closebox_add(t_gui_box *box, t_widget_callback callback)
{
    t_widget *w;
    t_widget_data_closebox *wd;

    t_frame frame;
    frame.pos.x = box->frame.size.x - 10;
    frame.pos.y = -15;
    frame.size.x = 7;
    frame.size.y = 6;

    w = widget_new(box, WIDGET_TYPE_CLOSEBOX, &frame);
    widget_set_mouse_buttons_mask(w, 1);

    w->redraw_func = widget_closebox_redraw;
    w->update_func = widget_closebox_update;

    w->data = malloc(sizeof (t_widget_data_closebox));
    wd = (t_widget_data_closebox*)w->data;
    wd->lighted = FALSE;
    wd->callback = callback;

    return w;
}

void        widget_closebox_redraw(t_widget *w)
{
    t_widget_data_closebox* wd = (t_widget_data_closebox*)w->data;

	ALLEGRO_COLOR color;

    // Get appropriate color
    // Note: using titlebar text active/unactive color to display the closebox star
    color = (wd->lighted) ? COLOR_SKIN_WINDOW_TITLEBAR_TEXT : COLOR_SKIN_WINDOW_TITLEBAR_TEXT_UNACTIVE;

    // Draw box-closing star using the LARGE font
    // MEKA_FONT_STR_STAR correspond to a string holding the * picture in the font
	al_set_target_bitmap(gui_buffer);
    Font_Print (F_LARGE, MEKA_FONT_STR_STAR, 
        w->box->frame.pos.x + w->frame.pos.x, w->box->frame.pos.y + w->frame.pos.y - 4, color);
}

void        widget_closebox_update(t_widget *w)
{
    t_widget_data_closebox* wd = (t_widget_data_closebox*)w->data;

    // Mouse handling
    if (!wd->lighted)
    {
        if ((w->mouse_action & WIDGET_MOUSE_ACTION_HOVER) && (w->mouse_buttons & 1))
        {
            wd->lighted = TRUE;
        }
    }
    else
    {
        if (!(w->mouse_action & WIDGET_MOUSE_ACTION_HOVER))
        {
            wd->lighted = FALSE;
        }
        else
        {
            if ((w->mouse_buttons_previous & 1) && !(w->mouse_buttons & 1))
            {
                wd->lighted = FALSE;
                {
                    wd->callback(w);
                }
            }
        }
    }

    // Keyboard
    // CTRL-F4 close window
    if (gui_box_has_focus(w->box))
    {
        if ((g_keyboard_modifiers & ALLEGRO_KEYMOD_CTRL) && Inputs_KeyPressed(ALLEGRO_KEY_F4, FALSE))
            wd->callback(w);
    }

    // Always set dirty flag
    // This is a hack to get always redrawn, because closebox is over titlebar which is always redrawn by GUI system now
    w->dirty = TRUE;
}

//-----------------------------------------------------------------------------

t_widget *  widget_button_add(t_gui_box *box, const t_frame *frame, int mouse_buttons_mask, t_widget_callback callback, t_widget_button_style style, const char *label)
{
    t_widget *w;
    t_widget_data_button *wd;

    w = widget_new(box, WIDGET_TYPE_BUTTON, frame);
    widget_set_mouse_buttons_mask(w, mouse_buttons_mask);

    w->redraw_func = widget_button_redraw;
    w->update_func = widget_button_update;

    w->data = malloc(sizeof (t_widget_data_button));
    wd = (t_widget_data_button*)w->data;
    wd->label = label ? strdup(label) : NULL;
    wd->style = style;
    wd->selected = FALSE;
    wd->callback = callback;

    return w;
}

void        widget_button_update(t_widget *w)
{
    t_widget_data_button* wd = (t_widget_data_button*)w->data;

	bool    clicked;

    // Check if we need to fire the callback
    clicked = ((w->mouse_action & WIDGET_MOUSE_ACTION_CLICK) && (w->mouse_buttons & w->mouse_buttons_mask) && !(w->mouse_buttons_previous & w->mouse_buttons_mask));

	// Update mouse buttons
	// w->mouse_buttons_previous = w->mouse_buttons;
	w->mouse_buttons_activation = w->mouse_buttons;
	w->mouse_buttons = 0;

	// Fire the callback. Note that it is done AFTER updating the mouse buttons, this
    // is to avoid recursive calls if the callback ask for a GUI update (something
    // which the file browser does when clicking on "Load Names").
    if (clicked)
	{
        wd->callback(w);
	}
}

void        widget_button_redraw(t_widget *w)
{
    t_widget_data_button* wd = (t_widget_data_button*)w->data;
    
    const ALLEGRO_COLOR bg_color = wd->selected ? COLOR_SKIN_WIDGET_GENERIC_SELECTION : COLOR_SKIN_WIDGET_GENERIC_BACKGROUND;

	al_set_target_bitmap(w->box->gfx_buffer);
    switch (wd->style)
    {
    case WIDGET_BUTTON_STYLE_INVISIBLE:
        break;
    case WIDGET_BUTTON_STYLE_SMALL:
        {
            const t_frame *frame = &w->frame;
            const t_font_id font_id = F_SMALL;
	        al_draw_filled_rectangle(frame->pos.x + 2, frame->pos.y + 2, frame->pos.x + frame->size.x - 1, frame->pos.y + frame->size.y - 1, bg_color);
            gui_rect(w->box->gfx_buffer, LOOK_THIN, frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x, frame->pos.y + frame->size.y, COLOR_SKIN_WIDGET_GENERIC_BORDER);
            Font_Print(font_id, wd->label, frame->pos.x + ((frame->size.x - Font_TextLength(font_id, wd->label)) / 2), frame->pos.y + ((frame->size.y - Font_Height(font_id)) / 2) + 1, COLOR_SKIN_WIDGET_GENERIC_TEXT);
            break;
        }
    case WIDGET_BUTTON_STYLE_BIG:
        {
            const t_frame *frame = &w->frame;
            const t_font_id font_id = F_LARGE;
	        al_draw_filled_rectangle(frame->pos.x + 2, frame->pos.y + 2, frame->pos.x + frame->size.x - 1, frame->pos.y + frame->size.y - 1, bg_color);
            gui_rect(w->box->gfx_buffer, LOOK_ROUND, frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x, frame->pos.y + frame->size.y, COLOR_SKIN_WIDGET_GENERIC_BORDER);
            Font_Print(font_id, wd->label, frame->pos.x + ((frame->size.x - Font_TextLength(font_id, wd->label)) / 2), frame->pos.y + ((frame->size.y - Font_Height(font_id)) / 2) + 1, COLOR_SKIN_WIDGET_GENERIC_TEXT);
            break;
        }
    }
}

void        widget_button_set_selected(t_widget *w, bool selected)
{
    t_widget_data_button* wd = (t_widget_data_button*)w->data;
    wd->selected = selected;
    w->dirty = TRUE;
}

//-----------------------------------------------------------------------------

t_widget *  widget_scrollbar_add(t_gui_box *box, t_widget_scrollbar_type scrollbar_type, const t_frame *frame, int *v_max, int *v_start, int *v_per_page, t_widget_callback callback)
{
    t_widget *w;
    t_widget_data_scrollbar *wd;

    w = widget_new(box, WIDGET_TYPE_SCROLLBAR, frame);
    widget_set_mouse_buttons_mask(w, 1);

    w->redraw_func = widget_scrollbar_redraw;
    w->update_func = widget_scrollbar_update;

    w->data = (t_widget_data_scrollbar *)malloc (sizeof (t_widget_data_scrollbar));
    wd = (t_widget_data_scrollbar*)w->data;
    wd->scrollbar_type = scrollbar_type;
    wd->v_max = v_max;
    wd->v_start = v_start;
    wd->v_per_page = v_per_page;
    wd->callback = callback;

    return w;
}

void        widget_scrollbar_update(t_widget *w)
{
    t_widget_data_scrollbar* wd = (t_widget_data_scrollbar*)w->data;

    int mx = w->mouse_x;
    int my = w->mouse_y;

    if (!(w->mouse_buttons & w->mouse_buttons_mask))
        return;

    // First click needs to be inside. After, we tolerate moving a little outside.
    if (w->mouse_action & WIDGET_MOUSE_ACTION_CLICK)
    {
        // OK
    }
    else
    {
        if (w->mouse_buttons_previous & w->mouse_buttons_mask)
        {
            if (wd->scrollbar_type == WIDGET_SCROLLBAR_TYPE_VERTICAL)
            {
                if (mx < 0-WIDGET_SCROLLBAR_MOUSE_TOLERANCE || mx >= w->frame.size.x + WIDGET_SCROLLBAR_MOUSE_TOLERANCE)
                    return;
                if (my < 0 || my >= w->frame.size.y)
                    return;
            }
            else if (wd->scrollbar_type == WIDGET_SCROLLBAR_TYPE_HORIZONTAL)
            {
                if (mx < 0 || mx >= w->frame.size.x)
                    return;
                if (my < 0-WIDGET_SCROLLBAR_MOUSE_TOLERANCE || my >= w->frame.size.y + WIDGET_SCROLLBAR_MOUSE_TOLERANCE)
                    return;
            }
        }
        else
        {
            return;
        }
    }

    {
        int v_start;

		// Move
        switch (wd->scrollbar_type)
        {
        case WIDGET_SCROLLBAR_TYPE_VERTICAL:
            v_start = ((my * *wd->v_max) / w->frame.size.y) - (*wd->v_per_page / 2);
            break;
        case WIDGET_SCROLLBAR_TYPE_HORIZONTAL:
            v_start = ((mx * *wd->v_max) / w->frame.size.x) - (*wd->v_per_page / 2);
            break;
        }

		// Clamp
		if (v_start > *wd->v_max - *wd->v_per_page) 
			v_start = *wd->v_max - *wd->v_per_page;
		if (v_start < 0) 
			v_start = 0;

        *wd->v_start = v_start;
    }

    // Moved, set dirty flag
	w->dirty = TRUE;

    // Callback
	if (wd->callback != NULL)
		wd->callback(w);
}

void        widget_scrollbar_redraw(t_widget *w)
{
    t_widget_data_scrollbar* wd = (t_widget_data_scrollbar*)w->data;

	// Clear bar
	al_set_target_bitmap(w->box->gfx_buffer);
	al_draw_filled_rectangle(w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x + 1, w->frame.pos.y + w->frame.size.y + 1, COLOR_SKIN_WIDGET_SCROLLBAR_BACKGROUND);

    // Draw position box
    float max = *wd->v_max;
    if (max < *wd->v_per_page) 
        max = *wd->v_per_page;

    switch (wd->scrollbar_type)
    {
    case WIDGET_SCROLLBAR_TYPE_VERTICAL:
        {
            const float pos = w->frame.pos.y + ((*wd->v_start * w->frame.size.y) / max);
            const float size = (*wd->v_per_page * w->frame.size.y) / max;
            al_draw_filled_rectangle(w->frame.pos.x, pos, w->frame.pos.x + w->frame.size.x + 1, pos + size + 1, COLOR_SKIN_WIDGET_SCROLLBAR_SCROLLER);
            break;
        }
    case WIDGET_SCROLLBAR_TYPE_HORIZONTAL:
        {
            const float pos = w->frame.pos.x + ((*wd->v_start * w->frame.size.x) / max);
            const float size = (*wd->v_per_page * w->frame.size.x) / max;
            al_draw_filled_rectangle(pos, w->frame.pos.y, pos + size + 1, w->frame.pos.y + w->frame.size.y + 1, COLOR_SKIN_WIDGET_SCROLLBAR_SCROLLER);
            break;
        }
    }

}

//-----------------------------------------------------------------------------

t_widget *  widget_checkbox_add(t_gui_box *box, const t_frame *frame, bool *pvalue, t_widget_callback callback)
{
    t_widget *w;
    t_widget_data_checkbox *wd;

    w = widget_new(box, WIDGET_TYPE_CHECKBOX, frame);
    widget_set_mouse_buttons_mask(w, 1);

    w->redraw_func = widget_checkbox_redraw;
    w->update_func = widget_checkbox_update;

    w->data = malloc(sizeof (t_widget_data_checkbox));
    wd = (t_widget_data_checkbox*)w->data;
    wd->pvalue = pvalue;
    wd->callback = callback;

    return w;
}

// FIXME: potential bug there, if wd->pvalue is NULL..
void        widget_checkbox_update(t_widget *w)
{
    t_widget_data_checkbox *wd = (t_widget_data_checkbox*)w->data;

    if ((w->mouse_buttons & w->mouse_buttons_mask) && (w->mouse_buttons_previous == 0) && (w->mouse_action & WIDGET_MOUSE_ACTION_CLICK))
    {
        *(wd->pvalue) ^= 1; // inverse flag
        if (wd->callback != NULL)
            wd->callback(w);
        w->dirty = TRUE;
    }
    //w->mouse_buttons_previous = w->mouse_buttons;
    //w->mouse_buttons = 0;
}

void        widget_checkbox_redraw(t_widget *w)
{
    t_widget_data_checkbox* wd = (t_widget_data_checkbox*)w->data;

	al_set_target_bitmap(w->box->gfx_buffer);
	al_draw_rectangle(w->frame.pos.x + 0.5f, w->frame.pos.y + 0.5f, w->frame.pos.x + w->frame.size.x + 0.5f, w->frame.pos.y + w->frame.size.y + 0.5f, COLOR_SKIN_WIDGET_GENERIC_BORDER, 1.0f);
    al_draw_filled_rectangle(w->frame.pos.x + 1, w->frame.pos.y + 1, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, COLOR_SKIN_WIDGET_GENERIC_BACKGROUND);
    if (wd->pvalue && *(wd->pvalue))
    {
        // Note: using widget generic text color to display the cross
        al_draw_line(w->frame.pos.x + 2, w->frame.pos.y + 2, w->frame.pos.x + w->frame.size.x - 1, w->frame.pos.y + w->frame.size.y - 1, COLOR_SKIN_WIDGET_GENERIC_TEXT, 0);
        al_draw_line(w->frame.pos.x + w->frame.size.x - 1, w->frame.pos.y + 2, w->frame.pos.x + 2, w->frame.pos.y + w->frame.size.y - 1, COLOR_SKIN_WIDGET_GENERIC_TEXT, 0);
    }
}

void        widget_checkbox_set_pvalue(t_widget *w, bool *pvalue)
{
    t_widget_data_checkbox* wd = (t_widget_data_checkbox*)w->data;
    wd->pvalue = pvalue;
    w->dirty = TRUE;
}

//-----------------------------------------------------------------------------

void		widget_textbox_destroy(t_widget *w);

t_widget *      widget_textbox_add(t_gui_box *box, const t_frame *frame, int lines_max, t_font_id font_id)
{
    t_widget *  w;
    t_widget_data_textbox *wd;

    // Create widget
    w = widget_new(box, WIDGET_TYPE_TEXTBOX, frame);

	w->destroy_func = widget_textbox_destroy;
    w->redraw_func = widget_textbox_redraw;
    w->update_func = NULL;

    // Setup values & parameters
    w->data = malloc(sizeof (t_widget_data_textbox));
    wd = (t_widget_data_textbox*)w->data;
    wd->lines_num       = 0;
    wd->lines_max       = lines_max;
    wd->font_id         = font_id;
    wd->pcurrent_color  = &COLOR_SKIN_WINDOW_TEXT;

    // FIXME: To compute 'columns_max', we assume that 'M' is the widest character 
    // of a font. Which may or may not be true, and should be fixed. 
    wd->columns_max = w->frame.size.x / Font_TextLength(font_id, "M");
    wd->lines       = (t_widget_data_textbox_line*)malloc(sizeof (t_widget_data_textbox_line) * lines_max);
    for (int i = 0; i < lines_max; i++)
    {
        wd->lines[i].pcolor = wd->pcurrent_color;
        wd->lines[i].text = (char*)malloc (sizeof (char) * (wd->columns_max + 1));
        wd->lines[i].text[0] = EOSTR;
    }

    // Return newly created widget
    return (w);
}

void		widget_textbox_destroy(t_widget *w)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;
    for (int i = 0; i < wd->lines_max; i++)
		free(wd->lines[i].text);
	free(wd->lines);
}

void        widget_textbox_redraw(t_widget *w)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;

    const int fh = Font_Height (wd->font_id);
    int x = w->frame.pos.x;
    int y = w->frame.pos.y + w->frame.size.y - fh;
    int i;
	ALLEGRO_BITMAP *bmp = w->box->gfx_buffer;

    /*
    {
        static cnt = 0;
        Msg (MSGT_USER_INFOLINE, "widget_textbox_redraw() %d", cnt++);
    }
    // rect (gui.box_image [w->box_n], w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, COLOR_SKIN_BORDER);
    */

	al_set_target_bitmap(bmp);
    al_draw_filled_rectangle(w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x + 1, w->frame.pos.y + w->frame.size.y + 1, COLOR_SKIN_WINDOW_BACKGROUND);
    for (i = 0; i < wd->lines_num; i++)
    {
        if (wd->lines[i].text[0] != EOSTR)
            Font_Print (wd->font_id, wd->lines[i].text, x, y, *wd->lines[i].pcolor);
        y -= fh;
    }
}

void        widget_textbox_clear(t_widget *w)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;

	for (int i = 0; i < wd->lines_num; i++)
        wd->lines[i].text[0] = EOSTR;
    wd->lines_num = 0;

    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    w->dirty = TRUE;
    w->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
}

void        widget_textbox_set_current_color(t_widget *w, const ALLEGRO_COLOR *pcurrent_color)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;
    wd->pcurrent_color = pcurrent_color;
}

static void widget_textbox_print_scroll_nowrap(t_widget *w, const char *line)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;

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
    wd->lines[0].pcolor = wd->pcurrent_color;
  
    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    w->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
    w->dirty = TRUE;
}

void        widget_textbox_print_scroll(t_widget *w, int wrap, const char *line)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;

    int     pos;
    char    buf [MSG_MAX_LEN];
    const char *    src;

    if (!wrap || line[0] == EOSTR || Font_TextLength(wd->font_id, line) <= w->frame.size.x)
    {
        widget_textbox_print_scroll_nowrap(w, line);
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
        if (Font_TextLength(wd->font_id, buf) > w->frame.size.x)
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
            widget_textbox_print_scroll_nowrap(w, buf);
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
        widget_textbox_print_scroll_nowrap(w, buf);
}

void        widget_textbox_printf_scroll(t_widget *w, int wrap, const char *format, ...)
{
    // FIXME: yet unsupported
    assert(0);
}

//-----------------------------------------------------------------------------

void	widget_inputbox_destroy(t_widget *w);

t_widget *  widget_inputbox_add(t_gui_box *box, const t_frame *frame, int length_max, t_font_id font_id, void (*callback_enter)(t_widget *))
{
    t_widget *w;
    t_widget_data_inputbox *wd;

    // Get and verify size
    int size_x = frame->size.x;
    int size_y = frame->size.y;
    assert(size_x != -1);
    if (size_y == -1)
       size_y = Font_Height(font_id) + 4;

    // Create widget
    w = widget_new(box, WIDGET_TYPE_INPUTBOX, frame);
    w->frame.size.y = size_y;
    widget_set_mouse_buttons_mask(w, 1);

	w->destroy_func = widget_inputbox_destroy;
    w->redraw_func = widget_inputbox_redraw;
    w->update_func = widget_inputbox_update;

    // Setup values & parameters
    w->data = malloc(sizeof (t_widget_data_inputbox));
    wd = (t_widget_data_inputbox*)w->data;
    wd->flags               = WIDGET_INPUTBOX_FLAGS_DEFAULT;
    wd->content_type        = WIDGET_CONTENT_TYPE_TEXT;
    wd->insert_mode         = FALSE;
    wd->length_max          = length_max;
    assert(length_max != -1); // Currently, length must be fixed
    wd->value               = (char*)malloc (sizeof (char) * (length_max + 1));
    strcpy (wd->value, "");
    wd->cursor_pos          = wd->length = 0;
    wd->font_id             = font_id;
    wd->callback_enter      = callback_enter;
    wd->callback_edit       = NULL;
    wd->callback_completion = NULL;
    wd->callback_history    = NULL;

    // Return newly created widget
    return (w);
}

void	widget_inputbox_destroy(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
	free(wd->value);
}

bool    widget_inputbox_insert_char(t_widget *w, char c)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

    if (wd->insert_mode == FALSE)
    {
        // Limit field size
        if (wd->length == wd->length_max)
            return (FALSE);

        // Shift everything that is after cursor pos
        if (wd->cursor_pos < wd->length)
        {
            int i;
            for (i = wd->length; i > wd->cursor_pos; i--)
                wd->value[i] = wd->value[i-1];
        }
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
        wd->callback_edit(w);

    // Set dirty flag
    w->dirty = TRUE;
    w->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    return (TRUE);
}

bool    widget_inputbox_insert_string(t_widget *w, const char *str)
{
    char    c;
    while ((c = *str++) != '\0')
        if (!widget_inputbox_insert_char(w, c))
            return (FALSE);
    return (TRUE);
}

bool    widget_inputbox_delete_current_char(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

    if (wd->cursor_pos == 0)
    {
        return (FALSE);
    }
    else
    {
        // Shift everything that is after cursor
        int i;
        for (i = wd->cursor_pos; i < wd->length; i++)
            wd->value[i-1] = wd->value[i];
        wd->cursor_pos--;
        wd->length--;
        wd->value[wd->length] = '\0';

        // Set dirty flag
        w->dirty = TRUE;

        return (TRUE);
    }
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
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

	const int tm_delay = 15;
    const int tm_rate = 1;
    bool edited = FALSE;

    // Check if we have focus
    // FIXME: This is completely a hack since it checks BOX focus (and not Widget focus,
    // which is not possible right now). So if there's more than one inputbox in the same
    // box, currently both will be updated.
    if (!gui_box_has_focus (w->box))
        return;

    // Msg (MSGT_DEBUG, "cascii = %c, cscan = %04x", Inputs.KeyPressed.ascii, Inputs.KeyPressed.scancode);

    // Mouse click set position
    if (!(wd->flags & WIDGET_INPUTBOX_FLAGS_NO_MOVE_CURSOR))
	{
        if ((w->mouse_buttons & w->mouse_buttons_mask) && (w->mouse_buttons_previous == 0) && (w->mouse_action & WIDGET_MOUSE_ACTION_CLICK))
        {
            //const int mx = w->mouse_x -= (2 + 3); 
            int mx = w->mouse_x - (2 + 3); // 2+3=start of text entry, see _Redraw() // FIXME
            if (mx <= 0)
            {
                widget_inputbox_set_cursor_pos (w, 0);
            }
            else
            {
                // Locate clicked-on character
                int i;
                char s[2] = { EOSTR, EOSTR };
                for (i = 0; i < wd->length; i++)
                {
                    s[0] = wd->value[i];
                    mx -= Font_TextLength(wd->font_id, s);
                    if (mx <= 0)
                        break;
                }
                widget_inputbox_set_cursor_pos (w, i);
            }
        }
	}

    // Check for printable input keys
    t_list *keypress_queue = Inputs.KeyPressedQueue;
    while (((wd->length < wd->length_max) || (wd->insert_mode == TRUE && wd->cursor_pos < wd->length)) && 
        (keypress_queue != NULL))
    {
        t_key_press* keypress = (t_key_press*)keypress_queue->elem;
        keypress_queue = keypress_queue->next;

		if (isprint(keypress->ascii))
		{
			char c = keypress->ascii;
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
				// Insert character
				widget_inputbox_insert_char(w, c);
				Inputs_KeyPressQueue_Remove(keypress);
				return;
			}
		}
    }

    // Backspace, Delete
    if (!(wd->flags & WIDGET_INPUTBOX_FLAGS_NO_DELETE))
	{
        if (Inputs_KeyPressed_Repeat (ALLEGRO_KEY_BACKSPACE, FALSE, tm_delay, tm_rate) || Inputs_KeyPressed_Repeat (ALLEGRO_KEY_DELETE, TRUE, tm_delay, tm_rate))
        {
            if (wd->cursor_pos > 0)
            {
                // Delete current character
                widget_inputbox_delete_current_char(w);
                edited = TRUE;
            }

            // HACK: This avoid resetting while backspacing in the widget
            // if (key[KEY_BACKSPACE])
            ///    Inputs_KeyEat(KEY_BACKSPACE);
        }
	}

    if (!(wd->flags & WIDGET_INPUTBOX_FLAGS_NO_MOVE_CURSOR))
    {
        // Left/Right arrows: move cursor
        if (Inputs_KeyPressed_Repeat (ALLEGRO_KEY_LEFT, FALSE, tm_delay, tm_rate))
            if (wd->cursor_pos > 0)
            {
                wd->cursor_pos--;
                w->dirty = TRUE;
            }
        if (Inputs_KeyPressed_Repeat (ALLEGRO_KEY_RIGHT, FALSE, tm_delay, tm_rate))
            if (wd->cursor_pos < wd->length)
            {
                wd->cursor_pos++;
                w->dirty = TRUE;
            }

        // Home/End: set cursor to beginning/end of input box
        if (Inputs_KeyPressed(ALLEGRO_KEY_HOME, FALSE))
        {
            wd->cursor_pos = 0;
            w->dirty = TRUE;
        }
        if (Inputs_KeyPressed(ALLEGRO_KEY_END, FALSE))
        {
            wd->cursor_pos = wd->length;
            w->dirty = TRUE;
        }
    }

	// Completion callback
	if (wd->flags & WIDGET_INPUTBOX_FLAGS_COMPLETION)
	{
		if (Inputs_KeyPressed(ALLEGRO_KEY_TAB, false))
		{
			// Completion
			assert(wd->callback_completion != NULL);
			wd->callback_completion(w);
		}
	}

	// History callback
	if (wd->flags & WIDGET_INPUTBOX_FLAGS_HISTORY)
	{
		if (Inputs_KeyPressed(ALLEGRO_KEY_UP, false))
		{
			// History Up
			assert(wd->callback_history != NULL);
			wd->callback_history(w, +1);
		}
		if (Inputs_KeyPressed(ALLEGRO_KEY_DOWN, false))
		{
			// History Down
			assert(wd->callback_history != NULL);
			wd->callback_history(w, -1);
		}
	}

    // Edit callback
    if (edited && wd->callback_edit)
        wd->callback_edit(w);

    // Enter: validate
    if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_ENTER, FALSE, 30, 3) || Inputs_KeyPressed_Repeat(ALLEGRO_KEY_PAD_ENTER, FALSE, 30, 3))
        if (wd->callback_enter)
            wd->callback_enter(w);

    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    if (w->dirty)
        w->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
}

void        widget_inputbox_redraw(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

	ALLEGRO_BITMAP *gfx_buffer = w->box->gfx_buffer;
    const bool draw_cursor = !(wd->flags & WIDGET_INPUTBOX_FLAGS_NO_CURSOR);
    const bool highlight_all = !(wd->flags & WIDGET_INPUTBOX_FLAGS_HIGHLIGHT_CURRENT_CHAR);

    // Draw border & fill text area
	al_set_target_bitmap(gfx_buffer);
    al_draw_rectangle(w->frame.pos.x + 0.5f, w->frame.pos.y + 0.5f, w->frame.pos.x + w->frame.size.x + 0.5f, w->frame.pos.y + w->frame.size.y + 0.5f, COLOR_SKIN_WIDGET_GENERIC_BORDER, 1.0f);
    al_draw_filled_rectangle(w->frame.pos.x + 1, w->frame.pos.y + 1, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, COLOR_SKIN_WIDGET_GENERIC_BACKGROUND);

    // Draw text & cursor
    {
        int x = w->frame.pos.x + 2 + 3; // Note: +3 to center font
        int y = w->frame.pos.y + (w->frame.size.y - Font_Height(wd->font_id)) / 2 + 1;
        int i;
        for (i = 0; i < wd->length + 1; i++)
        {
            if (draw_cursor && i == wd->cursor_pos)
            {
                // Draw cursor line
                int cursor_y1 = w->frame.pos.y + 2;
                int cursor_y2 = cursor_y1 + w->frame.size.y - 2*2;
                al_draw_vline(x, cursor_y1, cursor_y2, COLOR_SKIN_WIDGET_GENERIC_TEXT);
            }
            if (i < wd->length)
            {
                // Draw one character
                const ALLEGRO_COLOR color = (highlight_all || i == wd->cursor_pos) ? COLOR_SKIN_WIDGET_GENERIC_TEXT : COLOR_SKIN_WIDGET_GENERIC_TEXT_UNACTIVE;
                char ch[2];
                ch[0] = wd->value[i];
                ch[1] = '\0';
				Font_Print(wd->font_id, ch, x, y, color);
                x += Font_TextLength(wd->font_id, ch); // A bit slow
            }
        }
    }
}

const char *widget_inputbox_get_value(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    return (wd->value);
}

int         widget_inputbox_get_value_length(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    return (wd->length);
}

void        widget_inputbox_set_value(t_widget *w, const char *value)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
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
    //w->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    // Set dirty flag
    w->dirty = TRUE;
}

int         widget_inputbox_get_cursor_pos(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    return (wd->cursor_pos);
}

void        widget_inputbox_set_cursor_pos(t_widget *w, int cursor_pos)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

    if (cursor_pos < 0)
        return;
    if (cursor_pos > wd->length_max)
        return;

    // Set cursor position to given position
    // Eventually we'll add some parameters to avoid or tweak that behavior...
    wd->cursor_pos = cursor_pos;
    
    // Tells GUI that the containing box should be redrawn
    // FIXME: ideally, only the widget should be redrawn
    // w->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    // Set dirty flag
    w->dirty = TRUE;
}

void        widget_inputbox_set_callback_enter(t_widget *w, void (*callback_enter)(t_widget *))
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    wd->callback_enter = callback_enter;
}

void        widget_inputbox_set_callback_edit(t_widget *w, void (*callback_edit)(t_widget *))
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    wd->callback_edit = callback_edit;
}

void        widget_inputbox_set_flags(t_widget *w, int/*t_widget_inputbox_flags*/ flags, bool enable)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    if (enable)
        wd->flags |= flags;
    else
        wd->flags &= ~flags;
    // FIXME: changing NO_CURSOR dynamically won't refresh
}

void        widget_inputbox_set_content_type(t_widget *w, t_widget_content_type content_type)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    wd->content_type = content_type;
}

void        widget_inputbox_set_insert_mode(t_widget *w, int insert_mode)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    wd->insert_mode = insert_mode;
}

void        widget_inputbox_set_callback_completion(t_widget *w, bool (*callback_completion)(t_widget *widget))
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    assert(callback_completion != NULL);
    wd->callback_completion = callback_completion;
}

void        widget_inputbox_set_callback_history(t_widget *w, bool (*callback_history)(t_widget *widget, int level))
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    assert(callback_history != NULL);
    wd->callback_history = callback_history;
}

//-----------------------------------------------------------------------------
