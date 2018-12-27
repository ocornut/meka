//-----------------------------------------------------------------------------
// MEKA - g_widget.c
// GUI Widgets - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "g_tools.h"
#include "g_widget.h"
#include "inputs_t.h"
#include "keyinfo.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define WIDGET_SCROLLBAR_MOUSE_TOLERANCE	(9999)		// In pixels

//-----------------------------------------------------------------------------
// Private Data
//-----------------------------------------------------------------------------

struct t_widget_data_closebox
{
    int					active;
	t_widget_callback	callback;
};

struct t_widget_data_button
{
    char *						label;
    t_font_id					font_id;
	int							highlight_on_click;
	bool						grayed_out;
	t_widget_callback			callback;
};

struct t_widget_data_scrollbar
{
    t_widget_scrollbar_type     scrollbar_type;
    const int *                 v_max;
    int *                       v_start;
    int                         v_step_per_page;
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
	t_widget_data_textbox_line* lines;
    t_font_id                   font_id;
    const ALLEGRO_COLOR *       pcurrent_color;
};

// FIXME: add focus parameters: update inputs on box OR widget focus
// FIXME: Do not show cursor without focus ?
struct t_widget_data_inputbox
{
    int							flags;			// enum t_widget_inputbox_flags // FIXME-ENUM
    t_widget_content_type       content_type;
    bool                        overwrite_mode;
    char *                      text;
	char *						tmp_buffer;
    int                         length;
    int                         length_max;
    int                         sel_begin;
	int                         sel_end;		// == cursor_pos
    t_font_id                   font_id;
	int							cursor_blink_timer;
    void                        (*callback_edit)(t_widget *inputbox);
    void                        (*callback_enter)(t_widget *inputbox);
    bool                        (*callback_completion)(t_widget *inputbox);
    bool                        (*callback_history)(t_widget *inputbox, int level);
};

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static t_widget *  widget_new(t_gui_box *box, t_widget_type type, const t_frame *frame);

void	widget_button_destroy(t_widget* w);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

bool	widgets_update_box(t_gui_box *b, int mouse_x, int mouse_y)
{
    bool widget_active = false;

    for (t_list *widgets = b->widgets; widgets != NULL; widgets = widgets->next)
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
            if (gui.mouse.focus == GUI_FOCUS_NONE || (gui.mouse.focus == GUI_FOCUS_WIDGET && gui.mouse.focus_widget == w))
            {
                if (w->mouse_buttons & w->mouse_buttons_mask)
                {
                    // FIXME-FOCUS
                    //gui_mouse.pressed_on = PRESSED_ON_WIDGET;
                    gui.mouse.focus = GUI_FOCUS_WIDGET;
					gui.mouse.focus_box = w->box;
                    gui.mouse.focus_widget = w;
                    w->mouse_action |= WIDGET_MOUSE_ACTION_CLICK;
                    widget_active = true;
                }
            }
        }
    }

    return widget_active;
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
    w->enabled                  = true;
	w->highlight				= false;
    w->box                      = box;
    w->frame                    = *frame;
    w->mouse_x                  = -1;
    w->mouse_y                  = -1;
    w->mouse_buttons            = 0;
    w->mouse_buttons_previous   = 0;
    w->mouse_buttons_mask       = 0x00;
	w->mouse_buttons_activation = 0;
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

void        widget_set_enabled(t_widget *w, bool v)
{
	if (w->enabled != v)
	{
	    w->enabled = v;
		w->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
	}
}

void        widget_set_highlight(t_widget *w, bool v)
{
	w->highlight = v;
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
	t_font_id font_id = (t_font_id)g_configuration.font_menus;

    t_frame frame;
	frame.size.x = Font_Height(font_id) - 5;
	frame.size.y = Font_Height(font_id) - 5;
    frame.pos.x = box->frame.size.x - (frame.size.x + 3);
    frame.pos.y = -(frame.size.y + 5);

    t_widget *w = widget_new(box, WIDGET_TYPE_CLOSEBOX, &frame);
    widget_set_mouse_buttons_mask(w, 1);

    w->redraw_func = widget_closebox_redraw;
    w->update_func = widget_closebox_update;

    w->data = malloc(sizeof (t_widget_data_closebox));
    t_widget_data_closebox *wd = (t_widget_data_closebox*)w->data;
    wd->active = false;
    wd->callback = callback;

    return w;
}

