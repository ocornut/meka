//-----------------------------------------------------------------------------
// MEKA - shared.h
// Shared headers, includes, variables and constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------

#define MEKA_SOUND				// Enable sound
#define MEKA_ZIP				// Enable ZIP file support
#define MEKA_JOYPAD				// Enable joypad support
#define MEKA_Z80_DEBUGGER		// Enable Z80 debugger
#define MARAT_Z80				// Z80 emulator to use

#define MEKA_NAME               "MEKA"
#define MEKA_VERSION            "0.80-alpha"
#define MEKA_VERSION_HIGH       (0)
#define MEKA_VERSION_LOW        (80)
#define MEKA_NAME_VERSION       MEKA_NAME " " MEKA_VERSION
#define MEKA_HOMEPAGE           "http://www.smspower.org/meka"
#define MEKA_AUTHORS            "Omar Cornut (Bock) and contributors"
#define MEKA_AUTHORS_SHORT      "Omar Cornut and contributors"
#define MEKA_DATE               "1998-2016"

extern char MEKA_BUILD_DATE[];
extern char MEKA_BUILD_TIME[];

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------

#include "system.h"
#include "libmisc.h"

//-----------------------------------------------------------------------------
// Z80 CPU cores
//-----------------------------------------------------------------------------

#ifdef MARAT_Z80
// with Marat Faizullin's Z80 Core
  #include "z80marat/Z80.h"
#elif RAZE_Z80
// with Richard Mitton's Z80 Core
  #include "z80raze/raze.h"
#elif MAME_Z80
// with MAME's Z80 Core
  #include "z80mame/z80.h"
#else
  #error "No CPU choosen for compilation."
#endif

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

struct t_tv_type;
struct t_widget;

//-----------------------------------------------------------------------------
// MEKA shared components includes
//-----------------------------------------------------------------------------
// FIXME: remove them if possible, in favor of manual include of what's needed.
//-----------------------------------------------------------------------------
#include "meka.h"           // Main structures and definitions
#include "bmemory.h"        // Backed memory devices emulation
#include "country.h"        // Country interfacing and emulation
#include "cpu.h"            // Interface with CPU / Interrupt emulation
#include "data.h"           // Data file loading and data processing
#include "drivers.h"        // Drivers structures
#include "errors.h"         // Errors code and handling
#include "gui.h"            // Graphical User Interface (includes 14 other files)
#include "fonts.h"          // Fonts helper and wrapper
#include "inputs.h"         // Inputs processing
#include "inputs_u.h"       // Inputs update
#include "ioports.h"        // IO Ports emulation
#include "machine.h"        // Emulated machine initialization/reset
#include "mainloop.h"       // Main Loop
#include "message.h"        // Messages system and all messages
#include "misc.h"           // Miscellaneous
#include "skin.h"           // Interface Skins
#include "sound/sound.h"    // Sound Engine (include other files)
#include "tools.h"          // Various tools
#include "vmachine.h"       // Virtual machine system

//-----------------------------------------------------------------------------
