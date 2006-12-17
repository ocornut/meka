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
// vsync            -> external to blitter, FS only
// triple_buffering -> external to blitter, FS only
//
// video_depth      -> 16 FS, auto GUI? What would be the point of 32 in FS mode?
//
//-----------------------------------------------------------------------------


#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "eagle.h"
#include "fskipper.h"
#include "hq2x.h"
#include "palette.h"
#include "vdp.h"
#include "glasses.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

BITMAP *         Blit_Buffer_LineScratch;    // Line buffer stratch pad
BITMAP *         Blit_Buffer_Double;         // Double-sized buffer
BITMAP *         Blit_Buffer_NativeTemp;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Blit_Init (void)
{
    Blit_Buffer_LineScratch = create_bitmap_ex(16, MAX_RES_X * 2, 1);   // FIXME-DEPTH
    Blit_Buffer_Double      = create_bitmap_ex(16, (MAX_RES_X + 32) * 2, (MAX_RES_Y + 32)*2);
    Blit_Buffer_NativeTemp  = NULL;

    blit_cfg.tv_mode_factor = 1.5f;

    // Initialize HQ2X filters
    HQ2X_Init();
}

static const t_blitters_table_entry     Blitters_Table[BLITTER_MAX] =
{
    { Blit_Fullscreen_Normal,           1,      1 },
    { Blit_Fullscreen_Double,           2,      2 },
    { Blit_Fullscreen_Scanlines,        1,      2 },
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
    if (Blitters.current->vsync)
        if (!(fskipper.Mode == FRAMESKIP_MODE_AUTO && fskipper.Automatic_Speed > 70))
            vsync ();

    // Clear Screen if it has been asked
    if (Video.clear_need)
    {
        // int cpu_capabilities_backup = cpu_capabilities;
        // cpu_capabilities &= ~CPU_MMX;
        Video.clear_need = FALSE;
        if (Blitters.current->flip)
        {
            clear_to_color (fs_page_0, Border_Color);
            clear_to_color (fs_page_1, Border_Color);
        }
        else
        {
            clear_to_color (screen, Border_Color);
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

void    Blit_Fullscreen_Normal (void)
{
    Blit_Fullscreen_Misc ();

    //if (Blitters.current->video_depth == 16)
    //    set_color_conversion(COLORCONV_8_TO_16 | COLORCONV_EXPAND_256);
    //if (Blitters.current->video_depth == 32)
    //    assert(0);

    if (!Blitters.current->stretch)
    {
        // Note: 'blit' converts 16 to native format
        blit (screenbuffer, fs_out,
            blit_cfg.src_sx, blit_cfg.src_sy,
            blit_cfg.dst_sx, blit_cfg.dst_sy,
            cur_drv->x_res,  cur_drv->y_res);
    }
    else
    {
        // Note: 'stretch_blit' doesn't convert!
        if (Blitters.current->video_depth != 16)
        {
            // Need this for conversion
            blit (screenbuffer, Blit_Buffer_NativeTemp,
                blit_cfg.src_sx, blit_cfg.src_sy,
                blit_cfg.src_sx, blit_cfg.src_sy,
                cur_drv->x_res,  cur_drv->y_res);
            stretch_blit(Blit_Buffer_NativeTemp, fs_out, 
                blit_cfg.src_sx, blit_cfg.src_sy,
                cur_drv->x_res, cur_drv->y_res,
                0,0, Video.res_x, Video.res_y);
        }
        else
        {
            stretch_blit(screenbuffer, fs_out, 
                blit_cfg.src_sx, blit_cfg.src_sy,
                cur_drv->x_res, cur_drv->y_res,
                0,0, Video.res_x, Video.res_y);
        }
    }

    //if (Blitters.current->video_depth != 8)
    //    set_color_conversion(COLORCONV_NONE);
}

void    Blit_Fullscreen_Double (void)
{
    // x1 -> x2
    stretch_blit(screenbuffer, Blit_Buffer_Double, 
        blit_cfg.src_sx, blit_cfg.src_sy,
        cur_drv->x_res, cur_drv->y_res,
        0,0, 
        cur_drv->x_res*2, cur_drv->y_res*2
        );
    Blit_Fullscreen_Misc ();
    blit (Blit_Buffer_Double, fs_out,
        0, 0,
        blit_cfg.dst_sx, blit_cfg.dst_sy,
        cur_drv->x_res * 2, cur_drv->y_res * 2);
}

void    Blit_Fullscreen_Eagle (void)
{
  int   i;

  // Eagle, x1 -> x2
  for (i = blit_cfg.src_sy; i < blit_cfg.src_sy + cur_drv->y_res; i ++)
  {
      eagle_mmx16 ((unsigned long *)((u16 *)screenbuffer->line[i] + blit_cfg.src_sx),
             (unsigned long *)((u16 *)screenbuffer->line[i + 1] + blit_cfg.src_sx),
              (short)cur_drv->x_res,
              screenbuffer->seg,
              Blit_Buffer_Double->line[i * 2],
              Blit_Buffer_Double->line[i * 2 + 1]);
  }
  Blit_Fullscreen_Misc ();
  blit (Blit_Buffer_Double, fs_out,
         1, blit_cfg.src_sy * 2,
         blit_cfg.dst_sx, blit_cfg.dst_sy,
         cur_drv->x_res * 2 - 1, cur_drv->y_res * 2 - 1);
}

void    Blit_Fullscreen_HQ2X (void)
{
    // Perform HQ2X into double buffer
    hq2x_16((unsigned char *)(screenbuffer->line[blit_cfg.src_sy]), (unsigned char *)(Blit_Buffer_Double->line[blit_cfg.src_sy * 2]), MAX_RES_X+32, cur_drv->y_res, (MAX_RES_X+32)*4);
    Blit_Fullscreen_Misc ();

    if (!Blitters.current->stretch)
    {
        blit (Blit_Buffer_Double, fs_out,
            blit_cfg.src_sx * 2, blit_cfg.src_sy * 2,
            blit_cfg.dst_sx, blit_cfg.dst_sy,
            cur_drv->x_res * 2 - 1, cur_drv->y_res * 2 - 1);
    }
    else
    {
        // Note: 'stretch_blit' doesn't convert!
        if (Blitters.current->video_depth != 16)
        {
            // Need this for conversion
            blit (Blit_Buffer_Double, Blit_Buffer_NativeTemp,
                blit_cfg.src_sx * 2, blit_cfg.src_sy * 2,
                blit_cfg.src_sx * 2, blit_cfg.src_sx * 2,
                cur_drv->x_res * 2 - 1, cur_drv->y_res * 2 - 1);
            stretch_blit(Blit_Buffer_NativeTemp, fs_out, 
                blit_cfg.src_sx * 2, blit_cfg.src_sy * 2,
                cur_drv->x_res * 2 - 1, cur_drv->y_res * 2 - 1,
                0,0, Video.res_x, Video.res_y);
        }
        else
        {
            stretch_blit(Blit_Buffer_Double, fs_out, 
                blit_cfg.src_sx * 2, blit_cfg.src_sy * 2,
                cur_drv->x_res * 2 - 1, cur_drv->y_res * 2 - 1,
                0,0, Video.res_x, Video.res_y);
        }
    }
}

void    Blit_Fullscreen_Scanlines (void)
{
  int   i;

  Blit_Fullscreen_Misc ();
  for (i = 0; i < cur_drv->y_res; i ++)
      {
      blit (screenbuffer, fs_out,
            blit_cfg.src_sx, blit_cfg.src_sy + i,
            blit_cfg.dst_sx, blit_cfg.dst_sy + (i * 2),
            cur_drv->x_res, 1);
      }
}

void    Blit_Fullscreen_TV_Mode (void)
{
  int   i, j;
  u8 *  psrc;
  u8 *  pdst;

  Blit_Fullscreen_Misc ();
  for (i = 0; i < cur_drv->y_res; i ++)
      {
      blit (screenbuffer, fs_out,
         blit_cfg.src_sx, blit_cfg.src_sy + i,
         blit_cfg.dst_sx, blit_cfg.dst_sy + (i * 2),
         cur_drv->x_res, 1);
      j = cur_drv->x_res;
      psrc = &screenbuffer->line[blit_cfg.src_sy + i][blit_cfg.src_sx];
      pdst = &Blit_Buffer_LineScratch->line[0][0];
      assert((j & 3) == 0);
      while (j > 4)
         {
         int color = *(int *)psrc;
         psrc += 4;
         // FIXME: the test is due to black & white colors
         // If we can have them set in the upper color area, then
         // the test could be safely removed
         // Note: & 0xC0 is to only increase game colors (0-63)
         /*
         if (!(color & 0x000000C0))
            color += GUI_COL_AVAIL_START;
         if (!(color & 0x0000C000))
            color += GUI_COL_AVAIL_START << 8;
         if (!(color & 0x00C00000))
            color += GUI_COL_AVAIL_START << 16;
         if (!(color & 0xC0000000))
            color += GUI_COL_AVAIL_START << 24;
         */
         *(int *)pdst = color;
         pdst += 4;
         j -= 4;
         }
      blit (Blit_Buffer_LineScratch, fs_out,
          0, 0,
          blit_cfg.dst_sx, blit_cfg.dst_sy + (i * 2) + 1,
          cur_drv->x_res, 1);
      }
}

void    Blit_Fullscreen_TV_Mode_Double (void)
{
  int   i;
  for (i = 0; i < cur_drv->y_res; i ++)
      {
      byte *psrc  = &screenbuffer->line[blit_cfg.src_sy + i][blit_cfg.src_sx];
      byte *pdst1 = Blit_Buffer_Double->line[(i * 2)];
      byte *pdst2 = Blit_Buffer_Double->line[(i * 2) + 1];
      int j = cur_drv->x_res;
      while (j--)
         {
         byte b = *psrc++;
         word color = b | (b << 8);
         *(word *)pdst1 = color;
         pdst1 += 2;
         // Note: & 0xC0 is to only increase game colors (0-63)
         /*
         if (!(color & 0xC0C0))
            color += GUI_COL_AVAIL_START | (GUI_COL_AVAIL_START << 8);
        */
         *(word *)pdst2 = color;
         pdst2 += 2;
         }
      }
  Blit_Fullscreen_Misc ();
  blit (Blit_Buffer_Double, fs_out,
         0, 0,
         blit_cfg.dst_sx, blit_cfg.dst_sy,
         cur_drv->x_res * 2, cur_drv->y_res * 2);
}

// Blit screenbuffer to video memory in fullscreen mode
void    Blit_Fullscreen (void)
{
    blit_cfg.src_sx = cur_drv->x_start;
    blit_cfg.src_sy = cur_drv->y_show_start;
    blit_cfg.dst_sx = Video.game_area_x1;
    blit_cfg.dst_sy = Video.game_area_y1;

    if (gui_status.timeleft && Configuration.show_fullscreen_messages)
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

    Blitters_Table [Blitters.current->blitter].func ();

    if (Blitters.current->triple_buffering)
    {
        while (poll_scroll())
            rest(0); // was: yield_timeslice(), deprecated in Allegro in favor of rest(0)

        request_video_bitmap(fs_out);
        Video.page_flipflop = (Video.page_flipflop + 1) % 3;
        switch(Video.page_flipflop)
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
    } 
    else if (Blitters.current->flip)
    {
        show_video_bitmap(fs_out);
        Video.page_flipflop ^= 1;
        if (Video.page_flipflop == 0)
            fs_out = fs_page_0;
        else
            fs_out = fs_page_1;
    }
}

void    Blitters_Get_Factors (int *x, int *y)
{
    *x = Blitters_Table[Blitters.current->blitter].x_fact;
    *y = Blitters_Table[Blitters.current->blitter].y_fact;
}

void    Blit_GUI (void)
{
    // Wait for VSync if necessary
    if (Configuration.video_mode_gui_vsync)
    {
        // FIXME: see note about line below in Blit_Fullscreen()
        if (!(fskipper.Mode == FRAMESKIP_MODE_AUTO && fskipper.Automatic_Speed > 70))
            // ^^^ I once commented this line. Was there a reason ?
            vsync ();
        // Update 3-D Glasses (if VSync)
        if (Glasses.Enabled)
            Glasses_Update ();
    }

    // Update palette if necessary
    //Palette_Sync ();

    // Blit
    switch (Configuration.video_mode_gui_access_mode)
    {
    case GUI_FB_ACCESS_DIRECT:
        // Nothing to do
        break;
    case GUI_FB_ACCESS_BUFFERED:
        blit (gui_buffer, screen, 0, 0, 0, 0, Configuration.video_mode_gui_res_x, Configuration.video_mode_gui_res_y);
        break;
    case GUI_FB_ACCESS_FLIPPED:
        // Nothing to do
        break;
    }

    // Update 3-D Glasses (if no VSync)
    if (!Configuration.video_mode_gui_vsync)
        if (Glasses.Enabled)
            Glasses_Update ();
}

//-----------------------------------------------------------------------------

