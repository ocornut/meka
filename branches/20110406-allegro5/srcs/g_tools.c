//-----------------------------------------------------------------------------
// MEKA - g_tools.c
// GUI Tools - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "g_tools.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_frame_clear(ALLEGRO_BITMAP *dst, const t_frame *frame, ALLEGRO_COLOR color)
{
	al_set_target_bitmap(dst);
    al_draw_filled_rectangle(frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x, frame->pos.y + frame->size.y, color);
}

void    gui_rect (ALLEGRO_BITMAP *bmp, int look, int x1, int y1, int x2, int y2, ALLEGRO_COLOR c_fg)
{
	al_set_target_bitmap(bmp);
    switch (look)
    {
    case LOOK_THIN:
        al_draw_rectangle(x1 + 1.5f, y1 + 1.5f, x2 - 0.5f, y2 - 0.5f, c_fg, 1);
        break;
    case LOOK_ROUND:
        al_draw_rectangle(x1 + 1.5f, y1 + 0.5f, x2 - 0.5f, y2 + 0.5f, c_fg, 1);
        al_draw_rectangle(x1 + 0.5f, y1 + 1.5f, x2 + 0.5f, y2 - 0.5f, c_fg, 1);
        break;
    default:
        assert(0);
    }
}

void    gui_rect_titled (ALLEGRO_BITMAP *bmp, char *Text, int FontIdx,
                         int look, int x1, int y1, int x2, int y2,
                         ALLEGRO_COLOR c_fg, ALLEGRO_COLOR c_bg, ALLEGRO_COLOR c_text)
{
    int lx, ly;
    gui_rect (bmp, look, x1, y1, x2, y2, c_fg);
    lx = Font_TextLength (FontIdx, Text) + (2 * GUI_LOOK_FRAME_PAD_HEAD2_X) + 3;
    ly = Font_Height (FontIdx) / 2;

	al_set_target_bitmap(bmp);
    al_draw_filled_rectangle(x1 + GUI_LOOK_FRAME_PAD_HEAD1_X, y1 - ly, x1 + GUI_LOOK_FRAME_PAD_HEAD1_X + lx + 1, y1 + ly + 1, c_bg);
    al_draw_line(x1 + GUI_LOOK_FRAME_PAD_HEAD1_X, y1 - ly + 1, x1 + GUI_LOOK_FRAME_PAD_HEAD1_X, y1 + ly, c_fg, 1.0f);
    al_draw_line(x1 + lx + GUI_LOOK_FRAME_PAD_HEAD1_X + 1, y1 - ly + 1, x1 + lx + GUI_LOOK_FRAME_PAD_HEAD1_X + 1, y1 + ly, c_fg, 1.0f);
    Font_Print (FontIdx, bmp, Text, 2 + x1 + GUI_LOOK_FRAME_PAD_HEAD1_X + GUI_LOOK_FRAME_PAD_HEAD2_X, y1 - ly, c_text);
}

//-----------------------------------------------------------------------------

