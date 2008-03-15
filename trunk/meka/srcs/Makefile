#------------------------------------------------------
# MEKA - Makefile
# 1998-2008
# http://www.smspower.org/meka
# (c) Omar Cornut and contributors
#     Hiromitsu Shioya (Sound stuff), Started on the 12 February 1999
#     Marat Faizullin (Z80Marat/*), 1994-1998
# Refer to MEKA.TXT file for full credits information.
#------------------------------------------------------
# Best viewed with TAB=8
#------------------------------------------------------
# Use 'help' rule ('make help') to get command line help.
# Read SOURCES.TXT for help compiling.
#------------------------------------------------------

#------------------------------------------------------
# MEKA may be difficult to compile! Refer to documentation.
# The build process was made for my own usage of my own computer,
# and there might be some leftover of that time (hardcoded paths,
# hand modified libraries...). 
# Of course, MEKA should be leaning toward ease of compilation, 
# so feel free to comment or correct things.
#------------------------------------------------------
# May require those environment variables:
#   DXSDKDIR    Set to DirectX SDK directory [win32]
#   NASM        Set to NASM executable
#   ZLIBDIR     Set to ZLIB directory
#   LIBPNGDIR   Set to LIBPNG directory
# Beware that the Makefile won't notify you if one of them
# if used (will use empty strings, can cause compilation error)
#------------------------------------------------------

#------------------------------------------------------
# System target to compile MEKA for (dos|unix|win32)
# All systems must currently run on X86 archicture.
# - unix  : UN*X system. Makefile up to date. May require system/library tweaking.
# - win32 : Windows 32-bits. Makefile NOT up to date (can be updated). Hardcoded paths. Prefer using MsVc\Meka.sln (MSVC++ 7.1)
# - dos   : MS-DOS or compatible. Could work. Not maintened anymore.
#------------------------------------------------------
# SYSTEM = dos
# SYSTEM = win32
SYSTEM = unix
# SYSTEM = macosx
#------------------------------------------------------
BUILD = release
# BUILD = debug
#------------------------------------------------------

#---[ DOS/DJGPP ]--------------------------------------
ifeq ($(SYSTEM), dos)
#--- Executable name
EXE     = ../meka.exe
#--- Output directory
OD      = obj
#--- Compilation
CC      = gcc -Werror
CC_OUT  = -o
LINKER  = gcc
ASM     = nasm
OTYPE   = coff
#--- Tools
RM      = rm
MV      = ren
MKDIR   = mkdir
#--- Definitions
DEF_OS  = -DARCH_DOS -DX86_ASM
INC_OS  = -I../libs/seal/include -I$(LIBPNGDIR) -I$(ZLIBDIR)
X86_ASM = yes
#--- Libraries
LIB_OS  = -lm -L$(LIBPNGDIR)
endif

#---[ UNIX/GCC ]---------------------------------------
ifeq ($(SYSTEM), unix)
#--- Executable name
EXE     = ../meka
#--- Output directory
OD      = obj
#--- Compilation
CC      = gcc
CC_OUT  = -o
LINKER  = gcc
ASM     = nasm
OTYPE   = elf
#--- Tools
RM      = rm
MV      = mv
MKDIR   = mkdir
#--- Definitions
DEF_OS  = -DARCH_UNIX -DX86_ASM -DASM_SYMBOLS_REQUIRE_UNDERSCORE
INC_OS  = -Ilibs -I../include		                               # This may require an update
X86_ASM = yes
#--- Libraries
LIB_OS  = -Llibs -L/usr/X11R6/lib -lX11 -lXext -lm -lpthread -L../lib  # This may require an update
LIB_OS += -lXcursor -lXpm
LIB_OS += -lXxf86dga -lXxf86vm
# -lossaudio
endif

