# Microsoft Developer Studio Project File - Name="Meka" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Meka - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Meka.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Meka.mak" CFG="Meka - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Meka - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Meka - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Meka - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Meka - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".." /I "..\tools" /I "..\sound" /I "d:\djgpp\seal\src" /I "d:\djgpp\zlib113" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "X86_ASM" /D "MEKA_SOUND" /D "MEKA_EAGLE" /D "MEKA_ZIP" /D "MEKA_JOY" /D "MEKA_Z80_DEBUGGER" /D "MARAT_Z80" /YX /FD /GZ /c
# SUBTRACT CPP /u
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib alleg.lib audw32vc.lib zlib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"debug/mekaw.exe" /pdbtype:sept /libpath:"d:/djgpp/seal/lib/win32/" /libpath:"d:/djgpp/zlib113" /section:.text,erw /section:.data,erw /section:.bss,erw
# SUBTRACT LINK32 /verbose /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "Meka - Win32 Release"
# Name "Meka - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Group "PSG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sound\psg.c
# End Source File
# Begin Source File

SOURCE=..\sound\psg.h
# End Source File
# End Group
# Begin Group "Emu2413"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sound\emu2413\2413tone.h
# End Source File
# Begin Source File

SOURCE=..\sound\emu2413\emu2413.c
# End Source File
# Begin Source File

SOURCE=..\sound\emu2413\emu2413.h
# End Source File
# Begin Source File

SOURCE=..\sound\emu2413\emutypes.h
# End Source File
# Begin Source File

SOURCE=..\sound\emu2413\mekaintf.c
# End Source File
# Begin Source File

SOURCE=..\sound\emu2413\mekaintf.h
# End Source File
# End Group
# Begin Group "Ym2413hd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\sound\ym2413hd.c
# End Source File
# Begin Source File

SOURCE=..\sound\ym2413hd.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\sound\deftypes.h
# End Source File
# Begin Source File

SOURCE=..\sound\fmeditor.c
# End Source File
# Begin Source File

SOURCE=..\sound\fmeditor.h
# End Source File
# Begin Source File

SOURCE=..\sound\fmunit.c
# End Source File
# Begin Source File

SOURCE=..\sound\fmunit.h
# End Source File
# Begin Source File

SOURCE=..\sound\s_init.c
# End Source File
# Begin Source File

SOURCE=..\sound\s_log.c
# End Source File
# Begin Source File

SOURCE=..\sound\s_log.h
# End Source File
# Begin Source File

SOURCE=..\sound\s_misc.c
# End Source File
# Begin Source File

SOURCE=..\sound\s_misc.h
# End Source File
# Begin Source File

SOURCE=..\sound\s_opl.c
# End Source File
# Begin Source File

SOURCE=..\sound\s_opl.h
# End Source File
# Begin Source File

SOURCE=..\sound\sasound.c
# End Source File
# Begin Source File

SOURCE=..\sound\sasound.h
# End Source File
# Begin Source File

SOURCE=..\sound\sound.c
# End Source File
# Begin Source File

SOURCE=..\sound\sound.h
# End Source File
# Begin Source File

SOURCE=..\sound\vgm.c
# End Source File
# Begin Source File

SOURCE=..\sound\vgm.h
# End Source File
# Begin Source File

SOURCE=..\sound\wav.c
# End Source File
# Begin Source File

SOURCE=..\sound\wav.h
# End Source File
# End Group
# Begin Group "Tools"

# PROP Default_Filter ""
# Begin Group "Zip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\zip\unzip.c
# End Source File
# Begin Source File

SOURCE=..\zip\unzip.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\tools\liblist.c
# End Source File
# Begin Source File

SOURCE=..\tools\liblist.h
# End Source File
# Begin Source File

SOURCE=..\tools\libmy.c
# End Source File
# Begin Source File

SOURCE=..\tools\libmy.h
# End Source File
# Begin Source File

SOURCE=..\tools\libparse.c
# End Source File
# Begin Source File

SOURCE=..\tools\libparse.h
# End Source File
# Begin Source File

