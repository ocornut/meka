//-----------------------------------------------------------------------------
// MEKA - options.c
// Options Box - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "app_options.h"
#include "g_file.h"
#include "g_widget.h"
#include "palette.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define OPTIONS_BUTTON_X        (54)
#define OPTIONS_BUTTON_Y        (25)
#define OPTIONS_PAD_X           (6)
#define OPTIONS_PAD_Y           (6)
#define OPTIONS_CHECK_X         (11)
#define OPTIONS_CHECK_Y         (11)

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static void     Options_Layout_AddLine(bool setup, t_frame *frame, const char *text, bool *v, void (*func)());
static void     Options_Layout(t_app_options *app, bool setup);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

static void    Option_Switch_Uses_VLFN(void)
{
    FB_Load_Directory ();
}

static void    Option_Switch_NES_Crap(void)
{
    g_Configuration.enable_NES = FALSE;
    Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_NES_Deny_Facts));
}

void    Options_Init_Applet(void)
{
    t_frame frame;

    // Create box
    frame.pos.x     = 252;
    frame.pos.y     = 66;
    frame.size.x    = 330;
    frame.size.y    = 200;
    Options.box = gui_box_new(&frame, Msg_Get (MSG_Options_BoxTitle));
    Desktop_Register_Box("OPTIONS", Options.box, 0, &Options.active);

    // Layout
    Options_Layout(&Options, TRUE);

    // Setup other members
    Options.active = FALSE;
}

static void     Options_Layout_AddLine(bool setup, t_frame *frame, const char *text, bool *v, void (*func)())
{
    const int shift_y = 3;

    frame->pos.x = OPTIONS_PAD_X;
    if (setup)
        widget_checkbox_add(Options.box, frame, v, func); // FIXME: Cast
    frame->pos.x += OPTIONS_CHECK_X + 8;
    Font_Print(F_MIDDLE, Options.box->gfx_buffer, text, frame->pos.x, frame->pos.y + shift_y, COLOR_SKIN_WINDOW_TEXT);
    frame->pos.y += OPTIONS_CHECK_Y + 4;
}

static void     Options_Layout(t_app_options *app, bool setup)
{
    t_frame frame;

    // Clear
    clear_to_color(app->box->gfx_buffer, COLOR_SKIN_WINDOW_BACKGROUND);

    if (setup)
    {
        // Add closebox widget
        widget_closebox_add(Options.box, Options_Switch);

        // Add close button
        frame.pos.x = Options.box->frame.size.x - OPTIONS_BUTTON_X - OPTIONS_PAD_X;
        frame.pos.y = Options.box->frame.size.y - OPTIONS_BUTTON_Y - OPTIONS_PAD_Y;
        frame.size.x = OPTIONS_BUTTON_X;
        frame.size.y = OPTIONS_BUTTON_Y;
        widget_button_add(Options.box, &frame, 1, Options_Switch, WIDGET_BUTTON_STYLE_BIG, Msg_Get(MSG_Options_Close));
    }

    // Draw option lines
    frame.pos.y  = OPTIONS_PAD_Y;
    frame.size.x = OPTIONS_CHECK_X;
    frame.size.y = OPTIONS_CHECK_Y;
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_BIOS_Enable),					&g_Configuration.enable_BIOS,					NULL);
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_DB_Display),					&g_Configuration.fb_uses_DB,					Option_Switch_Uses_VLFN);
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_Product_Number),				&g_Configuration.show_product_number,			NULL);
    //Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_Bright_Palette),			&g_Configuration.palette_type,					Palette_Emu_Reload);
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_Allow_Opposite_Directions),	&g_Configuration.allow_opposite_directions,		NULL);
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_Load_Close),					&g_Configuration.fb_close_after_load,			NULL);
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_Load_FullScreen),				&g_Configuration.fullscreen_after_load,			NULL);
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_FullScreen_Messages),			&g_Configuration.show_fullscreen_messages,		NULL);
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_GUI_VSync),					&g_Configuration.video_mode_gui_vsync,			NULL);
	Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_Capture_Crop_Align),			&g_Configuration.capture_crop_align_8x8,		NULL);
    Options_Layout_AddLine(setup, &frame, Msg_Get(MSG_Options_NES_Enable),					&g_Configuration.enable_NES,					Option_Switch_NES_Crap);
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

void    Options_Switch(void)
{
    Options.active ^= 1;
    gui_box_show(Options.box, Options.active, TRUE);
    gui_menu_inverse_check(menus_ID.file, 4);
}

//-----------------------------------------------------------------------------