#---[ MACOSX/GCC ]---------------------------------------
ifeq ($(SYSTEM), macosx)
#--- Executable name
EXE     = ../meka
#--- Output directory
OD      = obj
#--- Compilation
CC      = gcc
CC_OUT  = -o
LINKER  = gcc
ASM     = nasm
OTYPE   = macho
#--- Tools
RM      = rm
MV      = mv
MKDIR   = mkdir
#--- Definitions
DEF_OS  = -DARCH_MACOSX -DUNIX -DX86_ASM
INC_OS  = -Ilibs -I../include -I../libs/seal/include `pkg-config libpng --cflags` # This may require an update
X86_ASM = yes
#--- Libraries
LIB_OS  = `pkg-config libpng --libs`
endif

#---[ WINDOWS/MSVC ]-----------------------------------
ifeq ($(SYSTEM), win32)
#--- Executable name
EXE     = ../mekaw.exe
#--- Output directory
OD      = objw
#--- Compilation
CC      = runner.exe cl @ -nologo
CC_OUT  = -Fo
LINKER  = runner.exe link @
ASM     = nasmw
OTYPE   = win32
#--- Tools
RM      = rm
MV      = mv
MKDIR   = md
RC      = rc

#--- Definitions
DEF_OS  = -DARCH_WIN32 -DX86_ASM
INC_OS  = -I../libs/seal/include -I$(LIBPNGDIR) -I$(ZLIBDIR)
X86_ASM = yes

#--- Librairies
LIB_OS          = kernel32.lib user32.lib gdi32.lib comdlg32.lib ole32.lib ddraw.lib winmm.lib dsound.lib dinput.lib dxguid.lib
LIB_PATHS       = -libpath:$(MSVCDIR)/lib/ -libpath:$(MSVCDIR)/PlatformSDK/lib/ -libpath:$(DXSDKDIR)/lib

#--- Ressources
RESSOURCES = mekaw.res

endif

#-----------------------------------------------------
# Settings
#-----------------------------------------------------
# Note: As for the Z80 CPU core, only Marat's core is maintained.
#-----------------------------------------------------
OPT_SOUND = yes
OPT_DEBUGGER = yes
ifeq ($(SYSTEM), macosx)
OPT_DEBUGGER = no
endif
OPT_JOY = yes
# CPU_CORE = raze
CPU_CORE = marat
# CPU_CORE = mame
#-----------------------------------------------------

#-----------------------------------------------------
# Option : Sound emulation and output
#-----------------------------------------------------
# Note: I don't think this can be safely disabled anymore,
# so let's pretend it's not an option, it is mandatory.
#-----------------------------------------------------
ifeq ($(OPT_SOUND), yes)
DEF_SOUND = -DMEKA_SOUND
OBJ_SOUND = $(OD)/sound/sound.o $(OD)/sound/s_misc.o $(OD)/sound/s_opl.o $(OD)/sound/s_log.o $(OD)/sound/sasound.o $(OD)/sound/psg.o $(OD)/sound/fmeditor.o $(OD)/sound/wav.o $(OD)/sound/vgm.o $(OD)/sound/fmunit.o $(OD)/sound/ym2413hd.o 
OBJ_SOUND += $(OD)/sound/emu2413/emu2413.o $(OD)/sound/emu2413/mekaintf.o
ifeq ($(SYSTEM), dos)
LIB_SOUND = -L../libs/seal/lib -laudio
endif

ifeq ($(SYSTEM), unix)
LIB_SOUND = -lseal
endif

ifeq ($(SYSTEM), macosx)
OBJ_SOUND += $(OD)/sound/null_seal.o
endif

ifeq ($(SYSTEM), win32)
LIB_SOUND = ../libs/seal/lib/win32/audw32vc.lib
endif
endif

#-----------------------------------------------------
# Option : Joypad/joystick support
#-----------------------------------------------------
ifeq ($(OPT_JOY), yes)
DEF_JOY = -DMEKA_JOY
endif