SOURCE=..\tools\tfile.c
# End Source File
# Begin Source File

SOURCE=..\tools\tfile.h
# End Source File
# End Group
# Begin Group "CPU"

# PROP Default_Filter ""
# Begin Group "M6502"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\m6502\codes.h
# End Source File
# Begin Source File

SOURCE=..\m6502\m6502.c
# End Source File
# Begin Source File

SOURCE=..\m6502\m6502.h
# End Source File
# Begin Source File

SOURCE=..\m6502\tables.h
# End Source File
# End Group
# Begin Group "Z80marat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\z80marat\Codes.h
# End Source File
# Begin Source File

SOURCE=..\z80marat\CodesCB.h
# End Source File
# Begin Source File

SOURCE=..\z80marat\CodesED.h
# End Source File
# Begin Source File

SOURCE=..\z80marat\CodesXCB.h
# End Source File
# Begin Source File

SOURCE=..\z80marat\CodesXX.h
# End Source File
# Begin Source File

SOURCE=..\z80marat\Debug.c
# End Source File
# Begin Source File

SOURCE=..\z80marat\Tables.h
# End Source File
# Begin Source File

SOURCE=..\z80marat\Z80.c
# End Source File
# Begin Source File

SOURCE=..\z80marat\Z80.h
# End Source File
# Begin Source File

SOURCE=..\z80marat\Z80Call.c
# End Source File
# End Group
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\about.c
# End Source File
# Begin Source File

SOURCE=..\about.h
# End Source File
# Begin Source File

SOURCE=..\desktop.c
# End Source File
# Begin Source File

SOURCE=..\desktop.h
# End Source File
# Begin Source File

SOURCE=..\g_action.c
# End Source File
# Begin Source File

SOURCE=..\g_action.h
# End Source File
# Begin Source File

SOURCE=..\g_apps.c
# End Source File
# Begin Source File

SOURCE=..\g_apps.h
# End Source File
# Begin Source File

SOURCE=..\g_box.c
# End Source File
# Begin Source File

SOURCE=..\g_box.h
# End Source File
# Begin Source File

SOURCE=..\g_colors.c
# End Source File
# Begin Source File

SOURCE=..\g_colors.h
# End Source File
# Begin Source File

SOURCE=..\g_emu.c
# End Source File
# Begin Source File

SOURCE=..\g_emu.h
# End Source File
# Begin Source File

SOURCE=..\g_file.c
# End Source File
# Begin Source File

SOURCE=..\g_file.h
# End Source File
# Begin Source File

SOURCE=..\g_init.c
# End Source File
# Begin Source File

SOURCE=..\g_init.h
# End Source File
# Begin Source File

SOURCE=..\g_menu.c
# End Source File
# Begin Source File

SOURCE=..\g_menu.h
# End Source File
# Begin Source File

SOURCE=..\g_menu_i.c
# End Source File
# Begin Source File

SOURCE=..\g_menu_i.h
# End Source File
# Begin Source File

SOURCE=..\g_menu_t.c
# End Source File
# Begin Source File

SOURCE=..\g_menu_t.h
# End Source File
# Begin Source File

SOURCE=..\g_mouse.c
# End Source File
# Begin Source File

SOURCE=..\g_mouse.h
# End Source File
# Begin Source File

SOURCE=..\g_tools.c
# End Source File
# Begin Source File

SOURCE=..\g_tools.h
# End Source File
# Begin Source File

SOURCE=..\g_update.c
# End Source File
# Begin Source File

SOURCE=..\g_update.h
# End Source File
# Begin Source File

SOURCE=..\g_widget.c
# End Source File
# Begin Source File

SOURCE=..\g_widget.h
# End Source File
# Begin Source File

SOURCE=..\gui.c
# End Source File
# Begin Source File

SOURCE=..\gui.h
# End Source File
# Begin Source File

SOURCE=..\options.c
# End Source File
# Begin Source File

SOURCE=..\options.h
# End Source File
# Begin Source File

SOURCE=..\specials.c

!IF  "$(CFG)" == "Meka - Win32 Release"

