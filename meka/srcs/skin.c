//-----------------------------------------------------------------------------
// MEKA - skin.c
// Interface Skins - Code
//-----------------------------------------------------------------------------
// Note: 'skins' referred as 'themes' to user.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "skin_bg.h"
#include "libparse.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define SKIN_FADE_SPEED_DEFAULT     (1.0f / 40.0f)      // Takes 40 frames (2/3 of a second) to switch

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_skin_manager
{
    t_list *            skins;
    t_skin *            skin_current;
    bool                skin_fade;
    float               skin_fade_pos;
    float               skin_fade_speed;
    t_skin *            skin_fade_from;
    char *              skin_configuration_name;
    ALLEGRO_BITMAP *    background_picture;
    bool                quit_after_fade;
};

static t_skin_manager   Skins;

struct t_skin_color
{
    const char *        identifier;
    const char *        default_identifier;
};

// Table associating skin color definitions to name
const t_skin_color      SkinColorData[SKIN_COLOR_MAX_] =
{
    { "background_color",                       NULL                            }, // 0
    { "background_grid_color",                  "background_color"              },
    { "window_border_color",                    NULL                            },
    { "window_background_color",                NULL                            },
    { "window_titlebar_color",                  NULL                            },
    { "window_titlebar_text_color",             NULL                            },
    { "window_titlebar_text_unactive_color",    NULL                            },
    { "window_text_color",                      NULL                            },
    { "window_text_highlight_color",            NULL                            },
    { "window_separators_color",                "window_border_color"           },
    { "menu_background_color",                  NULL                            }, // 10
    { "menu_border_color",                      NULL                            },
    { "menu_selection_color",                   NULL                            },
    { "menu_text_color",                        NULL                            },
    { "menu_text_unactive_color",               NULL                            },
    { "widget_generic_background_color",        "menu_background_color"         },
    { "widget_generic_selection_color",         "menu_selection_color"          },
    { "widget_generic_border_color",            "window_border_color"           },
    { "widget_generic_text_color",              "window_text_highlight_color"   },
    { "widget_generic_text_unactive_color",     "window_text_color"             },
    { "widget_listbox_background_color",        "menu_background_color"         }, // 20
    { "widget_listbox_border_color",            "menu_border_color"             },
    { "widget_listbox_selection_color",         "menu_selection_color"          },
    { "widget_listbox_text_color",              "menu_text_color"               },
    { "widget_scrollbar_background_color",      "window_background_color"       },
    { "widget_scrollbar_scroller_color",        "menu_selection_color"          },
    { "widget_statusbar_background_color",      "menu_background_color"         },
    { "widget_statusbar_border_color",          "menu_border_color"             },
    { "widget_statusbar_text_color",            "menu_text_color"               },
};

ALLEGRO_COLOR       SkinCurrent_NativeColorTable[SKIN_COLOR_MAX_];

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static t_skin *     Skin_New(const char *name);
static void         Skin_Delete(t_skin *skin);
static bool         Skin_IsValid(t_skin *skin);

static void         Skins_UpdateNativeColorTable(void);

//-----------------------------------------------------------------------------
// SKIN - Functions
//-----------------------------------------------------------------------------

t_skin *    Skin_New(const char *name)
{
    t_skin *skin;
    int i;

    // Check parameters
    assert(name != NULL);

    // Create and setup empty skin
    skin = (t_skin*)malloc(sizeof(t_skin));
    skin->enabled = FALSE;
    skin->name = strdup(name);
    skin->authors = NULL;
    for (i = 0; i < SKIN_COLOR_MAX_; i++)
    {
        skin->colors[i] = 0xFF00FF;
        skin->colors_defined[i] = FALSE;
    }
    skin->effect = SKIN_EFFECT_NONE;

    // Setup empty gradients
    skin->gradient_menu.enabled = FALSE;
    skin->gradient_menu.color_start = skin->gradient_menu.color_end = 0xFF00FF;
    skin->gradient_menu.pos_start = skin->gradient_menu.pos_end = 100;
    skin->gradient_window_titlebar.enabled = FALSE;
    skin->gradient_window_titlebar.color_start = skin->gradient_window_titlebar.color_end = 0xFF00FF;
    skin->gradient_window_titlebar.pos_start = skin->gradient_window_titlebar.pos_end = 100;

    // Setup background picture
    skin->background_picture = NULL;
    skin->background_picture_mode = SKIN_BACKGROUND_PICTURE_MODE_DEFAULT;

    return (skin);
}

