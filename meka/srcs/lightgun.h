//-----------------------------------------------------------------------------
// MEKA - lightgun.h
// Light Phaser Emulation - Headers
//-----------------------------------------------------------------------------

// Hard-coded Light-phaser function to reduce mismatch between user cursor
enum t_lightphaser_emu_func
{
    LIGHTPHASER_EMU_FUNC_DEFAULT = 0,
    LIGHTPHASER_EMU_FUNC_MISSILE_DEFENSE_3D = 1,
    LIGHTPHASER_EMU_FUNC_3DGUNNER = 2
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    LightPhaser_Init();
void    LightPhaser_Sync(int player, byte *);
void    LightPhaser_Update(int player, int device_x, int device_y);
u8      LightPhaser_GetX();

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_light_phaser
{
    bool    Enabled;        // Set if has least one Light Phaser is now enabled
    int     LastSync;
    u8      X[2];
    u8      Y[2];
    t_lightphaser_emu_func  EmuFunc;
};

extern t_light_phaser  LightPhaser;

//-----------------------------------------------------------------------------

