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

static u8    Sprites_On_Line[192 + 32];

ALLEGRO_COLOR TMS9918_Palette[16] =
{
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

static u16*	GFX_ScreenData = NULL;
static int	GFX_ScreenPitch = 0;

#define PIXEL_TYPE              u16
#define PIXEL_PALETTE_TABLE     Palette_EmulationToHostGame

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TMS9918_Palette_Setup(void)
{
    int i;

    // Set TMS9918 emulation palette
    Palette_Emulation_SetColor(0, TMS9918_Palette[sms.VDP[7] & 15]);
    for (i = 1; i != 16; i++)
        Palette_Emulation_SetColor(i, TMS9918_Palette[i]);
}

// Note: this is used by tools only (not actual emulation refresh)
void    VDP_Mode0123_DrawTile(ALLEGRO_BITMAP *dst, ALLEGRO_LOCKED_REGION* dst_region, int x, int y, const u8 *pixels_data, int fgcolor_host, int bgcolor_host)
{
	const int color_format = al_get_bitmap_format(dst);
    switch (al_get_pixel_format_bits(color_format))
    {
    case 16:
        {
			u16* dst_data = (u16*)dst_region->data;
			const int dst_pitch = dst_region->pitch >> 1;
			const u16 fgcolor = fgcolor_host;
			const u16 bgcolor = bgcolor_host;
            for (int i = 0; i != 8; i++)
            {
                const u8 cc = *pixels_data++;
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
    case 32:
        {
			u32* dst_data = (u32*)dst_region->data;
			const int dst_pitch = dst_region->pitch >> 2;
			const u32 fgcolor = fgcolor_host;
			const u32 bgcolor = bgcolor_host;
            for (int i = 0; i != 8; i++)
            {
                const u8 cc = *pixels_data++;
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
		Msg(MSGT_USER, "video_m2: unsupported color format: %d", color_format);
        break;
    }
}

void    VDP_Mode0123_DrawTile(ALLEGRO_BITMAP *dst, ALLEGRO_LOCKED_REGION* dst_region, int x, int y, const u8 *pixels_data, const u8 *colors_data)
{
	const int color_format = al_get_bitmap_format(dst);
    switch (al_get_pixel_format_bits(color_format))
    {
    case 16:
        {
			u16* dst_data = (u16*)dst_region->data;
			const int dst_pitch = dst_region->pitch >> 1;
            for (int i = 0; i != 8; i++)
            {
                const u8 cc = *pixels_data++;
				const u8 color_indexes = *colors_data++;
				const u16 fgcolor = Palette_EmulationToHostGui[color_indexes >> 4];
				const u16 bgcolor = Palette_EmulationToHostGui[color_indexes & 0x0F];
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
    case 32:
        {
			u32* dst_data = (u32*)dst_region->data;
			const int dst_pitch = dst_region->pitch >> 2;
            for (int i = 0; i != 8; i++)
            {
                const u8 cc = *pixels_data++;
				const u8 color_indexes = *colors_data++;
				const u32 fgcolor = Palette_EmulationToHostGui[color_indexes >> 4];
				const u32 bgcolor = Palette_EmulationToHostGui[color_indexes & 0x0F];
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
		Msg(MSGT_USER, "video_m2: unsupported color format: %d", color_format);
        break;
    }
}

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
            const u8 *tile_n = g_machine.VDP.name_table_address + (j * 5);

            // 8 left pixels are black
            dst[0] = dst[1] = dst[2] = dst[3] = dst[4] = dst[5] = dst[6] = dst[7] = COLOR_BLACK16;    // FIXME-BORDER
            dst += 8;

            for (i = 8; i != (40 * 6) + 8; i += 6)
            {
                const u8 *p2  = g_machine.VDP.sg_pattern_gen_address + (*tile_n << 3) + k;
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

void    Display_Background_1 (void)
{
    int    i, j, j2;
    int    x, y = 0;
    const u8 *tile_n = g_machine.VDP.name_table_address;           //-- Tile Table --------//

    // DRAW ALL TILES ------------------------------------------------------------
    for (i = 0; i != 24; i++)
    {
        x = 0;
        for (j = 0; j != 32; j++)
        {
            // Draw one tile
            const u8 * p1 = g_machine.VDP.sg_pattern_gen_address  + (*tile_n << 3);
            const u8 * p2 = g_machine.VDP.sg_color_table_address + (*tile_n >> 3);
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

void    Display_Background_2 (void)
{
    const u8* pattern_name_table = g_machine.VDP.name_table_address;
    const int vsection_mask = sms.VDP[4] & 3;

    int y = 0;
    for (int vsection_idx = 0; vsection_idx < 3; vsection_idx++) // screen in 3 parts
    {
        const u8* tile_base = g_machine.VDP.sg_pattern_gen_address + ((vsection_idx & vsection_mask) * 0x800);	// Pattern data base
        const u8* col_base = g_machine.VDP.sg_color_table_address + ((vsection_idx & vsection_mask) * 0x800);	// Color table base
        for (int ty = 0; ty < 8; ty++)
        {
            int x = 0;
            for (int tx = 0; tx < 32; tx++)
            {
				const u32 char_name_value = (*pattern_name_table++);
				const u32 char_name_addr = char_name_value * 8;
                const u8* char_pattern_data = tile_base + char_name_addr;
                const u8* char_color_data = col_base  + char_name_addr;

                // Draw one tile
                for (int j2 = 0; j2 < 8; j2++)
                {
					PIXEL_TYPE *dst = GFX_ScreenData + GFX_ScreenPitch * (y+j2) + x;
                    const u8 pattern_8 = (*char_pattern_data++);
					const u8 color_indexes = (*char_color_data++);
                    const PIXEL_TYPE color1 = PIXEL_PALETTE_TABLE[color_indexes >> 4];
                    const PIXEL_TYPE color2 = PIXEL_PALETTE_TABLE[color_indexes & 0x0F];
                    dst[0] = (pattern_8 & 0x80) ? color1 : color2;
                    dst[1] = (pattern_8 & 0x40) ? color1 : color2;
                    dst[2] = (pattern_8 & 0x20) ? color1 : color2;
                    dst[3] = (pattern_8 & 0x10) ? color1 : color2;
                    dst[4] = (pattern_8 & 0x08) ? color1 : color2;
                    dst[5] = (pattern_8 & 0x04) ? color1 : color2;
                    dst[6] = (pattern_8 & 0x02) ? color1 : color2;
                    dst[7] = (pattern_8 & 0x01) ? color1 : color2;
                }
                x += 8;
            }
            y += 8;
        }
    }
}

void    Display_Background_3 (void)
{
    int         x, y, z;
    const u8 *  pattern = g_machine.VDP.name_table_address;

    for (y = 0; y != 192; y += 32)
    {
        for (x = 0; x != 256; x += 8)
        {
            const u8 *tiles_data = g_machine.VDP.sg_pattern_gen_address + (*pattern++ * 8);
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
void    Draw_Sprite_Mono_Double (const u8 *src, int x, int y, int fcolor_idx)
{
    if (fcolor_idx & 0x80) 
        x -= 32;
    fcolor_idx &= 0x0F;
    if (fcolor_idx == 0)
        return;
    const PIXEL_TYPE fcolor = PIXEL_PALETTE_TABLE[fcolor_idx];

    for (int j = 0; j != 8; j++, src++)
    {
        if (y < 0 || y > 190)
        {
            y += 2;
            continue;
        }
        else
        {
            const u8 src8 = *src;
            if (!(g_configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
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
            if (!(g_configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
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
void    Draw_Sprite_Mono (const u8 *src, int x, int y, int fcolor_idx)
{
    if (fcolor_idx & 0x80) 
        x -= 32;
    fcolor_idx &= 0x0F;
    if (fcolor_idx == 0)
        return;
    PIXEL_TYPE fcolor = PIXEL_PALETTE_TABLE[fcolor_idx];

    for (int j = 0; j != 8; j++, y++, src++)
    {
        if (y < 0 || y > 191)
            continue;
        if (!(g_configuration.sprite_flickering & SPRITE_FLICKERING_ENABLED) || Sprites_On_Line [y] <= 4)
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
    const int Sprite_Mode = Sprites_Double | Sprites_16x16;
    const int Mask = Table_Mask [Sprite_Mode];
    const int sprites_height = Table_Height [Sprite_Mode];

    // No sprites in Video Mode 0 (Text Mode)
    if (tsms.VDP_VideoMode == 0)
        return;

    memset (Sprites_On_Line, 0, 192 + 32);

    // Find last sprite
	const u8* sat = g_machine.VDP.sprite_attribute_table;
	int i;
    for (i = 0; i < 32 * 4; i += 4)
    {
        int y = sat[i];
        if ((y ++) == 0xD0) 
            break;
        if (y > 0xD0) y -= 0xFF;
        for (int j = y; j < y + sprites_height; j++)
            if (j >= 0)
                Sprites_On_Line [j]++;
    }
    i -= 4;

    // Display sprites -----------------------------------------------------------
    while (i >= 0)
    {
        // Calculate vertical position and handle special meanings ----------------
        int y = sat[i];
        if ((y ++) == 0xD0) 
            break;
        if (y > 0xD0) 
            y -= 0x100;
        // Calculate horizontal position ------------------------------------------
        int x = sat[i + 1];
        // Calculate tile starting address in VRAM --------------------------------
        const u8* k = (u8 *)((long int)((sat[i + 2] & Mask) << 3) + (long int)g_machine.VDP.sprite_pattern_gen_address);
        switch (Sprite_Mode)
        {
            // 8x8 (used in: Sokouban)
        case 0: //----------- address -- x position -- y position -- color
            Draw_Sprite_Mono (k, x, y, sat[i + 3]);
            break;
            // 16x16 - 8x8 Doubled (used in: ?)
        case 1: //----------- address -- x position -- y position -- color
            Draw_Sprite_Mono_Double (k, x, y, sat[i + 3]);
            break;
            // 16x16 (used in most games)
        case 2: //----------- address -- x position --- y position --- color -----
            Draw_Sprite_Mono (k,      x,     y,     sat[i + 3]);
            Draw_Sprite_Mono (k + 8,  x,     y + 8, sat[i + 3]);
            Draw_Sprite_Mono (k + 16, x + 8, y,     sat[i + 3]);
            Draw_Sprite_Mono (k + 24, x + 8, y + 8, sat[i + 3]);
            break;
        case 3: //------------------ address ---- x position ---- y position --- color ----
            Draw_Sprite_Mono_Double (k,      x,      y,      sat[i + 3]);
            Draw_Sprite_Mono_Double (k + 8,  x,      y + 16, sat[i + 3]);
            Draw_Sprite_Mono_Double (k + 16, x + 16, y,      sat[i + 3]);
            Draw_Sprite_Mono_Double (k + 24, x + 16, y + 16, sat[i + 3]);
            break;
        }
        i -= 4;
        // Decrease Sprites_On_Line values ----------------------------------------
        for (int j = y; j < y + sprites_height; j++) 
            if (j >= 0) 
                Sprites_On_Line [j]--;
    }
}

void    Refresh_Modes_0_1_2_3(void)
{
	GFX_ScreenData = (u16*)g_screenbuffer_locked_region->data;
	GFX_ScreenPitch = g_screenbuffer_locked_region->pitch / sizeof(u16);	// Pitch in u16 pixel unit to ease pointer manipulations

	// Display Background
    if (opt.Layer_Mask & LAYER_BACKGROUND)
    {
        if (Display_ON)
        {
            switch (tsms.VDP_VideoMode)
            {
            case 0: Display_Text_0(); break;
            case 1: Display_Background_1(); break;
            case 2: Display_Background_2(); break;
            case 3: Display_Background_3(); break;
            }
        }
        else
        {
            // Clear screen
			al_set_target_bitmap(screenbuffer);
			alx_locked_draw_filled_rectangle(g_screenbuffer_locked_region, 0, 0, SMS_RES_X, SMS_RES_Y, BORDER_COLOR);
        }
    }
    else
    {
        // Clear screen with yellow-ish color
		al_set_target_bitmap(screenbuffer);
		alx_locked_draw_filled_rectangle(g_screenbuffer_locked_region, 0, 0, SMS_RES_X, SMS_RES_Y, COLOR_DEBUG_BACKDROP);
    }
    // Display Sprites
    if ((opt.Layer_Mask & LAYER_SPRITES) && Display_ON)
    {
        Display_Sprites_1_2_3();
    }
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
        TileSrc = g_machine.VDP.sprite_pattern_gen_address | ((long)(SprSrc[2] & Mask) << 3);
        TileDst = g_machine.VDP.sprite_pattern_gen_address | ((long)(SprDst[2] & Mask) << 3);

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
           //Msg(MSGT_USER, "Sprites %d & %d Collide", n1, n2);
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
  const int   mask = Table_Mask [Sprites_Double | Sprites_16x16];
  const int   size = Table_Height [Sprites_Double | Sprites_16x16];

  int   src_n,     dst_n;
  int   src_y,     dst_y;
  const u8 * src_spr;
  const u8 * src_tile;
  const u8 * dst_spr;
  const u8 * dst_tile;

  int   delta_x;
  int   delta_y;

  const u8* sat = g_machine.VDP.sprite_attribute_table;
  for (src_n = 0, src_spr = sat, src_y = src_spr[0]; src_n < 31 && src_y != 208;
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
        //if (delta_y >= size || delta_y <= -size)
        //   Msg(MSGT_USER, "delta_y = %d", delta_y);

        // Compute delta_x, skip if the sprites cannot overlap
        delta_x = src_spr[1] - dst_spr[1];
        if (delta_x >= size || delta_x <= -size)
           continue;

        // Prepare pointers to the first tile of each sprite
        src_tile = (u8 *)((long int)g_machine.VDP.sprite_pattern_gen_address | ((long int)(src_spr[2] & mask) << 3));
        dst_tile = (u8 *)((long int)g_machine.VDP.sprite_pattern_gen_address | ((long int)(dst_spr[2] & mask) << 3));

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
			const u8* tmp = src_tile;
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
           // Msg(MSGT_USER, "Sprites %d & %d Collide", src_n, dst_n);
           sms.VDP_Status |= VDP_STATUS_SpriteCollision; // Set VDP Status Collision Bit
           return;
           }

        }
     }
}

//-----------------------------------------------------------------------------