!ELSEIF  "$(CFG)" == "Meka - Win32 Debug"

# ADD CPP /I "d:\djgpp\zlib\\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\specials.h
# End Source File
# Begin Source File

SOURCE=..\themes.c
# End Source File
# Begin Source File

SOURCE=..\themes.h
# End Source File
# Begin Source File

SOURCE=..\themes_b.c
# End Source File
# Begin Source File

SOURCE=..\themes_b.h
# End Source File
# End Group
# Begin Group "Video"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\beam.c
# End Source File
# Begin Source File

SOURCE=..\beam.h
# End Source File
# Begin Source File

SOURCE=..\blit.c
# End Source File
# Begin Source File

SOURCE=..\blit.h
# End Source File
# Begin Source File

SOURCE=..\blitintf.c
# End Source File
# Begin Source File

SOURCE=..\blitintf.h
# End Source File
# Begin Source File

SOURCE=..\palette.c
# End Source File
# Begin Source File

SOURCE=..\palette.h
# End Source File
# Begin Source File

SOURCE=..\tileview.c
# End Source File
# Begin Source File

SOURCE=..\tileview.h
# End Source File
# Begin Source File

SOURCE=..\tms_vdp.c
# End Source File
# Begin Source File

SOURCE=..\tms_vdp.h
# End Source File
# Begin Source File

SOURCE=..\video.c
# End Source File
# Begin Source File

SOURCE=..\video.h
# End Source File
# Begin Source File

SOURCE=..\video_c.c
# End Source File
# Begin Source File

SOURCE=..\video_c.h
# End Source File
# Begin Source File

SOURCE=..\video_m2.c
# End Source File
# Begin Source File

SOURCE=..\video_m2.h
# End Source File
# Begin Source File

SOURCE=..\video_m5.c
# End Source File
# Begin Source File

SOURCE=..\video_m5.h
# End Source File
# Begin Source File

SOURCE=..\video_t.c
# End Source File
# Begin Source File

SOURCE=..\video_t.h
# End Source File
# Begin Source File

SOURCE=..\videoasm.asm

!IF  "$(CFG)" == "Meka - Win32 Release"

