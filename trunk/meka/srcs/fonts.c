//-----------------------------------------------------------------------------
// MEKA - fonts.c
// Fonts Tools (mostly wrapping Allegro functionalities now) - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_font		Fonts[FONTID_COUNT_];
t_font *	FontCurrent = NULL;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

static void Font_Declare(t_font_id id, const char* text_id, const char* load_filename, int load_size)
{
	Fonts[id].id = id;
	Fonts[id].text_id = text_id;
	Fonts[id].load_filename = load_filename;
	Fonts[id].load_size = load_size;
}

void    Fonts_Init()
{
	Font_Declare(FONTID_SMALL,			"CLASSIC_SMALL",	"font_2.tga",					0);
	Font_Declare(FONTID_MEDIUM,			"CLASSIC_MEDIUM",	"font_1.tga",					0);
	Font_Declare(FONTID_LARGE,			"CLASSIC_LARGE",	"font_0.tga",					0);
	Font_Declare(FONTID_PROGGY_TINY,		"PROGGY_TINY",		"fonts/ProggyTinySZ.ttf",		-10);
	Font_Declare(FONTID_PROGGY_SMALL,	"PROGGY_SMALL",		"fonts/ProggySmall.ttf",		-10);
	Font_Declare(FONTID_PROGGY_SQUARE,	"PROGGY_SQUARE",	"fonts/ProggySquareSZ.ttf",		-11);
	Font_Declare(FONTID_PROGGY_CLEAN,	"PROGGY_CLEAN",		"fonts/ProggyCleanSZ.ttf",		-13);
	Font_Declare(FONTID_PCMONO,			"PCMONO",			"fonts/PixelCarnageMono.ttf",	-16);
	Font_Declare(FONTID_CRISP,			"CRISP",			"fonts/Crisp.ttf",				-16);
	// NB: actual data loaded in Data.c
}

void    Fonts_Close()
{
}

// Register font to the fonts system
void    Fonts_DeclareFont(t_font_id font_id, const char* text_id)
{
    t_font *font		= &Fonts[font_id];
    font->id            = font_id;
    font->height        = al_get_font_line_height(font->library_data);
	font->text_id		= text_id;
}

void    Font_SetCurrent(t_font_id font_id)
{
    FontCurrent = &Fonts[font_id];
}

// from imgui.cpp
const char* CalcWordWrapPosition(t_font* font, const char* text, const char* text_end, int wrap_width)
{
	int line_width = 0;
	int word_width = 0;
	int blank_width = 0;

	const char* word_end = text;
	const char* prev_word_end = NULL;
	bool inside_word = true;

	const char* s = text;
	while (s < text_end)
	{
		const char c = *s;

		if (c == '\n')
		{
			line_width = word_width = blank_width = 0;
			inside_word = true;
			s++;
			continue;
		}

		const char buf[2] = { c, 0 };
		const int char_width = Font_TextWidth(font->id, buf);
		if (c == ' ' || c == '\t')
		{
			if (inside_word)
			{
				line_width += blank_width;
				blank_width = 0;
			}
			blank_width += char_width;
			inside_word = false;
		}
		else
		{
			word_width += char_width;
			if (inside_word)
			{
				word_end = s + 1;
			}
			else
			{
				prev_word_end = word_end;
				line_width += word_width + blank_width;
				word_width = blank_width = 0;
			}

			// Allow wrapping after punctuation.
			inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
		}

		// We ignore blank width at the end of the line (they can be skipped)
		// Words that cannot possibly fit within an entire line will be cut anywhere.
		if (line_width + word_width >= wrap_width)
		{
			if (word_width < wrap_width)
				s = prev_word_end ? prev_word_end : word_end;
			break;
		}

		s++;
	}

	return s;
}

// Print given string with parameters using current font
void    Font_Print(t_font_id font_id, const char *text, int x, int y, ALLEGRO_COLOR color, int wrap_width)
{
	t_font* font = (font_id == FONTID_CUR) ? FontCurrent : &Fonts[font_id];

	while (*text)
	{
		// Split by line
		const char* line_start = text;
		const char* line_end = strchr(line_start, '\n');
		if (!line_end)
			line_end = line_start + strlen(line_start);

		// Wrap
		if (wrap_width > 0)
		{
			const char* new_line_end = CalcWordWrapPosition(font, line_start, line_end, wrap_width);
			if (line_end != new_line_end)
			{
				line_end = new_line_end;
				while (*line_end == ' ' || *line_end == '\t' || *line_end == '\r' || *line_end == '\n')
					line_end++;
			}
		}

		// Copy it into a buffer - Allegro doesn't provide the right apis
		char buf[MSG_MAX_LEN];
		memcpy(buf, line_start, line_end-line_start);
		buf[line_end-line_start] = 0;
		al_draw_text(font->library_data, color, x, y, ALLEGRO_ALIGN_LEFT, buf);

		y += font->height;

		text = line_end;
		if (*text == '\n')
			text++;
	}
}

// Print given string, centered around a given x position
void    Font_PrintCentered(t_font_id font_id, const char *text, int x, int y, ALLEGRO_COLOR color)
{
	t_font* font = (font_id == FONTID_CUR) ? FontCurrent : &Fonts[font_id];

	al_draw_text(font->library_data, color, x, y, ALLEGRO_ALIGN_CENTRE, text);
}

int     Font_Height(t_font_id font_id)
{
	t_font* font = (font_id == FONTID_CUR) ? FontCurrent : &Fonts[font_id];

	return font->height;
}

int      Font_TextWidth(t_font_id font_id, const char* text, const char* text_end)
{
	t_font* font = (font_id == FONTID_CUR) ? FontCurrent : &Fonts[font_id];

	if (text_end)
	{
		// FIXME-OPT: allegro needs better API
		static char buf[MSG_MAX_LEN];
		snprintf(buf, countof(buf), "%.*s", text_end-text, text);
		buf[countof(buf)-1] = 0;
		return al_get_text_width(font->library_data, buf);
	}
	else
	{
		return al_get_text_width(font->library_data, text);
	}
}

int      Font_TextHeight(t_font_id font_id, const char *text, int wrap_width)
{
	t_font* font = (font_id == FONTID_CUR) ? FontCurrent : &Fonts[font_id];

	int h = 0;
	while (*text)
	{
		// Split by line
		const char* line_start = text;
		const char* line_end = strchr(line_start, '\n');
		if (!line_end)
			line_end = line_start + strlen(line_start);

		// Wrap
		if (wrap_width > 0)
		{
			const char* new_line_end = CalcWordWrapPosition(font, line_start, line_end, wrap_width);
			if (line_end != new_line_end)
			{
				line_end = new_line_end;
				while (*line_end == ' ' || *line_end == '\t' || *line_end == '\r' || *line_end == '\n')
					line_end++;
			}
		}

		h += font->height;

		text = line_end;
		if (*text == '\n')
			text++;
	}
	return h;
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
