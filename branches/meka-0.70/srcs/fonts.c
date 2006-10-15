//-----------------------------------------------------------------------------
// MEKA - fonts.c
// Fonts Tools (mostly wrapping Allegro functionalities now) - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Fonts_Init (void)
// Initialize fonts system
//-----------------------------------------------------------------------------
void    Fonts_Init (void)
{
    // See DATA.C
}

//-----------------------------------------------------------------------------
// Fonts_Close (void)
// Close fonts system
//-----------------------------------------------------------------------------
void    Fonts_Close (void)
{
}

//-----------------------------------------------------------------------------
// Fonts_AddFont (int font_id, FONT *library_data)
// Register font to the fonts system
//-----------------------------------------------------------------------------
void    Fonts_AddFont (int font_id, FONT *library_data)
{
    t_meka_font *font   = &Fonts[font_id];
    font->id            = font_id;
    font->library_data  = library_data;
    font->height        = text_height (library_data);
}

//-----------------------------------------------------------------------------
// Font_SetCurrent (int font_id)
// Set current font
//-----------------------------------------------------------------------------
void    Font_SetCurrent (int font_id)
{
    FontCurrent = &Fonts[font_id];
}

//-----------------------------------------------------------------------------
// Font_Print (int font_id, BITMAP *dst, const char *text, int x, int y, int color);
// Print given string with parameters using current font
//-----------------------------------------------------------------------------
void    Font_Print (int font_id, BITMAP *dst, const char *text, int x, int y, int color)
{
    if (font_id == -1)
        font_id = FontCurrent->id;
    // textout (dst, Fonts[font_id].library_data, text, x, y, color);
    textout_ex (dst, Fonts[font_id].library_data, text, x, y, color, -1);
}

//-----------------------------------------------------------------------------
// Font_PrintCentered (int font_id, BITMAP *dst, const char *text, int x_center, int y, int color)
// Print given string, centered around a given x position
//-----------------------------------------------------------------------------
void    Font_PrintCentered (int font_id, BITMAP *dst, const char *text, int x_center, int y, int color)
{
    if (font_id == -1)
        font_id = FontCurrent->id;
    textout_centre_ex (dst, Fonts[font_id].library_data, text, x_center, y, color, -1);
}

//-----------------------------------------------------------------------------
// Font_Height (int font_id)
// Return height of given font
//-----------------------------------------------------------------------------
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
    return text_length (Fonts[font_id].library_data, text);
}

//-----------------------------------------------------------------------------

