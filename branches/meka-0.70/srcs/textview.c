//-----------------------------------------------------------------------------
// MEKA - textview.c
// Text Viewer Applet - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"
#include "inputs_t.h"

//-----------------------------------------------------------------------------

void            TextViewer_Init(void)
{
    TextViewer.Active = 0;
    TextViewer.CurrentFile = -1;
    TextViewer.TV = TextViewer_New ("Text Viewer",  // Name
        TEXTVIEW_COLUMNS, TEXTVIEW_LINES,           // Size
        F_MIDDLE,                                   // Font
        22, 54,                                     // Position
        TEXTVIEW_PAD_X);                            // Padding
    widget_closebox_add (TextViewer.TV->ID_Box, TextViewer_Switch_Close);
    Desktop_Register_Box ("TEXTVIEW", TextViewer.TV->ID_Box, 0, &TextViewer.Active);
}

int             TextViewer_Open(t_textviewer *TV, char *name, char *filename)
{
 FILE           *f;
 t_list         *list_lines;
 char           s[512];
 int            i;

 if ((f = fopen(filename, "rt")) == 0)
    return (MEKA_ERR_FILE_OPEN);

 // Set new title
 gui_box_set_title (gui.box[TV->ID_Box], name);

 // FIXME: should be using t_tfile there..
 if (TV->Lines)
    {
    for (i = 0; i < TV->Num_Lines; i++)
        free (TV->Lines [i]);
    TV->Lines = NULL;
    }
 list_lines = NULL;
 TV->Num_Lines = 0;
 while (!feof (f))
    {
    memset (s, 0, 512);
    fgets (s, 511, f);
    Chomp (s);
    // FIXME: Non ASCII. Used to display properly in MS-DOS only.
    // Replace characters 250 (E by 46 (.) and 249 (E by 183 (·)
    // which are equivalents in the used font
    /*
    for (i = 0; s[i]; i++)
       {
       if (s[i] == 250) s[i] = '.';    // FIXME
       if (s[i] == 249) s[i] = 183;    // FIXME
       }
    */
    list_add_to_end (&list_lines, strdup (s));
    TV->Num_Lines++;
    }
 fclose (f);
 TV->Lines = list_to_tab(list_lines);
 list_free_no_elem(&list_lines);
 TV->Need_Update = YES;
 TV->Pos_Y = 0;
 TV->Max_Pos_Y = TV->Num_Lines * TV->Font_Height;
 TV->Max_Start_Pos_Y = TV->Max_Pos_Y - (TV->Size_Y * TV->Font_Height);
 TextViewer_Update(TV);
 return (MEKA_ERR_OK);
}

t_textviewer    *TextViewer_New(char *Name, int Size_X, int Size_Y, int ID_Font,
                                int Pos_X, int Pos_Y, int Pad_X)
{
 t_textviewer *TV;

 TV = malloc (sizeof (*TV));
 TV->Vel_Y = TV->Pos_Y = 0;
 TV->Size_X = Size_X;
 TV->Size_Y = Size_Y;
 TV->Pos_Y = TV->Max_Pos_Y = 0;
 TV->Lines = 0; /* nothing loaded yet */
 TV->Num_Lines = 0; /* nothing loaded yet */
 TV->Need_Update = YES;
 TV->ID_Font = ID_Font;
 TV->Font_Height = Font_Height (TV->ID_Font);
 TV->Pos_Per_Page = TV->Size_Y * TV->Font_Height;
 TV->ID_Box = gui_box_create (Pos_X, Pos_Y,
                             (Size_X * TV->Font_Height /* FIXME */) + (Pad_X) - 1,
                             ((Size_Y + 2) * TV->Font_Height) - 1, /* +2 for vertical padding */
                             Name);
 TV->ID_BoxGfx = create_bitmap (gui.box[TV->ID_Box]->frame.size.x + 1, gui.box[TV->ID_Box]->frame.size.y + 1);
 gui_set_image_box (TV->ID_Box, TV->ID_BoxGfx);

 TV->Pad_X = Pad_X;
 TV->Pad_Y = TV->Font_Height;

 {
   t_frame frame;
   frame.pos.x = gui.box[TV->ID_Box]->frame.size.x - /* TV->Pad_X - */ TEXTVIEW_SCROLLBAR_LX;
   frame.pos.y = 0; /* TV->Pad_Y / 2; */
   frame.size.x = TEXTVIEW_SCROLLBAR_LX;
   frame.size.y = gui.box[TV->ID_Box]->frame.size.y; /* - TV->Pad_Y; */
 
   widget_scrollbar_add (TV->ID_Box, &frame, &TV->Max_Pos_Y, &TV->Pos_Y, &TV->Pos_Per_Page, TextViewer_Refresh);
   line (TV->ID_BoxGfx, frame.pos.x - 1, frame.pos.y, frame.pos.x - 1, frame.pos.y + frame.size.y, GUI_COL_BORDERS);
   //gui_rect (TV->ID_BoxGfx, LOOK_ROUND, x1 - 2, y1 - 2, x1 + x2 + 2, y1 + y2 + 2, GUI_COL_BORDERS);
 }

  return (TV);
}

