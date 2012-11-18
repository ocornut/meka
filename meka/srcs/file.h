//-----------------------------------------------------------------------------
// MEKA - file.h
// ROM File Loading & File Tools - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

enum t_load_mode
{
	LOAD_MODE_COMMANDLINE,
	LOAD_MODE_GUI
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

bool            Load_ROM                (t_load_mode load_mode, bool user_verbose);
bool            Load_ROM_Command_Line  ();
bool            Reload_ROM             ();

void            Filenames_Init         ();     // Initialize filenames used by emulator (path to config files, etc.)
void            Filenames_Init_ROM     ();     // Initialize filenames for current ROM

//-----------------------------------------------------------------------------

