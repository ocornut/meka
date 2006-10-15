//-----------------------------------------------------------------------------
// MEKA - lightgun.c
// Light Phaser Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

void    LightGun_Init (void)
{
    LightGun.Enabled = NO;
    LightGun.LastSync = 0;
    LightGun.X [PLAYER_1] = LightGun.X [PLAYER_2] = 128;//cur_drv->x_res / 2;
    LightGun.Y [PLAYER_1] = LightGun.Y [PLAYER_2] = 96;//cur_drv->y_res / 2;
}

// Set mouse range - called in fullscreen mode only
void    LightGun_Mouse_Range (int ML8)
{
    int   m = (ML8 ? 8 : 0);
    // Msg(MSGT_DEBUG, "ReRange %02X", ML8);
    set_mouse_range (m, 0, cur_drv->x_res - 1, cur_drv->y_res - 1);
}

byte    LightGun_X (void)
{
    int   r;
    r = LightGun.X [LightGun.LastSync] ; // + ((Mask_Left_8) ? 8 : 0);
    return (16 + (r / 2));
}

// FIXME: This old-fashioned code is incorrect. Light Phaser should be
// emulated using TH bit correctly and horizontal latch.
void    LightGun_Sync (int player, byte *v)
{
    int   dx, dy;

    *v |= ((player == PLAYER_1) ? 0x40 : 0x80);
    dx = LightGun.X [player] - Beam_Calc_X ();
    dy = LightGun.Y [player] - tsms.VDP_Line;

    if (dy > -4 && dy < 4)                 // Arbitrary values found after
        if (dx > -48 && dx < 48)            // trying differents settings
        {
            *v &= (player == PLAYER_1) ? ~0x40 : ~0x80;
            LightGun.LastSync = player;
        }
}

// Light Phaser update function
// This is supposed to work with an analog mouse
// Fullscreen mode should be enabled to work properly
void    LightGun_Update (int player, int device_x, int device_y)
{
    LightGun.X [player] = device_x;
    LightGun.Y [player] = device_y;
    // LightGun_X = ((mouse_x + 28 + ((256 - mouse_x) >> 5)) & 0xFF) >> 1;
    // LightGun_Y = mouse_y + 1;
}

//-----------------------------------------------------------------------------

