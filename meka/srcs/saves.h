//-----------------------------------------------------------------------------
// MEKA - saves.h
// Save States - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MEKA_SAVESTATE_VERSION (0x0E)

// 0x06 -> 0x07
// - sms.SRAM_Pages is multiplied by two
//   Now a page is 8 kb instead of 16 kb (because of NES)

// 0x07 -> 0x08
// - saved PSG data

// 0x08 -> 0x09
// - rewrote 93c46 emulation
//   93c46 data of version <= 0x07 is not loaded

// 0x09 -> 0x0A
// - upgraded Z80 core, IFF_2 flag in IFF is now 0x08 instead of 0x40
//   old save states will copy the bit

// 0x0A -> 0x0B
// - updated PSG implementation and changed saved data (other than registers)

// 0x0B -> 0x0C
// - added ROM CRC32 (not used now, but will be useful for tools in the future)

// 0x0C -> 0x0D
// - added scanline counter. 
//   Having it fixed was a terrible mistake that I did not notice until I looked
//   at the code recently. It screw up debugging and can break compatibility when
//   my CPU emulation loop changes.

// 0x0D -> 0x0E
// - added 4th mapper register *only if needed* (by some Korean mappers)
//   so that import code in other emulator won't break for most games

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void  SaveState_Load();
void  SaveState_Save();
void  SaveState_SetSlot(int n);
void  SaveState_SetPrevSlot();
void  SaveState_SetNextSlot();
void  Load_Game_Fixup();

//-----------------------------------------------------------------------------

int   Load_Game_MSV(FILE *f);       // Save game in MEKA format
int   Save_Game_MSV(FILE *f);       // Load game in MEKA format
int   Load_Game_MSD(FILE *f);       // Load game in Massage format

void  Save_Set_Slot(int n);
void  Save_Get_Filename(char *str);

//-----------------------------------------------------------------------------
