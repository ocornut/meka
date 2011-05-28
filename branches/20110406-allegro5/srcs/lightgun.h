//-----------------------------------------------------------------------------
// MEKA - lightgun.h
// Light Phaser Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    LightPhaser_Init(void);
void    LightPhaser_Sync(int player, byte *);
void    LightPhaser_Update(int player, int device_x, int device_y);
u8		LightPhaser_GetX(void);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_light_phaser
{
	bool	Enabled;		// Set if has least one Light Phaser is now enabled
	int		LastSync;
	u8		X[2];
	u8		Y[2];
};

extern t_light_phaser  LightPhaser;

//-----------------------------------------------------------------------------

