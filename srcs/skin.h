//-----------------------------------------------------------------------------
// MEKA - skin.h
// Interface Skins - Headers
//-----------------------------------------------------------------------------
// Note: 'skins' referred as 'themes' to user.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// User definitions
#define COLOR_SKIN_BACKGROUND                       SkinCurrent_NativeColorTable[0]
#define COLOR_SKIN_BACKGROUND_GRID                  SkinCurrent_NativeColorTable[1]
#define COLOR_SKIN_WINDOW_BORDER                    SkinCurrent_NativeColorTable[2]
#define COLOR_SKIN_WINDOW_BACKGROUND                SkinCurrent_NativeColorTable[3]
#define COLOR_SKIN_WINDOW_TITLEBAR                  SkinCurrent_NativeColorTable[4]
#define COLOR_SKIN_WINDOW_TITLEBAR_TEXT             SkinCurrent_NativeColorTable[5]
#define COLOR_SKIN_WINDOW_TITLEBAR_TEXT_UNACTIVE    SkinCurrent_NativeColorTable[6]
#define COLOR_SKIN_WINDOW_TEXT                      SkinCurrent_NativeColorTable[7]
#define COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT            SkinCurrent_NativeColorTable[8]
#define COLOR_SKIN_WINDOW_SEPARATORS                SkinCurrent_NativeColorTable[9]
#define COLOR_SKIN_MENU_BACKGROUND                  SkinCurrent_NativeColorTable[10]
#define COLOR_SKIN_MENU_BORDER                      SkinCurrent_NativeColorTable[11]
#define COLOR_SKIN_MENU_SELECTION                   SkinCurrent_NativeColorTable[12]
#define COLOR_SKIN_MENU_TEXT                        SkinCurrent_NativeColorTable[13]
#define COLOR_SKIN_MENU_TEXT_UNACTIVE               SkinCurrent_NativeColorTable[14]
#define COLOR_SKIN_WIDGET_GENERIC_BACKGROUND        SkinCurrent_NativeColorTable[15]
#define COLOR_SKIN_WIDGET_GENERIC_SELECTION         SkinCurrent_NativeColorTable[16]
#define COLOR_SKIN_WIDGET_GENERIC_BORDER            SkinCurrent_NativeColorTable[17]
#define COLOR_SKIN_WIDGET_GENERIC_TEXT              SkinCurrent_NativeColorTable[18]
#define COLOR_SKIN_WIDGET_GENERIC_TEXT_UNACTIVE     SkinCurrent_NativeColorTable[19]
#define COLOR_SKIN_WIDGET_LISTBOX_BACKGROUND        SkinCurrent_NativeColorTable[20]
#define COLOR_SKIN_WIDGET_LISTBOX_BORDER            SkinCurrent_NativeColorTable[21]
#define COLOR_SKIN_WIDGET_LISTBOX_SELECTION         SkinCurrent_NativeColorTable[22]
#define COLOR_SKIN_WIDGET_LISTBOX_TEXT              SkinCurrent_NativeColorTable[23]
#define COLOR_SKIN_WIDGET_SCROLLBAR_BACKGROUND      SkinCurrent_NativeColorTable[24]
#define COLOR_SKIN_WIDGET_SCROLLBAR_SCROLLER        SkinCurrent_NativeColorTable[25]
#define COLOR_SKIN_WIDGET_STATUSBAR_BACKGROUND      SkinCurrent_NativeColorTable[26]
#define COLOR_SKIN_WIDGET_STATUSBAR_BORDER          SkinCurrent_NativeColorTable[27]
#define COLOR_SKIN_WIDGET_STATUSBAR_TEXT            SkinCurrent_NativeColorTable[28]

#define COLOR_SKIN_INDEX(_COLOR)					((int)(&_COLOR - &SkinCurrent_NativeColorTable[0]))

#define SKIN_COLOR_MAX_                             29

#define SKIN_GRADIENT_NATIVE_COLOR_BUFFER_SIZE		(256)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

extern ALLEGRO_COLOR	SkinCurrent_NativeColorTable[SKIN_COLOR_MAX_];

struct t_skin_gradient
{
	bool				enabled;			// if not enabled, fill with native_gradient_start[0]
	int					pos_start;			// 0-100%
	int					pos_end;			// 0-100%, >= pos_start
	u32 				color_start;
	u32 				color_end;
	ALLEGRO_COLOR 		native_color_start;
	ALLEGRO_COLOR		native_color_end;
	ALLEGRO_COLOR		native_color_buffer[SKIN_GRADIENT_NATIVE_COLOR_BUFFER_SIZE];
};

enum t_skin_effect
{
	SKIN_EFFECT_NONE	= 0,
	SKIN_EFFECT_BLOOD	= 1,
	SKIN_EFFECT_HEARTS	= 2,
};

enum t_skin_background_picture_mode
{
    SKIN_BACKGROUND_PICTURE_MODE_CENTER         = 0,
    SKIN_BACKGROUND_PICTURE_MODE_STRETCH        = 1,
    SKIN_BACKGROUND_PICTURE_MODE_STRETCH_INT    = 2,
    SKIN_BACKGROUND_PICTURE_MODE_TILE           = 3,
    SKIN_BACKGROUND_PICTURE_MODE_DEFAULT        = SKIN_BACKGROUND_PICTURE_MODE_STRETCH,
};

struct t_skin
{
    bool                            enabled;
    char *                          name;
    char *                          authors;
    u32                             colors[SKIN_COLOR_MAX_];
    bool                            colors_defined[SKIN_COLOR_MAX_];
	t_skin_gradient		            gradient_window_titlebar;
	t_skin_gradient		            gradient_menu;
	t_skin_effect		            effect;
    char *                          background_picture;
    t_skin_background_picture_mode  background_picture_mode;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void                Skins_Init_Values               (void);
void				Skins_Init                      (void);
void				Skins_Load                      (const char *filename);
void				Skins_Close                     (void);

void				Skins_StartupFadeIn				(void);
void				Skins_Apply						(void);
void				Skins_Update                    (void);
void			    Skins_MenuInit                  (int menu_id);
void                Skins_Select                    (t_skin *skin, bool fade);

void                Skins_QuitAfterFade             (void);

void                Skins_SetSkinConfiguration      (const char *skin_name);

t_skin *			Skins_GetCurrentSkin			(void);
t_skin *            Skins_FindSkinByName            (const char *skin_name);
ALLEGRO_BITMAP *    Skins_GetBackgroundPicture      (void);

//-----------------------------------------------------------------------------

void				SkinGradient_DrawHorizontal(t_skin_gradient *gradient, ALLEGRO_BITMAP *bitmap, t_frame *frame);
void				SkinGradient_DrawVertical(t_skin_gradient *gradient, ALLEGRO_BITMAP *bitmap, t_frame *frame);

//-----------------------------------------------------------------------------
