//-----------------------------------------------------------------------------
// MEKA - g_tools.c
// GUI Tools - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_frame_clear(BITMAP *dst, const t_frame *frame, int color)
{
    rectfill(dst, frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x - 1, frame->pos.y + frame->size.y - 1, color);
}

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
    default:
        assert(0);
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

//-----------------------------------------------------------------------------

