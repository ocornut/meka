//-----------------------------------------------------------------------------
// MEKA - ioports.c
// I/O Ports Emulation - Code
//-----------------------------------------------------------------------------
// FIXME: Old, crappy, incorrect mess.
// FIXME: Implement proper port map, mirroring, etc. Per system.
//-----------------------------------------------------------------------------

//#define DEBUG_IO
#include "shared.h"
#include "beam.h"
#include "bios.h"
#include "vdp.h"
#include "commport.h"
#include "fdc765.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

#define IO_LOG_WRITE      \
        { Msg (MSGT_DEBUG, Msg_Get (MSG_Debug_Trap_Port_Write), CPU_GetPC, Port, Value); }

#define IO_LOG_READ       \
        { Msg (MSGT_DEBUG, Msg_Get (MSG_Debug_Trap_Port_Read), CPU_GetPC, Port); }

//-----------------------------------------------------------------------------

// OUTPUT - CALLED BY THE EMULATED Z80 ----------------------------------------
//-----------------------------------------------------------------------------
void	Out_SMS (word Port, byte Value)
{
switch (Port /* & 0xFF*/)
 {
 // 0xBE/190 : VDP Data Port --------------------------------------------------
 case 0xBE: Tms_VDP_Out_Data (Value);
            return;

 // 0xBD/189 - 0xBF/191 : VDP Address Port ------------------------------------
 case 0xBD: case 0xBF: Tms_VDP_Out_Address (Value);
            return;

 // 0x3F/63 : LightGun & Nationalisation Port ---------------------------------
 case 0x3F: if ((tsms.Periph_Nat & 0xF) != (Value & 0xF))
               {
               Inputs.SportsPad_Latch [0] = Inputs.SportsPad_Latch [1] = 1;
               }
            tsms.Periph_Nat = Value;
            if (tsms.Periph_Nat == 0x0D) 
                Inputs.SportsPad_Latch [0] ^= 1;
            if (tsms.Periph_Nat == 0x07) 
                Inputs.SportsPad_Latch [1] ^= 1;
            // IO_LOG_WRITE;
            return;

 // 0xF0/240 and 0xF1/241: FM Chipset Ports ------------------------------------
 case 0xF0:
   sms.FM_Register = Value & 0x3F;
   return;
 case 0xF1:
   // Here we are not testing if FM Unit is enabled
   // Because SMS Japanese BIOS always use FM Unit (as well as PSG).
   // if (Sound.FM_Enabled)
      {
      if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
         {
         VGM_Data_Add_FM (&Sound.LogVGM, (Value << 8) | sms.FM_Register);
         }
      FM_Write (sms.FM_Register, Value);
      }
   return;

 // 0xF2/242: FM Chipset Detection --------------------------------------------
 case 0xF2: if (Sound.FM_Enabled)
               {
               sms.FM_Magic = Value;
               }
            //IO_LOG_WRITE;
            return;

 // 0x7E/126 - 0x7F/127 : PSG Port --------------------------------------------
 // At least Cosmic Spacehead uses 0x7E for writing.
 case 0x7E: case 0x7F:
            // Msg (MSGT_DEBUG, "At %04Xh @ %d: PSG = %02Xh", sms.R.PC.W, sms.Pages_Reg [2], Value);
            // PSG_0_Write (Value);
            SN76489_Write (Value);
            return;

 // 0xDE/221 : Keyboard Raster Port -------------------------------------------
 case 0xDE: sms.Input_Mode = Value; // & 7; // Upper bits needed for SK-1100 detection
            // Msg (MSGT_DEBUG, "At %04Xh: Port 0xDE = %02Xh", sms.R.PC.W, Value);
            return;

 // Gear-to-gear Emulation ----------------------------------------------------
 case 0x01: Comm_Write_01 (Value); return;
 case 0x02: Comm_Write_02 (Value); return;
 case 0x03: Comm_Write_03 (Value); return;
 case 0x05: Comm_Write_05 (Value); return;

 // Game Gear Stereo ----------------------------------------------------------
 // FIXME: emulate stereo!
 case 0x06: if (cur_drv->id == DRV_GG)
               {
               SN76489_StereoWrite (Value);
               if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
                  VGM_Data_Add_GG_Stereo (&Sound.LogVGM, Value);
               }
            return;

 case 0x3E: // RAM [0] = Value & 7;
            // Msgt (MSGT_DEBUG, "At %04Xh: [%02Xh] = %02Xh", sms.R.PC.W, Port, Value);
            return;

 // 0xFF/255: Switch from BIOS to Cartridge -----------------------------------
 // FIXME: This is awful! If anyone sees this, say goodbye to my honor.
 case 0xFF: if ((machine & (MACHINE_ROM_LOADED | MACHINE_NOT_IN_BIOS)) == MACHINE_ROM_LOADED)
			{
				BIOS_Switch_to_Game ();
			}
            return;
 }

#ifdef DEBUG_IO
 IO_LOG_WRITE;
#endif
}

