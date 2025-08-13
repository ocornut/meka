//-----------------------------------------------------------------------------
// MEKA - blit.c
// Blitters - Code
//-----------------------------------------------------------------------------
// FIXME: lots of room for optimization in the copying functions.
// FIXME: need a full rewrite/rethinking. Now that hi-color modes are well
// supported, this will be more straightforward than before.
//-----------------------------------------------------------------------------
//
// Typical resolutions:
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
#include "fskipper.h"
#include "glasses.h"
#include "hq2x.h"
#include "palette.h"
#include "vdp.h"
#include "video.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_allegro5.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

ALLEGRO_BITMAP * Blit_Buffer_LineScratch = NULL;    // Line buffer scratch pad, 16-bits
ALLEGRO_BITMAP * Blit_Buffer_Double = NULL;         // Double-sized buffer, 16-bits

struct t_blitters_table_entry
{
    void    (*blit_func)();
    int     x_fact;
    int     y_fact;
};

struct t_blit_cfg
{
    int     src_pos_x;
    int     src_pos_y;
    int     src_size_x;
    int     src_size_y;
    int     dst_scale;
    float   tv_mode_factor;
};

t_blit_cfg  g_blit;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Blit_Init()
{
    g_blit.src_pos_x = 0;
    g_blit.src_pos_y = 0;
    g_blit.src_size_x = 0;
    g_blit.src_size_y = 0;
    g_blit.dst_scale = 0;
    g_blit.tv_mode_factor = 0.700f; // FIXME-TUNING
    Blit_CreateVideoBuffers();

    // Initialize HQ2X filters
    HQ2X_Init();
}

void    Blit_DestroyVideoBuffers()
{
    if (Blit_Buffer_LineScratch)
    {
        al_destroy_bitmap(Blit_Buffer_LineScratch);
        Blit_Buffer_LineScratch = NULL;
    }
    if (Blit_Buffer_Double)
    {
        al_destroy_bitmap(Blit_Buffer_Double);
        Blit_Buffer_Double = NULL;
    }
}

void    Blit_CreateVideoBuffers()
{
    Blit_DestroyVideoBuffers();

    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_NO_PRESERVE_TEXTURE);
    al_set_new_bitmap_format(g_config.video_game_format_request);
    Blit_Buffer_LineScratch = al_create_bitmap(MAX_RES_X * 2, 1);
    Blit_Buffer_Double      = al_create_bitmap((MAX_RES_X + 32) * 2, (MAX_RES_Y + 32)*2);
}

static const t_blitters_table_entry     Blitters_Table[BLITTER_MAX] =
{
    { Blit_Fullscreen_Normal,           1,      1 },
    { Blit_Fullscreen_TV_Mode,          1,      2 },
    { Blit_Fullscreen_TV_Mode_Double,   2,      2 },
    { Blit_Fullscreen_HQ2X,             2,      2 },
};

static void Blit_Fullscreen_Misc()
{
    // Wait for VSync if necessary
    // (not done if speed is higher than 70 hz)
    // FIXME: 70 should be replaced by actual screen refresh rate ... how can we obtain it ?
    if (g_config.video_mode_game_vsync)
    {
        if (!(fskipper.Mode == FRAMESKIP_MODE_THROTTLED && fskipper.Throttled_Speed > 70))
            al_wait_for_vsync();
    }

    // Clear Screen if it has been asked
    if (g_video.clear_requests > 0)
    {
        g_video.clear_requests--;
        al_set_target_bitmap(al_get_backbuffer(g_display));
        al_clear_to_color(BORDER_COLOR);
    }

    // Update 3-D Glasses
    if (Glasses.Enabled)
        Glasses_Update();
}

