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

void    gui_frame_clear(ALLEGRO_BITMAP *dst, const t_frame *frame, ALLEGRO_COLOR color);

void    gui_rect(int look, int x1, int y1, int x2, int y2, ALLEGRO_COLOR c_fg);
void    gui_rect_titled(char *Text, t_font_id font_id, int look, int x1, int y1, int x2, int y2, ALLEGRO_COLOR c_fg, ALLEGRO_COLOR c_bg, ALLEGRO_COLOR c_font);

static INLINE
bool    frame_contains_point(const t_frame *frame, int point_x, int point_y)
{
    return (point_x >= frame->pos.x && point_y >= frame->pos.y && point_x < frame->pos.x + frame->size.x && point_y < frame->pos.y + frame->size.y);
}

//-----------------------------------------------------------------------------