void        Skin_Delete(t_skin *skin)
{
    // Check parameters
    assert(skin != NULL);

    // Delete members
    free(skin->name);
    if (skin->authors)
        free(skin->authors);
    if (skin->background_picture)
        free(skin->background_picture);

    // Delete
    free(skin);
}

bool        Skin_IsValid(t_skin *skin)
{
    int     i;
    for (i = 0; i < SKIN_COLOR_MAX_; i++)
    {
        if (!skin->colors_defined)
            return false;
    }
    return true;
}

bool        Skin_PostProcess(t_skin *skin)
{
    bool valid = TRUE;
    int i;

    // Apply default colors
    for (i = 0; i < SKIN_COLOR_MAX_; i++)
    {
        if (!skin->colors_defined[i])
        {
            if (SkinColorData[i].default_identifier != NULL)
            {
                // FIXME: Find index from identifier.. Ahem...
                int identifier_idx;
                for (identifier_idx = 0; identifier_idx < SKIN_COLOR_MAX_; identifier_idx++)
                    if (!strcmp(SkinColorData[identifier_idx].identifier, SkinColorData[i].default_identifier))
                    {
                        skin->colors[i] = skin->colors[identifier_idx];
                        skin->colors_defined[i] = skin->colors_defined[identifier_idx];
                        break;
                    }
            }
            if (!skin->colors_defined[i])
                valid = FALSE;
        }
    }

    // Apply gradient colors
    skin->gradient_window_titlebar.color_start = skin->colors[COLOR_SKIN_INDEX(COLOR_SKIN_WINDOW_TITLEBAR)];
    skin->gradient_menu.color_start            = skin->colors[COLOR_SKIN_INDEX(COLOR_SKIN_MENU_BACKGROUND)];
    if (!skin->gradient_window_titlebar.enabled)
        skin->gradient_window_titlebar.color_end = skin->gradient_window_titlebar.color_start;
    if (!skin->gradient_menu.enabled)
        skin->gradient_menu.color_end = skin->gradient_menu.color_start;

    return (valid);
}

//-----------------------------------------------------------------------------
// SKINS - Functions
//-----------------------------------------------------------------------------

void        Skins_Init_Values()
{
    Skins.skin_configuration_name = NULL;
}

//-----------------------------------------------------------------------------
// Skins_Init(void)
// Initialize skin system
//-----------------------------------------------------------------------------
void        Skins_Init(void)
{
    t_skin *skin_first_valid = NULL;

    Skins.skins                 = NULL;
    Skins.skin_current          = NULL;    // To be set before running program
    Skins.skin_fade             = FALSE;
    Skins.skin_fade_pos         = 0.0f;
    Skins.skin_fade_speed       = SKIN_FADE_SPEED_DEFAULT;
    Skins.skin_fade_from        = NULL;
    Skins.background_picture    = NULL;
    Skins.quit_after_fade       = FALSE;

    // Load skins from MEKA.THM file
    Skins_Load(g_env.Paths.SkinFile);

    // Post process skins and verify skin validity (all colors sets, etc)
    {
        t_list *skins;
        for (skins = Skins.skins; skins != NULL; skins = skins->next)
        {
            t_skin* skin = (t_skin*)skins->elem;
            Skin_PostProcess(skin);
            if (Skin_IsValid(skin))
            {
                skin->enabled = TRUE;
                if (!skin_first_valid)
                    skin_first_valid = skin;
            }
            else
            {
                skin->enabled = FALSE;
                ConsolePrintf(Msg_Get(MSG_Theme_Error_Theme_Missing_Data), skin->name);
                ConsoleEnablePause();
            }
        }
    }

    // Verify that we have at least 1 loaded skin
    if (skin_first_valid == NULL)
        Quit_Msg("%s", Msg_Get(MSG_Theme_Error_Not_Enough));

    // Set current skin
    Skins.skin_current = NULL;
    if (Skins.skin_configuration_name != NULL)
        Skins.skin_current = Skins_FindSkinByName(Skins.skin_configuration_name);
    if (Skins.skin_current == NULL)
        Skins.skin_current = skin_first_valid;
}

