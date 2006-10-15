//-----------------------------------------------------------------------------
// MEKA - specials.c
// Special effects - Code
// was: Bloodlust theme                                          B.elieve
//-----------------------------------------------------------------------------
// FIXME: merge with effects.* ?
//-----------------------------------------------------------------------------

// FIXME: optimize colors/palette stuff, which are handled horribly

#include "shared.h"

//-----------------------------------------------------------------------------

// UPDATE BLOOD [AFTER] -------------------------------------------------------
void    special_effects_update_after (void)
{
  int   i;
  static int snow_wind = 0;
  t_gui_box *b;

  switch (Themes.special)
    {
    // BLOOD DROPS -------------------------------------------------------------
    case SPECIAL_BLOOD:
         // Create new drops ---------------------------------------------------
         for (i = 0; i < 6; i ++)
             gui_applet_blood_create (1 + Random(4), gui_mouse.x - 2 + Random(5), gui_mouse.y - 2 + Random(5));
         for (i = 0; i < 3; i ++)
             gui_applet_blood_create (1 + Random(4), gui_mouse.x - 4 + Random(9), gui_mouse.y - 4 + Random(9));
         gui_applet_blood_create (1 + Random(4), gui_mouse.x - 5 + Random(11), gui_mouse.y - 5 + Random(11));
         b = gui.box[gui.box_plan[0]];
         if (b->attr & A_Show)
            for (i = 0; i < 5; i ++)
                gui_applet_blood_create (1 + Random(4), 
                                         b->frame.pos.x - 2 + Random(b->frame.size.x + 4), 
                                         b->frame.pos.y + b->frame.size.y + 2);
         // Make the drops fall ------------------------------------------------
         for (i = 0; i < MAX_BLOOD_DROP; i ++)
             {
             if ((blood[i].v) && (Random(4) != 0))
                blood[i].y ++;
             if (blood[i].x < 0 || blood[i].x >= gui.info.screen.x
                 || blood[i].y < 0 || blood[i].y >= gui.info.screen.y)
                     blood[i].v = 0;
             }
         // Save old colors ----------------------------------------------------
         for (i = 0; i < MAX_BLOOD_DROP; i ++)
             if (blood[i].v)
                 blood[i].save = gui_buffer->line[blood[i].y][blood[i].x];
         // Draw blood drops ---------------------------------------------------
         for (i = 0; i < MAX_BLOOD_DROP; i ++)
             if (blood[i].v)
                 putpixel (gui_buffer, blood[i].x, blood[i].y, blood[i].v + GUI_COL_THEME_START + 4);
         break;
    // FLOATING HEARTS ---------------------------------------------------------
    case SPECIAL_HEARTS:
         // Create a new heart -------------------------------------------------
         if (Random(60) == 0)
            gui_applet_blood_create (Random(2), Random(gui.info.screen.x), gui.info.screen.y - gui.info.bars_height);
         // Make hearts floating -----------------------------------------------
         for (i = 0; i < MAX_HEARTS; i ++)
             {
             blood [i].y --;
             if (Random(2)) blood [i].x --; else blood [i].x ++;
             }
         // Save old graphics --------------------------------------------------
         for (i = 0; i < MAX_HEARTS; i ++)
             blit (gui_buffer, hearts_save [i], blood [i].x, blood [i].y, 0, 0, Graphics.Misc.Heart1->w, Graphics.Misc.Heart1->h);
         // Draw hearts --------------------------------------------------------
         for (i = 0; i < MAX_HEARTS; i ++)
             draw_sprite (gui_buffer, blood[i].v ? Graphics.Misc.Heart1 : Graphics.Misc.Heart2, blood[i].x, blood[i].y);
         break;
    // SNOW FLAKES -------------------------------------------------------------
    case SPECIAL_SNOW:
         if (snow_wind == 0)
            {
            if (Random(1000) == 0)
               snow_wind = 1;
            }
         else
            {
            if (Random(500) == 0)
               snow_wind = 0;
            }
         // Make the flakes fall -----------------------------------------------
         for (i = 0; i < MAX_SNOW_FLAKES; i ++)
             {
             if (Random(4) != 0)
                {
                snow[i].y ++;
                if (snow[i].y == gui.info.screen.y - gui.info.bars_height - 2)
                   snow[i].y = gui.info.bars_height + 2;
                }
             if (Random(10) == 0)
                {
                snow[i].x --;
                if (snow[i].x < 0)
                   snow[i].x += gui.info.screen.x;
                }
             else
             if (Random(10) == 0 || snow_wind == 1)
                {
                snow[i].x ++;
                if (snow[i].x >= gui.info.screen.x)
                   snow[i].x -= gui.info.screen.x;
                }
             }
         // Save old colors ----------------------------------------------------
         for (i = 0; i < MAX_SNOW_FLAKES; i ++)
             snow[i].save = getpixel(gui_buffer, snow[i].x, snow[i].y);
         // Draw snow flakes ---------------------------------------------------
         for (i = 0; i < MAX_SNOW_FLAKES; i ++)
             putpixel (gui_buffer, snow[i].x, snow[i].y, GUI_COL_WHITE);
         break;
    }
}

