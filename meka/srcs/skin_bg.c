//-----------------------------------------------------------------------------
// MEKA - themes_b.c
// Interface - Background Refresh - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "skin_bg.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

static void     Skins_Background_Redraw_Grid(void)
{
    al_clear_to_color(COLOR_SKIN_BACKGROUND);

    // Draw grid
    if (!alx_color_equals(&COLOR_SKIN_BACKGROUND_GRID, &COLOR_SKIN_BACKGROUND))
    {
        int i;
        const ALLEGRO_COLOR color = COLOR_SKIN_BACKGROUND_GRID;
        for (i = gui.info.grid_distance; i < gui.info.screen.y; i += gui.info.grid_distance)
            al_draw_line(0, i, gui.info.screen.x, i, color, 0);
        for (i = gui.info.grid_distance; i < gui.info.screen.x; i += gui.info.grid_distance)
            al_draw_line(i, 0, i, gui.info.screen.y, color, 0);
    }
}

static void     Skins_Background_Draw_Tile(ALLEGRO_BITMAP *bmp)
{
    int     sx, sy;
    int     cx, cy;

    sx = al_get_bitmap_width(bmp);
    sy = al_get_bitmap_height(bmp);
    cy = 0;
    while (cy < gui.info.screen.y)
    {
        cx = 0;
        while (cx < gui.info.screen.x)
        {
            al_draw_bitmap(bmp, cx, cy, 0);
            cx += sx;
        }
        cy += sy;
    }
}

static void     Skins_Background_Draw_Stretch(ALLEGRO_BITMAP *bmp)
{
	al_draw_scaled_bitmap(bmp, 
		0.0f, 0.0f, al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
		0, gui.info.bars_height + 2,
		gui.info.screen.x, gui.info.screen.y - 2 * (gui.info.bars_height + 2),
		0);
}

static void     Skins_Background_Draw_StretchInteger(ALLEGRO_BITMAP *bmp)
{
    int         size_x;
    int         size_y;
    int         factor = 0;

    // Find best fit factor
	const int w = al_get_bitmap_width(bmp);
	const int h = al_get_bitmap_height(bmp);
    do
    {
        factor += 1;
        size_x = (gui.info.screen.x - w * factor) / 2;
        size_y = (gui.info.screen.y - 2 * (gui.info.bars_height + 2) - h * factor) / 2;
    }
    while (size_x >= 0 && size_y >= 0);
    factor -= 1;

    // Draw grid
    Skins_Background_Redraw_Grid();

    if (factor == 0)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Theme_Error_BG_Big));
        return;
    }

    {
        int sx = (gui.info.screen.x - w * factor) / 2;
        int sy = (gui.info.screen.y - 2 * (gui.info.bars_height + 2) - h * factor) / 2;
        sy += gui.info.bars_height + 2;
		al_draw_scaled_bitmap(bmp, 0, 0, w, h, sx, sy, w * factor, h * factor, 0x0000);
    }
}

static void     Skins_Background_Draw_Center(ALLEGRO_BITMAP *bmp)
{
    const int   pos_x = (gui.info.screen.x - al_get_bitmap_width(bmp)) >> 1;
    const int   pos_y = (gui.info.screen.y - al_get_bitmap_height(bmp)) >> 1;
    Skins_Background_Redraw_Grid();
	al_draw_bitmap(bmp, pos_x, pos_y, 0x0000);
}


void    Skins_Background_Redraw(void)
{
#ifdef DEBUG_WHOLE
	Msg(MSGT_DEBUG, "Skins_Background_Redraw();");
#endif

	gui.info.must_redraw = TRUE;
	
    al_set_target_bitmap(gui_background);

    t_skin* skin = Skins_GetCurrentSkin();
    ALLEGRO_BITMAP* background = Skins_GetBackgroundPicture();
	if (background != NULL)
	{
        switch (skin->background_picture_mode)
		{
		case SKIN_BACKGROUND_PICTURE_MODE_CENTER:
			Skins_Background_Draw_Center(background);
			break;
		case SKIN_BACKGROUND_PICTURE_MODE_STRETCH:
			Skins_Background_Draw_Stretch(background);
			break;
		case SKIN_BACKGROUND_PICTURE_MODE_STRETCH_INT:
			Skins_Background_Draw_StretchInteger(background);
			break;
		case SKIN_BACKGROUND_PICTURE_MODE_TILE:
			Skins_Background_Draw_Tile(background);
			break;
        default:
            //assert(0);
            Skins_Background_Draw_Stretch(background);
            break;
		}
	}
	else
	{
		Skins_Background_Redraw_Grid();
		VMachine_Draw();
	}

    // Draw SK-1100 centered on bottom
    if (Inputs.SK1100_Enabled)
	{
		ALLEGRO_BITMAP *bmp = Graphics.Inputs.SK1100_Keyboard;
		al_draw_bitmap(bmp,
			(gui.info.screen.x - al_get_bitmap_width(bmp)) / 2,
			(gui.info.screen.y - al_get_bitmap_height(bmp) - 40), 0x0000);
	}
}

//-----------------------------------------------------------------------------

