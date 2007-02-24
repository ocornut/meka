//-----------------------------------------------------------------------------
// MEKA - video_c.h
// Video / C Functions - Headers
//-----------------------------------------------------------------------------
// NOTE:
//  This file also contains definition pointing to X86 versions, if enabled.
//-----------------------------------------------------------------------------

#ifdef X86_ASM
 #define Decode_Tile(n)                 Decode_Tile_ASM (n, tgfx.Tile_Decoded[n])
 #define Find_Last_Sprite(wide,h,vline) { if (wide) Find_Last_Sprite_ASM_Wide (h, vline); else Find_Last_Sprite_ASM (h, vline); }
 #define Display_BackGround_Line_5      Display_BackGround_Line_5_C /* ASM version not yet supported */
 #define Sprite_Collide_Line            Sprite_Collide_Line_ASM
#else
 #define Decode_Tile(n)                 Decode_Tile_C (n, tgfx.Tile_Decoded[n])
 #define Find_Last_Sprite(wide,h,vline) { if (wide) Find_Last_Sprite_C_Wide (h, vline); else Find_Last_Sprite_C (h, vline); }
 #define Display_BackGround_Line_5      Display_BackGround_Line_5_C
 #define Sprite_Collide_Line            Sprite_Collide_Line_C
#endif

//-----------------------------------------------------------------------------