#-----------------------------------------------------
# Option : ZIP support
#-----------------------------------------------------
DEF_ZIP = -DMEKA_ZIP
OBJ_ZIP = $(OD)/libaddon/zip/unzip.o
ifneq ($(SYSTEM), win32)
LIB_ZIP = -lz
else
LIB_ZIP = $(ZLIBDIR)/zlibs.lib
endif
#-----------------------------------------------------

#-----------------------------------------------------
# Option : PNG Support
#-----------------------------------------------------
DEF_PNG = -DMEKA_PNG
OBJ_PNG = $(OD)/libaddon/png/loadpng.o $(OD)/libaddon/png/savepng.o $(OD)/libaddon/png/regpng.o
ifneq ($(SYSTEM), win32)
LIB_PNG = -lpng
else
LIB_PNG = $(LIBPNGDIR)/libpng.lib
endif
#-----------------------------------------------------

#-----------------------------------------------------
# Libraries
#-----------------------------------------------------

# Allegro
ifeq ($(SYSTEM), dos)
LIB_ALLEG = -lalleg
endif

ifeq ($(SYSTEM), unix)
LIB_ALLEG = `allegro-config --libs`
endif

ifeq ($(SYSTEM), macosx)
LIB_ALLEG = `allegro-config --libs`
endif

ifeq ($(SYSTEM), win32)
LIB_ALLEG = alleg_s.lib
endif

# Full library list
ifeq ($(SYSTEM), win32)
LIB = $(LIB_PATHS) $(RESSOURCES)
LIB += $(LIB_ALLEG) $(LIB_SOUND) $(LIB_ZIP) $(LIB_OS) -nodefaultlib:msvcrt
else
LIB = $(LIB_ALLEG) $(LIB_SOUND) $(LIB_OS) $(LIB_PNG) $(LIB_ZIP)
endif

#-----------------------------------------------------
# Objects to create and link
#-----------------------------------------------------

OBJ_GUI   = $(OD)/gui.o $(OD)/g_action.o $(OD)/g_box.o $(OD)/g_menu.o $(OD)/g_menu_i.o $(OD)/g_menu_t.o $(OD)/g_mouse.o $(OD)/g_init.o $(OD)/g_tools.o $(OD)/g_widget.o $(OD)/g_update.o $(OD)/specials.o $(OD)/desktop.o $(OD)/skin.o $(OD)/skin_bg.o
OBJ_GAPPS = $(OD)/g_file.o $(OD)/app_game.o $(OD)/app_options.o $(OD)/textbox.o $(OD)/app_mapview.o $(OD)/app_tileview.o $(OD)/app_textview.o $(OD)/app_techinfo.o $(OD)/app_memview.o $(OD)/app_palview.o $(OD)/app_about.o
OBJ_EMU   = $(OD)/mainloop.o $(OD)/ioports.o $(OD)/commport.o $(OD)/vmachine.o $(OD)/bios.o $(OD)/country.o $(OD)/bmemory.o $(OD)/mappers.o $(OD)/eeprom.o $(OD)/beam.o $(OD)/tvtype.o
OBJ_INP   = $(OD)/inputs.o $(OD)/inputs_c.o $(OD)/inputs_f.o $(OD)/inputs_i.o $(OD)/inputs_t.o $(OD)/inputs_u.o $(OD)/lightgun.o $(OD)/keyboard.o $(OD)/sportpad.o $(OD)/rapidfir.o $(OD)/tvoekaki.o $(OD)/glasses.o
OBJ_FEAT  = $(OD)/checksum.o $(OD)/db.o $(OD)/vlfn.o $(OD)/patch.o $(OD)/games.o $(OD)/saves.o
OBJ_CFG   = $(OD)/config.o $(OD)/config_v.o $(OD)/config_j.o
OBJ_MISC  = $(OD)/misc.o $(OD)/build.o $(OD)/fonts.o $(OD)/file.o $(OD)/data.o $(OD)/tools.o $(OD)/tools_t.o $(OD)/keyinfo.o $(OD)/drivers.o $(OD)/message.o $(OD)/capture.o $(OD)/errors.o $(OD)/sdsc.o $(OD)/setup.o
OBJ_BLIT  = $(OD)/blit.o $(OD)/blitintf.o $(OD)/eagle.o $(OD)/hq2x.o $(OD)/hq2x16.o $(OD)/hq2x32.o
OBJ_CPU   = $(OD)/machine.o $(OD)/cpu.o $(OD)/m6502.a
OBJ_VIDEO = $(OD)/video.o $(OD)/video_m2.o $(OD)/video_m5.o $(OD)/video_c.o $(OD)/vdp.o $(OD)/palette.o $(OD)/effects.o $(OD)/fskipper.o
OBJ_MACH  = $(OD)/sg1ksc3k.o $(OD)/sf7000.o $(OD)/nes.o $(OD)/nes_maps.o $(OD)/nes_ppu.o $(OD)/coleco.o $(OD)/fdc765.o
OBJ_TOOLS = $(OD)/memory.o $(OD)/tools/libmy.o $(OD)/tools/liblist.o $(OD)/tools/libparse.o $(OD)/tools/tfile.o $(OD)/osd/misc.o $(OD)/osd/timer.o

