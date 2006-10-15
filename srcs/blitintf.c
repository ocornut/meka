//-----------------------------------------------------------------------------
// MEKA - blitintf.c
// Blitter configuration file and GUI interface - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "config_v.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_blitter *     Blitter_New (char *name)
{
    t_blitter * b;
    char *      p;

    // Look for system identifier
    p = name + strlen(name) - 1;
    if (*p == ']')
        *p = EOSTR;
    p = strstr(name, BLITTER_OS_SEP);

    // Ignore blitter if it is not for current system
#ifdef DOS
    if (p != NULL)
        if (stricmp(p + strlen(BLITTER_OS_SEP), BLITTER_OS_DOS) != 0)
            return (BLITTER_IGNORE);
#elif WIN32
    if (p == NULL || stricmp(p + strlen(BLITTER_OS_SEP), BLITTER_OS_WIN) != 0)
        return (BLITTER_IGNORE);
#elif UNIX
    if (p == NULL || stricmp(p + strlen(BLITTER_OS_SEP), BLITTER_OS_UNIX) != 0)
        return (BLITTER_IGNORE);
#endif
    if (p != NULL)
        *p = EOSTR;

    // Allocate a blitter and set it with name and default values
    b = malloc(sizeof (t_blitter));
    b->name             = strdup(name);
    b->res_x            = 320;
    b->res_y            = 240;
    b->blitter          = BLITTER_NORMAL;
    b->driver           = GFX_AUTODETECT_FULLSCREEN;
    b->flip             = FALSE;
    b->tv_colors        = FALSE;
    b->vsync            = FALSE;
    b->refresh_rate     = 0; // Default
    b->video_depth      = 8;
    b->stretch          = FALSE;// BLITTER_STRETCH_MAX_INT;
    b->triple_buffering = FALSE;
    return (b);
}

void            Blitter_Delete (t_blitter *b)
{
    free(b->name);
    free(b);
}

//-----------------------------------------------------------------------------

void            Blitters_Free (void)
{
    list_free_custom (&blitters.list, Blitter_Delete);
}

// FIXME: Crap
void            Blitters_Current_Update (void)
{
    int         cnt;
    t_list *    list;

    list = blitters.list;
    cnt = blitters.current_num;
    while (list && cnt -- > 0)
        list = list->next;
    if (list == 0)
        Quit_Msg (Msg_Get (MSG_Blitters_Not_Found));
    blitters.current = list->elem;
    if (blitters.current->blitter == BLITTER_TVMODE
        || blitters.current->blitter == BLITTER_TVMODE_DOUBLE)
        blitters.current->tv_colors = YES;
    else
        blitters.current->tv_colors = NO;
}

static const char * Blitters_Def_Variables [] =
{
    "res",
    "blitter",
    "driver",
    "flip",
    "vsync",
    "refresh_rate",
    "stretch",
    "video_depth",
    "triple_buffering",
    NULL
};

int         Blitters_Parse_Line (char *s, char *s_case)
{
    char    w[256];
    int     i, line_len;

    line_len = strlen(s);
    if (s[0] == '[')
    {
        blitters.current = Blitter_New(&s_case[1]);
        if (blitters.current != BLITTER_IGNORE)
        {
            blitters.num += 1;
            list_add(&blitters.list, blitters.current);
        }
        return (0);
    }

    // Skip line when we're inside a blitter we can ignore
    if (blitters.current == BLITTER_IGNORE) 
        return (0);

    // Set attributes
    Get_First_Word (s, w, '=');
    for (i = 0; Blitters_Def_Variables [i]; i++)
        if (!strcmp (w, Blitters_Def_Variables [i]))
            break;
    if (blitters.num == 0)
        return (2);
    switch (i)
    {
        // Resolution
    case 0:
        Get_First_Word (s, w, 'x');
        blitters.current->res_x = atoi (w);
        blitters.current->res_y = atoi (s);
        break;
        // Blitter
    case 1:
        blitters.current->blitter = Blitters_Str2Num (s);
        break;
        // Driver
    case 2:
        blitters.current->driver = VideoDriver_FindByDesc(s)->drv_id;
        break;
        // Flip
    case 3:
        blitters.current->flip = TRUE;
        break;
        // VSync
    case 4:
        blitters.current->vsync = TRUE;
        break;
        // Refresh Rate
    case 5:
        if (!strcmp(w, "auto"))
            blitters.current->refresh_rate = 0;
        else
            blitters.current->refresh_rate = atoi(s);
        break;
        // Stretch
    case 6:
        blitters.current->stretch = TRUE;
        break;
        // Video Depth
    case 7:
        blitters.current->video_depth = atoi(s);
        break;
        // Triple Buffering
    case 8:
        blitters.current->triple_buffering = TRUE;
        break;
    default:
        return (1);
    }
    return (0);
}

