//-----------------------------------------------------------------------------
// MEKA - fdc765.c
// FDC765 (Floppy Disk Drive) Emulator - Code
//-----------------------------------------------------------------------------
// Ulrich Cordes
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

// Modified for SEGA SF-7000
// by Marc 'Mavati' Le Douarain
// in November 2000

// Generally cleaned/modified/translated/fixed for MEKA
// by Omar 'Bock' Cornut
// in November 2000

// Note from the original author:
// If you want to make changes, please do not(!) use TABs !!!!!
// (obviously abused by Omar, anyway it was a big source cleaning that I made)

#include "shared.h"
#include "fdc765.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

FDC765_Disk dsk[FDC765_MAXDRIVES];
u8			FDC765_Cmd_For_SF7000;

//-----------------------------------------------------------------------------

byte    FloppyMotor;                    // True= Motor ON, False= Motor OFF
byte    FDCCurrDrv;                     // Current drive
byte    FDCWrProtect[FDC765_MAXDRIVES]; // Write protection, not used so far
byte    FDCCurrTrack[FDC765_MAXDRIVES]; // Current track of each drive
byte    FDCCurrSide[FDC765_MAXDRIVES];  // Current side of each drive
byte    ExecCmdPhase;                   // TRUE=Kommandophase findet gerade statt
byte    ResultPhase;                    // TRUE=Result-Phase findet gerade statt
byte    StatusRegister;                 // Status Register
word    StatusCounter;
byte    st0, st1, st2, st3;

byte    FDCCommand [9];            /* Feld für Kommandos  */
byte    FDCResult [7];             /* Feld für Ergebnisse */

word    FDCPointer;                /* Zeiger auf die akt. Variable im Komando-Feld (beim Übertragen) */
word    FDCCmdPointer;             /* Zeiger auf das aktuell zu übertragende Zeichen (READ/WRITE)    */
word    FDCResPointer;             /* Zeiger auf das akt. Result                                     */
word    FDCResCounter;             /* Anzahl der Results, die Zurückgegeben werden                   */
unsigned long FDCDataPointer;      /* Sektor-Zeiger (Zähler) */
unsigned long FDCDataLength;       /* Anzahl der zu lesenden Daten */
word    TrackIndex;                /* Index auf dsk[].Tracks[....] */
unsigned long TrackDataStart;      /* Startposition der Daten des akt. Sektors im Track */

