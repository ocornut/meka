//-----------------------------------------------------------------------------
// MEKA - palette.c
// Palette management - Code
//-----------------------------------------------------------------------------
// Dynamic palette management which reference count and attempting to
// minimize hardware palette change.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"

// #define DEBUG_PALETTE

int     Palette_Debug = NO;

//-----------------------------------------------------------------------------
// Palette_Init ()
// Initialize palette engine
//-----------------------------------------------------------------------------
void    Palette_Init (void)
{
    int   i;

    // Clear hardware palette
    Palette_Dirty_All = YES;
    for (i = 0; i < 256; i++)
    {
        RGB *c = &Palette_Current[i];
        c->r = c->g = c->b = c->filler = 0;
    }

    // Clear emulation palette
    Palette_Emu_Cycle_Start = 0;
    Palette_Emu_Dirty_Any = NO;
    for (i = 0; i < PALETTE_EMU_HOST_SIZE; i++)
    {
        t_color_infos *ci = &Palette_Emu_Infos[i];
        ci->idx   = i;  // Palette_Infos[] cover first 64 colors linearly
        ci->refs  = 0;  // No references yet
        ci->dirty = NO; // Not changed
        ci->lock  = NO; // Not locked
    }

    // Clear emulation references
    Palette_Refs_Dirty_Any = NO;
    for (i = 0; i < PALETTE_EMU_GAME_SIZE ; i++)
    {
        Palette_Refs [i] = 0;
        Palette_Refs_Dirty [i] = NO;
    }
}

//-----------------------------------------------------------------------------
// Palette_Close ()
// Close palette engine
//-----------------------------------------------------------------------------
void    Palette_Close (void)
{
}

//-----------------------------------------------------------------------------
// Palette_Sync ()
// Synchronize palette with hardware
//-----------------------------------------------------------------------------
void    Palette_Sync (void)
{
    int   i;
    int   sync_min = 256, sync_max = -1, sync_count = 0;

    #ifdef DEBUG_PALETTE
        // Msg (MSGT_DEBUG, "Palette_Sync()");
    #endif

    // Update GUI Colors if needed
    if (gui_palette_need_update)
        gui_palette_update ();

    // Synchronize all if requested
    if (Palette_Dirty_All)
    {
        Palette_Dirty_All = NO;
        sync_min   = 0;
        sync_max   = 255;
        sync_count = 256;

        // Flag Emulation Colors data
        if (Palette_Emu_Dirty_Any)
        {
            Palette_Emu_Dirty_Any = NO;
            for (i = 0; i < PALETTE_EMU_HOST_SIZE; i++)
                Palette_Emu_Infos [i].dirty = NO;
        }

        // Update colors for TV blitter
        if (blitters.current->tv_colors && Meka_State == MEKA_STATE_FULLSCREEN)
            for (i = 0; i < PALETTE_EMU_HOST_SIZE; i++)
            {
                RGB c = Palette_Current[i];
                c.r /= blit_cfg.tv_mode_factor;
                c.g /= blit_cfg.tv_mode_factor;
                c.b /= blit_cfg.tv_mode_factor;
                Palette_Current[i + GUI_COL_AVAIL_START] = c;
            }
    }
    else
        // Synchronize Emulation Colors
        if (Palette_Emu_Dirty_Any)
        {
            Palette_Emu_Dirty_Any = NO;

            for (i = 0; i < PALETTE_EMU_HOST_SIZE; i++)
            {
                t_color_infos *ci = &Palette_Emu_Infos [i];
                if (ci->dirty)
                {
                    ci->dirty = NO;
                    if (i < sync_min) sync_min = i;
                    if (i > sync_max) sync_max = i;
                    sync_count++;
                    // set_color (i, Palette_Current[i]);
                    #ifdef DEBUG_PALETTE
                        Msg (MSGT_DEBUG, "Updating hardware color [% 2d] -> R=% 2d, G=% 2d, B=% 2d",
                            i, Palette_Current[i].r, Palette_Current[i].g, Palette_Current[i].b);
                    #endif

                    // Update colors for TV blitter
                    if (blitters.current->tv_colors && Meka_State == MEKA_STATE_FULLSCREEN)
                    {
                        RGB c = Palette_Current[i];
                        int i2 = i + GUI_COL_AVAIL_START;
                        c.r /= blit_cfg.tv_mode_factor;
                        c.g /= blit_cfg.tv_mode_factor;
                        c.b /= blit_cfg.tv_mode_factor;
                        Palette_Current[i2] = c;
                        if (i2 < sync_min) sync_min = i2;
                        if (i2 > sync_max) sync_max = i2;
                        sync_count++;
                        // set_color (i2, &c);
                        // FIXME: 'GUI_COL_AVAIL_START', because of this, we are
                        // hardcoding the Palette_Data size to 64, while it could
                        // be theorically higher in fullscreen mode
                    }
                }
            }
        }

        // Perform palette upload on video card if necessary
        if (sync_min <= sync_max)
        {
            if (Palette_Debug)
            {
                Msg (MSGT_DEBUG, "[PALETTE] sync() min = %d, max = %d, count = %d", sync_min, sync_max, sync_count);
            }
            set_palette_range (Palette_Current, sync_min, sync_max, FALSE);
        }
}

