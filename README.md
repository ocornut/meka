MEKA
====

- Homepage: http://www.smspower.org/meka
- Forum: http://www.smspower.org/forums/f7-MEKA

Latest Beta Windows binaries:
- https://github.com/ocornut/meka/releases
- Also may want to follow this thread: http://www.smspower.org/forums/13019

MEKA is a multi-machine emulator. The following machines are supported by MEKA:

- Sega Game 1000 (SG-1000)
- Sega Computer 3000 (SC-3000)
- Sega Super Control Station (SF-7000)
- Sega Mark III (+ FM Unit)
- Sega Master System (SMS)
- Sega Game Gear (GG)
- ColecoVision (COLECO)
- Othello Multivision (OMV)

Along with a wide range of peripherals and exotic games support. 

MEKA should compile for MS-Windows, GNU/Linux and OSX (older versions support MS-DOS).

MEKA was started in 1998 and first released on April 3rd 1999. It was my first major C project, so the codebase is old, ugly and messy! It is still being updated sometimes.

In spite of its old age and very clunky technology, MEKA is among the most exhaustive Sega 8-bit emulator in term of coverage of obscure games, peripherals. And also provide competent debugging and reverse engineering tools. It is still maintained for those purpose but doesn't has much use for the average player. 

Credits
-------
(old pre-github credits from meka.txt)
 
```
Omar Cornut (Bock): Main development, coordination
Marat Fayzullin (RST38H): Zilog Z80 CPU emulation core. Implemented with modifications in MEKA.
Mitsutaka Okazaki: Software Yamaha 2413 emulator (EMU2413).
Maxim: Software PSG emulator.
Hiromitsu Shioya (Hiro-shi): Original sound engine (now obsolete).
Ulrich Cordes: FDC765 emulator for SF-7000. Implemented with modifications by Marc Le Douarain.
Valerie Tching (Kittie): Initial version of the Memory Editor.
Johan Euphrosine (Proppy): GNU/Linux+OSX tweaks.
DJRobX: Blitters and video improvements, HQ2X interface.
Maxim Stephin (MaxSt): HQ2X graphics filter.
Dirk Stevens: Eagle graphics filter.
```

Clone
-----

```
git clone --recursive https://github.com/ocornut/meka.git meka
```

Build
-----

- Windows: Using Visual Studio 2017 (free community edition is ok), open `meka/srcs/projects/msvc/Meka.sln` and build. The project is setup to grab dependencies (Allegro, Nasm) using Nuget.
- Linux: `cd meka/srcs ; make`
- OSX: `cd meka/srcs ; make` or open `meka/srcs/projects/xcode` with xcode (may be not up to date)

This project is looking for Linux and OSX maintainers.

Gallery
-------

![Debugger Screenshot](http://www.smspower.org/forums/files/meka_080_wip_debugger_823.png)

![Debugger Screenshot](http://www.smspower.org/meka/gallery/meka072-wip-sagaia.png)