void        widget_closebox_redraw(t_widget *w)
{
    t_widget_data_closebox* wd = (t_widget_data_closebox*)w->data;

	w->frame.pos.x = w->box->frame.size.x - (w->frame.size.x + 1);
	w->frame.pos.y = -(w->frame.size.y + 5);

    // Get appropriate color
    // Note: using titlebar text active/inactive color to display the closebox star
    ALLEGRO_COLOR color = (w->mouse_action & WIDGET_MOUSE_ACTION_HOVER) ? COLOR_SKIN_WINDOW_TITLEBAR_TEXT : COLOR_SKIN_WINDOW_TITLEBAR_TEXT_UNACTIVE;

    // Draw box-closing star using the LARGE font
	al_set_target_bitmap(gui_buffer);
	al_draw_filled_rectangle(
		w->box->frame.pos.x + w->frame.pos.x + 2, 					w->box->frame.pos.y + w->frame.pos.y + 2, 
		w->box->frame.pos.x + w->frame.pos.x + w->frame.size.x - 2,	w->box->frame.pos.y + w->frame.pos.y + w->frame.size.y - 2, color);
	al_draw_rectangle(
		w->box->frame.pos.x + w->frame.pos.x + 2,					w->box->frame.pos.y + w->frame.pos.y + 2, 
		w->box->frame.pos.x + w->frame.pos.x + w->frame.size.x - 2,	w->box->frame.pos.y + w->frame.pos.y + w->frame.size.y - 2, COLOR_SKIN_WINDOW_BACKGROUND, 1);
}

