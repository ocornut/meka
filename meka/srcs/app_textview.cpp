//-----------------------------------------------------------------------------
// MEKA - textview.c
// Text Viewer Applet - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_textview.h"
#include "desktop.h"
#include "g_widget.h"
#include "inputs_t.h"
#include "libparse.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_app_textviewer   TextViewer;

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static void		TextViewer_Layout(t_app_textviewer* app, bool setup);
static void		TextViewer_ScrollbarCallback();
static void		TextViewer_Update_Inputs(t_app_textviewer* app);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void	TextViewer_Init(t_app_textviewer* app)
{
    assert(app == &TextViewer); // WIP multiple instances not supported

    // Setup members
    app->active      = FALSE;
    app->dirty       = TRUE;
    app->current_file= -1;
    app->font        = (t_font_id)g_configuration.font_documentation;
    app->font_height = Font_Height(app->font);

    // Create box
    t_frame frame;
    frame.pos.x		= 290;
    frame.pos.y		= 65;
    frame.size.x	= 550;
    frame.size.y	= 620;
    app->box = gui_box_new(&frame, "Text Viewer");
	app->box->flags |= GUI_BOX_FLAGS_ALLOW_RESIZE;

	// Register to desktop
	Desktop_Register_Box("DOCUMENTATION", app->box, FALSE, &app->active);

	// Layout
	TextViewer_Layout(app, true);

	// Setup other members
	app->text_lines              = NULL;
	app->text_lines_count        = 0;
	app->text_size_y             = 0;
	app->scroll_position_y       = 0;
	app->scroll_position_y_max   = 0;
	app->scroll_velocity_y       = 0;
}

void	TextViewer_Close(t_app_textviewer* app)
{
    gui_box_delete(app->box);
    app->box = NULL;
    for (int i = 0; i != app->text_lines_count; i++)
        free(app->text_lines[i]);
    free(app->text_lines);
    app->text_lines = NULL;
    app->text_lines_count = 0;
}

void	TextViewer_Layout(t_app_textviewer* app, bool setup)
{
	app->text_frame.pos.x        = TEXTVIEWER_PADDING;
	app->text_frame.pos.y        = TEXTVIEWER_PADDING;
	app->text_frame.size.x       = app->box->frame.size.x - (2 * TEXTVIEWER_PADDING) - TEXTVIEWER_SCROLLBAR_SIZE_X;
	app->text_frame.size.y       = app->box->frame.size.y - (2 * TEXTVIEWER_PADDING);
	app->text_size_per_page      = app->box->frame.size.y;

    t_frame frame;

    al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    // Add closebox widget
    if (setup)
        widget_closebox_add(app->box, (t_widget_callback)TextViewer_Switch_Close);

    // Add scrollbar
    frame.pos.x = app->box->frame.size.x - TEXTVIEWER_SCROLLBAR_SIZE_X;
    frame.pos.y = 0;
    frame.size.x = TEXTVIEWER_SCROLLBAR_SIZE_X;
    frame.size.y = app->box->frame.size.y - 16;
    if (setup)
	{
        app->widget_scrollbar = widget_scrollbar_add(app->box, WIDGET_SCROLLBAR_TYPE_VERTICAL, &frame, &app->text_size_y, &app->scroll_position_y, app->text_size_per_page, (t_widget_callback)TextViewer_ScrollbarCallback);
	}
	else
	{
		app->widget_scrollbar->frame = frame;
		widget_scrollbar_set_page_step(app->widget_scrollbar, app->text_size_per_page);
	}
}

int		TextViewer_Open(t_app_textviewer* app, const char* title, const char* filename)
{
    // Open and read file
	t_tfile *   tf;
    if ((tf = tfile_read(filename)) == NULL)
        return (MEKA_ERR_FILE_OPEN);

    // Free existing lines
    if (app->text_lines != NULL)
    {
        for (int i = 0; i != app->text_lines_count; i++)
            free(app->text_lines[i]);
        free(app->text_lines);
        app->text_lines = NULL;
        app->text_lines_count = 0;
    }

    // Allocate and copy new lines
    app->text_lines = (char**)malloc(sizeof(char *) * tf->data_lines_count);
    app->text_lines_count = tf->data_lines_count;
    int i = 0;
    for (t_list* lines = tf->data_lines; lines; lines = lines->next)
    {
        const char *line = (const char *)lines->elem;
        app->text_lines[i] = strdup(line);
        i++;
    }

    // Free text file data
    tfile_free(tf);

    // Update members
    app->dirty                   = TRUE;
    app->text_size_y             = app->text_lines_count * app->font_height;
    app->scroll_position_y       = 0;
    app->scroll_position_y_max   = app->text_size_y - app->text_size_per_page;

    // Set new title
    gui_box_set_title(app->box, title);

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
    TextViewer_Switch(&TextViewer, Msg_Get(MSG_Doc_BoxTitle), g_env.Paths.DocumentationMain, DOC_MAIN); 
}

