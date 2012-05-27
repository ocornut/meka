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
    // Fill
    clear_to_color(gui_background, COLOR_SKIN_BACKGROUND);

    // Draw grid
    if (COLOR_SKIN_BACKGROUND_GRID != COLOR_SKIN_BACKGROUND)
    {
        int i;
        const u32 color = COLOR_SKIN_BACKGROUND_GRID;
        for (i = gui.info.grid_distance; i < gui.info.screen.y; i += gui.info.grid_distance)
            line (gui_background, 0, i, gui.info.screen.x, i, color);
        for (i = gui.info.grid_distance; i < gui.info.screen.x; i += gui.info.grid_distance)
            line (gui_background, i, 0, i, gui.info.screen.y, color);
    }
}

static void     Skins_Background_Draw_Tile(BITMAP *bmp)
{
    int     sx, sy;
    int     cx, cy;

    sx = (bmp->w);
    sy = (bmp->h);
    cy = 0;
    while (cy < gui.info.screen.y)
    {
        cx = 0;
        while (cx < gui.info.screen.x)
        {
            blit (bmp, gui_background,
                0, 0, cx, cy, bmp->w, bmp->h);
            cx += sx;
        }
        cy += sy;
    }
}

static void     Skins_Background_Draw_Stretch(BITMAP *bmp)
{
    stretch_blit(bmp, gui_background,
        0, 0,
        bmp->w, bmp->h,
        0, gui.info.bars_height + 2,
        gui.info.screen.x, gui.info.screen.y - 2 * (gui.info.bars_height + 2));
}

static void     Skins_Background_Draw_StretchInteger(BITMAP *bmp)
{
    int         size_x;
    int         size_y;
    int         factor = 0;

    // Find factor
    do
    {
        factor += 1;
        size_x = (gui.info.screen.x - bmp->w * factor) / 2;
        size_y = (gui.info.screen.y - 2 * (gui.info.bars_height + 2) - bmp->h * factor) / 2;
    }
    while (size_x >= 0 && size_y >= 0);
    factor -= 1;

    // Draw grid
    Skins_Background_Redraw_Grid();

    if (factor == 0)
    {
        Msg (MSGT_USER, Msg_Get (MSG_Theme_Error_BG_Big));
        return;
    }

    {
        int sx = (gui.info.screen.x - bmp->w * factor) / 2;
        int sy = (gui.info.screen.y - 2 * (gui.info.bars_height + 2) - bmp->h * factor) / 2;
        sy += gui.info.bars_height + 2;
        stretch_blit(bmp, gui_background,
            0, 0,
            bmp->w, bmp->h,
            sx, sy,
            bmp->w * factor, bmp->h * factor);
    }
}

static void     Skins_Background_Draw_Center(BITMAP *bmp)
{
    const int   pos_x = (gui.info.screen.x - bmp->w) >> 1;
    const int   pos_y = (gui.info.screen.y - bmp->h) >> 1;
    Skins_Background_Redraw_Grid();
    blit(bmp, gui_background, 0, 0, pos_x, pos_y, bmp->w, bmp->h);
}


void    Skins_Background_Redraw(void)
{
    t_skin *skin;
    BITMAP *background;

#ifdef DEBUG_WHOLE
	Msg (MSGT_DEBUG, "Skins_Background_Redraw();");
#endif

	gui.info.must_redraw = TRUE;
	
    skin = Skins_GetCurrentSkin();
    background = Skins_GetBackgroundPicture();
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
    if (Inputs.Keyboard_Enabled)
	{
		BITMAP *bmp = Graphics.Inputs.SK1100_Keyboard;
		draw_sprite(gui_background, bmp,
			(gui.info.screen.x - bmp->w) / 2,
			(gui.info.screen.y - bmp->h - 40));
	}
}

//-----------------------------------------------------------------------------

