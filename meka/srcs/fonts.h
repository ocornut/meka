//-----------------------------------------------------------------------------
// MEKA - fonts.h
// Fonts Tools (mostly wrapping Allegro functionalities) - Headers
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

enum t_font_id
{
	F_CURRENT	= -1,		// Note: Code tends to write -1 directly as an abbrevation
	F_LARGE		= 0,
	F_MEDIUM	= 1,
	F_SMALL		= 2,
};

#define         MEKA_FONT_MAX           (3)

#define         MEKA_FONT_STR_STAR      "\xC2\x80"	// 128 in UTF8
#define         MEKA_FONT_STR_CHECKED   "\xC2\x81"	// 129 in UTF8
#define         MEKA_FONT_STR_ARROW     ">"			// (not using the one stored at 130)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_font
{
    t_font_id		id;
    ALLEGRO_FONT *	library_data;
    int				height;
};

extern t_font		Fonts[MEKA_FONT_MAX];
extern t_font *		FontCurrent;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Fonts_Init          (void);
void    Fonts_Close         (void);
void    Fonts_DeclareFont   (t_font_id font_id, ALLEGRO_FONT *library_data);

//-----------------------------------------------------------------------------

void    Font_SetCurrent     (t_font_id font_id);
void    Font_Print          (t_font_id font_id, const char *text, int x, int y, ALLEGRO_COLOR color);
void    Font_PrintCentered  (t_font_id font_id, const char *text, int x, int y, ALLEGRO_COLOR color);
int     Font_Height         (t_font_id font_id = F_CURRENT);
int     Font_TextLength     (t_font_id font_id, const char *text);

struct FontPrinter
{
	t_font_id		font_id;
	ALLEGRO_COLOR	color;
	
	FontPrinter(t_font_id _font_id);
	FontPrinter(t_font_id _font_id, ALLEGRO_COLOR _color);

	void	Printf(v2i pos, const char* format, ...) const;
};

//-----------------------------------------------------------------------------