static void Blit_Fullscreen_Message(ALLEGRO_BITMAP* dst, float time_remaining)
{
    al_set_target_bitmap(dst);

    int x = 10;
    int y = al_get_bitmap_height(dst) - 16;
    if (time_remaining < 0.4f)
        y += (int)ImLinearRemapClamp(0.4f, 0.0f, 0.0f, 20.0f, time_remaining);
    al_draw_filled_rectangle(0, y-6, al_get_bitmap_width(dst), al_get_bitmap_height(dst), al_map_rgba(0,0,0,128));

    // FIXME-OPT: use a dedicated font. This is slow as hell!!
    Font_SetCurrent(FONTID_LARGE);
    Font_Print(FONTID_CUR, g_gui_status.message, x - 1, y - 1, COLOR_BLACK);
    Font_Print(FONTID_CUR, g_gui_status.message, x,     y - 1, COLOR_BLACK);
    Font_Print(FONTID_CUR, g_gui_status.message, x + 1, y - 1, COLOR_BLACK);
    Font_Print(FONTID_CUR, g_gui_status.message, x - 1, y + 1, COLOR_BLACK);
    Font_Print(FONTID_CUR, g_gui_status.message, x,     y + 1, COLOR_BLACK);
    Font_Print(FONTID_CUR, g_gui_status.message, x + 1, y + 1, COLOR_BLACK);
    Font_Print(FONTID_CUR, g_gui_status.message, x - 1, y,     COLOR_BLACK);
    Font_Print(FONTID_CUR, g_gui_status.message, x + 1, y,     COLOR_BLACK);
    Font_Print(FONTID_CUR, g_gui_status.message, x,     y,     COLOR_WHITE);
}

void Blit_Fullscreen_UpdateBounds()
{
    // Scale implied by the blitter effect (eg: HQ2X double the resolution)
    const int blit_scale_x = Blitters_Table[Blitters.current->blitter].x_fact;
    const int blit_scale_y = Blitters_Table[Blitters.current->blitter].y_fact;

    g_blit.src_pos_x = blit_scale_x * g_driver->x_start;
    g_blit.src_pos_y = blit_scale_y * g_driver->y_show_start;
    g_blit.src_size_x = blit_scale_x * g_driver->x_res;
    g_blit.src_size_y = blit_scale_y * g_driver->y_res;
    g_blit.dst_scale = 1;

    const t_blitter_stretch stretch_mode = Blitters.current->stretch;
    if (stretch_mode == BLITTER_STRETCH_MAX_INT)
    {
        // Automatic integer scale
        const int scale_x = (int)g_video.res_x / g_blit.src_size_x;
        const int scale_y = (int)g_video.res_y / g_blit.src_size_y;
        g_blit.dst_scale = MIN(scale_x, scale_y);
    }

    if (stretch_mode == BLITTER_STRETCH_MAX)
    {
        // Cover all screen
        g_video.game_area_x1 = 0;
        g_video.game_area_y1 = 0;
        g_video.game_area_x2 = g_video.res_x;
        g_video.game_area_y2 = g_video.res_y;
        g_blit.dst_scale = 0;
    }
    else
    {
        // Integer scale
        g_video.game_area_x1 = (g_video.res_x - g_blit.src_size_x*g_blit.dst_scale) / 2;
        g_video.game_area_y1 = (g_video.res_y - g_blit.src_size_y*g_blit.dst_scale) / 2;
        g_video.game_area_x2 = g_video.game_area_x1 + g_blit.src_size_x*g_blit.dst_scale;
        g_video.game_area_y2 = g_video.game_area_y1 + g_blit.src_size_y*g_blit.dst_scale;
    }
}

// This is the actual final blitting function.
static void Blit_Fullscreen_CopyStretch(ALLEGRO_BITMAP *src_buffer)
{
    al_set_target_bitmap(fs_out);
    if (g_blit.dst_scale == 1)
    {
        al_draw_bitmap_region(src_buffer,
            g_blit.src_pos_x, g_blit.src_pos_y,
            g_blit.src_size_x, g_blit.src_size_y,
            g_video.game_area_x1, g_video.game_area_y1,
            0x0000);
    }
    else
    {
        al_draw_scaled_bitmap(src_buffer,
            g_blit.src_pos_x, g_blit.src_pos_y,
            g_blit.src_size_x, g_blit.src_size_y,
            g_video.game_area_x1, g_video.game_area_y1,
            g_video.game_area_x2 - g_video.game_area_x1, g_video.game_area_y2 - g_video.game_area_y1,
            0x0000);
    }
}

void    Blit_Fullscreen_Normal()
{
    Blit_Fullscreen_Misc();
    Blit_Fullscreen_CopyStretch(screenbuffer);
}