// INPUT - CALLED BY THE EMULATED Z80 -----------------------------------------
u8		In_SMS (u16 Port)
{
    // FIXME: Proper mirroring/port mapping is not emulated.
    // Note how this is dependant of the SMS/GG revision.
    switch (Port /*& 0xFF*/)
    {
        // VDP Status
    case 0xBD: 
    case 0xBF: 
        return Tms_VDP_In_Status ();

        // VDP Data Port
    case 0xBE: 
        return Tms_VDP_In_Data ();

        // Scanline
    case 0x7E: 
        return Beam_Y ();

        // Horizontal X position (FIXME: emulate latch properly)
    case 0x7F: 
        return Beam_X ();

        // Input Port 1 (Controller 1 and part of Controller 2)
    case 0xC0:
    case 0xDC: 
        return Input_Port_DC ();

        // Input Port 2 (Controller 2 & Latches)
    case 0xC1:
    case 0xDD: 
        return Input_Port_DD ();

        // Keyboard scan / printer / cassette
    case 0xDE: 
        if (Inputs.SK1100_Enabled)
            return (sms.Input_Mode);
        return (0xFF);

        // Joystick 3 Port (Game Gear)
    case 0x00: 
        if (cur_drv->id == DRV_GG)
        {
            switch (sms.Country)
            {
            case COUNTRY_EXPORT:    tsms.Control_GG |= 0x40; break;
            case COUNTRY_JAPAN:     tsms.Control_GG &= 0xBF; break;
            }
            return (tsms.Control_GG /* | 0x3F */);
        }
        // FIXME: There is a difference here between models of SMS, and Wonder Boy in Monster World
        // takes advantage of it.
        return (0x00);

        // FM Unit Detection
    case 0xF2: 
        return (sms.FM_Magic);

        // Gear-to-Gear
    case 0x01: return (Comm_Read_01 ());
    case 0x02: return (Comm_Read_02 ());
    case 0x03: return (Comm_Read_03 ());
    case 0x04: return (Comm_Read_04 ());
    case 0x05: return (Comm_Read_05 ());

        /*
        // [Arcade] Inputs
        //      Bit 0 : Coin
        //      Bit 6 : Start
    case 0xE0:
        {
            u8 v = 0xFF;
            //IO_LOG_READ;
            if (key[KEY_1]) v &= ~0x80;
            if (key[KEY_2]) v &= ~0x40;
            if (key[KEY_3]) v &= ~0x20;
            if (key[KEY_4]) v &= ~0x10;
            if (key[KEY_5]) v &= ~0x08;
            if (key[KEY_6]) v &= ~0x04;
            if (key[KEY_7]) v &= ~0x02;
            if (key[KEY_8]) v &= ~0x01;
            return (v);
        }
        // [Arcade] Dipswitches
    case 0x50:
        {
            u8 v = 0xFF;
            IO_LOG_READ;
            if (key[KEY_Q]) v &= ~0x80;
            if (key[KEY_W]) v &= ~0x40;
            if (key[KEY_E]) v &= ~0x20;
            if (key[KEY_R]) v &= ~0x10;
            if (key[KEY_T]) v &= ~0x08;
            if (key[KEY_Y]) v &= ~0x04;
            if (key[KEY_U]) v &= ~0x02;
            if (key[KEY_I]) v &= ~0x01;
            return (v);
        }
        */

    }

#ifdef DEBUG_IO
    IO_LOG_READ;
#endif

    return (0xFF);
}

