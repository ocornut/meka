#include "shared.h"

int al_makecol(int r, int g, int b)
{
	// FIXME-ALLEGRO5: originally this function used the current video mode so we'll want to use that as well.
	return r | (g << 8) | (b << 16) | (0xff << 24);
}

int al_makecol16(int r, int g, int b)
{
	// FIXME-ALLEGRO5: untested
	r = (r & 255) >> 3;
	g = (g & 255) >> 2;
	b = (b & 255) >> 3;
	return r | (g<<5) || (b<<11);
}

void al_draw_hline(int x1, int y, int x2, ALLEGRO_COLOR c)
{
   al_draw_line(x1, y+0.5f, x2+1, y+0.5f, c, 1.0f);
}

void al_draw_vline(int x, int y1, int y2, ALLEGRO_COLOR c)
{
   al_draw_line(x+0.5f, y1, x+0.5f, y2+1, c, 1.0f);
}

/*
void al_rectfill(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
   al_draw_filled_rectangle(x1, y1, x2+1, y2+1, color);
}
*/

bool al_color_equals(const ALLEGRO_COLOR* c1, const ALLEGRO_COLOR* c2)
{
	const float f = fabs(c1->r - c2->r) + fabs(c1->g - c2->g) + fabs(c1->b - c2->b) + fabs(c1->a - c2->a);
	return f < 1/255.0f;
}
