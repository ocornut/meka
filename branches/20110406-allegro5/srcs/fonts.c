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
void    Fonts_DeclareFont (int font_id, ALLEGRO_FONT *library_data)
{
    t_font *font		= &Fonts[font_id];
    font->id            = font_id;
    font->library_data  = library_data;
    font->height        = al_get_font_line_height(library_data);
}

void    Font_SetCurrent (int font_id)
{
    FontCurrent = &Fonts[font_id];
}

// Print given string with parameters using current font
void    Font_Print (int font_id, ALLEGRO_BITMAP *dst, const char *text, int x, int y, ALLEGRO_COLOR color)
{
    if (font_id == -1)
        font_id = FontCurrent->id;
	al_set_target_bitmap(dst);
    al_draw_text(Fonts[font_id].library_data, color, x, y, ALLEGRO_ALIGN_LEFT, text);
}

// Print given string, centered around a given x position
void    Font_PrintCentered (int font_id, ALLEGRO_BITMAP *dst, const char *text, int x, int y, ALLEGRO_COLOR color)
{
    if (font_id == -1)
        font_id = FontCurrent->id;
	al_set_target_bitmap(dst);
    al_draw_text(Fonts[font_id].library_data, color, x, y, ALLEGRO_ALIGN_CENTRE, text);
}

// Return height of given font
int     Font_Height (int font_id)
{
    if (font_id == -1)
        font_id = FontCurrent->id;
    return Fonts[font_id].height;
}

//-----------------------------------------------------------------------------
// Font_TextLength (int font_id, const char *text)
// Return length of given text in pixel
//-----------------------------------------------------------------------------
int      Font_TextLength (int font_id, const char *text)
{
    if (font_id == -1)
        font_id = FontCurrent->id;
    return al_get_text_width(Fonts[font_id].library_data, text);
}

//-----------------------------------------------------------------------------

