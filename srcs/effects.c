//-----------------------------------------------------------------------------
// MEKA - effects.c
// Various effects - Code
//-----------------------------------------------------------------------------
// FIXME: merge with specials.* ?
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define EFFECT_TV_EMU_COLOR_START        (32)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Effects_Init_Values (void)
{
 effects.TV_Enabled    = YES;
 effects.TV_Start_Line = 192 / 2;
}

//-----------------------------------------------------------------------------
// Effects_TV_Update (void)
// Update TV Effect
//-----------------------------------------------------------------------------
// NOTE: this was written age ago. May want to rewrite it.
//-----------------------------------------------------------------------------
void    Effects_TV_Update (void)
{
 byte   c = 0;
 int    i, j, k, l;
 byte  *p1, *p2, *p3, *p4;

 int    rx = cur_drv->x_res;
 int    ry = cur_drv->y_res;
 int    rx_d2 = rx / 2;
 int    ry_d2 = ry / 2;

 // Note: computing offset like that assert the fact that screenbuffer
 // is a *linear* buffer. It is the case now since it is a memory buffer.
 int start_offset = (cur_drv->y_show_start * (screenbuffer->line[1] - screenbuffer->line[0])) + cur_drv->x_start;

 for (j = 0; j < ry_d2; j ++)
     {
     p1 = screenbuffer->line [j] + start_offset;
     p2 = screenbuffer->line [j + ry_d2] + start_offset;
     p3 = p1 + rx_d2;
     p4 = p2 + rx_d2;
     for (i = 0; i < rx_d2; i ++)
         {
         c = (Random(12) + EFFECT_TV_EMU_COLOR_START);
         *p1++ = *p2++ = *p3++ = *p4++ = c;
         }
     }
 for (i = 0; i < 25; i ++)
     {
     c = (Random(12) + EFFECT_TV_EMU_COLOR_START);
     j = Random(rx); // x
     k = Random(ry); // y
     l = Random(30); // len
     j -= l;
     if (j < 0) j = 0;
     p1 = screenbuffer->line [k] + j + start_offset;
     while (l > 0)
        {
        *p1 = c;
        p1 ++;
        l --;
        }
     }

 j = Random(ry);
 k = Random(16) + (rx - 16);
 i = Random(16);
 p1 = screenbuffer->line [j] + i + start_offset;
 for (; i < k; i ++)
     {
     c = (Random(3) + EFFECT_TV_EMU_COLOR_START);
     if (Random(16) > 0)
        {
        *p1 = c;
        }
     p1 ++;
     }

 if (effects.TV_Start_Line > 0)
    {
    for (j = 0; j < effects.TV_Start_Line; j ++)
        {
        p1 = screenbuffer->line[j] + start_offset;
        for (i = 0; i < rx; i ++)
            {
            if (Random(effects.TV_Start_Line) > 0)
               {
               *p1 = EFFECT_TV_EMU_COLOR_START;
               }
            p1 ++;
            }
        }
    for (j = ry - 1; j > (ry - effects.TV_Start_Line); j --)
        {
        p1 = screenbuffer->line[j] + start_offset;
        for (i = 0; i < rx; i ++)
            {
            if (Random(effects.TV_Start_Line) > 0)
               {
               *p1 = EFFECT_TV_EMU_COLOR_START;
               }
            p1 ++;
            }
        }
    effects.TV_Start_Line -= 24;
    }
}

//-----------------------------------------------------------------------------
// Effects_TV_Init_Colors (void)
// Initialize TV Effect palette (in emulation palette)
//-----------------------------------------------------------------------------
void    Effects_TV_Init_Colors (void)
{
 int    i;
 RGB    color;

 for (i = 0; i < 12; i ++)
     {
     color.r = color.g = color.b = i * 4;
     color.filler = 0;
     Palette_SetColor_Emulation (EFFECT_TV_EMU_COLOR_START + i, color);
     }
}

//-----------------------------------------------------------------------------