// INITIALIZE SNOW FLAKES POSITIONS -------------------------------------------
void    special_effects_snow_init (int i)
{
    snow [i].x = Random(gui.info.screen.x);
    snow [i].y = gui.info.bars_height + Random(gui.info.screen.y - 2 * (gui.info.bars_height + 2));
    snow [i].save = -1;
}

// UPDATE BLOOD [BEFORE] ------------------------------------------------------
void    special_effects_update_before (void)
{
  int   i;

  switch (Themes.special)
    {
    // BLOOD DROPS -------------------------------------------------------------
    case SPECIAL_BLOOD:
         for (i = 0; i < MAX_BLOOD_DROP; i ++)
             {
             if ((blood[i].v) && (blood[i].save != -1))
                {
                putpixel (gui_buffer, blood[i].x, blood[i].y, blood[i].save);
                }
             }
         break;
    // SNOW FLAKES -------------------------------------------------------------
    case SPECIAL_SNOW:
         for (i = 0; i < MAX_SNOW_FLAKES; i ++)
             if (snow[i].save != -1)
                putpixel (gui_buffer, snow[i].x, snow[i].y, snow[i].save);
         break;
    // FLOATING HEARTS ---------------------------------------------------------
    case SPECIAL_HEARTS:
         // Save old graphics --------------------------------------------------
         for (i = 0; i < MAX_HEARTS; i ++)
             blit (hearts_save [i], gui_buffer, 0, 0, blood [i].x, blood [i].y, Graphics.Misc.Heart1->w, Graphics.Misc.Heart1->h);
         break;
    }
}

// CREATE A BLOOD DROP --------------------------------------------------------
void    gui_applet_blood_create (int v, int x, int y)
{
  int   max;

  switch (Themes.special)
     {
     case SPECIAL_BLOOD:
          max = MAX_BLOOD_DROP;
          break;
     case SPECIAL_SNOW:
          max = MAX_SNOW_FLAKES;
          break;
     case SPECIAL_HEARTS:
          max = MAX_HEARTS;
          break;
     default:
          max = 0;
          break;
     }

  if (Random(20) != 0) // Do not create the drop every 20 times ---------------
     {
     blood [apps.opt.Blood_Current_Drop].v = v;
     blood [apps.opt.Blood_Current_Drop].x = x;
     blood [apps.opt.Blood_Current_Drop].y = y;
     blood [apps.opt.Blood_Current_Drop].save = -1;
     }
  apps.opt.Blood_Current_Drop ++;
  if (apps.opt.Blood_Current_Drop >= max)
     {
     apps.opt.Blood_Current_Drop = 0;
     }
}

// INIT BLOOD -----------------------------------------------------------------
void    special_effects_init (void)
{
  int   i;

  apps.opt.Blood_Current_Drop = 0;
  for (i = 0; i < MAX_BLOOD_DROP; i ++)
      {
      gui_applet_blood_create (0, 0, 0);
      }
  for (i = 0; i < MAX_SNOW_FLAKES; i ++)
      {
      special_effects_snow_init (i);
      }
  for (i = 0; i < MAX_HEARTS; i ++)
      {
      hearts_save [i] = create_bitmap (Graphics.Misc.Heart1->w, Graphics.Misc.Heart1->h);
      }
}

//-----------------------------------------------------------------------------