//-----------------------------------------------------------------------------
// Palette_Sync_All ()
// Flag all palette to be re-uploaded to hardware on next synchronization
//-----------------------------------------------------------------------------
void    Palette_Sync_All (void)
{
    Palette_Dirty_All = YES;
}

//-----------------------------------------------------------------------------
// Palette_Emu_Unlock_All ()
// Unlock all emulation allocated colors
// Called at the end of emulation frame (all colors are unlocked back)
//-----------------------------------------------------------------------------
// FIXME: would be nice only to do that when necessary
//-----------------------------------------------------------------------------
void    Palette_Emu_Unlock_All (void)
{
    int   i;

    #ifdef DEBUG_PALETTE
        // Msg (MSGT_DEBUG, "Palette_Unlock_All()");
    #endif
    for (i = 0; i < PALETTE_EMU_HOST_SIZE; i++)
        Palette_Emu_Infos [i].lock = NO;
}

//-----------------------------------------------------------------------------
// Palette_SetColor (int n_hardware, RGB color)
// Set palette color
//-----------------------------------------------------------------------------
// NOTE: This must be used with care and understanding, as it currently
//       flag the whole palette as dirty.
//-----------------------------------------------------------------------------
void    Palette_SetColor (int n_hardware, RGB color)
{
    Palette_Current [n_hardware] = color;
    Palette_Dirty_All = YES;
}

//-----------------------------------------------------------------------------
// Palette_SetColor_Range (int n_hardware, int start, int end, RGB *colors)
// Set palette color range
//-----------------------------------------------------------------------------
// NOTE: This must be used with care and understanding, as it currently
//       flag the whole palette as dirty.
//-----------------------------------------------------------------------------
void    Palette_SetColor_Range (int n_start, int n_end, RGB *colors)
{
    while (n_start <= n_end)
    {
        Palette_Current [n_start] = *colors;
        n_start++;
        colors++;
    }
    Palette_Dirty_All = YES;
}

