//-----------------------------------------------------------------------------
// MEKA - sf7000.c
// SF-7000 (Super Control Station) Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "mappers.h"
#include "bios.h"
#include "sf7000.h"

//-----------------------------------------------------------------------------
// The SF-7000 is an addon to the SC-3000. It provides the later one with:
//   - a 3 inches floppy disk drive
//   - 64 kb of RAM
//   - a Centronics parallel port
//   - a RS232 Serial port
// The SF-7000 also contains an embedded 8 kb ROM called the IPL (Initial
// Program Loader) which is disablable after use and replaced by 8 kb of RAM
// in the memory map. The SF-7000 always boot from the IPL, at location 0x0000.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MEMORY MAP
//    0x0000 -> 0x3FFF : IPL ROM or RAM (depending on port 0xE6, bit 6)
//    0x4000 -> 0xFFFF : RAM
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT MAP (Note: those are called from IOPORTS.C)
//-----------------------------------------------------------------------------
// P.P.I (8255A)
//   0xE4 - PA: Input        - FDC/Printer check
//   0xE5 - PB: Input        - Printer data output (parallel)
//   0xE6 - PC: Output       - FDC/Printer control
//   0xE7 - Control Register
//-----------------------------------------------------------------------------
// FDC (765AC)
//   0xE0 - Status Register
//   0xE1 - Data Register
//-----------------------------------------------------------------------------
// USART (8251)
//   0xE8 - USARTD (Data)
//   0xE9 - USARTC (Command)
//-----------------------------------------------------------------------------
// Meaning of P.P.I. ports:
//    PA0 - FDC INT : INT signal from inout from FDC
//    PA1 - BUSY from Centronics printer
//    PA2 - Pin 17 of the FDC ?
// PB0..7 - Data outputs to Centronics printer
//    PC0 - /INUSE signal to FDD
//    PC1 - /MOTOR ON signal to FDD
//    PC2 - TC signal to FDD
//    PC3 - RESET signal to FDC
//    PC4 - N.C.
//    PC5 - N.C.
//    PC6 - /ROM SEL: Switching between IPL ROM and RAM
//    PC7 - /STROBE to Centronics printer
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// >>> Si tu as des docs a propos du registre de controle, je ne dis pas non ;)
// >> Quel registre de contr“le ? FDC? 8255?
// > Oui, celui qui est mapp‚ sur le port 0xE7.
// J'ai une description complŠte dans un bouquin pour interfacer le z80.
// J'ai sous la main un cours d'IUT, voici en gros le principe :
// --- Lors de l'init, utiliser CTRL avec :
// bit 7 : … 1
// bit 6 et 5 : mode de fonctionnement Groupe A
// bit 4 : sens port A (0=en sorties)
// bit 3 : sens 1/2 port C sup‚rieur (0=en sorties)
// bit 2 : mode de fonctionnement Groupe B
// bit 1 : sens port B (0=en sorties)
// bit 0 : sens 1/2 port C inf‚rieur (0=en sorties)
// --- Op‚rations de Set/Reset du port C, utiliser CTRL avec :
// bit 7 : … 0
// bit 6,5,4 : inutilis‚s
// bit 3,2,1 : num‚ro du bit du Port C … affecter
// bit 0 : valeur 0 ou 1
// exemple : LD A,5 ; OUT CTRL = mise … 1 du bit 2 du port C
//-----------------------------------------------------------------------------

t_sf7000 SF7000;

void    SF7000_Reset (void)
{
	SF7000.Port_E4 = 0x00;
	SF7000.Port_E5 = 0x00;
	SF7000.Port_E6 = 0x00;
	SF7000.Port_E7 = 0x00;
	SF7000.Port_E8 = 0x00;
	SF7000.Port_E9 = 0x00;
}

void    SF7000_IPL_Mapping_Update (void)
{
	if (IPL_Disabled)
	{
		Map_8k_RAM (0, 0);
		Map_8k_RAM (1, 1);
	}
	else
	{
		Map_16k_Other (0, BIOS_ROM_SF7000);
	}
}

// [MAPPER: SF-7000] WRITE BYTE -----------------------------------------------
WRITE_FUNC (Write_Mapper_SF7000)
{
	/*
	int Page = Addr >> 13;
	if ((Page == 0 || Page == 1) && (IPL_Enabled))
	{
	//Out_SF7000 (0xE6, SF7000.Port_E6 | 0x40);
	//Write_Error (Addr, Value);
	Msg(MSGT_DEBUG, Msg_Get(MSG_Debug_Trap_Write), sms.R.PC.W, Value, Addr);
	return;
	}
	Mem_Pages [Page] [Addr] = Value;
	*/

	// Allowing to write even with IPL is enabled is needed
	// (Sega Basic Disk Version needs that at least)
	RAM [Addr] = Value;
}

//-----------------------------------------------------------------------------

