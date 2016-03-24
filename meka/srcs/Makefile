#------------------------------------------------------
# MEKA - Makefile
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
# TODO: Add command-line parameter to select this
#------------------------------------------------------

# code found on:
# http://stackoverflow.com/questions/714100/os-detecting-makefile
# Did not try to detect DOS, maybe this link can help:
# http://www.opensource.apple.com/source/X11proto/X11proto-57/freetype/freetype-2.4.4/builds/dos/detect.mk
ifeq ($(OS),Windows_NT)
    SYSTEM = win32
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        SYSTEM = unix
    endif
    ifeq ($(UNAME_S),Darwin)
        SYSTEM = macosx
    endif
endif

#------------------------------------------------------
BUILD = release
# BUILD = debug
#------------------------------------------------------
OBJCFLAGS = ""

#---[ DOS/DJGPP ]--------------------------------------
ifeq ($(SYSTEM), dos)
#--- Executable name
EXE     = ../meka.exe
#--- Output directory
OD		= obj
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
DEF_OS  = -DARCH_DOS
INC_OS  = -I$(LIBPNGDIR) -I$(ZLIBDIR)
#--- Libraries
LIB_OS  = -lm -L$(LIBPNGDIR)
ALL_FILES=*.*
endif

#---[ UNIX/GCC ]---------------------------------------
ifeq ($(SYSTEM), unix)
#--- Executable name
EXE     = ../meka
#--- Output directory
OD      = obj
#--- Compilation
CC      = g++
CC_OUT  = -o
LINKER  = g++
ASM     = nasm
OTYPE   = elf
#--- Tools
RM      = rm
MV      = mv
MKDIR   = mkdir
#--- Definitions
DEF_OS  = -DARCH_UNIX -DASM_SYMBOLS_REQUIRE_UNDERSCORE
INC_OS  = -Ilibs -I../include -I/usr/include/allegro5
#--- Libraries
LIB_OS  = 
ALL_FILES=*
endif

#---[ MACOSX/Clang ]---------------------------------------
ifeq ($(SYSTEM), macosx)
#--- Executable name
EXE     = ../meka
#--- Output directory
OD      = obj
#--- Compilation
CC      = clang++
CC_OUT  = -o
LINKER  = clang++
ASM     = nasm
OTYPE   = macho
#--- Tools
RM      = rm
MV      = mv
MKDIR   = mkdir
#--- Definitions
DEF_OS  = -DARCH_MACOSX -DUNIX
INC_OS  = -Ilibs -I../include -I/usr/local/include
#--- Libraries
LIB_OS = -l freetype -framework OpenGL -framework CoreFoundation -framework OpenAL -framework AudioToolbox -framework AppKit -framework IOKit -framework Foundation
LIB_PATHS = -L/usr/local/lib
ALL_FILES=*.*
#--- Platform specific object
OBJ_PLATFORM = $(OD)/platform/macosx/osxhelpers.o
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
DEF_OS  = -DARCH_WIN32
INC_OS  = -I$(LIBPNGDIR) -I$(ZLIBDIR)

#--- Librairies
LIB_OS          = kernel32.lib user32.lib gdi32.lib comdlg32.lib ole32.lib ddraw.lib winmm.lib dsound.lib dinput.lib dxguid.lib
LIB_PATHS       = -libpath:$(MSVCDIR)/lib/ -libpath:$(MSVCDIR)/PlatformSDK/lib/ -libpath:$(DXSDKDIR)/lib

#--- Ressources
RESSOURCES = mekaw.res

ALL_FILES=*.*
endif

#-----------------------------------------------------
# Option : Sound emulation and output
#-----------------------------------------------------
OBJ_SOUND = $(OD)/sound/sound.o $(OD)/sound/s_misc.o $(OD)/sound/sound_logging.o $(OD)/sound/psg.o $(OD)/sound/fmeditor.o $(OD)/sound/wav.o $(OD)/sound/vgm.o $(OD)/sound/fmunit.o
OBJ_SOUND += $(OD)/sound/emu2413/emu2413.o $(OD)/sound/emu2413/mekaintf.o

#-----------------------------------------------------
# Option : ZIP support
#-----------------------------------------------------
OBJ_ZIP = $(OD)/unzip.o
ifneq ($(SYSTEM), win32)
LIB_ZIP = -lz
else
LIB_ZIP = $(ZLIBDIR)/zlibs.lib
endif
#-----------------------------------------------------

#-----------------------------------------------------
# Option : PNG Support
#-----------------------------------------------------
OBJ_PNG = 
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
LIB_ALLEG = `pkg-config --cflags --libs allegro-5.0 allegro_image-5.0 allegro_audio-5.0 allegro_font-5.0 allegro_primitives-5.0 allegro_ttf-5.0`
endif

