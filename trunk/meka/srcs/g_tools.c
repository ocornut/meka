//-----------------------------------------------------------------------------
// MEKA - g_tools.c
// GUI Tools - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

void    gui_rect (BITMAP *bmp, int look, int x1, int y1, int x2, int y2, int c_fg)
{
    switch (look)
    {
    case LOOK_THIN:
        rect (bmp, x1 + 1, y1 + 1, x2 - 1, y2 - 1, c_fg);
        break;
    case LOOK_ROUND:
        rect (bmp, x1 + 1, y1, x2 - 1, y2, c_fg);
        rect (bmp, x1, y1 + 1, x2, y2 - 1, c_fg);
        break;
    }
}

void    gui_rect_titled (BITMAP *bmp, char *Text, int FontIdx,
                         int look, int x1, int y1, int x2, int y2,
                         int c_fg, int c_bg, int c_text)
{
    int lx, ly;
    gui_rect (bmp, look, x1, y1, x2, y2, c_fg);
    lx = Font_TextLength (FontIdx, Text) + (2 * GUI_LOOK_FRAME_PAD_HEAD2_X) + 3;
    ly = Font_Height (FontIdx) / 2;
    rectfill (bmp, x1 + GUI_LOOK_FRAME_PAD_HEAD1_X, y1 - ly, x1 + GUI_LOOK_FRAME_PAD_HEAD1_X + lx, y1 + ly, c_bg);
    line (bmp, x1 + GUI_LOOK_FRAME_PAD_HEAD1_X - 1, y1 - ly + 1, x1 + GUI_LOOK_FRAME_PAD_HEAD1_X - 1, y1 + ly, c_fg);
    line (bmp, x1 + lx + GUI_LOOK_FRAME_PAD_HEAD1_X + 1, y1 - ly + 1, x1 + lx + GUI_LOOK_FRAME_PAD_HEAD1_X + 1, y1 + ly, c_fg);
    Font_Print (FontIdx, bmp, Text, 2 + x1 + GUI_LOOK_FRAME_PAD_HEAD1_X + GUI_LOOK_FRAME_PAD_HEAD2_X, y1 - ly, c_text);
}

// FADE ALL COLORS TO BLACK ---------------------------------------------------
// (FIXME: this is blocking effect ...)
void            gui_fade_to_black (void)
{
    int         i;
    int         keep_going;
    PALETTE     pal;
    int         min, max;

    // Get current palette
    get_palette (pal);

    // For whatever reason, masks bit that should be unused anyway
    // FIXME: why did I write that??
    for (i = 0; i < MAX_COLORS; i++)
    {
        pal[i].r &= 63;
        pal[i].g &= 63;
        pal[i].b &= 63;
    }

    keep_going = YES;
    while (keep_going)
    {
        // Rest 10 ms, so that the whole fade is max 256*10ms = 2.56 seconds
        rest (10);

        keep_going = NO;
        min = MAX_COLORS;
        max = 0;
        for (i = 0; i < MAX_COLORS; i ++)
        {
            if (pal[i].r > 0)
            { pal[i].r --; keep_going = YES; if (min > i) min = i; if (max < i) max = i; }
            if (pal[i].g > 0)
            { pal[i].g --; keep_going = YES; if (min > i) min = i; if (max < i) max = i; }
            if (pal[i].b > 0)
            { pal[i].b --; keep_going = YES; if (min > i) min = i; if (max < i) max = i; }
        }
        if (keep_going)
            set_palette_range (pal, min, max, NO);

        // Take advantage of auto frame-skipper timer handler to time out
        // FIXME: depends on auto frame-skipper settings... should setup our own timer
        // or rest, or do another way.
        //{
        //    int fc = fskipper.Automatic_Frame_Elapsed;
        //    while (fc == fskipper.Automatic_Frame_Elapsed);
        //}
    }
}

//-----------------------------------------------------------------------------

