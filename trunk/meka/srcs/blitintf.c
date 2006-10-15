//-----------------------------------------------------------------------------
// MEKA - blitintf.c
// Blitter configuration file and GUI interface - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "config_v.h"
#include "tools/libparse.h"
#include "tools/tfile.h"

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static int     Blitters_Str2Num (const char *s);

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
            return (NULL);
#elif WIN32
    if (p == NULL || stricmp(p + strlen(BLITTER_OS_SEP), BLITTER_OS_WIN) != 0)
        return (NULL);
#elif UNIX
    if (p == NULL || stricmp(p + strlen(BLITTER_OS_SEP), BLITTER_OS_UNIX) != 0)
        return (NULL);
#endif
    if (p != NULL)
        *p = EOSTR;

    // Allocate a blitter and set it with name and default values
    b = malloc(sizeof (t_blitter));
    b->name             = strdup(name);
    b->index            = Blitters.count;
    b->res_x            = 320;
    b->res_y            = 240;
    b->blitter          = BLITTER_NORMAL;
    b->driver           = GFX_AUTODETECT_FULLSCREEN;
    b->flip             = FALSE;
    b->tv_colors        = FALSE;
    b->vsync            = FALSE;
    b->refresh_rate     = 0;                                        // Default
    b->video_depth      = Configuration.video_mode_desktop_depth;   // Default
    b->stretch          = BLITTER_STRETCH_NONE; // BLITTER_STRETCH_MAX_INT;
    b->triple_buffering = FALSE;
    return (b);
}

void            Blitter_Delete (t_blitter *b)
{
    free(b->name);
    free(b);
}

//-----------------------------------------------------------------------------

void            Blitters_Close(void)
{
    list_free_custom(&Blitters.list, Blitter_Delete);
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

static int  Blitters_Parse_Line (char *s, char *s_case)
{
    char    w[256];
    int     i, line_len;

    line_len = strlen(s);
    if (s[0] == '[')
    {
        Blitters.current = Blitter_New(&s_case[1]);
        if (Blitters.current != NULL)
        {
            Blitters.count++;
            list_add_to_end(&Blitters.list, Blitters.current);
        }
        return (MEKA_ERR_OK);
    }

    // Skip line when we're inside a blitter we can ignore
    if (Blitters.current == NULL) 
        return (MEKA_ERR_OK);

    // Set attributes
    // FIXME: use libparse
    parse_getword(w, sizeof(w), &s, "=", ';', PARSE_FLAGS_NONE);
    for (i = 0; Blitters_Def_Variables [i]; i++)
        if (!strcmp (w, Blitters_Def_Variables [i]))
            break;
    if (Blitters.current == NULL)
        return (MEKA_ERR_MISSING);
    switch (i)
    {
        // Resolution
    case 0:
        {
            int x, y;
            parse_skip_spaces(&s);
            if (sscanf(s, "%dx%d", &x, &y) == 2)
            {
                Blitters.current->res_x = x;
                Blitters.current->res_y = y;
            }
            return MEKA_ERR_OK;
        }
        // Blitter
    case 1:
        Blitters.current->blitter = Blitters_Str2Num (s);
        if (Blitters.current->blitter == BLITTER_TVMODE || Blitters.current->blitter == BLITTER_TVMODE_DOUBLE)
            Blitters.current->tv_colors = TRUE;
        else
            Blitters.current->tv_colors = FALSE;
        return MEKA_ERR_OK;
        // Driver
    case 2:
        Blitters.current->driver = VideoDriver_FindByDesc(s)->drv_id;
        return MEKA_ERR_OK;
        // Flip
    case 3:
        Blitters.current->flip = TRUE;
        return MEKA_ERR_OK;
        // VSync
    case 4:
        Blitters.current->vsync = TRUE;
        return MEKA_ERR_OK;
        // Refresh Rate
    case 5:
        if (!strcmp(w, "auto"))
            Blitters.current->refresh_rate = 0;
        else
            Blitters.current->refresh_rate = atoi(s);
        return MEKA_ERR_OK;
        // Stretch
    case 6:
        Blitters.current->stretch = BLITETR_STRETCH_MAX_INT;
        return MEKA_ERR_OK;
        // Video Depth
    case 7:
        if (!strcmp(s, "auto"))
        {
            Blitters.current->video_depth = Configuration.video_mode_desktop_depth;
            return MEKA_ERR_OK;
        }
        else
        {
            const int depth = atoi(s);
            if (depth != 16 && depth != 24 && depth != 32)
                return (MEKA_ERR_VALUE_INCORRECT);
            Blitters.current->video_depth = atoi(s);
            return MEKA_ERR_OK;
        }
        return MEKA_ERR_VALUE_INCORRECT;
        // Triple Buffering
    case 8:
        Blitters.current->triple_buffering = TRUE;
        return MEKA_ERR_OK;
    default:
        return MEKA_ERR_UNKNOWN;
    }
    return MEKA_ERR_OK;
}

void    Blitters_Init_Values (void)
{
    Blitters.current = NULL;
    Blitters.blitter_configuration_name = NULL;
}

void            Blitters_Init (void)
{
    t_tfile *   tf;
    t_list *    lines;
    int         line_cnt;

    ConsolePrint (Msg_Get(MSG_Blitters_Loading));

    Blitters.list = NULL;
    Blitters.current = NULL;

    // Open and read file
    if ((tf = tfile_read (Blitters.filename)) == NULL)
        Quit_Msg (meka_strerror());
    ConsolePrint ("\n");

    // Parse each line
    line_cnt = 0;
    for (lines = tf->data_lines; lines; lines = lines->next)
    {
        char *line;
        char s1 [256], s2 [256];
        int i, j;

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
        case MEKA_ERR_UNKNOWN:          tfile_free (tf); Quit_Msg (Msg_Get (MSG_Blitters_Error_Unrecognized), line_cnt);
        case MEKA_ERR_MISSING:          tfile_free (tf); Quit_Msg (Msg_Get (MSG_Blitters_Error_Missing), line_cnt);
        case MEKA_ERR_VALUE_INCORRECT:  tfile_free (tf); Quit_Msg (Msg_Get (MSG_Blitters_Error_Incorrect_Value), line_cnt);
        }
    }

    // Free file data
    tfile_free (tf);

    // Requires at least 1 blitter
    if (Blitters.count == 0)
        Quit_Msg (Msg_Get (MSG_Blitters_Error_Not_Enough));

    // Current blitter
    if (Blitters.blitter_configuration_name != NULL)
        Blitters.current = Blitters_FindBlitterByName(Blitters.blitter_configuration_name);
    if (Blitters.current == NULL)
        Blitters.current = Blitters.list->elem; // first
}