void        widget_closebox_update(t_widget *w)
{
    t_widget_data_closebox* wd = (t_widget_data_closebox*)w->data;

    // Mouse handling
    if (!wd->active)
    {
        if ((w->mouse_action & WIDGET_MOUSE_ACTION_HOVER) && (w->mouse_buttons & 1))
            wd->active = true;
    }
    else
    {
        if (!(w->mouse_action & WIDGET_MOUSE_ACTION_HOVER))
        {
            wd->active = false;
        }
        else
        {
            if ((w->mouse_buttons_previous & 1) && !(w->mouse_buttons & 1))
            {
                wd->active = false;
                wd->callback(w);
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
}

//-----------------------------------------------------------------------------

t_widget *  widget_button_add(t_gui_box *box, const t_frame *frame, int mouse_buttons_mask, t_widget_callback callback, t_font_id font_id, const char *label, void* user_data)
{
    t_widget* w = widget_new(box, WIDGET_TYPE_BUTTON, frame);
    widget_set_mouse_buttons_mask(w, mouse_buttons_mask);

	w->destroy_func = widget_button_destroy;
    w->redraw_func = widget_button_redraw;
    w->update_func = widget_button_update;

    w->data = malloc(sizeof (t_widget_data_button));
    t_widget_data_button* wd = (t_widget_data_button*)w->data;
    wd->label = label ? strdup(label) : NULL;
    wd->font_id = font_id;
	wd->highlight_on_click = 0;
	wd->grayed_out = FALSE;
    wd->callback = callback;

	if (user_data != NULL)
		w->user_data = user_data;
	
    return w;
}

void	widget_button_destroy(t_widget* w)
{
	t_widget_data_button* wd = (t_widget_data_button*)w->data;
	if (wd->label)
		free(wd->label);
}

void	widget_button_update(t_widget *w)
{
    t_widget_data_button* wd = (t_widget_data_button*)w->data;

    // Check if we need to fire the callback
    bool clicked = false;
	if (!wd->grayed_out)
		if ((w->mouse_action & WIDGET_MOUSE_ACTION_CLICK) && (w->mouse_buttons & w->mouse_buttons_mask) && !(w->mouse_buttons_previous & w->mouse_buttons_mask))
			clicked = true;

	// Update mouse buttons
	// w->mouse_buttons_previous = w->mouse_buttons;
	w->mouse_buttons_activation = w->mouse_buttons;
	w->mouse_buttons = 0;
	if (wd->highlight_on_click > 0)
		wd->highlight_on_click--;

	// Fire the callback. Note that it is done AFTER updating the mouse buttons, this
    // is to avoid recursive calls if the callback ask for a GUI update (something
    // which the file browser does when clicking on "Load Names").
    if (clicked)
		widget_button_trigger(w);
}

void	widget_button_trigger(t_widget* w)
{
	t_widget_data_button* wd = (t_widget_data_button*)w->data;
	wd->highlight_on_click = 6;
	if (wd->callback)
		wd->callback(w);
}

void        widget_button_redraw(t_widget *w)
{
    t_widget_data_button* wd = (t_widget_data_button*)w->data;
    
	if (wd->font_id == FONTID_NONE)
		return;

	const ALLEGRO_COLOR bg_color = (w->highlight || wd->highlight_on_click>0) ? COLOR_SKIN_WIDGET_GENERIC_SELECTION : COLOR_SKIN_WIDGET_GENERIC_BACKGROUND;
	const ALLEGRO_COLOR text_color = wd->grayed_out ? COLOR_SKIN_WIDGET_GENERIC_TEXT_UNACTIVE : COLOR_SKIN_WIDGET_GENERIC_TEXT;
	const t_frame *frame = &w->frame;

	al_set_target_bitmap(w->box->gfx_buffer);
	al_draw_filled_rectangle(frame->pos.x + 2, frame->pos.y + 2, frame->pos.x + frame->size.x - 1, frame->pos.y + frame->size.y - 1, bg_color);
	if (Font_Height(wd->font_id) > 11)
		gui_rect(LOOK_ROUND, frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x, frame->pos.y + frame->size.y, COLOR_SKIN_WIDGET_GENERIC_BORDER);
	else
		gui_rect(LOOK_THIN, frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x, frame->pos.y + frame->size.y, COLOR_SKIN_WIDGET_GENERIC_BORDER);
	if (wd->label)
		Font_Print(wd->font_id, wd->label, frame->pos.x + ((frame->size.x - Font_TextWidth(wd->font_id, wd->label)) / 2), frame->pos.y + ((frame->size.y - Font_Height(wd->font_id)) / 2) + 1, text_color);
}

void    widget_button_set_grayed_out(t_widget *w, bool grayed_out)
{
	t_widget_data_button* wd = (t_widget_data_button*)w->data;
	if (wd->grayed_out != grayed_out)
	{
		wd->grayed_out = grayed_out;
	}
}

void	widget_button_set_label(t_widget *w, const char* label)
{
	t_widget_data_button* wd = (t_widget_data_button*)w->data;
	if (wd->label)
		free(wd->label);
	wd->label = label ? strdup(label) : NULL;
}

//-----------------------------------------------------------------------------

t_widget *  widget_scrollbar_add(t_gui_box *box, t_widget_scrollbar_type scrollbar_type, const t_frame *frame, const int *v_max, int *v_start, int v_step_per_page, t_widget_callback callback)
{
    t_widget* w = widget_new(box, WIDGET_TYPE_SCROLLBAR, frame);
    widget_set_mouse_buttons_mask(w, 1);

    w->redraw_func = widget_scrollbar_redraw;
    w->update_func = widget_scrollbar_update;

    w->data = (t_widget_data_scrollbar *)malloc (sizeof (t_widget_data_scrollbar));
    t_widget_data_scrollbar* wd = (t_widget_data_scrollbar*)w->data;
    wd->scrollbar_type = scrollbar_type;
    wd->v_max = v_max;
    wd->v_start = v_start;
    wd->v_step_per_page = v_step_per_page;
    wd->callback = callback;

    return w;
}

void    widget_scrollbar_update(t_widget *w)
{
    t_widget_data_scrollbar* wd = (t_widget_data_scrollbar*)w->data;

    const int mx = w->mouse_x;
    const int my = w->mouse_y;

    if (!(w->mouse_buttons & w->mouse_buttons_mask))
        return;

    // First click needs to be inside. After, we tolerate moving a little outside.
	if (gui.mouse.focus_widget != w)
		return;

	if ((w->mouse_buttons_previous & w->mouse_buttons_mask) == 0)
		return;

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

	// Move
	int v_start;
    switch (wd->scrollbar_type)
    {
    case WIDGET_SCROLLBAR_TYPE_VERTICAL:
        v_start = ((my * *wd->v_max) / w->frame.size.y) - (wd->v_step_per_page / 2);
        break;
    case WIDGET_SCROLLBAR_TYPE_HORIZONTAL:
        v_start = ((mx * *wd->v_max) / w->frame.size.x) - (wd->v_step_per_page / 2);
        break;
    }

	// Clamp
	if (v_start > *wd->v_max - wd->v_step_per_page) 
		v_start = *wd->v_max - wd->v_step_per_page;
	if (v_start < 0) 
		v_start = 0;
    *wd->v_start = v_start;

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
	al_draw_rectangle(w->frame.pos.x - 0.5f, w->frame.pos.y - 0.5f, w->frame.pos.x + w->frame.size.x + 1.5f, w->frame.pos.y + w->frame.size.y + 1.5f, COLOR_SKIN_WINDOW_SEPARATORS, 1.0f);

    // Draw position box
    float max = *wd->v_max;
    if (max < wd->v_step_per_page) 
        max = wd->v_step_per_page;

    switch (wd->scrollbar_type)
    {
    case WIDGET_SCROLLBAR_TYPE_VERTICAL:
        {
            const float pos = w->frame.pos.y + ((*wd->v_start * w->frame.size.y) / max);
            const float size = (wd->v_step_per_page * w->frame.size.y) / max;
            al_draw_filled_rectangle(w->frame.pos.x, pos, w->frame.pos.x + w->frame.size.x + 1, pos + size + 1, COLOR_SKIN_WIDGET_SCROLLBAR_SCROLLER);
            break;
        }
    case WIDGET_SCROLLBAR_TYPE_HORIZONTAL:
        {
            const float pos = w->frame.pos.x + ((*wd->v_start * w->frame.size.x) / max);
            const float size = (wd->v_step_per_page * w->frame.size.x) / max;
            al_draw_filled_rectangle(pos, w->frame.pos.y, pos + size + 1, w->frame.pos.y + w->frame.size.y + 1, COLOR_SKIN_WIDGET_SCROLLBAR_SCROLLER);
            break;
        }
    }
}

void	widget_scrollbar_set_page_step(t_widget *w, int page_step)
{
	t_widget_data_scrollbar* wd = (t_widget_data_scrollbar*)w->data;
	wd->v_step_per_page = page_step;
}

//-----------------------------------------------------------------------------

t_widget *  widget_checkbox_add(t_gui_box *box, const t_frame *frame, bool *pvalue, t_widget_callback callback)
{
    t_widget* w = widget_new(box, WIDGET_TYPE_CHECKBOX, frame);
    widget_set_mouse_buttons_mask(w, 1);

    w->redraw_func = widget_checkbox_redraw;
    w->update_func = widget_checkbox_update;

    w->data = malloc(sizeof (t_widget_data_checkbox));
    t_widget_data_checkbox* wd = (t_widget_data_checkbox*)w->data;
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
}

//-----------------------------------------------------------------------------

void		widget_textbox_destroy(t_widget *w);

t_widget *  widget_textbox_add(t_gui_box *box, const t_frame *frame, t_font_id font_id)
{
    // Create widget
    t_widget* w = widget_new(box, WIDGET_TYPE_TEXTBOX, frame);
	w->destroy_func = widget_textbox_destroy;
    w->redraw_func = widget_textbox_redraw;
    w->update_func = NULL;

    // Setup values & parameters
    w->data = malloc(sizeof (t_widget_data_textbox));
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;
    wd->lines_num       = 0;
    wd->lines_max       = 100;
    wd->font_id         = font_id;
    wd->pcurrent_color  = &COLOR_SKIN_WINDOW_TEXT;

    wd->lines = (t_widget_data_textbox_line*)malloc(sizeof(t_widget_data_textbox_line) * wd->lines_max);
    for (int i = 0; i < wd->lines_max; i++)
    {
        wd->lines[i].pcolor = wd->pcurrent_color;
        wd->lines[i].text = NULL;
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

    const int fh = Font_Height(wd->font_id);
    const int x = w->frame.pos.x;
    int y = w->frame.pos.y + w->frame.size.y;

	al_set_target_bitmap(w->box->gfx_buffer);
    al_draw_filled_rectangle(w->frame.pos.x, w->frame.pos.y, w->frame.pos.x + w->frame.size.x + 1, w->frame.pos.y + w->frame.size.y + 1, COLOR_SKIN_WINDOW_BACKGROUND);
    for (int i = 0; i < wd->lines_num; i++)
    {
		if (y < w->frame.pos.y - fh)
			break;
		
		t_widget_data_textbox_line* entry = &wd->lines[i];

		const int wrap_width = w->frame.size.x;
		const int h = Font_TextHeight(wd->font_id, entry->text, wrap_width);
		y -= h;

        if (wd->lines[i].text[0] == EOSTR)
			continue;
        Font_Print(wd->font_id, entry->text, x, y, *entry->pcolor, wrap_width);
    }
}

void        widget_textbox_clear(t_widget *w)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;

	for (int i = 0; i < wd->lines_num; i++)
        wd->lines[i].text[0] = EOSTR;
    wd->lines_num = 0;
}

void        widget_textbox_set_current_color(t_widget *w, const ALLEGRO_COLOR *pcurrent_color)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;
    wd->pcurrent_color = pcurrent_color;
}

void		widget_textbox_print_scroll(t_widget *w, bool wrap, const char *line)
{
    t_widget_data_textbox* wd = (t_widget_data_textbox*)w->data;

    // Shift all lines by one position
    // FIXME-OPT: circular queue
    if (wd->lines_num > 0)
    {
		if (wd->lines[wd->lines_max - 1].text)
		{
			free(wd->lines[wd->lines_max - 1].text);
			wd->lines[wd->lines_max - 1].text = NULL;
		}

        for (int n = wd->lines_max - 1; n > 0; n--)
            wd->lines[n] = wd->lines[n - 1];
        wd->lines[0].text = NULL;
    }

    // Increment number of lines counter
    if (wd->lines_num < wd->lines_max)
        wd->lines_num++;

    // Copy new line
	if (wd->lines_max > 0)
	{
	    wd->lines[0].text = strdup(line);
		wd->lines[0].pcolor = wd->pcurrent_color;
	}
}

/*
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
        // FIXME-OPT: Dumb
		const int char_w = Font_TextLength(wd->font_id, buf);
        if (char_w > w->frame.size.x)
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
*/

void        widget_textbox_printf_scroll(t_widget *w, bool wrap, const char* fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, countof(buf), fmt, args);
	va_end(args);
	widget_textbox_print_scroll(w, wrap, buf);
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
    wd->overwrite_mode      = false;
    wd->length_max          = length_max;
    assert(length_max != -1); // Currently, length must be fixed
    wd->text				= new char[length_max + 1];
    strcpy(wd->text, "");
	wd->tmp_buffer			= new char[length_max + 1];
	wd->length				= 0;
    wd->sel_begin = wd->sel_end = 0;
    wd->font_id             = font_id;
	wd->cursor_blink_timer	= 0;
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
	delete[] wd->text;
	delete[] wd->tmp_buffer;
}

void	widget_inputbox_delete_selection(t_widget* w)
{
	t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

	// Delete selection
	if (widget_inputbox_has_selection(w))
	{
		const int sel_len = MAX(wd->sel_begin, wd->sel_end) - MIN(wd->sel_begin, wd->sel_end);
		for (int i = MAX(wd->sel_begin, wd->sel_end); i < wd->length; i++)
			wd->text[i - sel_len] = wd->text[i];
		wd->sel_begin = wd->sel_end = MIN(wd->sel_begin, wd->sel_end);
		wd->length -= sel_len;
	}
}

bool    widget_inputbox_insert_chars(t_widget* w, const char* str)
{
	t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

	widget_inputbox_delete_selection(w);
	assert(!widget_inputbox_has_selection(w));

	// How much can we fit?
	int str_len = strlen(str);
	if (wd->overwrite_mode)
	{
		str_len = MIN(str_len, wd->length_max - wd->sel_end);
		assert(wd->sel_end + str_len <= wd->length_max);
	}
	else
	{
		str_len = MIN(str_len, wd->length_max - wd->length);
		assert(wd->length + str_len <= wd->length_max);
	}

	if (str_len == 0)
		return false;
	//const char* str_end = str + str_len;

	if (!wd->overwrite_mode)
	{
		// Shift everything that is after cursor pos
		for (int i = wd->length; i > wd->sel_end; i--)
			wd->text[i] = wd->text[i-str_len];
	}

	// Insert at cursor pos
	for (int i = 0; i < str_len; i++)
		wd->text[wd->sel_end + i] = str[i];
	wd->sel_begin = wd->sel_end = wd->sel_end + str_len;
	wd->length += str_len;
	wd->text[wd->length] = '\0';

	// Edit callback
	if (wd->callback_edit)
		wd->callback_edit(w);

	// Set dirty flag
	wd->cursor_blink_timer = 0;

    return true;
}

void    widget_inputbox_delete_current_char(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

	wd->cursor_blink_timer = 0;

	assert(!widget_inputbox_has_selection(w));

	// Shift everything that is after cursor
	for (int i = wd->sel_end; i < wd->length; i++)
		wd->text[i-1] = wd->text[i];
	wd->sel_begin = wd->sel_end = wd->sel_end - 1;
	wd->length--;
	wd->text[wd->length] = '\0';
}

static char	widget_inputbox_translate_char(t_widget* w, char c)
{
	t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

	if (!isprint(c))
		return 0;

	if (wd->content_type == WIDGET_CONTENT_TYPE_DECIMAL)
	{
		if (!(c >= '0' && c <= '9'))
			return 0;
	}
	else if (wd->content_type == WIDGET_CONTENT_TYPE_HEXADECIMAL)
	{
		if (c >= 'a' && c <= 'f')
			c += 'A' - 'a';
		if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
			return 0;
	}
	else if (wd->content_type == WIDGET_CONTENT_TYPE_DEC_HEX_BIN)
	{
		if (c >= 'a' && c <= 'f')
			c += 'A' - 'a';
		if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c == '$' || c == '%' || c == '#')))
			return 0;
	}
	return c;
}

