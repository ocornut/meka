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

extern RGB      Palette_Emulation[PALETTE_EMU_GAME_SIZE];
extern int      Palette_EmulationToHost[PALETTE_EMU_GAME_SIZE];
extern int      Palette_EmulationToHost16[PALETTE_EMU_GAME_SIZE];
extern int      Palette_EmulationFlags[PALETTE_EMU_GAME_SIZE];
extern bool     Palette_EmulationDirtyAny;

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
void    Palette_Emulation_SetColor(int idx, RGB color);

void    Palette_Compute_RGB_SMS (RGB *color, int i);
void    Palette_Compute_RGB_GG  (RGB *color, int i);

//-----------------------------------------------------------------------------

