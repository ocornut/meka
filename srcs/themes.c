//-----------------------------------------------------------------------------
// MEKA - themes.c
// Interface Themes - Code
//-----------------------------------------------------------------------------

#include "shared.h"

// Note: for whatever reason, VC 7.0 crashed when compiling this file...
#ifdef WIN32
#pragma optimize("g", off)
#endif

//-----------------------------------------------------------------------------

void            Themes_Update_Colors (byte colors [GUI_COL_THEME_NUM][3])
{
  int           i;
  PALETTE       Pal;
  RGB *         color;

  for (i = 0; i < GUI_COL_THEME_NUM; i ++)
      {
      color = &Pal[GUI_COL_THEME_START + i];
      color->r = colors[i][0];
      color->g = colors[i][1];
      color->b = colors[i][2];
      }
  for (i = 1; i < GUI_COL_THEME_GRADIENTS_NUM; i ++)
      {
      int v;
      color = &Pal[GUI_COL_THEME_GRADIENTS_START + i - 1];
      v = colors[GUI_COL_THEME_NUM - 1][0] - (GUI_COL_THEME_GRADIENTS_POWER * i);
      color->r = ((v > 0) ? v : 0);
      v = colors[GUI_COL_THEME_NUM - 1][1] - (GUI_COL_THEME_GRADIENTS_POWER * i);
      color->g = ((v > 0) ? v : 0);
      v = colors[GUI_COL_THEME_NUM - 1][2] - (GUI_COL_THEME_GRADIENTS_POWER * i);
      color->b = ((v > 0) ? v : 0);
      }
  Palette_SetColor_Range(GUI_COL_THEME_START,
                         GUI_COL_THEME_START + GUI_COL_THEME_NUM + GUI_COL_THEME_GRADIENTS_NUM - 1,
                         Pal + GUI_COL_THEME_START);
}

// UPDATE COLOR FADE IF NECESSARY ---------------------------------------------
void    Themes_Update (void)
{
 int    i;
 int    has_changed = NO;

 if (Themes.currently_switching == 0)
    {
    return;
    }
 for (i = 0; i < (GUI_COL_THEME_NUM * 3); i ++)
     {
     if (Themes.temp[i / 3][i % 3] < theme_current->colors[i / 3][i % 3])
        {
        Themes.temp[i / 3][i % 3] ++;
        if (Themes.temp[i / 3][i % 3] < theme_current->colors[i / 3][i % 3])
           {
           Themes.temp[i / 3][i % 3] ++;
           }
        has_changed = YES;
        continue;
        }
     if (Themes.temp[i / 3][i % 3] > theme_current->colors[i / 3][i % 3])
        {
        Themes.temp[i / 3][i % 3] --;
        if (Themes.temp[i / 3][i % 3] > theme_current->colors[i / 3][i % 3])
           {
           Themes.temp[i / 3][i % 3] --;
           }
        has_changed = YES;
        continue;
        }
     }
 if (has_changed == NO)
    {
    Themes.currently_switching = NO;
    return;
    }
 Themes_Update_Colors (Themes.temp);
}

void    Themes_Background_Update (void)
{
  static char *last_loaded = NULL;

  /* if (ThemeBackground.picture)
     {
     destroy_bitmap (ThemeBackground.picture);
     ThemeBackground.picture = NULL;
     } */

  ThemeBackground.picture_ok = NO;
  if (theme_current->background_picture)
     {
     if (theme_current->background_picture == last_loaded)
        {
        ThemeBackground.picture_ok = YES;
        }
     else
        {
        int        i;
        PALETTE    pal;

        sprintf (ThemeBackground.filename, "%s/%s", Env.Paths.EmulatorDirectory, theme_current->background_picture);
	if (ThemeBackground.picture)
	  destroy_bitmap (ThemeBackground.picture);
        ThemeBackground.picture = load_bitmap (ThemeBackground.filename, pal);
        if (ThemeBackground.picture)
           {
           ThemeBackground.picture_ok = YES;
           last_loaded = theme_current->background_picture;
           // FIXME: verify that there's too many colors here
           Data_Bitmap_IncrementColor (ThemeBackground.picture, GUI_COL_AVAIL_START, -1);
           for (i = 0; i < GUI_COL_AVAIL_NUM; i ++)
               ThemeBackground.pal [i] = pal [i];
           }
        else
           {
           Msg (MSGT_USER, Msg_Get (MSG_Theme_Error_BG));
           Msg (MSGT_USER_BOX, Msg_Get (MSG_Theme_Error_BG_FileName), theme_current->background_picture);
           }
        }
     }

  Regenerate_Background ();
}

