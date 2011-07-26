//-----------------------------------------------------------------------------
// MEKA - video_c.c
// Video / C Functions - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "video_m5.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Decode_Tile_C (int tile_n, byte *start)
{
    ConsolePrint ("video_c.c::Decode_Tile_C() is empty,\ntiles will not be showing.\n");
}

void    Find_Last_Sprite(int sprites_height, int VDP_Line)
{
	int sprites_count = 0;
	int sprites_on_line = 0;

	const u8* sat = g_machine.VDP.sprite_attribute_table;
    while (sprites_count < 64)
    {
		int y;
        if ((y = sat[sprites_count]) == 208)
            break;
        sprites_count++;
        if (y > 224)
			y -= 256;
        const int line = VDP_Line - y - 1;
        if (line >= 0 && line < sprites_height)
            sprites_on_line++;
    }

	// Assign to globals
    Sprite_Last = sprites_count - 1;
	Sprites_on_Line = sprites_on_line;
}

// FIXME-EMU: Not sure if this behavior is correct but it served us well so far.
void    Find_Last_Sprite_Wide(int sprites_height, int VDP_Line)
{
	int sprites_count = 0;
	int sprites_on_line = 0;

	const u8* sat = g_machine.VDP.sprite_attribute_table;
	while (sprites_count < 64)
    {
		int y;
        if ((y = sat[sprites_count++]) > 224)
            y -= 256;
        const int line = VDP_Line - y - 1;
        if (line >= 0 && line < sprites_height)
            sprites_on_line++;
    }

	// Assign to globals
    Sprite_Last = sprites_count - 1;
	Sprites_on_Line = sprites_on_line;
}

//-----------------------------------------------------------------------------
