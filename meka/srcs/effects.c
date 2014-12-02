//-----------------------------------------------------------------------------
// MEKA - effects.c
// Various effects - Code
//-----------------------------------------------------------------------------
// FIXME: merge with specials.* ?
// FIXME: This is super old code. Everything is nonsense from beginning to end. 
// I cannot read that anymore! Argh.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "palette.h"
#include "effects.h"
#include "video.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TV_EFFECT_COLORS_MAX				(12)	// FIXME: Changing this will break the code below!

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_tv_effect
{
	int		start_line;
	u16		colors[TV_EFFECT_COLORS_MAX];
};

static t_tv_effect	tv_effect;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Effects_TV_Init(void)
{
	tv_effect.start_line = SMS_RES_Y / 2;
}

void    Effects_TV_Reset(void)
{
    tv_effect.start_line = SMS_RES_Y / 2;
}

void    Effects_TV_Update (void)
{
	// Update color palette
	for (int i = 0; i != TV_EFFECT_COLORS_MAX; i++)
		tv_effect.colors[i] = Palette_MakeHostColor(g_screenbuffer_format, i * 16, i * 16, i * 16);

    const int rx = g_driver->x_res;
    const int ry = g_driver->y_res;
    const int rx_d2 = rx / 2;
    const int ry_d2 = ry / 2;

	assert(Screenbuffer_IsLocked());
	u16* screen_data = (u16*)g_screenbuffer_locked_region->data;
	const int screen_pitch16 = g_screenbuffer_locked_region->pitch / sizeof(u16);

	const int start_offset = (g_driver->y_show_start * screen_pitch16) + g_driver->x_start;

	int i, j, k;

	// Fill with random pixels
	// Same data in the four quarters (to reduce number of calls to Random().. this was somewhat an issue back on old 486).
    for (j = 0; j != ry_d2; j ++)
    {
        u16* p1 = (u16 *)(screen_data + j*screen_pitch16 + start_offset);
        u16* p2 = (u16 *)(screen_data + (j+ry_d2)*screen_pitch16 + start_offset);
        u16* p3 = p1 + rx_d2;
        u16* p4 = p2 + rx_d2;
        for (i = 0; i != rx_d2; i ++)
        {
			const u16 color = tv_effect.colors[RandomInt(TV_EFFECT_COLORS_MAX)];
            *p1++ = *p2++ = *p3++ = *p4++ = color;
        }
    }

	// Random lines
    for (i = 0; i < 25; i ++)
    {
		int len;
        const u16 color = tv_effect.colors[RandomInt(TV_EFFECT_COLORS_MAX)];
        j = RandomInt(rx); // x
        k = RandomInt(ry); // y
        len = RandomInt(30); // len
        j -= len;
        if (j < 0) 
			j = 0;
        u16* p1 = (u16 *)(screen_data + k*screen_pitch16 + j + start_offset);
        while (len-- != 0)
            *p1++ = color;
    }

    j = RandomInt(ry);
    k = RandomInt(16) + (rx - 16);
    i = RandomInt(16);
    u16* p1 = (u16 *)(screen_data + j*screen_pitch16 + i + start_offset);
    for (; i < k; i ++)
    {
		int r = RandomInt(16);
		if (r != 0)
			*p1 = tv_effect.colors[r % 3];
        p1 ++;
    }

    if (tv_effect.start_line > 0)
    {
        for (j = 0; j != tv_effect.start_line; j++)
        {
            u16* p1 = (u16 *)(screen_data + j*screen_pitch16 + start_offset);
            for (i = 0; i != rx; i++)
            {
                if (RandomInt(tv_effect.start_line) != 0)
                {
                    *p1 = tv_effect.colors[0];
                }
                p1 ++;
            }
        }
        for (j = ry - 1; j > (ry - tv_effect.start_line); j --)
        {
            u16* p1 = (u16 *)(screen_data + j*screen_pitch16 + start_offset);
            for (i = 0; i < rx; i ++)
            {
                if (RandomInt(tv_effect.start_line) != 0)
                {
                    *p1 = tv_effect.colors[0];
                }
                p1 ++;
            }
        }
        tv_effect.start_line -= 24;
    }
}

//-----------------------------------------------------------------------------