static int  Skins_ParseGradient(char *line, t_skin_gradient *gradient)
{
    u32 color_end;
    int pos_start, pos_end;
    char value[256];

    if (gradient->enabled)
        return MEKA_ERR_ALREADY_DEFINED;
    if (!parse_getword(value, sizeof(value), &line, "", ';', PARSE_FLAGS_NONE))
        return MEKA_ERR_SYNTAX;
    if (sscanf(value, "%d%%-%d%% #%06x", &pos_start, &pos_end, &color_end) != 3)
        return MEKA_ERR_SYNTAX;
    if (pos_start < 0 || pos_start > 100 || pos_end < 0 || pos_end > 100 || pos_end < pos_start)
        return MEKA_ERR_VALUE_OUT_OF_BOUND;
    gradient->enabled       = TRUE;
    gradient->color_end     = color_end;
    gradient->pos_start     = pos_start;
    gradient->pos_end       = pos_end;
    return MEKA_ERR_OK;
}

static int  Skins_ParseLine(char *line)
{
    if (line[0] == '[')
    {
		line++;

		// Get name
		char skin_name[256];
        if (!parse_getword(skin_name, sizeof(skin_name), &line, "]", ';', PARSE_FLAGS_DONT_EAT_SEPARATORS))
            return MEKA_ERR_SYNTAX;
        if (*line != ']')
            return MEKA_ERR_SYNTAX;

		// Create new skin
		t_skin* skin = Skin_New(skin_name);
        list_add(&Skins.skins, skin);
        Skins.skin_current = skin;
        return MEKA_ERR_OK;
    }
    else
    {
        t_skin *skin = Skins.skin_current;

        // Read line
		char var[256];
		char value[256];
        if (!parse_getword(var, sizeof(var), &line, "=", ';', PARSE_FLAGS_NONE))
            return MEKA_ERR_OK;
        parse_skip_spaces(&line);

		if (!skin)
			return MEKA_ERR_MISSING;

        // Check if it's a skin color
		int i;
        for (i = 0; i < SKIN_COLOR_MAX_; i++)
            if (strcmp(var, SkinColorData[i].identifier) == 0)
                break;
        if (i != SKIN_COLOR_MAX_)
        {
            // Got a color attributes
            u32 color;
            if (skin->colors_defined[i])
                return MEKA_ERR_ALREADY_DEFINED;
            if (!parse_getword(value, sizeof(value), &line, "", ';', PARSE_FLAGS_NONE))
                return MEKA_ERR_SYNTAX;
            if (sscanf(value, "#%06x", &color) != 1)
                return MEKA_ERR_SYNTAX;
            skin->colors[i] = color;
            skin->colors_defined[i] = TRUE;
            return MEKA_ERR_OK;
        }

        // Another type of attribute, check them manually

        // - Authors
        if (strcmp(var, "authors") == 0)
        {
            if (skin->authors != NULL)
                return MEKA_ERR_ALREADY_DEFINED;
            if (!parse_getword(value, sizeof(value), &line, "", ';', PARSE_FLAGS_NONE))
                return MEKA_ERR_SYNTAX;
            skin->authors = strdup(value);
            return MEKA_ERR_OK;
        }

        // - Comments
        if (strcmp(var, "comments") == 0)
        {
            // Currently ignored
            return MEKA_ERR_OK;
        }

        // - Background Picture
        if (strcmp(var, "background_picture") == 0)
        {
            if (skin->background_picture != NULL)
                return MEKA_ERR_ALREADY_DEFINED;
            line = strchr(line, '\"');
            if (!line)
                return MEKA_ERR_SYNTAX;
            line++;
            if (!parse_getword(value, sizeof(value), &line, "\"", ';', PARSE_FLAGS_NONE))
                return MEKA_ERR_SYNTAX;
            line++;
            skin->background_picture = strdup(value);
            if (parse_getword(value, sizeof(value), &line, " \t", ';', PARSE_FLAGS_NONE))
            {
                if (!strcmp(value, "center"))
                    skin->background_picture_mode = SKIN_BACKGROUND_PICTURE_MODE_CENTER;
                else if (!strcmp(value, "stretch"))
                    skin->background_picture_mode = SKIN_BACKGROUND_PICTURE_MODE_STRETCH;
                else if (!strcmp(value, "stretch_int"))
                    skin->background_picture_mode = SKIN_BACKGROUND_PICTURE_MODE_STRETCH_INT;
                else if (!strcmp(value, "tile"))
                    skin->background_picture_mode = SKIN_BACKGROUND_PICTURE_MODE_TILE;
                else
                    return MEKA_ERR_SYNTAX;
            }
            return MEKA_ERR_OK;
        }

        // - Window TitleBar Gradient
        if (strcmp(var, "window_titlebar_gradient") == 0)
            return Skins_ParseGradient(line, &skin->gradient_window_titlebar);

        // - Menu Gradient
        if (strcmp(var, "menu_gradient") == 0)
            return Skins_ParseGradient(line, &skin->gradient_menu);

        // - Effect
        if (strcmp(var, "effect") == 0)
        {
            if (skin->effect != SKIN_EFFECT_NONE)
                return MEKA_ERR_ALREADY_DEFINED;
            if (!parse_getword(value, sizeof(value), &line, "", ';', PARSE_FLAGS_NONE))
                return MEKA_ERR_SYNTAX;
            if (!strcmp(value, "none"))
                skin->effect = SKIN_EFFECT_NONE;
            else if (!strcmp(value, "blood"))
                skin->effect = SKIN_EFFECT_BLOOD;
            else if (!strcmp(value, "hearts"))
                skin->effect = SKIN_EFFECT_HEARTS;
            else
                return MEKA_ERR_SYNTAX;
            return MEKA_ERR_OK;
        }

        // FIXME
        return MEKA_ERR_SYNTAX;
        //return MEKA_ERR_OK;
    }
}

