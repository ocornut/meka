//-----------------------------------------------------------------------------
// MEKA - tileview.c
// Tile Viewer - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"
#include "tileview.h"

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

void    TileViewer_Change_Palette (void);
void    TileViewer_SelectedTile_Select (t_widget *w);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// DRAW A 8x8 SPRITE TILE -----------------------------------------------------
static void     Draw_One_Tile (byte *Palette, byte *p0, int x, int y)
{
    int         i;
    u8 *        p2;

    for (i = 0; i < 8; i ++)
    {
        p2 = &apps.gfx.Tiles->line[y++][x];
        *p2++ = Palette[*p0++];
        *p2++ = Palette[*p0++];
        *p2++ = Palette[*p0++];
        *p2++ = Palette[*p0++];
        *p2++ = Palette[*p0++];
        *p2++ = Palette[*p0++];
        *p2++ = Palette[*p0++];
        *p2   = Palette[*p0++];
    }
}

void    TileViewer_Init_Values (void)
{
    TileViewer.palette          = 0;
    TileViewer.palette_max      = 2;
    TileViewer.tile_displayed   = -1;
    TileViewer.tile_hovered     = -1;
    TileViewer.tile_selected    = -1;
    TileViewer.tiles_count      = 448;
    TileViewer.tiles_width      = -1;
    TileViewer.tiles_height     = -1;
    TileViewer.tiles_display_zone   = NULL;
}

// CREATE AND INITIALIZE TILE VIEWER APPLET ----------------------------------
void    TileViewer_Init (void)
{
    int tile_size_x, tile_size_y;
    t_frame frame;

    TileViewer.tiles_width  = 16;
    TileViewer.tiles_height = TileViewer.tiles_count / TileViewer.tiles_width;
    tile_size_x = TileViewer.tiles_width  * 8;
    tile_size_y = TileViewer.tiles_height * 8;

    apps.id.Tiles = gui_box_create (503, 48, tile_size_x - 1, tile_size_y + 13 - 1,
        Msg_Get (MSG_TilesViewer_BoxTitle));
    apps.gfx.Tiles = create_bitmap (tile_size_x, tile_size_y + 13);
    gui_set_image_box (apps.id.Tiles, apps.gfx.Tiles);
    gui.box [apps.id.Tiles]->update = TileViewer_Update;
    widget_closebox_add (apps.id.Tiles, TileViewer_Switch);

    frame.pos.x = 0;
    frame.pos.y = 0;
    frame.size.x = tile_size_x - 1;
    frame.size.y = tile_size_y - 1;
    TileViewer.tiles_display_zone = widget_button_add (apps.id.Tiles, &frame, 1, TileViewer_SelectedTile_Select);
    widget_button_add (apps.id.Tiles, &frame, 2, TileViewer_Change_Palette);
    line (apps.gfx.Tiles, 0, tile_size_y, tile_size_x - 1, tile_size_y, GUI_COL_BORDERS);
    gui_rect (apps.gfx.Tiles, LOOK_THIN, 2, tile_size_y + 1, 2 + 11, tile_size_y + 1 + 11, GUI_COL_BORDERS);

    Desktop_Register_Box ("TILES", apps.id.Tiles, 0, &apps.active.Tiles);
}

