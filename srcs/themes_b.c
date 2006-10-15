//-----------------------------------------------------------------------------
// MEKA - themes_b.c
// Themes Background Refresh - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

void        Regenerate_Background_Grid (void)
{
    int     i;

    clear_to_color (gui_background, GUI_COL_BACKGROUND);
    for (i = gui.info.grid_distance; i < gui.info.screen.y; i += gui.info.grid_distance)
        line (gui_background, 0, i, gui.info.screen.x, i, GUI_COL_BACKGROUND_2);
    for (i = gui.info.grid_distance; i < gui.info.screen.x; i += gui.info.grid_distance)
        line (gui_background, i, 0, i, gui.info.screen.y, GUI_COL_BACKGROUND_2);
}

void    Regenerate_Background_Stretched (void)
{
    stretch_blit (ThemeBackground.picture, gui_background,
        0, 0,
        ThemeBackground.picture->w, ThemeBackground.picture->h,
        0, gui.info.bars_height + 2,
        gui.info.screen.x, gui.info.screen.y - 2 * (gui.info.bars_height + 2));
}

void    Regenerate_Background_Int_Stretched (void)
{
  int   sx, sy;
  int   factor = 0;

  do
     {
     factor += 1;
     sx = (gui.info.screen.x - ThemeBackground.picture->w * factor) / 2;
     sy = (gui.info.screen.y - 2 * (gui.info.bars_height + 2) - ThemeBackground.picture->h * factor) / 2;
     }
  while (sx >= 0 && sy >= 0);
  factor -= 1;

  Regenerate_Background_Grid ();
  if (factor <= 0)
     {
     Msg (MSGT_USER, Msg_Get (MSG_Theme_Error_BG_Big));
     return;
     }

  sx = (gui.info.screen.x - ThemeBackground.picture->w * factor) / 2;
  sy = (gui.info.screen.y - 2 * (gui.info.bars_height + 2) - ThemeBackground.picture->h * factor) / 2;
  sy += gui.info.bars_height + 2;
  /*
  ConsolePrintf("pic = %p, gui_bg = %p\npic(%d,%d),bg(%d,%d)", 
	       ThemeBackground.picture, gui_background,
		ThemeBackground.picture->w,
		ThemeBackground.picture->h,
		gui_background->w,
		gui_background->h
		);
  ConsolePrintf("sx = %d, sy = %d\n", sx, sy);
  ConsolePrintf("gui.info.screen=%d,%d\n", gui.info.screen_x, gui.info.screen_y);
  */
  stretch_blit (ThemeBackground.picture, gui_background,
                0, 0,
                ThemeBackground.picture->w, ThemeBackground.picture->h,
                sx, sy,
                gui.info.screen.x - 2 * sx, gui.info.screen.y - 2 * sy);
}

void        Regenerate_Background_Tiled (void)
{
    int     sx, sy;
    int     cx, cy;

    sx = (ThemeBackground.picture->w);
    sy = (ThemeBackground.picture->h);
    cy = 0;
    while (cy < gui.info.screen.y)
    {
        cx = 0;
        while (cx < gui.info.screen.x)
        {
            blit (ThemeBackground.picture, gui_background,
                0, 0, cx, cy, ThemeBackground.picture->w, ThemeBackground.picture->h);
            cx += sx;
        }
        cy += sy;
    }
}

void        Regenerate_Background_Centered (void)
{
    int     sx, sy;

    Regenerate_Background_Grid ();
    sx = (gui.info.screen.x - ThemeBackground.picture->w) / 2;
    sy = (gui.info.screen.y - ThemeBackground.picture->h) / 2;
    blit (ThemeBackground.picture, gui_background,
        0, 0, sx, sy, ThemeBackground.picture->w, ThemeBackground.picture->h);
}


// CREATE GUI BACKGROUND ------------------------------------------------------
void    Regenerate_Background (void)
{
  #ifdef DEBUG_WHOLE
    Msg (MSGT_DEBUG, "Regenerate_Background();");
  #endif

  gui.info.must_redraw = YES;
  gui_palette_need_update = YES;

  if (ThemeBackground.picture_ok)
     {
     switch (theme_current->background_mode)
        {
        case THEME_BG_STRETCHED:
             Regenerate_Background_Stretched ();
             break;
        case THEME_BG_INT_STRETCHED:
	     Regenerate_Background_Int_Stretched ();
             break;
        case THEME_BG_TILED:
             Regenerate_Background_Tiled ();
             break;
        default:
        case THEME_BG_CENTERED:
             Regenerate_Background_Centered ();
             break;
        }
     }
  else
     {
     Regenerate_Background_Grid ();
     VMachine_Draw ();
     }
  if (Inputs.Keyboard_Enabled)
     {
         BITMAP *b = Graphics.Inputs.SK1100_Keyboard;
         draw_sprite (gui_background, b,
             (gui.info.screen.x - b->w) / 2,
             (gui.info.screen.y - b->h - 40));
     }
}

//-----------------------------------------------------------------------------