ifeq ($(SYSTEM), macosx)
LIB_ALLEG = -lallegro -lallegro_font -lallegro_audio -lallegro_primitives -lallegro_main -lallegro_image -lallegro_ttf
endif

ifeq ($(SYSTEM), win32)
LIB_ALLEG = alleg_s.lib
endif

# Full library list
ifeq ($(SYSTEM), win32)
LIB = $(LIB_PATHS) $(RESSOURCES)
LIB += $(LIB_ALLEG) $(LIB_SOUND) $(LIB_ZIP) $(LIB_OS) -nodefaultlib:msvcrt
else
LIB = $(LIB_PATHS) $(LIB_ALLEG) $(LIB_SOUND) $(LIB_OS) $(LIB_PNG) $(LIB_ZIP)
endif

#-----------------------------------------------------
# Objects to create and link
#-----------------------------------------------------

OBJ_GUI   = $(OD)/gui.o $(OD)/g_action.o $(OD)/g_box.o $(OD)/g_menu.o $(OD)/g_menu_i.o $(OD)/g_menu_t.o $(OD)/g_mouse.o $(OD)/g_init.o $(OD)/g_tools.o $(OD)/g_widget.o $(OD)/g_update.o $(OD)/desktop.o $(OD)/skin.o $(OD)/skin_bg.o $(OD)/skin_fx.o
OBJ_GAPPS = $(OD)/app_cheatfinder.o $(OD)/app_filebrowser.o $(OD)/app_game.o $(OD)/app_options.o $(OD)/textbox.o $(OD)/app_mapview.o $(OD)/app_tileview.o $(OD)/app_textview.o $(OD)/app_techinfo.o $(OD)/app_memview.o $(OD)/app_palview.o $(OD)/app_about.o
OBJ_EMU   = $(OD)/mainloop.o $(OD)/ioports.o $(OD)/commport.o $(OD)/vmachine.o $(OD)/bios.o $(OD)/country.o $(OD)/bmemory.o $(OD)/mappers.o $(OD)/eeprom.o $(OD)/beam.o $(OD)/tvtype.o
OBJ_INP   = $(OD)/inputs.o $(OD)/inputs_c.o $(OD)/inputs_f.o $(OD)/inputs_i.o $(OD)/inputs_t.o $(OD)/inputs_u.o $(OD)/lightgun.o $(OD)/periph.o $(OD)/rapidfir.o $(OD)/tvoekaki.o $(OD)/glasses.o $(OD)/sk1100.o
OBJ_FEAT  = $(OD)/checksum.o $(OD)/db.o $(OD)/vlfn.o $(OD)/patch.o $(OD)/saves.o
OBJ_CFG   = $(OD)/config.o
OBJ_MISC  = $(OD)/allegro4to5.o	 $(OD)/misc.o $(OD)/build.o $(OD)/fonts.o $(OD)/file.o $(OD)/data.o $(OD)/tools.o $(OD)/keyinfo.o $(OD)/drivers.o $(OD)/message.o $(OD)/capture.o $(OD)/errors.o $(OD)/sdsc.o $(OD)/setup.o $(OD)/libmisc.o $(OD)/libparse.o
#OBJ_BLIT  = $(OD)/blit.o $(OD)/blitintf.o $(OD)/hq2x.o $(OD)/hq2x16.o $(OD)/hq2x32.o
OBJ_BLIT  = $(OD)/blit.o $(OD)/blitintf.o $(OD)/hq2x.o
OBJ_CPU   = $(OD)/machine.o $(OD)/cpu.o
OBJ_VIDEO = $(OD)/video.o $(OD)/video_m2.o $(OD)/video_m5.o $(OD)/video_c.o $(OD)/vdp.o $(OD)/palette.o $(OD)/effects.o $(OD)/fskipper.o
OBJ_MACH  = $(OD)/sg1ksc3k.o $(OD)/sf7000.o $(OD)/coleco.o $(OD)/fdc765.o

#-----------------------------------------------------
# Includes directory
#-----------------------------------------------------
INCLUDES = -I. $(INC_OS)

#-----------------------------------------------------
# Preprocessor Definitions
#-----------------------------------------------------
DEFINES = $(DEF_OS)
# DEFINES += -DDEBUG_WHOLE
# DEFINES += -DDEBUG_IO
# DEFINES += -DDEBUG_PAGES
# DEFINES += -DDEBUG_MEM
# DEFINES += -DDEBUG_VDP
# DEFINES += -DDEBUG_PALETTE

#-----------------------------------------------------
# Option : Debugger
#-----------------------------------------------------
OBJ_CPU += $(OD)/debug.o $(OD)/debugger.o $(OD)/debughelper.o $(OD)/datadump.o