ifeq ($(X86_ASM), yes)
OBJ_EMU += $(OD)/mappersa.o
OBJ_VIDEO += $(OD)/videoasm.o
endif

#-----------------------------------------------------
# Includes directory
#-----------------------------------------------------
INCLUDES = -I. -I./tools -I./sound $(INC_OS)

#-----------------------------------------------------
# Preprocessor Definitions
#-----------------------------------------------------
DEFINES = $(DEF_OS) $(DEF_SOUND) $(DEF_ZIP) $(DEF_PNG) $(DEF_JOY)
# DEFINES += -DDEBUG_WHOLE
# DEFINES += -DDEBUG_IO
# DEFINES += -DDEBUG_PAGES
# DEFINES += -DDEBUG_MEM
# DEFINES += -DDEBUG_VDP
# DEFINES += -DDEBUG_PALETTE

#-----------------------------------------------------
# Option : Debugger
#-----------------------------------------------------
ifeq ($(OPT_DEBUGGER), yes)
DEFINES += -DMEKA_Z80_DEBUGGER
OBJ_CPU += $(OD)/debug.o $(OD)/debugger.o $(OD)/datadump.o
endif

#-----------------------------------------------------
# Compiler Flags
#-----------------------------------------------------
CFLAGS = $(INCLUDES) $(DEFINES)

ifeq ($(SYSTEM), win32)
# Win32
# (Note:TARGET unsupported for Win32 makefile compilation)
CFLAGS += -W2 -Gd -Ox -GB -MT
else

# UNIX/MS-DOS
# RELEASE build
ifeq ($(BUILD), release)
CFLAGS += -Wall -march=pentium -O6 -ffast-math -fno-strength-reduce -funroll-all-loops -fomit-frame-pointer 
endif
# DEBUG build
ifeq ($(BUILD), debug)
CFLAGS += -Wall -g -ggdb
endif

endif

#-----------------------------------------------------
# Z80 CPU Cores
#-----------------------------------------------------

# Marat Faizullin's Z80 Core
ifeq ($(CPU_CORE), marat)
CFLAGS += -DMARAT_Z80
OBJ_CPU += $(OD)/z80marat.a
endif

# Richard Mitton's RAZE Z80 Core (obsolete)
ifeq ($(CPU_CORE), raze)
CFLAGS += -DRAZE_Z80
OBJ_CPU += z80raze/raze.o
endif

# MAME Z80 Core (obsolete)
ifeq ($(CPU_CORE), mame)
CFLAGS += -DMAME_Z80
OBJ_CPU += z80mame/z80.o
endif