// SET CURRENT COLORS TO A SPECIFIC THEME -------------------------------------
void    Themes_Set (byte n, byte mode)
{
 int    i;

 if (n > Themes.max)
    n = 0;
 switch (mode)
    {
    case THEME_CHANGE_QUICK:
         {
         Themes.currently_switching = NO;
         Themes_Update_Colors (Themes.data[n]->colors);
         break;
         }
    case THEME_CHANGE_FADE:
         {
         Themes.currently_switching = YES;
         for (i = 0; i < (GUI_COL_THEME_NUM * 3); i ++)
             {
             Themes.temp[i / 3][i % 3] = theme_current->colors[i / 3][i % 3];
             }
         break;
         }
    }
 Themes.current = n;
 theme_current  = Themes.data[Themes.current];
 Themes.special = theme_current->special;
 gui.info.bar_gradients         = theme_current->bar_gradients;
 gui.info.menu_gradients        = theme_current->menu_gradients;
 gui.info.bar_gradients_unused  = theme_current->bar_gradients_unused;
 gui.info.menu_gradients_unused = theme_current->menu_gradients_unused;
 gui.info.bar_gradients_ratio   = theme_current->bar_gradients_ratio;
 gui.info.menu_gradients_ratio  = theme_current->menu_gradients_ratio;
 Themes_Background_Update ();
}

t_theme *       Theme_New(char *name)
{
 int            i, j;
 t_theme *      t;

 t = malloc (sizeof (t_theme));
 if (t)
    {
    int name_len = strlen (name);
    t->name = malloc (name_len /*- 1*/);
    strncpy (t->name, &name [1], name_len - 2);
    t->name [name_len - 2] = EOSTR;
    t->background_picture = 0;
    t->background_mode = THEME_BG_STRETCHED;
    t->bar_gradients = 1;
    t->bar_gradients_unused = 2;
    t->bar_gradients_ratio = 1.8;
    t->menu_gradients = 1;
    t->menu_gradients_unused = 2;
    t->menu_gradients_ratio = 1.4;
    for (i = 0; i < GUI_COL_THEME_NUM; i ++)
        for (j = 0; j < 3; j ++)
            t->colors [i] [j] = 0;
    t->special = SPECIAL_NOTHING;
    }
 return (t);
}

static char *Theme_Def_Variables [] =
   {
   "background", "text",
   "fill", "borders", "highlight", "bars",
   "bar_gradient", "menu_gradient",
   "special", "background_pic",
   NULL
   };

// ANALYSE A SINGLE LINE OF A THEME FILE --------------------------------------
int     Analyse_Line_Theme (char *s, char *s_case)
{
 int    i, j, k;
 char   s1 [256];
 char	var_name [256], var_name_case[256];
 int    start_at, num;

 if (s [0] == '[')
    {
    Themes.max ++;
    if (Themes.max == 0)
       Themes.data = malloc (sizeof (t_theme *));
    else
       Themes.data = realloc (Themes.data, (Themes.max + 1) * sizeof (t_theme *));
    Themes.data [Themes.max] = Theme_New (s_case);
    if (Themes.data [Themes.max] == 0)
       return (1);
    return (0);
    }

 if (Themes.max < 0)
    return (3);

 Get_First_Word (s, var_name, '=');
 Get_First_Word (s_case, var_name_case, '=');
 for (i = 0; Theme_Def_Variables [i]; i++)
     if (!strcmp (var_name, Theme_Def_Variables [i]))
         break;

 switch (i)
    {
    // Background
    case 0: num = 2 * 3; start_at = GUI_COL_BACKGROUND; break;
    // Text
    case 1: num = 3 * 3; start_at = GUI_COL_TEXT_ACTIVE; break;
    // Fill
    case 2: num = 1 * 3; start_at = GUI_COL_FILL; break;
    // Borders
    case 3: num = 1 * 3; start_at = GUI_COL_BORDERS; break;
    // Highlight
    case 4: num = 1 * 3; start_at = GUI_COL_HIGHLIGHT; break;
    // Bars
    case 5: num = 1 * 3; start_at = GUI_COL_BARS; break;
    // Bar_Gradient
    case 6: Get_First_Word (s, s1, ',');
            if (!StrNull (s1))
               {
               Themes.data [Themes.max]->bar_gradients = (atoi (s1) > 0) ? 1 : 0;
               }
            Get_First_Word (s, s1, ',');
            if (!StrNull (s1))
               {
               Themes.data [Themes.max]->bar_gradients_unused = atoi (s1);
               }
            Get_First_Word (s, s1, ',');
            if (!StrNull (s1))
               {
               k = atof (s1);
               if (k < 1) k = 1;
               Themes.data [Themes.max]->bar_gradients_ratio = k;
               }
            return (0);
    // Menu_Gradient
    case 7: Get_First_Word (s, s1, ',');
            if (!StrNull (s1))
               {
               Themes.data [Themes.max]->menu_gradients = (atoi (s1) > 0) ? 1 : 0;
               }
            Get_First_Word (s, s1, ',');
            if (!StrNull (s1))
               {
               Themes.data [Themes.max]->menu_gradients_unused = atoi (s1);
               }
            Get_First_Word (s, s1, ',');
            if (!StrNull (s1))
               {
               k = atof (s1);
               if (k < 1) k = 1;
               Themes.data [Themes.max]->menu_gradients_ratio = k;
               }
            return (0);
    // Special
    case 8: if (strcmp (s, "blood") == 0)
               Themes.data [Themes.max]->special = SPECIAL_BLOOD;
            else
            if (strcmp (s, "hearts") == 0)
               Themes.data [Themes.max]->special = SPECIAL_HEARTS;
            else
            if (strcmp (s, "snow") == 0)
               Themes.data [Themes.max]->special = SPECIAL_SNOW;
            return (0);
    // Background_Pic
    case 9: Get_First_Word (s_case, s1, ',');
            Themes.data [Themes.max]->background_picture = strdup (s1);
            Get_First_Word (s_case, s1, ',');
            if (!StrNull (s1))
               {
               Themes.data [Themes.max]->background_mode = atoi (s1);
               }
            return (0);
    // Unrecognized Variable
    default: return (2);
    }
 if (num > 0)
    {
    j = 0;
    start_at -= GUI_COL_THEME_START;
    while (1)
       {
       Get_First_Word (s, s1, ',');
       if (StrNull (s1)) break;
       k = (atoi (s1) < 64) ? atoi (s1) : 63;
       Themes.data [Themes.max]->colors [(j / 3) + start_at] [j % 3] = k;
       if (++j >= num) break;
       }
    }
 return (0);
}

