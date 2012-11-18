//
// Meka - FMUNIT.H
// FM Unit - Miscellaneous & wrapper to emulators
//

#define YM2413_REGISTERS        (64)    // 64 registers area
#define YM2413_INSTRUMENTS      (16)    // 16 instruments (0 is user defined)
#define YM2413_VOLUME_STEPS     (16)    // 16 different volume steps
#define YM2413_VOLUME_MASK      (0x0F)

//-----------------------------------------------------------------------------

// Registers (pointer to current FM emulator register)
extern u8 *				FM_Regs;

// Instruments Name (strings)
extern const char *     FM_Instruments_Name [YM2413_INSTRUMENTS];

// Mask of Registers data to save for a state save
extern const u8			FM_Regs_SavingFlags [YM2413_REGISTERS];

//-----------------------------------------------------------------------------
// Interface
//-----------------------------------------------------------------------------

struct t_fm_unit_interface
{
	const char*	desc;
	const char*	author;
	void (*f_reset)     ();
	void (*f_write)      (int Register, int Value);
	void (*f_mute)      ();
	void (*f_resume)    ();
	void (*f_regenerate)();
};

void    FM_Set_Interface (t_fm_unit_interface *intf, byte *new_fm_regs);
void    FM_Null_Active  ();

extern t_fm_unit_interface* FM_Unit_Current;

//-----------------------------------------------------------------------------
// Interface Functions Accesses macros
//-----------------------------------------------------------------------------

#define FM_Reset()      FM_Unit_Current->f_reset()
#define FM_Write(R,V)	FM_Unit_Current->f_write(R, V);
#define FM_Mute()       FM_Unit_Current->f_mute()
#define FM_Resume()     FM_Unit_Current->f_resume()
#define FM_Regenerate()	FM_Unit_Current->f_regenerate()

//-----------------------------------------------------------------------------

void    FM_Save(FILE *f);
void    FM_Load(FILE *f);

//-----------------------------------------------------------------------------