/*
void    BlittersPrint (void)
{
 t_list *l;
 t_blitter *b;
 l = blitters.list;

 while (l)
   {
   b = l->elem;
   ConsolePrintf ("--\nNAME=%s\nX=%d,Y=%d\nDRIVER=%d\nDRIVER_WIN=%d\nBLITTER=%d\nFLIP=%d\n",
                   b->name, b->res_x, b->res_y, b->driver, b->driver_win, b->blitter, b->flip);
   l = l->next;
   }
}
*/

void            Blitters_Load (void)
{
  int            i, j;
  char           s1 [256], s2 [256];

  t_tfile        *tf;
  t_list         *lines;
  char           *line;
  int            line_cnt;

  ConsolePrint (Msg_Get (MSG_Blitters_Loading));

  blitters.list = 0;
  blitters.current = 0;

  // Open and read file --------------------------------------------------------
  if ((tf = tfile_read (blitters.filename)) == NULL)
     Quit_Msg (meka_strerror());

  // Ok
  ConsolePrint ("\n");

  // Parse each line -----------------------------------------------------------
  line_cnt = 0;
  for (lines = tf->data_lines; lines; lines = lines->next)
     {
     line_cnt += 1;
     line = lines->elem;

     for (i = 0, j = 0; line [i] != 0 && line [i] != ';'; i ++)
         if ((line [0] == '[') || (line [i] != ' ' && line [i] != '\t'))
            s2 [j ++] = line [i];
     s2 [j] = 0;
     if (StrNull (s2))
        continue;

     strcpy (s1, s2);
     strlwr (s1);

        switch (Blitters_Parse_Line (s1, s2))
        {
        case 1: tfile_free (tf); Quit_Msg (Msg_Get (MSG_Blitters_Unrecognized), line_cnt);
        case 2: tfile_free (tf); Quit_Msg (Msg_Get (MSG_Blitters_Missing), line_cnt);
        }
     }

  // Free file data ------------------------------------------------------------
  tfile_free (tf);

  blitters.num = list_size (blitters.list);
  if (blitters.num == 0)
     Quit_Msg (Msg_Get (MSG_Blitters_Not_Enough));
  if (blitters.current_num >= blitters.num)
     blitters.current_num = 0;

  list_reverse (&blitters.list);
  Blitters_Current_Update ();
  /* Blitters_Aff_All (); */
}

void    Blitters_Init_Values (void)
{
    blitters.current_num = 0;
}

static struct
{
  int value;
  char *name;
} Blitters_Str2Num_Table [] =
{
    { BLITTER_NORMAL,        "normal"        },
    { BLITTER_DOUBLE,        "double"        },
    { BLITTER_SCANLINES,     "scanlines"     },
    { BLITTER_SCANLINES,     "scanline"      },
    { BLITTER_TVMODE,        "tv"            },
    { BLITTER_TVMODE,        "tvmode"        },
    { BLITTER_TVMODE,        "joseph"        },
    { BLITTER_PARALLEL,      "parallel"      },
    { BLITTER_TVMODE_DOUBLE, "tvmode_double" },
    { BLITTER_EAGLE,         "eagle"         },
    { BLITTER_HQ2X,          "hq2x"          },
    { 0, 0 }
};

int     Blitters_Str2Num (char *s)
{
    int   i;
    for (i = 0; Blitters_Str2Num_Table[i].name; i ++)
        if (strcmp (s, Blitters_Str2Num_Table [i].name) == 0)
            return (Blitters_Str2Num_Table [i].value);
    return (BLITTER_NORMAL);
}

void    Blitters_Switch_Common (void)
{
    Blitters_Current_Update ();
    if (Meka_State == MEKA_STATE_FULLSCREEN)
        Video_Setup_State ();
    Msg (MSGT_USER, Msg_Get (MSG_Blitters_Set), blitters.current->name);
    gui_menu_un_check (menus_ID.blitters);
    gui_menu_check (menus_ID.blitters, blitters.current_num);
}

void    Blitters_Switch (void)
{
    blitters.current_num = (blitters.current_num + 1) % blitters.num;
    Blitters_Switch_Common();
}

void    Blitters_Switch_Menu (int n)
{
    blitters.current_num = n;
    Blitters_Switch_Common();
}

void    Blitters_Menu_Init (int menu_id)
{
    t_list *l = blitters.list;
    t_blitter *b;

    while (l)
    {
        b = l->elem;
        menu_add_item (menu_id,
            b->name,
            AM_Active | ((b == blitters.current) ? AM_Checked : 0),
            Blitters_Switch_Menu);
        l = l->next;
    }
}

//-----------------------------------------------------------------------------

