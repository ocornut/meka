//-----------------------------------------------------------------------------
// MEKA - skin_fx.c
// Special effects - Code
// was: Bloodlust theme                                          B.elieve
//-----------------------------------------------------------------------------
// FIXME: Super old code, terrible. Don't look! Close your eyes! Leave now!
//-----------------------------------------------------------------------------

#include "shared.h"
#include "skin_fx.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

#define MAX_BLOOD_DROP          (500)
#define MAX_HEARTS              (16)

struct t_skinfx_particle
{
	int   v;			// 0: not active, 1->4: active
	float x, y;
	float ix, iy;		// Initial position
	float vx, vy;		// Velocity
	float sin_amp;
	float sin_phase;
	float sin_speed;
	ALLEGRO_COLOR save;	// Backup of pixel
};

static ALLEGRO_BITMAP *		hearts_save[MAX_HEARTS];
static t_skinfx_particle	g_skinfx_particles[MAX_BLOOD_DROP];
static int                  g_skinfx_particles_next_spawn;

//-----------------------------------------------------------------------------
// Forward Declaration
//-----------------------------------------------------------------------------

static void gui_applet_blood_create (int v, int x, int y);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------


void SkinFx_Init()
{
    g_skinfx_particles_next_spawn = 0;
    for (int i = 0; i != MAX_BLOOD_DROP; i ++)
        gui_applet_blood_create(0, 0, 0);
    for (int i = 0; i != MAX_HEARTS; i ++)
        hearts_save [i] = NULL;
}

void SkinFx_CreateVideoBuffers()
{
	const int hw = al_get_bitmap_width(Graphics.Misc.Heart1);
	const int hh = al_get_bitmap_height(Graphics.Misc.Heart1);
    for (int i = 0; i != MAX_HEARTS; i ++)
    {
		if (hearts_save[i] != NULL)
			al_destroy_bitmap(hearts_save[i]);
        hearts_save [i] = al_create_bitmap(hw, hh);
    }
}

