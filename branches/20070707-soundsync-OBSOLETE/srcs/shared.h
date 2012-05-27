//-----------------------------------------------------------------------------
// MEKA - shared.h
// Shared headers, includes, variables and constants
//-----------------------------------------------------------------------------

#include "system.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MEKA_NAME               "MEKA"
#define MEKA_VERSION            "0.72"
#define MEKA_VERSION_HIGH       (0)
#define MEKA_VERSION_LOW        (72)
#define MEKA_NAME_VERSION       MEKA_NAME " " MEKA_VERSION
#define MEKA_HOMEPAGE           "http://www.smspower.org/meka"
#define MEKA_AUTHORS            "Omar Cornut (Bock) and contributors"
#define MEKA_AUTHORS_SHORT      "Omar Cornut and contributors"
#define MEKA_DATE               "1998-2007"

extern char MEKA_BUILD_DATE[];
extern char MEKA_BUILD_TIME[];

#define __MEKA_SOUND__

//-----------------------------------------------------------------------------
// Tools / Library Includes
//-----------------------------------------------------------------------------
// Those should be compiling without MEKA
//-----------------------------------------------------------------------------

#include "libmy.h"        // LIBMY.H     Own replacements for some LibC functions
#include "liblist.h"      // LIBLIST.H   Chained list functionnalities
#include "memory.h"       // MEMORY.H    Stupid Malloc/Realloc/Free wrappers

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

typedef struct t_tv_type    t_tv_type;          // from TVTYPE.H
typedef struct t_widget     t_widget;           // from G_WIDGET.H

//-----------------------------------------------------------------------------
// MEKA shared components includes
//-----------------------------------------------------------------------------
// Note: remove them if possible, in favor of manual include of what's needed.
// See commented list of includes below.
//-----------------------------------------------------------------------------
#include "meka.h"           // Main structures and definitions
#include "bmemory.h"        // Backed memory devices emulation
#include "coleco.h"         // Colecovision specifics emulation
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
#include "sf7000.h"         // SF-7000 emulation
#include "sg1ksc3k.h"       // SG-1000/SC-3000 specifics emulation
#include "skin.h"           // Interface Skins
#include "specials.h"       // GUI specials effects
#include "textbox.h"        // Text box GUI Applet
#include "tools.h"          // Various tools
#include "tools_t.h"        // Various time related tools
#include "tvtype.h"         // TV Types interfacing and data table
#include "vmachine.h"       // Virtual machine system

// SOUND ENGINE ---------------------------------------------------------------
#include "sound.h"          // Sound Engine (include other files)
//#include "fmunit.h"       // FM Unit wrapper to emulators
//#include "fmeditor.h"	    // FM instrument editor applet
//#include "psg.h"          // PSG SN-76496 emulator
//#include "sasound.h"      // Sound system (by Hiroshi)
//#include "s_log.h"        // Sound logging
//#include "s_misc.h"       // Miscellaenous
//#include "s_opl.h"        // OPL
//#include "vgm.h"          // VGM file creation
//#include "ym2413hd.h"     // FM emulator / OPL
//#include "wav.h"          // WAV file creation
//#include "emu2413.h"      // FM emulator / Digital

//-----------------------------------------------------------------------------
// MEKA non-shared includes
// Those are listed here for reference purposes, but should be included
// manually by each file using them.
//-----------------------------------------------------------------------------

// #include "tools/tfile.h"     // Text file reading
// #include "tools/libparse.h"  // Parsing structures and tools

// #include "about.h"       // About box
// #include "areplay.h"     // Action Replay emulation
// #include "beam.h"        // TV beam emulation
// #include "bios.h"        // BIOS interface
// #include "blit.h"        // Blitters
// #include "blit_c.h"      // Blitters/Video Configuration Applet
// #include "blitintf.h"    // Blitters interfacing
// #include "capture.h"     // Screen capture
// #include "checksum.h"    // Checksum processing
// #include "config.h"      // Configuration file handling
// #include "config_j.h"    // Configuration / Joystick drivers list
// #include "config_v.h"    // Configuration / Video drivers list
// #include "commport.h"    // Communication port / Gear-to-Gear emulation
// #include "datadump.h"    // Data dumper (VRAM, RAM, ROM..)
// #include "db.h"          // DataBase
// #include "debugger.h"    // Debugger
// #include "desktop.h"     // Desktop loading/saving functionnality
// #include "eagle.h"       // EAGLE graphic filter
// #include "eeprom.h"      // EEPROM (93c46) emulation
// #include "effects.h"     // Nifty effects (TV, etc..)
// #include "fdc765.h"      // Floppy Disk emulator (for SF-7000)
// #include "file.h"        // File (ROM) loading and processing, filename generation
// #include "fskipper.h"    // Frame skipper and auto frame skipper
// #include "games.h"       // Easter eggs mini games
// #include "glasses.h"     // 3-D Glasses support and emulation
// #include "inputs_c.h"    // Inputs configuration stuff
// #include "inputs_f.h"    // Inputs file parser/writer
// #include "inputs_i.h"    // Inputs initialization
// #include "inputs_t.h"    // Inputs tools
// #include "keyboard.h"    // Sega Keyboard emulation
// #include "keyinfo.h"     // Keyboard keys definitions (name, scancode, etc.)
// #include "lightgun.h"    // Light Phaser emulation
// #include "mappers.h"     // Mappers system and mappers emulation
// #include "memview.h"     // Memory viewer GUI Applet
// #include "nes.h"         // NES emulation
// #include "options.h"     // Options applet
// #include "palette.h"     // Palette system and processing
// #include "patch.h"       // Patching system
// #include "rapidfir.h"    // Rapid Fire emulation
// #include "register.h"    // Registered version check
// #include "saves.h"       // Savestates loading/saving
// #include "sdsc.h"        // SDSC ROM Tag (designed by S8-Dev)
// #include "setup.h"       // Interactive Setup
// #include "sportpad.h"    // Sega Sports Pad emulation
// #include "techinfo.h"    // Technical information Applet
// #include "textview.h"    // Text viewer GUI Applet
// #include "tileview.h"    // Tile viewer GUI Applet
// #include "tvoekaki.h"    // Sega TV Oekaki emulation
// #include "vdp.h"         // VDP I/O emulation & misc stuff
// #include "video.h"       // Main video functions
// #include "video_c.h"     // C Replacement functions for existing ASM functions
// #include "video_m2.h"    // Video modes 0,1,2,3 rendering (SG/SC/COL)
// #include "video_m5.h"    // Video mode 4 rendering (SMS/GG)
// #include "vlfn.h"        // Virtual long file names system

//-----------------------------------------------------------------------------
// Ressources
//-----------------------------------------------------------------------------
#ifdef ARCH_WIN32
#include "MsVc/resource.h"  // RESOURCE.H   Windows ressources definitions
#endif

//-----------------------------------------------------------------------------
