//-----------------------------------------------------------------------------
// MEKA - blitintf.c
// Blitter configuration file and GUI interface - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "video.h"
#include "libparse.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_blitters Blitters;

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static int     Blitters_Str2Num(const char *s);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Note: 'name' not const because the string is actually overwritten (could be done without)
t_blitter *     Blitter_New(char *name)
{
    // Look for system identifier
    char * p = name + strlen(name) - 1;
    if (*p == ']')
        *p = EOSTR;
    p = strstr(name, BLITTER_OS_SEP);

    // Ignore blitter if it is not for current system
#ifdef ARCH_WIN32
    if (p == NULL || stricmp(p + strlen(BLITTER_OS_SEP), BLITTER_OS_WIN) != 0)
        return (NULL);
#elif ARCH_UNIX
    if (p == NULL || stricmp(p + strlen(BLITTER_OS_SEP), BLITTER_OS_UNIX) != 0)
        return (NULL);
#endif
    if (p != NULL)
        *p = EOSTR;

    // Allocate a blitter and set it with name and default values
    t_blitter * b = (t_blitter*)malloc(sizeof (t_blitter));
    b->name             = strdup(name);
    b->index            = Blitters.count;
    b->res_x            = 320;
    b->res_y            = 240;
    b->blitter          = BLITTER_NORMAL;
    b->tv_colors        = FALSE;
    b->refresh_rate     = 0;                                        // Default
    b->stretch          = BLITTER_STRETCH_NONE; // BLITTER_STRETCH_MAX_INT;
    return (b);
}

void Blitter_Delete(t_blitter *b)
{
    free(b->name);
    free(b);
}

//-----------------------------------------------------------------------------

void Blitters_Close()
{
    list_free_custom(&Blitters.list, (t_list_free_handler)Blitter_Delete);
}

static const char * Blitters_Def_Variables[] =
{
    "res",
    "blitter",
    "refresh_rate",
    "stretch",
    NULL
};

static int  Blitters_Parse_Line(char *s, char *s_case)
{
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
    char w[256];
    parse_getword(w, sizeof(w), &s, "=", ';', PARSE_FLAGS_NONE);
	int i;
    for (i = 0; Blitters_Def_Variables [i]; i++)
        if (!strcmp(w, Blitters_Def_Variables [i]))
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
    case 2:
        if (!strcmp(w, "auto"))
            Blitters.current->refresh_rate = 0;
        else
            Blitters.current->refresh_rate = atoi(s);
        return MEKA_ERR_OK;
        // Stretch
    case 3:
        Blitters.current->stretch = BLITTER_STRETCH_MAX_INT;
        return MEKA_ERR_OK;
    }
    return MEKA_ERR_UNKNOWN;
}

void    Blitters_Init_Values()
{
    Blitters.current = NULL;
    Blitters.blitter_configuration_name = NULL;
}

void	Blitters_Init()
{
    ConsolePrint(Msg_Get(MSG_Blitters_Loading));

    Blitters.list = NULL;
    Blitters.current = NULL;

    // Open and read file
    t_tfile * tf;
    if ((tf = tfile_read (Blitters.filename)) == NULL)
        Quit_Msg("%s", meka_strerror());
    ConsolePrint("\n");

    // Parse each line
    int line_cnt = 0;
    for (t_list* lines = tf->data_lines; lines; lines = lines->next)
    {
        const char* line = (char*)lines->elem;
        line_cnt += 1;

		int i, j;
        char s1 [256], s2 [256];
        for (i = 0, j = 0; line [i] != 0 && line [i] != ';'; i ++)
            if ((line [0] == '[') || (line [i] != ' ' && line [i] != '\t'))
                s2 [j ++] = line [i];
        s2 [j] = 0;
        if (StrIsNull (s2))
            continue;

        strcpy(s1, s2);
        StrLower(s1);

        switch (Blitters_Parse_Line (s1, s2))
        {
        case MEKA_ERR_UNKNOWN:          tfile_free(tf); Quit_Msg(Msg_Get(MSG_Blitters_Error_Unrecognized), line_cnt);
        case MEKA_ERR_MISSING:          tfile_free(tf); Quit_Msg(Msg_Get(MSG_Blitters_Error_Missing), line_cnt);
        case MEKA_ERR_VALUE_INCORRECT:  tfile_free(tf); Quit_Msg(Msg_Get(MSG_Blitters_Error_Incorrect_Value), line_cnt);
        }
    }

    // Free file data
    tfile_free(tf);

    // Requires at least 1 blitter
    if (Blitters.count == 0)
        Quit_Msg("%s", Msg_Get(MSG_Blitters_Error_Not_Enough));

    // Current blitter
    if (Blitters.blitter_configuration_name != NULL)
        Blitters.current = Blitters_FindBlitterByName(Blitters.blitter_configuration_name);
    if (Blitters.current == NULL)
        Blitters.current = (t_blitter*)Blitters.list->elem; // first
}

t_blitter * Blitters_FindBlitterByName(const char *name)
{
    for (t_list* blitters = Blitters.list; blitters != NULL; blitters = blitters->next)
    {
        t_blitter* blitter = (t_blitter*)blitters->elem;
        if (stricmp(blitter->name, name) == 0)
            return (blitter);
    }
    return (NULL);
}

const static struct
{
    int value;
    const char *name;
} Blitters_Str2Num_Table [] =
{
    { BLITTER_NORMAL,        "normal"        },
    { BLITTER_TVMODE,        "tv"            },
    { BLITTER_TVMODE,        "tvmode"        },
    { BLITTER_TVMODE_DOUBLE, "tvmode_double" },
    { BLITTER_EAGLE,         "eagle"         },
    { BLITTER_HQ2X,          "hq2x"          },
    { 0, 0 }
};

static int     Blitters_Str2Num(const char *s)
{
    for (int i = 0; Blitters_Str2Num_Table[i].name; i ++)
        if (strcmp(s, Blitters_Str2Num_Table[i].name) == 0)
            return (Blitters_Str2Num_Table[i].value);
    return (BLITTER_NORMAL);
}

void    Blitters_Switch_Common (void)
{
    if (g_env.state == MEKA_STATE_GAME)
        Video_Setup_State();
    Msg(MSGT_USER, Msg_Get(MSG_Blitters_Set), Blitters.current->name);
    gui_menu_uncheck_all (menus_ID.blitters);
    gui_menu_check (menus_ID.blitters, Blitters.current->index);
}

void    Blitters_SwitchNext(void)
{
    int index = (Blitters.current->index + 1) % Blitters.count;
	t_list* blitters;
    for (blitters = Blitters.list; blitters != NULL; blitters = blitters->next)
        if (index-- == 0)
            break;
    assert(blitters != NULL);
    Blitters.current = (t_blitter*)blitters->elem;
    Blitters_Switch_Common();
}

static void    Blitters_Switch_Handler (t_menu_event *event)
{
    Blitters.current = (t_blitter *)event->user_data;
    Blitters_Switch_Common();
}

void    Blitters_Menu_Init (int menu_id)
{
    for (t_list* blitters = Blitters.list; blitters != NULL; blitters = blitters->next)
    {
        t_blitter* blitter = (t_blitter*)blitters->elem;
        menu_add_item(menu_id,
            blitter->name,
			NULL,
            MENU_ITEM_FLAG_ACTIVE | ((blitter == Blitters.current) ? MENU_ITEM_FLAG_CHECKED : 0),
            (t_menu_callback)Blitters_Switch_Handler, blitter);
    }
}

//-----------------------------------------------------------------------------