#define DOC_MAIN        (0)
#ifdef WIN32
  #define DOC_MAINW       (1)
  #define DOC_COMPAT      (2)
  #define DOC_MULTI       (3)
  #define DOC_CHANGES     (4)
  #define DOC_MAX         (5)
#elif UNIX
  #define DOC_MAINU       (1)
  #define DOC_COMPAT      (2)
  #define DOC_MULTI       (3)
  #define DOC_CHANGES     (4)
  #define DOC_MAX         (5)
#else
  #define DOC_COMPAT      (1)
  #define DOC_MULTI       (2)
  #define DOC_CHANGES     (3)
  #define DOC_MAX         (4)
#endif

void            TextViewer_Switch_Doc_Main(void)
{ TextViewer_Switch (Msg_Get (MSG_Doc_BoxTitle), Env.Paths.DocumentationMain,       DOC_MAIN); }

#ifdef WIN32
void            TextViewer_Switch_Doc_MainW(void)
{ TextViewer_Switch (Msg_Get (MSG_Doc_BoxTitle), Env.Paths.DocumentationMainW,      DOC_MAINW); }
#endif

#ifdef UNIX
void            TextViewer_Switch_Doc_MainU(void)
{ TextViewer_Switch (Msg_Get (MSG_Doc_BoxTitle), Env.Paths.DocumentationMainU,      DOC_MAINU); }
#endif

void            TextViewer_Switch_Doc_Compat(void)
{ TextViewer_Switch (Msg_Get (MSG_Doc_BoxTitle), Env.Paths.DocumentationCompat,     DOC_COMPAT); }

void            TextViewer_Switch_Doc_Multiplayer_Games(void)
{ TextViewer_Switch (Msg_Get (MSG_Doc_BoxTitle), Env.Paths.DocumentationMulti,      DOC_MULTI); }

void            TextViewer_Switch_Doc_Changes(void)
{ TextViewer_Switch (Msg_Get (MSG_Doc_BoxTitle), Env.Paths.DocumentationChanges,    DOC_CHANGES); }

void            TextViewer_Switch(char *box_title, char *filename, int current_file)
{
    if (TextViewer.CurrentFile != current_file)
    {
        if (TextViewer_Open (TextViewer.TV, box_title, filename) != MEKA_ERR_OK)
            Msg (MSGT_USER, Msg_Get (MSG_Doc_File_Error));
        TextViewer.CurrentFile = current_file;
        TextViewer.Active = 1;
    }
    else
    {
        if (TextViewer.Active ^= 1)
            Msg (MSGT_USER, Msg_Get (MSG_Doc_Enabled));
        else
            Msg (MSGT_USER, Msg_Get (MSG_Doc_Disabled));
    }
    gui_box_show (gui.box[TextViewer.TV->ID_Box], TextViewer.Active, TRUE);
    gui_menu_un_check_area (menus_ID.help, 0, DOC_MAX - 1);
    if (TextViewer.Active)
        gui_menu_check (menus_ID.help, current_file);
}

void            TextViewer_Switch_Close(void)
{
    TextViewer.Active = 0;
    Msg (MSGT_USER, Msg_Get (MSG_Doc_Disabled));
    gui_box_show (gui.box[TextViewer.TV->ID_Box], TextViewer.Active, TRUE);
    gui_menu_un_check_area (menus_ID.help, 0, DOC_MAX - 1);
}

void            TextViewer_Refresh(void)
{
    TextViewer.TV->Need_Update = YES;
    TextViewer_Update (TextViewer.TV);
}

