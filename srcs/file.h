//-----------------------------------------------------------------------------
// MEKA - file.h
// ROM File Loading & File Tools - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define LOAD_COMMANDLINE  (0)
#define LOAD_INTERFACE    (1)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

bool            Load_ROM                (int mode, int user_verbose);
bool            Load_ROM_Command_Line   (void);
bool            Reload_ROM              (void);

void            Filenames_Init          (void);     // Initialize filenames used by emulator (path to config files, etc.)
void            Filenames_Init_ROM      (void);     // Initialize filenames for current ROM

//-----------------------------------------------------------------------------

