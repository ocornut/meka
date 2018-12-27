//-----------------------------------------------------------------------------
// MEKA - options.c
// Options Box - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "app_options.h"
#include "app_filebrowser.h"
#include "g_widget.h"
#include "palette.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_app_options Options;

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define OPTIONS_PAD_X           (6)
#define OPTIONS_PAD_Y           (6)

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static void     Options_Layout(t_app_options *app, bool setup);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

static void    Option_Switch_Uses_VLFN()
{
    FB_Load_Directory();
}

static void    Option_Switch_NES_Crap()
{
    g_configuration.enable_NES = FALSE;
    Msg(MSGT_STATUS_BAR, "%s", Msg_Get(MSG_NES_Deny_Facts));
}

void    Options_Init_Applet()
{
    // Create box
	t_frame frame;
    frame.pos.x     = 437;
    frame.pos.y     = 102;
    frame.size.x    = 400;
    frame.size.y    = 220;
    Options.box = gui_box_new(&frame, Msg_Get(MSG_Options_BoxTitle));
    Desktop_Register_Box("OPTIONS", Options.box, 0, &Options.active);

    // Layout
    Options_Layout(&Options, TRUE);

    // Setup other members
    Options.active = FALSE;
}

static void     Options_Layout_AddLine(t_app_options* app, bool setup, t_frame *frame, const char *text, bool *v, void (*func)())
{
    frame->pos.x = OPTIONS_PAD_X;
    if (setup)
        widget_checkbox_add(Options.box, frame, v, (t_widget_callback)func); // FIXME: Cast
    frame->pos.x += frame->size.x + Font_TextWidth(app->font_id, " ");
    Font_Print(app->font_id, text, frame->pos.x, frame->pos.y+2, COLOR_SKIN_WINDOW_TEXT);
    frame->pos.y += (int)(Font_Height(app->font_id) * 1.3f);
}

static void     Options_Layout(t_app_options* app, bool setup)
{
	app->font_id = (t_font_id)g_configuration.font_options;

	t_frame frame;

    al_set_target_bitmap(app->box->gfx_buffer);
	al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    if (setup)
    {
        // Add closebox widget
        widget_closebox_add(Options.box, (t_widget_callback)Options_Switch);

        // Add close button
		frame.size.x = Font_TextWidth(app->font_id, Msg_Get(MSG_Options_Close))+15;
		frame.size.y = Font_Height(app->font_id)*2;
        frame.pos.x = Options.box->frame.size.x - frame.size.x - OPTIONS_PAD_X;
        frame.pos.y = Options.box->frame.size.y - frame.size.y - OPTIONS_PAD_Y;
        widget_button_add(Options.box, &frame, 1, (t_widget_callback)Options_Switch, app->font_id, Msg_Get(MSG_Options_Close));
    }

    // Draw option lines
	frame.pos.x = Font_Height(app->font_id)-2;
	frame.pos.y = OPTIONS_PAD_Y;
    frame.size.x = Font_Height(app->font_id);
    frame.size.y = Font_Height(app->font_id);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_BIOS_Enable),				&g_configuration.enable_BIOS,					NULL);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_DB_Display),					&g_configuration.fb_uses_DB,					Option_Switch_Uses_VLFN);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_Product_Number),				&g_configuration.show_product_number,			NULL);
    //Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_Bright_Palette),			&g_configuration.palette_type,					Palette_Emu_Reload);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_Allow_Opposite_Directions),	&g_configuration.allow_opposite_directions,		NULL);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_Load_Close),					&g_configuration.fb_close_after_load,			NULL);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_Load_FullScreen),			&g_configuration.fullscreen_after_load,			NULL);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_FullScreen_Messages),		&g_configuration.show_fullscreen_messages,		NULL);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_GUI_VSync),					&g_configuration.video_mode_gui_vsync,			NULL);
	Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_Capture_Crop_Align),			&g_configuration.capture_crop_align_8x8,		NULL);
    Options_Layout_AddLine(app, setup, &frame, Msg_Get(MSG_Options_NES_Enable),					&g_configuration.enable_NES,					Option_Switch_NES_Crap);
}

void    Options_Update(void)
{
    t_app_options *app = &Options;  // Global instance

    // Skip update if not active
    if (!app->active)
        return;

    // If skin has changed, redraw everything
    if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        Options_Layout(app, FALSE);
        app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
    }
}

void    Options_Switch()
{
    Options.active ^= 1;
    gui_box_show(Options.box, Options.active, TRUE);
    gui_menu_toggle_check(menus_ID.file, 8);
}

//-----------------------------------------------------------------------------