void            TextViewer_Update(t_textviewer *TV)
{
 int            i, y, ly;

 if (!TV->Need_Update)
    return;
 TV->Need_Update = 0;
 ly = TV->Pos_Y / TV->Font_Height;
 y = TV->Pad_Y - (TV->Pos_Y % TV->Font_Height);
 rectfill (TV->ID_BoxGfx, TV->Pad_X, TV->Pad_Y, TV->ID_BoxGfx->w - TV->Pad_X - TEXTVIEW_SCROLLBAR_LX - 4, TV->ID_BoxGfx->h - TV->Pad_Y, GUI_COL_FILL);
 Font_SetCurrent (TV->ID_Font);
 for (i = ly; i < ly + TV->Size_Y + 1 && i < TV->Num_Lines; i++)
     {
     set_clip (TV->ID_BoxGfx, TV->Pad_X, TV->Pad_Y, TV->ID_BoxGfx->w - TV->Pad_X - TEXTVIEW_SCROLLBAR_LX - 4, TV->ID_BoxGfx->h - TV->Pad_Y);
     Font_Print (-1, TV->ID_BoxGfx, TV->Lines[i], TV->Pad_X, y, GUI_COL_TEXT_IN_BOX);
     set_clip (TV->ID_BoxGfx, 0, 0, TV->ID_BoxGfx->w, TV->ID_BoxGfx->h);
     y += TV->Font_Height;
     }
 rectfill (TV->ID_BoxGfx, TV->Pad_X, 0, TV->ID_BoxGfx->w - TV->Pad_X - TEXTVIEW_SCROLLBAR_LX - 4, TV->Pad_Y, GUI_COL_FILL);
 rectfill (TV->ID_BoxGfx, TV->Pad_X, TV->ID_BoxGfx->h - TV->Pad_Y, TV->ID_BoxGfx->w - TV->Pad_X - TEXTVIEW_SCROLLBAR_LX - 4, TV->ID_BoxGfx->h, GUI_COL_FILL);
 gui.box [TV->ID_Box]->must_redraw = YES;
}

void            TextViewer_Update_Inputs(t_textviewer *TV)
{
 // Check for focus
 if (!gui_box_has_focus (gui.box[TV->ID_Box]) && TV->Vel_Y == 0)
     return;

 // Update mouse inputs
 if (gui_mouse.z_rel != 0)
    {
	if (gui_mouse.z_rel > 0)
       TV->Vel_Y -= TEXTVIEW_VEL_STD * 4.0f;
	if (gui_mouse.z_rel < 0)
       TV->Vel_Y += TEXTVIEW_VEL_STD * 4.0f;
    }

 // Update keyboard inputs
    if (Inputs_KeyPressed (KEY_HOME, NO))
    {
        TV->Pos_Y = 0;
        TV->Vel_Y = 0;
        // TV->Vel_Y = -TEXTVIEW_VEL_MAX;
    }
 else
 if (Inputs_KeyPressed (KEY_END, NO))
    {
    TV->Pos_Y = TV->Max_Start_Pos_Y;
    TV->Vel_Y = 0;
    // TV->Vel_Y = TEXTVIEW_VEL_MAX;
    }
 else
 if (Inputs_KeyPressed_Repeat (KEY_PGDN, NO, 25, 14))
    {
    TV->Vel_Y += TEXTVIEW_VEL_STD * 5.5f;
    }
 else
 if (Inputs_KeyPressed_Repeat (KEY_PGUP, NO, 25, 14))
    {
    TV->Vel_Y -= TEXTVIEW_VEL_STD * 5.5f;
    }
 else // FIXME:
 if (/*Test_Typematic_Key (KEY_DOWN, 12, 0)*/ key[KEY_DOWN] && !key[KEY_UP])
    {
    TV->Vel_Y += TEXTVIEW_VEL_STD;
    }
 else
 if (/*Test_Typematic_Key (KEY_UP, 12, 0)*/ key[KEY_UP] && !key[KEY_DOWN])
    {
    TV->Vel_Y -= TEXTVIEW_VEL_STD;
    }
 else
 if (TV->Vel_Y == 0)
    return;

 // Limit
 if (TV->Vel_Y < -TEXTVIEW_VEL_MAX) TV->Vel_Y = -TEXTVIEW_VEL_MAX;
 if (TV->Vel_Y > +TEXTVIEW_VEL_MAX) TV->Vel_Y = +TEXTVIEW_VEL_MAX;

 // Update cinetic
 TV->Pos_Y += TV->Vel_Y;
 if (TV->Pos_Y < 0)
    {
    TV->Pos_Y = 0;
    TV->Vel_Y = - (TV->Vel_Y / 1.5);
    }
 if (TV->Pos_Y > TV->Max_Start_Pos_Y)
    {
    TV->Pos_Y = TV->Max_Start_Pos_Y;
    TV->Vel_Y = - (TV->Vel_Y / 1.5);
    }
 if (TV->Vel_Y != 0)
    {
    if (TV->Vel_Y > 3) TV->Vel_Y -= 3;
    else
    if (TV->Vel_Y < -3) TV->Vel_Y += 3;
    else
    TV->Vel_Y = 0;
    }

// if (TV->Vel_Y != 0)
    TV->Need_Update = 1;
 TextViewer_Update(TextViewer.TV);
}

//-----------------------------------------------------------------------------