//-----------------------------------------------------------------------------
// widget_inputbox_update (t_widget *)
// Update input box based on user keyboard inputs
//-----------------------------------------------------------------------------
void        widget_inputbox_update(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

	const int tm_delay = 15;
    const int tm_rate = 1;

	const bool is_ctrl_pressed = (g_keyboard_modifiers & ALLEGRO_KEYMOD_CTRL) != 0;
	const bool is_shift_pressed = (g_keyboard_modifiers & ALLEGRO_KEYMOD_SHIFT) != 0;

    bool edited = false;

    // Check if we have focus
    // FIXME: This is completely a hack since it checks BOX focus (and not Widget focus,
    // which is not possible right now). So if there's more than one inputbox in the same
    // box, currently both will be updated.
    if (!gui_box_has_focus(w->box))
	{
		wd->cursor_blink_timer = 0;
        return;
	}
	wd->cursor_blink_timer = (wd->cursor_blink_timer + 1) % 70;

    // Msg(MSGT_DEBUG, "cascii = %c, cscan = %04x", Inputs.KeyPressed.ascii, Inputs.KeyPressed.scancode);

    // Mouse click set position
    if (!(wd->flags & WIDGET_INPUTBOX_FLAGS_NO_MOVE_CURSOR))
	{
        if ((w->mouse_buttons & w->mouse_buttons_mask) && (w->mouse_buttons_previous == 0) && (w->mouse_action & WIDGET_MOUSE_ACTION_CLICK))
        {
            //const int mx = w->mouse_x -= (2 + 3); 
            int mx = w->mouse_x - (2 + 3); // 2+3=start of text entry, see _Redraw() // FIXME
            if (mx <= 0)
            {
                widget_inputbox_set_cursor_pos(w, 0);
            }
            else
            {
                // Locate clicked-on character
                int i;
                char s[2] = { EOSTR, EOSTR };
                for (i = 0; i < wd->length; i++)
                {
                    s[0] = wd->text[i];
                    mx -= Font_TextWidth(wd->font_id, s);
                    if (mx <= 0)
                        break;
                }
                widget_inputbox_set_cursor_pos(w, i);
            }
        }
	}

    // Check for printable input keys
    t_list *keypress_queue = Inputs.KeyPressedQueue;
    while (keypress_queue != NULL)
    {
		t_key_press* keypress = (t_key_press*)keypress_queue->elem;
        keypress_queue = keypress_queue->next;

		if (isprint(keypress->ascii))
		{
			const char c = widget_inputbox_translate_char(w, keypress->ascii);
			if (c != 0)
			{
				// Insert character
				char sc[2];
				sc[0] = c;
				sc[1] = '\0';
				widget_inputbox_insert_chars(w, sc);
				Inputs_KeyPressQueue_Remove(keypress);
				return;
			}
		}
    }

	// Clipboard Copy
	if (is_ctrl_pressed && Inputs_KeyPressed(ALLEGRO_KEY_C, true))
	{
		if (wd->flags & WIDGET_INPUTBOX_FLAGS_NO_SELECTION)
		{
			OSD_ClipboardSetText(wd->text, wd->text + wd->length);
		}
		else if (wd->sel_begin != wd->sel_end)
		{
			OSD_ClipboardSetText(wd->text + MIN(wd->sel_begin, wd->sel_end), wd->text + MAX(wd->sel_begin, wd->sel_end));
		}
	}

	// Clipboard Paste (slower repeat rate)
	if (is_ctrl_pressed && Inputs_KeyPressed_Repeat(ALLEGRO_KEY_V, false, tm_delay, tm_rate*3))
	{
		if (char* s = OSD_ClipboardGetText())
		{
			bool allowed = true;
			for (int i = 0; s[i]; i++)
			{
				if (widget_inputbox_translate_char(w, s[i]) == 0)
				{
					allowed = false;
					break;
				}
			}
			if (allowed)
				widget_inputbox_insert_chars(w, s);
			free(s);
		}
	}

    if (!(wd->flags & WIDGET_INPUTBOX_FLAGS_NO_DELETE))
	{
		// Clipboard Cut
		if (is_ctrl_pressed && Inputs_KeyPressed(ALLEGRO_KEY_X, true))
		{
			if (wd->sel_begin != wd->sel_end)
			{
				OSD_ClipboardSetText(wd->text + MIN(wd->sel_begin, wd->sel_end), wd->text + MAX(wd->sel_begin, wd->sel_end));
				widget_inputbox_delete_selection(w);
			}
		}

		// Backspace
		if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_BACKSPACE, FALSE, tm_delay, tm_rate))
        {
			if (widget_inputbox_has_selection(w))
			{
				widget_inputbox_delete_selection(w);
			}
			else if (wd->sel_end > 0)
			{
				// Delete previous character
				widget_inputbox_delete_current_char(w);
				edited = TRUE;
			}

            // HACK: This avoid resetting while backspacing in the widget
            if (Inputs_KeyPressed(ALLEGRO_KEY_BACKSPACE, FALSE))
                Inputs_KeyEat(ALLEGRO_KEY_BACKSPACE);
        }

		// Delete
		if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_DELETE, TRUE, tm_delay, tm_rate))
		{
			if (widget_inputbox_has_selection(w))
			{
				widget_inputbox_delete_selection(w);
			}
			else if (wd->sel_end < wd->length)
			{
				// Delete next character
				wd->sel_begin++;
				wd->sel_end++;
				widget_inputbox_delete_current_char(w);
				edited = TRUE;
			}
		}
	}

    if (!(wd->flags & WIDGET_INPUTBOX_FLAGS_NO_MOVE_CURSOR))
    {
		// Left/Right arrows: move cursor
		const char* word_delimiters = " \t\r\n,:;";
		if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_LEFT, FALSE, tm_delay, tm_rate))
		{
			int offset = -1;
			if (is_ctrl_pressed)
			{
				int i = wd->sel_end - 1;
				while (i >= 0 && strchr(word_delimiters, wd->text[i]))
					i--;
				while (i >= 0 && !strchr(word_delimiters, wd->text[i]))
					i--;
				offset = i - wd->sel_end + 1;
			}
			widget_inputbox_set_selection_end(w, wd->sel_end + offset);
			if (!is_shift_pressed)
				wd->sel_begin = wd->sel_end;
		}
		if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_RIGHT, FALSE, tm_delay, tm_rate))
		{
			int offset = +1;
			if (is_ctrl_pressed)
			{
				int i = wd->sel_end;
				while (i < wd->length && !strchr(word_delimiters, wd->text[i]))
					i++;
				while (i < wd->length && strchr(word_delimiters, wd->text[i]))
					i++;
				offset = i - wd->sel_end;
			}
			widget_inputbox_set_selection_end(w, wd->sel_end + offset);
			if (!is_shift_pressed)
				wd->sel_begin = wd->sel_end;
		}

		// Home/End: set cursor to beginning/end of input box
		if (Inputs_KeyPressed(ALLEGRO_KEY_HOME, FALSE))
		{
			widget_inputbox_set_selection_end(w, 0);
			if (!is_shift_pressed)
				wd->sel_begin = wd->sel_end;
		}
		if (Inputs_KeyPressed(ALLEGRO_KEY_END, FALSE))
		{
			widget_inputbox_set_selection_end(w, wd->length);
			if (!is_shift_pressed)
				wd->sel_begin = wd->sel_end;
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
}

