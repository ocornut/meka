//-----------------------------------------------------------------------------
// MEKA - video_c.c
// Video / C Functions - Code
//-----------------------------------------------------------------------------

//#ifndef X86_ASM

#include "shared.h"

//-----------------------------------------------------------------------------

void    Decode_Tile_C (int tile_n, byte *start)
{
    ConsolePrint ("video_c.c::Decode_Tile_C() is empty,\ntiles will not be showing.\n");
}

void    Find_Last_Sprite_C (int Height, int VDP_Line)
{
    int y;
    int line;

    Sprite_Last = 0;
    Sprites_on_Line = 0;
    while (Sprite_Last < 64)
    {
        if ((y = SPR_AREA [Sprite_Last]) == 208)
            break;
        Sprite_Last ++;
        if (y > 224) y -= 256;
        line = VDP_Line - y - 1;
        if (line >= 0 && line < Height)
            Sprites_on_Line ++;
    }
    Sprite_Last --;
}

void    Find_Last_Sprite_C_Wide (int Height, int VDP_Line)
{
    int y;
    int line;

    Sprite_Last = 0;
    Sprites_on_Line = 0;
    while (Sprite_Last < 64)
    {
        if ((y = SPR_AREA [Sprite_Last++]) > 224)
            y -= 256;
        line = VDP_Line - y - 1;
        if (line >= 0 && line < Height)
            Sprites_on_Line ++;
    }
    Sprite_Last --;
}

//#endif

//-----------------------------------------------------------------------------
