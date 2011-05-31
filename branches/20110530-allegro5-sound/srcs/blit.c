//-----------------------------------------------------------------------------
// MEKA - blit.c
// Blitters - Code
//-----------------------------------------------------------------------------
// FIXME: many room for optimization in the copying functions.
// FIXME: need a full rewrite/rethinking. Now that hi-color modes are well
// supported, this will be more straightforward than before.
//-----------------------------------------------------------------------------
//
// WIP notes
//
// SMS      256x192     512x384     768x576     1024x768
// SMS-EXT  256x224     512x448     768x672     1024x896
// GG       160x144     320x288     480x432     640x576     800x720     960x864
//
// refresh_rate     -> external to blitter, FS only
// flip             -> external to blitter, FS only
//
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "eagle.h"
#include "fskipper.h"
#include "glasses.h"
#include "hq2x.h"
#include "palette.h"
#include "vdp.h"
#include "video.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

ALLEGRO_BITMAP * Blit_Buffer_LineScratch = NULL;	// Line buffer scratch pad, 16-bits
ALLEGRO_BITMAP * Blit_Buffer_Double = NULL;			// Double-sized buffer, 16-bits

t_blit_cfg blit_cfg;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Blit_Init (void)
{
    blit_cfg.tv_mode_factor = 0.700f;	// FIXME-TUNING
	Blit_CreateVideoBuffers();

    // Initialize HQ2X filters
    HQ2X_Init();
}

void	Blit_CreateVideoBuffers()
{
	if (Blit_Buffer_LineScratch)
		al_destroy_bitmap(Blit_Buffer_LineScratch);
	if (Blit_Buffer_Double)
		al_destroy_bitmap(Blit_Buffer_Double);

	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	al_set_new_bitmap_format(g_Configuration.video_game_format_request);
    Blit_Buffer_LineScratch = al_create_bitmap(MAX_RES_X * 2, 1);
    Blit_Buffer_Double      = al_create_bitmap((MAX_RES_X + 32) * 2, (MAX_RES_Y + 32)*2);
}

static const t_blitters_table_entry     Blitters_Table[BLITTER_MAX] =
{
    { Blit_Fullscreen_Normal,           1,      1 },
    { Blit_Fullscreen_Double,           2,      2 },
    { Blit_Fullscreen_TV_Mode,          1,      2 },
    { Blit_Fullscreen_TV_Mode_Double,   2,      2 },
    { Blit_Fullscreen_Eagle,            2,      2 },
    { Blit_Fullscreen_HQ2X,             2,      2 },
};

void    Blit_Fullscreen_Misc(void)
{
    // Wait for VSync if necessary
    // (not done if speed is higher than 70 hz)
    // FIXME: 70 should be replaced by actual screen refresh rate ... can we obtain it ?
    if (g_Configuration.video_mode_game_vsync)
	{
        if (!(fskipper.Mode == FRAMESKIP_MODE_THROTTLED && fskipper.Throttled_Speed > 70))
            al_wait_for_vsync();
	}

    // Clear Screen if it has been asked
    if (Video.clear_requests > 0)
    {
        Video.clear_requests--;
		al_set_target_bitmap(al_get_backbuffer(g_display));
        al_clear_to_color(BORDER_COLOR);
	}

    // Update 3-D Glasses
    if (Glasses.Enabled)
        Glasses_Update();
}