#-----------------------------------------------------
# Compiler Flags
#-----------------------------------------------------
ifeq ($(SYSTEM), win32)
# Win32
# (Note:TARGET unsupported for Win32 makefile compilation)
CFLAGS = -W2 -Gd -Ox -GB -MT
else

# UNIX/MS-DOS/MAC
ifndef CFLAGS

# RELEASE build
ifeq ($(BUILD), release)
CFLAGS = -Wall -O3 -ffast-math -fno-strength-reduce -funroll-all-loops -fomit-frame-pointer -x c++
endif

# DEBUG build
ifeq ($(BUILD), debug)
CFLAGS = -Wall -g -ggdb
endif

endif

endif

CFLAGS += $(INCLUDES) $(DEFINES)

#-----------------------------------------------------
# Z80 CPU Cores
#-----------------------------------------------------

OBJ_CPU += $(OD)/z80marat.a

#-----------------------------------------------------
# Complete Objects List
#-----------------------------------------------------
OBJ_MEKA = $(OD)/meka.o $(OBJ_CPU) $(OBJ_VIDEO) $(OBJ_EMU) $(OBJ_INP) $(OBJ_FEAT) $(OBJ_MISC) $(OBJ_CFG) $(OBJ_GUI) $(OBJ_GAPPS) $(OBJ_SOUND) $(OBJ_MACH) $(OBJ_ZIP) $(OBJ_PNG) $(OBJ_BLIT) $(OBJ_TOOLS) $(OBJ_LIBS) $(OBJ_PLATFORM)

#-----------------------------------------------------
# Linking Rule
#-----------------------------------------------------
exe	: makedir $(EXE)

$(EXE)	: $(OBJ_MEKA)
ifneq ($(SYSTEM), win32)
	$(LINKER) -o $@ $(OBJ_MEKA) $(LIB)
else
	$(LINKER) -nologo -release -subsystem:windows -out:$@ -section:.text,erw -section:.data,erw -section:.bss,erw $(OBJ_MEKA) $(LIB)
endif

#-----------------------------------------------------
# Compiling Rules
#-----------------------------------------------------

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
$(OD)/z80marat.a : z80marat/Z80.c z80marat/Z80.h z80marat/Codes.h z80marat/CodesED.h z80marat/CodesCB.h z80marat/CodesXX.h z80marat/Tables.h z80marat/CodesXCB.h z80marat/Debug.c z80marat/Z80Call.c z80marat/Z80DebugHelpers.cpp
	$(CC) $(CFLAGS) -c z80marat/Z80.c $(CC_OUT)$@

$(OD)/debug.o : z80marat/Debug.c z80marat/Z80.h
	$(CC) $(CFLAGS) -c z80marat/Debug.c $(CC_OUT)$@

$(OD)/debughelper.o : z80marat/Z80DebugHelpers.cpp z80marat/Z80.h
	$(CC) $(CFLAGS) -c z80marat/Z80DebugHelpers.cpp $(CC_OUT)$@ 

# Generic Rules

$(OD)/%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< $(CC_OUT)$@

$(OD)/%.o : %.asm
	$(ASM) $(DEF_OS) -f $(OTYPE) $< -o $@

$(OD)/%.a : %.asm
	$(ASM) $(DEF_OS)-f $(OTYPE) $< -o $@

$(OD)/sound/%.o : sound/%.c sound/%.h
	$(CC) $(CFLAGS) -c $< $(CC_OUT)$@

$(OD)/platform/macosx/%.o : platform/macosx/%.mm platform/macosx/%.h
	$(CC) $(OBJCFLAGS) -c $< $(CC_OUT)$@

#-----------------------------------------------------
# Help
#-----------------------------------------------------

