//-----------------------------------------------------------------------------
// MEKA - shared.h
// Shared headers, includes, variables and constants
//-----------------------------------------------------------------------------

#include "system.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define PROG_NAME               "Meka"
#define VERSION                 "0.70"
#define VERSION_HIGH            (0)
#define VERSION_LOW             (70)
#define PROG_NAME_VER           PROG_NAME " " VERSION
#define PROG_HOMEPAGE           "http://www.smspower.org/meka"
#define PROG_AUTHORS            "Omar Cornut (Bock), Hiromitsu Shioya, Marat Faizullin & co."
#define PROG_AUTHORS_SHORT      "Omar (Bock) and contributors"
#define PROG_DATE               "1998-2005"

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
#include "tfile.h"        // TFILE.H     Text File handling library
#include "memory.h"       // MEMORY.H    Stupid Malloc/Realloc/Free wrappers

//-----------------------------------------------------------------------------
// Z80 CPU cores
//-----------------------------------------------------------------------------

// with Marat Faizullin's Z80 Core
#ifdef MARAT_Z80
  #include "z80marat/Z80.h"
// with Richard Mitton's Z80 Core
#elif RAZE_Z80
  #include "z80raze/raze.h"
// with Neil Bradley's Z80 Core
#elif NEIL_Z80
  #include "mz80.h"
  #include "mcpu.h"
// with Marcel de Kogel's Z80 Core
#elif MDK_Z80
  #include "z80mk/z80.h"
// with MAME's Z80 Core
#elif MAME_Z80
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

#include "meka.h"           // MEKA.H       Main structures and definitions
#include "areplay.h"        // AREPLAY.H    Action Replay emulation
#include "beam.h"           // BEAM.H       TV beam emulation
#include "bmemory.h"        // BMEMORY.H    Backed memory devices emulation
#include "checksum.h"       // CHECKSUM.H   Checksum processing
#include "clock.h"          // CLOCK.H      Clock/Profiling functions
#include "commport.h"       // COMMPORT.H   Communication port Gear-to-Gear emulation
#include "coleco.h"         // COLECO.H     Colecovision specifics emulation
#include "country.h"        // COUNTRY.H    Country interfacing and emulation
#include "cpu.h"            // CPU.H        Interface with CPU / Interrupt emulation
#include "data.h"           // DATA.H       Data file loading and data processing
#include "datadump.h"       // DATADUMP.H   Data dumper (VRAM, RAM, ROM..)
#include "drivers.h"        // DRIVERS.H    Drivers structures
#include "eeprom.h"         // EEPROM.H     EEPROM (93c46) emulation
#include "effects.h"        // EFFECTS.H    Nifty effects (TV, etc..)
#include "errors.h"         // ERRORS.H     Errors code and handling
#include "games.h"          // GAMES.H      Easter eggs mini games
#include "glasses.h"        // GLASSES.H    3D Glasses support and emulation
#include "gui.h"            // GUI.H        Graphical User Interface (includes 14 other files)
#include "fdc765.h"         // FDC765.H     Floppy Disk emulator (for SF-7000)
#include "fonts.h"          // FONTS.H      Fonts helper and wrapper
#include "inputs.h"         // INPUTS.H     Inputs processing
#include "inputs_c.h"       // INPUTS_C.H   Inputs configuration stuff
#include "inputs_f.h"       // INPUTS_F.H   Inputs file parser/writer
#include "inputs_i.h"       // INPUTS_I.H   Inputs initialization stuff
#include "inputs_u.h"       // INPUTS_U.H   Inputs update
#include "ioports.h"        // IOPORTS.H    IO Ports emulation
#include "keyinfo.h"        // KEYINFO.H    Keyboard keys definitions (name, scancode, etc.)
#include "lightgun.h"       // LIGHTGUN.H   Light Phaser emulation
#include "machine.h"        // MACHINE.H    Emulated machine initialization/reset
#include "mainloop.h"       // MAINLOOP.H   Main Loop
#include "message.h"        // MESSAGE.H    Messages system and all messages
#include "misc.h"           // MISC.H       Miscellaneous
#include "nes.h"            // NES.H        NES emulation
#include "palette.h"        // PALETTE.H    Palette system and processing
#include "rapidfir.h"       // RAPIDFIR.H   Rapid Fire emulation
#include "sf7000.h"         // SF7000.H     SF-7000 emulation
#include "sg1ksc3k.h"       // SG1KSC3K.H   SG-1000/SC-3000 specifics emulation
#include "specials.h"       // SPECIALS.H   GUI specials effects
#include "sportpad.h"       // SPORTPAD.H   Sega Sportpad emulation
#include "textbox.h"        // TEXTBOX.H    Text box GUI Applet
#include "textview.h"       // TEXTVIEW.H   Text viewer GUI Applet
#include "themes.h"         // THEMES.H     Themes handling and interfacing
#include "themes_b.h"       // THEMES_B.H   Themes background drawing
#include "tools.h"          // TOOLS.H      Various tools
#include "tools_t.h"        // TOOLS_T.H    Various time related tools
#include "tvoekaki.h"       // TVOEKAKI.H   Sega TV Oekaki emulation
#include "tvtype.h"         // TVTYPE.H     TV Types interfacing and data table
#include "video.h"          // VIDEO.H      Main video functions
#include "video_c.h"        // VIDEO_C.H    C Replacement functions for existing ASM functions
#include "video_t.h"        // VIDEO_T.H    Table generation for video emulation
#include "video_m2.h"       // VIDEO_M2.H   Video modes 0,1,2,3 emulation (SG/SC/COL)
#include "video_m5.h"       // VIDEO_M5.H   Video mode 5 emulation (SMS/GG)
#include "vmachine.h"       // VMACHINE.H   Virtual machine system
// SOUND ENGINE ---------------------------------------------------------------
#include "sound.h"          // SOUND.H      Sound Engine (include other files)
//#include "fmunit.h"       // FMUNIT.H     FM Unit wrapper to emulators
//#include "fmeditor.h"	    // FMEDITOR.H   FM instrument editor applet
//#include "psg.h"          // PSG.H        PSG SN-76496 emulator
//#include "sasound.h"      // SASOUND.H    Sound system (by Hiroshi)
//#include "s_log.h"        // S_LOG.H      Sound logging
//#include "s_misc.h"       // S_MISC.H     Miscellaenous
//#include "s_opl.h"        // S_OPL.H	    OPL
//#include "vgm.h"          // VGM.H        VGM file creation
//#include "ym2413hd.h"     // YM2413HD.H   FM emulator / OPL
//#include "wav.h"          // WAV.H        WAV file creation
//#include "..mekaintf.h"   // EMU2413.H    FM emulator / Digital
// EAGLE ----------------------------------------------------------------------
#ifdef MEKA_EAGLE
 #include "eagle.h"         // EAGLE.H      EAGLE graphic filter