//-----------------------------------------------------------------------------
// Skins_Load(const char *filename)
// Load given Meka Skin file
//-----------------------------------------------------------------------------
void        Skins_Load(const char *filename)
{
    ConsolePrint(Msg_Get(MSG_Theme_Loading));

    // Open and read file
    t_tfile* tf;
	tf = tfile_read(filename);
    if (tf == NULL)
    {
        ConsolePrintf ("%s\n", meka_strerror());
        return;
    }

    // Ok
    ConsolePrint("\n");

    // Parse each line
    int line_cnt = 0;
    for (t_list* lines = tf->data_lines; lines; lines = lines->next)
    {
        char* line = (char*)lines->elem;
        line_cnt += 1;
        switch (Skins_ParseLine(line))
        {
        case MEKA_ERR_SYNTAX:
            tfile_free(tf); 
            Quit_Msg(Msg_Get(MSG_Theme_Error_Syntax), line_cnt);
            break;
        case MEKA_ERR_MISSING:
            tfile_free(tf); 
            Quit_Msg(Msg_Get(MSG_Theme_Error_Missing_Theme_Name), line_cnt);
            break;
        case MEKA_ERR_ALREADY_DEFINED:
            tfile_free(tf);
            Quit_Msg(Msg_Get(MSG_Theme_Error_Attribute_Defined), line_cnt);
            break;
        case MEKA_ERR_VALUE_OUT_OF_BOUND:
            tfile_free(tf);
            Quit_Msg(Msg_Get(MSG_Theme_Error_Out_of_Bound), line_cnt);
            break;
        }
    }

    // Reverse list
    list_reverse(&Skins.skins);
    Skins.skin_current = NULL;

    // Free file data
    tfile_free(tf);
}

void        Skins_Close(void)
{
    // Free all strings
    list_free_custom (&Skins.skins, (t_list_free_handler)Skin_Delete);
    free(Skins.skin_configuration_name);
    Skins.skins = NULL;
    Skins.skin_current = NULL;
}

void        Skins_SetSkinConfiguration(const char *skin_name)
{
    if (Skins.skin_configuration_name != NULL)
        free(Skins.skin_configuration_name);
    Skins.skin_configuration_name = strdup(skin_name);
}

