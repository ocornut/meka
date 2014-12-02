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
	FONTID_NONE			= -2,
	FONTID_CUR			= -1,	// NB: Code tends to write -1 directly as an abbreviation
	FONTID_SMALL		= 0,
	FONTID_MEDIUM		= 1,
	FONTID_LARGE		= 2,
	FONTID_PROGGY_TINY,
	FONTID_PROGGY_SMALL,
	FONTID_PROGGY_SQUARE,
	FONTID_PROGGY_CLEAN,
	FONTID_PCMONO,
	FONTID_CRISP,
	FONTID_COUNT_,
};

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
	const char*		text_id;
	const char*		load_filename;
	int				load_size;
};

extern t_font		Fonts[FONTID_COUNT_];
extern t_font *		FontCurrent;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Fonts_Init          ();
void    Fonts_Close         ();

//-----------------------------------------------------------------------------

void    Font_SetCurrent     (t_font_id font_id);
void    Font_Print          (t_font_id font_id, const char *text, int x, int y, ALLEGRO_COLOR color, int wrap_width = -1);
void    Font_PrintCentered  (t_font_id font_id, const char *text, int x, int y, ALLEGRO_COLOR color);
int     Font_Height         (t_font_id font_id = FONTID_CUR);
int     Font_TextWidth      (t_font_id font_id, const char *text, const char* text_end = NULL);
int     Font_TextHeight     (t_font_id font_id, const char *text, int wrap_width = -1);

struct FontPrinter
{
	t_font_id		font_id;
	ALLEGRO_COLOR	color;
	
	FontPrinter(t_font_id _font_id);
	FontPrinter(t_font_id _font_id, ALLEGRO_COLOR _color);

	void	Printf(v2i pos, const char* format, ...) const;
};

//-----------------------------------------------------------------------------