t_blitter * Blitters_FindBlitterByName(const char *name)
{
    t_list *blitters;
    
    for (blitters = Blitters.list; blitters != NULL; blitters = blitters->next)
    {
        t_blitter *blitter = blitters->elem;
        if (stricmp(blitter->name, name) == 0)
            return (blitter);
    }
    return (NULL);
}

const static struct
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
    { BLITTER_TVMODE_DOUBLE, "tvmode_double" },
    { BLITTER_EAGLE,         "eagle"         },
    { BLITTER_HQ2X,          "hq2x"          },
    { 0, 0 }
};

static int     Blitters_Str2Num (const char *s)
{
    int   i;
    for (i = 0; Blitters_Str2Num_Table[i].name; i ++)
        if (strcmp (s, Blitters_Str2Num_Table [i].name) == 0)
            return (Blitters_Str2Num_Table [i].value);
    return (BLITTER_NORMAL);
}

void    Blitters_Switch_Common (void)
{
    if (Meka_State == MEKA_STATE_FULLSCREEN)
        Video_Setup_State ();
    Msg (MSGT_USER, Msg_Get (MSG_Blitters_Set), Blitters.current->name);
    gui_menu_un_check (menus_ID.blitters);
    gui_menu_check (menus_ID.blitters, Blitters.current->index);
}

void    Blitters_SwitchNext(void)
{
    int index;
    t_list *blitters;
  
    index = (Blitters.current->index + 1) % Blitters.count;
    for (blitters = Blitters.list; blitters != NULL; blitters = blitters->next)
        if (index-- == 0)
            break;
    assert(blitters != NULL);
    Blitters.current = blitters->elem;
    Blitters_Switch_Common();
}

static void    Blitters_Switch_Handler (t_menu_event *event)
{
    Blitters.current = (t_blitter *)event->user_data;
    Blitters_Switch_Common();
}

void    Blitters_Menu_Init (int menu_id)
{
    t_list *blitters;
    for (blitters = Blitters.list; blitters != NULL; blitters = blitters->next)
    {
        t_blitter *blitter = blitters->elem;
        menu_add_item(menu_id,
            blitter->name,
            AM_Active | ((blitter == Blitters.current) ? AM_Checked : 0),
            Blitters_Switch_Handler, blitter);
    }
}

//-----------------------------------------------------------------------------

