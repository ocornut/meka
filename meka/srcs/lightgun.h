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
void    LightPhaser_SetupMouseRange(bool left_most_column_masked);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
	bool	Enabled;		// Set if has least one Light Phaser is now enabled
	int		LastSync;
	u8		X[2];
	u8		Y[2];
} t_light_phaser;

t_light_phaser  LightPhaser;

//-----------------------------------------------------------------------------

