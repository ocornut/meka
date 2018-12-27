//
// Meka - FMUNIT.C
// FM Unit - Miscellaneous & wrapper to emulators
//

#include "shared.h"
#include "fmunit.h"
#include "emu2413/mekaintf.h"

//-----------------------------------------------------------------------------

// Registers (pointer to current FM emulator register)
u8 *					FM_Regs = NULL;

// Interface
void                    FM_Null_Interface_Function(void) { }
t_fm_unit_interface     FM_Null_Interface =
{
  "FM Null Interface",
  "<none>",
  FM_Null_Interface_Function,
  (void (*)(int, int))FM_Null_Interface_Function,
  FM_Null_Interface_Function,
  FM_Null_Interface_Function,
  FM_Null_Interface_Function
};

t_fm_unit_interface *   FM_Unit_Current = &FM_Null_Interface;

//-----------------------------------------------------------------------------

// Instruments Name (strings)
const char *FM_Instruments_Name[YM2413_INSTRUMENTS] =
{
  "User Voice",
  "Violin",
  "Guitar",
  "Piano",
  "Flute",
  "Clarinet",
  "Oboe",
  "Trumpet",
  "Organ",
  "Horn",
  "Synthesizer",
  "Harpsichord",
  "Vibraphone",
  "Synthetizer bass",
  "Acoustic bass",
  "Electric bass",
};

//-----------------------------------------------------------------------------

// Mask of Registers data to save for a state save
const u8	FM_Regs_SavingFlags[YM2413_REGISTERS] =
{
  // Registers
  // 00-07: user-definable tone channel - left at 0xff for now
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  // 0E:    rhythm mode control - only bit 5 since rest are unused/keys
  0,    0,    0,    0,    0,    0,    0x20, 0,
  // 10-18: tone F-number low bits - want all bits
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  // 19-1F: not needed
  0,    0,    0,    0,    0,    0,    0,
  // 20-28: tone F-number high bit, octave set, "key" & sustain
  //      0x3f = all
  //      0x2f = all but key
  //      0x1f = all but sustain
  //      0x0f = all but key and sustain
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  // 29-2F: not needed
  0,    0,    0,    0,    0,    0,    0,
  // 30-38: instrument number/volume - want all bits
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  // 39: not needed
  0
};

// Active given interface
void    FM_Set_Interface(t_fm_unit_interface *intf, byte *new_fm_regs)
{
	if (FM_Unit_Current != NULL)
		FM_Mute();

	if ((new_fm_regs != FM_Regs) && FM_Regs)
	{
		// Msg(MSGT_DEBUG, "%s: Copying FM registers...", __FUNCTION__);
		memcpy (new_fm_regs, FM_Regs, YM2413_REGISTERS);
	}

	FM_Unit_Current = intf;
	FM_Regs = new_fm_regs;
	FM_Regenerate();
}

// Active the dummy/null FM interface
void    FM_Null_Active()
{
	// Create a dummy set of register.
	static u8 FM_OPL_Regs [YM2413_REGISTERS];

	FM_Set_Interface (&FM_Null_Interface, FM_OPL_Regs);
}

void    FM_Save (FILE *f)
{
  fwrite (FM_Regs, YM2413_REGISTERS, 1, f);
}

// Load FM registers from given file pointer and call emulator Reload function
// Note: only the registers are saved/loaded currently
// If this has to change, please pay attention to the fact that MSD loading
// use this fonction to load old Massage save states.
void    FM_Load (FILE *f)
{
	fread (FM_Regs, YM2413_REGISTERS, 1, f);
	FM_Regenerate();
}

//-----------------------------------------------------------------------------

