//-----------------------------------------------------------------------------
// MEKA - nes_ppu.h
// Nintendo PPU Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    NES_PPU_Set_Mirroring           (int Mirroring);
void    NES_PPU_Map                     (int page_start, int page_num, byte *data_start);

void    NES_PPU_Write                   (word Addr, byte Value);
byte    NES_PPU_Read                    (word Addr);

void    NES_PPU_Refresh                 (int Line);
void    NES_PPU_Refresh_BgFg            (int Line);
void    NES_PPU_Refresh_Sprites         (int Line);
void    NES_PPU_Refresh_Sprites_S0Hit   (int Line);

//-----------------------------------------------------------------------------

