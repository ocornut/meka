#include <allegro5/allegro_primitives.h>

void al_draw_hline(int x1, int y, int x2, ALLEGRO_COLOR c)
{
   al_draw_line(x1, y, x2+0.5, y, c, 1);
}

void al_draw_vline(int x, int y1, int y2, ALLEGRO_COLOR c)
{
   al_draw_line(x, y1, x+0.5, y2+0.5, c, 1);
}

/*
void al_rectfill(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
   al_draw_filled_rectangle(x1, y1, x2+1, y2+1, color);
}
*/