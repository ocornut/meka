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

ALLEGRO_BITMAP *         Blit_Buffer_LineScratch;	// Line buffer scratch pad, 16-bits
ALLEGRO_BITMAP *         Blit_Buffer_Double;		// Double-sized buffer, 16-bits

t_blit_cfg blit_cfg;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Blit_Init (void)
{
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_BGR_565);
    Blit_Buffer_LineScratch = al_create_bitmap(MAX_RES_X * 2, 1);
    Blit_Buffer_Double      = al_create_bitmap((MAX_RES_X + 32) * 2, (MAX_RES_Y + 32)*2);
    blit_cfg.tv_mode_factor = 0.700f;	// FIXME-TUNING

    // Initialize HQ2X filters
    HQ2X_Init();
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

void    Blit_Fullscreen_Misc (void)
{
    // Wait for VSync if necessary
    // (not done if speed is higher than 70 hz)
    // FIXME: 70 should be replaced by actual screen refresh rate ... can we obtain it ?
    if (g_Configuration.video_mode_game_vsync)
	{
        if (!(fskipper.Mode == FRAMESKIP_MODE_AUTO && fskipper.Automatic_Speed > 70))
            al_wait_for_vsync();
	}

    // Clear Screen if it has been asked
    if (Video.clear_request)
    {
        // int cpu_capabilities_backup = cpu_capabilities;
        // cpu_capabilities &= ~CPU_MMX;
        Video.clear_request = FALSE;
        if (g_Configuration.video_mode_game_page_flipping)
        {
			al_set_target_bitmap(fs_page_0);
            al_clear_to_color(Border_Color);
			al_set_target_bitmap(fs_page_1);
            al_clear_to_color(Border_Color);
        }
        else
        {
			al_set_target_bitmap(al_get_backbuffer(g_display));
            al_clear_to_color(Border_Color);
        }
        // cpu_capabilities = cpu_capabilities_backup;
    }

    // Update 3-D Glasses
    if (Glasses.Enabled)
        Glasses_Update ();

    // Update palette if necessary
    //Palette_Sync ();
}

// FIXME: if blitting will be done outside of screen (because of y)
// wrap accordingly
void        Blit_Fullscreen_Message (void)
{
    int     x, y;
    int     fy;

    x = blit_cfg.src_sx + 8;
    if ((cur_drv->id == DRV_SMS) && (Mask_Left_8))
        x += 8;

    fy = Blitters_Table[Blitters.current->blitter].y_fact;
    y = blit_cfg.src_sy + cur_drv->y_res;
    if (y * fy > Video.res_y)
        y -= ((y * fy) - Video.res_y) / (fy * 2);
    y -= 14;

    Font_SetCurrent (F_SMALL);
    // FIXME: use a dedicated font. This is slow as hell!!
    Font_Print (-1, screenbuffer, gui_status.message, x - 1, y - 1, COLOR_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x,     y - 1, COLOR_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x + 1, y - 1, COLOR_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x - 1, y + 1, COLOR_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x,     y + 1, COLOR_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x + 1, y + 1, COLOR_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x - 1, y,     COLOR_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x + 1, y,     COLOR_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x,     y,     COLOR_WHITE);
}

