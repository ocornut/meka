//-----------------------------------------------------------------------------
// MEKA - nes.h
// Nintendo Emulation - Headers
//-----------------------------------------------------------------------------

#include "m6502/m6502.h"
#include "nes_maps.h"
#include "nes_ppu.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// #define NEW_S0HIT

typedef struct  s_nes
 {
   M6502        Regs;
   byte         Joy_Strobe;
   byte         Joy_Status;
   int          Joy_Status_Read;
   byte         Object_RAM [256];
   byte         Object_ADX;
   byte         PPU_Read;
   byte         PPU_Read_Latch;
   byte         CR0, CR1;
   byte         Toggle;
   byte         Scroll [2];
   int          Mirroring;
   #ifdef NEW_S0HIT
      int       Sprite_0_Hit;
   #endif
 }              t_nes;

t_nes           *nes;
byte            *NES_Header;
byte            *NES_Prg;
int              NES_Prg_Cnt;
int              NES_Prg_Mask;
byte            *NES_Chr;
int              NES_Chr_Cnt;
int              NES_Chr_Mask;
t_nes_mapper    *NES_Mapper;
void           (*NES_Mapper_Write)(word, byte);
byte            *NES_VRAM_Banks[16];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    NES_Init                        (void);
void    NES_Palette_Set                 (void);
void    NES_Reset                       (void);

void    Wr6502                          (register word Addr, register byte Value);
byte    Rd6502                          (register word Addr);
byte    Loop6502                        (register M6502 *R);

void    NES_Decode_Tile                 (int n);
void    NES_Decode_Tiles                (void);

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Bits of the CR0 Register
#define NES_CR0_VIRQEN          (0x80)        /* Vertical blank interrupt enable */
#define NES_CR0_PPU_SEL         (0x40)        /* PPU master / slave select */
#define NES_CR0_SPR_SEL         (0x20)        /* Object size select */
#define NES_CR0_BG_PAT          (0x10)        /* Background pattern select */
#define NES_CR0_SPR_PAT         (0x08)        /* Object pattern select */
#define NES_CR0_INC             (0x04)        /* PPU increment factor */
#define NES_CR0_NT_ADDR_MASK    (0x01|0x02)

// Bits of the CR1 Register
#define NES_CR1_MONO            (0x01)        /* Color / monotone */
#define NES_CR1_BG_CLIP         (0x02)        /* Background clipping */
#define NES_CR1_SPR_CLIP        (0x04)        /* Object clipping */
#define NES_CR1_BG_ON           (0x08)        /* Background blanking */
#define NES_CR1_SPR_ON          (0x10)        /* Object blanking */
#define NES_CR1_BG_COLOR_MASK   (0x20|0x40|0x80)

// Mirroring
#define NES_MIRRORING_NO        (0)
#define NES_MIRRORING_H         (1)
#define NES_MIRRORING_V         (2)
#define NES_MIRRORING_4S        (3)

// Scrolling
#define NES_SCROLL_HORIZONTAL   (0)
#define NES_SCROLL_VERTICAL     (1)

// Sprites Attributes
#define NES_SPR_VFLIP           (0x80)
#define NES_SPR_HFLIP           (0x40)
#define NES_SPR_PRIORITY        (0x20)
#define NES_SPR_COLOR_MASK      (0x01|0x02)

// Constants for PPU status byte
#define NES_PPU_VBLANK          (0x80)        /* Vertical blank status flag */
#define NES_PPU_S0HIT           (0x40)        /* Sprite #0 hit */
#define NES_PPU_SOVR            (0x20)        /* Sprite overflow flag */
#define NES_PPU_VRAMEN          (0x10)        /* VRAM access enable / disable */

#define NES_VBlank_ON           (nes->CR0 & NES_CR0_VIRQEN)
#define NES_Display_BG          (nes->CR1 & NES_CR1_BG_ON)
#define NES_Display_SPR         (nes->CR1 & NES_CR1_SPR_ON)
#define NES_Mask_Left_BG        (nes->CR1 & NES_CR1_BG_CLIP)
#define NES_Mask_Left_SPR       (nes->CR1 & NES_CR1_SPR_CLIP)
#define NES_Sprites_8x16        (nes->CR0 & NES_CR0_SPR_SEL)

// Constants for NES header informations
#define NESHEAD_PRG_COUNT(h)    (h [4])
#define NESHEAD_CHR_COUNT(h)    (h [5])
#define NESHEAD_MAPPER(h)       ((h [6] >> 4) & 0x0F)
#define NESHEAD_MIRRORING(h)    ((h [6] >> 0) & 0x01)
#define NESHEAD_SAVERAM(h)      ((h [6] >> 1) & 0x01)
#define NESHEAD_TRAINER(h)      ((h [6] >> 2) & 0x01)
#define NESHEAD_FOURSCREEN(h)   ((h [6] >> 3) & 0x01)

//-----------------------------------------------------------------------------

