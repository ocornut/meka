//-----------------------------------------------------------------------------
// MEKA - video_m2.h
// TMS9918 Video Modes Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Display_Text_0             (void);
void    Display_Sprites_1_2_3      (void);
void    Display_Background_1       (void);
void    Display_Background_2       (void);
void    Display_Background_3       (void);
void    Refresh_Modes_0_1_2_3      (void);

//void    Check_Sprites_Collision_Modes_1_2_3 (void);
void    Check_Sprites_Collision_Modes_1_2_3_Line (int line);

void                    TMS9918_Palette_Set        (void);
extern const RGB        TMS9918_Palette [16];

void    Draw_Tile_Mono             (BITMAP *where, byte *addr, int x, int y, byte fcolor, byte bgcolor);
void    Draw_Sprite_Mono           (BITMAP *where, byte *addr, int x, int y, byte fcolor);
void    Draw_Sprite_Mono_Double    (BITMAP *where, byte *addr, int x, int y, byte fcolor);

//-----------------------------------------------------------------------------

