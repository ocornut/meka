//-----------------------------------------------------------------------------
// MEKA - fonts.c
// Fonts Tools (mostly wrapping Allegro functionalities now) - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_font		Fonts[MEKA_FONT_MAX];
t_font *	FontCurrent;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Fonts_Init (void)
{
    // See DATA.C
}

void    Fonts_Close (void)
{
}

// Register font to the fonts system
void    Fonts_DeclareFont (t_font_id font_id, ALLEGRO_FONT *library_data)
{
    t_font *font		= &Fonts[font_id];
    font->id            = font_id;
    font->library_data  = library_data;
    font->height        = al_get_font_line_height(library_data);
}

void    Font_SetCurrent (t_font_id font_id)
{
    FontCurrent = &Fonts[font_id];
}

// Print given string with parameters using current font
void    Font_Print(t_font_id font_id, const char *text, int x, int y, ALLEGRO_COLOR color)
{
    if (font_id == F_CURRENT)
        font_id = FontCurrent->id;
    al_draw_text(Fonts[font_id].library_data, color, x, y, ALLEGRO_ALIGN_LEFT, text);
}

// Print given string, centered around a given x position
void    Font_PrintCentered(t_font_id font_id, const char *text, int x, int y, ALLEGRO_COLOR color)
{
    if (font_id == F_CURRENT)
        font_id = FontCurrent->id;
    al_draw_text(Fonts[font_id].library_data, color, x, y, ALLEGRO_ALIGN_CENTRE, text);
}

// Return height of given font
int     Font_Height(t_font_id font_id)
{
    if (font_id == F_CURRENT)
        font_id = FontCurrent->id;
    return Fonts[font_id].height;
}

// Return length of given text in pixel
int      Font_TextLength(t_font_id font_id, const char *text)
{
    if (font_id == F_CURRENT)
        font_id = FontCurrent->id;
    return al_get_text_width(Fonts[font_id].library_data, text);
}

//-----------------------------------------------------------------------------

FontPrinter::FontPrinter(t_font_id _font_id)
{
	this->font_id = _font_id;
	this->color = COLOR_SKIN_WINDOW_TEXT;
}

FontPrinter::FontPrinter(t_font_id _font_id, ALLEGRO_COLOR _color)
{
	this->font_id = _font_id;
	this->color = _color;
}

void FontPrinter::Printf(v2i pos, const char* format, ...) const
{
	char buf[512];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, countof(buf), format, args);
	va_end(args);
	Font_Print(this->font_id, buf, pos.x, pos.y, this->color);
}

//-----------------------------------------------------------------------------
