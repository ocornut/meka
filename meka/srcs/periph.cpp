#include "shared.h"
#include "periph.h"
#include "inputs_t.h"
#include "lightgun.h"
#include "rapidfir.h"

// FIXME: Clean/write and merge lightgun.* here
// FIXME: Clean/write and merge sportpad.* here
// FIXME: Clean/write and merge paddle control stuff here

void Peripherals_Init()
{
	Peripherals_MachineReset();
}

void Peripherals_MachineReset()
{
    LightPhaser_Init();
    RapidFire_Init();

	for (int i = 0; i < 2; i++)
	{
		// Paddle Control
		// FIXME
		{
			t_peripheral_paddle* p = &Inputs.Paddle[i];
			p->x = 0;
		}

		// Sports Pad
		// FIXME
		{
			t_peripheral_sportspad* p = &Inputs.SportsPad[i];
			p->x = p->y = 0;
			p->latch = 0;
		}

		// Graphic Board v2
		{
			t_peripheral_graphic_board_v2* p = &Inputs.GraphicBoardV2[i];
			p->unknown = 0xFE;
			p->buttons = 0;
			p->x = p->y = 0;
			p->read_index = 0;
		}
	}
}

// FIXME: Rewrite properly
void Peripherals_WritePort3F(u8 old_value, u8 new_value)
{
	// Graphic Board v2.0
	if (Inputs.Peripheral[0] == INPUT_GRAPHICBOARD_V2)
	{
		if ((old_value ^ new_value)	& 0x20)
			Inputs.GraphicBoardV2[0].read_index++;
		if (new_value & 0x10)
			Inputs.GraphicBoardV2[0].read_index = 0;
	}
	if (Inputs.Peripheral[1] == INPUT_GRAPHICBOARD_V2)
	{
		if ((old_value ^ new_value)	& 0x80)
			Inputs.GraphicBoardV2[1].read_index++;
		if (new_value & 0x40)
			Inputs.GraphicBoardV2[1].read_index = 0;
	}

	// SportsPad
	// FIXME: This works-ish for known software but it's nonsense, need to rewrite
	if ((old_value & 0xF) != (new_value & 0xF))
		Inputs.SportsPad[0].latch = Inputs.SportsPad[1].latch = 1;
	if (new_value == 0x0D) 
		Inputs.SportsPad[0].latch ^= 1;
	if (new_value == 0x07) 
		Inputs.SportsPad[1].latch ^= 1;
}

// FIXME: This is terrible. Write something a little less 1998 please.
// 1. Rewrite Sports Pad emulation correctly (now that we know how latch workes)
// 2. Tune the function below so that it is playable with a mouse
static void    SportsPad_Update_Axis(s8 *v, int move)
{
    int   vel;
    int   limit = 50; // Technically 63, but in practice it is much less

    vel = *v + (move / 2);

    if (vel > limit) vel = limit;
    else if (vel < -limit) vel = -limit;

    if (vel > 0) vel -= 1;
    if (vel < 0) vel += 1;

    if (vel >= 10) vel -= 10;
    else
        if (vel <= -10) vel += 10;

    *v = vel;
}

void    Peripherals_SportsPad_Update(int player, int device_x_rel, int device_y_rel)
{
    SportsPad_Update_Axis((s8*)&Inputs.SportsPad[player].x, -device_x_rel);
    SportsPad_Update_Axis((s8*)&Inputs.SportsPad[player].y, -device_y_rel);
}

void	Peripherals_GraphicBoardV2_Update(int player, int x, int y, int buttons)
{
	t_peripheral_graphic_board_v2* p = &Inputs.GraphicBoardV2[player];

	y += 32;
	y += 4;
	x -= 4;

	if (x < 0) x = 0;
	if (x > 255) x = 255;
	p->x = (u8)x;

	if (y < 0) y = 0;
	if (y > 255) y = 255;
	p->y = (u8)y;

	p->buttons = buttons;
	if (player == 0)
	{
		if (Inputs_KeyDown(ALLEGRO_KEY_1)) p->buttons ^= (1 << 0);
		if (Inputs_KeyDown(ALLEGRO_KEY_2)) p->buttons ^= (1 << 1);
		if (Inputs_KeyDown(ALLEGRO_KEY_3)) p->buttons ^= (1 << 2);
	}
	else if (player == 1)
	{
		if (Inputs_KeyDown(ALLEGRO_KEY_4)) p->buttons ^= (1 << 0);
		if (Inputs_KeyDown(ALLEGRO_KEY_5)) p->buttons ^= (1 << 1);
		if (Inputs_KeyDown(ALLEGRO_KEY_6)) p->buttons ^= (1 << 2);
	}
	
	//Msg(MSGT_DEBUG, "x=%d y=%d, %x\n", p->x, p->y, p->buttons);
}

