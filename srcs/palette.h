//-----------------------------------------------------------------------------
// MEKA - palette.h
// Palette management - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define PALETTE_EMU_HOST_SIZE   (64)      // In Palette_Current, Colors 0 to 63 are usable for emulation
#define PALETTE_EMU_GAME_SIZE   (32)      // Max of all emulated system palette size

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

#ifdef MEKA_DEPTH_8

typedef struct
{
 int    idx;            // Index of the color in Palette_Current
 int    refs;           // Number of times the color is referenced
 int    dirty;          // 1 if has to be updated on video card
 int    lock;           // 1 if it is locked
}       t_color_infos;

// Palette is used as following:
//   0 -  63  Emulation colors
//  64 -  83  Black, White & common colors
//  84 - 102  Keyboard
// 103 - 127  GUI & gradients colors
// 128 - 255  Background picture or machine

PALETTE Palette_Current;                            // Current host palette
int     Palette_Dirty_All;                          // Dirty flag

int     Palette_Emu_Cycle_Start;                    // Cyclic start for researching an empty cell
int     Palette_Emu_Dirty_Any;                      // Dirty flag
t_color_infos Palette_Emu_Infos [PALETTE_EMU_HOST_SIZE];  // Infos per color

byte    Palette_Refs [PALETTE_EMU_GAME_SIZE];       // Machine palette
int     Palette_Refs_Dirty [PALETTE_EMU_GAME_SIZE]; // Machine palette dirty flag
int     Palette_Refs_Dirty_Any;                     // Set if any of the above is set

extern int Palette_Debug; // Set to enable some tracing

#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Palette_Init            (void);
void    Palette_Close           (void);

void    Palette_Sync            (void);
void    Palette_Sync_All        (void);

void    Palette_Emu_Unlock_All  (void);
void    Palette_Emu_Reset       (void);
void    Palette_Emu_Reload      (void);

void    Palette_SetColor                 (int n_hard, RGB color);
void    Palette_SetColor_Range           (int n_start, int n_end, RGB *colors);

void    Palette_SetColor_Reference       (int n,        RGB color);
void    Palette_SetColor_Reference_Force (int n_machine,int n_emu);
void    Palette_SetColor_Emulation       (int n_emu,    RGB color);

void    Palette_Compute_RGB_SMS (RGB *color, int i);
void    Palette_Compute_RGB_GG  (RGB *color, int i);

//-----------------------------------------------------------------------------

