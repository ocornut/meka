//-----------------------------------------------------------------------------
// MEKA - lightgun.c
// Light Phaser Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "beam.h"
#include "lightgun.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_light_phaser			LightPhaser;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    LightPhaser_Init(void)
{
    LightPhaser.Enabled = FALSE;
    LightPhaser.LastSync = 0;
    LightPhaser.X [PLAYER_1] = LightPhaser.X [PLAYER_2] = 128;//g_driver->x_res / 2;
    LightPhaser.Y [PLAYER_1] = LightPhaser.Y [PLAYER_2] = 96;//g_driver->y_res / 2;
}

u8		LightPhaser_GetX(void)
{
    const int r = LightPhaser.X [LightPhaser.LastSync] ; // + ((Mask_Left_8) ? 8 : 0);
    return (u8)(16 + (r / 2));
}

// FIXME: This old-fashioned code is incorrect. Light Phaser should be
// emulated using TH bit correctly and horizontal latch.
void    LightPhaser_Sync(int player, byte *v)
{
    *v |= ((player == PLAYER_1) ? 0x40 : 0x80);
    const int dx = LightPhaser.X[player] - Beam_Calc_X();
    const int dy = LightPhaser.Y[player] - tsms.VDP_Line;

    if (dy > -4 && dy < 4)
	{
		// Arbitrary values found after trying differents settings
        if (dx > -48 && dx < 48)
        {
            *v &= (player == PLAYER_1) ? ~0x40 : ~0x80;
            LightPhaser.LastSync = player;
        }
	}
}

// Light Phaser update function
// This is supposed to work with an analog mouse
// Fullscreen mode should be enabled to work properly
void    LightPhaser_Update(int player, int device_x, int device_y)
{
    LightPhaser.X[player] = device_x;
    LightPhaser.Y[player] = device_y;
    // LightGun_X = ((mouse_x + 28 + ((256 - mouse_x) >> 5)) & 0xFF) >> 1;
    // LightGun_Y = mouse_y + 1;
}

//-----------------------------------------------------------------------------