static void Blit_Fullscreen_Message(ALLEGRO_BITMAP* dst, int time_left)
{
    int     x, y;

	if (dst == screenbuffer)
	{
		x = blit_cfg.src_sx + 8;
		if ((cur_drv->id == DRV_SMS) && (Mask_Left_8))
			x += 8;

		int fy = Blitters_Table[Blitters.current->blitter].y_fact;
		y = blit_cfg.src_sy + cur_drv->y_res;
		if (y * fy > Video.res_y)
			y -= ((y * fy) - Video.res_y) / (fy * 2);
		y -= 14;
	}
	else
	{
		x = 10;
		y = al_get_bitmap_height(dst) - 16;
		if (time_left < 20)
			y += (20 - time_left);
		al_draw_filled_rectangle(0, y-6, al_get_bitmap_width(dst), al_get_bitmap_height(dst), al_map_rgba(0,0,0,128));
	}

	// FIXME-OPT: use a dedicated font. This is slow as hell!!
    Font_SetCurrent(F_LARGE);
    Font_Print (-1, dst, gui_status.message, x - 1, y - 1, COLOR_BLACK);
    Font_Print (-1, dst, gui_status.message, x,     y - 1, COLOR_BLACK);
    Font_Print (-1, dst, gui_status.message, x + 1, y - 1, COLOR_BLACK);
    Font_Print (-1, dst, gui_status.message, x - 1, y + 1, COLOR_BLACK);
    Font_Print (-1, dst, gui_status.message, x,     y + 1, COLOR_BLACK);
    Font_Print (-1, dst, gui_status.message, x + 1, y + 1, COLOR_BLACK);
    Font_Print (-1, dst, gui_status.message, x - 1, y,     COLOR_BLACK);
    Font_Print (-1, dst, gui_status.message, x + 1, y,     COLOR_BLACK);
    Font_Print (-1, dst, gui_status.message, x,     y,     COLOR_WHITE);
}

static void	Blit_Fullscreen_CopyStretch(ALLEGRO_BITMAP *src_buffer, int input_res_sx, int input_res_sy, int dst_scale)
{
	al_set_target_bitmap(fs_out);

	const int src_px = blit_cfg.src_sx * input_res_sx;
	const int src_py = blit_cfg.src_sy * input_res_sy;
	const int src_sx = cur_drv->x_res * input_res_sx;
	const int src_sy = cur_drv->y_res * input_res_sy;

	const t_blitter_stretch stretch = Blitters.current->stretch;
	if (stretch == BLITTER_STRETCH_NONE)
	{
		if (dst_scale == 1)
		{
			al_draw_bitmap_region(src_buffer, 
				src_px, src_py,
				src_sx, src_sy,
				blit_cfg.dst_sx, blit_cfg.dst_sy,
				0x0000);
		}
		else
		{
			al_draw_scaled_bitmap(src_buffer,
				src_px, src_py,
				src_sx, src_sy,
				blit_cfg.dst_sx, blit_cfg.dst_sy,
				cur_drv->x_res  * input_res_sx * dst_scale, cur_drv->y_res  * input_res_sy * dst_scale,
				0x0000);
		}
	}
	else if (stretch == BLITTER_STRETCH_MAX_INT)
	{
		// Integer scale
		const int scale_x = (int)Video.res_x / src_sx;
		const int scale_y = (int)Video.res_y / src_sy;
		const int scale = MIN(scale_x, scale_y);
		al_draw_scaled_bitmap(src_buffer,
			src_px, src_py,
			src_sx, src_sy,
			(Video.res_x/2)-(src_sx*scale)/2, (Video.res_y/2)-(src_sy*scale)/2,
			src_sx*scale, src_sy*scale,
			0x0000);
	}
	else
	{
		// FIXME: MAX_RATIO vs MAX
		al_draw_scaled_bitmap(src_buffer,
			src_px, src_py,
			src_sx, src_sy,
			0, 0, 
			Video.res_x, Video.res_y,
			0x0000);
	}
}

void    Blit_Fullscreen_Normal (void)
{
    Blit_Fullscreen_Misc();
	Blit_Fullscreen_CopyStretch(screenbuffer, 1, 1, 1);
}

void    Blit_Fullscreen_Double (void)
{
    Blit_Fullscreen_Misc();
	Blit_Fullscreen_CopyStretch(screenbuffer, 1, 1, 2);
}

