//-----------------------------------------------------------------------------
// MEKA - video_m2.c
// TMS9918 Video Modes Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "palette.h"
#include "vdp.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

static u8    Sprites_On_Line [192 + 32];

// FIXME-ALLEGRO5: Palette is in float format, must be converted
const ALLEGRO_COLOR TMS9918_Palette [16] =
 {
     // FIXME: Proper palette
   /* 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xC0, 0x20,
   0x60, 0xE0, 0x60, 0x20, 0x20, 0xE0, 0x40, 0x60, 0xE0,
   0xA0, 0x20, 0x20, 0x40, 0xC0, 0xE0, 0xE0, 0x20, 0x20,
   0xE0, 0x60, 0x60, 0xC0, 0xC0, 0x20, 0xC0, 0xC0, 0x80,
   0x20, 0x80, 0x20, 0xC0, 0x40, 0xA0, 0xA0, 0xA0, 0xA0,
   0xE0, 0xE0, 0xE0, */
   { (4*0x00)/255.0f, (4*0x00)/255.0f, (4*0x00)/255.0f, 1.0f }, /*  0: Transparent   */
   { (4*0x00)/255.0f, (4*0x00)/255.0f, (4*0x00)/255.0f, 1.0f }, /*  1: Black         */
   { (4*0x08)/255.0f, (4*0x30)/255.0f, (4*0x08)/255.0f, 1.0f }, /*  2: Medium Green  */
   { (4*0x18)/255.0f, (4*0x38)/255.0f, (4*0x18)/255.0f, 1.0f }, /*  3: Light Green   */
   { (4*0x08)/255.0f, (4*0x08)/255.0f, (4*0x38)/255.0f, 1.0f }, /*  4: Dark Blue     */
   { (4*0x10)/255.0f, (4*0x18)/255.0f, (4*0x38)/255.0f, 1.0f }, /*  5: Light Blue    */
   { (4*0x28)/255.0f, (4*0x08)/255.0f, (4*0x08)/255.0f, 1.0f }, /*  6: Dark Red      */
   { (4*0x10)/255.0f, (4*0x30)/255.0f, (4*0x38)/255.0f, 1.0f }, /*  7: Cyan          */
   { (4*0x38)/255.0f, (4*0x08)/255.0f, (4*0x08)/255.0f, 1.0f }, /*  8: Medium Red    */
   { (4*0x38)/255.0f, (4*0x18)/255.0f, (4*0x18)/255.0f, 1.0f }, /*  9: Light Red     */
   { (4*0x30)/255.0f, (4*0x30)/255.0f, (4*0x08)/255.0f, 1.0f }, /* 10: Dark Yellow   */
   { (4*0x30)/255.0f, (4*0x30)/255.0f, (4*0x20)/255.0f, 1.0f }, /* 11: Light Yellow  */
   { (4*0x08)/255.0f, (4*0x20)/255.0f, (4*0x08)/255.0f, 1.0f }, /* 12: Dark Green    */
   { (4*0x30)/255.0f, (4*0x10)/255.0f, (4*0x28)/255.0f, 1.0f }, /* 13: Magenta       */
   { (4*0x28)/255.0f, (4*0x28)/255.0f, (4*0x28)/255.0f, 1.0f }, /* 14: Grey          */
   { (4*0x38)/255.0f, (4*0x38)/255.0f, (4*0x38)/255.0f, 1.0f }  /* 15: White         */
 };

//-----------------------------------------------------------------------------
// Color configuration
// (Note: absolutly uses those define, so it'll be easier to switch to direct
//  24/32 output someday)
//-----------------------------------------------------------------------------

static ALLEGRO_LOCKED_REGION*	GFX_ScreenRegion = NULL;
static u16*						GFX_ScreenData = NULL;
static int						GFX_ScreenPitch = 0;

#define PIXEL_TYPE              u16
#define PIXEL_PALETTE_TABLE     Palette_EmulationToHost16

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TMS9918_Palette_Set ()
// Setup TMS9918 palette
//-----------------------------------------------------------------------------
void    TMS9918_Palette_Set (void)
{
    int i;

    // Set TMS9918 emulation palette
    Palette_Emulation_SetColor(0, TMS9918_Palette[sms.VDP[7] & 15]);
    for (i = 1; i != 16; i++)
        Palette_Emulation_SetColor(i, TMS9918_Palette[i]);
}