const byte bytes_in_cmd[32] =
{
    1,  /*  0 = none                                */
	1,  /*  1 = none                                */
	9,  /*  2 = READ TRACK, not implemented         */
	3,  /*  3 = SPECIFY                             */
	2,  /*  4 = SENSE DRIVE STATUS                  */
	9,  /*  5 = WRITE DATA                          */
	9,  /*  6 = READ DATA                           */
	2,  /*  7 = RECALIBRATE                         */
	1,  /*  8 = SENSE INTERRUPT STATUS              */
	9,  /*  9 = WRITE DELETED DATA, not implemented */
	2,  /* 10 = READ SECTOR ID                      */
	1,  /* 11 = none                                */
	9,  /* 12 = READ DELETED DATA, not implemented  */
	6,  /* 13 = FORMAT A TRACK                      */
	1,  /* 14 = none                                */
	3,  /* 15 = SEEK                                */
	1,  /* 16 = none                                */
	9,  /* 17 = SCAN EQUAL                          */
	1,  /* 18 = none                                */
	1,  /* 19 = none                                */
	1,  /* 20 = none                                */
	1,  /* 21 = none                                */
	1,  /* 22 = none                                */
	1,  /* 23 = none                                */
	1,  /* 24 = none                                */
	9,  /* 25 = SCAN LOW OR EQUAL                   */
	1,  /* 26 = none                                */
	1,  /* 27 = none                                */
	1,  /* 28 = none                                */
	1,  /* 29 = none                                */
	9,  /* 30 = SCAN HIGH OR EQUAL                  */
	1   /* 31 = none                                */
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
void    GetRes7 (void)                      /* Return 7 result bytes */
{
  FDCResult[0] = st0;
  FDCResult[1] = st1;
  FDCResult[2] = st2;
  FDCResult[3] = FDCCommand[2];         /* C, H, R, N */
  FDCResult[4] = FDCCommand[3];
  FDCResult[5] = FDCCommand[4];
  FDCResult[6] = FDCCommand[5];
  StatusRegister = 0xD0;                /* Ready to return results */
  StatusCounter = 100;
  FDCResPointer = 0;
  FDCResCounter = 7;
  st0 = st1 = st2 = 0;
  ExecCmdPhase = FALSE;
  ResultPhase = TRUE;
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
void    FDCExecWriteCommand (register byte Value)
{
  switch (FDCCommand [0])
    {
    case 2:             /* Read track */
      FDCCurrDrv = FDCCommand[1] & 3;
      FDCCurrSide[FDCCurrDrv] = (FDCCommand[1] >> 2) & 1;
      FDCCurrTrack[FDCCurrDrv] = FDCCommand[2];
      if (dsk[FDCCurrDrv].HasDisk == FALSE)
        {
        st0 = FDCCurrDrv | 0xD8;  /* Equipment check, Not ready */
        GetRes7();
        }
      else
        {
        FDCCurrSide[FDCCurrDrv] = (FDCCommand[1] >> 2) & 1;
        FDCCurrTrack[FDCCurrDrv] = FDCCommand[2];
        ExecCmdPhase = TRUE;
        TrackIndex = FDCCurrTrack[FDCCurrDrv] * dsk[FDCCurrDrv].Header.nbof_heads + FDCCurrSide[FDCCurrDrv];
        TrackDataStart = ((FDCCommand[4] & 0x0F)-1) << 9;
//        FDCDataLength = (dsk[FDCCurrDrv].Tracks[TrackIndex].BPS * dsk[FDCCurrDrv].Tracks[TrackIndex].SPT) << 9;
        FDCDataLength = (FDC765_BPS * FDC765_SPT) << 9;
        FDCDataPointer = 0;
        StatusCounter = 100;
        StatusRegister = 0xF0;     /* RQM=1, DIO=FDC->CPU, EXM=1, CB=1 */
        }
      break;

    case 3:             /* Specify */
      break;

    case 4:             /* Sense drive status */
      FDCCurrDrv = FDCCommand[1] & 3;
      st3 = FDCCommand[1] & 7;
      if (FDCWrProtect[FDCCurrDrv]) st3 |= 0x40;
      if (dsk[FDCCurrDrv].HasDisk) st3 |= 0x20;
      if (FDCCurrTrack[FDCCurrDrv] == 0) st3 |= 0x10;
      if (st3 & 4) st3 |= 8; /* Two side drive */
      FDCResCounter = 1;       /* Ein Result-Byte, das zurück gegeben wird */
      FDCResPointer = 0;
      FDCResult[0] = st3;
      ExecCmdPhase = FALSE;
      ResultPhase = TRUE;
      StatusCounter = 100;
      StatusRegister = 0xD0;		    /* Ready to return results */
      break;

    case 5:              /* Write data */
      if (!ExecCmdPhase)
        {
        FDCCurrDrv = FDCCommand[1] & 3;
        FDCCurrSide[FDCCurrDrv] = (FDCCommand[1] >> 2) & 1;
        FDCCurrTrack[FDCCurrDrv] = FDCCommand[2];
        ExecCmdPhase = TRUE;
        if (dsk[FDCCurrDrv].HasDisk == FALSE)
          {
          st0 = FDCCurrDrv | 0xD8;  /* Equipment check, Not ready */
          GetRes7();
          }
        else
          {
          TrackIndex = FDCCurrTrack[FDCCurrDrv] * dsk[FDCCurrDrv].Header.nbof_heads + FDCCurrSide[FDCCurrDrv];
          TrackDataStart = ((FDCCommand[4] & 0x0F)-1) << 9;
          FDCDataLength = 512 + ((FDCCommand[4] - FDCCommand[6]) << 9);
          FDCDataPointer = 0;
          StatusCounter = 100;
          StatusRegister = 0xB0;     /* RQM=1, DIO=CPU->FDC, EXM=1, CB=1 */
          }
        }
      else
        {
        dsk[FDCCurrDrv].Tracks[TrackIndex].DiscData[TrackDataStart + FDCDataPointer] = Value;
        FDCDataPointer ++;
        if (FDCDataPointer == FDCDataLength)
          {
          st0 = FDCCommand[1] & 7;
          GetRes7();
          }
        }
      break;

    case 6:                      /* Read data */
      FDCCurrDrv = FDCCommand[1] & 3;
      FDCCurrSide[FDCCurrDrv] = (FDCCommand[1] >> 2) & 1;
      FDCCurrTrack[FDCCurrDrv] = FDCCommand[2];
      if (dsk[FDCCurrDrv].HasDisk == FALSE)
        {
        st0 = FDCCurrDrv | 0xD8;  /* Equipment check, Not ready */
        GetRes7();
        }
      else
        {
        ExecCmdPhase = TRUE;
        TrackIndex = FDCCurrTrack[FDCCurrDrv] * dsk[FDCCurrDrv].Header.nbof_heads + FDCCurrSide[FDCCurrDrv];
//        TrackDataStart = ((FDCCommand[4] & 0x0F)-1) << 9;
//        FDCDataLength = 512 + (((FDCCommand[4] & 0xF) - (FDCCommand[6] & 0xF))<<9);
        TrackDataStart = ((FDCCommand[4] & 0x1F)-1) << 8;
        FDCDataLength = TrackDataStart+256;// + (((FDCCommand[4] & 0xF) - (FDCCommand[6] & 0xF))<<8);
        FDCDataPointer = 0;
        StatusCounter = 100;
        StatusRegister = 0xF0; /* RQM=1, DIO=FDC->CPU, EXM=1, CB=1 */
        }
      break;

   case 7:                     /* Recalibrate (Track 0 Lookup) */
      st0 = st1 = st2 = 0;
      FDCCurrDrv = FDCCommand[1] & 3;
      st0 = FDCCommand[1] & 7;
      if (dsk[FDCCurrDrv].HasDisk == FALSE)
        {
        st0 |= 0xD8;  /* Equipment check, Not ready */
        }
      else
        {
        if (FDCCurrTrack[FDCCurrDrv] > 77)
          {
          FDCCurrTrack[FDCCurrDrv] -= 77;
          st0 |= 0x30;
          }
        else
          {
          FDCCurrTrack[FDCCurrDrv] = 0;
          st0 |= 0x20;
          }
        }
      StatusCounter = 100;
      StatusRegister = 0x80 | (1 << (FDCCommand[1] & 3)); /* RQM=1, DIO=CPU->FDC, EXM = 0 */
      ExecCmdPhase = FALSE;
      break;

    case 8:                    /* Sense Interrupt */
      StatusRegister = 0xD0;   /* RQM=1, DIO=FDC->CPU, EXM = 0, CB=1, DB0-DB3 = 0 */
      FDCResCounter = 2;       /* Two Result-Bytes, die zurück gegeben werden */
      FDCResPointer = 0;
      //st0 = FDCCurrDrv | (FDCCurrSide[FDCCurrDrv]<<2);
      if (dsk[FDCCurrDrv].HasDisk == FALSE)
         st0 |= 0x08;                  /* Drive not ready */
      if (!(st0 & 0x38)) st0 |= 0x80;  /* If no interrupt is available */
/* MLD */
// Needed else SF-7000 IPL says 'Cannot read this disk'
st0 &= 0x3F;
      FDCResult[0] = st0; st0 = 0x00;
      FDCResult[1] = FDCCurrTrack [FDCCurrDrv];
      ExecCmdPhase = FALSE;
      ResultPhase = TRUE;
      StatusCounter = 100;
      break;

    case 10:                   /* ID des nächsten Sektors lesen */
      FDCCurrDrv = FDCCommand[1] & 3;
      FDCCurrSide[FDCCurrDrv] = (FDCCommand[1] >> 2) & 1;
      if (dsk[FDCCurrDrv].HasDisk == FALSE)
        {
        st0 = FDCCurrDrv | 0xD8;  /* Equipment check, Not ready */
        GetRes7();
        }
      else
        {
        TrackIndex = FDCCurrTrack[FDCCurrDrv] * dsk[FDCCurrDrv].Header.nbof_heads + FDCCurrSide[FDCCurrDrv];
        st0 = FDCCommand[1] & 7;
        GetRes7();
//        FDCResult[5] = dsk[FDCCurrDrv].Tracks[TrackIndex].sector[0].sector;   /* 0x01=IBM, 0x41=Data, 0xC1=System */
        FDCResult[5] = 0x41;   /* 0x01=IBM, 0x41=Data, 0xC1=System */
        }
      break;

    case 15:                    /* SEEK - Spur suchen */
      StatusCounter = 100;
      StatusRegister = 0x80 | (1 << (FDCCommand[1] & 3));
      FDCCurrDrv = FDCCommand[1] & 3;
      FDCCurrSide[FDCCurrDrv] = (FDCCommand[1] >> 2) & 1;
      if (dsk[FDCCurrDrv].HasDisk == FALSE)
        {
        st0 = FDCCurrDrv | 0xD8;  /* Equipment check, Not ready */
        GetRes7();
        }
      else
        {
        FDCCurrTrack[FDCCurrDrv] = FDCCommand[2];
        /* Diskette eingelegt? */
        if (dsk[FDCCurrDrv].HasDisk == TRUE)
          st0 = 0x20 | (FDCCommand[1] & 7); /* SEEK end + HD + US1 + US0 */
        else
          st0 = 0x08 | (FDCCommand[1] & 7); /* NOT READY + HD + US1 + US0 */
        ExecCmdPhase = FALSE;
        }
      break;

    default:
      Msg(MSGT_DEBUG, Msg_Get(MSG_FDC765_Unknown_Write), FDCCommand [0]);
      break;
  }
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
byte    FDCExecReadCommand (void)
{
  byte  ret = 0;

  switch (FDCCommand [0])
    {
    case 2:
      ret = dsk[FDCCurrDrv].Tracks[TrackIndex].DiscData[TrackDataStart + FDCDataPointer];
      FDCDataPointer ++;
      if (FDCDataPointer == FDCDataLength)
        {
        st0 = (FDCCommand[1] & 7) | 0x40;   /* Unit, head, command canceled */
        st1 = 0x80;                         /* End of track error           */
        GetRes7();
        }
      break;

    case 6:
      ret = dsk[FDCCurrDrv].Tracks[TrackIndex].DiscData[TrackDataStart + FDCDataPointer];
      FDCDataPointer ++;
      if (FDCDataPointer == FDCDataLength)
        {
        st0 = (FDCCommand[1] & 7) | 0x40;   /* Unit, head, command canceled */
        st1 = 0x80;                         /* End of track error           */
        GetRes7();
        }
      break;

    default:
      Msg(MSGT_DEBUG, Msg_Get(MSG_FDC765_Unknown_Read), FDCCommand [0]);
      break;
  }
  return ret;
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
byte    FDCGetResult (void)
{
  byte  ret = FDCResult[FDCResPointer];

  FDCResPointer ++;
  if (FDCResPointer == FDCResCounter)
    {
    StatusRegister = 0x80;
    ResultPhase = FALSE;
    }
  return ret;
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
void    FDC765_Init (void)
{
  int   i;

  for (i = 0; i < FDC765_MAXDRIVES /* was 1 ?!? */; i++)
    {
    dsk[i].HasDisk = FALSE;
    dsk[i].Tracks = NULL;
    dsk[i].TracksSize = 0;
    }
  FDC765_Reset();
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
void    FDC765_Close (void)
{
  int   i;

  for (i = 0; i < FDC765_MAXDRIVES /* was 1 ?!? */; i++)
    {
    // WriteDskImage (i);
    free (dsk[i].Tracks); // free() is doing the NULL test
    }
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
void    FDC765_Reset (void)
{
  int   i;

  FloppyMotor = 0;
  FDCPointer = 0;
  ExecCmdPhase = FALSE;
  ResultPhase = FALSE;
  StatusRegister = 128;

  for (i = 0; i < FDC765_MAXDRIVES; i++)
    {
    FDCCurrTrack[i] = 0;
    FDCWrProtect[i] = FALSE;
    }
  for (i = 0; i < 9; i++)
    FDCCommand[i] = 0;
  FDC765_Cmd_For_SF7000 = FALSE;
}


/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
void    FDC765_Data_Write (register byte Value)
{
  if (!ExecCmdPhase)
     {
     if (FDCPointer == 0)
        {
        FDCCommand [0] = Value & 0x1F;  /* New Command */
        FDCPointer ++;
        StatusRegister |= 0x10;         /* FDC Busy */
        }
     else
     if (FDCPointer < bytes_in_cmd[FDCCommand[0]])
        {
        FDCCommand[FDCPointer] = Value; // Parameter for the command
        FDCPointer ++;
        }

     if (FDCPointer == bytes_in_cmd[FDCCommand[0]])
        {
        FDCPointer = 0;
        StatusRegister |= 0x20;
        FDCExecWriteCommand (Value);                     /* Kommando ausführen */
        FDC765_Cmd_For_SF7000 = TRUE;
        }
     }
  else
     {
     FDCExecWriteCommand (Value);                     /* Kommando ausführen */
     }
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
byte    FDC765_Data_Read (void)
{
  FDC765_Cmd_For_SF7000 = FALSE;
  if (ExecCmdPhase)
     return FDCExecReadCommand();
  if (ResultPhase)
     return FDCGetResult();
  return 0;
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
byte    FDC765_Status_Read (void)
{
  // if (StatusCounter > 0)
  //    {
  //    StatusCounter --;
  //    return 0;
  //    }
  return StatusRegister;
}

/*********************************************************************/
/**                                                                 **/
/*********************************************************************/
void    FDC765_Disk_Write_Get (int DrvNum, void **Data, int *DataSize)
{
  if (dsk[DrvNum].HasDisk)
     {
     *Data = dsk[DrvNum].Tracks;
     *DataSize = dsk[DrvNum].TracksSize;
     }
  else
     {
     *Data = NULL;
     *DataSize = 0;
     }
}

void    FDC765_Disk_Remove (int DrvNum)
{
  dsk[DrvNum].HasDisk = FALSE;
  free (dsk[DrvNum].Tracks); // free does the NULL pointer test
}

/*********************************************************************/
/**                                                                 **/
/** Zeigt einen Datei-Dialog zur Auswahl von Disk-Image-Dateien an, **/
/** schließt eine evtl. bereits geöffnete Datei, öffnet die ausge-  **/
/** wählte Image-Datei und liest diese in den Speicher ein.         **/
/** Der für die Track-Informationen und die Daten des Disk-Images   **/
/** benötigte Speicher wird jedoch nur einmal mit MALLOC vom Syste  **/
/** angefordert und immer wieder verwendet, bis die Emulation be-   **/
/** endet wird.                                                     **/
/**                                                                 **/
/** DRVNUM = Driver Number (0 for A: and 1 for B:)                  **/
/**                                                                 **/
/*********************************************************************/

void    FDC765_Disk_Insert (int DrvNum, void *Data, int DataSize)
{
  FDC765_Disk *Disk = &dsk[DrvNum];

  // Write existing disk ?
  // WriteDskImage (DrvNum);

  // Remove existing disk
  FDC765_Disk_Remove (DrvNum);

  // Set HasDisk and Write Protection flags
  Disk->HasDisk = TRUE;
  FDCWrProtect[DrvNum] = TRUE; // Write protection always ON yet

  // No header in sf7000 image disks, initialization here
  Disk->Header.nbof_tracks = 40;
  Disk->Header.nbof_heads = 1;
  Disk->Header.tracksize = (16 * 0x100);

  // Calculating track size and allocating memory for it
  Disk->TracksSize = Disk->Header.tracksize * Disk->Header.nbof_tracks * Disk->Header.nbof_heads;
  Disk->Tracks = (FDC765_Track *)malloc (Disk->TracksSize);

  // Copying memory from data source
  memcpy (Disk->Tracks, Data, DataSize);
  if (DataSize > Disk->TracksSize)
     {
     Msg(MSGT_USER, Msg_Get(MSG_FDC765_Disk_Too_Large1), DataSize, Disk->TracksSize);
     Msg(MSGT_USER_BOX, "%s", Msg_Get(MSG_FDC765_Disk_Too_Large2));
     }
  if (DataSize < Disk->TracksSize)
     {
     Msg(MSGT_USER, Msg_Get(MSG_FDC765_Disk_Too_Small1), DataSize, Disk->TracksSize);
     Msg(MSGT_USER_BOX, "%s", Msg_Get(MSG_FDC765_Disk_Too_Small2));
     memset ((byte *)Disk->Tracks + DataSize, 0, Disk->TracksSize - DataSize);
     }
}

//-----------------------------------------------------------------------------

