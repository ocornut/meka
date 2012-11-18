#include "shared.h"
#include "palette.h"

void al_draw_hline(int x1, int y, int x2, ALLEGRO_COLOR c)
{
   al_draw_line(x1, y+0.5f, x2+1, y+0.5f, c, 0);
}

void al_draw_vline(int x, int y1, int y2, ALLEGRO_COLOR c)
{
   al_draw_line(x+0.5f, y1, x+0.5f, y2+1, c, 0);
}

bool alx_color_equals(const ALLEGRO_COLOR* c1, const ALLEGRO_COLOR* c2)
{
	const float f = fabs(c1->r - c2->r) + fabs(c1->g - c2->g) + fabs(c1->b - c2->b) + fabs(c1->a - c2->a);
	return f < 1/255.0f;
}

void alx_locked_draw_filled_rectangle(ALLEGRO_LOCKED_REGION* dst_region, int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	ALLEGRO_BITMAP *dst = al_get_target_bitmap();
	const int color_format = al_get_bitmap_format(dst);
    switch (al_get_pixel_format_bits(color_format))
	{
	case 16:
		{
			const u16 host_color16 = (u16)Palette_MakeHostColor(color_format, color);
			const u32 host_color32 = host_color16 | (host_color16 << 16);

			const int dst_pitch = dst_region->pitch >> 1;
			u16* dst_data_line = (u16*)dst_region->data + (y1 * dst_pitch + x1);
			const int x_len32 = (x2 - x1) >> 1;
			for (int y = y1; y != y2; y++, dst_data_line += dst_pitch)
			{
				u32* dst_data = (u32 *)dst_data_line;
				for (int x = x_len32; x != 0; x--)
					*dst_data++ = host_color32;
			}
			break;
		}
	case 32:
		{
			const u32 host_color32 = Palette_MakeHostColor(color_format, color);

			const int dst_pitch = dst_region->pitch >> 2;
			u32* dst_data_line = (u32*)dst_region->data + (y1 * dst_pitch + x1);
			const int x_len32 = (x2 - x1);
			for (int y = y1; y != y2; y++, dst_data_line += dst_pitch)
			{
				u32* dst_data = (u32 *)dst_data_line;
				for (int x = x_len32; x != 0; x--)
					*dst_data++ = host_color32;
			}
			break;
		}
	}
}
