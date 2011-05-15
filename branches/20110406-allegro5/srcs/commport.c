//-----------------------------------------------------------------------------
// MEKA - commport.c
// Gear-to-Gear Communication Port Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "commport.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_gear_to_gear  Gear_to_Gear;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// [01]  R/W
//       Bits 6 to 0: PC6 -> PC0
//       Read/Write data when the EXT connector is used as 7-bit I/O port.

// [02]  R/W
//       Bit 7: NMI Enabled
//         0: NMI are generated at the fall of PC6, 1: no NMI generated
//         Note: the bit has to be re-set everytime to keep NMI enabled
//       Bits 6 to 0: DirectionPC6 -> DirectionPC0
//         0 = input, 1 = output

// [03]  R/W
//       Bits 7 to 0: TD7 -> TD0
//       Used to send data during serial communication.

// [04]  R
//       Bits 7 to 0: RD7 -> RD0
//       Used to receive data during serial communication

// [05]  R/W
//       Bits 7 to 6: BS1 -> BS0 (write only?)
//         Baud rate setting
//         00: 4800 Bds (default), 01: 2400 Bds, 10: 1200 Bds, 11: 300 Bds
//       Bit 5: RON
//         0: Receive disable, 1: Receive enable
//         PC5 is forced to input mode (ignoring DPC5)
//       Bit 4: TON
//         0: Send disable, 1: Send enable
//         PC4 is forced to output mode (ignoring DPC4)
//       Bit 3: INT
//         0: no NMI generated, 1: an NMI is generated when data is received
//       Bit 2: FRER (read only)
//         0: No framing error, 1: framing error
//       Bit 1: RXRD (read only)
//         0: Nothing received, 1: something has been received
//       Bit 0: TXFL (read only)
//         0: Data to send has been transfered, 1: not yet

//-----------------------------------------------------------------------------

// Reset emulated communication port
void    Comm_Reset (void)
{
  Gear_to_Gear.Config = 0;
  Gear_to_Gear.Data_Direction_NMIe = 0;
  Gear_to_Gear.Data_01 = 0;
}

int     Comm_Read_01 (void)
{
  return (Gear_to_Gear.Data_01 & 0x7F);
}

void    Comm_Write_01 (int value)
{
  Gear_to_Gear.Config |= 1;                      // not yet sent (never sent!)
  Gear_to_Gear.Data_01 = value & 0x7F;
}

// Read Data Direction and NMI Configuration
int     Comm_Read_02 (void)
{
  return (Gear_to_Gear.Data_Direction_NMIe);
}

// Write Data Direction and NMI Configuration
void    Comm_Write_02 (int value)
{
  Gear_to_Gear.Data_Direction_NMIe = value & 0xFF;
}

// Read Data During Serial Communication
int     Comm_Read_03 (void)
{
  return (0x00);
}

// Send Data During Serial Communication
void    Comm_Write_03 (int value)
{
  Gear_to_Gear.Config |= 1;                      // not yet sent (never sent!)
 /* Ignoring actual write */
}

// Receive Data During Serial Communications ----------------------------------
int     Comm_Read_04 (void)
{
  return (0x00);
}

// Configuration / Status byte ------------------------------------------------
int     Comm_Read_05 (void)
{
  byte t = Gear_to_Gear.Config;
  // return (0);
  Gear_to_Gear.Config &= 252; // 11111100
  return (t);
}

// Configuration / Status byte ------------------------------------------------
void    Comm_Write_05 (int value)
{
  Gear_to_Gear.Config |= (value & /*0x38*/ 0xF8);
  // 0x38 = 0x08 | 0x10 | 0x20
  // 0xF8 = 0x08 | 0x10 | 0x20 | 0x40 | 0x80
}

//-----------------------------------------------------------------------------