static void	Blit_Fullscreen_CopyStretch(ALLEGRO_BITMAP *src_buffer, int input_res_sx, int input_res_sy, int dst_scale)
{
	al_set_target_bitmap(fs_out);
	if (!Blitters.current->stretch)
	{
		if (dst_scale == 1)
		{
			al_draw_bitmap_region(src_buffer, 
				blit_cfg.src_sx * input_res_sx, blit_cfg.src_sy * input_res_sy,
				cur_drv->x_res * input_res_sx, cur_drv->y_res * input_res_sy,
				blit_cfg.dst_sx, blit_cfg.dst_sy,
				0x0000);
		}
		else
		{
			al_draw_scaled_bitmap(src_buffer,
				blit_cfg.src_sx * input_res_sx, blit_cfg.src_sy * input_res_sy,
				cur_drv->x_res  * input_res_sx, cur_drv->y_res  * input_res_sy,
				blit_cfg.dst_sx, blit_cfg.dst_sy,
				cur_drv->x_res  * input_res_sx * dst_scale, cur_drv->y_res  * input_res_sy * dst_scale,
				0x0000);
		}
	}
	else
	{
		al_draw_scaled_bitmap(src_buffer,
			blit_cfg.src_sx * input_res_sx, blit_cfg.src_sy * input_res_sy,
			cur_drv->x_res  * input_res_sx, cur_drv->y_res  * input_res_sy,
			0, 0, Video.res_x, Video.res_y,
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
    Blit_Fullscreen_Misc ();
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
	Blit_Fullscreen_Misc ();
	Blit_Fullscreen_CopyStretch(Blit_Buffer_Double, 2, 2, 1);
}

void    Blit_Fullscreen_HQ2X (void)
{
#if 0 // FIXME-ALLEGRO5: blit
    // Perform HQ2X into double buffer
	// FIXME-OPT: Apply on full width.
    hq2x_16(
		(unsigned char *)((u16 *)screenbuffer->line[blit_cfg.src_sy] + 0), 
		(unsigned char *)((u16 *)Blit_Buffer_Double->line[blit_cfg.src_sy * 2] + 0), 
		MAX_RES_X+32, cur_drv->y_res, (MAX_RES_X+32)*4);
#endif
    Blit_Fullscreen_Misc();
	Blit_Fullscreen_CopyStretch(Blit_Buffer_Double, 2, 2, 1);
}

void    Blit_Fullscreen_TV_Mode (void)
{
#if 0	// FIXME-ALLEGRO5: blit
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
void    Blit_Fullscreen (void)
{
    blit_cfg.src_sx = cur_drv->x_start;
    blit_cfg.src_sy = cur_drv->y_show_start;
    blit_cfg.dst_sx = Video.game_area_x1;
    blit_cfg.dst_sy = Video.game_area_y1;

    if (gui_status.timeleft && g_Configuration.show_fullscreen_messages)
    {
        Blit_Fullscreen_Message ();
        gui_status.timeleft --;
    }

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

	al_flip_display();

    if (Video.triple_buffering_activated)
    {
		// FIXME-ALLEGRO5: Triple buffering
		assert(0);
#if 0
		while (poll_scroll())
            rest(0); // was: yield_timeslice(), deprecated in Allegro in favor of rest(0)

        request_video_bitmap(fs_out);
        Video.page_flipflop = (Video.page_flipflop + 1) % 3;
        switch (Video.page_flipflop)
        {
        case 0:
            fs_out = fs_page_0;
            break;
        case 1:
            fs_out = fs_page_1;
            break;
        case 2:
            fs_out = fs_page_2;
            break;
        }
#endif
    } 
    else if (g_Configuration.video_mode_game_page_flipping)
    {
		// FIXME-ALLEGRO5: Page flipping
		assert(0);
#if 0
        show_video_bitmap(fs_out);
        Video.page_flipflop ^= 1;
        if (Video.page_flipflop == 0)
            fs_out = fs_page_0;
        else
            fs_out = fs_page_1;
#endif
	}
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
        if (!(fskipper.Mode == FRAMESKIP_MODE_AUTO && fskipper.Automatic_Speed > 70))
            al_wait_for_vsync();
        // Update 3-D Glasses (if VSync)
        if (Glasses.Enabled)
            Glasses_Update();
    }

    // Update palette if necessary
    //Palette_Sync ();

    // Blit
	{
		ALLEGRO_BITMAP* backbuffer = al_get_backbuffer(g_display);
		al_set_target_bitmap(backbuffer);
		al_draw_bitmap(gui_buffer, 0, 0, 0x0000);
	}
    //blit (gui_buffer, screen, 0, 0, 0, 0, g_Configuration.video_mode_gui_res_x, g_Configuration.video_mode_gui_res_y);
	al_flip_display();

    // Update 3-D Glasses (if no VSync)
    if (!g_Configuration.video_mode_gui_vsync)
        if (Glasses.Enabled)
            Glasses_Update ();
}

//-----------------------------------------------------------------------------