!ELSEIF  "$(CFG)" == "Meka - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
InputPath=..\videoasm.asm
InputName=videoasm

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	c:/cygwin/bin/nasm.exe -f win32 $(InputPath) -o $(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Peripherals"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\glasses.c
# End Source File
# Begin Source File

SOURCE=..\glasses.h
# End Source File
# Begin Source File

SOURCE=..\keyboard.c
# End Source File
# Begin Source File

SOURCE=..\keyboard.h
# End Source File
# Begin Source File

SOURCE=..\keysname.c
# End Source File
# Begin Source File

SOURCE=..\keysname.h
# End Source File
# Begin Source File

SOURCE=..\lightgun.c
# End Source File
# Begin Source File

SOURCE=..\lightgun.h
# End Source File
# Begin Source File

SOURCE=..\rapidfir.c
# End Source File
# Begin Source File

SOURCE=..\rapidfir.h
# End Source File
# Begin Source File

SOURCE=..\sportpad.c
# End Source File
# Begin Source File

SOURCE=..\sportpad.h
# End Source File
# Begin Source File

SOURCE=..\tvoekaki.c
# End Source File
# Begin Source File

SOURCE=..\tvoekaki.h
# End Source File
# End Group
# Begin Group "Machines"

# PROP Default_Filter ""
# Begin Group "NES"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\nes.c
# End Source File
# Begin Source File

SOURCE=..\nes.h
# End Source File
# Begin Source File

SOURCE=..\nes_maps.c
# End Source File
# Begin Source File

SOURCE=..\nes_maps.h
# End Source File
# Begin Source File

SOURCE=..\nes_pal.h
# End Source File
# Begin Source File

SOURCE=..\nes_ppu.c
# End Source File
# Begin Source File

SOURCE=..\nes_ppu.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\bios.c
# End Source File
# Begin Source File

SOURCE=..\bios.h
# End Source File
# Begin Source File

SOURCE=..\coleco.c
# End Source File
# Begin Source File

SOURCE=..\coleco.h
# End Source File
# Begin Source File

SOURCE=..\drivers.c
# End Source File
# Begin Source File

SOURCE=..\drivers.h
# End Source File
# Begin Source File

SOURCE=..\ioports.c
# End Source File
# Begin Source File

SOURCE=..\ioports.h
# End Source File
# Begin Source File

SOURCE=..\machine.c
# End Source File
# Begin Source File

SOURCE=..\machine.h
# End Source File
# Begin Source File

SOURCE=..\sf7000.c
# End Source File
# Begin Source File

SOURCE=..\sf7000.h
# End Source File
# Begin Source File

SOURCE=..\sg1ksc3k.c
# End Source File
# Begin Source File

SOURCE=..\sg1ksc3k.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\areplay.c
# End Source File
# Begin Source File

SOURCE=..\areplay.h
# End Source File
# Begin Source File

SOURCE=..\bmemory.c
# End Source File
# Begin Source File

SOURCE=..\bmemory.h
# End Source File
# Begin Source File

SOURCE=..\capture.c
# End Source File
# Begin Source File

SOURCE=..\capture.h
# End Source File
# Begin Source File

SOURCE=..\checksum.c
# End Source File
# Begin Source File

SOURCE=..\checksum.h
# End Source File
# Begin Source File

SOURCE=..\clock.c
# End Source File
# Begin Source File

SOURCE=..\clock.h
# End Source File
# Begin Source File

SOURCE=..\commport.c
# End Source File
# Begin Source File

SOURCE=..\commport.h
# End Source File
# Begin Source File

SOURCE=..\config.c
# End Source File
# Begin Source File

SOURCE=..\config.h
# End Source File
# Begin Source File

SOURCE=..\config_j.c
# End Source File
# Begin Source File

SOURCE=..\config_j.h
# End Source File
# Begin Source File

SOURCE=..\config_v.c
# End Source File
# Begin Source File

SOURCE=..\config_v.h
# End Source File
# Begin Source File

SOURCE=..\country.c
# End Source File
# Begin Source File

SOURCE=..\country.h
# End Source File
# Begin Source File

SOURCE=..\cpu.c
# End Source File
# Begin Source File

SOURCE=..\cpu.h
# End Source File
# Begin Source File

SOURCE=..\data.c
# End Source File
# Begin Source File

SOURCE=..\data.h
# End Source File
# Begin Source File

SOURCE=..\datadump.c
# End Source File
# Begin Source File

SOURCE=..\datadump.h
# End Source File
# Begin Source File

SOURCE=..\debugger.c
# End Source File
# Begin Source File

SOURCE=..\debugger.h
# End Source File
# Begin Source File

SOURCE=..\eagle.asm

!IF  "$(CFG)" == "Meka - Win32 Release"

!ELSEIF  "$(CFG)" == "Meka - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
InputPath=..\eagle.asm
InputName=eagle

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	c:/cygwin/bin/nasm.exe -f win32 $(InputPath) -o $(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\eagle.h
# End Source File
# Begin Source File

SOURCE=..\eeprom.c
# End Source File
# Begin Source File

SOURCE=..\eeprom.h
# End Source File
# Begin Source File

SOURCE=..\effects.c
# End Source File
# Begin Source File

SOURCE=..\effects.h
# End Source File
# Begin Source File

SOURCE=..\errors.c
# End Source File
# Begin Source File

SOURCE=..\errors.h
# End Source File
# Begin Source File

SOURCE=..\fdc765.c
# End Source File
# Begin Source File

SOURCE=..\fdc765.h
# End Source File
# Begin Source File

SOURCE=..\file.c
# End Source File
# Begin Source File

SOURCE=..\file.h
# End Source File
# Begin Source File

SOURCE=..\fonts.c
# End Source File
# Begin Source File

SOURCE=..\fonts.h
# End Source File
# Begin Source File

SOURCE=..\fskipper.c
# End Source File
# Begin Source File

SOURCE=..\fskipper.h
# End Source File
# Begin Source File

SOURCE=..\games.c
# End Source File
# Begin Source File

SOURCE=..\games.h
# End Source File
# Begin Source File

SOURCE=..\grab.h
# End Source File
# Begin Source File

SOURCE=..\inputs.c
# End Source File
# Begin Source File

SOURCE=..\inputs.h
# End Source File
# Begin Source File

SOURCE=..\inputs_c.c
# End Source File
# Begin Source File

SOURCE=..\inputs_c.h
# End Source File
# Begin Source File

SOURCE=..\inputs_f.c
# End Source File
# Begin Source File

SOURCE=..\inputs_f.h
# End Source File
# Begin Source File

SOURCE=..\inputs_i.c
# End Source File
# Begin Source File

SOURCE=..\inputs_i.h
# End Source File
# Begin Source File

SOURCE=..\inputs_t.c
# End Source File
# Begin Source File

SOURCE=..\inputs_t.h
# End Source File
# Begin Source File

SOURCE=..\inputs_u.c
# End Source File
# Begin Source File

SOURCE=..\inputs_u.h
# End Source File
# Begin Source File

SOURCE=..\mainloop.c
# End Source File
# Begin Source File

SOURCE=..\mainloop.h
# End Source File
# Begin Source File

SOURCE=..\mappers.c
# End Source File
# Begin Source File

SOURCE=..\mappers.h
# End Source File
# Begin Source File

SOURCE=..\mappersa.asm

!IF  "$(CFG)" == "Meka - Win32 Release"

!ELSEIF  "$(CFG)" == "Meka - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
InputPath=..\mappersa.asm
InputName=mappersa

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	c:/cygwin/bin/nasm.exe -f win32 $(InputPath) -o $(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\meka.c
# End Source File
# Begin Source File

SOURCE=..\meka.h
# End Source File
# Begin Source File

SOURCE=..\memory.c
# End Source File
# Begin Source File

SOURCE=..\memory.h
# End Source File
# Begin Source File

SOURCE=..\message.c
# End Source File
# Begin Source File

SOURCE=..\message.h
# End Source File
# Begin Source File

SOURCE=..\misc.c
# End Source File
# Begin Source File

SOURCE=..\misc.h
# End Source File
# Begin Source File

SOURCE=..\patch.c
# End Source File
# Begin Source File

SOURCE=..\patch.h
# End Source File
# Begin Source File

SOURCE=..\register.c
# End Source File
# Begin Source File

SOURCE=..\register.h
# End Source File
# Begin Source File

SOURCE=..\saves.c
# End Source File
# Begin Source File

SOURCE=..\saves.h
# End Source File
# Begin Source File

SOURCE=..\sdsc.c
# End Source File
# Begin Source File

SOURCE=..\sdsc.h
# End Source File
# Begin Source File

SOURCE=..\shared.h
# End Source File
# Begin Source File

SOURCE=..\system.h
# End Source File
# Begin Source File

SOURCE=..\techinfo.c
# End Source File
# Begin Source File

SOURCE=..\techinfo.h
# End Source File
# Begin Source File

SOURCE=..\textbox.c
# End Source File
# Begin Source File

SOURCE=..\textbox.h
# End Source File
# Begin Source File

SOURCE=..\textview.c
# End Source File
# Begin Source File

SOURCE=..\textview.h
# End Source File
# Begin Source File

SOURCE=..\tools.c
# End Source File
# Begin Source File

SOURCE=..\tools.h
# End Source File
# Begin Source File

SOURCE=..\tools_t.c
# End Source File
# Begin Source File

SOURCE=..\tools_t.h
# End Source File
# Begin Source File

SOURCE=..\tvtype.c
# End Source File
# Begin Source File

SOURCE=..\tvtype.h
# End Source File
# Begin Source File

SOURCE=..\vlfn.c
# End Source File
# Begin Source File

SOURCE=..\vlfn.h
# End Source File
# Begin Source File

SOURCE=..\vmachine.c
# End Source File
# Begin Source File

SOURCE=..\vmachine.h
# End Source File
# Begin Source File

SOURCE=..\vmachpal.h
# End Source File
# End Target
# End Project