void            TextViewer_Switch_Doc_Compat(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get(MSG_Doc_BoxTitle), g_env.Paths.DocumentationCompat, DOC_COMPAT);
}

void            TextViewer_Switch_Doc_Multiplayer_Games(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get(MSG_Doc_BoxTitle), g_env.Paths.DocumentationMulti, DOC_MULTI); 
}

void            TextViewer_Switch_Doc_Changes(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get(MSG_Doc_BoxTitle), g_env.Paths.DocumentationChanges, DOC_CHANGES); 
}

void            TextViewer_Switch_Doc_Debugger(void)
{ 
    TextViewer_Switch(&TextViewer, Msg_Get(MSG_Doc_BoxTitle), g_env.Paths.DocumentationDebugger, DOC_DEBUGGER); 
}

void	TextViewer_Switch(t_app_textviewer *tv, const char *title, const char *filename, int current_file)
{
    if (tv->current_file != current_file)
    {
        if (TextViewer_Open(tv, title, filename) != MEKA_ERR_OK)
            Msg(MSGT_USER, "%s", Msg_Get(MSG_Doc_File_Error));
        tv->active = TRUE;
        tv->current_file = current_file;
    }
    else
    {
        if (tv->active ^= 1)
            Msg(MSGT_USER, "%s", Msg_Get(MSG_Doc_Enabled));
        else
            Msg(MSGT_USER, "%s", Msg_Get(MSG_Doc_Disabled));
    }

    gui_box_show(tv->box, tv->active, TRUE);
    gui_menu_uncheck_range(menus_ID.help, 0, DOC_MAX - 1);
    if (tv->active)
        gui_menu_check(menus_ID.help, current_file);
}

void	TextViewer_Switch_Close(void)
{
    t_app_textviewer *tv = &TextViewer; // Global instance
    tv->active = FALSE;
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Doc_Disabled));
    gui_box_show(tv->box, tv->active, TRUE);
    gui_menu_uncheck_range(menus_ID.help, 0, DOC_MAX - 1);
}

static void     TextViewer_ScrollbarCallback(void)
{
    t_app_textviewer *tv = &TextViewer; // Global instance
    tv->dirty = TRUE;
}

void	TextViewer_Update(t_app_textviewer *tv)
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
    tv->dirty = FALSE;
    {
        // Uses Allegro clipping functionality as a helper
		al_set_target_bitmap(tv->box->gfx_buffer);
		//al_set_clipping_rectangle(tv->text_frame.pos.x, tv->text_frame.pos.y, tv->text_frame.pos.x + tv->text_frame.size.x, tv->text_frame.pos.y + tv->text_frame.size.y);

        // Clear all since Allegro 5 doesn't seem to do clipping on font
		al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);
	    
		// Draw separator between text and scrollbar
		t_frame frame = tv->widget_scrollbar->frame;
		al_draw_line(frame.pos.x, frame.pos.y, frame.pos.x, frame.pos.y + frame.size.y + 1, COLOR_SKIN_WINDOW_SEPARATORS, 0);
        /*al_draw_filled_rectangle(
            tv->text_frame.pos.x, tv->text_frame.pos.y, tv->text_frame.pos.x + tv->text_frame.size.x + 1, tv->text_frame.pos.y + tv->text_frame.size.y + 1,
            COLOR_SKIN_WINDOW_BACKGROUND);*/

        // Draw lines
        const int line_y = tv->scroll_position_y / tv->font_height;
        int y = tv->text_frame.pos.y - (tv->scroll_position_y % tv->font_height);
        for (int i = line_y; i < tv->text_lines_count; i++)
        {
            Font_Print(tv->font, tv->text_lines[i], tv->text_frame.pos.x, y, COLOR_SKIN_WINDOW_TEXT);
            y += tv->font_height;
			if (y > tv->text_frame.pos.y + tv->text_frame.size.y)
				break;
        }

        // Disable clipping
        //al_set_clipping_rectangle(0, 0, al_get_bitmap_width(tv->box->gfx_buffer), al_get_bitmap_height(tv->box->gfx_buffer));
    }
}

static void     TextViewer_Update_Inputs(t_app_textviewer *tv)
{
    // Check for focus
    if (!gui_box_has_focus(tv->box)) //  && tv->vel_y == 0)
        return;

    // Update mouse wheel inputs
    if (gui.mouse.wheel_rel != 0)
    {
        if (gui.mouse.wheel_rel > 0)
            tv->scroll_velocity_y -= TEXTVIEWER_SCROLL_VELOCITY_BASE * 4.0f;
        if (gui.mouse.wheel_rel < 0)
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
