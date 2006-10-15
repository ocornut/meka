//-----------------------------------------------------------------------------
// MEKA - themes.h
// Interface Themes - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Load_Theme_List           (void);
int     Analyse_Line_Theme        (char *s, char *s_case);
void    Init_Themes_Menu          (int menu_id);

void    Themes_Update             (void);
void    Themes_Set                (byte n, byte mode);
void    Action_Theme              (byte n);
void    Themes_Init_Values        (void);
void    Themes_Background_Update  (void);

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define THEME_CHANGE_QUICK        (0)
#define THEME_CHANGE_FADE         (1)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
 char * name;
 char * background_picture;
 int    background_mode;
 byte   special;
 byte   bar_gradients, bar_gradients_unused;
 float  bar_gradients_ratio;
 byte   menu_gradients, menu_gradients_unused;
 float  menu_gradients_ratio;
 byte   colors [GUI_COL_THEME_NUM] [3];
} t_theme;

t_theme *       theme_current; // FIXME: move in Themes

typedef struct
{
 int            max;
 int            current;
 char           filename [FILENAME_LEN];
 byte           currently_switching;
 t_theme **     data;
 byte           temp [GUI_COL_THEME_NUM] [3];
 int            special;
} t_themes;

t_themes        Themes;

//-----------------------------------------------------------------------------

#define THEME_BG_STRETCHED     (0)
#define THEME_BG_INT_STRETCHED (1)
#define THEME_BG_TILED         (2)
#define THEME_BG_CENTERED      (3)

typedef struct
{
 char           filename [FILENAME_LEN];
 BITMAP *       picture;
 int            picture_ok;
 RGB            pal [128];
} t_theme_bg;

t_theme_bg      ThemeBackground;

//-----------------------------------------------------------------------------


