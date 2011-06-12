//-----------------------------------------------------------------------------
// MEKA - textview.c
// Text Viewer Applet - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_textview.h"
#include "desktop.h"
#include "g_widget.h"
#include "inputs_t.h"
#include "tools/tfile.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_app_textviewer   TextViewer;

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static t_app_textviewer *   TextViewer_New(const char *, int, int, int, int, int, int);
static void                 TextViewer_Layout(t_app_textviewer *tv, bool setup);
static void                 TextViewer_ScrollbarCallback();
static void                 TextViewer_Update_Inputs(t_app_textviewer *tv);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void            TextViewer_Init(t_app_textviewer *tv)
{
    assert(tv == &TextViewer); // WIP multiple instanciation not supported

    // Setup members
    tv->active      = FALSE;
    tv->dirty       = TRUE;
    tv->current_file= -1;
    tv->font        = F_MIDDLE;
    tv->font_height = Font_Height(tv->font);
    tv->size_x		= TEXTVIEWER_COLUMNS;
    tv->size_y      = TEXTVIEWER_LINES;

    // Create box
    t_frame frame;
    frame.pos.x		= 290;
    frame.pos.y		= 65;
    frame.size.x	= (tv->size_x * tv->font_height) + TEXTVIEWER_PADDING - 1;
    frame.size.y	= ((tv->size_y + 2) * tv->font_height) - 1;                  // +2 for vertical padding
    tv->box = gui_box_new(&frame, "Text Viewer");
    Desktop_Register_Box("TEXTVIEW", tv->box, FALSE, &tv->active);

    // Layout
    TextViewer_Layout(tv, TRUE);

    // Setup other members
    tv->text_lines              = NULL;
    tv->text_lines_count        = 0;
    tv->text_frame.pos.x        = TEXTVIEWER_PADDING;
    tv->text_frame.pos.y        = TEXTVIEWER_PADDING;
    tv->text_frame.size.x       = tv->box->frame.size.x - (2 * TEXTVIEWER_PADDING) - TEXTVIEWER_SCROLLBAR_SIZE_X;
    tv->text_frame.size.y       = tv->box->frame.size.y - (2 * TEXTVIEWER_PADDING);
    tv->text_size_y             = 0;
    tv->text_size_per_page      = tv->size_y * tv->font_height;
    tv->scroll_position_y       = 0;
    tv->scroll_position_y_max   = 0;
    tv->scroll_velocity_y       = 0;
}

void            TextViewer_Layout(t_app_textviewer *tv, bool setup)
{
    t_frame frame;

    al_set_target_bitmap(tv->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    // Add closebox widget
    if (setup)
        widget_closebox_add(tv->box, (t_widget_callback)TextViewer_Switch_Close);

    // Add scrollbar
    frame.pos.x = tv->box->frame.size.x - TEXTVIEWER_SCROLLBAR_SIZE_X;
    frame.pos.y = 0;
    frame.size.x = TEXTVIEWER_SCROLLBAR_SIZE_X;
    frame.size.y = tv->box->frame.size.y;
    if (setup)
        tv->widget_scrollbar = widget_scrollbar_add(tv->box, WIDGET_SCROLLBAR_TYPE_VERTICAL, &frame, &tv->text_size_y, &tv->scroll_position_y, &tv->text_size_per_page, (t_widget_callback)TextViewer_ScrollbarCallback);

    // Draw separator between text and scrollbar
    al_draw_line(frame.pos.x, frame.pos.y, frame.pos.x, frame.pos.y + frame.size.y + 1, COLOR_SKIN_WINDOW_SEPARATORS, 0);
    //gui_rect (TV->ID_BoxGfx, LOOK_ROUND, x1 - 2, y1 - 2, x1 + x2 + 2, y1 + y2 + 2, COLOR_SKIN_WINDOW_SEPARATORS);
}

int             TextViewer_Open(t_app_textviewer *tv, const char *title, const char *filename)
{
    t_tfile *   tf;
    t_list *    lines;
    int         i;

    // Open and read file
    if ((tf = tfile_read(filename)) == NULL)
        return (MEKA_ERR_FILE_OPEN);

    // Free existing lines
    if (tv->text_lines != NULL)
    {
        for (i = 0; i != tv->text_lines_count; i++)
            free(tv->text_lines[i]);
        free(tv->text_lines);
        tv->text_lines = NULL;
        tv->text_lines_count = 0;
    }

    // Allocate and copy new lines
    tv->text_lines = (char**)malloc(sizeof(char *) * tf->data_lines_count);
    tv->text_lines_count = tf->data_lines_count;
    i = 0;
    for (lines = tf->data_lines; lines; lines = lines->next)
    {
        const char *line = (const char *)lines->elem;
        tv->text_lines[i] = strdup(line);
        i++;
    }

    // Free text file data
    tfile_free(tf);

    // Update members
    tv->dirty                   = TRUE;
    tv->text_size_y             = tv->text_lines_count * tv->font_height;
    tv->scroll_position_y       = 0;
    tv->scroll_position_y_max   = tv->text_size_y - tv->text_size_per_page;

    // Set new title
    gui_box_set_title(tv->box, title);

    return (MEKA_ERR_OK);
}

#define DOC_MAIN        (0)
#define DOC_COMPAT      (1)
#define DOC_MULTI       (2)
#define DOC_CHANGES     (3)
#define DOC_DEBUGGER    (4)
#define DOC_MAX         (5)

void            TextViewer_Switch_Doc_Main(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get (MSG_Doc_BoxTitle), g_env.Paths.DocumentationMain, DOC_MAIN); 
}