void        widget_inputbox_redraw(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;

	ALLEGRO_BITMAP *gfx_buffer = w->box->gfx_buffer;

    const bool draw_cursor = !(wd->flags & WIDGET_INPUTBOX_FLAGS_NO_CURSOR) && (wd->cursor_blink_timer < 50);
    const bool highlight_all = !(wd->flags & WIDGET_INPUTBOX_FLAGS_HIGHLIGHT_CURRENT_CHAR);

    // Draw border & fill text area
	al_set_target_bitmap(gfx_buffer);
    al_draw_rectangle(w->frame.pos.x + 0.5f, w->frame.pos.y + 0.5f, w->frame.pos.x + w->frame.size.x + 0.5f, w->frame.pos.y + w->frame.size.y + 0.5f, COLOR_SKIN_WIDGET_GENERIC_BORDER, 1.0f);

	const ALLEGRO_COLOR bg_color = (w->highlight) ? COLOR_SKIN_WIDGET_GENERIC_SELECTION : COLOR_SKIN_WIDGET_GENERIC_BACKGROUND;
    al_draw_filled_rectangle(w->frame.pos.x + 1, w->frame.pos.y + 1, w->frame.pos.x + w->frame.size.x, w->frame.pos.y + w->frame.size.y, bg_color);

	int x = w->frame.pos.x + 2 + 3; // Note: +3 to center font
	const int y = w->frame.pos.y + (w->frame.size.y - Font_Height(wd->font_id)) / 2 + 1;

	// Selection
	if (wd->sel_begin != wd->sel_end)
	{
		const int sel_min = MIN(wd->sel_begin, wd->sel_end);
		const int sel_max = MAX(wd->sel_begin, wd->sel_end);

		sprintf(wd->tmp_buffer, "%.*s", sel_min, wd->text);
		const int sel_min_x = Font_TextWidth(wd->font_id, wd->tmp_buffer);

		sprintf(wd->tmp_buffer, "%.*s", sel_max - sel_min, wd->text + sel_min);
		const int sel_max_x = sel_min_x + Font_TextWidth(wd->font_id, wd->tmp_buffer);

		const ALLEGRO_COLOR inv_color = COLOR_SKIN_WINDOW_BACKGROUND;
		al_draw_filled_rectangle(x + sel_min_x, w->frame.pos.y + 1+1, x + sel_max_x, w->frame.pos.y + w->frame.size.y-1, inv_color);
	}

	// Draw text & cursor
	for (int i = 0; i < wd->length + 1; i++)
	{
		if (draw_cursor && (i == wd->sel_end))
		{
			// Draw cursor line
			int cursor_y1 = w->frame.pos.y + 2;
			int cursor_y2 = cursor_y1 + w->frame.size.y - 2*2;
			al_draw_vline(x, cursor_y1, cursor_y2, COLOR_SKIN_WIDGET_GENERIC_TEXT);
		}
		if (i < wd->length)
		{
			// Draw one character
			ALLEGRO_COLOR color = (highlight_all || i == wd->sel_end) ? COLOR_SKIN_WIDGET_GENERIC_TEXT : COLOR_SKIN_WIDGET_GENERIC_TEXT_UNACTIVE;
			char ch[2];
			ch[0] = wd->text[i];
			ch[1] = '\0';
			Font_Print(wd->font_id, ch, x, y, color);
			x += Font_TextWidth(wd->font_id, ch); // A bit slow
		}
	}
}

