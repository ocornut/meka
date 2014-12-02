//-----------------------------------------------------------------------------
// MEKA - fdc765.h
// FDC765 (Floppy Disk Drive) Emulator - Headers
//-----------------------------------------------------------------------------
// Originally from Ulrich Cordes
// Modifications for SF-7000 by Marc Le Douarain, Omar Cornut
//-----------------------------------------------------------------------------

// Original file header:

/***************************************/
/**                                   **/
/** AMSTRAD/Schneider CPC-Emulator    **/
/** for Linux and X11                 **/
/**                                   **/
/** (c) 1999,                         **/
/** Ulrich Cordes                     **/
/** Bergstrasse 8                     **/
/** 34292 AHNATAL / Germany           **/
/**                                   **/
/** email:  ulrich.cordes@gmx.de      **/
/** WWW:    http://www.amstrad-cpc.de **/
/**                                   **/
/***************************************/

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define FDC765_MAXDRIVES   (2)    // Number of emulated drives
#define FDC765_SPT         (16)   // (sectors per track) (9, max. 18 possible)
#define FDC765_BPS         (2)    // (bytes per sector) (2 for 0x200 Bytes)

//-----------------------------
// Disk Header (256 bytes)
//-----------------------------
struct FDC765_DiskHeader
{
  byte   tag[0x30];     /* 00-21  MV - CPC ...                                      */
                        /* 22-2F  unused (0)                                        */
  byte   nbof_tracks;   /* 30     number of tracks (40)                             */
  byte   nbof_heads;    /* 31     number of heads (1) 2 not yet supported by cpcemu */
  short  tracksize;	/*        short must be 16bit integer                       */
                        /* 32-33  tracksize (including 0x100 bytes header)          */
                        /*        9 sectors * 0x200 bytes each + header = 0x1300    */
  byte   unused[0xcc];  /* 34-FF  unused (0)                                        */
};

struct FDC765_Track
{
  byte                  DiscData [4096];        // 16*256 bytes Data
};

struct FDC765_Disk
{
  byte                  HasDisk;                // TRUE if a disk is inserted
  FDC765_DiskHeader     Header;                 // then the structure is valid
  FDC765_Track         *Tracks;
  int                   TracksSize;
};

// Initialize/Close funtions --------------------------------------------------
void    FDC765_Init             (void);         // Initialization
void    FDC765_Close            (void);         // Final Closure
void    FDC765_Reset            (void);         // Reset
//-----------------------------------------------------------------------------

// Disk Management functions --------------------------------------------------
void    FDC765_Disk_Insert      (int DrvNum, void *Data, int DataSize);
void    FDC765_Disk_Remove      (int DrvNum);
void    FDC765_Disk_Write_Get   (int DrvNum, void **Data, int *DataSize);
//-----------------------------------------------------------------------------

// Emulation functions --------------------------------------------------------
void    FDC765_Data_Write       (register byte Value);
byte    FDC765_Data_Read        (void);
byte    FDC765_Status_Read      (void);
//-----------------------------------------------------------------------------

// Added by Marc Le Douarain for SF-7000 emulation ----------------------------
// (Pin 17 of the FDC is connected to SF-7000 [PA2] input port)
extern u8	FDC765_Cmd_For_SF7000;

//-----------------------------------------------------------------------------

