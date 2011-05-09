
void al_draw_hline(int x1, int y, int x2, ALLEGRO_COLOR c);
void al_draw_vline(int x, int y1, int y2, ALLEGRO_COLOR c);

unsigned int al_makecol32(int r, int g, int b);
unsigned short al_makecol16(int r, int g, int b);

bool al_color_equals(const ALLEGRO_COLOR* c1, const ALLEGRO_COLOR* c2);