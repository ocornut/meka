//-----------------------------------------------------------------------------
// MEKA - saves.h
// Save States - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MSV_VERSION (0x0D)

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

//-----------------------------------------------------------------------------
// Type
//-----------------------------------------------------------------------------

/*
struct t_mekastate_header
{
    char    magic[4];               // "MEKA-State"
    char    content_creator[32];    // eg: "MEKA 0.69/W"
};
*/

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void  Load_Game (void);              //---------------------------- load a game
void  Save_Game (void);              //---------------------------- save a game
void  Load_Game_Misc (void);         // set some variables after loading a game

//-----------------------------------------------------------------------------

int   Load_Game_MSV (FILE *f);       //------------- save a game in Meka format
int   Save_Game_MSV (FILE *f);       //------------- load a game in Meka format
int   Load_Game_STA (FILE *f);       //------------ load a game in BrSMS format
int   Load_Game_MSD (FILE *f);       //---------- load a game in Massage format

void  Save_Set_Slot (int n);         //---- set variables for current save slot
void  Save_Get_Filename (char *str); //..

//-----------------------------------------------------------------------------
