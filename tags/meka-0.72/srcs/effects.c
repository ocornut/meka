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

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TV_EFFECT_COLORS_MAX				(12)	// FIXME: Changing this will break the code below!

#define EFFECT_TV_EMU_COLOR_START			(32)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
	int		start_line;
	u16		colors[TV_EFFECT_COLORS_MAX];
} t_tv_effect;

t_tv_effect	tv_effect;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Effects_TV_Init(void)
{
	int    i;
	tv_effect.start_line = SMS_RES_Y / 2;
	for (i = 0; i != TV_EFFECT_COLORS_MAX; i++)
		tv_effect.colors[i] = makecol16(i * 16, i * 16, i * 16);
}

void    Effects_TV_Reset(void)
{
    tv_effect.start_line = SMS_RES_Y / 2;
}

//-----------------------------------------------------------------------------
// Effects_TV_Update (void)
// Update TV Effect
//-----------------------------------------------------------------------------
#if 0
// Better rewrite, WIP
void    Effects_TV_Update(void)
{
	int y;
	const int res_x = cur_drv->x_res;
	const int res_y = cur_drv->y_res;
	const int start_x = cur_drv->x_start;
	const int start_y = cur_drv->y_show_start;

	for (y = 0; y != (res_y / 2); y++)
	{
		int x;
		u16 *	p1 = (u16 *)screenbuffer->line[start_y + y] + (start_x);
		for (x = 0; x != res_x; x++)
		{
			const int color = tv_effect.colors[Random(TV_EFFECT_COLORS_MAX)];
			////int i = Random(10);
			*p1++ = color;//makecol16(i * 16, i * 16, i * 16);;
		}
	}
}

#else
void    Effects_TV_Update (void)
{
    int    i, j, k;
    u16 *  p1, *p2, *p3, *p4;

    int    rx = cur_drv->x_res;
    int    ry = cur_drv->y_res;
    int    rx_d2 = rx / 2;
    int    ry_d2 = ry / 2;

    int start_offset = (cur_drv->y_show_start * (screenbuffer->line[1] - screenbuffer->line[0])) + (cur_drv->x_start * sizeof(u16));

	// Fill with random pixels
	// Same data in the four quarters (to reduce number of calls to Random().. this was somewhat an issue back on old 486).
    for (j = 0; j != ry_d2; j ++)
    {
        p1 = (u16 *)(screenbuffer->line [j] + start_offset);
        p2 = (u16 *)(screenbuffer->line [j + ry_d2] + start_offset);
        p3 = p1 + rx_d2;
        p4 = p2 + rx_d2;
        for (i = 0; i != rx_d2; i ++)
        {
			const u16 color = tv_effect.colors[Random(TV_EFFECT_COLORS_MAX)];
            *p1++ = *p2++ = *p3++ = *p4++ = color;
        }
    }

	// Random lines
    for (i = 0; i < 25; i ++)
    {
		int len;
        const u16 color = tv_effect.colors[Random(TV_EFFECT_COLORS_MAX)];
        j = Random(rx); // x
        k = Random(ry); // y
        len = Random(30); // len
        j -= len;
        if (j < 0) 
			j = 0;
        p1 = (u16 *)(screenbuffer->line [k] + (j * sizeof(u16)) + start_offset);
        while (len-- != 0)
            *p1++ = color;
    }

    j = Random(ry);
    k = Random(16) + (rx - 16);
    i = Random(16);
    p1 = (u16 *)(screenbuffer->line [j] + (i * sizeof(u16)) + start_offset);
    for (; i < k; i ++)
    {
		int r = Random(16);
		if (r != 0)
			*p1 = tv_effect.colors[r % 3];
        p1 ++;
    }

    if (tv_effect.start_line > 0)
    {
        for (j = 0; j != tv_effect.start_line; j++)
        {
            p1 = (u16 *)(screenbuffer->line[j] + start_offset);
            for (i = 0; i != rx; i++)
            {
                if (Random(tv_effect.start_line) != 0)
                {
                    *p1 = tv_effect.colors[0];
                }
                p1 ++;
            }
        }
        for (j = ry - 1; j > (ry - tv_effect.start_line); j --)
        {
            p1 = (u16 *)(screenbuffer->line[j] + start_offset);
            for (i = 0; i < rx; i ++)
            {
                if (Random(tv_effect.start_line) != 0)
                {
                    *p1 = tv_effect.colors[0];
                }
                p1 ++;
            }
        }
        tv_effect.start_line -= 24;
    }

}
#endif // 0

//-----------------------------------------------------------------------------

