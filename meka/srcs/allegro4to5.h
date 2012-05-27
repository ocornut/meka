
void al_draw_hline(int x1, int y, int x2, ALLEGRO_COLOR c);
void al_draw_vline(int x, int y1, int y2, ALLEGRO_COLOR c);

bool alx_color_equals(const ALLEGRO_COLOR* c1, const ALLEGRO_COLOR* c2);
void alx_locked_draw_filled_rectangle(ALLEGRO_LOCKED_REGION* dst_region, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
