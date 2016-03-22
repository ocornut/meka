//-----------------------------------------------------------------------------
// Meka - video_m5.h
// SMS/GG Video Mode Rendering - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Decode_Tile_C (int, byte *);
void    Sprite_Collide_Line_C (byte *p_src, int x);

void	Find_Last_Sprite(int sprites_height, int VDP_Line);
void    Find_Last_Sprite_Wide(int sprites_height, int VDP_Line);

void    Display_BackGround_Line_5(void);

extern "C"
{
extern int      Sprite_Last;
extern int      Sprites_on_Line;
}

void            Refresh_Line_5 (void);
void            Refresh_Sprites_5 (bool draw);

//-----------------------------------------------------------------------------

void            VDP_Mode4_DrawTile(ALLEGRO_BITMAP *dst, ALLEGRO_LOCKED_REGION* dst_region, const u8 *pixels, const u32 *palette_host, int x, int y, int flip);

//-----------------------------------------------------------------------------