void            TextViewer_Switch_Doc_Compat(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get (MSG_Doc_BoxTitle), g_env.Paths.DocumentationCompat, DOC_COMPAT);
}

void            TextViewer_Switch_Doc_Multiplayer_Games(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get (MSG_Doc_BoxTitle), g_env.Paths.DocumentationMulti, DOC_MULTI); 
}

void            TextViewer_Switch_Doc_Changes(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get (MSG_Doc_BoxTitle), g_env.Paths.DocumentationChanges, DOC_CHANGES); 
}

void            TextViewer_Switch_Doc_Debugger(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get (MSG_Doc_BoxTitle), g_env.Paths.DocumentationDebugger, DOC_DEBUGGER); 
}

void            TextViewer_Switch(t_app_textviewer *tv, const char *title, const char *filename, int current_file)
{
    if (tv->current_file != current_file)
    {
        if (TextViewer_Open(tv, title, filename) != MEKA_ERR_OK)
            Msg(MSGT_USER, Msg_Get(MSG_Doc_File_Error));
        tv->active = TRUE;
        tv->current_file = current_file;
    }
    else
    {
        if (tv->active ^= 1)
            Msg(MSGT_USER, Msg_Get(MSG_Doc_Enabled));
        else
            Msg(MSGT_USER, Msg_Get(MSG_Doc_Disabled));
    }

    gui_box_show(tv->box, tv->active, TRUE);
    gui_menu_un_check_area(menus_ID.help, 0, DOC_MAX - 1);
    if (tv->active)
        gui_menu_check(menus_ID.help, current_file);
}

void            TextViewer_Switch_Close(void)
{
    t_app_textviewer *tv = &TextViewer; // Global instance
    tv->active = FALSE;
    Msg(MSGT_USER, Msg_Get(MSG_Doc_Disabled));
    gui_box_show(tv->box, tv->active, TRUE);
    gui_menu_un_check_area(menus_ID.help, 0, DOC_MAX - 1);
}

static void     TextViewer_ScrollbarCallback(void)
{
    t_app_textviewer *tv = &TextViewer; // Global instance
    tv->dirty = TRUE;
}

