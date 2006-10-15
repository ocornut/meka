//-----------------------------------------------------------------------------
// MEKA - g_tools.h
// GUI Tools - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define LOOK_UNKNOWN_   (-1)
#define LOOK_THIN       (0)
#define LOOK_ROUND      (1)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_frame_clear(BITMAP *dst, const t_frame *frame, int color);

void    gui_rect (BITMAP *, int look, int x1, int y1, int x2, int y2, int c_fg);
void    gui_rect_titled (BITMAP *bmp, char *Text, int FontIdx, int look, int x1, int y1, int x2, int y2, int c_fg, int c_bg, int c_font);

//-----------------------------------------------------------------------------

