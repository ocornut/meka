;-----------------------------------------------------------------------------
; MEKA - Patches Database
; http://www.smspower.org/meka
;-----------------------------------------------------------------------------
; Patches available in the default MEKA distribution are meant to fix
; or improve game/software behavior. This patching functionnality can 
; also be used as a basic cheating tool.
; For technical help, check http://www.smspower.org/dev
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Syntax:
;   [crc]
;   commands...
;
; CRC can be in any of the following format:
;  - [*]                                Match and apply patch to any file
;  - [xxxxxxxxxxxxxxxx]                 Match meka.nam format CRC
;  - [mekacrc:xxxxxxxxxxxxxxxx]         "
;  - [crc32:xxxxxxxx]                   Match CRC32
;
; Commands:
;  - ROM[<addr>] = <byte> [,<byte>..]   Write given byte(s) at given ROM address, on loading
;  - MEM[<addr>] = <byte> [,<byte>..]   Write given byte(s) at given memory map address, every 1/60th second 
;
; In memory map, RAM range is C000-DFFF.
;
; All values are in hexadecimal base.
;
; An ACTION REPLAY code can be easily converted in MEKA patching format:
;     00xx-xxyy -> MEM[xxxx] = yy
; eg: 00CF-4A02 -> MEM[CF4A] = 02
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Generic, pause-button hack [SMS]
;-----------------------------------------------------------------------------
; Can be useful for VGM logging purpose. Read instruction at:
;   http://www.smspower.org/music/tools/pausehack/index.shtml
; Author: Maxim
;-----------------------------------------------------------------------------
;[*]
;ROM[0066] = FB, 18, FE
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Wonder Boy III: The Dragon's Trap [SMS]
;-----------------------------------------------------------------------------
; Patch ROM to enable FM Unit support in European/US mode.
; The non-patched game uses the FM Unit only in Japanese mode.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 9C7847B5CB3F875F]
ROM[025B] = 00, 00
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Monica em: O Resgate [SMS]
;-----------------------------------------------------------------------------
; Patch ROM to enable FM Unit support in European/US mode.
; The non-patched game uses the FM Unit only in Japanese mode.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 0E91C33709612BD2]
ROM[025B] = 00, 00
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; American Pro Football [SMS]
;-----------------------------------------------------------------------------
; Fix a bug preventing the game to function if FM Unit is detected.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: F5E365059B891882]
ROM[05A7] = AF, 00
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Walter Payton Football [SMS]
; Fix a bug preventing the game to function if FM Unit is detected.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 2893A79D37A62301]
ROM[0592] = AF, 00
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Sports Pad Soccer [SMS]
;-----------------------------------------------------------------------------
; Fix a bug preventing the game to function if FM Unit is not detected.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 6333A82396B2CF88]
ROM[0173] = 0C
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Wanted [SMS]
;-----------------------------------------------------------------------------
; Fix a bug preventing the game to function if FM Unit is detected.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 1E9957BF6F2A4B4F]
ROM[045D] = 00, 00
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Ninja Gaiden [GG]
;-----------------------------------------------------------------------------
; Sega logo patch (the logo was altered by pirates in this version)
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 4B1A5BE482A63301]
ROM[8004] = FF
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Sega Basic Level 3 1.0 (Export) [SC-3000]
;-----------------------------------------------------------------------------
; This dump has byte 0 incorrect. Patch it.
; Author: Ricardo Bittencourt
;-----------------------------------------------------------------------------
[mekacrc: 242CE122F9157C23]
ROM[0000] = C3
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Sega Basic Level 3 1.1 (Japanese) [SC-3000]
;-----------------------------------------------------------------------------
; Hardware check patch
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 0261FB18D318752A]
ROM[689c] = 00, 00, 00
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Music [BAD] [SC-3000]
;-----------------------------------------------------------------------------
; This dump has byte 0 incorrect. Patch it.
; Author: Ricardo Bittencourt
;-----------------------------------------------------------------------------
[crc32: b67ea1c4]
ROM[0000] = C3
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; 20 em 1 [SMS]
;-----------------------------------------------------------------------------
; Disable the first controller port read, done outside of a loop.
; Inputs are updated by interrupt, and at the time the value was loaded,
; interrupts did not happens. It is very tricky and probably due to bad 
; programming and timing issues, but I don't know why no emulator seems 
; to do it well.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 02079D9D9AF6B27B]
ROM[0FB5] = DD, 4E, FF
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Alex Kidd in Miracle World BIOS [SMS]
;-----------------------------------------------------------------------------
; Allows the BIOS dump to run as a cartridge.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: E7605A0A2D3939B6]
ROM[17EDE] = 00, 00, 00
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Hang On / Safari Hunt BIOS [SMS]
;-----------------------------------------------------------------------------
; Allows the BIOS dump to run as a cartridge.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: E48EA39C2A04D150]
ROM[1C12F] = 00, 00, 00
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Sonic the Hedgehog BIOS [SMS]
; Allows the BIOS dump to run as a cartridge.
; Author: Omar Cornut / Bock
;-----------------------------------------------------------------------------
[mekacrc: 21346172A6AAF296]
ROM[26FA] = 00, 00, 00
;-----------------------------------------------------------------------------
