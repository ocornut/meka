//-----------------------------------------------------------------------------
// MEKA - fonts.h
// Fonts Tools (mostly wrapping Allegro functionalities) - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define         F_LARGE                 (0)
#define         F_MIDDLE                (1)
#define         F_SMALL                 (2)
#define         MEKA_FONT_MAX           (3)

#define         MEKA_FONT_STR_STAR      "€"
#define         MEKA_FONT_STR_CHECKED   ""
#define         MEKA_FONT_STR_ARROW     ">" // "‚"

#define         MEKA_FONT_CHAR_STAR     (128)
#define         MEKA_FONT_CHAR_CHECKED  (129)
#define         MEKA_FONT_CHAR_ARROW    (130)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
    int         id;
    ALLEGRO_FONT *      library_data;
    int         height;
}               t_meka_font;

t_meka_font     Fonts[MEKA_FONT_MAX];
t_meka_font *   FontCurrent;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Fonts_Init          (void);
void    Fonts_Close         (void);
void    Fonts_AddFont       (int font_id, ALLEGRO_FONT *library_data);

//-----------------------------------------------------------------------------

void    Font_SetCurrent     (int font_id);
void    Font_Print          (int font_id, ALLEGRO_BITMAP *dst, const char *text, int x, int y, ALLEGRO_COLOR color);
void    Font_PrintCentered  (int font_id, ALLEGRO_BITMAP *dst, const char *text, int x, int y, ALLEGRO_COLOR color);
int     Font_Height         (int font_id);
int     Font_TextLength     (int font_id, const char *text);

//-----------------------------------------------------------------------------

