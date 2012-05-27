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

void	TMS9918_Palette_Setup(void);
extern ALLEGRO_COLOR	TMS9918_Palette[16];

//-----------------------------------------------------------------------------

void     VDP_Mode0123_DrawTile(ALLEGRO_BITMAP *dst, ALLEGRO_LOCKED_REGION* dst_region, int x, int y, const u8 *pixels_data, int fgcolor, int bgcolor);
void     VDP_Mode0123_DrawTile(ALLEGRO_BITMAP *dst, ALLEGRO_LOCKED_REGION* dst_region, int x, int y, const u8 *pixels_data, const u8 *colors_data);

//-----------------------------------------------------------------------------

