//-----------------------------------------------------------------------------
// MEKA - mcpu.c
// Interface for Neil's Z80 CPU emulator - Code
//-----------------------------------------------------------------------------
// NOTE: outdated/unused
//-----------------------------------------------------------------------------

struct z80PortRead ReadPorts[] =
{
 { (UINT16) 0x00,    (UINT16) 0xFF,    InZ80   },
 { (UINT16) -1,      (UINT16) -1,      NULL    }
};

struct z80PortWrite WritePorts[] =
{
 { (UINT16) 0x00,    (UINT16) 0xFF,    OutZ80  },
 { (UINT16) -1,      (UINT16) -1,      NULL    }
};

struct MemoryReadByte ReadMem[] =
{
 { (UINT32) 0x0000,  (UINT32) 0xFFFF,  RdZ80   },
 { (UINT32) -1,      (UINT32) -1,      NULL    }
};

struct MemoryWriteByte WriteMem[] =
{
 { (UINT32) 0x0000,  (UINT32) 0xFFFF,  WrZ80   },
 { (UINT32) -1,      (UINT32) -1,      NULL    }
};

//-----------------------------------------------------------------------------

