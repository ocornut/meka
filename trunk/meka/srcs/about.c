//-----------------------------------------------------------------------------
// MEKA - about.c
// About Box - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"
#include "inputs_t.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
    t_gui_box *     box;
} t_about_box;

t_about_box         AboutBox;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    About_Switch (void)
{
    int    menu_pos;

    #ifdef DOS
        menu_pos = 4;
    #else
        menu_pos = 5;
    #endif
    apps.active.About ^= 1;
    gui_box_show (gui.box[apps.id.About], apps.active.About, TRUE);
    gui_menu_inverse_check (menus_ID.help, menu_pos);

    // Easter egg: BrainWash
    if (Inputs_KeyPressed (KEY_LCONTROL, NO))
        BrainWash_Start ();
}

void    About_Init (void)
{
  int   i, x, y;

  apps.id.About = gui_box_create (285, 60, 346, 92, Msg_Get (MSG_About_BoxTitle));
  AboutBox.box = gui.box[apps.id.About];
  apps.gfx.About = create_bitmap (AboutBox.box->frame.size.x + 1, AboutBox.box->frame.size.y + 1);
  gui_set_image_box (apps.id.About, apps.gfx.About);
  Desktop_Register_Box ("ABOUT", apps.id.About, 0, &apps.active.About);

  widget_closebox_add (AboutBox.box->stupid_id, About_Switch);
  draw_sprite (apps.gfx.About, Graphics.Misc.Dragon, 10, (AboutBox.box->frame.size.y - Graphics.Misc.Dragon->h) / 2);

  y = 9+8;
  Font_SetCurrent (F_LARGE);
  for (i = 0; i < 3; i ++)
      {
      switch (i)
         {
         case 0: sprintf (GenericBuffer, Msg_Get(MSG_About_Line_Meka_Date), PROG_NAME_VER, PROG_DATE); break;
         case 1: sprintf (GenericBuffer, Msg_Get(MSG_About_Line_Authors), PROG_AUTHORS_SHORT); break;
         case 2: sprintf (GenericBuffer, Msg_Get(MSG_About_Line_Homepage), PROG_HOMEPAGE); break;
         /*
         case 3: if (registered.is)
                    {
                    strcpy (GenericBuffer, "Registered version");
                    }
                 else
                    {
                    strcpy (GenericBuffer, "Unregistered version");
                    }
                 break;
         */
         }
      x = (( (AboutBox.box->frame.size.x - Graphics.Misc.Dragon->h - 18 - 6) - Font_TextLength (-1, GenericBuffer) ) / 2) + Graphics.Misc.Dragon->h + 8 + 6;
      Font_Print (-1, apps.gfx.About, GenericBuffer, x, y, GUI_COL_TEXT_IN_BOX);
      y += Font_Height (-1) + 3;
      }
}

//-----------------------------------------------------------------------------

