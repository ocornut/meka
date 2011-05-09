//-----------------------------------------------------------------------------
// Meka - video_m5.h
// SMS/GG Video Mode Rendering - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

#ifdef X86_ASM
 extern int     Decode_Tile_ASM (int, byte *);
 extern int     Decode_Tile_ASM_Init (void);
 extern int     Find_Last_Sprite_ASM (int Height, int VDP_Line);
 extern int     Find_Last_Sprite_ASM_Wide (int Height, int VDP_Line);
 extern void    Sprite_Collide_Line_ASM (byte *p_src, int x);
#else
 void           Decode_Tile_C (int, byte *);
 void           Find_Last_Sprite_C (int Height, int VDP_Line);
 void           Find_Last_Sprite_C_Wide (int Height, int VDP_Line);
 void           Sprite_Collide_Line_C (byte *p_src, int x);
#endif

void            Display_BackGround_Line_5_C (void);

int             Sprite_Last;
int             Sprites_on_Line;

void            Refresh_Line_5 (void);
void            Refresh_Sprites_5 (bool draw);

//-----------------------------------------------------------------------------

void            VDP_Mode4_DrawTile(ALLEGRO_BITMAP *dst, const u8 *pixels, const u32 *palette_host, int x, int y, int flip);

//-----------------------------------------------------------------------------
