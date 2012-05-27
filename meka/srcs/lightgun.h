//-----------------------------------------------------------------------------
// MEKA - lightgun.h
// Light Phaser Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    LightGun_Sync (int player, byte *);
void    LightGun_Update (int player, int device_x, int device_y);

byte    LightGun_X (void);
void    LightGun_Init (void);
void    LightGun_Mouse_Range (int);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
 int    Enabled; // Set if has least one Light Phaser is now enabled
 int    LastSync;
 byte   X [2];
 byte   Y [2];
} t_light_phaser;

t_light_phaser  LightGun; // FIXME: rename this symbol

//-----------------------------------------------------------------------------