const char *widget_inputbox_get_value(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    return (wd->text);
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
        strcpy(wd->text, value);
        wd->length = len;
    }
    else
    {
        strncpy (wd->text, value, wd->length_max);
        wd->text[wd->length_max] = '\0';
        wd->length = wd->length_max;
    }

    // Set cursor to end of text
    wd->sel_begin = wd->sel_end = wd->length;
}

int         widget_inputbox_get_cursor_pos(t_widget *w)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    return (wd->sel_end);
}

void        widget_inputbox_set_cursor_pos(t_widget *w, int cursor_pos)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
	wd->cursor_blink_timer = 0;
    wd->sel_begin = wd->sel_end = Clamp<int>(cursor_pos, 0, wd->length);
}

bool		widget_inputbox_has_selection(t_widget *w)
{
	t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
	return wd->sel_begin != wd->sel_end;
}

void        widget_inputbox_set_selection(t_widget *w, int sel_begin, int sel_end)
{
	t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
	wd->cursor_blink_timer = 0;
	wd->sel_begin = Clamp<int>(sel_begin, 0, wd->length);
	wd->sel_end = Clamp<int>(sel_end, 0, wd->length);
}

void        widget_inputbox_set_selection_end(t_widget *w, int sel_end)
{
	t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
	wd->cursor_blink_timer = 0;
	wd->sel_end = Clamp<int>(sel_end, 0, wd->length);
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

void        widget_inputbox_set_overwrite_mode(t_widget *w, bool overwrite_mode)
{
    t_widget_data_inputbox* wd = (t_widget_data_inputbox*)w->data;
    wd->overwrite_mode = overwrite_mode;
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