void    Blit_Fullscreen_HQ2X()
{
    // Perform HQ2X into double buffer
    // FIXME-OPT: Applied on full width.
#if 0 // FIXME-ALLEGRO5: blitter hq2x
    ALLEGRO_LOCKED_REGION* lr_src = al_lock_bitmap(screenbuffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
    ALLEGRO_LOCKED_REGION* lr_dst = al_lock_bitmap(Blit_Buffer_Double, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);

    u8* addr_src = ((u8*)lr_src->data + (lr_src->pitch * blit_cfg.src_sy * 1) + (0 * sizeof(u16)));
    u8* addr_dst = ((u8*)lr_dst->data + (lr_dst->pitch * blit_cfg.src_sy * 2) + (0 * sizeof(u16)));
    hq2x_16(addr_src, addr_dst, MAX_RES_X+32, g_driver->y_res, (MAX_RES_X+32)*4);
    al_unlock_bitmap(screenbuffer);
    al_unlock_bitmap(Blit_Buffer_Double);
#endif
    Blit_Fullscreen_Misc();
    Blit_Fullscreen_CopyStretch(Blit_Buffer_Double);
}

void    Blit_Fullscreen_TV_Mode()
{
#if 0 // FIXME-ALLEGRO5: blitter tv mode
    int i;
    for (i = 0; i < g_driver->y_res; i ++)
    {
        const u16 *psrc  = (u16 *)screenbuffer->line[blit_cfg.src_sy + i] + blit_cfg.src_sx;
        u16 *pdst1 = (u16 *)Blit_Buffer_Double->line[(blit_cfg.src_sy + i) * 2] + (blit_cfg.src_sx * 1);
        u16 *pdst2 = (u16 *)Blit_Buffer_Double->line[(blit_cfg.src_sy + i) * 2 + 1] + (blit_cfg.src_sx * 1);
        int j = g_driver->x_res;
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
    Blit_Fullscreen_CopyStretch(Blit_Buffer_Double);
}

// FIXME-OPT: Obviously this is very slow. Just trying to get something working for 0.72. Later shall work better solution (generating inline assembly, etc).
void    Blit_Fullscreen_TV_Mode_Double()
{
#if 0 // FIXME-ALLEGRO5: blitter tv mode double
    int i;
    for (i = 0; i < g_driver->y_res; i ++)
    {
        const u16 *psrc  = (u16 *)screenbuffer->line[blit_cfg.src_sy + i] + blit_cfg.src_sx;
        u16 *pdst1 = (u16 *)Blit_Buffer_Double->line[(blit_cfg.src_sy + i) * 2] + (blit_cfg.src_sx * 2);
        u16 *pdst2 = (u16 *)Blit_Buffer_Double->line[(blit_cfg.src_sy + i) * 2 + 1] + (blit_cfg.src_sx * 2);
        int j = g_driver->x_res;
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
#endif
    Blit_Fullscreen_Misc();
    Blit_Fullscreen_CopyStretch(Blit_Buffer_Double);
}

static void Blit_RenderImGui()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
}

// Blit screenbuffer to video memory in fullscreen mode
void    Blit_Fullscreen()
{
    Blit_Fullscreen_UpdateBounds();

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

    Blitters_Table[Blitters.current->blitter].blit_func();

    if (g_gui_status.time_remaining > 0.0f && g_config.show_fullscreen_messages)
    {
        Blit_Fullscreen_Message(fs_out, g_gui_status.time_remaining);
        g_gui_status.time_remaining -= ImGui::GetIO().DeltaTime;
    }

    Blit_RenderImGui();

    al_flip_display();
}

void    Blit_GUI()
{
    // Wait for VSync if necessary
    if (g_config.video_mode_gui_vsync)
    {
        // FIXME: see note about line below in Blit_Fullscreen()
        if (!(fskipper.Mode == FRAMESKIP_MODE_THROTTLED && fskipper.Throttled_Speed > 70))
            al_wait_for_vsync();
        // Update 3-D Glasses (if VSync)
        if (Glasses.Enabled)
            Glasses_Update();
        PROFILE_STEP("gui_vsync");
    }

    // Blit
    ALLEGRO_BITMAP* backbuffer = al_get_backbuffer(g_display);
    al_set_target_bitmap(backbuffer);
    al_draw_bitmap(gui_buffer, 0, 0, 0x0000);
    PROFILE_STEP("al_draw_bitmap()");

    Blit_RenderImGui();

    al_flip_display();
    PROFILE_STEP("al_flip_display");

    // Update 3-D Glasses (if no VSync)
    if (!g_config.video_mode_gui_vsync)
        if (Glasses.Enabled)
            Glasses_Update();
}

//-----------------------------------------------------------------------------