void    Blit_Fullscreen_Eagle (void)
{
	assert(0);
#if 0 // FIXME-ALLEGRO5: blit
	// Eagle, x1 -> x2
	int i;
	for (i = blit_cfg.src_sy; i < blit_cfg.src_sy + cur_drv->y_res; i ++)
	{
		eagle_mmx16(
			(unsigned long *)((u16 *)screenbuffer->line[i] + blit_cfg.src_sx),
			(unsigned long *)((u16 *)screenbuffer->line[i + 1] + blit_cfg.src_sx),
			(short)cur_drv->x_res * 2,
			screenbuffer->seg,
			(u16 *)Blit_Buffer_Double->line[i * 2] + (blit_cfg.src_sx * 2),
			(u16 *)Blit_Buffer_Double->line[i * 2 + 1] + (blit_cfg.src_sx * 2));
	}
#endif
	Blit_Fullscreen_Misc();
	Blit_Fullscreen_CopyStretch(Blit_Buffer_Double, 2, 2, 1);
}

void    Blit_Fullscreen_HQ2X (void)
{
    // Perform HQ2X into double buffer
	// FIXME-OPT: Applied on full width.
#if 0 // FIXME-ALLEGRO5: blit
	ALLEGRO_LOCKED_REGION* lr_src = al_lock_bitmap(screenbuffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	ALLEGRO_LOCKED_REGION* lr_dst = al_lock_bitmap(Blit_Buffer_Double, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);

	u8* addr_src = ((u8*)lr_src->data + (lr_src->pitch * blit_cfg.src_sy * 1) + (0 * sizeof(u16)));
	u8* addr_dst = ((u8*)lr_dst->data + (lr_dst->pitch * blit_cfg.src_sy * 2) + (0 * sizeof(u16)));
	hq2x_16(addr_src, addr_dst, MAX_RES_X+32, cur_drv->y_res, (MAX_RES_X+32)*4);
	al_unlock_bitmap(screenbuffer);
	al_unlock_bitmap(Blit_Buffer_Double);
#endif
    Blit_Fullscreen_Misc();
	Blit_Fullscreen_CopyStretch(Blit_Buffer_Double, 2, 2, 1);
}

void    Blit_Fullscreen_TV_Mode (void)
{
#if 0 // FIXME-ALLEGRO5: blit
	int i;
	for (i = 0; i < cur_drv->y_res; i ++)
	{
		const u16 *psrc  = (u16 *)screenbuffer->line[blit_cfg.src_sy + i] + blit_cfg.src_sx;
		u16 *pdst1 = (u16 *)Blit_Buffer_Double->line[(blit_cfg.src_sy + i) * 2] + (blit_cfg.src_sx * 1);
		u16 *pdst2 = (u16 *)Blit_Buffer_Double->line[(blit_cfg.src_sy + i) * 2 + 1] + (blit_cfg.src_sx * 1);
		int j = cur_drv->x_res;
		while (j-- != 0)
		{
			const u16 color_org = *psrc++;
			const u32 color_mod_r = ((color_org      ) & 0x1F) * blit_cfg.tv_mode_factor;
			const u32 color_mod_g = ((color_org >> 5 ) & 0x3F) * blit_cfg.tv_mode_factor;
			const u32 color_mod_b = ((color_org >> 11) & 0x1F) * blit_cfg.tv_mode_factor;
			const u16 color_mod = (color_mod_r) | (color_mod_g << 5) | (color_mod_b << 11);
			*((u16 *)pdst1)++ = color_org;
			*((u16 *)pdst2)++ = color_mod;
		}
	}
#endif
	Blit_Fullscreen_Misc();
	Blit_Fullscreen_CopyStretch(Blit_Buffer_Double, 1, 2, 1);
}

// FIXME-OPT: Obviously this is very slow. Just trying to get something working for 0.72. Later shall work better solution (generating inline assembly, etc).
void    Blit_Fullscreen_TV_Mode_Double (void)
{
#if 0 // FIXME-ALLEGRO5: blit
	int i;
	for (i = 0; i < cur_drv->y_res; i ++)
	{
		const u16 *psrc  = (u16 *)screenbuffer->line[blit_cfg.src_sy + i] + blit_cfg.src_sx;
		u16 *pdst1 = (u16 *)Blit_Buffer_Double->line[(blit_cfg.src_sy + i) * 2] + (blit_cfg.src_sx * 2);
		u16 *pdst2 = (u16 *)Blit_Buffer_Double->line[(blit_cfg.src_sy + i) * 2 + 1] + (blit_cfg.src_sx * 2);
		int j = cur_drv->x_res;
		while (j-- != 0)
		{
			const u16 color_org = *psrc++;
			const u32 color_org_32 = color_org | (color_org << 16);
			const u32 color_mod_r = ((color_org      ) & 0x1F) * blit_cfg.tv_mode_factor;
			const u32 color_mod_g = ((color_org >> 5 ) & 0x3F) * blit_cfg.tv_mode_factor;
			const u32 color_mod_b = ((color_org >> 11) & 0x1F) * blit_cfg.tv_mode_factor;
			const u16 color_mod = (color_mod_r) | (color_mod_g << 5) | (color_mod_b << 11);
			const u32 color_mod_32 = color_mod | (color_mod << 16);
			*(u32 *)pdst1 = color_org_32;
			*(u32 *)pdst2 = color_mod_32;
			pdst1 += 2;
			pdst2 += 2;
			// Note: adding ++ to the above u32 * cast somehow cause problems with GCC
		}
	}
#endif 0
	Blit_Fullscreen_Misc();
	Blit_Fullscreen_CopyStretch(Blit_Buffer_Double, 2, 2, 1);
}

// Blit screenbuffer to video memory in fullscreen mode
void    Blit_Fullscreen(void)
{
    blit_cfg.src_sx = cur_drv->x_start;
    blit_cfg.src_sy = cur_drv->y_show_start;
    blit_cfg.dst_sx = Video.game_area_x1;
    blit_cfg.dst_sy = Video.game_area_y1;

#if 0
    {
        static char buf[512];
        Font_Set (F_SMALL);
        if (fskipper.FPS_Temp == 0) strcpy(buf, "");
        sprintf(buf+strlen(buf), "%1d", fskipper.FPS_Temp % 10);
        //Font_Print (screenbuffer, buf, 49 + (fskipper.FPS_Temp % 10) * 6, 49 + (fskipper.FPS_Temp / 10)*10, COLOR_BLACK);
        //Font_Print (screenbuffer, buf, 50 + (fskipper.FPS_Temp % 10) * 6, 50 + (fskipper.FPS_Temp / 10)*10, COLOR_WHITE);
        Font_Print (screenbuffer, buf, 19, 49, COLOR_BLACK);
        Font_Print (screenbuffer, buf, 20, 50, COLOR_WHITE);
    }
#endif

    Blitters_Table [Blitters.current->blitter].func();

	if (gui_status.timeleft && g_Configuration.show_fullscreen_messages)
	{
		al_set_target_bitmap(fs_out);
		Blit_Fullscreen_Message(fs_out, gui_status.timeleft);
		gui_status.timeleft --;
	}

	al_flip_display();
}

void    Blitters_Get_Factors(int *x, int *y)
{
    *x = Blitters_Table[Blitters.current->blitter].x_fact;
    *y = Blitters_Table[Blitters.current->blitter].y_fact;
}

void    Blit_GUI (void)
{
    // Wait for VSync if necessary
    if (g_Configuration.video_mode_gui_vsync)
    {
        // FIXME: see note about line below in Blit_Fullscreen()
        if (!(fskipper.Mode == FRAMESKIP_MODE_THROTTLED && fskipper.Throttled_Speed > 70))
            al_wait_for_vsync();
        // Update 3-D Glasses (if VSync)
        if (Glasses.Enabled)
            Glasses_Update();
    }

    // Blit
	ALLEGRO_BITMAP* backbuffer = al_get_backbuffer(g_display);
	al_set_target_bitmap(backbuffer);
	al_draw_bitmap(gui_buffer, 0, 0, 0x0000);
	al_flip_display();

    // Update 3-D Glasses (if no VSync)
    if (!g_Configuration.video_mode_gui_vsync)
        if (Glasses.Enabled)
            Glasses_Update ();
}

//-----------------------------------------------------------------------------