//-----------------------------------------------------------------------------
// Palette_SetColor_Reference (int n, RGB color)
// Change machine color reference to given color
// Find already existing color or allocate new one in emulation palette
//-----------------------------------------------------------------------------
void    Palette_SetColor_Reference (int n, RGB color)
{
    int   i, j;
    int   unused;
    int   ref_old;

    ref_old = Palette_Refs [n];

    // Do nothing if color of given index is already the given one
    if (*(int *)&Palette_Current [ref_old] == *(int *)&color)
        return;

    #ifdef DEBUG_PALETTE
        Msg (MSGT_DEBUG, "(%03d/%s) Palette_SetColor_Reference(%i, RGB=%i,%i,%i)",
            tsms.VDP_Line, ((Display_ON) ? "On" : "Off"), n, color.r, color.g, color.b);
    #endif

    // Set emulated palette dirty flag
    Palette_Refs_Dirty [n] = YES;
    Palette_Refs_Dirty_Any = YES;

    // Decrease reference counter to the previous color
    Palette_Emu_Infos [ref_old].refs --;
    #ifdef DEBUG_PALETTE
        // This should never happens
        if (Palette_Emu_Infos [ref_old].refs < 0)
            Msg (MSGT_DEBUG, "Palette_SetColor_Reference(%i): Palette_Emu_Infos[%i].refs == %i !",
            n, ref_old, Palette_Emu_Infos[ref_old].refs);
    #endif

    // Set lock if needed
    // Note: due to test below,
    // it is MANDATORY that Palette_Sync is called on cur_drv->y_show_end

    // if ((tsms.VDP_Line < cur_drv->y_show_start || tsms.VDP_Line > cur_drv->y_show_end)
    /*    && Palette_Emu_Infos [ref_old].refs == 0 */ //)
    //    Palette_Emu_Infos [ref_old].lock = NO;

    // if (tsms.VDP_Line >= cur_drv->y_show_start && tsms.VDP_Line <= cur_drv->y_show_end)
    if (Palette_Emu_Infos [ref_old].refs == 0)
    {
        #ifdef DEBUG_PALETTE
            Msg (MSGT_DEBUG, " -> Locking old reference %i (was set on line %i)", ref_old, Palette_Data_Line [ref_old]);
        #endif
        Palette_Emu_Infos [ref_old].lock = YES;
    }

    // Increment cycle position, so allocation will at best be done cyclicly
    // (this is a simple scheme to avoid overwriting colors that may be reused
    // very soon)
    Palette_Emu_Cycle_Start = (Palette_Emu_Cycle_Start + 1) % PALETTE_EMU_HOST_SIZE;

    // Search for this color in existing hardware palette
    unused = -1;
    i = Palette_Emu_Cycle_Start;
    for (j = 0; j < PALETTE_EMU_HOST_SIZE; j++)
    {
        if (*(int*)&Palette_Current[i] == *(int*)&color)
        {
            #ifdef DEBUG_PALETTE
                Msg (MSGT_DEBUG, " -> New reference is %i", i);
            #endif
            // If found..
            // Increase reference counter
            Palette_Emu_Infos[i].refs ++;
            // Set emulated palette table reference
            Palette_Refs[n] = i;
            // Then return from the function, we're done!
            return;
        }
        if (unused == -1 && Palette_Emu_Infos[i].refs == 0 && Palette_Emu_Infos[i].lock == NO)
            unused = i;
        i = (i + 1) & 63; // % PALETTE_EMU_HOST_SIZE;
    }

    #ifdef DEBUG_PALETTE
        // Msg (MSGT_DEBUG, "(%03d/%s) Palette_Set(%i, RGB=%i,%i,%i)", tsms.VDP_Line, Display_ON ? "On" : "Off", n, rgb.r, rgb.g, rgb.b);
        Msg (MSGT_DEBUG, " -> not found, will modify hardware palette");
    #endif

    // Color was not found. We will have to modify one of the hardware color
    // If none is available -> palette overflow!
    if (unused == -1)
    {
        // #ifdef DEBUG_PALETTE
        Msg (MSGT_DEBUG, "** PALETTE OVERFLOW **");
        // #endif
        // Set "unused" one as the one on the same location, we will overwrite it
        // unused = Palette_Refs [n];
        unused = ref_old;
    }

    // Set dirty flag
    Palette_Emu_Dirty_Any = YES;
    Palette_Emu_Infos [unused].dirty = YES;
    // Increment reference counter (== 1 if we're not on a palette overflow)
    Palette_Emu_Infos [unused].refs ++;
    // Set color (will be updated on next Palette_Sync)
    Palette_Current [unused] = color;

    // if (tsms.VDP_Line < cur_drv->y_show_start || tsms.VDP_Line > cur_drv->y_show_end)
    //    Palette_Emu_Infos [unused].lock = YES;
    // Palette_Emu_Infos [unused].lock = YES;

    // Set emulated palette table reference
    Palette_Refs [n] = unused;
}

//-----------------------------------------------------------------------------
// Palette_SetColor_Reference_Force (int n, int hn)
// Force reference
//-----------------------------------------------------------------------------
// NOTE: Only used by machine/driver initialization
//-----------------------------------------------------------------------------
void    Palette_SetColor_Reference_Force (int n_machine, int n_emu)
{
    // Increment reference counter on emulation allocated palette
    Palette_Emu_Infos [n_emu].refs ++;
    // Set reference
    Palette_Refs [n_machine]       = n_emu;
    Palette_Refs_Dirty [n_machine] = YES;
    Palette_Refs_Dirty_Any         = YES;
}

//-----------------------------------------------------------------------------
// Palette_SetColor_Emulation (int n, RGB color)
// Set emulation color directly
//-----------------------------------------------------------------------------
// NOTE: Only used by machine/driver initialization.
//       Note how it set .refs to zero which is "initialization" behavior.
//-----------------------------------------------------------------------------
void    Palette_SetColor_Emulation (int n_emu, RGB color)
{
    Palette_Emu_Dirty_Any = YES;
    Palette_Emu_Infos [n_emu].refs  = 0;
    Palette_Emu_Infos [n_emu].dirty = YES;
    // Update hardware palette
    Palette_Current [n_emu] = color;
}

