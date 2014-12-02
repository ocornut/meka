@echo Creating Dist/mekaw.zip
cd ..
mkdir Dist
@echo Compressing
tools\dist_win\upx.exe -9 mekaw.exe
@echo Packaging
tools\dist_win\zip.exe -9 Dist\mekaw.zip meka.blt meka.dat mekaw.cfg mekaw.exe meka.inp meka.msg meka.nam meka.pat meka.thm
tools\dist_win\zip.exe -9 Dist\mekaw.zip icons.zip setup.bat
tools\dist_win\zip.exe -9 Dist\mekaw.zip meka.txt changes.txt compat.txt debugger.txt multi.txt tech.txt
tools\dist_win\zip.exe -9 Dist\mekaw.zip -r Data\*.*
tools\dist_win\zip.exe -9 Dist\mekaw.zip -r Data\fonts\*.*
tools\dist_win\zip.exe -9 Dist\mekaw.zip -r Themes\*.png Themes\Credits.txt
@echo Done!
@echo -- Check MEKA.BLT ! --
@echo -- Check MEKA.INP ! Joypad auto, on --
pause
