//-----------------------------------------------------------------------------
// MEKA - options.c
// Options Box - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "options.h"
#include "g_file.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------

void    Option_Switch_Uses_VLFN (void)
{
    FB_Load_Directory ();
}

void    Option_Switch_NES_Crap (void)
{
    Configuration.enable_NES = NO;
    Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_NES_Deny_Facts));
}

static void    Options_Add_Line (t_frame *frame, char *text, bool *v, void (*func)())
{
    int shift_y = 3; // Crap Constant - Damn, I hate Constants!

    frame->pos.x = OPTIONS_PAD_X;
    widget_checkbox_add (Options.ID, frame, (byte *)v, func); // FIXME: Cast
    frame->pos.x += OPTIONS_CHECK_X + 8;
    Font_Print (-1, Options.Bmp, text, frame->pos.x, frame->pos.y + shift_y, GUI_COL_TEXT_IN_BOX);
    frame->pos.y += OPTIONS_CHECK_Y + 4;
}

void    Options_Init_Applet (void)
{
    t_frame frame;

    Options.Active = 0;
    Options.Res_X = 330;
    Options.Res_Y = 200; // 170; // testing inputbox widget
    Options.ID = gui_box_create (252, 66, Options.Res_X, Options.Res_Y, Msg_Get (MSG_Options_BoxTitle));
    Options.Bmp = create_bitmap (Options.Res_X + 1, Options.Res_Y + 1);
    gui_set_image_box (Options.ID, Options.Bmp);
    Desktop_Register_Box ("OPTIONS", Options.ID, 0, &Options.Active);

    widget_closebox_add (Options.ID, Options_Switch);
    frame.pos.x = Options.Res_X - OPTIONS_BUTTON_X - OPTIONS_PAD_X;
    frame.pos.y = Options.Res_Y - OPTIONS_BUTTON_Y - OPTIONS_PAD_Y;
    frame.size.x = OPTIONS_BUTTON_X;
    frame.size.y = OPTIONS_BUTTON_Y;
    widget_button_add_draw (Options.ID, &frame, LOOK_ROUND, 0, 1, Options_Switch, Msg_Get (MSG_Options_Close));

    frame.pos.y  = OPTIONS_PAD_Y;
    frame.size.x = OPTIONS_CHECK_X;
    frame.size.y = OPTIONS_CHECK_Y;
    Font_SetCurrent (F_MIDDLE);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_BIOS_Enable),         &Configuration.enable_BIOS,                NULL);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_DB_Display),          &Configuration.fb_uses_DB,                 Option_Switch_Uses_VLFN);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_Product_Number),      &Configuration.show_product_number,        NULL);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_Bright_Palette),      &Configuration.palette_type,               Palette_Emu_Reload);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_Allow_Opposite_Directions),     &Configuration.allow_opposite_directions,     NULL);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_Load_Close),          &Configuration.fb_close_after_load,        NULL);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_Load_FullScreen),     &Configuration.fullscreen_after_load,      NULL);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_FullScreen_Messages), &Configuration.show_fullscreen_messages,   NULL);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_GUI_VSync),           &cfg.GUI_VSync,                            NULL);
    Options_Add_Line (&frame, Msg_Get (MSG_Options_NES_Activated),       &Configuration.enable_NES,                 Option_Switch_NES_Crap);

    // FIXME: testing new widget
    /*
    {
        t_widget *w;
        frame.size.x = 200;
        w = widget_inputbox_add(Options.ID, &frame, 30, F_MIDDLE, NULL);
        widget_inputbox_set_value(w, "Hello, world!");
    }
    */
}

void    Options_Switch (void)
{
    Options.Active ^= 1;
    gui_box_show (gui.box[Options.ID], Options.Active, TRUE);
    gui_menu_inverse_check (menus_ID.file, 4);
}

//-----------------------------------------------------------------------------