t_skin *    Skins_FindSkinByName(const char *skin_name)
{
    for (t_list* skins = Skins.skins; skins != NULL; skins = skins->next)
    {
        t_skin* skin = (t_skin*)skins->elem;
        if (skin->enabled && stricmp(skin_name, skin->name) == 0)
            return (skin);
    }
    return (NULL);
}

void        Skins_Apply(void)
{
    // Update native color table
    Skins_UpdateNativeColorTable();

    // Dirty
    gui.info.must_redraw = TRUE;
    Skins_Background_Redraw();

    // Layout all boxes
    gui_relayout_all();
}

void        Skins_StartupFadeIn(void)
{
    // Setup fade-in from black, or using some special effect?
    t_skin *skin = Skins.skin_current;
    Skins.skin_current = NULL;
    Skins_Select(skin, FALSE);
    Skins_Apply();
}

void        Skins_Select(t_skin *skin, bool fade)
{
    if (skin == Skins.skin_current)
        return;

    // Setup fade
    if (fade)
    {
        Skins.skin_fade         = TRUE;
        Skins.skin_fade_pos     = 0.0f;
        Skins.skin_fade_speed   = SKIN_FADE_SPEED_DEFAULT;
        Skins.skin_fade_from    = Skins.skin_current;
    }
    Skins.skin_current = skin;

    // Load background picture, if any
    if (Skins.background_picture != NULL)
    {
        al_destroy_bitmap(Skins.background_picture);
        Skins.background_picture = NULL;
    }
    if (Skins.skin_current->background_picture != NULL)
    {
        char filename[FILENAME_LEN];
        sprintf(filename, "%s/%s", g_env.Paths.EmulatorDirectory, Skins.skin_current->background_picture);
        Skins.background_picture = al_load_bitmap(filename);
        if (Skins.background_picture == NULL)
        {
            Msg(MSGT_USER, "%s", Msg_Get(MSG_Theme_Error_BG));
            Msg(MSGT_USER_BOX, Msg_Get(MSG_Theme_Error_BG_FileName), Skins.skin_current->background_picture);
        }
    }
}

void        Skins_Update(void)
{
    //{
    //    char buf[128];
    //    sprintf(buf, "Skin %p %p", Skins.skin_current->colors[0], SkinCurrent_NativeColorTable[0]);
    //    Font_Print(F_LARGE, gui_buffer, buf, 20, 20, COLOR_WHITE);
    //}

    // Update fade (if any)
    if (Skins.skin_fade)
    {
        Skins.skin_fade_pos += Skins.skin_fade_speed;
        if (Skins.skin_fade_pos >= 1.0f)
        {
            Skins.skin_fade = FALSE;
            Skins.skin_fade_pos = 1.0f;
        }
        Skins_Apply();
    }

    // Quit after fade
    if (Skins.quit_after_fade && !Skins.skin_fade)
        opt.Force_Quit = TRUE;
}

//-----------------------------------------------------------------------------

static ALLEGRO_COLOR Skins_ColorBlendToNative(u32 color1, u32 color2, float fact1, float fact2)
{
    const int r1 = (color1 & 0x00FF0000) >> 16;
    const int g1 = (color1 & 0x0000FF00) >> 8;
    const int b1 = (color1 & 0x000000FF) >> 0;
    const int r2 = (color2 & 0x00FF0000) >> 16;
    const int g2 = (color2 & 0x0000FF00) >> 8;
    const int b2 = (color2 & 0x000000FF) >> 0;
    const int r  = (fact1 * r1) + (fact2 * r2);
    const int g  = (fact1 * g1) + (fact2 * g2);
    const int b  = (fact1 * b1) + (fact2 * b2);
    assert(r >= 0 && r <= 255);
    assert(g >= 0 && g <= 255);
    assert(b >= 0 && b <= 255);
    return al_map_rgb(r, g, b);
}

static u32  Skins_ColorBlend(u32 color1, u32 color2, float fact1, float fact2)
{
    const int r1 = (color1 & 0x00FF0000) >> 16;
    const int g1 = (color1 & 0x0000FF00) >> 8;
    const int b1 = (color1 & 0x000000FF) >> 0;
    const int r2 = (color2 & 0x00FF0000) >> 16;
    const int g2 = (color2 & 0x0000FF00) >> 8;
    const int b2 = (color2 & 0x000000FF) >> 0;
    const int r  = (fact1 * r1) + (fact2 * r2);
    const int g  = (fact1 * g1) + (fact2 * g2);
    const int b  = (fact1 * b1) + (fact2 * b2);
    assert(r >= 0 && r <= 255);
    assert(g >= 0 && g <= 255);
    assert(b >= 0 && b <= 255);
    return (r << 16) | (g << 8) | (b);
}