// Called when reseting machine -----------------------------------------------
void    Palette_Emu_Reset (void)
{
    int   i;
    RGB   color;
    byte  PRAM_Fake;
    byte *PRAM_Backup;

    switch (cur_drv->vdp)
    {
    case VDP_TMS:  TMS9918_Palette_Set ();      return;
    case VDP_NES:  NES_Palette_Set ();          return;
    }
    // cur_drv->vdp == VDP_SMSGG
    // SMS Palette will be entirely set by default
#ifdef DEBUG_PALETTE
    Msg (MSGT_DEBUG, "Palette_Emu_Reset() SMS");
#endif

    // Set default emulation palette (to SMS palette)
    PRAM_Backup = PRAM;
    for (i = 0; i < 64; i++)
    {
        PRAM_Fake = i;
        PRAM = &PRAM_Fake - i;
        Palette_Compute_RGB_SMS (&color, i); // When looking at PRAM[i] it will be PRAM_Fake !
        Palette_SetColor_Emulation (i, color);
    }
    PRAM = PRAM_Backup;

    // Set all default references to 0 (black color on the machine)
    for (i = 0; i < 32; i++)
        Palette_SetColor_Reference_Force (i, 0);
}

// Reload palette data (fixed or from PRAM) -----------------------------------
// Called when changing video mode on the fly ---------------------------------
void    Palette_Emu_Reload (void)
{
    int   i;
    RGB   color;

    switch (cur_drv->vdp)
    {
    case VDP_TMS:  TMS9918_Palette_Set ();      return;
    case VDP_NES:  NES_Palette_Set ();          return;
    }

    // cur_drv->vdp == VDP_SMSGG
    // SMS/GG Palette will be reloaded
#ifdef DEBUG_PALETTE
    Msg (MSGT_DEBUG, "Palette_Emu_Reload() SMS/GG");
#endif

    switch (cur_drv->id)
    {
    case DRV_SMS:
        for (i = 0; i < 32; i++)
        {
            Palette_Compute_RGB_SMS (&color, i);
            Palette_SetColor_Reference (i, color);
        }
        break;
    case DRV_GG:
        for (i = 0; i < 32; i++)
        {
            Palette_Compute_RGB_GG (&color, i * 2);
            Palette_SetColor_Reference (i, color);
        }
        break;
    }
}

//-----------------------------------------------------------------------------

void    Palette_Compute_RGB_SMS (RGB *color, int i)
{
    if (Configuration.palette_type == PALETTE_TYPE_BRIGHT)
    {
        color->r = (PRAM [i] & 0x03) | ((PRAM [i] << 2) & 0x0C) | ((PRAM [i] << 4) & 0x30);
        color->g = ((PRAM [i] >> 2) & 0x03) | (PRAM [i] & 0x0C) | ((PRAM [i] << 2) & 0x30);
        color->b = ((PRAM [i] >> 4) & 0x03) | ((PRAM [i] >> 2) & 0x0C) | (PRAM [i] & 0x30);
    }
    else // Configuration.palette_type == PALETTE_TYPE_MUTED
    {
        color->r = (PRAM [i] & 0x03) << 4;
        color->g = (PRAM [i] & 0x0C) << 2;
        color->b = (PRAM [i] & 0x30);
    }
    // Mask colors components and set filler to zero
    (*(int *)color) &= 0x003F3F3F;
    // Color->r &= 63; Color->g &= 63; Color->b &= 63; // Is this needed ?
    // Color->filler = 0x00; // This is needed, as we're comparing RGB directly
}

// Note: if changing the meaning of 'i', please update datadump.c which uses it
void    Palette_Compute_RGB_GG (RGB *color, int i)
{
    if (Configuration.palette_type == PALETTE_TYPE_BRIGHT)
    {
        // ----bbbb ggggrrrr (GG) -> --rrrrrr --gggggg --bbbbbb (RGB)
        color->r = ((PRAM [i] >> 2) & 0x03) | ((PRAM [i] << 2) & 0x3C);
        color->g = ((PRAM [i] >> 6) & 0x03) | ((PRAM [i] >> 2) & 0x3C);
        color->b = ((PRAM[i+1]>> 2) & 0x03) | ((PRAM[i+1]<< 2) & 0x3C);
    }
    else // Configuration.palette_type == PALETTE_TYPE_MUTED
    {
        color->r = (PRAM [i] & 0x0F) << 2;
        color->g = (PRAM [i] & 0xF0) >> 2;
        color->b = (PRAM [i + 1] & 0x0F) << 2;
    }
    // Mask colors components and set filler to zero
    (*(int *)color) &= 0x003F3F3F;
    // Color->r &= 63; Color->g &= 63; Color->b &= 63; // Is this needed ?
    // Color->filler = 0x00; // This is needed, as we're comparing RGB directly
}

//-----------------------------------------------------------------------------
