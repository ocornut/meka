@echo off
@rem MY OLD OBSOLETE CODING FRONTEND FOR MS-DOS

set project=MEKA

rem set game=games\wboy3.sms
rem set game=games\ps1-j.sms
rem set game=games\rambo3.sms /LOG log2 /LOAD 1
set game=games\psychofx.sms
rem set game=e:\roms\sms\rtype.sms
rem set game=games\ggarden.sg
rem set game=games\e-119.sc
goto menu

:url
echo.
call url /gofav 5
cd c:\omar\sms\meka
goto menu

:menu
rem echo.
rem echo   Source: %project%.c
rem echo   Executable: %project%.exe
rem echo   Jeu: %game%
echo ÚÄÄ-Ä--úúMúEúKúAúú-Ä-ÄÄ¿
echo ³ L. Launch Executable ³
echo ³ C. Compile Program   ³
echo ³ E. Edit Sources      ³
echo ³ R. Create Releases   ³
echo ³ U. Launch URL        ³
echo ³ S. Launch Symify     ³
echo ³ Q. Quit              ³
echo ÀÄÄ--ú             ú-ÄÄÙ
choice /c:LCERUSQ /n
if errorlevel 7 goto save
if errorlevel 6 goto symify
if errorlevel 5 goto url
if errorlevel 4 goto release
if errorlevel 3 goto edit
if errorlevel 2 goto compile
if errorlevel 1 goto run

:run
echo.
%project%.exe %game%
goto menu

:compile
echo.
echo Compiling..
make
goto menu

:symify
symify %PROJECT%.exe
pause >nul
goto menu

:release
echo.
echo Create [B]inaries or [S]ources package ?
echo       ([W]indow Binaries)
echo       ([U]nix Binaries)
choice /c:bwusq /n
if errorlevel 5 goto save
if errorlevel 4 goto r_src
if errorlevel 3 goto r_bin_unix
if errorlevel 2 goto r_bin_win
if errorlevel 1 goto r_bin
goto release

:r_bin
echo.
echo Creating REL\ ..
mkdir rel
echo Creating REL\MEKA.ZIP ..
echo -Stripping
call strip meka.exe
echo -Compressing
call upx -9 meka.exe
rem echo -Converting to COFF
rem call exe2coff meka.exe
rem echo -Stripping
rem call strip meka.
rem echo -Merging with DPMI server
rem copy /b skydpmi.exe+meka. meka.exe
rem echo -Deleting temporary file
rem del meka.
echo -Packaging
call zip rel\meka.zip meka.blt meka.cfg meka.dat meka.exe meka.inp meka.msg meka.nam meka.pat meka.thm meka.txt changes.txt compat.txt  multi.txt tech.txt icons.zip
echo -Done
echo.
echo *Check MEKA.BLT*
echo *Check MEKA.INP: joypad auto, on *
echo .
goto menu

:r_bin_win
echo Creating REL\MEKAW.ZIP ..
echo -Compressing
call upx -9 mekaw.exe
echo -Packaging
call zip rel\mekaw.zip meka.blt mekaw.cfg meka.dat mekaw.exe meka.inp meka.msg meka.nam meka.pat meka.thm meka.txt mekaw.txt changes.txt compat.txt multi.txt tech.txt icons.zip *.dll
echo -Done
echo .
goto menu

:r_bin_unix
echo Creating REL\MEKANIX.ZIP ..
echo -Getting files
copy mekanix\meka.exe .
copy mekanix\meka.cfg .
echo -Packaging
call zip rel\mekanix.zip meka.blt meka.cfg meka.dat meka.exe meka.inp meka.msg meka.nam meka.pat meka.thm meka.txt mekanix.txt changes.txt compat.txt multi.txt tech.txt icons.zip
echo -Done
echo .
goto menu

:r_src
echo.
echo Creating REL\ ..
mkdir rel
echo Creating REL\MEKA-SRC.ZIP ..
call zip rel\meka-src.zip *.c *.h *.s *.asm meka.blt meka.dat meka.inp meka.msg meka.nam meka.pat meka.thm mekaw.ico mekaw.res makefile
call zip rel\meka-src.zip *.txt todo icons.zip code.bat
call zip -r rel\meka-src.zip m6502\*.h m6502\*.c
rem call zip -r rel\meka-src.zip mithril\makefile mithril\*.h mithril\*.c
call zip -r rel\meka-src.zip sound\*.h sound\*.c
call zip -r rel\meka-src.zip sound\emu2413\*.h sound\emu2413\*.c
call zip -r rel\meka-src.zip tools\*.h tools\*.c
call zip -r rel\meka-src.zip docs\*.*
call zip -r rel\meka-src.zip MsVc\*.*
call zip -r rel\meka-src.zip zip\*.h zip\*.c
call zip -r rel\meka-src.zip z80marat\*.*
rem call zip -r rel\meka-src.zip z80raze\makefile z80raze\*.c z80raze\*.h z80raze\*.inc z80raze\*.asm z80raze\*.reg z80raze\*.txt
echo.
goto menu

:edit
echo.
call box
goto menu

:save
echo.
if not exist %project%.c goto fin
echo Saving main sources..
copy %project%.sa1 %project%.sa2 >nul
copy %project%.sav %project%.sa1 >nul
copy %project%.c %project%.sav >nul

:fin
set project=
set game=
echo.
