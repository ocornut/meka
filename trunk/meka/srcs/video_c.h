//-----------------------------------------------------------------------------
// MEKA - video_c.h
// Video / C Functions - Headers
//-----------------------------------------------------------------------------
// NOTE:
//  This file also contains definition pointing to X86 versions, if enabled.
//-----------------------------------------------------------------------------

#ifdef X86_ASM
 #define Decode_Tile(n)                 Decode_Tile_ASM (n, tgfx.Tile_Decoded[n])
 #define Sprite_Collide_Line            Sprite_Collide_Line_ASM
#else
 #define Decode_Tile(n)                 Decode_Tile_C (n, tgfx.Tile_Decoded[n])
 #define Sprite_Collide_Line            Sprite_Collide_Line_C
#endif

//-----------------------------------------------------------------------------