static ALLEGRO_COLOR Skins_ColorToNative(u32 color)
{
    const int r = (color & 0x00FF0000) >> 16;
    const int g = (color & 0x0000FF00) >> 8;
    const int b = (color & 0x000000FF) >> 0;
    return al_map_rgb(r, g, b);
}

static void Skins_UpdateNativeGradient(t_skin_gradient *gradient, u32 color_start, u32 color_end)
{
    gradient->native_color_start    = Skins_ColorToNative(color_start);
    gradient->native_color_end      = Skins_ColorToNative(color_end);
    for (int i = 0; i < SKIN_GRADIENT_NATIVE_COLOR_BUFFER_SIZE; i++)
    {
        const float fact2 = (float)i / (float)(SKIN_GRADIENT_NATIVE_COLOR_BUFFER_SIZE - 1);
        const float fact1 = 1.0f - fact2;
        gradient->native_color_buffer[i] = Skins_ColorBlendToNative(color_start, color_end, fact1, fact2);
    }
}

static void Skins_UpdateNativeColorTable(void)
{
    int     i;
    t_skin *skin = Skins.skin_current;
    assert(skin != NULL);

    if (Skins.skin_fade)
    {
        // Blend colors when fading
        const float fact1 = Skins.skin_fade_pos;
        const float fact2 = 1.0f - fact1;
        const t_skin *skin1 = skin;
        const t_skin *skin2 = Skins.skin_fade_from;

        // Update main colors
        for (i = 0; i < SKIN_COLOR_MAX_; i++)
            SkinCurrent_NativeColorTable[i] = Skins_ColorBlendToNative(skin1->colors[i], skin2->colors[i], fact1, fact2);

        // Update gradient data
        Skins_UpdateNativeGradient(&skin->gradient_menu, 
            Skins_ColorBlend(skin1->gradient_menu.color_start, skin2->gradient_menu.color_start, fact1, fact2),
            Skins_ColorBlend(skin1->gradient_menu.color_end,   skin2->gradient_menu.color_end,   fact1, fact2));
        Skins_UpdateNativeGradient(&skin->gradient_window_titlebar, 
            Skins_ColorBlend(skin1->gradient_window_titlebar.color_start, skin2->gradient_window_titlebar.color_start, fact1, fact2),
            Skins_ColorBlend(skin1->gradient_window_titlebar.color_end,   skin2->gradient_window_titlebar.color_end,   fact1, fact2));
    }
    else
    {
        // Update main colors
        for (i = 0; i < SKIN_COLOR_MAX_; i++)
            SkinCurrent_NativeColorTable[i] = Skins_ColorToNative(skin->colors[i]);

        // Update gradient data
        Skins_UpdateNativeGradient(&skin->gradient_menu, skin->gradient_menu.color_start, skin->gradient_menu.color_end);
        Skins_UpdateNativeGradient(&skin->gradient_window_titlebar, skin->gradient_window_titlebar.color_start, skin->gradient_window_titlebar.color_end);
    }
}

static void Skins_MenuHandlerSelectSkin(t_menu_event *event)
{
    // Switch smoothly to new theme
    t_skin* skin = (t_skin*)event->user_data;
    Skins_Select(skin, TRUE);
    Skins_Apply();

    // Check new selected theme in menu
    gui_menu_uncheck_all (menus_ID.themes);
    gui_menu_check (menus_ID.themes, event->menu_item_idx);
}

void        Skins_MenuInit(int menu_id)
{
    for (t_list* skins = Skins.skins; skins != NULL; skins = skins->next)
    {
        t_skin* skin = (t_skin*)skins->elem;
        if (skin->enabled)
        {
            menu_add_item(menu_id, skin->name, NULL,
                MENU_ITEM_FLAG_ACTIVE | ((Skins.skin_current == skin) ? MENU_ITEM_FLAG_CHECKED : 0),
                (t_menu_callback)Skins_MenuHandlerSelectSkin, skin);
        }
    }
}

