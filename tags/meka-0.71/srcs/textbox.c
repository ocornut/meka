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

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TB_MESSAGE_LINES        (22) // 50
#define TB_MESSAGE_COLUMNS      (40)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TB_Message_Init_Values (void)
{
    TB_Message.Active           = NO;
    TB_Message.log_file         = NULL;
    TB_Message.log_filename     = NULL;
    TB_Message.box              = NULL;
    TB_Message.box_gfx          = NULL;
    TB_Message.widget_textbox   = NULL;
}

void        TB_Message_Init (void)
{
    int     box_id;
    int     font_id;
    t_frame frame;

    TB_Message.Active = YES;

    // Create box
    font_id = F_MIDDLE;
    frame.pos.x  = 10;
    frame.pos.y  = 270;
    frame.size.x = (TB_MESSAGE_COLUMNS * Font_Height (font_id)) + (4*2); // 4*2=padding
    frame.size.y = (TB_MESSAGE_LINES   * Font_Height (font_id)) + (2*2); // 2*2=padding
    box_id = gui_box_create (frame.pos.x, frame.pos.y, frame.size.x, frame.size.y, Msg_Get (MSG_Message_BoxTitle));
    TB_Message.box = gui.box[box_id];
    TB_Message.box_gfx = create_bitmap (frame.size.x + 1, frame.size.y + 1);
    gui_set_image_box (box_id, TB_Message.box_gfx);

    // Register to desktop
    Desktop_Register_Box ("MESSAGES", box_id, YES, &TB_Message.Active);

    // Add closebox widget
    widget_closebox_add (box_id, TB_Message_Switch);

    // Create textbox widget
    frame.pos.x = 4;
    frame.pos.y = 2;
    frame.size.x = TB_Message.box->frame.size.x - (4*2);
    frame.size.y = TB_Message.box->frame.size.y - (2*2);
    TB_Message.widget_textbox = widget_textbox_add(box_id, &frame, TB_MESSAGE_LINES, font_id);

    // Open log file
    if (TB_Message.log_filename != NULL)
    {
        TB_Message.log_file = fopen (TB_Message.log_filename, "a+t");
        if (TB_Message.log_file)
            fprintf (TB_Message.log_file, Msg_Get (MSG_Log_Session_Start), meka_date_getf ());
    }
}

void    TB_Message_Switch (void)
{
    if (TB_Message.Active ^= 1)
        Msg (MSGT_USER, Msg_Get (MSG_Message_Enabled));
    else
        Msg (MSGT_USER, Msg_Get (MSG_Message_Disabled));
    gui_box_show (TB_Message.box, TB_Message.Active, TRUE);
    gui_menu_inverse_check (menus_ID.tools, 0);
}

void    TB_Message_Print (char *line)
{
    widget_textbox_print_scroll(TB_Message.widget_textbox, YES, line);
    if (TB_Message.log_file)
       fprintf (TB_Message.log_file, "%s\n", line);
}

void    TB_Message_Destroy (void)
{
    // FIXME: widgets are not meant to be destroyed yet
    // ...
    if (TB_Message.log_file)
    {
        fclose (TB_Message.log_file);
        TB_Message.log_file = NULL;
    }
    if (TB_Message.log_filename)
    {
        free (TB_Message.log_filename);
        TB_Message.log_filename = NULL;
    }
}

//-----------------------------------------------------------------------------
