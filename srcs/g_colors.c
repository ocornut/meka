//-----------------------------------------------------------------------------
// MEKA - g_colors.c
// GUI Color Management & Setting - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

// This palette is generated from the ALL-V?.PCX file
static byte GUI_Misc_Colors [GUI_COL_MISC_NUM * 3] =
{
 0, 0, 0,
 20, 20, 20,
 30, 30, 30,
 48, 48, 48,
 64, 64, 64,
 96, 96, 96,
 129, 129, 129,
 170, 170, 170,
 178, 182, 178,
 216, 216, 200,
 255, 255, 255,
 64, 0, 0,
 194, 0, 0,
 255, 0, 0,
 64, 144, 16,
 255, 223, 0,
 0, 64, 129,
 0, 129, 194,
 0, 194, 194,
 200, 200, 184,
 // Keyboard colors
 4, 44, 4,
 28, 28, 36,
 36, 36, 52,
 52, 52, 68,
 60, 60, 60,
 60, 68, 100,
 84, 92, 140,
 84, 100, 132,
 100, 108, 156,
 156, 156, 4,
 196, 188, 4,
 220, 220, 100,
 252, 236, 4,
 228, 236, 188,
 60, 60, 84,
 60, 60, 60,
 220, 220, 220,
 255, 255, 0,
 20, 20, 120,
};

//-----------------------------------------------------------------------------

// INITIALIZE DEFAULT GUI COLORS ----------------------------------------------
void    gui_init_colors (void)
{
  int   i;

  // Set basic colors ---------------------------------------------------------
  // (including Black [0] and White [9])
  for (i = 0; i < GUI_COL_MISC_NUM; i ++)
     {
     RGB color;
     color.r = GUI_Misc_Colors [i * 3 + 0] / 4;
     color.g = GUI_Misc_Colors [i * 3 + 1] / 4;
     color.b = GUI_Misc_Colors [i * 3 + 2] / 4;
     Palette_SetColor (GUI_COL_START + i, color);
     }

  // Set theme colors ---------------------------------------------------------
  Themes_Set (Themes.current, THEME_CHANGE_QUICK);
  // Set TV colors ------------------------------------------------------------
  // Effects_TV_Init_Colors (); // FIXME: use GUI palette
  // Set dirty flag for Virtual Machine colors --------------------------------
  gui_palette_need_update = YES;
}

void    gui_palette_update (void)
{
  gui_palette_need_update = NO;
  if (ThemeBackground.picture_ok /* theme_current->background_picture */)
     {
     Palette_SetColor_Range (GUI_COL_AVAIL_START,
                             GUI_COL_AVAIL_START + GUI_COL_AVAIL_NUM - 1,
                             ThemeBackground.pal);
     }
  else
     {
     VMachine_Init_Colors ();
     }
}

//-----------------------------------------------------------------------------