void        Skins_QuitAfterFade(void)
{
    Skins.quit_after_fade = TRUE;
}

t_skin *    Skins_GetCurrentSkin(void)
{
    return Skins.skin_current;
}

ALLEGRO_BITMAP *    Skins_GetBackgroundPicture(void)
{
    return Skins.background_picture;
}

//-----------------------------------------------------------------------------
// Functions - Gradients
//-----------------------------------------------------------------------------

void    SkinGradient_DrawHorizontal(t_skin_gradient *gradient, ALLEGRO_BITMAP *bitmap, t_frame *frame)
{
    const int x1 = frame->pos.x;
    const int y1 = frame->pos.y;
    const int x2 = frame->pos.x + frame->size.x;
    const int y2 = frame->pos.y + frame->size.y;

    if (!gradient->enabled)
    {
        // Fill with start color
		al_set_target_bitmap(bitmap);
        al_draw_filled_rectangle(x1, y1, x2+1, y2+1, gradient->native_color_start);
    }
    else
    {
        // Draw gradient
        const int gradient_pos_start = ((x2 - x1) * gradient->pos_start) / 100;
        const int gradient_pos_end   = ((x2 - x1) * gradient->pos_end)   / 100;
        const int gradient_size      = gradient_pos_end - gradient_pos_start;
		al_set_target_bitmap(bitmap);
        if (gradient_pos_start != 0)
            al_draw_filled_rectangle(x1, y1+0.5f, x1 + gradient_pos_start + 1, y2+1.5f, gradient->native_color_start);
		if ( gradient_size > 0 )
		{
			for (int n = 0; n <= gradient_size; n++)
			{
				const int gradient_idx = n * (SKIN_GRADIENT_NATIVE_COLOR_BUFFER_SIZE - 1) / gradient_size;
				const int x = x1 + n + gradient_pos_start;
				const ALLEGRO_COLOR color = gradient->native_color_buffer[gradient_idx];
				al_draw_line(x+0.5f, y1+0.5f, x+0.5f, y2+1.5f, color, 0.0f);
			}
		}
        if (gradient_pos_end < frame->size.x+1)
            al_draw_filled_rectangle(x1 + gradient_pos_end, y1+0.5f, x2+1, y2+1.5f, gradient->native_color_end);
    }
}

void    SkinGradient_DrawVertical(t_skin_gradient *gradient, ALLEGRO_BITMAP *bitmap, t_frame *frame)
{
    const int x1 = frame->pos.x;
    const int y1 = frame->pos.y;
    const int x2 = frame->pos.x + frame->size.x;
    const int y2 = frame->pos.y + frame->size.y;

	al_set_target_bitmap(bitmap);
    if (!gradient->enabled)
    {
        // Fill with start color
        al_draw_filled_rectangle(x1, y1, x2+1, y2+1, gradient->native_color_start);
    }
    else
    {
        // Draw gradient
        const int gradient_pos_start = ((y2 - y1) * gradient->pos_start) / 100;
        const int gradient_pos_end   = ((y2 - y1) * gradient->pos_end)   / 100;
        const int gradient_size      = gradient_pos_end - gradient_pos_start;
        if (gradient_pos_start != 0)
            al_draw_filled_rectangle(x1, y1, x2 + 1, y1 + gradient_pos_start + 1, gradient->native_color_start);
		if ( gradient_size > 0 )
		{
			for (int n = 0; n <= gradient_size; n++)
			{
				const int gradient_idx = n * (SKIN_GRADIENT_NATIVE_COLOR_BUFFER_SIZE - 1) / gradient_size;
				const int y = y1 + n + gradient_pos_start;
				const ALLEGRO_COLOR color = gradient->native_color_buffer[gradient_idx];
				al_draw_hline(x1, y, x2, color);
			}
		}
        if (gradient_pos_end < frame->size.y+1)
            al_draw_filled_rectangle(x1, y1 + gradient_pos_end, x2 + 1, y2 + 1, gradient->native_color_end);
    }
}

//-----------------------------------------------------------------------------
