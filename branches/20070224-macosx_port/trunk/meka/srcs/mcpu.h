//-----------------------------------------------------------------------------
// MEKA - mcpu.h
// Interface for Neil's Z80 CPU emulator - Headers
//-----------------------------------------------------------------------------
// NOTE: outdated/unused
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// void WrZ80 (UINT32, UINT8);
// UINT8 RdZ80 (UINT32);
// void OutZ80 (UINT16, UINT8);
// UINT16 InZ80 (UINT16);

void    WrZ80   (UINT32, UINT8, struct MemoryWriteByte *);
UINT8   RdZ80   (UINT32, struct MemoryReadByte *);
void    OutZ80  (UINT16, UINT8, struct z80PortWrite *);
UINT16  InZ80   (UINT16, struct z80PortRead *);

//-----------------------------------------------------------------------------