#endif

//-----------------------------------------------------------------------------
// MEKA non-shared includes
// Those are listed here for reference purposes, but should be included
// manually by each file using them.
//-----------------------------------------------------------------------------

// #include "libparse.h"    // LIBPARSE.H   Parsing structures and tools

// #include "about.h"       // ABOUT.H      About box
// #include "bios.h"        // BIOS.H       BIOS interface
// #include "blit.h"        // BLIT.H       Blitters
// #include "blit_c.h"      // BLIT_C.H     Blitters/Video Configuration Applet
// #include "blitintf.h"    // BLITINTF.H   Blitters interfacing
// #include "capture.h"     // CAPTURE.H    Screen capture
// #include "config.h"      // CONFIG.H     Configuration file handling
// #include "config_j.h"    // CONFIG_J.H   Configuration / Joystick drivers list
// #include "config_v.h"    // CONFIG_V.H   Configuration / Video drivers list
// #include "db.h"          // DB.H         DataBase
// #include "debugger.h"    // DEBUGGER.H   Debugger
// #include "desktop.h"     // DESKTOP.H    Desktop loading/saving functionnality
// #include "file.h"        // FILE.H       File (ROM) loading and processing, filename generation
// #include "fskipper.h"    // FSKIPPER.H   Frame skipper and auto frame skipper
// #include "inputs_t.h"    // INPUTS_T.H   Inputs tools
// #include "keyboard.h"    // KEYBOARD.H   Sega Keyboard emulation
// #include "mappers.h"     // MAPPERS.H    Mappers system and mappers emulation
// #include "memview.h"     // MEMVIEW.H    Memory viewer GUI Applet
// #include "options.h"     // OPTIONS.H    Options applet
// #include "patch.h"       // PATCH.H      Patching system
// #include "register.h"    // REGISTER.H   Registered version check
// #include "saves.h"       // SAVES.H      Savestates loading/saving
// #include "sdsc.h"        // SDSC.H       SDSC ROM Tag (designed by S8-Dev)
// #include "setup.h"       // SETUP.H      Interactive Setup
// #include "techinfo.h"    // TECHINFO.H   Technical information Applet
// #include "tileview.h"    // TILEVIEW.H   Tile viewer GUI Applet
// #include "vdp.h"         // VDP.H        VDP I/O emulation & misc stuff
// #include "vlfn.h"        // VLFN.H       Virtual long file names system

//-----------------------------------------------------------------------------
// Ressources
//-----------------------------------------------------------------------------
#ifdef WIN32
#include "MsVc/resource.h"  // RESOURCE.H   Windows ressources definitions
#endif

//-----------------------------------------------------------------------------
