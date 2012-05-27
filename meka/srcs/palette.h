//-----------------------------------------------------------------------------
// MEKA - palette.h
// Palette management - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define PALETTE_EMU_GAME_SIZE   (32)    // Max of all emulated system palette size

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

extern ALLEGRO_COLOR Palette_Emulation[PALETTE_EMU_GAME_SIZE];
extern u32			 Palette_EmulationToHostGui[PALETTE_EMU_GAME_SIZE];
extern u16			 Palette_EmulationToHostGame[PALETTE_EMU_GAME_SIZE];
extern int			 Palette_EmulationFlags[PALETTE_EMU_GAME_SIZE];
extern bool			 Palette_EmulationDirtyAny;

typedef enum
{
    PALETTE_EMULATION_FLAGS_DIRTY   = 0x0001,
} t_palette_emulation_flags;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Palette_Init(void);
void    Palette_Close(void);
void    Palette_UpdateAfterRedraw();

void    Palette_Emulation_Reset();
void    Palette_Emulation_Reload();
void    Palette_Emulation_SetColor(int idx, ALLEGRO_COLOR color);

void    Palette_Compute_RGB_SMS (ALLEGRO_COLOR *color, int i);
void    Palette_Compute_RGB_GG  (ALLEGRO_COLOR *color, int i);

u32		Palette_MakeHostColor(int format, int r, int g, int b);
u32		Palette_MakeHostColor(int format, ALLEGRO_COLOR color);

//-----------------------------------------------------------------------------