// OUTPUT - CALLED BY THE EMULATED Z80 ----------------------------------------
void Out_SF7000 (word Port, byte Value)
{
switch (Port)
 {
 // 0xBE/190 : VDP Data Port --------------------------------------------------
 case 0xBE: Tms_VDP_Out_Data (Value);
            return;

 // 0xBD/189 - 0xBF/191 : VDP Address Port ------------------------------------
 case 0xBD: case 0xBF: Tms_VDP_Out_Address (Value);
            return;

 // 0x7E/126 - 0x7F/127 : PSG Port --------------------------------------------
 case 0x7E: case 0x7F: SN76489_Write (Value); /* PSG_0_Write (Value); */ return;

 // 0xDE/221 : Keyboard Raster Port -------------------------------------------
 case 0xDE: sms.Input_Mode = Value; // & 7; // Upper bits needed for SK-1100 detection
            return;

 // SF-7000 Stuff -------------------------------------------------------------
 //--[ FDC ]-------------------------------------------------------------------
 // case 0xE0: /* Status */ return;
 case 0xE1: FDC765_Data_Write (Value); return;
 //--[ P.P.I. ]----------------------------------------------------------------
 //case 0xE4: // FDC/Printer check
 //    IO_LOG_WRITE; SF7000.Port_E4 = Value;
 //    return;
 case 0xE5: // Printer data output (parallel)
      //IO_LOG_WRITE; SF7000.Port_E5 = Value;
      return;
 case 0xE6: // FDC/Printer control
      //IO_LOG_WRITE;
      SF7000.Port_E6 = Value;
      SF7000_IPL_Mapping_Update ();
      if ((SF7000.Port_E6 & 0x03) == 0x03) // ???
         {
         // Reset Floppy Disk
         FDC765_Reset ();
         // Need to trigger a NMI there ?
         CPU_ForceNMI = TRUE;
         }
      return;
 case 0xE7: // Control Register
      //IO_LOG_WRITE;
      SF7000.Port_E7 = Value;
      if (!(Value & 0x80))
         {
         byte Mask = 1 << ((Value >> 1) & 0x07);
         if (Value & 0x01)
            SF7000.Port_E6 |= Mask;
         else
            SF7000.Port_E6 &= ~Mask;

         if (Value & 0x04)
            {
            FDC765_Reset ();
            FDC765_Cmd_For_SF7000 = TRUE;
            }
         }

      SF7000_IPL_Mapping_Update ();
      return;
 //--[ USART 8251 ]------------------------------------------------------------
 case 0xE8: /* IO_LOG_WRITE; */ SF7000.Port_E8 = Value; return;
 case 0xE9: /* IO_LOG_WRITE; */ SF7000.Port_E9 = Value; return;
 }

#ifdef DEBUG_IO
 IO_LOG_WRITE;
#endif
}

// INPUT - CALLED BY THE EMULATED Z80 -----------------------------------------
byte In_SF7000 (word Port)
{
 switch (Port /*& 0xFF*/)
 {
 // 0xBD/189 - 0xBF/191 : VDP Status ------------------------------------------
 case 0xBD: case 0xBF: return (Tms_VDP_In_Status ());

 // 0xBE/190 : VDP Data Port --------------------------------------------------
 case 0xBE: return (Tms_VDP_In_Data ());

 // 0xC0/192 - 0xDC/220 : Joystick 1 Port -------------------------------------
 case 0xC0:
 case 0xDC: return (Input_Port_DC ());

 // 0xC1/193 - 0xDD/221 : Joystick 2 & LightGun Latch Port --------------------
 case 0xC1:
 case 0xDD: return (Input_Port_DD ());

 // 0xDE: Keyboard scan / printer / cassette ----------------------------------
 case 0xDE: return sms.Input_Mode;

 // SF-7000 Stuff -------------------------------------------------------------
 //--[ FDC ]-------------------------------------------------------------------
 case 0xE0: return FDC765_Status_Read ();
 case 0xE1: return FDC765_Data_Read ();
 //--[ P.P.I. ]----------------------------------------------------------------
 case 0xE4: // FDC/Printer control
 {
      static int MysteriousTime = 0x3200;
      //IO_LOG_READ;
      if (--MysteriousTime <= 0)
         {
         MysteriousTime = 0x3200;
         SF7000.Port_E4 ^= 4;
         }
      return FDC765_Cmd_For_SF7000 | SF7000.Port_E4 /* & 4 */;
 }
 case 0xE5: // Printer data output (parallel)
      //IO_LOG_READ;
      return SF7000.Port_E5;
 case 0xE6: // FDC/Printer control
      //IO_LOG_READ;
      return SF7000.Port_E6;
 case 0xE7: /// Control Register
      //IO_LOG_READ;
      return SF7000.Port_E7;
 //--[ USART ]-----------------------------------------------------------------
 case 0xE8: /* IO_LOG_READ; */ return SF7000.Port_E8;
 case 0xE9: /* IO_LOG_READ; */ return SF7000.Port_E9;
 }

#ifdef DEBUG_IO
 IO_LOG_READ;
#endif

return (0xFF);
}

//-----------------------------------------------------------------------------