static void	SkinFx_UpdateBlood(void)
{
	int i;
	
	// This is the colors originally used when MEKA was working in palette mode
	// Nowadays, I guess the logic should be changed to take a single base color and create altered variations of it
	ALLEGRO_COLOR blood_colors[4];
	blood_colors[0] = COLOR_SKIN_WINDOW_BACKGROUND;
	blood_colors[1] = COLOR_SKIN_WINDOW_BORDER;
	blood_colors[2] = COLOR_SKIN_MENU_SELECTION;
	blood_colors[3] = COLOR_SKIN_MENU_BACKGROUND;

	// Create new drops around cursor
	for (i = 0; i < 4; i ++)
		gui_applet_blood_create(RandomInt(4), gui.mouse.x - 2 + RandomInt(5), gui.mouse.y - 2 + RandomInt(5));
	for (i = 0; i < 2; i ++)
		gui_applet_blood_create(RandomInt(4), gui.mouse.x - 4 + RandomInt(9), gui.mouse.y - 4 + RandomInt(9));
	gui_applet_blood_create(RandomInt(4), gui.mouse.x - 5 + RandomInt(11), gui.mouse.y - 5 + RandomInt(11));

	// Create new drops below currently focused window
	t_gui_box* b = gui.boxes_z_ordered[0];
	if (b && (b->flags & GUI_BOX_FLAGS_ACTIVE))
    {
		for (i = 0; i < 4; i ++)
		{
			gui_applet_blood_create(RandomInt(4), 
				b->frame.pos.x - 2 + RandomInt(b->frame.size.x + 4), b->frame.pos.y + b->frame.size.y + 2);
		}
    }

	// Update drops
	for (i = 0; i < MAX_BLOOD_DROP; i ++)
	{
		t_skinfx_particle* p = &g_skinfx_particles[i];
		p->x += p->vx;
		p->y += p->vy;
		if (p->x < 0 || p->x >= gui.info.screen.x || p->y < 0 || p->y >= gui.info.screen.y)
			p->v = 0;
	}

	al_lock_bitmap(gui_buffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
	al_set_target_bitmap(gui_buffer);

	// Save old colors
	for (i = 0; i < MAX_BLOOD_DROP; i ++)
	{
		t_skinfx_particle* p = &g_skinfx_particles[i];
		if (p->v)
			p->save = al_get_pixel(gui_buffer, p->x, p->y);
	}

	// Draw blood drops
	for (i = 0; i < MAX_BLOOD_DROP; i ++)
	{
		t_skinfx_particle* p = &g_skinfx_particles[i];
		if (p->v)
			al_put_pixel(p->x, p->y, blood_colors[p->v]);
	}

	al_unlock_bitmap(gui_buffer);
}

static void SkinFx_UpdateHearts()
{
	// Create a new heart
	if (RandomInt(60) == 0)
		gui_applet_blood_create (RandomInt(2), RandomInt(gui.info.screen.x), gui.info.screen.y - gui.info.bars_height);

	// Floating hearts
	for (int i = 0; i < MAX_HEARTS; i ++)
	{
		t_skinfx_particle* p = &g_skinfx_particles[i];
		p->y += p->vy;
		p->sin_phase += p->sin_speed;
		p->x = p->ix + p->sin_amp * sinf(p->sin_phase);
	}

	// Save old graphics
	const int w = al_get_bitmap_width(Graphics.Misc.Heart1);
	const int h = al_get_bitmap_height(Graphics.Misc.Heart1);
	for (int i = 0; i < MAX_HEARTS; i ++)
	{
		t_skinfx_particle* p = &g_skinfx_particles[i];
		al_set_target_bitmap(hearts_save[i]);
		al_draw_bitmap_region(gui_buffer, p->x, p->y, w, h, 0, 0, 0x0000);
	}

	// Draw hearts
	al_set_target_bitmap(gui_buffer);
	for (int i = 0; i < MAX_HEARTS; i ++)
	{
		t_skinfx_particle* p = &g_skinfx_particles[i];
		al_draw_bitmap(p->v ? Graphics.Misc.Heart1 : Graphics.Misc.Heart2, p->x, p->y, 0);
	}
}

void    special_effects_update_after (void)
{
return;
	t_skin *skin = Skins_GetCurrentSkin();
	switch (skin->effect)
	{
	case SKIN_EFFECT_BLOOD:
		SkinFx_UpdateBlood();
		break;
	case SKIN_EFFECT_HEARTS:
		SkinFx_UpdateHearts();
		break;
	}
}

void    special_effects_update_before (void)
{
return;
	t_skin *skin = Skins_GetCurrentSkin();

	al_lock_bitmap(gui_buffer, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);

	al_set_target_bitmap(gui_buffer);
	switch (skin->effect)
	{
		// BLOOD DROPS -------------------------------------------------------------
	case SKIN_EFFECT_BLOOD:
		for (int i = 0; i < MAX_BLOOD_DROP; i ++)
		{
			t_skinfx_particle* p = &g_skinfx_particles[i];
			if (p->v && p->save.a != 0)
				al_put_pixel(p->x, p->y, p->save);
		}
		break;
	case SKIN_EFFECT_HEARTS:
		// Save old graphics --------------------------------------------------
		const int w = al_get_bitmap_width(Graphics.Misc.Heart1);
		const int h = al_get_bitmap_height(Graphics.Misc.Heart1);
		for (int i = 0; i < MAX_HEARTS; i ++)
		{
			t_skinfx_particle* p = &g_skinfx_particles[i];
			al_draw_bitmap_region(hearts_save[i], 0, 0, w, h, p->x, p->y, 0x0000);
		}
		break;
	}
	al_unlock_bitmap(gui_buffer);
}

static void    gui_applet_blood_create (int v, int x, int y)
{
	int   max = 1;

	t_skin *skin = Skins_GetCurrentSkin();
	t_skinfx_particle* p = &g_skinfx_particles[g_skinfx_particles_next_spawn];
	switch (skin->effect)
	{
	case SKIN_EFFECT_BLOOD:
		max = MAX_BLOOD_DROP;
		p->v = v;
		p->x = p->ix = x;
		p->y = p->iy = y;
		p->vy = 0.0f;
		p->vy = RandomFloat(0.30f, 0.70f);
		p->sin_amp = 0.0f;
		p->sin_phase = 0.0f;
		p->sin_speed = RandomFloat(-0.02f, +0.02f);
		p->save = al_map_rgba(0,0,0,0);
		break;
	case SKIN_EFFECT_HEARTS:
		max = MAX_HEARTS;
		p->v = v;
		p->x = p->ix = x;
		p->y = p->iy = y;
		p->vx = 0.0f;
		p->vy = RandomFloat(-1.2f, -0.8f);
		p->sin_amp = RandomFloat(20.0f, 70.0f);
		p->sin_phase = RandomFloat(0.0f, MATH_PI*2.0f);
		p->sin_speed = RandomFloat(-0.02f, +0.02f);
		p->save = al_map_rgba(0,0,0,0);
		break;
	}

	g_skinfx_particles_next_spawn = (g_skinfx_particles_next_spawn + 1) % max;
}

//-----------------------------------------------------------------------------