// INITIALIZE DEFAULT THEMES --------------------------------------------------
void            Load_Theme_List (void)
{
 int            i, j;
 char           s1 [256], s2 [256];

 t_tfile        *tf;
 t_list         *lines;
 char           *line;
 int            line_cnt;

 ConsolePrint (Msg_Get (MSG_Theme_Loading));

 Themes.max = -1;
 Themes.currently_switching = 0;

 // Open and read file --------------------------------------------------------
 tf = tfile_read (Themes.filename);
 if (tf == NULL)
    Quit_Msg (meka_strerror());

 // Ok
 ConsolePrint ("\n");

 // Parse each line -----------------------------------------------------------
 line_cnt = 0;
 for (lines = tf->data_lines; lines; lines = lines->next)
    {
    line_cnt += 1;
    line = lines->elem;

    // FIXME
    for (i = 0, j = 0; i < 256; i ++)
        {
        if ((line [i] == ';') || (line [i] == EOSTR))
           {
           s2 [j] = EOSTR;
           break;
           }
        if ((line [0] == '[') || (line [i] != ' ' && line [i] != '\t'))
           {
           s2 [j ++] = line [i];
           }
        }
    if (StrNull (s2))
       {
       continue;
       }
    strcpy (s1, s2);
    strlwr (s1);

    switch (Analyse_Line_Theme (s1, s2))
      {
      case 1: tfile_free(tf); Quit_Msg (Msg_Get (MSG_Error_Memory));
      case 2: tfile_free(tf); Quit_Msg (Msg_Get (MSG_Theme_Unrecognized), line_cnt);
      case 3: tfile_free(tf); Quit_Msg (Msg_Get (MSG_Theme_Missing), line_cnt);
      }
    }

 // Free file data ------------------------------------------------------------
 tfile_free(tf);

 if (Themes.max == -1)
    {
    Quit_Msg (Msg_Get (MSG_Theme_Not_Enough));
    }
 if (Themes.current > Themes.max)
    Themes.current = 0;
 theme_current = Themes.data[Themes.current];
}

// ACTION: CHANGE CURRENT THEME -----------------------------------------------
void    Action_Theme (byte n)
{
  Themes_Set (n, THEME_CHANGE_FADE);
  gui_menu_un_check (menus_ID.themes);
  gui_menu_check (menus_ID.themes, n);
}

void    Init_Themes_Menu (int menu_id)
{
  int   i;

  for (i = 0; i <= Themes.max; i ++) // ? <=
      {
      menu_add_item (menu_id,
                     Themes.data[i]->name,
                     AM_Active | ((Themes.current == i) ? AM_Checked : 0),
                     (void *)Action_Theme);
      }
}

void    Themes_Init_Values (void)
{
  Themes.current = 0;
  ThemeBackground.picture = 0;
  ThemeBackground.picture_ok = 0;
}

//-----------------------------------------------------------------------------

