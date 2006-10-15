//-----------------------------------------------------------------------------
// MEKA - blit.c
// Blitters - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "fskipper.h"
#include "vdp.h"

//-----------------------------------------------------------------------------

void    Blit_Init (void)
{
    Work_Line = create_bitmap (MAX_RES_X * 2, 1);
    blit_cfg.tv_mode_factor = 1.5f;
}

t_blitters_table_entry Blitters_Table[BLITTER_MAX] =
{
    { Blit_Fullscreen_Normal,           1,      1 },
    { Blit_Fullscreen_Double,           2,      2 },
    { Blit_Fullscreen_Scanlines,        1,      2 },
    { Blit_Fullscreen_TV_Mode,          1,      2 },
    { Blit_Fullscreen_Parallel,         2,      1 },
    { Blit_Fullscreen_TV_Mode_Double,   2,      2 },
#ifdef MEKA_EAGLE
    { Blit_Fullscreen_Eagle,            2,      2 }
#endif
};

void    Blit_Fullscreen_Misc (void)
{
    // Wait for VSync if necessary
    // (not done if speed is higher than 70 hz)
    // FIXME: 70 should be replaced by actual screen refresh rate ... can we obtain it ?
    Clock_Start (CLOCK_VSYNC);
    if (blitters.current->vsync)
        if (!(fskipper.Mode == FRAMESKIP_MODE_AUTO && fskipper.Automatic_Speed > 70))
            vsync ();
    Clock_Stop (CLOCK_VSYNC);

    // Clear Screen if it has been asked
    if (Video.clear_need)
    {
        // int cpu_capabilities_backup = cpu_capabilities;
        // cpu_capabilities &= ~CPU_MMX;
        Video.clear_need = NO;
        if (blitters.current->flip)
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
    Clock_Start (CLOCK_GFX_PALETTE);
    Palette_Sync ();
    Clock_Stop (CLOCK_GFX_PALETTE);
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

    fy = Blitters_Table[blitters.current->blitter].y_fact;
    y = blit_cfg.src_sy + cur_drv->y_res;
    if (y * fy > Video.res_y)
        y -= ((y * fy) - Video.res_y) / (fy * 2);
    y -= 14;

    Font_SetCurrent (F_SMALL);
    // FIXME: use a dedicated font. This is slow as hell!!
    Font_Print (-1, screenbuffer, gui_status.message, x - 1, y - 1, GUI_COL_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x,     y - 1, GUI_COL_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x + 1, y - 1, GUI_COL_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x - 1, y + 1, GUI_COL_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x,     y + 1, GUI_COL_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x + 1, y + 1, GUI_COL_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x - 1, y,     GUI_COL_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x + 1, y,     GUI_COL_BLACK);
    Font_Print (-1, screenbuffer, gui_status.message, x,     y,     GUI_COL_WHITE);
}

void    Blit_Fullscreen_Normal (void)
{
    Blit_Fullscreen_Misc ();
    blit (screenbuffer, fs_out,
        blit_cfg.src_sx, blit_cfg.src_sy,
        blit_cfg.dst_sx, blit_cfg.dst_sy,
        cur_drv->x_res,  cur_drv->y_res);
}

void    Blit_Fullscreen_Double (void)
{
  byte  b;
  int   i, j;
  byte *psrc;
  byte *pdst1;
  byte *pdst2;

  for (i = 0; i < cur_drv->y_res; i ++)
      {
      psrc = screenbuffer->line[i + blit_cfg.src_sy] + blit_cfg.src_sx;
      pdst1 = double_buffer->line[(i * 2)];
      pdst2 = double_buffer->line[(i * 2) + 1];
      j = cur_drv->x_res;
      while (j --)
         {
         b = *psrc ++;
         *pdst1++ = b; *pdst2++ = b;
         *pdst1++ = b; *pdst2++ = b;
         }
      }
  Blit_Fullscreen_Misc ();
  blit (double_buffer, fs_out,
         0, 0,
         blit_cfg.dst_sx, blit_cfg.dst_sy,
         cur_drv->x_res * 2, cur_drv->y_res * 2);
}

#ifdef MEKA_EAGLE
void    Blit_Fullscreen_Eagle (void)
{
  int   i;

  for (i = blit_cfg.src_sy; i < blit_cfg.src_sy + cur_drv->y_res; i ++)
      eagle ((unsigned long *)(screenbuffer->line[i] + blit_cfg.src_sx),
             (unsigned long *)(screenbuffer->line[i + 1] + blit_cfg.src_sx),
              (short)cur_drv->x_res,
              screenbuffer->seg,
              double_buffer->line[i * 2],
              double_buffer->line[i * 2 + 1]);

  Blit_Fullscreen_Misc ();
  blit (double_buffer, fs_out,
         1, blit_cfg.src_sy * 2,
         blit_cfg.dst_sx, blit_cfg.dst_sy,
         cur_drv->x_res * 2 - 1, cur_drv->y_res * 2 - 1);
}

#endif /* ifdef MEKA_EAGLE */

void    Blit_Fullscreen_Parallel (void)
{
  Blit_Fullscreen_Misc ();
  blit (screenbuffer_1, fs_out,
         blit_cfg.src_sx, blit_cfg.src_sy,
         blit_cfg.dst_sx, blit_cfg.dst_sy,
         cur_drv->x_res, cur_drv->y_res);
  blit_cfg.dst_sx += cur_drv->x_res;
  blit (screenbuffer_2, fs_out,
         blit_cfg.src_sx, blit_cfg.src_sy,
         blit_cfg.dst_sx, blit_cfg.dst_sy,
         cur_drv->x_res, cur_drv->y_res);
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
  byte  b;
  int   i, j;
  byte *psrc;
  byte *pdst;

  Blit_Fullscreen_Misc ();
  for (i = 0; i < cur_drv->y_res; i ++)
      {
      blit (screenbuffer, fs_out,
         blit_cfg.src_sx, blit_cfg.src_sy + i,
         blit_cfg.dst_sx, blit_cfg.dst_sy + (i * 2),
         cur_drv->x_res, 1);
      j = cur_drv->x_res;
      psrc = &screenbuffer->line[blit_cfg.src_sy + i][blit_cfg.src_sx];
      pdst = &Work_Line->line[0][0];
      while (j > 4)
         {
         int color = *((int *)psrc)++;
         // FIXME: the test is due to black & white colors
         // If we can have them set in the upper color area, then
         // the test could be safely removed
         // Note: & 0xC0 is to only increase game colors (0-63)
         if (!(color & 0x000000C0))
            color += GUI_COL_AVAIL_START;
         if (!(color & 0x0000C000))
            color += GUI_COL_AVAIL_START << 8;
         if (!(color & 0x00C00000))
            color += GUI_COL_AVAIL_START << 16;
         if (!(color & 0xC0000000))
            color += GUI_COL_AVAIL_START << 24;
         *((int *)pdst)++ = color;
         j -= 4;
         }
      while (j--)
         {
         b = *psrc++;
         if (!(b & 0xC0))
            b += GUI_COL_AVAIL_START;
         *pdst++ = b;
         }
      blit (Work_Line, fs_out,
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
      byte *pdst1 = double_buffer->line[(i * 2)];
      byte *pdst2 = double_buffer->line[(i * 2) + 1];
      int j = cur_drv->x_res;
      while (j--)
         {
         byte b = *psrc++;
         word color = b | (b << 8);
         *((word *)pdst1)++ = color;
         // Note: & 0xC0 is to only increase game colors (0-63)
         if (!(color & 0xC0C0))
            color += GUI_COL_AVAIL_START | (GUI_COL_AVAIL_START << 8);
         *((word *)pdst2)++ = color;
         }
      }
  Blit_Fullscreen_Misc ();
  blit (double_buffer, fs_out,
         0, 0,
         blit_cfg.dst_sx, blit_cfg.dst_sy,
         cur_drv->x_res * 2, cur_drv->y_res * 2);
}

// Blit screenbuffer to video memory in fullscreen mode
void    Blit_Fullscreen (void)
{
  Clock_Start (CLOCK_GFX_BLIT);
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
      //Font_Print (screenbuffer, buf, 49 + (fskipper.FPS_Temp % 10) * 6, 49 + (fskipper.FPS_Temp / 10)*10, GUI_COL_BLACK);
      //Font_Print (screenbuffer, buf, 50 + (fskipper.FPS_Temp % 10) * 6, 50 + (fskipper.FPS_Temp / 10)*10, GUI_COL_WHITE);
      Font_Print (screenbuffer, buf, 19, 49, GUI_COL_BLACK);
      Font_Print (screenbuffer, buf, 20, 50, GUI_COL_WHITE);
  }
#endif

  Blitters_Table [blitters.current->blitter].func ();

  if (blitters.current->flip)
     {
     show_video_bitmap (fs_out);
     Video.page_flipflop ^= 1;
     if (Video.page_flipflop == 0)
        fs_out = fs_page_0;
     else
        fs_out = fs_page_1;
     }

  Clock_Stop (CLOCK_GFX_BLIT);
}

void    Blitters_Get_Factors (int *x, int *y)
{
    *x = Blitters_Table[blitters.current->blitter].x_fact;
    *y = Blitters_Table[blitters.current->blitter].y_fact;
}

void    Blit_GUI (void)
{
    // Wait for VSync if necessary
    Clock_Start (CLOCK_VSYNC);
    if (cfg.GUI_VSync)
    {
        // FIXME: see note about line below in Blit_Fullscreen()
        if (!(fskipper.Mode == FRAMESKIP_MODE_AUTO && fskipper.Automatic_Speed > 70))
            // ^^^ I once commented this line. Was there a reason ?
            vsync ();
        // Update 3-D Glasses (if VSync)
        if (Glasses.Enabled)
            Glasses_Update ();
    }
    Clock_Stop (CLOCK_VSYNC);

    // Update palette if necessary
    Clock_Start (CLOCK_GFX_PALETTE);
    Palette_Sync ();
    Clock_Stop (CLOCK_GFX_PALETTE);

    // Blit
    Clock_Start (CLOCK_GUI_BLIT);
    switch (cfg.GUI_Access_Mode)
    {
    case GUI_FB_ACCESS_DIRECT:
        // Nothing to do
        break;
    case GUI_FB_ACCESS_BUFFERED:
        blit (gui_buffer, screen, 0, 0, 0, 0, cfg.GUI_Res_X, cfg.GUI_Res_Y);
        break;
    case GUI_FB_ACCESS_FLIPPED:
        // Nothing to do
        break;
    }

    // Update 3-D Glasses (if no VSync)
    if (!cfg.GUI_VSync)
        if (Glasses.Enabled)
            Glasses_Update ();
    Clock_Stop (CLOCK_GUI_BLIT);
}

//-----------------------------------------------------------------------------