#-----------------------------------------------------
# Complete Objects List
#-----------------------------------------------------
OBJ_MEKA = $(OD)/meka.o $(OBJ_CPU) $(OBJ_VIDEO) $(OBJ_EMU) $(OBJ_INP) $(OBJ_FEAT) $(OBJ_MISC) $(OBJ_CFG) $(OBJ_GUI) $(OBJ_GAPPS) $(OBJ_SOUND) $(OBJ_MACH) $(OBJ_ZIP) $(OBJ_PNG) $(OBJ_BLIT) $(OBJ_TOOLS) $(OBJ_LIBS)

#-----------------------------------------------------
# Linking Rule
#-----------------------------------------------------

exe	: makedir buildupd.exe $(EXE)

$(EXE)	: $(OBJ_MEKA)
ifneq ($(SYSTEM), win32)
	$(LINKER) -o $@ $(OBJ_MEKA) $(LIB)
else
	$(LINKER) -nologo -release -subsystem:windows -out:$@ -section:.text,erw -section:.data,erw -section:.bss,erw $(OBJ_MEKA) $(LIB)
endif

#-----------------------------------------------------
# Compiling Rules
#-----------------------------------------------------

# Compiling MEKA.C call buildupd.exe first to get build time
$(OD)/meka.o : meka.c meka.h shared.h
	./buildupd.exe
	$(CC) $(CFLAGS) -c meka.c $(CC_OUT)$@

# Custom Dependencies
$(OD)/gui.o : gui.c gui.h shared.h meka.h
$(OD)/build.o : build.c
$(OD)/g_menu.o : g_menu.c g_menu.h g_menu_t.c g_menu_t.h g_menu_i.c g_menu_i.h
$(OD)/g_menu_i.o : g_menu.c g_menu.h g_menu_t.c g_menu_t.h g_menu_i.c g_menu_i.h
$(OD)/g_menu_t.o : g_menu.c g_menu.h g_menu_t.c g_menu_t.h g_menu_i.c g_menu_i.h
# $(OD)/cpu.o : cpu.c cpu.h shared.h meka.h
# $(OD)/palette.o : palette.c palette.h video.h shared.h meka.h
# $(OD)/inputs.o : inputs.c inputs.h keyboard.c keyboard.h keyb_map.h
# $(OD)/data.o : data.c data.h g_colors.h

# CPU emulators related compilation
$(OD)/m6502.a : m6502/m6502.c m6502/m6502.h m6502/tables.h m6502/codes.h
	$(CC) $(CFLAGS) -c m6502/m6502.c $(CC_OUT)$@

$(OD)/z80marat.a : z80marat/Z80.c z80marat/Z80.h z80marat/Codes.h z80marat/CodesED.h z80marat/CodesCB.h z80marat/CodesXX.h z80marat/Tables.h z80marat/CodesXCB.h z80marat/Debug.c z80marat/Z80Call.c
	$(CC) $(CFLAGS) -c z80marat/Z80.c $(CC_OUT)$@

$(OD)/debug.o : z80marat/Debug.c z80marat/Z80.h
	$(CC) $(CFLAGS) -c z80marat/Debug.c $(CC_OUT)$@

# Generic Rules

$(OD)/%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< $(CC_OUT)$@

$(OD)/%.o : %.asm
	$(ASM) $(DEF_OS) -f $(OTYPE) $< -o $@

$(OD)/%.a : %.asm
	$(ASM) $(DEF_OS)-f $(OTYPE) $< -o $@

$(OD)/osd/%.o : osd/%.c osd/%.h
	$(CC) $(CFLAGS) -c $< $(CC_OUT)$@

$(OD)/tools/%.o : tools/%.c tools/%.h
	$(CC) $(CFLAGS) -c $< $(CC_OUT)$@

$(OD)/sound/%.o : sound/%.c sound/%.h
	$(CC) $(CFLAGS) -c $< $(CC_OUT)$@

$(OD)/libaddon/png/%.o : libaddon/png/%.c
	$(CC) $(CFLAGS) -c $< $(CC_OUT)$@