// Note: this is used by tools only (not actual emulation refresh)
void    VDP_Mode0123_DrawTile(ALLEGRO_BITMAP *dst, const u8 *pixels, int x, int y, int fgcolor, int bgcolor)
{
	const ALLEGRO_PIXEL_FORMAT color_format = al_get_bitmap_format(dst);
	ALLEGRO_LOCKED_REGION* dst_region = al_lock_bitmap(dst, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    switch (color_format)
    {
    case ALLEGRO_PIXEL_FORMAT_RGB_565:
        {
			u16* dst_data = (u16*)dst_region->data;
			const int dst_pitch = dst_region->pitch >> 1;
            int i;
            for (i = 0; i != 8; i++)
            {
                const u8 cc = *pixels++;
                u16 *dst8 = dst_data + (dst_pitch*y) + x;
                dst8[0] = (cc & 0x80) ? fgcolor : bgcolor;
                dst8[1] = (cc & 0x40) ? fgcolor : bgcolor;
                dst8[2] = (cc & 0x20) ? fgcolor : bgcolor;
                dst8[3] = (cc & 0x10) ? fgcolor : bgcolor;
                dst8[4] = (cc & 0x08) ? fgcolor : bgcolor;
                dst8[5] = (cc & 0x04) ? fgcolor : bgcolor;
                dst8[6] = (cc & 0x02) ? fgcolor : bgcolor;
                dst8[7] = (cc & 0x01) ? fgcolor : bgcolor;
                y++;
            }
            break;
        }
    case ALLEGRO_PIXEL_FORMAT_RGBA_8888:
        {
			u32* dst_data = (u32*)dst_region->data;
			const int dst_pitch = dst_region->pitch >> 2;
            int i;
            for (i = 0; i != 8; i++)
            {
                const u8 cc = *pixels++;
                u32 *dst8 = dst_data + (dst_pitch*y) + x;
                dst8[0] = (cc & 0x80) ? fgcolor : bgcolor;
                dst8[1] = (cc & 0x40) ? fgcolor : bgcolor;
                dst8[2] = (cc & 0x20) ? fgcolor : bgcolor;
                dst8[3] = (cc & 0x10) ? fgcolor : bgcolor;
                dst8[4] = (cc & 0x08) ? fgcolor : bgcolor;
                dst8[5] = (cc & 0x04) ? fgcolor : bgcolor;
                dst8[6] = (cc & 0x02) ? fgcolor : bgcolor;
                dst8[7] = (cc & 0x01) ? fgcolor : bgcolor;
                y++;
            }
            break;
        }
    default:
        assert(0);
        Msg(MSGT_USER, "video_m2: unsupported color format!");
        break;
    }
	al_unlock_bitmap(dst);
}

// DISPLAY TEXT MODE 0 SCREEN -------------------------------------------------
void    Display_Text_0 (void)
{
    int         i, j;
    const PIXEL_TYPE fgcolor = PIXEL_PALETTE_TABLE[sms.VDP[7] >> 4];
    const PIXEL_TYPE bgcolor = PIXEL_PALETTE_TABLE[0];

    for (j = 0; j != (24 * 8); j += 8)
    {
        int k;
        for (k = 0; k != 8; k ++)
        {
            PIXEL_TYPE *dst = GFX_ScreenData + GFX_ScreenPitch*(j + k);
            const u8 *tile_n = BACK_AREA + (j * 5);

            // 8 left pixels are black
            dst[0] = dst[1] = dst[2] = dst[3] = dst[4] = dst[5] = dst[6] = dst[7] = COLOR_BLACK16;    // FIXME-BORDER
            dst += 8;

            for (i = 8; i != (40 * 6) + 8; i += 6)
            {
                const u8 *p2  = SG_BACK_TILE + (*tile_n << 3) + k;
                const u8 src6 = *p2;
                dst[0] = (src6 & 0x80) ? fgcolor : bgcolor;
                dst[1] = (src6 & 0x40) ? fgcolor : bgcolor;
                dst[2] = (src6 & 0x20) ? fgcolor : bgcolor;
                dst[3] = (src6 & 0x10) ? fgcolor : bgcolor;
                dst[4] = (src6 & 0x08) ? fgcolor : bgcolor;
                dst[5] = (src6 & 0x04) ? fgcolor : bgcolor;
                dst += 6;
                tile_n++;
            }

            // 8 right pixels are black
            dst[0] = dst[1] = dst[2] = dst[3] = dst[4] = dst[5] = dst[6] = dst[7] = COLOR_BLACK16;    // FIXME-BORDER
        }
    }
}

// DISPLAY BACKGROUND VIDEO MODE 1 --------------------------------------------
void    Display_Background_1 (void)
{
    int    i, j, j2;
    int    x, y = 0;
    const u8 *tile_n = BACK_AREA;           //-- Tile Table --------//

    // DRAW ALL TILES ------------------------------------------------------------
    for (i = 0; i != 24; i++)
    {
        x = 0;
        for (j = 0; j != 32; j++)
        {
            // Draw one tile
            const u8 * p1 = SG_BACK_TILE  + (*tile_n << 3);
            const u8 * p2 = SG_BACK_COLOR + (*tile_n >> 3);
            const PIXEL_TYPE color1 = PIXEL_PALETTE_TABLE[*p2 >> 4];
            const PIXEL_TYPE color2 = PIXEL_PALETTE_TABLE[(*p2) & 0x0F];
            for (j2 = 0; j2 != 8; j2++)
            {
                const u8    src8 = *p1++;
                PIXEL_TYPE *dst8 = GFX_ScreenData + GFX_ScreenPitch*(y + j2) + x;
                dst8[0] = (src8 & 0x80) ? color1 : color2;
                dst8[1] = (src8 & 0x40) ? color1 : color2;
                dst8[2] = (src8 & 0x20) ? color1 : color2;
                dst8[3] = (src8 & 0x10) ? color1 : color2;
                dst8[4] = (src8 & 0x08) ? color1 : color2;
                dst8[5] = (src8 & 0x04) ? color1 : color2;
                dst8[6] = (src8 & 0x02) ? color1 : color2;
                dst8[7] = (src8 & 0x01) ? color1 : color2;
            }
            tile_n++;
            x += 8;
        }
        y += 8;
    }
}

// DISPLAY BACKGROUND VIDEO MODE 2 --------------------------------------------
void    Display_Background_2 (void)
{
    int    i, j, j2, k;

    byte   *col_base;                     //-- Color Table Base --//
    byte   *tile_base;                    //-- Tile Data Base ----//
    byte   *p1,                           //-- Tile Data ---------//
           *p2,                           //-- Color Table -------//
           *p4 = BACK_AREA;               //-- Tile Table --------//

    register int x, y;

    int mask = sms.VDP [4] & 3;

    // DRAW ALL TILES ------------------------------------------------------------
    y = 0;
    for (k = 0; k < 3; k ++)
    {
        col_base = SG_BACK_COLOR + ((k & mask) * 0x800);
        tile_base = SG_BACK_TILE + ((k & mask) * 0x800);
        for (i = 0; i < 8; i++)
        {
            x = 0;
            for (j = 0; j < 32; j++)
            {
                p1 = tile_base + (*p4 * 8);
                p2 = col_base  + (*p4++ * 8);

                // Draw one tile
                for (j2 = 0; j2 < 8; j2 ++, p2++)
                {
                    PIXEL_TYPE *dst = GFX_ScreenData + GFX_ScreenPitch * (y + j2) + x;
                    const PIXEL_TYPE color1 = PIXEL_PALETTE_TABLE[*p2 >> 4];
                    const PIXEL_TYPE color2 = PIXEL_PALETTE_TABLE[(*p2) & 0x0F];
                    const u8 cc = (*p1++);
                    dst[0] = (cc & 0x80) ? color1 : color2;
                    dst[1] = (cc & 0x40) ? color1 : color2;
                    dst[2] = (cc & 0x20) ? color1 : color2;
                    dst[3] = (cc & 0x10) ? color1 : color2;
                    dst[4] = (cc & 0x08) ? color1 : color2;
                    dst[5] = (cc & 0x04) ? color1 : color2;
                    dst[6] = (cc & 0x02) ? color1 : color2;
                    dst[7] = (cc & 0x01) ? color1 : color2;
                }
                x += 8;
            }
            y += 8;
        }
    }
}

// DISPLAY BACKGROUND VIDEO MODE 3 --------------------------------------------
void    Display_Background_3 (void)
{
    int         x, y, z;
    const u8 *  pattern = BACK_AREA;

    for (y = 0; y != 192; y += 32)
    {
        for (x = 0; x != 256; x += 8)
        {
            const u8 *tiles_data = SG_BACK_TILE + (*pattern++ * 8);
            for (z = 0; z != 8; z ++)
            {
                PIXEL_TYPE *dst;
                const PIXEL_TYPE color1 = PIXEL_PALETTE_TABLE[*tiles_data >> 4];
                const PIXEL_TYPE color2 = PIXEL_PALETTE_TABLE[*tiles_data & 0x0F];
                
                dst = GFX_ScreenData + GFX_ScreenPitch * (y + 0) + x;
                dst[0] = dst[1] = dst[2] = dst[3] = color1;
                dst[4] = dst[5] = dst[6] = dst[7] = color2;
                dst = GFX_ScreenData + GFX_ScreenPitch * (y + 1) + x;
                dst[0] = dst[1] = dst[2] = dst[3] = color1;
                dst[4] = dst[5] = dst[6] = dst[7] = color2;
                dst = GFX_ScreenData + GFX_ScreenPitch * (y + 2) + x;
                dst[0] = dst[1] = dst[2] = dst[3] = color1;
                dst[4] = dst[5] = dst[6] = dst[7] = color2;
                dst = GFX_ScreenData + GFX_ScreenPitch * (y + 3) + x;
                dst[0] = dst[1] = dst[2] = dst[3] = color1;
                dst[4] = dst[5] = dst[6] = dst[7] = color2;
                y += 4;
                tiles_data++;
            }
            y -= 32;
        }
        pattern += 96;
    }
}

// DRAW A MAGNIFIED MONOCHROME SPRITE TILE ------------------------------------
void    Draw_Sprite_Mono_Double (u8 *src, int x, int y, int fcolor_idx)
{
    int         j;
    PIXEL_TYPE  fcolor;

    if (fcolor_idx & 0x80) 
        x -= 32;
    fcolor_idx &= 0x0F;
    if (fcolor_idx == 0)
        return;
    fcolor = PIXEL_PALETTE_TABLE[fcolor_idx];

    for (j = 0; j != 8; j++, src++)
    {
        if (y < 0 || y > 190)
        {
            y += 2;
            continue;
        }
        else
        {
            const u8 src8 = *src;
            if (!(g_Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
            {
                PIXEL_TYPE* dst8 = GFX_ScreenData + GFX_ScreenPitch * y + x;
                if  (src8 & 0x80) { dst8[0]  = dst8[1]  = fcolor; }
                if  (src8 & 0x40) { dst8[2]  = dst8[3]  = fcolor; }
                if  (src8 & 0x20) { dst8[4]  = dst8[5]  = fcolor; }
                if  (src8 & 0x10) { dst8[6]  = dst8[7]  = fcolor; }
                if  (src8 & 0x08) { dst8[8]  = dst8[9]  = fcolor; }
                if  (src8 & 0x04) { dst8[10] = dst8[11] = fcolor; }
                if  (src8 & 0x02) { dst8[12] = dst8[13] = fcolor; }
                if  (src8 & 0x01) { dst8[14] = dst8[15] = fcolor; }
            }
            // if (Sprites_On_Line [y] == 5) { }
            y++;
            if (!(g_Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
            {
                PIXEL_TYPE* dst8 = GFX_ScreenData + GFX_ScreenPitch * y + x;
                if  (src8 & 0x80) { dst8[0]  = dst8[1]  = fcolor; }
                if  (src8 & 0x40) { dst8[2]  = dst8[3]  = fcolor; }
                if  (src8 & 0x20) { dst8[4]  = dst8[5]  = fcolor; }
                if  (src8 & 0x10) { dst8[6]  = dst8[7]  = fcolor; }
                if  (src8 & 0x08) { dst8[8]  = dst8[9]  = fcolor; }
                if  (src8 & 0x04) { dst8[10] = dst8[11] = fcolor; }
                if  (src8 & 0x02) { dst8[12] = dst8[13] = fcolor; }
                if  (src8 & 0x01) { dst8[14] = dst8[15] = fcolor; }
            }
            // if (Sprites_On_Line [y] == 5) { }
            y++;
        }
    }
}

// DRAW A MONOCHROME SPRITE TILE ----------------------------------------------
void    Draw_Sprite_Mono (u8 *src, int x, int y, int fcolor_idx)
{
    int         j;
    PIXEL_TYPE  fcolor;

    if (fcolor_idx & 0x80) 
        x -= 32;
    fcolor_idx &= 0x0F;
    if (fcolor_idx == 0)
        return;
    fcolor = PIXEL_PALETTE_TABLE[fcolor_idx];

    for (j = 0; j != 8; j++, y++, src++)
    {
        if (y < 0 || y > 191)
            continue;
        if (!(g_Configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
        {
            const u8     src8 = *src;
            PIXEL_TYPE* dst8 = GFX_ScreenData + GFX_ScreenPitch * y + x;
            if  (src8 & 0x80) dst8[0] = fcolor; 
            if  (src8 & 0x40) dst8[1] = fcolor; 
            if  (src8 & 0x20) dst8[2] = fcolor; 
            if  (src8 & 0x10) dst8[3] = fcolor; 
            if  (src8 & 0x08) dst8[4] = fcolor; 
            if  (src8 & 0x04) dst8[5] = fcolor; 
            if  (src8 & 0x02) dst8[6] = fcolor; 
            if  (src8 & 0x01) dst8[7] = fcolor; 
        }
        // if (Sprites_On_Line [y] == 5) { }
    }
}

static const int Table_Height [4] = { 8, 16, 16, 32 };
static const int Table_Mask [4] =   { 0xFF, 0xFF, 0xFC, 0xFC };

// DISPLAY SPRITES IN VIDEO MODE 1/2/3 ----------------------------------------
void    Display_Sprites_1_2_3 (void)
{
    u8 *    k;
    int     i, j;
    int     x, y;
    const int Sprite_Mode = Sprites_Double | Sprites_16x16;
    const int Mask = Table_Mask [Sprite_Mode];
    const int sprites_height = Table_Height [Sprite_Mode];

    // No sprites in Video Mode 0 (Text Mode)
    if (tsms.VDP_VideoMode == 0)
        return;

    memset (Sprites_On_Line, 0, 192 + 32);

    // Find last sprite
    for (i = 0; i < 32 * 4; i += 4)
    {
        y = sprite_attribute_table[i];
        if ((y ++) == 0xD0) 
            break;
        if (y > 0xD0) y -= 0xFF;
        for (j = y; j < y + sprites_height; j++)
            if (j >= 0)
                Sprites_On_Line [j]++;
    }
    i -= 4;

    // Display sprites -----------------------------------------------------------
    while (i >= 0)
    {
        // Calculate vertical position and handle special meanings ----------------
        y = sprite_attribute_table[i];
        if ((y ++) == 0xD0) 
            break;
        if (y > 0xD0) 
            y -= 0x100;
        // Calculate horizontal position ------------------------------------------
        x = sprite_attribute_table[i + 1];
        // Calculate tile starting address in VRAM --------------------------------
        k = (u8 *)((int)((sprite_attribute_table[i + 2] & Mask) << 3) + (int)cur_machine.VDP.sprite_pattern_base_address);
        switch (Sprite_Mode)
        {
            // 8x8 (used in: Sokouban)
        case 0: //----------- address -- x position -- y position -- color
            Draw_Sprite_Mono (k, x, y, sprite_attribute_table[i + 3]);
            break;
            // 16x16 - 8x8 Doubled (used in: ?)
        case 1: //----------- address -- x position -- y position -- color
            Draw_Sprite_Mono_Double (k, x, y, sprite_attribute_table[i + 3]);
            break;
            // 16x16 (used in most games)
        case 2: //----------- address -- x position --- y position --- color -----
            Draw_Sprite_Mono (k,      x,     y,     sprite_attribute_table[i + 3]);
            Draw_Sprite_Mono (k + 8,  x,     y + 8, sprite_attribute_table[i + 3]);
            Draw_Sprite_Mono (k + 16, x + 8, y,     sprite_attribute_table[i + 3]);
            Draw_Sprite_Mono (k + 24, x + 8, y + 8, sprite_attribute_table[i + 3]);
            break;
        case 3: //------------------ address ---- x position ---- y position --- color ----
            Draw_Sprite_Mono_Double (k,      x,      y,      sprite_attribute_table[i + 3]);
            Draw_Sprite_Mono_Double (k + 8,  x,      y + 16, sprite_attribute_table[i + 3]);
            Draw_Sprite_Mono_Double (k + 16, x + 16, y,      sprite_attribute_table[i + 3]);
            Draw_Sprite_Mono_Double (k + 24, x + 16, y + 16, sprite_attribute_table[i + 3]);
            break;
        }
        i -= 4;
        // Decrease Sprites_On_Line values ----------------------------------------
        for (j = y; j < y + sprites_height; j++) 
            if (j >= 0) 
                Sprites_On_Line [j]--;
    }
}

void    Refresh_Modes_0_1_2_3 (void)
{
	GFX_ScreenRegion = al_lock_bitmap(screenbuffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
	GFX_ScreenData = GFX_ScreenRegion->data;
	GFX_ScreenPitch = GFX_ScreenRegion->pitch;

	// Display Background
    if (opt.Layer_Mask & LAYER_BACKGROUND)
    {
        if (Display_ON)
        {
            switch (tsms.VDP_VideoMode)
            {
            case 0: Display_Text_0 (); break;
            case 1: Display_Background_1 (); break;
            case 2: Display_Background_2 (); break;
            case 3: Display_Background_3 (); break;
            }
        }
        else
        {
            // Clear screen
			al_set_target_bitmap(screenbuffer);
            al_clear_to_color(Border_Color);
        }
    }
    else
    {
        // Clear screen with yellow-ish color
		// FIXME-ALLEGRO5: color value
		al_set_target_bitmap(screenbuffer);
		al_clear_to_color(al_map_rgb(200,200,0));
        //clear_to_color (screenbuffer, 95);  // see video_m5.c [20050403] For sprite ripping
    }
    // Display Sprites
    if ((opt.Layer_Mask & LAYER_SPRITES) && Display_ON)
    {
        Display_Sprites_1_2_3 ();
    }

	al_unlock_bitmap(screenbuffer);
}

/*
**     _____
**    |     |
**  1 |   __|__
**    |__|__|  |
**       |     | 2
**       |_____|
**
*/
// FIXME: Zoomed sprites are actually not handled well in the collision tests
/*
void    Check_Sprites_Collision_Modes_1_2_3 (void)
{
 int    Mask = Table_Mask [Sprites_Double | Sprites_16x16];
 int    Size = Table_Height [Sprites_Double | Sprites_16x16];
 byte   Sprite_Last;
 byte   n1, n2, dx, dy;
 byte   *SprSrc, *SprDst;
 byte   *TileSrc, *TileDst, *TileTmp;

 Sprite_Last = 0;
 SprSrc = sprite_attribute_table;
 while (Sprite_Last < 32 && SprSrc[0] != 208)
    { Sprite_Last++; SprSrc += 4; }

 for (n1 = 0, SprSrc = sprite_attribute_table; n1 < Sprite_Last; n1++, SprSrc += 4)
     {
     // Skip if this sprite is not drawn
     if ((SprSrc[3] & 0x0F) == 0)
        continue;

     for (n2 = n1 + 1, SprDst = SprSrc + 4; n2 < Sprite_Last; n2++, SprDst += 4)
        {
        // Skip if this sprite is not drawn
        if ((SprDst[3] & 0x0F) == 0)
           continue;

        // Compute delta-y between the two sprites, skip if they cannot overlap
        dy = SprSrc[0] - SprDst[0];
        if ((dy >= Size) && (dy <= 256 - Size))
           continue;

        // Compute delta-x between the two sprites, skip if they cannot overlap
        dx = SprSrc[1] - SprDst[1];
        if ((dx >= Size) && (dx <= 256 - Size))
           continue;

        // Prepare pointers to the first tile line of each sprite
        TileSrc = cur_machine.VDP.sprite_pattern_base_address | ((long)(SprSrc[2] & Mask) << 3);
        TileDst = cur_machine.VDP.sprite_pattern_base_address | ((long)(SprDst[2] & Mask) << 3);

        if (dy < Size)
           {
           TileDst += dy;
           }
        else
           {
           dy = 256 - dy;
           TileSrc += dy;
           }
        if (dx > 256 - Size)
           {
           dx = 256 - dx;
           TileTmp = TileSrc; TileSrc = TileDst; TileDst = TileTmp;
           }

        // Finally, attempt to compare actual sprites pixels
        while (dy < Size)
           {
           if (Size == 8)
              {
              if (*TileDst & (*TileSrc >> dx))
                 break;
              }
           else
              {
              if ( (((word)*TileDst << 8) + *(TileDst + 16))
               & ((((word)*TileSrc << 8) + *(TileSrc + 16)) >> dx) )
                 break;
              }
           dy++;
           TileSrc++;
           TileDst++;
           }

        if (dy < Size)
           {
           //Msg (MSGT_USER, "Sprites %d & %d Collide", n1, n2);
           sms.VDP_Status |= VDP_STATUS_SpriteCollision; // Set VDP Status Collision Bit
           return;
           }
        }
    }
}
*/

// FIXME: Zoomed sprites are actually not handled well in the collision tests
void    Check_Sprites_Collision_Modes_1_2_3_Line (int line)
{
  int   mask = Table_Mask [Sprites_Double | Sprites_16x16];
  int   size = Table_Height [Sprites_Double | Sprites_16x16];

  int   src_n,     dst_n;
  int   src_y,     dst_y;
  u8 *	src_spr,  *dst_spr;
  u8 *	src_tile, *dst_tile;

  int   delta_x;
  int   delta_y;

  for (src_n = 0, src_spr = sprite_attribute_table, src_y = src_spr[0];
       src_n < 31 && src_y != 208;
       src_n++, src_spr += 4, src_y = src_spr[0])
     {
     // Skip if this sprite does not cover current line
     if (line < src_y || line >= src_y + size)
        continue;

     // Skip if this sprite is not shown (color 0)
     if ((src_spr[3] & 0x0F) == 0)
        continue;

     // Now for all other sprites to compare with...
     for (dst_n = src_n + 1, dst_spr = src_spr + 4, dst_y = dst_spr[0];
          dst_n < 32 && dst_y != 208;
          dst_n++, dst_spr += 4, dst_y = dst_spr[0])
        {
        // Skip if this sprite does not cover current line
        if (line < dst_y || line >= dst_y + size)
           continue;

        // Skip if this sprite is not shown (color 0)
        if ((dst_spr[3] & 0x0F) == 0)
           continue;

        // Compare delta_y
        delta_y = src_y - dst_y;
        if (delta_y >= size || delta_y <= -size)
           Msg (MSGT_USER, "delta_y = %d", delta_y);

        // Compute delta_x, skip if the sprites cannot overlap
        delta_x = src_spr[1] - dst_spr[1];
        if (delta_x >= size || delta_x <= -size)
           continue;

        // Prepare pointers to the first tile of each sprite
        src_tile = (u8 *)((int)cur_machine.VDP.sprite_pattern_base_address | ((int)(src_spr[2] & mask) << 3));
        dst_tile = (u8 *)((int)cur_machine.VDP.sprite_pattern_base_address | ((int)(dst_spr[2] & mask) << 3));

        // Offset those pointers to the first tile line
        if (delta_y > 0)
           dst_tile += delta_y;
        else
        if (delta_y < 0)
        {
           delta_y = -delta_y;
           src_tile += delta_y;
        }

        // Inverse sprites if delta_x < 0 for the purpose of the comparaison
        if (delta_x < 0)
           {
           byte *tmp = src_tile;
           src_tile  = dst_tile;
           dst_tile  = tmp;
           delta_x = -delta_x;
           }

        // Finally compare actual sprites pixels
        if (size == 8)
           {
           while (delta_y < 8)
              {
              if (*dst_tile++ & (*src_tile++ >> delta_x))
                 break;
              delta_y++;
              }
           }
        else
           {
           while (delta_y < size)
              {
              if ( (((word)*dst_tile << 8) | *(dst_tile + 16))
                & ((((word)*src_tile << 8) | *(src_tile + 16)) >> delta_x) )
                 break;
              delta_y++;
              dst_tile++;
              src_tile++;
              }
           }

        if (delta_y < size)
           {
           // Msg (MSGT_USER, "Sprites %d & %d Collide", src_n, dst_n);
           sms.VDP_Status |= VDP_STATUS_SpriteCollision; // Set VDP Status Collision Bit
           return;
           }

        }
     }
}

//-----------------------------------------------------------------------------

