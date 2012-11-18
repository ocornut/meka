//-----------------------------------------------------------------------------
// MEKA - palette.c
// Palette management - Code
//-----------------------------------------------------------------------------
// (This used to be a quite fancy dynamic palette management system to attempt
// to minimize hardware palette change, but its mostly meaningless today)
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "palette.h"
#include "video.h"
#include "video_m2.h"

// #define DEBUG_PALETTE

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

ALLEGRO_COLOR   Palette_Emulation[PALETTE_EMU_GAME_SIZE];
u32				Palette_EmulationToHostGui[PALETTE_EMU_GAME_SIZE];
u16				Palette_EmulationToHostGame[PALETTE_EMU_GAME_SIZE];
int				Palette_EmulationFlags[PALETTE_EMU_GAME_SIZE];
bool			Palette_EmulationDirtyAny;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Initialize palette engine
void    Palette_Init (void)
{

    Palette_Emulation_Reset();
}

// Close palette engine
void    Palette_Close(void)
{
}

void    Palette_UpdateAfterRedraw()
{
    // Clear dirty flags
    for (int i = 0; i != PALETTE_EMU_GAME_SIZE; i++)
        Palette_EmulationFlags[i] &= ~PALETTE_EMULATION_FLAGS_DIRTY;
    Palette_EmulationDirtyAny = FALSE;
}

void    Palette_Emulation_Reset()
{
    for (int i = 0; i != PALETTE_EMU_GAME_SIZE; i++)
    {
        Palette_Emulation[i] = COLOR_BLACK;
        Palette_EmulationToHostGui[i] = 0;
        Palette_EmulationToHostGame[i] = 0;
        Palette_EmulationFlags[i] = PALETTE_EMULATION_FLAGS_DIRTY;
    }
    Palette_EmulationDirtyAny = TRUE;
    Palette_Emulation_Reload();
}

// Reload palette data (fixed or from PRAM)
// Called when changing video mode on the fly
void    Palette_Emulation_Reload (void)
{
    switch (g_driver->vdp)
    {
    case VDP_TMS9918:  
        TMS9918_Palette_Setup();
        return;
    }

    // g_driver->vdp == VDP_SMSGG
    // SMS/GG Palette will be reloaded
#ifdef DEBUG_PALETTE
    Msg(MSGT_DEBUG, "Palette_Emulation_Reload() SMS/GG");
#endif

    switch (g_driver->id)
    {
    case DRV_SMS:
        for (int i = 0; i != 32; i++)
        {
			ALLEGRO_COLOR color;
            Palette_Compute_RGB_SMS(&color, i);
            Palette_Emulation_SetColor(i, color);
        }
        break;
    case DRV_GG:
        for (int i = 0; i != 32; i++)
        {
			ALLEGRO_COLOR color;
            Palette_Compute_RGB_GG(&color, i * 2);
            Palette_Emulation_SetColor(i, color);
        }
        break;
    }
}

u32		Palette_MakeHostColor(int color_format, int r, int g, int b)
{
	assert( (r&~0xFF)==0 && (g&~0xFF)==0 && (b&~0xFF)==0 );

	switch (color_format)
	{
	case ALLEGRO_PIXEL_FORMAT_RGB_565:
		r >>= 3;
		g >>= 2;
		b >>= 3;
		return (r << 11) | (g << 5) | (b);
	case ALLEGRO_PIXEL_FORMAT_RGB_555:
		r >>= 3;
		g >>= 3;
		b >>= 3;
		return (r << 10) | (g << 5) | (b);
	case ALLEGRO_PIXEL_FORMAT_BGR_565:
		r >>= 3;
		g >>= 2;
		b >>= 3;
		return (b << 11) | (g << 5) | (r);
	case ALLEGRO_PIXEL_FORMAT_BGR_555:
		r >>= 3;
		g >>= 3;
		b >>= 3;
		return (b << 10) | (g << 5) | (b);
	case ALLEGRO_PIXEL_FORMAT_ARGB_8888:
	case ALLEGRO_PIXEL_FORMAT_XRGB_8888:
		return (0xFF << 24) | (r << 16) | (g << 8) | (b);
	case ALLEGRO_PIXEL_FORMAT_RGBA_8888:
	case ALLEGRO_PIXEL_FORMAT_RGBX_8888:
		return    (r << 24) | (g << 16) | (b << 8) | (0xFF);
	case ALLEGRO_PIXEL_FORMAT_ABGR_8888:
	case ALLEGRO_PIXEL_FORMAT_XBGR_8888:
		return (0xFF << 24) | (b << 16) | (g << 8) | (r);
	}

	if (color_format != 0)	// During init
		Msg(MSGT_DEBUG, "Palette_MakeHostColor() failed, unknown format: %d", color_format);
	return 0;
}

u32		Palette_MakeHostColor(int format, ALLEGRO_COLOR color)
{
	return Palette_MakeHostColor(format, color.r*255, color.g*255, color.b*255);
}

void    Palette_Emulation_SetColor(int idx, ALLEGRO_COLOR color)
{
    assert(idx >= 0 && idx < 32);
    Palette_Emulation[idx] = color;
	Palette_EmulationToHostGui[idx] = Palette_MakeHostColor(g_gui_buffer_format, color);
    Palette_EmulationToHostGame[idx] = Palette_MakeHostColor(g_screenbuffer_format, color);
    Palette_EmulationFlags[idx] |= PALETTE_EMULATION_FLAGS_DIRTY;
    Palette_EmulationDirtyAny = TRUE;
}

//-----------------------------------------------------------------------------

void    Palette_Compute_RGB_SMS(ALLEGRO_COLOR *out_color, int i)
{
    int v;
	int r, g, b;

    v = PRAM[i] & 0x03;
    r = (v) | (v << 2) | (v << 4) | (v << 6);

    v = (PRAM[i] >> 2) & 0x03;
    g = (v) | (v << 2) | (v << 4) | (v << 6);

    v = (PRAM[i] >> 4) & 0x03;
    b = (v) | (v << 2) | (v << 4) | (v << 6);

    // Save output
	*out_color = al_map_rgb(r, g, b);
}

// Note: if changing the meaning of 'i', please update datadump.c which uses it
void    Palette_Compute_RGB_GG(ALLEGRO_COLOR *out_color, int i)
{
    int v;
	int r, g, b;

    // ----bbbb ggggrrrr (GG) -> --rrrrrr --gggggg --bbbbbb (RGB)
	v = PRAM[i] & 0x0F;
	r = (v) | (v << 4);

	v = PRAM[i] & 0xF0;
	g = (v >> 4) | (v);

	v = PRAM[i + 1] & 0x0F;
	b = (v) | (v << 4);

    // Save output
	*out_color = al_map_rgb(r, g, b);
}

//-----------------------------------------------------------------------------
