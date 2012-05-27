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
    bool        active;
    char *      log_filename;
    FILE *      log_file;
    t_gui_box * box;
    BITMAP *    box_gfx;
    t_widget *  widget_textbox;
} t_app_messages;

t_app_messages  TB_Message;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TB_Message_Init_Values  (void);
void    TB_Message_Init         (void);
void    TB_Message_Update       (void);
void    TB_Message_Switch       (void);
void    TB_Message_Destroy      (void);
void    TB_Message_Print        (const char *line);

//-----------------------------------------------------------------------------

#endif // _MEKA_TEXTBOX_H_


