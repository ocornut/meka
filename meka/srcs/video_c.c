//-----------------------------------------------------------------------------
// MEKA - video_c.c
// Video / C Functions - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "video_m5.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Decode_Tile_C (int tile_n, u8* dst)
{
	const u8* src = &VRAM[tile_n << 5];
	
	// byte 0->3: bitplane x for line 0
	int lines = 8;
	while (lines-- != 0)
	{
		const u32 src0 = src[0];
		const u32 src1 = src[1];
		const u32 src2 = src[2];
		const u32 src3 = src[3];
		dst[0] = ((src0 & 0x80)>>7) | ((src1 & 0x80)>>6) | ((src2 & 0x80)>>5) | ((src3 & 0x80)>>4);
		dst[1] = ((src0 & 0x40)>>6) | ((src1 & 0x40)>>5) | ((src2 & 0x40)>>4) | ((src3 & 0x40)>>3);
		dst[2] = ((src0 & 0x20)>>5) | ((src1 & 0x20)>>4) | ((src2 & 0x20)>>3) | ((src3 & 0x20)>>2);
		dst[3] = ((src0 & 0x10)>>4) | ((src1 & 0x10)>>3) | ((src2 & 0x10)>>2) | ((src3 & 0x10)>>1);
		dst[4] = ((src0 & 0x08)>>3) | ((src1 & 0x08)>>2) | ((src2 & 0x08)>>1) | ((src3 & 0x08)   );
		dst[5] = ((src0 & 0x04)>>2) | ((src1 & 0x04)>>1) | ((src2 & 0x04)   ) | ((src3 & 0x04)<<1);
		dst[6] = ((src0 & 0x02)>>1) | ((src1 & 0x02)   ) | ((src2 & 0x02)<<1) | ((src3 & 0x02)<<2);
		dst[7] = ((src0 & 0x01)   ) | ((src1 & 0x01)<<1) | ((src2 & 0x01)<<2) | ((src3 & 0x01)<<3);
		src += 4;
		dst += 8;
	}
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
