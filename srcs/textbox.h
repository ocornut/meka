//-----------------------------------------------------------------------------
// MEKA - textbox.h
// Text Box Applet - Headers
//-----------------------------------------------------------------------------

#ifndef _MEKA_TEXTBOX_H_
#define _MEKA_TEXTBOX_H_

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
    byte        Active;
    char *      log_filename;
    FILE *      log_file;
    t_gui_box * box;
    BITMAP *    box_gfx;
    t_widget *  widget_textbox;
} t_messages_app;

t_messages_app  TB_Message;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TB_Message_Init_Values  (void);
void    TB_Message_Init         (void);
void    TB_Message_Switch       (void);
void    TB_Message_Destroy      (void);
void    TB_Message_Print        (char *line);

//-----------------------------------------------------------------------------

#endif // _MEKA_TEXTBOX_H_