help :
	@echo "Rules:"
	@echo "  exe             - Build executable, independent of platform"
	@echo "  dist            - Build sources and binary distribution"
	@echo "  dist_src        - Build sources distribution"
	@echo "  dist_bin_dos    - Build binary DOS distribution"
	@echo "  dist_bin_win32  - Build binary Win32 distribution"
	@echo "  dist_bin_unix   - Build binary UNIX distribution"
	@echo "  clean           - Clean most object files (excluding z80 stuff, etc.)"
	@echo "  clean_all       - Clean object files"
	@echo "  makedir         - Create output directories"

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
# 	This change in directories does not work in linux.
# 	Changed all the relative paths below to fix it.
# 	Another aproach is:
# 	http://stackoverflow.com/questions/1789594/how-to-write-cd-command-in-makefile
# 	cd ..
	-$(MKDIR) ../Dist
	zip -9 ../Dist/meka-src.zip ../meka.blt ../meka.dat ../meka.inp ../meka.msg ../meka.nam ../meka.pat ../meka.thm
	zip -9 ../Dist/meka-src.zip ../*.cfg ../*.txt ../icons.zip ../setup.bat
	zip -9 -r ../Dist/meka-src.zip ../Data/$(ALL_FILES)
	zip -9 -r ../Dist/meka-src.zip ../Themes/$(ALL_FILES)
	zip -9 -r ../Dist/meka-src.zip ../libs/$(ALL_FILES)
	zip -9 -D ../Dist/meka-src.zip ../srcs/$(ALL_FILES)
	zip -9 -r ../Dist/meka-src.zip ../srcs/datasrc/$(ALL_FILES)
# 	I dont have this folder on linux
	-zip -9 -r ../Dist/meka-src.zip ../srcs/docs/$(ALL_FILES)
	zip -9 -r ../Dist/meka-src.zip ../srcs/sound/*.h ../srcs/sound/*.c
	zip -9 -r ../Dist/meka-src.zip ../srcs/sound/emu2413/*.h ../srcs/sound/emu2413/*.c
	zip -9 -r ../Dist/meka-src.zip ../srcs/z80marat/$(ALL_FILES)
	@echo Deleting VisualStudio temporary files, be sure that VisualStudio is closed.
	-$(RM) -f srcs/MsVc/*.ncb
# 	I dont have this folder on linux
	-zip -9 -D ../Dist/meka-src.zip ../srcs/MsVc/$(ALL_FILES)
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
	zip -9 Dist/meka.zip -r Data/*.*
	zip -9 Dist/meka.zip -r Themes/*.png Themes/README.txt
	@echo Done!
	@echo -- Check MEKA.BLT ! --
	@echo -- Check MEKA.INP ! Joypad auto, on --

dist_bin_win32 :
	@echo Creating Dist/mekaw.zip
	cd ..
	-$(MKDIR) Dist
	@echo Compressing
	-libs/dist_win/upx.exe -9 mekaw.exe
	@echo Packaging
	libs/dist_win/zip.exe -9 Dist/mekaw.zip meka.blt meka.dat mekaw.cfg mekaw.exe meka.inp meka.msg meka.nam meka.pat meka.thm
	libs/dist_win/zip.exe -9 Dist/mekaw.zip icons.zip setup.bat
	libs/dist_win/zip.exe -9 Dist/mekaw.zip meka.txt changes.txt compat.txt debugger.txt multi.txt tech.txt
	libs/dist_win/zip.exe -9 Dist/mekaw.zip -r Themes/*.png Themes/Credits.txt
	libs/dist_win/zip.exe -9 Dist/mekaw.zip -r Data/*.*
	@echo Done!
	@echo -- Check MEKA.BLT ! --
	@echo -- Check MEKA.INP ! Joypad auto, on --

# Not sure if this still work
ifeq ($(SYSTEM), unix)
dist_bin_unix :
	@echo Creating Dist/mekanix.tgz
	@echo Compressing
	-upx -9 ../meka
	@echo Packaging
	-mkdir ../Dist/
	-mkdir ../Dist/mekanix/
	cp ../meka ../meka.blt ../meka.cfg ../meka.dat ../meka.inp ../meka.msg ../meka.nam ../meka.pat ../meka.thm ../icons.zip ../Dist/mekanix/
	cp ../meka.txt ../changes.txt ../compat.txt ../debugger.txt ../multi.txt ../tech.txt ../Dist/mekanix/
	cp -R ../Themes ../Dist/mekanix/
	cp -R ../Data ../Dist/mekanix/
	tar cvzf ../Dist/mekanix.tgz ../Dist/mekanix
	rm -fR ../Dist/mekanix/
	@echo Done!
	@echo -- Check MEKA.BLT ! --
	@echo -- Check MEKA.INP ! Joypad auto, on --
endif

# Cleaning rules -------------------------------------
clean :		clean_backup
	$(RM) -f $(OD)/*.o
	$(RM) -f $(OD)/sound/*.o
	$(RM) -f $(OD)/sound/emu2413/*.o

clean_all :	clean
	$(RM) -f $(OD)/*.a
	$(RM) -f MsVc/*.ncb

clean_backup :
	$(RM) -f *~ *.c~ *.h~
	$(RM) -f sound/*.c~ sound/*.h~
	$(RM) -f sound/emu2413/*.c~ sound/emu2413/*.h~

clean_sound :
	$(RM) -f $(OD)/sound/*.o

# Directories
makedir: $(OD) $(OD)/sound $(OD)/sound/emu2413 $(OD)/platform/macosx
$(OD):
	-$(MKDIR) $(OD)
$(OD)/sound:
	-$(MKDIR) $(OD)/sound
$(OD)/sound/emu2413:
	-$(MKDIR) $(OD)/sound/emu2413

$(OD)/platform/macosx:
	@$(MKDIR) -p $(OD)/platform/macosx
#------------------------------------------------------
# EOF
#------------------------------------------------------
