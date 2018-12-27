//-----------------------------------------------------------------------------
// MEKA - textbox.c
// Text Box Applet - Code
//-----------------------------------------------------------------------------
// FIXME: this could be merged into message.c someday, and made obsolete.
// It was previously the full messages applet, now that there is a widget
// for that, it is getting useless.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"
#include "textbox.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_app_messages  TB_Message;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TB_Message_Init_Values(void)
{
    t_app_messages *app = &TB_Message;  // Global instance

    app->active           = FALSE;
    app->log_file         = NULL;
    app->log_filename     = NULL;
    app->box              = NULL;
    app->widget_textbox   = NULL;
}

static void TB_Message_Layout(t_app_messages *app, bool setup)
{
    al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    // Add closebox widget
	if (setup)
		widget_closebox_add(app->box, (t_widget_callback)TB_Message_Switch);

    // Create textbox widget
	t_frame frame;
    frame.pos.x = 4;
    frame.pos.y = 2;
    frame.size.x = app->box->frame.size.x - (4*2);
    frame.size.y = app->box->frame.size.y - (2*2);
	if (setup)
		app->widget_textbox = widget_textbox_add(app->box, &frame, (t_font_id)g_configuration.font_messages);
	else
		app->widget_textbox->frame = frame;
}

void        TB_Message_Init()
{
    t_app_messages *app = &TB_Message;  // Global instance
	t_font_id font_id = (t_font_id)g_configuration.font_messages;

    app->active = true;

    // Create box
	t_frame frame;
    frame.pos.x  = 16;
    frame.pos.y  = 626;
    frame.size.x = (48 * Font_Height(font_id)) + (4*2); // 4*2=padding
    frame.size.y = (8 * Font_Height(font_id)) + (2*2); // 2*2=padding
    app->box = gui_box_new(&frame, Msg_Get(MSG_Message_BoxTitle));
	app->box->flags |= GUI_BOX_FLAGS_ALLOW_RESIZE;

    // Register to desktop
    Desktop_Register_Box("MESSAGES", app->box, true, &app->active);

    // Layout
    TB_Message_Layout(app, true);

    // Open log file
    if (app->log_filename != NULL)
    {
        app->log_file = fopen(app->log_filename, "a+t");
        if (app->log_file)
            fprintf(app->log_file, Msg_Get(MSG_Log_Session_Start), meka_date_getf());
    }
}

void    TB_Message_Update()
{
    t_app_messages *app = &TB_Message;  // Global instance

    // Skip update if not active
    if (!app->active)
        return;

    // If skin has changed, redraw everything
    if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        TB_Message_Layout(app, FALSE);
        app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
    }
}

void    TB_Message_Switch(void)
{
    t_app_messages *app = &TB_Message;  // Global instance

    if (app->active ^= 1)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Message_Enabled));
    else
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Message_Disabled));
    gui_box_show (app->box, app->active, TRUE);
    gui_menu_toggle_check (menus_ID.tools, 0);
}

void    TB_Message_Print (const char *line)
{
    t_app_messages *app = &TB_Message;  // Global instance

    widget_textbox_print_scroll(app->widget_textbox, TRUE, line);
    if (app->log_file)
       fprintf(app->log_file, "%s\n", line);
}

void    TB_Message_Destroy (void)
{
    t_app_messages *app = &TB_Message;  // Global instance

    // FIXME: widgets are not meant to be destroyed yet
    // ...
    if (app->log_file)
    {
        fclose (app->log_file);
        app->log_file = NULL;
    }
    if (app->log_filename)
    {
        free (app->log_filename);
        app->log_filename = NULL;
    }
}

//-----------------------------------------------------------------------------