$(OD)/libaddon/zip/%.o : libaddon/zip/%.c libaddon/zip/%.h
	$(CC) $(CFLAGS) -c $< $(CC_OUT)$@

#-----------------------------------------------------
# Help
#-----------------------------------------------------

help :
	@echo "Rules:"
	@echo "  exe             - Build executable"
	@echo "  dist            - Build all distributions"
	@echo "  dist_src        - Build sources distribution"
	@echo "  dist_bin_dos    - Build binary DOS distribution"
	@echo "  dist_bin_win32  - Build binary Win32 distribution"
	@echo "  dist_bin_unix   - Build binary UNIX distribution"
	@echo "  clean           - Clean most object files (excluding z80 stuff, etc.)"
	@echo "  clean_all       - Clean object files"
	@echo "  makedir         - Create output directories"

#-----------------------------------------------------
# Tools
#-----------------------------------------------------

buildupd.exe : buildupd.c
	gcc buildupd.c -o buildupd.exe -s $(DEF_OS) -Wall

#-----------------------------------------------------
# Distribution
#-----------------------------------------------------

ifeq ($(SYSTEM), unix)
dist :	dist_src dist_bin_unix
else
dist :	dist_src dist_bin_dos dist_bin_win32
endif

dist_src :
	@echo Creating Dist/meka-src.zip
	cd ..
	-$(MKDIR) Dist
	-$(RM) srcs/buildupd.exe
	zip -9 Dist/meka-src.zip meka.blt meka.dat meka.inp meka.msg meka.nam meka.pat meka.thm
	zip -9 Dist/meka-src.zip *.cfg *.txt icons.zip audiow32.dll setup.bat
	zip -9 -r Dist/meka-src.zip data/*.*
	zip -9 -r Dist/meka-src.zip libs/*.*
	zip -9 -D Dist/meka-src.zip srcs/*.*
	zip -9 -r Dist/meka-src.zip srcs/docs/*.*
	zip -9 -r Dist/meka-src.zip srcs/libaddon/*.*
	zip -9 -r Dist/meka-src.zip srcs/m6502/*.*
	zip -9 -r Dist/meka-src.zip srcs/osd/*.h srcs/osd/*.c
	zip -9 -r Dist/meka-src.zip srcs/sound/*.h srcs/sound/*.c
	zip -9 -r Dist/meka-src.zip srcs/sound/emu2413/*.h srcs/sound/emu2413/*.c
	zip -9 -r Dist/meka-src.zip srcs/tools/*.h srcs/tools/*.c
	zip -9 -r Dist/meka-src.zip srcs/z80marat/*.*
	@echo Deleting VisualStudio temporary files, be sure that VisualStudio is closed.
	-$(RM) -f srcs/MsVc/*.ncb
	zip -9 -D Dist/meka-src.zip srcs/MsVc/*.*
#	zip -9 -r Dist/meka-src.zip srcs/mithril/*.*
#	zip -9 -r Dist/meka-src.zip srcs/z80mame/*.*
#	zip -9 -r Dist/meka-src.zip srcs/z80mk/*.*
#	zip -9 -r Dist/meka-src.zip srcs/z80raze/*.*
	@echo Done!
	
dist_bin_dos :
	@echo Creating Dist/meka.zip
	cd ..
	-$(MKDIR) Dist
	@echo Stripping and compressing
	-strip meka.exe
	-upx -9 meka.exe
	@echo Packaging
	zip -9 Dist/meka.zip meka.blt meka.cfg meka.dat meka.exe meka.inp meka.msg meka.nam meka.pat meka.thm
	zip -9 Dist/meka.zip icons.zip
	zip -9 Dist/meka.zip meka.txt changes.txt compat.txt debugger.txt multi.txt tech.txt 
	zip -9 Dist/meka.zip -r Themes/*.png Themes/Credits.txt
	@echo Done!
	@echo -- Check MEKA.BLT ! --
	@echo -- Check MEKA.INP ! Joypad auto, on --

dist_bin_win32 :
	@echo Creating Dist/mekaw.zip
	cd ..
	-$(MKDIR) Dist
	@echo Compressing
	-upx -9 mekaw.exe
	@echo Packaging
	zip -9 Dist/mekaw.zip meka.blt meka.dat mekaw.cfg mekaw.exe meka.inp meka.msg meka.nam meka.pat meka.thm
	zip -9 Dist/mekaw.zip icons.zip audiow32.dll setup.bat
	zip -9 Dist/mekaw.zip meka.txt mekaw.txt changes.txt compat.txt debugger.txt multi.txt tech.txt
	zip -9 Dist/mekaw.zip -r Themes/*.png Themes/Credits.txt
	@echo Done!
	@echo -- Check MEKA.BLT ! --
	@echo -- Check MEKA.INP ! Joypad auto, on --

# Not sure if this still work
ifeq ($(SYSTEM), unix)
dist_bin_unix :
	@echo Creating Dist/mekanix.tgz
	@echo Compressing
	-upx -9 meka
	@echo Packaging
	-mkdir ./Dist/
	-mkdir ./Dist/mekanix/
	cp meka meka.blt meka.cfg meka.dat meka.inp meka.msg meka.nam meka.pat meka.thm icons.zip ./mekanix/
	cp meka.txt mekanix.txt changes.txt compat.txt debugger.txt multi.txt tech.txt ./mekanix/
	cp -R Themes ./mekanix/
	tar cvzf Dist/mekanix.tgz ./mekanix
	rm -fR ./mekanix/
	@echo Done!
	@echo -- Check MEKA.BLT ! --
	@echo -- Check MEKA.INP ! Joypad auto, on --
endif

# Cleaning rules -------------------------------------
clean :		clean_backup
	$(RM) -f buildupd.exe
	$(RM) -f $(OD)/*.o
	$(RM) -f $(OD)/osd/*.o
	$(RM) -f $(OD)/tools/*.o
	$(RM) -f $(OD)/sound/*.o
	$(RM) -f $(OD)/sound/emu2413/*.o
	$(RM) -f $(OD)/libaddon/png/*.o
	$(RM) -f $(OD)/libaddon/zip/*.o

clean_all :	clean
	$(RM) -f $(OD)/*.a
	$(RM) -f MsVc/*.ncb

clean_backup :
	$(RM) -f *~ *.c~ *.h~
	$(RM) -f osd/*.c~ osd/*.h~
	$(RM) -f tools/*.c~ tools/*.h~
	$(RM) -f sound/*.c~ sound/*.h~
	$(RM) -f sound/emu2413/*.c~ sound/emu2413/*.h~
	$(RM) -f libaddon/png/*.c~ libaddon/png/*.h~
	$(RM) -f libaddon/zip/*.c~ libaddon/zip/*.h~

clean_sound :
	$(RM) -f $(OD)/sound/*.o

# Directories
makedir: $(OD) $(OD)/osd $(OD)/tools $(OD)/sound $(OD)/sound/emu2413 $(OD)/libaddon $(OD)/libaddon/png $(OD)/libaddon/zip
$(OD):
	-$(MKDIR) $(OD)
$(OD)/osd:
	-$(MKDIR) $(OD)/osd
$(OD)/tools:
	-$(MKDIR) $(OD)/tools
$(OD)/sound:
	-$(MKDIR) $(OD)/sound
$(OD)/sound/emu2413:
	-$(MKDIR) $(OD)/sound/emu2413
$(OD)/libaddon:
	-$(MKDIR) $(OD)/libaddon
$(OD)/libaddon/png:
	-$(MKDIR) $(OD)/libaddon/png
$(OD)/libaddon/zip:
	-$(MKDIR) $(OD)/libaddon/zip

#------------------------------------------------------
# EOF
#------------------------------------------------------