// UPDATE THE TILE VIEWER APPLET ----------------------------------------------
void    TileViewer_Update (void)
{
    int     x, y;
    int     must_redraw = NO;
    int     tile_current;
    bool    tile_current_refresh;
    int     tile_current_addr;
    bool    palette_dirty = Palette_Emu_Dirty_Any || Palette_Refs_Dirty_Any;

    // Update hovered tile index
    {
        int mx = TileViewer.tiles_display_zone->mx;
        int my = TileViewer.tiles_display_zone->my;
        // Msg (MSGT_USER, "mx = %d, my = %d", mx, my);
        if (mx != -1 && my != -1)
            TileViewer.tile_hovered = ((my / 8) * 16) + mx / 8;
        else
            TileViewer.tile_hovered = -1;
    }

    // Compute the tile that is to display in the bottom info line
    tile_current = (TileViewer.tile_hovered != -1) ? TileViewer.tile_hovered : TileViewer.tile_selected;
    tile_current_refresh = (tile_current == -1) ? FALSE : ((tile_current != TileViewer.tile_displayed || palette_dirty || tgfx.Tile_Dirty [tile_current]));
    tile_current_addr = -1;

    // Then redraw all tiles
    switch (cur_drv->vdp)
    {
    case VDP_SMSGG:
        {
            int     n = 0;
            char *  nd = &tgfx.Tile_Decoded[0][0];
            u8 *    palette = TileViewer.palette ? &Palette_Refs[16] : &Palette_Refs[0];
            for (y = 0; y < TileViewer.tiles_height; y++)
                for (x = 0; x < TileViewer.tiles_width; x++)
                {
                    if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
                        Decode_Tile (n);
                    if (palette_dirty || tgfx.Tile_Dirty [n])
                    {
                        Draw_One_Tile (palette, nd, (x * 8), (y * 8));
                        tgfx.Tile_Dirty [n] = 0;
                        must_redraw = YES;
                    }
                    if (n == tile_current)
                        tile_current_addr = 0x0000 + (n * 32);
                    n ++;
                    nd += 64;
                }
                break;
        }
    case VDP_TMS:
        {
            const int fg_color = (TileViewer.palette + 1);
            const int bg_color = (fg_color != 1) ? 1 : 15;
            u8 * addr = SG_BACK_TILE;
            // addr = &VRAM[apps.opt.Tiles_Base];
            // addr = VRAM;
            int n = 0;
            for (y = 0; y < TileViewer.tiles_height; y ++)
                for (x = 0; x < TileViewer.tiles_width; x ++)
                {
                    if ((addr - VRAM) > 0x4000)
                        break;
                    Draw_Tile_Mono (apps.gfx.Tiles, addr, (x * 8), (y * 8), fg_color, bg_color);
                    if (n == tile_current)
                        tile_current_addr = 0x0000 + (n * 8);
                    n++;
                    addr += 8;
                }
                must_redraw = YES; // to be replaced later
                break;
        }
    case VDP_NES:
        {
            int     n = 0;
            char *  nd = &tgfx.Tile_Decoded[0][0];
            u8 *    palette = TileViewer.palette ? &PRAM[16] : &PRAM[0];
            for (y = 0; y < TileViewer.tiles_height; y ++)
                for (x = 0; x < TileViewer.tiles_width; x ++)
                {
                    if (tgfx.Tile_Dirty [n] & TILE_DIRTY_DECODE)
                        NES_Decode_Tile (n);
                    if (palette_dirty || tgfx.Tile_Dirty [n])
                    {
                        Draw_One_Tile (palette, nd, (x * 8), (y * 8));
                        tgfx.Tile_Dirty [n] = 0;
                        must_redraw = YES;
                    }
                    if (n == tile_current)
                        tile_current_addr = 0x0000 + (n * 32);
                    n += 1;
                    nd += 64;
                }
                break;
        }
    }
    Palette_Refs_Dirty_Any = NO;

    // First refresh bottom tile info
    if (tile_current_refresh)
    {
        char s[128];
        char addr[8];
        int y = (TileViewer.tiles_height * 8);

        if (tile_current_addr != -1)
            sprintf(addr, "0x%04X", tile_current_addr);
        else
            sprintf(addr, "????");

        sprintf (s, Msg_Get(MSG_TilesViewer_Tile), tile_current, tile_current, addr);
        rectfill (apps.gfx.Tiles, 16, y + 1, 127, y + 11, GUI_COL_FILL);
        blit (apps.gfx.Tiles, apps.gfx.Tiles, (tile_current % 16) * 8, (tile_current / 16) * 8, 4, TileViewer.tiles_height * 8 + 3, 8, 8);
        Font_Print (F_SMALL, apps.gfx.Tiles, s, 16, y + 1, GUI_COL_TEXT_IN_BOX);
        must_redraw = TRUE;
        TileViewer.tile_displayed = tile_current;
    }

    if (must_redraw)
        gui.box[apps.id.Tiles]->must_redraw = YES;
}

// CHANGE THE PALETTE USED IN THE TILE VIEWER ---------------------------------
void    TileViewer_Change_Palette (void)
{
    int   i;
    TileViewer.palette = (TileViewer.palette + 1) % TileViewer.palette_max;
    for (i = 0; i < MAX_TILES; i++)
        tgfx.Tile_Dirty [i] |= TILE_DIRTY_REDRAW;
    TileViewer_Update ();
}

void        TileViewer_Configure_PaletteMax (int palette_max)
{
    int     i;

    if (palette_max == TileViewer.palette_max)
        return;
    TileViewer.palette_max = palette_max;
    if (TileViewer.palette >= palette_max)
        TileViewer.palette %= palette_max;
    for (i = 0; i < MAX_TILES; i++)
        tgfx.Tile_Dirty [i] |= TILE_DIRTY_REDRAW;
    // FIXME: This can potentially crash in NES mode, as NES_VRAM_Banks[] pointers are not set the first time
    TileViewer_Update ();
}

void    TileViewer_SelectedTile_Select (t_widget *w)
{
    if (w->mx != -1 && w->my != -1)
    {
        TileViewer.tile_selected = ((w->my / 8) * 16) + w->mx / 8;
    }
}

// ACTION: ENABLE OR DISABLE TILES VIEWER -------------------------------------
void    TileViewer_Switch (void)
{
    if (apps.active.Tiles ^= 1)
        Msg (MSGT_USER, Msg_Get (MSG_TilesViewer_Enabled));
    else
        Msg (MSGT_USER, Msg_Get (MSG_TilesViewer_Disabled));
    gui_box_show (gui.box[apps.id.Tiles], apps.active.Tiles, TRUE);
    gui_menu_inverse_check (menus_ID.tools, 2);
}

//-----------------------------------------------------------------------------