void            TextViewer_Update(t_app_textviewer *tv)
{
    // Skip update if not active
    if (!tv->active)
        return;

    // If skin has changed, redraw everything
    if (tv->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        TextViewer_Layout(tv, FALSE);
        tv->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
        tv->dirty = TRUE;
    }

    // Update inputs
    TextViewer_Update_Inputs(tv);

    // Update velocity
	tv->scroll_velocity_y = Clamp(tv->scroll_velocity_y, -TEXTVIEWER_SCROLL_VELOCITY_MAX, +TEXTVIEWER_SCROLL_VELOCITY_MAX); 
    if (fabsf(tv->scroll_velocity_y) > 0.01f)
    {
        tv->dirty = TRUE;
        tv->scroll_position_y += tv->scroll_velocity_y;

        // Clamp position & bounce
        if (tv->scroll_position_y < 0)
        {
            tv->scroll_position_y = 0;
            tv->scroll_velocity_y = -(tv->scroll_velocity_y / 1.5f);
        }
        if (tv->scroll_position_y > tv->scroll_position_y_max)
        {
            tv->scroll_position_y = tv->scroll_position_y_max;
            tv->scroll_velocity_y = -(tv->scroll_velocity_y / 1.5f);
        }

        // Fade off velocity
		tv->scroll_velocity_y *= 0.93f;
    }

    // Skip redraw if not dirty
    if (!tv->dirty)
        return;

    // Redraw
    widget_set_dirty(tv->widget_scrollbar);
    tv->dirty = FALSE;
    tv->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
    {
        // Uses Allegro clipping functionnality as a helper
		al_set_target_bitmap(tv->box->gfx_buffer);
		//al_set_clipping_rectangle(tv->text_frame.pos.x, tv->text_frame.pos.y, tv->text_frame.pos.x + tv->text_frame.size.x, tv->text_frame.pos.y + tv->text_frame.size.y);

        // Clear all since Allegro 5 doesnt seem to do clipping on font
		al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);
	    
		// Draw separator between text and scrollbar
		t_frame frame = tv->widget_scrollbar->frame;
		al_draw_line(frame.pos.x, frame.pos.y, frame.pos.x, frame.pos.y + frame.size.y + 1, COLOR_SKIN_WINDOW_SEPARATORS, 0);
        /*al_draw_filled_rectangle(
            tv->text_frame.pos.x, tv->text_frame.pos.y, tv->text_frame.pos.x + tv->text_frame.size.x + 1, tv->text_frame.pos.y + tv->text_frame.size.y + 1,
            COLOR_SKIN_WINDOW_BACKGROUND);*/

        // Draw lines
        const int line_y = tv->scroll_position_y / tv->font_height;
        int y = tv->text_frame.pos.y - tv->scroll_position_y % tv->font_height;
        for (int i = line_y; i < line_y + tv->size_y + 1 && i < tv->text_lines_count; i++)
        {
            Font_Print(tv->font, tv->text_lines[i], tv->text_frame.pos.x, y, COLOR_SKIN_WINDOW_TEXT);
            y += tv->font_height;
        }

        // Disable clipping
        //al_set_clipping_rectangle(0, 0, al_get_bitmap_width(tv->box->gfx_buffer), al_get_bitmap_height(tv->box->gfx_buffer));

        //rectfill (TV->BoxGfx, TV->Pad_X, 0, TV->BoxGfx->w - TV->Pad_X - TEXTVIEW_SCROLLBAR_LX - 4, TV->Pad_Y, COLOR_SKIN_WINDOW_BACKGROUND);
        //rectfill (TV->BoxGfx, TV->Pad_X, TV->BoxGfx->h - TV->Pad_Y, TV->BoxGfx->w - TV->Pad_X - TEXTVIEW_SCROLLBAR_LX - 4, TV->BoxGfx->h, COLOR_SKIN_WINDOW_BACKGROUND);
    }
}

static void     TextViewer_Update_Inputs(t_app_textviewer *tv)
{
    // Check for focus
    if (!gui_box_has_focus(tv->box)) //  && tv->vel_y == 0)
        return;

    // Update mouse wheel inputs
    if (gui.mouse.z_rel != 0)
    {
        if (gui.mouse.z_rel > 0)
            tv->scroll_velocity_y -= TEXTVIEWER_SCROLL_VELOCITY_BASE * 4.0f;
        if (gui.mouse.z_rel < 0)
            tv->scroll_velocity_y += TEXTVIEWER_SCROLL_VELOCITY_BASE * 4.0f;
        tv->dirty = TRUE;
    }

    // Update keyboard inputs
    if (Inputs_KeyPressed (ALLEGRO_KEY_HOME, FALSE))
    {
        tv->scroll_position_y = 0;
        tv->scroll_velocity_y = 0;
        tv->dirty = TRUE;
    }
    else if (Inputs_KeyPressed (ALLEGRO_KEY_END, FALSE))
    {
        tv->scroll_position_y = tv->scroll_position_y_max;
        tv->scroll_velocity_y = 0;
        tv->dirty = TRUE;
    }
    else if (Inputs_KeyPressed_Repeat (ALLEGRO_KEY_PGDN, FALSE, 25, 12))
    {
        tv->scroll_velocity_y += TEXTVIEWER_SCROLL_VELOCITY_BASE * 10.0f;
        tv->dirty = TRUE;
    }
    else if (Inputs_KeyPressed_Repeat (ALLEGRO_KEY_PGUP, FALSE, 25, 12))
    {
        tv->scroll_velocity_y -= TEXTVIEWER_SCROLL_VELOCITY_BASE * 10.0f;
        tv->dirty = TRUE;
    }
    else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_DOWN, FALSE, 2, 1))
    {
        tv->scroll_velocity_y += TEXTVIEWER_SCROLL_VELOCITY_BASE;
        tv->dirty = TRUE;
    }
    else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_UP, FALSE, 2, 1))
    {
        tv->scroll_velocity_y -= TEXTVIEWER_SCROLL_VELOCITY_BASE;
        tv->dirty = TRUE;
    }
}

//-----------------------------------------------------------------------------
