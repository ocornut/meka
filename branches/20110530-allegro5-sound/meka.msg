;-----------------------------------------------------------------------------
; MEKA - Text Messages / Localization
; http://www.smspower.org/meka
;-----------------------------------------------------------------------------
; Please contact me if you have corrections to make or if
; you are willing to translate messages to a new language.
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; History
;----------------------------------------	-------------------------------------
; MEKA 0.73  : Update language:  Dutch
;              Added:            Msg_Menu_Video_Capture
;                                Msg_Menu_Video_Capture_CaptureScreen
;                                Msg_Menu_Video_Capture_CaptureScreenAll
;                                Msg_Menu_Video_Capture_IncludeGUI
;              Updated:          Msg_Options_Capture_Crop_Align
;              Renamed:          Msg_Options_Capture_Automatic_Crop_Align -> Msg_Options_Capture_Crop_Align
;              Removed:          Msg_Menu_Video_Screens*
; MEKA 0.72  : Added:            Msg_Inputs_Config_Peripheral_Click     
;                                Msg_Inputs_Config_Source_Enabled       
;                                Msg_Inputs_Config_Source_Player        
;                                Msg_Inputs_Config_Source_Emulate_Joypad
;                                Msg_Theme_Error_Syntax
;                                Msg_Theme_Error_Attribute_Defined
;                                Msg_Theme_Error_Out_of_Bound
;                                Msg_Theme_Error_Theme_Missing_Data
;                                Msg_Blitters_Error_Incorrect_Value
;                                Msg_Menu_Tools_TilemapViewer
;                                Msg_Menu_Help_Debugger
;                                Msg_Options_Capture_Automatic_Crop_Align
;              Renamed:          Msg_Inputs_Configuration_BoxTitle -> Msg_Inputs_Config_BoxTitle
;                                Msg_Theme_Not_Enough -> Msg_Theme_Error_Not_Enough
;                                Msg_Theme_Missing -> Msg_Theme_Error_Missing_Theme_Name
;                                Msg_Blitters_Not_Enough -> Msg_Blitters_Error_Not_Enough
;                                Msg_Blitters_Not_Found -> Msg_Blitters_Error_Not_Found
;                                Msg_Blitters_Missing -> Msg_Blitters_Error_Missing
;                                Msg_Blitters_Unrecognized -> Msg_Blitters_Error_Unrecognized
;                                MSG_Options_NES_Activated -> MSG_Options_NES_Enable
;              Removed:          Msg_Theme_Unrecognized
;                                Msg_VoiceRecognition_BoxTitle
;                                Msg_VoiceRecognition_Enabled
;                                Msg_VoiceRecognition_Disabled
;                                Msg_Menu_Sound_VoiceRecognition
;              Updated:          Msg_Window_Title
;                                Msg_Inputs_Joy_Init_None
;                                Msg_Inputs_Joy_Init_Found
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; Currently supporting:
;   English
;   French
;   Redneck
;   Dutch
;   Spanish
;   Portuguese
;   Italian
;   German
;   Swedish
;   Danish
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; How does it work ?
;
; - [xxx] define a new language
; - You can add messages for the same language at different places,
;   as long as they are defined after the corresponding [xxx] line.
;   However, for the purpose of easing maintainance, try to stick to
;   same order as the reference language (english).
; - First language (english) is the default language.
;   If another language is missing sentences, it will use the ones
;   from the default language. The default language must NOT be missing
;   ANY sentence! 
; - Accents are written using UTF Latin-Extended codepage. Usually,
;   Windows softwars uses this codepage. 
;   Under MS-DOS, initialization text-mode messages are using ANSI-DOS 
;   codepage. For this reason, accents may not be displayed properly
;   during initialization.
; - '%s' is usually replaced with text and '%d' or '%i' with a number
;   Order of those must NOT be changed. It might be a problem when
;   localizing some sentence. Please contact me if so, I'll try to work
;   on a solution.
; - ';' is the comment character in this file
;   As of yet, you must NOT use a ';' character in a sentence
; - 'WIP' (Work In Progress) on a line will disable warnings and error
;   messages for this language, if it this uncomplete. This is useful
;   for when a language is missing few sentences, in a release version.
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
; The messages listed below are to be given special care when translating,
; because they are to fit in fixed sized regions. If they're causing problem
; with your translation, the best is to keep them in english and report me
; so. Later on I hope to find a solution.
;-----------------------------------------------------------------------------
; Msg_Options_*
; Msg_FileBrowser_Load
; Msg_FileBrowser_Close
; Msg_FileBrowser_LoadNames
; Msg_FileBrowser_ReloadDir
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[English]
; Original text by Omar Cornut (Bock)
; Maintainer: Omar Cornut (omar AT miracleworld.net)
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Welcome to %s (c) %s"
Msg_Window_Title                        = "MEKA - In Pixels We Trust!"
Msg_Quit                                = "Good night, brave warrior. Good night, monster-land...."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "About Meka..."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "by %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Ok                                  = "Ok"
Msg_Failed                              = "Failed!"
Msg_Error_Base                          = "Error: %s"
Msg_Error_Error                         = "Fatal: error getting error type. Something must be wrong."
Msg_Error_Memory                        = "Not enough memory!"
Msg_Error_Param                         = "Parameter \"%s\" is invalid!\n"
Msg_Error_Syntax                        = "Syntax error!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Unable to switch to desired video mode (%dx%d)."
Msg_Error_Video_Mode_Back_To_GUI        = "Switching back to graphical user interface."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "File not found!"
Msg_Error_File_Read                     = "Unable to read from file!"
Msg_Error_File_Empty                    = "File is empty!"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "This version doesn't handle ZIP files!"
Msg_Error_ZIP_Loading                   = "Unable to read ZIP file!"
Msg_Error_ZIP_Internal                  = "Internal error while loading ZIP file!"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Error opening directory!"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Reset the game for the changes to take effect"
Msg_No_ROM                              = "No ROM was loaded!"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Initializing Allegro..."
Msg_Init_GUI                            = "Initializing graphical user interface..."
Msg_Init_Completed                      = "[READY]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Running                       = "Running setup..."
Msg_Setup_Setup                         = "Setup"
Msg_Setup_Soundcard_Select              = "Please select your audio device:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(DirectSound is highly recommended, if available)"
Msg_Setup_SampleRate_Select             = "Sample rate:"
Msg_Setup_Debugger_Enable               = "Enable Z80 debugger and associated features"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Screen captured to %s"
Msg_Capture_Error                       = "Unable to create screen capture!"
Msg_Capture_Error_File                  = "Unable to find valid filename to save screen capture!"

; On-Board Memory ------------------------------------------------------------
Msg_SRAM_Loaded                         = "Loaded SaveRAM from disk (%d Kb)"
Msg_SRAM_Load_Unable                    = "Unable to load SaveRAM from disk"
Msg_SRAM_Wrote                          = "Wrote SaveRAM to disk (%d Kb)"
Msg_SRAM_Write_Unable                   = "Unable to write SaveRAM to disk (%d Kb)"
Msg_93c46_Reset                         = "Resetting 93c46"
Msg_93c46_Loaded                        = "Loaded 93c46 EEPROM data from disk (%d bytes)"
Msg_93c46_Load_Unable                   = "Unable to load 93c46 EEPROM data from disk"
Msg_93c46_Wrote                         = "Wrote 93c46 EEPROM data to disk (%d bytes)"
Msg_93c46_Write_Unable                  = "Unable to write 93c46 EEPROM data to disk (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "TV Type is now %s"
Msg_TVType_Info_Speed                   = "(Original speed is %d Hz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Loading MEKA.BLT (video modes / blitters)... "
Msg_Blitters_Error_Not_Enough           = "Not enough blitters found! Check MEKA.BLT file or try re-installing."
Msg_Blitters_Error_Not_Found            = "Blitter not found!"
Msg_Blitters_Error_Missing              = "On line %d: No blitter name defined for storing value!"
Msg_Blitters_Error_Unrecognized         = "On line %d: Unrecognized variable!"
Msg_Blitters_Error_Incorrect_Value      = "On line %d: Incorrect value!"
Msg_Blitters_Set                        = "Now using \"%s\" fullscreen blitter."

; Nintendo -------------------------------------------------------------------
Msg_NES_Activate                        = "Fat plumber mode activated"
Msg_NES_Sucks                           = "Fatal error: user likes Nintendo!"
Msg_NES_Mapper_Unknown                  = "Unsupported Mapper: %d. Game should not function."
Msg_NES_Deny_Facts                      = "You can't deny facts."

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Initializing debugger... "
Msg_Debug_Welcome                       = "Welcome to Meka Debugger!"
Msg_Debug_Not_Available                 = "Debugger not available in this version!"
Msg_Debug_Brk_Need_Param                = "DEBUGBRK parameter requires an address!"
Msg_Debug_Trap_Read                     = "At PC:%04X - Read from %04X"
Msg_Debug_Trap_Write                    = "At PC:%04X - Write %02X to %04X"
Msg_Debug_Trap_Port_Read                = "At PC:%04X - Port [%02X] Read"
Msg_Debug_Trap_Port_Write               = "At PC:%04X - Port [%02X] Write %02X"
Msg_Debug_Symbols_Loaded                = "Loaded %d symbols from \"%s\"."
Msg_Debug_Symbols_Error                 = "Error loading symbols from \"%s\":"
Msg_Debug_Symbols_Error_Line            = "On line %d: unrecognized syntax."

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Data dumps will now be made in ASCII mode."
Msg_DataDump_Mode_Raw                   = "Data dumps will now be made in Raw mode."
Msg_DataDump_Error                      = "Error creating file to dump %s!"
Msg_DataDump_Error_OB_Memory            = "No on-board memory to dump!"
Msg_DataDump_Error_Palette              = "No palette to dump!"
Msg_DataDump_Error_Sprites              = "No sprites to dump!"
Msg_DataDump_Main                       = "Dumped %s (%d %s) in %s mode"

; Documentation --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Documentation"
Msg_Doc_File_Error                      = "Unable to open documentation!"
Msg_Doc_Enabled                         = "Documentation showing up."
Msg_Doc_Disabled                        = "Documentation is no more."

; Sprite Flickering ----------------------------------------------------------
Msg_Flickering_Auto                     = "Sprite flickering will be handled automatically."
Msg_Flickering_Yes                      = "Sprite flickering emulation enabled."
Msg_Flickering_No                       = "Sprite flickering emulation disabled."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Background refresh disabled"
Msg_Layer_BG_Enabled                    = "Background refresh enabled"
Msg_Layer_Spr_Disabled                  = "Sprites refresh disabled"
Msg_Layer_Spr_Enabled                   = "Sprites refresh enabled"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Unknown disk read command %3i"
Msg_FDC765_Unknown_Write                = "FDC765: Unknown disk write command %3i"
Msg_FDC765_Disk_Too_Large1              = "Disk Image is larger than expected (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Additional data will be ignored"
Msg_FDC765_Disk_Too_Small1              = "Disk Image is smaller than expected (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Missing data will be assumed as empty"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "Pen is touching graphic board"
Msg_TVOekaki_Pen_Away                   = "Pen is not touching graphic board"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Palette"
Msg_Palette_Disabled                    = "Palette viewer disabled"
Msg_Palette_Enabled                     = "Palette viewer enabled"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Messages"
Msg_Message_Disabled                    = "Message box disabled"
Msg_Message_Enabled                     = "Message box enabled"

; Technical Information box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Technical Information"
Msg_TechInfo_Disabled                   = "Technical information box disabled"
Msg_TechInfo_Enabled                    = "Technical information box enabled"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Tiles Viewer"
Msg_TilesViewer_Disabled                = "Tiles viewer disabled"
Msg_TilesViewer_Enabled                 = "Tiles viewer enabled"
Msg_TilesViewer_Tile                    = "Tile %d ($%X) @ %s"

; Memory Editor --------------------------------------------------------------
Msg_MemoryEditor_BoxTitle				= "Memory Editor"
Msg_MemoryEditor_Disabled				= "Memory Editor disabled"
Msg_MemoryEditor_Enabled				= "Memory Editor enabled"
Msg_MemoryEditor_WriteZ80_Unable		= "Unable to write to Z80 address space at this moment!"
Msg_MemoryEditor_Address_Out_of_Bound	= "Address %s is out of %s boundaries!"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Rapid Fire activated for Player %d Button %d"
Msg_RapidFire_JxBx_Off                  = "Rapid Fire disabled for Player %d Button %d"

; FM Unit (YM-2413 chipset) --------------------------------------------------
Msg_FM_Enabled                          = "FM Unit (YM-2413 chipset) emulation enabled"
Msg_FM_Disabled                         = "FM Unit (YM-2413 chipset) emulation disabled"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "Emulated machine is now European/US"
Msg_Country_JAP                         = "Emulated machine is now Japanese"

; Patching System ------------------------------------------------------------
Msg_Patch_Loading                       = "Loading MEKA.PAT (patches)... "
Msg_Patch_Missing                       = "On line %d: No checksum defined to apply command to!"
Msg_Patch_Unrecognized                  = "On line %d: Syntax error or unrecognized command!"
Msg_Patch_Value_Not_a_Byte              = "On line %d: Value %s do not fit into an unsigned byte!"
Msg_Patch_Out_of_Bound                  = "While patching %s: address $%04X is out of boundaries. Patch not applied."

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "3-D Glasses emulation enabled"
Msg_Glasses_Disabled                    = "3-D Glasses emulation disabled"
Msg_Glasses_Show_Both                   = "3-D effect kept as is."
Msg_Glasses_Show_Left                   = "3-D effect disabled by showing only left side."
Msg_Glasses_Show_Right                  = "3-D effect disabled by showing only right side."
Msg_Glasses_Com_Port                    = "3-D Glasses emulation enabled through COM port %d."
Msg_Glasses_Com_Port2                   = "(edit MEKA.CFG to select which COM port to use)"
Msg_Glasses_Com_Port_Open_Error			= "Error opening COM port %d. 3-D Glasses support will not function."
Msg_Glasses_Unsupported                 = "This game seems not to support 3-D Glasses."

; Inputs Initialization ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Initializing joystick... "
Msg_Inputs_Joy_Init_None                = "none found."
Msg_Inputs_Joy_Init_Found               = "%d found."
Msg_Inputs_Joy_Calibrate_Error          = "Error calibrating joystick. Aborting."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Input peripheral: Joypad"
Msg_Inputs_LightPhaser                  = "Input peripheral: Light Phaser"
Msg_Inputs_PaddleControl                = "Input peripheral: Paddle Control"
Msg_Inputs_SportsPad                    = "Input peripheral: Sports Pad"
Msg_Inputs_TVOekaki                     = "Input peripheral: TV Oekaki Graphic Board"
Msg_Inputs_Play_Digital                 = "Control with digital input device"
Msg_Inputs_Play_Mouse                   = "Control with analog input device (mouse)"
Msg_Inputs_Play_Digital_Unrecommended   = "A digital input device (eg: keyboard) is supported but NOT recommended"
Msg_Inputs_Play_Pen                     = "Control pen with mouse\nRight-click simulates removing the pen from the board"
Msg_Inputs_SK1100_Enabled               = "Sega Keyboard (SK-1100) emulation enabled"
Msg_Inputs_SK1100_Disabled              = "Sega Keyboard (SK-1100) emulation disabled"

; Input Configuration --------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Input Configuration"
Msg_Inputs_Config_Peripheral_Click      = "Click to select peripheral"
Msg_Inputs_Config_Source_Enabled        = "Enabled"
Msg_Inputs_Config_Source_Player         = "Controlling Player %d"
Msg_Inputs_Config_Source_Emulate_Joypad = "Emulate joypad"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Loading MEKA.INP (inputs sources definition)... "
Msg_Inputs_Src_Not_Enough               = "Not enough inputs sources found! Check MEKA.INP file or try re-installing."
Msg_Inputs_Src_Missing                  = "On line %d: No input source defined for storing value!"
Msg_Inputs_Src_Equal                    = "On line %d: Missing equal sign for variable assignment!"
Msg_Inputs_Src_Unrecognized             = "On line %d: Unrecognized variable \"%s\"!"
Msg_Inputs_Src_Syntax_Param             = "On line %d: Syntax error on variable parameter!"
Msg_Inputs_Src_Inconsistency            = "On line %d: inconsistency with previous settings!"
Msg_Inputs_Src_Map_Keyboard             = "Now press the key you want to assign to this action"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Action assigned to key \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Now press the button/axis you want to assign to this action"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Action assigned to joypad stick %i, axis %i, %c."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Action assigned to joypad button %i."
Msg_Inputs_Src_Map_Mouse                = "Now press the button you want to assign to this action"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Action assigned to mouse button %i."
Msg_Inputs_Src_Map_Mouse_No_A           = "Sorry - Mouse axis cannot be changed"
Msg_Inputs_Src_Map_Cancelled            = "Assignment cancelled!"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Machine emulation paused"
Msg_Machine_Resume                      = "Machine emulation resumed"
Msg_Machine_Reset                       = "Machine reset"

; Filenames Database ---------------------------------------------------------
Msg_FDB_Loading                         = "Loading MEKA.FDB (filenames database)... "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Loading MEKA.NAM (games names)... "
Msg_DB_Name_Default                     = "Game Screen"
Msg_DB_Name_NoCartridge                 = "No Cartridge"
Msg_DB_SyntaxError                      = "On line %d: Syntax error!"

; Configuration File ---------------------------------------------------------
Msg_Config_Loading                      = "Loading %s (configuration)... "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Loading MEKA.DAT (resources)... "

; Drivers --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Fatal: Unknown Machine Driver!"

; Overdump detection ---------------------------------------------------------
Msg_OverDump                            = "This ROM contains %d times the necessary data.\nPlease download SMS Checker to clean it (http://www.smspower.org/maxim/smschecker/)."

; Sound ----------------------------------------------------------------------
Msg_Sound_Init                          = "Initializing sound system..."
Msg_Sound_Init_Error_Audio              = " - Error opening Audio system. Try a different card and/or sample rate."
Msg_Sound_Init_Error_Blaster            = " - BLASTER variable not found, disabling FM sound!"
Msg_Sound_Init_Error_Blaster_A          = " - BLASTER variable does not contain address (A=xxx), disabling FM sound!"
Msg_Sound_Init_Error_Voices             = " - Voices initialization failed."
Msg_Sound_Init_Error_Voice_N            = " - Voice #%d creation failed."
Msg_Sound_Init_Soundcard                = " - Initializing sound card @ %d Hz"
Msg_Sound_Init_Soundcard_No             = "Can't initialize audio: no sound card selected!"
Msg_Sound_Init_SN76496                  = " - SN76496 digital emulator:"
Msg_Sound_Init_YM2413_Digital           = " - YM2413 digital emulator:"
Msg_Sound_Stream_Error                  = "Sound buffer stop. Restarting stream."
Msg_Sound_Volume_Changed                = "Volume changed to %d%%."

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Loading MEKA.THM (interface themes)... "
Msg_Theme_Error_Not_Enough              = "Not enough themes found! Check MEKA.THM file or try re-installing."
Msg_Theme_Error_Missing_Theme_Name      = "On line %d: No theme name defined for storing value!"
Msg_Theme_Error_Syntax                  = "On line %d: syntax error!"
Msg_Theme_Error_Attribute_Defined       = "On line %d: attribute already defined!"
Msg_Theme_Error_Out_of_Bound            = "On line %d: value out of bounds!"
Msg_Theme_Error_Theme_Missing_Data      = "Theme \"%s\" is missing data. Will be disabled."
Msg_Theme_Error_BG_Big                  = "Background picture to small to fit with integer scaling!"
Msg_Theme_Error_BG                      = "Error loading picture associated with theme!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Loading ROM... "
Msg_LoadROM_Success                     = "Successfully loaded %s"
Msg_LoadDisk_Success                    = "Successfully loaded %s as a floppy disk"
Msg_LoadROM_Comment                     = "Comment: %s"
Msg_LoadROM_SMSGG_Mode_Comment          = "This is a Game Gear ROM running in Master System compatibility mode."
Msg_LoadROM_Warning                     = "-Warning-"
Msg_LoadROM_Bad_Dump_Long               = "This ROM is known as being a bad dump.\nChances are that it will not function properly. Please download SMS Checker to check it (http://www.smspower.org/maxim/smschecker/)."
Msg_LoadROM_Bad_Dump_Short              = "Warning - This ROM is known as being a bad dump."
Msg_LoadROM_Product_Num                 = "Product Number: %s"
Msg_LoadROM_SDSC                        = "-SDSC Header-"
Msg_LoadROM_SDSC_Name                   = "Name: %s"
Msg_LoadROM_SDSC_Version                = "Version: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Date: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Author: %s"
Msg_LoadROM_SDSC_Release_Note           = "Release Note: %s"
Msg_LoadROM_SDSC_Unknown                = "<Unknown>"
Msg_LoadROM_SDSC_Error                  = "<Error>"
Msg_LoadROM_Reload_Reloaded             = "Loaded ROM."
Msg_LoadROM_Reload_No_ROM               = "No ROM to reload!"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Load ROM"
Msg_FileBrowser_Drive                   = "[Drive %c]"
Msg_FileBrowser_Load                    = "Load"
Msg_FileBrowser_Close                   = "Close"
Msg_FileBrowser_LoadNames               = "Load Names"
Msg_FileBrowser_ReloadDir               = "Reload Dir"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "FM Instruments Editor"
Msg_FM_Editor_Enabled                   = "FM Instruments Editor enabled"
Msg_FM_Editor_Disabled                  = "FM Instruments Editor disabled"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Frameskip value set to: Automatic (%d Hz)"
Msg_Frameskip_Standard                  = "Frameskip value set to: 1/%d"
Msg_FPS_Counter_Enabled                 = "FPS counter enabled"
Msg_FPS_Counter_Disabled                = "FPS counter disabled"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "LOG parameter requires a filename!"
Msg_Log_Session_Start                   = "-- Logging session started on %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "LOAD parameter requires a savestate number!"
Msg_Load_Error                          = "Error opening %s!"
Msg_Load_Not_Valid                      = "File %s is not a valid %s savegame!"
Msg_Load_Success                        = "State loaded from %s"
Msg_Load_Version                        = "%s is an unsupported savegame version!"
Msg_Load_Wrong_System                   = "%s is not a savegame for this system!"
Msg_Load_Massage                        = "File is in MSD format - converting"
Msg_Save_Not_in_BIOS                    = "Can't save while running BIOS!"
Msg_Save_Error                          = "Error writing to %s!"
Msg_Save_Success                        = "State saved to %s"
Msg_Save_Slot                           = "Current savegame slot is: %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Options"
Msg_Options_Close                       = "Close"
Msg_Options_BIOS_Enable                 = "Display BIOS logo."
Msg_Options_DB_Display                  = "Display names, flags and icons from database."
Msg_Options_Product_Number              = "Display product number from database."
Msg_Options_Bright_Palette              = "Enable bright palette on SMS and GG."
Msg_Options_Allow_Opposite_Directions   = "Allow pressing opposite joypad directions."
Msg_Options_Load_Close                  = "Close file browser after loading a game."
Msg_Options_Load_FullScreen             = "Switch to fullscreen mode after loading a game."
Msg_Options_FullScreen_Messages         = "Show messages in fullscreen mode."
Msg_Options_GUI_VSync                   = "Wait for VSync in interface mode."
Msg_Options_Capture_Crop_Align          = "Crop and align screen captures to 8x8 boundaries."
Msg_Options_NES_Enable                  = "Mario is not a fat plumber."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "Language set to %s"
Msg_Language_Set_Warning                = "Note: it is recommended that you restart MEKA to make sure all text updates."

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Started dumping sound output to %s"
Msg_Sound_Dumping_Stop                  = "Stopped dumping sound output (%2.02f seconds logged)"
Msg_Sound_Dumping_Error_File_1          = "Unable to find a valid filename to dump sound output!"
Msg_Sound_Dumping_Error_File_2          = "Error opening output file %s for dumping!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "VGM dumps will now be frame accurate (smaller files, skip voices)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "VGM dumps will now be sample accurate (bigger files, dump voices)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(VGM dumping will have to be restarted to take account of new accuracy)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "MAIN"
Msg_Menu_Main_LoadROM                   = "Load ROM"
Msg_Menu_Main_FreeROM                   = "Free ROM"
Msg_Menu_Main_SaveState                 = "Save State"
Msg_Menu_Main_LoadState                 = "Load State"
Msg_Menu_Main_Options                   = "Options"
Msg_Menu_Main_Language                  = "Language"
Msg_Menu_Main_Quit                      = "Quit"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Enabled                  = "Enabled"
Msg_Menu_Debug_Reload_ROM               = "Reload ROM"
Msg_Menu_Debug_Dump                     = "Dump"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "MACHINE"
Msg_Menu_Machine_Power                  = "Power"
Msg_Menu_Machine_Power_On               = "On"
Msg_Menu_Machine_Power_Off              = "Off"
Msg_Menu_Machine_Country                = "Country"
Msg_Menu_Machine_Country_EU             = "European/US"
Msg_Menu_Machine_Country_Jap            = "Japanese"
Msg_Menu_Machine_TVType                 = "TV Type"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Hard Pause"
Msg_Menu_Machine_HardReset              = "Hard Reset"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VIDEO"
Msg_Menu_Video_FullScreen               = "Fullscreen"
Msg_Menu_Video_Themes                   = "Themes"
Msg_Menu_Video_Blitters                 = "Blitters"
Msg_Menu_Video_Layers                   = "Layers"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Background"
Msg_Menu_Video_Flickering               = "Flickering"
Msg_Menu_Video_Flickering_Auto          = "Auto"
Msg_Menu_Video_Flickering_Yes           = "Yes"
Msg_Menu_Video_Flickering_No            = "No"
Msg_Menu_Video_3DGlasses                = "3-D Glasses"
Msg_Menu_Video_3DGlasses_Enabled        = "Enabled"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Show both sides"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Show left side"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Show right side"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Uses Glasses on COM Port"
Msg_Menu_Video_Capture                  = "Capture"
Msg_Menu_Video_Capture_CaptureScreen    = "Capture Screen"
Msg_Menu_Video_Capture_CaptureScreenAll = "Capture Screen (All)"
Msg_Menu_Video_Capture_IncludeGUI       = "Include GUI"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "SOUND"
Msg_Menu_Sound_FM                       = "FM Unit"
Msg_Menu_Sound_FM_Enabled               = "Enabled"
Msg_Menu_Sound_FM_Disabled              = "Disabled"
Msg_Menu_Sound_FM_Editor                = "Instruments Editor"
Msg_Menu_Sound_Volume                   = "Volume"
Msg_Menu_Sound_Volume_Mute              = "Mute"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Rate"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Channels"
Msg_Menu_Sound_Channels_Tone            = "Tone %d"
Msg_Menu_Sound_Channels_Noises          = "Noises"
Msg_Menu_Sound_Dump                     = "Dump"
Msg_Menu_Sound_Dump_WAV_Start           = "WAV Start"
Msg_Menu_Sound_Dump_WAV_Stop            = "WAV Stop"
Msg_Menu_Sound_Dump_VGM_Start           = "VGM Start"
Msg_Menu_Sound_Dump_VGM_Stop            = "VGM Stop"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "VGM Sample Accurate"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "INPUTS"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Light Phaser"
Msg_Menu_Inputs_PaddleControl           = "Paddle Control"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Graphic Board"
Msg_Menu_Inputs_SK1100                  = "Sega Keyboard"
Msg_Menu_Inputs_RapidFire               = "Rapid Fire"
Msg_Menu_Inputs_RapidFire_PxBx          = "Player %d Button %d"
Msg_Menu_Inputs_Configuration           = "Configuration"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "TOOLS"
Msg_Menu_Tools_Messages                 = "Messages"
Msg_Menu_Tools_Palette                  = "Palette"
Msg_Menu_Tools_TilesViewer              = "Tiles Viewer"
Msg_Menu_Tools_TilemapViewer            = "Tilemap Viewer"
Msg_Menu_Tools_TechInfo                 = "Tech Info"
Msg_Menu_Tools_MemoryEditor             = "Memory Editor"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "HELP"
Msg_Menu_Help_Documentation             = "Documentation"
Msg_Menu_Help_Documentation_W           = "Documentation (Windows)"
Msg_Menu_Help_Documentation_U           = "Documentation (Unix)"
Msg_Menu_Help_Compat                    = "Compatibility List"
Msg_Menu_Help_Multiplayer_Games         = "Multiplayer Games List"
Msg_Menu_Help_Changes                   = "Changes List"
Msg_Menu_Help_Debugger					= "Debugger"
Msg_Menu_Help_About                     = "About"

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Français]
; Maintainer: Omar Cornut (omar AT miracleworld.net)
; Original text by Overture 1928 (alissalde AT aol.fr)
; Merci à Eric pour ses lumières techniques!
WIP
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Bienvenue dans %s (c) %s"
;Msg_Window_Title                        = "MEKA - 8 bit ou rien!"
Msg_Quit                                = "Good night, brave warrior. Good night, monster-land...."
;Msg_Quit                               = "Bonne nuit, brave guerrier. Bonne nuit, pays des monstres...."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "A propos de Meka..."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "par %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Ok                                  = "Ok"
Msg_Failed                              = "Echec!"
Msg_Error_Base                          = "Erreur: %s"
Msg_Error_Error                         = "Fatal: l'erreur a fait une erreur. Quelque chose ne va pas."
Msg_Error_Memory                        = "Mémoire insuffisante!"
Msg_Error_Param                         = "Le paramètre \"%s\" est invalide!\n"
Msg_Error_Syntax                        = "Erreur de syntaxe!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Impossible de basculer vers le mode vidéo souhait‚ (%dx%d)."
Msg_Error_Video_Mode_Back_To_GUI        = "Passage en mode interface."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "Fichier introuvable!"
Msg_Error_File_Read                     = "Impossible de lire le fichier!"
Msg_Error_File_Empty                    = "Le fichier est vide!"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "Cette version ne supporte pas les fichiers ZIP!"
Msg_Error_ZIP_Loading                   = "Impossible de lire le fichier ZIP!"
Msg_Error_ZIP_Internal                  = "Erreur interne lors du chargement du fichier ZIP!"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Erreur en ouvrant le dossier!"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Réinitialiser le jeu pour que les changements prennent effet"
Msg_No_ROM                              = "Aucune ROM chargée!"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Initialisation d'Allegro..."
Msg_Init_GUI                            = "Initialisation de l'interface graphique..."
Msg_Init_Completed                      = "[PRET]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Running                       = "Execution du programme de configuration..."
Msg_Setup_Setup                         = "Configuration"
Msg_Setup_Soundcard_Select              = "Choisissez votre carte son:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(DirectSound est trŠs recommand‚, si disponible)"
Msg_Setup_SampleRate_Select             = "Fréquence d'échantillonage:"
Msg_Setup_Debugger_Enable               = "Active le debuggeur Z80 et autres fonctionnalites associees"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Ecran capturé dans %s"
Msg_Capture_Error                       = "Impossible de sauvegarder la capture d'écran!"
Msg_Capture_Error_File                  = "Impossible de trouver un nom de fichier valide pour sauvegarder la capture d'écran!"

; On-Board Memory ------------------------------------------------------------
Msg_SRAM_Loaded                         = "SaveRAM chargée depuis le disque (%d Kb)"
Msg_SRAM_Load_Unable                    = "Impossible de charger la SaveRAM depuis le disque"
Msg_SRAM_Wrote                          = "SaveRAM écrite sur le disque (%d Kb)"
Msg_SRAM_Write_Unable                   = "Impossible d'écrire la SaveRAM sur le disque (%d Kb)"
Msg_93c46_Reset                         = "Réinitialisation du 93c46"
Msg_93c46_Loaded                        = "Données EEPROM 93c46 chargées depuis le disque (%d bytes)"
Msg_93c46_Load_Unable                   = "Impossible de charger les données EEPROM 93c46 depuis le disque"
Msg_93c46_Wrote                         = "Données EEPROM 93c46 écrites sur le disque (%d bytes)"
Msg_93c46_Write_Unable                  = "Impossible d'écrire les données EEPROM 93c46 sur le disque (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "Le type de TV est maintenant %s"
Msg_TVType_Info_Speed                   = "(La vitesse originale est %d Hz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Chargement de MEKA.BLT (modes vid‚o / paramˆtres)... "
Msg_Blitters_Error_Not_Enough           = "Il manque des paramˆtres! V‚rifiez le fichier MEKA.BLT ou essayez de r‚-installer."
Msg_Blitters_Error_Not_Found            = "Paramˆtres introuvables!"
Msg_Blitters_Error_Missing              = "Ligne %d: Aucun nom de paramˆtre d‚fini pour la valeur en mˆmoire!"
Msg_Blitters_Error_Unrecognized         = "Ligne %d: Variable inconnue!"
Msg_Blitters_Set                        = "Paramêtre plein écran actuel \"%s\"."

; Nintendo -------------------------------------------------------------------
Msg_NES_Activate                        = "Mode gros plombier activé"
Msg_NES_Sucks                           = "Erreur fatale: l'utilisateur aime Nintendo!"
Msg_NES_Mapper_Unknown                  = "Mapper non supporté: %d. Le jeu ne devrait pas fonctionner."
Msg_NES_Deny_Facts                      = "Un fait est un fait."

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Initialisation du débuggueur... "
Msg_Debug_Welcome                       = "Bienvenue dans le débuggueur de Meka!"
Msg_Debug_Not_Available                 = "Le débuggueur n'est pas disponible dans cette version!"
Msg_Debug_Brk_Need_Param                = "Le paramêtre DEBUGBRK nécessite une adresse!"
Msg_Debug_Trap_Read                     = "A PC:%04X - Lecture depuis %04X"
Msg_Debug_Trap_Write                    = "A PC:%04X - Ecriture de %02X dans %04X"
Msg_Debug_Trap_Port_Read                = "A PC:%04X - Lecture Port [%02X] "
Msg_Debug_Trap_Port_Write               = "A PC:%04X - Ecriture %02X Port [%02X] "

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "L'acquisition des données sera maintenant faite en mode ASCII."
Msg_DataDump_Mode_Raw                   = "L'acquisition des données sera maintenant faite en mode Raw."
Msg_DataDump_Error                      = "Erreur en créant le fichier d'acquisition %s!"
Msg_DataDump_Error_OB_Memory            = "Pas de mémoire sur carte à acquérir!"
Msg_DataDump_Error_Palette              = "Pas de palette à acquérir!"
Msg_DataDump_Error_Sprites              = "Pas de sprites à acquérir!"
Msg_DataDump_Main                       = "Acquisition %s (%d %s) en mode %s"

; Documentation --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Documentation"
Msg_Doc_File_Error                      = "Impossible d'ouvrir la documentation!"
Msg_Doc_Enabled                         = "Documentation ouverte."
Msg_Doc_Disabled                        = "Documentation fermée."

; Sprite Flickering ----------------------------------------------------------
Msg_Flickering_Auto                     = "Le clignotement des sprites sera géré automatiquement."
Msg_Flickering_Yes                      = "L'émulation du clignotement des sprite est maintenant activée."
Msg_Flickering_No                       = "L'émulation du clignotement des sprites n'est plus activée."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Rafraichissement du fond d'écran désactivé"
Msg_Layer_BG_Enabled                    = "Rafraichissement du fond d'écran activé"
Msg_Layer_Spr_Disabled                  = "Rafraichissement des sprites désactivé"
Msg_Layer_Spr_Enabled                   = "Rafraichissement des sprites activé"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Commande %3i de lecture de disque inconnue"
Msg_FDC765_Unknown_Write                = "FDC765: Commande %3i d'écriture de disque inconnue"
Msg_FDC765_Disk_Too_Large1              = "L'image du disque est plus grande que prévue (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Les données supplémentaires seront ignorées"
Msg_FDC765_Disk_Too_Small1              = "L'image du disque est plus petite que prévue (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Les données manquantes seront considérées comme vides"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "Le stylo touche la table graphique"
Msg_TVOekaki_Pen_Away                   = "Le stylo ne touche pas la table graphique"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Palette"
Msg_Palette_Disabled                    = "Visualisateur de palette désactivé"
Msg_Palette_Enabled                     = "Visualisateur de palette activé"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Messages"
Msg_Message_Disabled                    = "Fenêtre des messages désactivée"
Msg_Message_Enabled                     = "Fenêtre des messages activée"

; Technical Information box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Informations Techniques"
Msg_TechInfo_Disabled                   = "Fenêtre d'informations techniques désactivée"
Msg_TechInfo_Enabled                    = "Fenêtre d'informations techniques activée"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Visionneur de tiles"
Msg_TilesViewer_Disabled                = "Visionneur de tiles désactivé"
Msg_TilesViewer_Enabled                 = "Visionneur de tiles activé"
Msg_TilesViewer_Tile                    = "Tile %d ($%X) @ %s"

; Memory Editor --------------------------------------------------------------
Msg_MemoryEditor_BoxTitle				= "Editeur mémoire"
Msg_MemoryEditor_Disabled				= "Editeur mémoire désactivé"
Msg_MemoryEditor_Enabled				= "Editeur mémoire activé"
Msg_MemoryEditor_WriteZ80_Unable		= "Impossible d'écrire dans l'espace d'addressage Z80 à cet instant!"
Msg_MemoryEditor_Address_Out_of_Bound	= "L'addresse %s est hors de la zone %s!"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Tir Automatique activé pour le joueur %d, bouton %d"
Msg_RapidFire_JxBx_Off                  = "Tir Automatique désactivé pour le joueur %d, bouton %d"

; FM Unit (YM-2413 chipset) --------------------------------------------------
Msg_FM_Enabled                          = "Emulation de l'unité FM (chipset YM-2413) activée"
Msg_FM_Disabled                         = "Emulation de l'unité FM (chipset YM-2413) désactivée"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "La machine émulée est maintenant Européenne/US"
Msg_Country_JAP                         = "La machine émulée est maintenant Japonaise"

; Patching System ------------------------------------------------------------
Msg_Patch_Loading                       = "Chargement de MEKA.PAT (patches)... "
Msg_Patch_Missing                       = "Ligne %d: Checksum non d‚finit pour appliquer la commande!"
Msg_Patch_Unrecognized                  = "Ligne %d: Erreur de syntaxe ou commande inconnue!"
Msg_Patch_Value_Not_a_Byte              = "Ligne %d: La valeur %s ne tient pas sur un octet non signé!"
Msg_Patch_Out_of_Bound                  = "Lors du patch de %s: la position $%04X est hors des limites. Patch non appliqué."

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "Emulation des lunettes 3-D activée"
Msg_Glasses_Disabled                    = "Emulation des lunettes 3-D désactivée"
Msg_Glasses_Show_Both                   = "Effet 3-D complet."
Msg_Glasses_Show_Left                   = "Effet 3-D désactivé en ne gardant que le côté gauche."
Msg_Glasses_Show_Right                  = "Effet 3-D désactivé en ne gardant que le côté droit."
Msg_Glasses_Com_Port                    = "Support des lunettes 3-D activé sur le port COM %d."
Msg_Glasses_Com_Port2                   = "(éditer MEKA.CFG pour sélectionner le port COM à utiliser)"
Msg_Glasses_Com_Port_Open_Error			= "Erreur à l'ouverture du port COM %d. Le support des lunettes 3-D ne fonctionnera pas."
Msg_Glasses_Unsupported                 = "Ce jeu ne supporte pas les lunettes 3-D."

; Inputs Initialization ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Initialisation des joysticks... "
Msg_Inputs_Joy_Init_None                = "aucun joystick trouv‚."
Msg_Inputs_Joy_Init_Found               = "%d trouv‚(s)."
Msg_Inputs_Joy_Calibrate_Error          = "Arrˆt : Erreur de calibrage de joystick."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Périphérique de contrôle: Joypad"
Msg_Inputs_LightPhaser                  = "Périphérique de contrôle: Light Phaser"
Msg_Inputs_PaddleControl                = "Périphérique de contrôle: Paddle Control"
Msg_Inputs_SportsPad                    = "Périphérique de contrôle: Sports Pad"
Msg_Inputs_TVOekaki                     = "Périphérique de contrôle: Tablette Graphique"
Msg_Inputs_Play_Digital                 = "Contrôle avec périphérique d'entrée digital"
Msg_Inputs_Play_Mouse                   = "Contrôle avec périphérique d'entrée analogique (souris)"
Msg_Inputs_Play_Digital_Unrecommended   = "Un périphérique d'entrée digital (ex: clavier) est supporté mais NON conseillé"
Msg_Inputs_Play_Pen                     = "Contrôle du stylo avec la souris\nLe click-droit simule la levée du stylo de la tablette"
Msg_Inputs_SK1100_Enabled               = "Emulation du Sega Keyboard (SK-1100) activée"
Msg_Inputs_SK1100_Disabled              = "Emulation du Sega Keyboard (SK-1100) désactivée"

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Configuration des contrôles"
Msg_Inputs_Config_Peripheral_Click      = "Clic pour sélectionner périphérique"
Msg_Inputs_Config_Source_Enabled        = "Actif"
Msg_Inputs_Config_Source_Player         = "Contrôle joueur %d"
Msg_Inputs_Config_Source_Emulate_Joypad = "Emule le joypad"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Chargement de MEKA.INP (d‚finition des sources de contr“le)... "
Msg_Inputs_Src_Not_Enough               = "Il manque des sources de contr“le! V‚rifiez le fichier MEKA.INP ou essayez de r‚-installer."
Msg_Inputs_Src_Missing                  = "Ligne %d: Aucune source de contr“le d‚finie pour la valeur en m‚moire!"
Msg_Inputs_Src_Equal                    = "Ligne %d: Signe ‚gal manquant pour l'attribution de la variable!"
Msg_Inputs_Src_Unrecognized             = "Ligne %d: Variable \"%s\" non reconnue!"
Msg_Inputs_Src_Syntax_Param             = "Ligne %d: Erreur de syntaxe sur paramˆtre variable!"
Msg_Inputs_Src_Inconsistency            = "Ligne %d: Ne correspond pas aux r‚glages pr‚c‚dents!"
Msg_Inputs_Src_Map_Keyboard             = "Appuyez sur la touche souhaitée pour y attribuer cette action"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Action attribuée à la touche \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Appuyez sur le bouton ou la direction souhaité pour y attribuer cette action"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Action attribuée au stick %i, axe %i, %c du joypad."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Action attribuée au bouton %i du joypad."
Msg_Inputs_Src_Map_Mouse                = "Appuyez sur le bouton souhaité pour y attribuer cette action"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Action attribuée au bouton %i de la souris."
Msg_Inputs_Src_Map_Mouse_No_A           = "Désolé - L'axe de la souris ne peut pas être changé"
Msg_Inputs_Src_Map_Cancelled            = "Attribution annulée!"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Emulation en pause"
Msg_Machine_Resume                      = "Emulation rétablie"
Msg_Machine_Reset                       = "Réinitialisation de la machine"

; Filenames Database ---------------------------------------------------------
Msg_FDB_Loading                         = "Chargement de MEKA.FDB (base de donnée fichiers)... "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Chargement de MEKA.NAM (noms des jeux)... "
Msg_DB_Name_Default                     = "Ecran de jeu"
Msg_DB_Name_NoCartridge                 = "Pas de cartouche"
Msg_DB_SyntaxError                      = "Ligne %d: Erreur de syntaxe!"

; Configuration File ---------------------------------------------------------
Msg_Config_Loading                      = "Chargement de %s (configuration)... "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Chargement de MEKA.DAT (ressources)... "

; Drivers --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Fatal: Pilote Machine Inconnu!"

; Overdump detection ---------------------------------------------------------
Msg_OverDump                            = "Cette ROM contient %d fois les données nécessaires.\nTéléchargez SMS Checker pour la vérifier (http://www.smspower.org/maxim/smschecker/)."

; Sound ----------------------------------------------------------------------
Msg_Sound_Init                          = "Initialisation du son..."
Msg_Sound_Init_Error_Audio              = " - Erreur en ouvrant le systˆme audio. Essayez une carte et/ou une fr‚quence diff‚rente."
Msg_Sound_Init_Error_Blaster            = " - Variable BLASTER introuvable, son FM d‚sactiv‚!"
Msg_Sound_Init_Error_Blaster_A          = " - La variable BLASTER ne contient l'adresse (A=xxx), son FM d‚sactiv‚!"
Msg_Sound_Init_Error_Voices             = " - Echec de l'initialisation des voies."
Msg_Sound_Init_Error_Voice_N            = " - Echec dans la cr‚ation de la voie #%d."
Msg_Sound_Init_Soundcard                = " - Initialisation de la carte son … %d Hz"
Msg_Sound_Init_Soundcard_No             = "Impossible d'initialiser l'audio: aucune carte son s‚lectionn‚e!"
Msg_Sound_Init_SN76496                  = " - Emulateur SN76496 digital:"
Msg_Sound_Init_YM2413_Digital           = " - Emulateur YM2413 digital:"
Msg_Sound_Stream_Error                  = "Arrêt du tampon son. Redémarrage du flux."
Msg_Sound_Volume_Changed                = "Volume changé à %d%%."

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Chargement de MEKA.THM (thêmes de l'interface)... "
Msg_Theme_Error_Not_Enough              = "Il manque des thêmes! Vérifiez le fichier MEKA.THM ou essayez de réinstaller."
Msg_Theme_Error_Missing_Theme_Name      = "Ligne %d: Aucun nom de thême défini pour stocker l'attribut!"
Msg_Theme_Error_Syntax                  = "Ligne %d: Erreur de syntaxe!"
Msg_Theme_Error_Attribute_Defined       = "Ligne %d: Attribute déja défini!"
Msg_Theme_Error_Out_of_Bound            = "Ligne %d: Valeur hors limites!"
Msg_Theme_Error_Theme_Missing_Data      = "Le thême \"%s\" est incomplet. Il sera désactivé."
Msg_Theme_Error_BG_Big                  = "Image d'arrière-plan trop petite pour l'ajuster complètement!"
Msg_Theme_Error_BG                      = "Erreur de chargement de l'image associée au thème!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Chargement de la ROM.. "
Msg_LoadROM_Success                     = "Chargement de %s réussi"
Msg_LoadDisk_Success                    = "Chargement de %s réussi depuis la disquette"
Msg_LoadROM_Comment                     = "Commentaire: %s"
Msg_LoadROM_SMSGG_Mode_Comment          = "Ceci est une ROM Game Gear fonctionnant en mode de compatibilité Master System."
Msg_LoadROM_Warning                     = "-Avertissement-"
Msg_LoadROM_Bad_Dump_Long               = "Cette ROM est connue comme étant un mauvais dump.\nIl se peut qu'elle ne fonctionne pas correctement. Téléchargez SMS Checker pour la vérifier (http://www.smspower.org/maxim/smschecker/)."
Msg_LoadROM_Bad_Dump_Short              = "Avertissement - Cette ROM est connue comme étant un mauvais dump."
Msg_LoadROM_Product_Num                 = "Numéro de produit: %s"
Msg_LoadROM_SDSC                        = "-En-tête SDSC-"
Msg_LoadROM_SDSC_Name                   = "Nom: %s"
Msg_LoadROM_SDSC_Version                = "Version: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Date: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Auteur: %s"
Msg_LoadROM_SDSC_Release_Note           = "Note: %s"
Msg_LoadROM_SDSC_Unknown                = "<Inconnu>"
Msg_LoadROM_SDSC_Error                  = "<Erreur>"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Charger une ROM"
Msg_FileBrowser_Drive                   = "[Disque %c]"
Msg_FileBrowser_Load                    = "Charger"
Msg_FileBrowser_Close                   = "Fermer"
Msg_FileBrowser_LoadNames               = "Lire noms"
Msg_FileBrowser_ReloadDir               = "Relire dossier"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "Editeur d'instruments FM"
Msg_FM_Editor_Enabled                   = "Editeur d'instruments FM activé"
Msg_FM_Editor_Disabled                  = "Editeur d'instruments FM désactivé"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Saut d'image automatique (à %d Hz)"
Msg_Frameskip_Standard                  = "Saut d'image fixé à: 1/%d images"
Msg_FPS_Counter_Enabled                 = "Compteur d'image par seconde activé"
Msg_FPS_Counter_Disabled                = "Compteur d'image par seconde désactivé"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "Le paramêtre LOG nécessite un nom de fichier!"
Msg_Log_Session_Start                   = "-- Rapport de session débuté le %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "Le paramêtre LOAD nécessite un numéro de sauvegarde!"
Msg_Load_Error                          = "Erreur en ouvrant %s!"
Msg_Load_Not_Valid                      = "Le fichier %s n'est pas une sauvegarde valide de %s!"
Msg_Load_Success                        = "Etat chargé depuis %s"
Msg_Load_Version                        = "%s n'est pas une version de sauvegarde supportée!"
Msg_Load_Wrong_System                   = "%s n'est pas une sauvegarde pour ce système!"
Msg_Load_Massage                        = "Le fichier est au format MSD - conversion en cours"
Msg_Save_Not_in_BIOS                    = "Impossible de sauver pendant le BIOS!"
Msg_Save_Error                          = "Erreur d'écriture dans %s!"
Msg_Save_Success                        = "Etat sauvegardé dans %s"
Msg_Save_Slot                           = "Emplacement actuel de sauvegarde: %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Options"
Msg_Options_Close                       = "Fermer"
Msg_Options_BIOS_Enable                 = "Afficher le logo du BIOS."
Msg_Options_DB_Display                  = "Afficher noms, drapeaux et icônes de la base."
Msg_Options_Product_Number              = "Afficher numéros de produit de la base."
Msg_Options_Bright_Palette              = "Activer palette vive pour SMS et GG."
Msg_Options_Allow_Opposite_Directions   = "Permettre l'appui de directions opposées."
Msg_Options_Load_Close                  = "Fermer l'explorateur après chargement d'un jeu."
Msg_Options_Load_FullScreen             = "Mode plein écran auto après chargement d'un jeu."
Msg_Options_FullScreen_Messages         = "Afficher les messages en mode plein écran."
Msg_Options_GUI_VSync                   = "Attendre la VSync en mode interface."
Msg_Options_NES_Enable                  = "Mario n'est pas un gros plombier."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "La langue est maintenant le %s"
Msg_Language_Set_Warning                = "Note: il est recommandé de redémarrer MEKA afin d'avoir tous les textes mis à jour."

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Acquisition de la sortie sonore dans %s"
Msg_Sound_Dumping_Stop                  = "Acquisition de la sortie sonore arrêtée (%2.02f secondes enregistrées)"
Msg_Sound_Dumping_Error_File_1          = "Impossible de trouver un nom de fichier valide pour extraire la sortie sonore!"
Msg_Sound_Dumping_Error_File_2          = "Erreur d'ouverture du fichier %s pour l'acquisition!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "L'acquisition VGM sera maintenant fidèle à la trame (fichiers plus petits, voies sautées)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "L'acquisition VGM sera maintenant fidèle à l'échantillon (fichiers plus importants, acquisition des voies)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(l'acquisition VGM devra être redémarrée pour tenir en compte du changement de paramètre)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "PRINCIPAL"
Msg_Menu_Main_LoadROM                   = "Charger ROM"
Msg_Menu_Main_FreeROM                   = "Libérer ROM"
Msg_Menu_Main_SaveState                 = "Sauver état"
Msg_Menu_Main_LoadState                 = "Charger état"
Msg_Menu_Main_Options                   = "Options"
Msg_Menu_Main_Language                  = "Langue"
Msg_Menu_Main_Quit                      = "Quitter"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Enabled                  = "Activé"
Msg_Menu_Debug_Dump                     = "Acquisition"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "MACHINE"
Msg_Menu_Machine_Power                  = "Marche/Arrêt"
Msg_Menu_Machine_Power_On               = "Allumer"
Msg_Menu_Machine_Power_Off              = "Eteindre"
Msg_Menu_Machine_Country                = "Pays"
Msg_Menu_Machine_Country_EU             = "Européenne/US"
Msg_Menu_Machine_Country_Jap            = "Japonaise"
Msg_Menu_Machine_TVType                 = "Type de TV"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Pause hard"
Msg_Menu_Machine_HardReset              = "Reset hard"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VIDEO"
Msg_Menu_Video_FullScreen               = "Plein écran"
Msg_Menu_Video_Themes                   = "Thèmes"
Msg_Menu_Video_Blitters                 = "Paramètres"
Msg_Menu_Video_Layers                   = "Plans"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Arrière plan"
Msg_Menu_Video_Flickering               = "Clignotement"
Msg_Menu_Video_Flickering_Auto          = "Automatique"
Msg_Menu_Video_Flickering_Yes           = "Activé"
Msg_Menu_Video_Flickering_No            = "Désactivé"
Msg_Menu_Video_3DGlasses                = "Lunettes 3-D"
Msg_Menu_Video_3DGlasses_Enabled        = "Activées"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Activer les deux côtés"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Activer le côté gauche"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Activer le côté droit"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Lunettes sur le Port COM"
Msg_Menu_Video_Capture                  = "Capture"
Msg_Menu_Video_Capture_CaptureScreen    = "Capturer l'écran"
Msg_Menu_Video_Capture_CaptureScreenAll = "Capturer l'écran (tous)"
Msg_Menu_Video_Capture_IncludeGUI       = "Inclure l'interface"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "SON"
Msg_Menu_Sound_FM                       = "Module FM"
Msg_Menu_Sound_FM_Enabled               = "Activé"
Msg_Menu_Sound_FM_Disabled              = "Désactivé"
Msg_Menu_Sound_FM_Editor                = "Editeur d'instruments"
Msg_Menu_Sound_Volume                   = "Volume"
Msg_Menu_Sound_Volume_Mute              = "Muet"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Fréquence"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Canaux"
Msg_Menu_Sound_Channels_Tone            = "Ton %d"
Msg_Menu_Sound_Channels_Noises          = "Bruits"
Msg_Menu_Sound_Dump                     = "Acquisition"
Msg_Menu_Sound_Dump_WAV_Start           = "Démarrer en WAV"
Msg_Menu_Sound_Dump_WAV_Stop            = "Arrêter en WAV"
Msg_Menu_Sound_Dump_VGM_Start           = "Démarrer en VGM"
Msg_Menu_Sound_Dump_VGM_Stop            = "Arrêter en VGM"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "Haute précision VGM"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "CONTROLE"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Light Phaser"
Msg_Menu_Inputs_PaddleControl           = "Paddle Control"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Table Graphique"
Msg_Menu_Inputs_SK1100                  = "Clavier Sega"
Msg_Menu_Inputs_RapidFire               = "Tir Automatique"
Msg_Menu_Inputs_RapidFire_PxBx          = "Joueur %d Bouton %d"
Msg_Menu_Inputs_Configuration           = "Configuration"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "OUTILS"
Msg_Menu_Tools_Messages                 = "Messages"
Msg_Menu_Tools_Palette                  = "Palette"
Msg_Menu_Tools_TilesViewer              = "Visionneur de tiles"
Msg_Menu_Tools_TechInfo                 = "Informations techniques"
Msg_Menu_Tools_MemoryEditor				= "Editeur mémoire"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "AIDE"
Msg_Menu_Help_Documentation             = "Documentation"
Msg_Menu_Help_Documentation_W           = "Documentation (Windows)"
Msg_Menu_Help_Documentation_U           = "Documentation (Unix)"
Msg_Menu_Help_Compat                    = "Compatibilité"
Msg_Menu_Help_Multiplayer_Games         = "Jeux Multijoueurs"
Msg_Menu_Help_Changes                   = "Mises à jour"
Msg_Menu_Help_About                     = "A propos"

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Redneck]
WIP
; Translation by the Dialectizer (http://www.rinkworks.com/dialect)
; Maintainer: Maxim (maxim AT mwos.cjb.net)
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Welcome t'%s (c) %s"
;Msg_Window_Title                        = "MEKA - 8-bit o' die!"
Msg_Quit                                = "Good night, brave warrio'. Good night, monster-lan'...."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "About Meka.."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "by %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Ok                                  = "Ok"
Msg_Failed                              = "Failed! Fry mah hide!"
Msg_Error_Base                          = "Erro': %s"
Msg_Error_Error                         = "Fatal: erro' gittin' erro' type. Sumpin muss be wrong, acco'din' t' th' code o' th' heells!"
Msg_Error_Memory                        = "Not inough memo'y! Fry mah hide!"
Msg_Error_Param                         = "Pareemeter \"%s\" is invalid! Fry mah hide!\n"
Msg_Error_Syntax                        = "Syntax erro'! Fry mah hide!"
Msg_Error_Video_Mode_Back_To_GUI        = "Switchin' back t'graphical user interface."

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Unable t'switch t'desuhed video mode (%dx%d)."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "File not foun'! Fry mah hide!"
Msg_Error_File_Read                     = "Unable t'read fum file!"
Msg_Error_File_Empty                    = "File is emppy! Fry mah hide!"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "This hyar vahshun don't han'le ZIP files!"
Msg_Error_ZIP_Loading                   = "Unable t'read ZIP file!"
Msg_Error_ZIP_Internal                  = "Internal erro' while loadin' ZIP file!"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Erro' openin' direcko'y! Fry mah hide!"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Reset th' game fo' th' changes t'take effeck"
Msg_No_ROM                              = "No ROM was loaded! Fry mah hide!"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Initializin' Allegro.."
Msg_Init_GUI                            = "Initializin' graphical user interface.."
Msg_Init_Completed                      = "[READY]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Setup                         = "Setup"
Msg_Setup_SampleRate_Select             = "Sample rate:"
Msg_Setup_Soundcard_Select              = "Please seleck yer audio device:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(Direck X is highly recommended, eff'n available)"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Screen cappured t'%s"
Msg_Capture_Error                       = "Unable t'create screen cappure!"
Msg_Capture_Error_File                  = "Unable t'find valid filename t'save screen cappure!"

; On-Board Memomy ------------------------------------------------------------
Msg_SRAM_Loaded                         = "Loaded SaveRAM fum disk (%d Kb)"
Msg_SRAM_Load_Unable                    = "Unable t'load SaveRAM fum disk"
Msg_SRAM_Wrote                          = "Wrote SaveRAM t'disk (%d Kb)"
Msg_SRAM_Write_Unable                   = "Unable t'write SaveRAM t'disk (%d Kb)"
Msg_93c46_Reset                         = "Resettin' 93c46"
Msg_93c46_Loaded                        = "Loaded 93c46 EEPROM data fum disk (%d bytes)"
Msg_93c46_Load_Unable                   = "Unable t'load 93c46 EEPROM data fum disk"
Msg_93c46_Wrote                         = "Wrote 93c46 EEPROM data t'disk (%d bytes)"
Msg_93c46_Write_Unable                  = "Unable t'write 93c46 EEPROM data t'disk (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "TV Type is now %s"
Msg_TVType_Info_Speed                   = "(Origeenal speed is %d Hz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Loadin' MEKA.BLT (video modes / blitters).. "
Msg_Blitters_Error_Not_Enough           = "Not inough blitters foun'! Fry mah hide! Check MEKA.BLT file o' try re-installin'."
Msg_Blitters_Error_Not_Found            = "Blitter not foun'! Fry mah hide!"
Msg_Blitters_Error_Missing              = "On line %d: No blitter name defined fo' sto'in' value!"
Msg_Blitters_Error_Unrecognized         = "On line %d: Unreckanized variable!"
Msg_Blitters_Set                        = "Now usin' \"%s\" fullscreen blitter."

; Naimo ----------------------------------------------------------------------
Msg_NES_Activate                        = "Fat plumber mode ackivated"
Msg_NES_Sucks                           = "Fatal erro': user likes Nintendo! Fry mah hide!"
Msg_NES_Mapper_Unknown                  = "Unsuppo'ted Mapper: %d, cuss it all t' tarnation. Game sh'd not funckshun."
Msg_NES_Deny_Facts                      = "Yo' kin deny facks."

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Initializin' debugger.. "
Msg_Debug_Welcome                       = "Welcome t'Meka Debugger! Fry mah hide!"
Msg_Debug_Not_Available                 = "Debugger not available in this hyar vahshun! Fry mah hide!"
Msg_Debug_Brk_Need_Param                = "DEBUGBRK opshun needs an address!"
Msg_Debug_Trap_Read                     = "At PC:%04X - Read fum %04X"
Msg_Debug_Trap_Write                    = "At PC:%04X - Write %02X t'%04X"
Msg_Debug_Trap_Port_Read                = "At PC:%04X - Po't [%02X] Read"
Msg_Debug_Trap_Port_Write               = "At PC:%04X - Po't [%02X] Write %02X"

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Follerin' data dumps will be made in ASCIah mode."
Msg_DataDump_Mode_Raw                   = "Follerin' data dumps will be made in Raw mode."
Msg_DataDump_Error                      = "Erro' creatin' file t'dump %s!"
Msg_DataDump_Error_OB_Memory            = "No on-board memo'y t'dump! Fry mah hide!"
Msg_DataDump_Error_Palette              = "No palette t'dump! Fry mah hide!"
Msg_DataDump_Error_Sprites              = "No sprites t'dump! Fry mah hide!"
Msg_DataDump_Main                       = "Dumped %s (%d %s) in %s mode"

; Docoomntashun --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Docoomntashun"
Msg_Doc_File_Error                      = "Unable t'open docoomntashun! Fry mah hide!"
Msg_Doc_Enabled                         = "Docoomntashun showin' up."
Msg_Doc_Disabled                        = "Docoomntashun is no mo'e."

; Sprite Flickerin' ----------------------------------------------------------
Msg_Flickering_Auto                     = "Sprite flickerin' will be han'led autymatically."
Msg_Flickering_Yes                      = "Sprite flickerin' emulashun is now inabled, cuss it all t' tarnation."
Msg_Flickering_No                       = "Sprite flickerin' emulashun is now disabled, cuss it all t' tarnation."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Backgroun' refresh disabled"
Msg_Layer_BG_Enabled                    = "Backgroun' refresh inabled"
Msg_Layer_Spr_Disabled                  = "Sprites refresh disabled"
Msg_Layer_Spr_Enabled                   = "Sprites refresh inabled"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Unknown disk read comman' %3i"
Msg_FDC765_Unknown_Write                = "FDC765: Unknown disk write comman' %3i"
Msg_FDC765_Disk_Too_Large1              = "Disk Image is larger than specked (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Addishunal data will be igno'ed"
Msg_FDC765_Disk_Too_Small1              = "Disk Image is smaller than specked (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Missin' data will be assoomd as emppy"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "Pen is touchin' graphic board"
Msg_TVOekaki_Pen_Away                   = "Pen is not touchin' graphic board"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Palette"
Msg_Palette_Disabled                    = "Palette viewer disabled"
Msg_Palette_Enabled                     = "Palette viewer inabled"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Messages"
Msg_Message_Disabled                    = "Message box disabled"
Msg_Message_Enabled                     = "Message box inabled"

; Technical Info'mashun box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Technical Info'mashun"
Msg_TechInfo_Disabled                   = "Technical info'mashun box disabled"
Msg_TechInfo_Enabled                    = "Technical info'mashun box inabled"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Tiles Viewer"
Msg_TilesViewer_Disabled                = "Tiles viewer disabled"
Msg_TilesViewer_Enabled                 = "Tiles viewer inabled"
Msg_TilesViewer_Tile                    = "Tile %d ($%X) @ %s"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Rapid Fire is now ackivated fo' Player %d Button %d"
Msg_RapidFire_JxBx_Off                  = "Rapid Fire is now disabled fo' Player %d Button %d"

; FM Unit (YM-2413 gritset) --------------------------------------------------
Msg_FM_Enabled                          = "FM Unit (YM-2413 gritset) emulashun inabled"
Msg_FM_Disabled                         = "FM Unit (YM-2413 gritset) emulashun disabled"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "Emulated machine is now European/US"
Msg_Country_JAP                         = "Emulated machine is now Japanese"

; Patchin' System ------------------------------------------------------------
Msg_Patch_Loading                       = "Loadin' MEKA.PAT (patches).. "
Msg_Patch_Missing                       = "On line %d: No checksum defined t'apply comman' to! Fry mah hide!"
Msg_Patch_Unrecognized                  = "On line %d: Syntax erro' o' unreckanized comman'! Fry mah hide!"
Msg_Patch_Value_Not_a_Byte              = "On line %d: Value %s does not fit into an unsigned byte!"
Msg_Patch_Out_of_Bound                  = "While patchin' %s: address $%04X is outta boun'aries. Patch not applied, cuss it all t' tarnation."

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "3-D Glasses emulashun inabled"
Msg_Glasses_Disabled                    = "3-D Glasses emulashun disabled"
Msg_Glasses_Show_Both                   = "3-D effeck kepp as is."
Msg_Glasses_Show_Left                   = "3-D effeck disabled by showin' only lef' side."
Msg_Glasses_Show_Right                  = "3-D effeck disabled by showin' only right side."
Msg_Glasses_Com_Port                    = "3-D Glasses emulashun inabled through COM Po't %d, cuss it all t' tarnation."
Msg_Glasses_Com_Port2                   = "(edit MEKA.CFG t'change th' Communicashun Po't)"
Msg_Glasses_Com_Port_Open_Error		    = "Error openin' COM po't %d, cuss it all t' tarnation. 3-D Glasses suppo't will not funckshun."
Msg_Glasses_Unsupported                 = "3-D Glasses seems t'be not suppo'ted by this hyar game."

; Inputs Initializashun ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Initializin' joystick. Shet mah mouth!. "
Msg_Inputs_Joy_Init_None                = "none foun'."
Msg_Inputs_Joy_Init_Found               = "%d foun'."
Msg_Inputs_Joy_Calibrate_Error          = "Erro' calibratin' joystick. Shet mah mouth! Abo'tin'."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Input peripheral: Joypad"
Msg_Inputs_LightPhaser                  = "Input peripheral: Light Phaser"
Msg_Inputs_PaddleControl                = "Input peripheral: Paddle Control"
Msg_Inputs_SportsPad                    = "Input peripheral: Spo'ts Pad"
Msg_Inputs_TVOekaki                     = "Input peripheral: TV Oekaki Graphic Board"
Msg_Inputs_Play_Digital                 = "Control wif digital input device"
Msg_Inputs_Play_Mouse                   = "Control wif analog input device (mouse)"
Msg_Inputs_Play_Pen                     = "Control pen wif mouse\nRight-click simulates removin' th' pen fum th' board"
Msg_Inputs_Play_Digital_Unrecommended   = "A digital input device (eg: keyboard) is suppo'ted but NOT recommended"

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Input Configurashun"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Loadin' MEKA.INP (inputs sources definishun).. "
Msg_Inputs_Src_Not_Enough               = "Not inough inputs sources foun'! Fry mah hide! Check MEKA.INP file o' try re-installin'."
Msg_Inputs_Src_Missing                  = "On line %d: No input source defined fo' sto'in' value!"
Msg_Inputs_Src_Equal                    = "On line %d: Missin' equal sign fo' variable assignment! Fry mah hide!"
Msg_Inputs_Src_Unrecognized             = "On line %d: Unreckanized variable \"%s\"! Fry mah hide!"
Msg_Inputs_Src_Syntax_Param             = "On line %d: Syntax erro' on variable pareemeter! Fry mah hide!"
Msg_Inputs_Src_Inconsistency            = "On line %d: Incoharncy wif previous settin's!"
Msg_Inputs_Src_Map_Keyboard             = "Now press th' key yer hankerin' t'assign t'this ackshun"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Ackshun assigned t'key \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Now press th' button/axis yer hankerin' t'assign t'this ackshun"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Ackshun assigned t'joypad stick %i, axis %i, %c."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Ackshun assigned t'joypad button %i."
Msg_Inputs_Src_Map_Mouse                = "Now press th' button yer hankerin' t'assign t'this ackshun"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Ackshun assigned t'mouse button %i."
Msg_Inputs_Src_Map_Mouse_No_A           = "So'ry - Mouse axis kinnot be changed"
Msg_Inputs_Src_Map_Cancelled            = "Assignment kincelled! Fry mah hide!"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Machine emulashun paused"
Msg_Machine_Resume                      = "Machine emulashun resoomd"
Msg_Machine_Reset                       = "Machine reseted"

; Virtual File Names ---------------------------------------------------------
;Msg_VFN_Loading                         = "Loadin' MEKA.VFN (virtual file names).. "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Loadin' MEKA.NAM (games names).. "
Msg_DB_Name_Default                     = "Game Screen"
Msg_DB_Name_NoCartridge                 = "No Cartridge"
Msg_DB_SyntaxError                      = "On line %d: Syntax erro'! Fry mah hide!"

; Configurashun File ---------------------------------------------------------
Msg_Config_Loading                      = "Loadin' %s (configurashun).. "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Loadin' MEKA.DAT (resources).. "

; Drivahs --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Fatal: Unknown Machine Drivah! Fry mah hide!"

; On overdump deteckshun -----------------------------------------------------
Msg_OverDump                            = "This hyar ROM corntains %d times th' necessary data.\nPlease download SMS Checker t'clean it (http://www.smspower.org/maxim/smschecker/)."

; Soun' ----------------------------------------------------------------------
Msg_Sound_Init                          = "Initializin' soun' system, dawgone it.."
Msg_Sound_Init_Error_Audio              = " - Erro' openin' Audio system, dawgone it. Try a diffrunt card an'/o' sample rate."
Msg_Sound_Init_Error_Blaster            = " - BLASTER variable not foun', disablin' FM soun'! Fry mah hide!"
Msg_Sound_Init_Error_Blaster_A          = " - BLASTER variable does not corntain address (A=xxx), disablin' FM soun'! Fry mah hide!"
Msg_Sound_Init_Error_Voices             = " - Voices initializashun failed, cuss it all t' tarnation."
Msg_Sound_Init_Error_Voice_N            = " - Voice #%d creashun failed, cuss it all t' tarnation."
Msg_Sound_Init_Soundcard                = " - Initializin' soun' card @ %d Hz, cuss it all t' tarnation.."
Msg_Sound_Init_Soundcard_No             = "Cain't initialize audio: no soun' card selecked! Fry mah hide!"
Msg_Sound_Volume_Changed                = "Voloom changed t'%d%%."
Msg_Sound_Stream_Error                  = "Soun' buffer stop. Restartin' stream, dawgone it."
Msg_Sound_Init_SN76496                  = " - SN76496 digital emulato':"
Msg_Sound_Init_YM2413_Digital           = " - YM2413 digital emulato':"

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Loadin' MEKA.THM (interface themes).. "
Msg_Theme_Error_Not_Enough              = "Not inough themes foun'! Fry mah hide! Check MEKA.THM file o' try re-installin'."
Msg_Theme_Error_Missing_Theme_Name      = "On line %d: No theme name defined fo' sto'in' value!"
Msg_Theme_Error_BG_Big                  = "Backgroun' pitcher t'small t'fit wif integer scalin'! Fry mah hide!"
Msg_Theme_Error_BG                      = "Erro' loadin' pitcher assosheeated wif theme!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loadin' ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Loadin' ROM.. "
Msg_LoadROM_Success                     = "Successfully loaded %s"
Msg_LoadDisk_Success                    = "Successfully loaded %s as a floppy disk"
Msg_LoadROM_Comment                     = "Comment: %s"
Msg_LoadROM_Warning                     = "-Warnin'-"
Msg_LoadROM_Bad_Dump_Long               = "This hyar ROM is known as bein' a bad dump.\nChances is thet it will not funckshun proper. Please download SMS Checker t'check it (http://www.smspower.org/maxim/smschecker/)."
Msg_LoadROM_SMSGG_Mode_Comment          = "This hyar is a Game Gear ROM runnin' in Master System compatibility mode."
Msg_LoadROM_Bad_Dump_Short              = "Warnin' - This hyar ROM is known as bein' a bad dump."
Msg_LoadROM_Product_Num                 = "Produck Number: %s"
Msg_LoadROM_SDSC                        = "-SDSC Header-"
Msg_LoadROM_SDSC_Name                   = "Name: %s"
Msg_LoadROM_SDSC_Version                = "Vershun: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Date: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Autho': %s"
Msg_LoadROM_SDSC_Release_Note           = "Release Note: %s"
Msg_LoadROM_SDSC_Unknown                = "<Unknown>"
Msg_LoadROM_SDSC_Error                  = "<Erro'>"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Load ROM"
Msg_FileBrowser_Drive                   = "[Drive %c]"
Msg_FileBrowser_Load                    = "Load"
Msg_FileBrowser_Close                   = "Close"
Msg_FileBrowser_LoadNames               = "Load Names"
Msg_FileBrowser_ReloadDir               = "Reload Dir"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "FM Instroomnts Edito'"
Msg_FM_Editor_Enabled                   = "FM Voice Edito' inabled"
Msg_FM_Editor_Disabled                  = "FM Voice Edito' disabled"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Frameskip value set to: Automatic (%d Hz)"
Msg_Frameskip_Standard                  = "Frameskip value set to: 1/%d"
Msg_FPS_Counter_Enabled                 = "FPS counter inabled"
Msg_FPS_Counter_Disabled                = "FPS counter disabled"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "LOG opshun needs a filename!"
Msg_Log_Session_Start                   = "-- Loggin' sesshun started on %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "LOAD opshun needs a savestate number! Fry mah hide!"
Msg_Load_Error                          = "Erro' openin' %s!"
Msg_Load_Not_Valid                      = "File %s is not a valid %s savegame!"
Msg_Load_Success                        = "State loaded fum %s"
Msg_Load_Version                        = "%s is an unsuppo'ted savegame vahshun! Fry mah hide!"
Msg_Load_Wrong_System                   = "%s is not a savegame fo' this hyar system! Fry mah hide!"
Msg_Load_Massage                        = "File is in MSD fo'mat - cornvahtin'"
Msg_Save_Not_in_BIOS                    = "Cain't save while runnin' BIOS! Fry mah hide!"
Msg_Save_Error                          = "Erro' writin' t'%s!"
Msg_Save_Success                        = "State saved t'%s"
Msg_Save_Slot                           = "Current savegame slot is: %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Opshuns"
Msg_Options_Close                       = "Close"
Msg_Options_Load_Close                  = "Close file browser af'er loadin' a game."
Msg_Options_Load_FullScreen             = "Switch t'fullscreen mode af'er loadin' a game."
Msg_Options_FullScreen_Messages         = "Show messages in fullscreen mode."
Msg_Options_GUI_VSync                   = "Wait fo' VSync in interface mode."
Msg_Options_NES_Enable                  = "Mario is a not fat plumber."
Msg_Options_BIOS_Enable                 = "Display BIOS logo."
Msg_Options_Bright_Palette              = "Enable bright palette on SMS an' GG."
Msg_Options_DB_Display                  = "Display names, flags an' icons fum database."
Msg_Options_Product_Number              = "Display produck number fum database."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "Language set t'%s"
Msg_Language_Set_Warning                = "Note: it is advised t'restart MEKA fo' all text t'update"

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Started dumpin' soun' output t'%s"
Msg_Sound_Dumping_Stop                  = "Stopped dumpin' soun' output (%2.02f seconds logged)"
Msg_Sound_Dumping_Error_File_1          = "Unable t'find a valid filename t'dump soun' output! Fry mah hide!"
Msg_Sound_Dumping_Error_File_2          = "Erro' openin' output file %s fo' dumpin'! Fry mah hide!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "VGM dumps will now be frame accurate (smaller files, skip voices)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "VGM dumps will now be sample accurate (mo' trimenjus files, dump voices)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(VGM dumpin' will hafta be restarted t'take account of noo accuracy)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "MAIN"
Msg_Menu_Main_LoadROM                   = "Load ROM"
Msg_Menu_Main_FreeROM                   = "Free ROM"
Msg_Menu_Main_SaveState                 = "Save State"
Msg_Menu_Main_LoadState                 = "Load State"
Msg_Menu_Main_Options                   = "Opshuns"
Msg_Menu_Main_Language                  = "Language"
Msg_Menu_Main_Quit                      = "Quit"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Dump                     = "Dump"
Msg_Menu_Debug_Enabled                  = "Enabled"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "MACHINE"
Msg_Menu_Machine_Power                  = "Power"
Msg_Menu_Machine_Power_On               = "On"
Msg_Menu_Machine_Power_Off              = "Off"
Msg_Menu_Machine_Country                = "Country"
Msg_Menu_Machine_Country_EU             = "European/US"
Msg_Menu_Machine_Country_Jap            = "Japanese"
Msg_Menu_Machine_TVType                 = "TV Type"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Hard Pause"
Msg_Menu_Machine_HardReset              = "Hard Reset"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VIDEO"
Msg_Menu_Video_FullScreen               = "Fullscreen"
Msg_Menu_Video_Themes                   = "Themes"
Msg_Menu_Video_Blitters                 = "Blitters"
Msg_Menu_Video_Layers                   = "Layers"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Backgroun'"
Msg_Menu_Video_Flickering               = "Flickerin'"
Msg_Menu_Video_Flickering_Auto          = "Auto"
Msg_Menu_Video_Flickering_Yes           = "Yessuh"
Msg_Menu_Video_Flickering_No            = "No"
Msg_Menu_Video_3DGlasses                = "3-D Glasses"
Msg_Menu_Video_3DGlasses_Enabled        = "Enabled"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Show both sides"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Show lef' side"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Show right side"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Uses Glasses on COM Po't"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "SOUN'"
Msg_Menu_Sound_FM                       = "FM Unit"
Msg_Menu_Sound_FM_Enabled               = "Inabled"
Msg_Menu_Sound_FM_Disabled              = "Disabled"
Msg_Menu_Sound_FM_Editor                = "Instroomnts Edito'"
Msg_Menu_Sound_Volume                   = "Voloom"
Msg_Menu_Sound_Volume_Mute              = "Mute"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Rate"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Channels"
Msg_Menu_Sound_Channels_Tone            = "Tone %d"
Msg_Menu_Sound_Channels_Noises          = "Noises"
Msg_Menu_Sound_Dump                     = "Dump"
Msg_Menu_Sound_Dump_WAV_Start           = "WAV Start"
Msg_Menu_Sound_Dump_WAV_Stop            = "WAV Stop"
Msg_Menu_Sound_Dump_VGM_Start           = "VGM Start"
Msg_Menu_Sound_Dump_VGM_Stop            = "VGM Stop"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "VGM Sample Accurate"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "INPUTS"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Light Phaser"
Msg_Menu_Inputs_PaddleControl           = "Paddle Control"
Msg_Menu_Inputs_SportsPad               = "Spo'ts Pad"
Msg_Menu_Inputs_GraphicBoard            = "Graphic Board"
Msg_Menu_Inputs_SK1100                  = "Sega Keyboard"
Msg_Menu_Inputs_RapidFire               = "Rapid Fire"
Msg_Menu_Inputs_RapidFire_PxBx          = "Player %d Button %d"
Msg_Menu_Inputs_Configuration           = "Configurashun"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "TOOLS"
Msg_Menu_Tools_Messages                 = "Messages"
Msg_Menu_Tools_Palette                  = "Palette"
Msg_Menu_Tools_TilesViewer              = "Tiles Viewer"
Msg_Menu_Tools_TechInfo                 = "Tech Info"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "HE'P"
Msg_Menu_Help_Documentation             = "He'p"
Msg_Menu_Help_About                     = "About"
Msg_Menu_Help_Documentation_W           = "Docoomntashun (Windows)"
Msg_Menu_Help_Documentation_U           = "Docoomntashun (Unix)"
Msg_Menu_Help_Compat                    = "Compatibility List"
Msg_Menu_Help_Multiplayer_Games         = "Multiplayer Games List"
Msg_Menu_Help_Changes                   = "Changes List"



;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Dutch]
WIP
; Dutch text door Mark van Tilburg
; Beheerder: Mark van Tilburg (markvt _@_ van-tilburg . info)
; Laatst bijgewerkt 07-07-2007
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Welkom bij %s (c) %s"
Msg_Window_Title                        = "MEKA - Wij Vertrouwen Op Pixels!"
Msg_Quit                                = "Slaap lekker, dappere strijder. goede nacht, monsterland...."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "Over Meka.."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "by %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Ok                                  = "Ok"
Msg_Failed                              = "Failed!"
Msg_Error_Base                          = "Error: %s"
Msg_Error_Error                         = "Fatal: error getting error type. Er moet iets fout zijn."
Msg_Error_Memory                        = "Niet genoeg geheugen!"
Msg_Error_Param                         = "Parameter \"%s\" is invalide!\n"
Msg_Error_Syntax                        = "Syntax fout!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Niet mogelijk naar de gewenste video mode te switchen (%dx%d)."
Msg_Error_Video_Mode_Back_To_GUI        = "Terugschakelen naar de grafische user interface."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "bestand niet gevonden!"
Msg_Error_File_Read                     = "onmogelijk om van bestand te lezen!"
Msg_Error_File_Empty                    = "bestand is leeg!"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "deze versie kan niet met ZIP bestanden werken!"
Msg_Error_ZIP_Loading                   = "niet mogelijk ZIP bestand te lezen"
Msg_Error_ZIP_Internal                  = "Interne error bij het openen van ZIP bestand!"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Error opening directory!"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Reset het spel om de wijzigingen te bekijken"
Msg_No_ROM                              = "Er is geen ROM geladen!"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Opstarten van Allegro.."
Msg_Init_GUI                            = "Opstarten grafische gebruikers interface"
Msg_Init_Completed                      = "[klaar]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Running                       = "Setup wordt uitgevoerd..."
Msg_Setup_Setup                         = "Setup"
Msg_Setup_Soundcard_Select              = "A.U.B. kies uw geluidskaart:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(Direct X is aanbevolen, als het beschikbaar is)"
Msg_Setup_SampleRate_Select             = "Sample rate:"
Msg_Setup_Debugger_Enable               = "Z80 debugger en bijbehorende functies inschakelen"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Screenfoto naar %s"
Msg_Capture_Error                       = "Niet mogelijk een schermfoto te maken!"
Msg_Capture_Error_File                  = "Niet mogelijk een bestandsnaam te vinden voor screenfoto!"

; On-Board Memory ------------------------------------------------------------
Msg_SRAM_Loaded                         = "SaveRAM van disk geladen (%d Kb)"
Msg_SRAM_Load_Unable                    = "niet mogelijk SaveRAM van disk te laden"
Msg_SRAM_Wrote                          = "SaveRAM to disk geschreven (%d Kb)"
Msg_SRAM_Write_Unable                   = "niet mogelijk SaveRAM naar disk te schrijven (%d Kb)"
Msg_93c46_Reset                         = "Herstarting 93c46"
Msg_93c46_Loaded                        = "geladen 93c46 EEPROM data van disk (%d bytes)"
Msg_93c46_Load_Unable                   = "niet mogelijk 93c46 EEPROM data van disk te laden"
Msg_93c46_Wrote                         = "93c46 EEPROM data naar disk geschreven (%d bytes)"
Msg_93c46_Write_Unable                  = "niet mogelijk 93c46 EEPROM data naar disk te schrijven (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "TV Type is nu %s"
Msg_TVType_Info_Speed                   = "(Originele snelheid is %d Hz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Laden van MEKA.BLT (video modes / blitters)... "
Msg_Blitters_Error_Not_Enough           = "niet genoeg blitters gevonden controleer MEKA.BLT herinstalleer"
Msg_Blitters_Error_Not_Found            = "Blitter niet gevonden!"
Msg_Blitters_Error_Missing              = "Op regel %d: Geen blitter naam ingesteld om de waarde te bewaren!"
Msg_Blitters_Error_Unrecognized         = "Op regel %d: niet herkende variable!"
Msg_Blitters_Error_Incorrect_Value      = "Op regel %d: Incorrect value!"
Msg_Blitters_Set                        = "Gebruikt nu \"%s\" volledig scherm."

; Nintendo -------------------------------------------------------------------
Msg_NES_Activate                        = "Dikke loodgieter mode geactiveerd"
Msg_NES_Sucks                           = "Fatale fout: gebruiker houd van Nintendo!"
Msg_NES_Mapper_Unknown                  = "Niet ondersteunde mapper: %d. Spel zou niet moeten functioneren"
Msg_NES_Deny_Facts                      = "Je kunt geen feiten ontkennen"

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Opstarten van de debugger "
Msg_Debug_Welcome                       = "Welkom bij de Meka Debugger!"
Msg_Debug_Not_Available                 = "Debugger is niet beschikbaar in deze versie!"
Msg_Debug_Brk_Need_Param                = "DEBUGBRK optie heeft een adres nodig!"
Msg_Debug_Trap_Read                     = "Op PC:%04X - lees van %04X"
Msg_Debug_Trap_Write                    = "Op PC:%04X - schrijf %02X tot %04X"
Msg_Debug_Trap_Port_Read                = "Op PC:%04X - Poort [%02X] lees"
Msg_Debug_Trap_Port_Write               = "Op PC:%04X - Poort [%02X] schrijf %02X"
Msg_Debug_Symbols_Loaded                = "%d symbolen geladen uit \"%s\"."
Msg_Debug_Symbols_Error                 = "Error bij het laden van de symbolen uit \"%s\":"
Msg_Debug_Symbols_Error_Line            = "Op regel %d: niet herkende syntax."

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Data dumps zullen gemaakt worden in ASCII mode"
Msg_DataDump_Mode_Raw                   = "Data dumps zullen gemaakt worden in RAW mode."
Msg_DataDump_Error                      = "Fout bij het maken van het bestand voor de dump %s!"
Msg_DataDump_Error_OB_Memory            = "Geen memory om te dumpen!"
Msg_DataDump_Error_Palette              = "Geen palet om te dumpen!"
Msg_DataDump_Error_Sprites              = "Geen sprites om te dumpen!"
Msg_DataDump_Main                       = "Dumped %s (%d %s) in %s mode"

; Documentation --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Documentatie"
Msg_Doc_File_Error                      = "Niet mogelijk de documentatie te openen!"
Msg_Doc_Enabled                         = "Documentatie geopend"
Msg_Doc_Disabled                        = "Documentatie gesloten"

; Sprite Flickering ----------------------------------------------------------
Msg_Flickering_Auto                     = "Sprite flikkeren zal automatisch gedaan worden."
Msg_Flickering_Yes                      = "Sprite flikkeren emulatie is nu aan."
Msg_Flickering_No                       = "Sprite flikkeren emulatie is nu uit."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Background herlaad uit"
Msg_Layer_BG_Enabled                    = "Background herlaad aan"
Msg_Layer_Spr_Disabled                  = "Sprites herlaad uit"
Msg_Layer_Spr_Enabled                   = "Sprites herlaad aan"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: onbekend disk lees commando %3i"
Msg_FDC765_Unknown_Write                = "FDC765: onbekend disk schrijf commando %3i"
Msg_FDC765_Disk_Too_Large1              = "Disk bestand is groter dan verwacht (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Extra data zal worden genegeerd"
Msg_FDC765_Disk_Too_Small1              = "Disk Image is kleiner dan verwacht (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Missende data wordt aangenomen als leeg"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "Pen raakt graphic board aan"
Msg_TVOekaki_Pen_Away                   = "Pen raakt graphic board niet aan"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Kleuren pallet"
Msg_Palette_Disabled                    = "Palet bekijker uit"
Msg_Palette_Enabled                     = "Palet bekijker aan"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Berichten"
Msg_Message_Disabled                    = "Berichtenbox uit"
Msg_Message_Enabled                     = "Berichtenbox aan"

; Technical Information box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Technische Informatie"
Msg_TechInfo_Disabled                   = "Technische informatie box uit"
Msg_TechInfo_Enabled                    = "Technische informatie box aan"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Tiles bekijker"
Msg_TilesViewer_Disabled                = "Tiles bekijker uit"
Msg_TilesViewer_Enabled                 = "Tiles bekijker aan"
Msg_TilesViewer_Tile                    = "Tile %d ($%X) @ %s"

; Memory Editor --------------------------------------------------------------
Msg_MemoryEditor_BoxTitle				= "Memory Editor"
Msg_MemoryEditor_Disabled				= "Memory Editor uitgeschakeld"
Msg_MemoryEditor_Enabled				= "Memory Editor ingeschakeld"
Msg_MemoryEditor_WriteZ80_Unable		= "Niet mogelijk om in Z80 addres te schrijven!"
Msg_MemoryEditor_Address_Out_of_Bound	= "Addres %s is niet binnen de toegestane %s grenzen!"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Snelschieten is geactiveerd voor speler %d knop %d"
Msg_RapidFire_JxBx_Off                  = "Snelschieten is gedeactiveerd voor speler %d knop %d"

; FM Unit (YM-2413 chipset) --------------------------------------------------
Msg_FM_Enabled                          = "FM Unit (YM-2413 chipset) emulatie aan"
Msg_FM_Disabled                         = "FM Unit (YM-2413 chipset) emulatie uit"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "Emulated machine is nu Europees/US"
Msg_Country_JAP                         = "Emulated machine is nu Japans"

; Patching System ------------------------------------------------------------
Msg_Patch_Loading                       = "Laden MEKA.PAT (patches)... "
Msg_Patch_Missing                       = "On line %d: Geen checksum ingesteld voor de patch!"
Msg_Patch_Unrecognized                  = "On line %d: Niet herkende instruktie!"
Msg_Patch_Value_Not_a_Byte              = "On line %d: waarde %s past niet in een unsigned byte!"
Msg_Patch_Out_of_Bound                  = "Tijdens het patchen van de ROM: geprobeerd om buiten het bestand te schrijven $%02X. Afbreken van de patch."

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "3-D bril emulatie aan"
Msg_Glasses_Disabled                    = "3-D bril emulatie uit"
Msg_Glasses_Show_Both                   = "3-D effect hetzelfde"
Msg_Glasses_Show_Left                   = "3-D effect uit u ziet alleen de linker kant."
Msg_Glasses_Show_Right                  = "3-D effect uit u ziet alleen de rechtse kant"
Msg_Glasses_Com_Port                    = "3-D Glasses emulatie geactiveerd door COM Poort %d."
Msg_Glasses_Com_Port2                   = "(wijzig MEKA.CFG om de te gebruiken COM poort in te stellen.)"
Msg_Glasses_Com_Port_Open_Error		    = "Error bij het openen van COM poort %d. 3-D bril ondersteuning zal niet werken."
Msg_Glasses_Unsupported                 = "Het lijkt erop alsof dit spel de 3d bril niet ondersteunt"

; Inputs Initialization ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Initializeren joystick.. "
Msg_Inputs_Joy_Init_None                = "geen gevonden."
Msg_Inputs_Joy_Init_Found               = "%d gevonden."
Msg_Inputs_Joy_Calibrate_Error          = "Error bij het afstellen van de joystick. Afbrekende."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Input apparaat: Joypad"
Msg_Inputs_LightPhaser                  = "Input apparaat: Light Phaser"
Msg_Inputs_PaddleControl                = "Input apparaat: Paddle Control"
Msg_Inputs_SportsPad                    = "Input apparaat: Sports Pad"
Msg_Inputs_TVOekaki                     = "Input apparaat: TV Oekaki Graphic Board"
Msg_Inputs_Play_Digital                 = "Controle met digitaal input device"
Msg_Inputs_Play_Mouse                   = "Controle met analoog input device (mouse)"
Msg_Inputs_Play_Digital_Unrecommended   = "Een dingitaal input apparaat (bv: toetsenbord) is ondersteund maar NIET aanbevolen"
Msg_Inputs_Play_Pen                     = "Controle pen with mouse\nRight-click simulates removing the pen from the board"
Msg_Inputs_SK1100_Enabled               = "Sega toetsenbord (SK-1100) emulatie ingeschakeld"
Msg_Inputs_SK1100_Disabled              = "Sega toetsenbord(SK-1100) emulatie uitgeschakeld"

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Input Configuratie"
Msg_Inputs_Config_Peripheral_Click      = "Klik om de accesoire te selecteren"
Msg_Inputs_Config_Source_Enabled        = "Ingeschakeld"
Msg_Inputs_Config_Source_Player         = "Bestuurd speler %d"
Msg_Inputs_Config_Source_Emulate_Joypad = "Joypad emuleren"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Laden van MEKA.INP (voegt sources definitions in)... "
Msg_Inputs_Src_Not_Enough               = "niet genoeg input sources gevonden! raadpleeg MEKA.INP bestand, of herinstalleer."
Msg_Inputs_Src_Missing                  = "On line %d: Geen input signaal om de waarde toe te kennen!"
Msg_Inputs_Src_Equal                    = "On line %d: Mist hetzelfde signaal voor de waarde!"
Msg_Inputs_Src_Unrecognized             = "On line %d: niet herkende variable \"%s\"!"
Msg_Inputs_Src_Syntax_Param             = "On line %d: Syntax error bij variabele parameter"
Msg_Inputs_Src_Inconsistency            = "On line %d: niet werkend met vorige instelling!"
Msg_Inputs_Src_Map_Keyboard             = "druk op de knop die je met deze aktie wilt gebruiken"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Aktie vastgemaakt aan knop \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Druk op de knop/as die je aan deze aktie wilt koppelen"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Aktie vastgemaakt aan joypad knuppel %i, as %i, %c."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Aktie vastgemaakt aan joypad knop %i."
Msg_Inputs_Src_Map_Mouse                = "Druk op de knop die je aan deze aktie wilt koppelen"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Aktie gekoppeld aan muis knop %i."
Msg_Inputs_Src_Map_Mouse_No_A           = "Sorry - Muis assen kunnen niet wordne verandert"
Msg_Inputs_Src_Map_Cancelled            = "koppeling geannuleerd!"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Machine emulatie gepauzeerd"
Msg_Machine_Resume                      = "Machine emulatie hervat"
Msg_Machine_Reset                       = "Machine reset"

; Filenames Database ---------------------------------------------------------
Msg_FDB_Loading                         = "Laden van MEKA.FDB (bestandsnamen database)... "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Laden van MEKA.NAM (Spel namen)... "
Msg_DB_Name_Default                     = "Spel scherm"
Msg_DB_Name_NoCartridge                 = "Geen Cartridge"
Msg_DB_SyntaxError                      = "Op regel %d: Syntax fout!"

; Configuration File ---------------------------------------------------------
Msg_Config_Loading                      = "Laden van %s (configuratie)... "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Laden van MEKA.DAT (resources)... "

; Drivers --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Fataal: Onbekende Machine Driver!"

; Overdump detection ---------------------------------------------------------
Msg_OverDump                            = "Deze ROM bevat %d keer de noodzakelijke data.\nDownload SMS Checker om deze data te verwijderen (http://www.smspower.org/maxim/smschecker/)."

; Sound ----------------------------------------------------------------------
Msg_Sound_Init                          = "Initialiseren geluids systeem.."
Msg_Sound_Init_Error_Audio              = " - Error opening Audio systeem. probeer een andere kaart of sample rate."
Msg_Sound_Init_Error_Blaster            = " - BLASTER variable niet gevonden, uitzetten van FM sound!"
Msg_Sound_Init_Error_Blaster_A          = " - BLASTER variable bevat het adres (A=xxx) niet, uitzetten van FM sound!"
Msg_Sound_Init_Error_Voices             = " - Voices initialisatie gefaaild."
Msg_Sound_Init_Error_Voice_N            = " - Voice #%d creatie gefaald."
Msg_Sound_Init_Soundcard                = " - Initialiseren geluidskaart @ %d Hz.."
Msg_Sound_Init_Soundcard_No             = "Kan audio niet initialiseren: geen geluidskaart geselecteerd!"
Msg_Sound_Init_SN76496                  = " - SN76496 digitale emulator:"
Msg_Sound_Init_YM2413_Digital           = " - YM2413 digitale emulator:"
Msg_Sound_Stream_Error                  = "Sound buffer stop. herstarten van stream."
Msg_Sound_Volume_Changed                = "Volume gewijzigd naar %d%%."

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Laden van MEKA.THM (interface themes)... "
Msg_Theme_Error_Not_Enough              = "niet genoeg themes gevonden! bekijk MEKA.THM bestand her installeer"
Msg_Theme_Error_Missing_Theme_Name      = "On line %d: Geen theme name gespecificeerd om waarde op te slaan!"
Msg_Theme_Error_Syntax                  = "Op regel %d: syntax error!"
Msg_Theme_Error_Attribute_Defined       = "Op regel %d: attribuut al gedefinieerd!"
Msg_Theme_Error_Out_of_Bound            = "Op regel %d: waarde niet toegestaan!"
Msg_Theme_Error_Theme_Missing_Data      = "Theme \"%s\" is missing data. Will be disabled."
Msg_Theme_Error_BG_Big                  = "Achtergrond plaatje naar klein om te passen met een integere schaling!"
Msg_Theme_Error_BG                      = "Error bij het laden van het plaatje wat bij het theme hoort!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Laden van ROM.. "
Msg_LoadROM_Success                     = "Succesvol geladen %s"
Msg_LoadDisk_Success                    = "Succesvol geladen %s als een floppy disk"
Msg_LoadROM_Comment                     = "Commentaar: %s"
Msg_LoadROM_SMSGG_Mode_Comment          = "Dit is een Game Gear rom die wordt geemuleerd in Master System modus."
Msg_LoadROM_Warning                     = "-Waarschuwing-"
Msg_LoadROM_Bad_Dump_Long               = "Deze ROM staat bekend als een slechte dump.\nEr bestaat een kans dat deze niet goed werkt. Download SMS Checker om deze rom te controleren (http://www.smspower.org/maxim/smschecker/)."
Msg_LoadROM_Bad_Dump_Short              = "Waarschuwing - Deze ROM staat bekend als een slechte dump."
Msg_LoadROM_Product_Num                 = "Produkt Nummer: %s"
Msg_LoadROM_SDSC                        = "-SDSC Hoofd-"
Msg_LoadROM_SDSC_Name                   = "Naam: %s"
Msg_LoadROM_SDSC_Version                = "Versie: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Datum: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Auteur: %s"
Msg_LoadROM_SDSC_Release_Note           = "Release Notities: %s"
Msg_LoadROM_SDSC_Unknown                = "<Onbekend>"
Msg_LoadROM_SDSC_Error                  = "<Fout>"
Msg_LoadROM_Reload_Reloaded             = "Rom geladen ROM."
Msg_LoadROM_Reload_No_ROM               = "Geen ROM om te herladen!"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Laad ROM"
Msg_FileBrowser_Drive                   = "[Drive %c]"
Msg_FileBrowser_Load                    = "Laad"
Msg_FileBrowser_Close                   = "Sluit"
Msg_FileBrowser_LoadNames               = "Laad Namen"
Msg_FileBrowser_ReloadDir               = "Herlaad Dir"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "FM Instrumenten Editor"
Msg_FM_Editor_Enabled                   = "FM Voice Editor aan"
Msg_FM_Editor_Disabled                  = "FM Voice Editor uit"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Frameskip waarde gezet op: Automatisch (%d Hz)"
Msg_Frameskip_Standard                  = "Frameskip waarde gezet op: 1/%d"
Msg_FPS_Counter_Enabled                 = "FPS teller ingeschakeld"
Msg_FPS_Counter_Disabled                = "FPS teller uitgeschakeld"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "LOG optie heeft een bestandsnaam nodig!"
Msg_Log_Session_Start                   = "-- Logge sessie gestart op %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "LAAD optie heeft een savestate nummer nodig!"
Msg_Load_Error                          = "Error openen van %s!"
Msg_Load_Not_Valid                      = "File %s is geen valide %s savegame!"
Msg_Load_Success                        = "State geladen van %s"
Msg_Load_Version                        = "%s is een niet ondersteunde savegame versie!"
Msg_Load_Wrong_System                   = "%s is geen savegame voor dit systeem!"
Msg_Load_Massage                        = "bestand is in MSD formaat - omzetten"
Msg_Save_Not_in_BIOS                    = "Kan niet opslaan wanneer de BIOS draait"
Msg_Save_Error                          = "Error bij het schrijven naar %s!"
Msg_Save_Success                        = "Status opgeslagen naar %s"
Msg_Save_Slot                           = "Dit savegame slot is: %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Opties"
Msg_Options_Close                       = "Sluit"
Msg_Options_BIOS_Enable                 = "Laat BIOS logo zien."
;Msg_Options_BIOS_Enable                 = "laat intern bios logo zien als het op het systeem zit."
Msg_Options_DB_Display                  = "Laat namen, vlaggen en iconen zien uit de database."
Msg_Options_Product_Number              = "Laat product nummer zien uit de database."
;Msg_Options_Product_Number              = "laat spel produkt nummer zien als het in de database staat."
Msg_Options_Bright_Palette              = "helder kleurenpalet voor Master System / Game Gear."
Msg_Options_Allow_Opposite_Directions   = "Toestaan indrukken van tegenovergestelde joypad richtingen."
Msg_Options_Load_Close                  = "sluit bestandsselecteerder na openen."
Msg_Options_Load_FullScreen             = "Schakel over naar fullscreen na een spel te laden."
;Msg_Options_VFN_Support                 = "Laat lange bestandsnamen zien en landen vlaggen."
Msg_Options_FullScreen_Messages         = "laat berichten zien in fullscreen mode."
Msg_Options_GUI_VSync                   = "Wacht for VSync in interface mode."
;Msg_Options_Capture_Automatic_Crop_Align= "Screenshots automatisch vergroten / verkleinen en uitlijnen."
Msg_Options_NES_Enable                  = "Mario is geen dikke loodgieter."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "Taal gezet op %s"
Msg_Language_Set_Warning                = "Notitie: het is aanbevolen meka te herstarten om er zeker van te zijn dat alle text wordt geupdated."

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "geStart dumping geluid naar %s"
Msg_Sound_Dumping_Stop                  = "geStopt dumping geluid naar (%2.02f seconden gelogd)"
Msg_Sound_Dumping_Error_File_1          = "niet mogelijk een valide bestandsnaam te vinden om de geluiddump in te laten!"
Msg_Sound_Dumping_Error_File_2          = "Error bij het openen van het ouput bestand %s voor het dumpen!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "VGM dumps zullen nu beter met frames werken (kleinere bestanden, stemmen overslaan)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "VGM dumps zullen nu beter met samples zijn (grotere bestanden, stemmen ook dumpen)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(VGM dumpen moet opnieuw opgestart worden om gebruik te maken van de nieuwe accuraatheid)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "BESTAND"
Msg_Menu_Main_LoadROM                   = "Laad ROM"
Msg_Menu_Main_FreeROM                   = "Haal ROM eruit"
Msg_Menu_Main_SaveState                 = "Status opslaan"
Msg_Menu_Main_LoadState                 = "Status laden"
Msg_Menu_Main_Options                   = "Opties"
Msg_Menu_Main_Language                  = "Taal"
Msg_Menu_Main_Quit                      = "Stoppen"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Enabled                  = "Ingeschakeld"
Msg_Menu_Debug_Reload_ROM               = "Herlaad ROM"
Msg_Menu_Debug_Dump                     = "Opslaan"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "MACHINE"
Msg_Menu_Machine_Power                  = "Power"
Msg_Menu_Machine_Power_On               = "Aan"
Msg_Menu_Machine_Power_Off              = "Uit"
Msg_Menu_Machine_Country                = "Land"
Msg_Menu_Machine_Country_EU             = "Europees/US"
Msg_Menu_Machine_Country_Jap            = "Japans"
Msg_Menu_Machine_TVType                 = "TV Type"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Harde Pause"
Msg_Menu_Machine_HardReset              = "Harde Reset"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VIDEO"
Msg_Menu_Video_FullScreen               = "Volledig scherm"
Msg_Menu_Video_Themes                   = "Thema's"
Msg_Menu_Video_Blitters                 = "Blitters"
Msg_Menu_Video_Layers                   = "Lagen"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Achtergrond"
Msg_Menu_Video_Flickering               = "Flikkeren"
Msg_Menu_Video_Flickering_Auto          = "Automatisch"
Msg_Menu_Video_Flickering_Yes           = "Ja"
Msg_Menu_Video_Flickering_No            = "Nee"
Msg_Menu_Video_3DGlasses                = "3-D Bril"
Msg_Menu_Video_3DGlasses_Enabled        = "Aan"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Laat beide kanten zien"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Laat linkerkant zien"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Laat rechterkant zien"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Gebruik bril op COM Poort"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "GELUID"
Msg_Menu_Sound_FM                       = "FM Unit"
Msg_Menu_Sound_FM_Enabled               = "Aan"
Msg_Menu_Sound_FM_Disabled              = "Uit"
Msg_Menu_Sound_FM_Editor                = "Instrumenten Editor"
Msg_Menu_Sound_Volume                   = "Volume"
Msg_Menu_Sound_Volume_Mute              = "Mute"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Rate"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Kanalen"
Msg_Menu_Sound_Channels_Tone            = "Toon %d"
Msg_Menu_Sound_Channels_Noises          = "Geluiden"
Msg_Menu_Sound_Dump                     = "Opslaan"
Msg_Menu_Sound_Dump_WAV_Start           = "WAV Start"
Msg_Menu_Sound_Dump_WAV_Stop            = "WAV Stop"
Msg_Menu_Sound_Dump_VGM_Start           = "VGM Start"
Msg_Menu_Sound_Dump_VGM_Stop            = "VGM Stop"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "VGM Sample Accuratie"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "CONTROLS"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Licht pistool"
Msg_Menu_Inputs_PaddleControl           = "Paddle Controller"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Graphisch Bord"
Msg_Menu_Inputs_SK1100                  = "Sega Toetsenbord"
Msg_Menu_Inputs_RapidFire               = "Snel vuren"
Msg_Menu_Inputs_RapidFire_PxBx          = "Speler %d Knop %d"
Msg_Menu_Inputs_Configuration           = "Configuratie"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "WERKSET"
Msg_Menu_Tools_Messages                 = "Berichten"
Msg_Menu_Tools_Palette                  = "Kleuren Palet"
Msg_Menu_Tools_TilesViewer              = "Tegel bekijker"
Msg_Menu_Tools_TilemapViewer            = "Tegelkaart Viewer"
Msg_Menu_Tools_TechInfo                 = "Technische Info"
Msg_Menu_Tools_MemoryEditor             = "Geheugen Editor"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "HELP"
Msg_Menu_Help_Documentation             = "Documentatie"
Msg_Menu_Help_Documentation_W           = "Documentatie (Windows)"
Msg_Menu_Help_Documentation_U           = "Documentatie (Unix)"
Msg_Menu_Help_Compat                    = "Compatibiliteits lijst"
Msg_Menu_Help_Multiplayer_Games         = "Mp spellen lijst"
Msg_Menu_Help_Changes                   = "Veranderingen"
Msg_Menu_Help_Debugger					= "Debugger"
Msg_Menu_Help_About                     = "Over"

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Español]
WIP
; Translation by Antonio Bazalo (Bazathlon)
; Maintainer: Antonio Bazalo (bazathlon AT wanadoo.es)
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Bienvenido a %s (c) %s"
;Msg_Window_Title                        = "MEKA - ¡8-bit o muere!"
Msg_Quit                                = "Good night, brave warrior. Good night, monster-land...."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "Acerca de..."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "por %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Failed                              = "¡Fallo!"
Msg_Error_Base                          = "Error: %s"
Msg_Error_Error                         = "Error fatal: algo debe ir mal."
Msg_Error_Memory                        = "No hay suficiente memoria"
Msg_Error_Param                         = "¡El parametro \"%s\" es invalido!\n"
Msg_Error_Syntax                        = "¡Error de sintaxis!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Inhabilitado el modo de video deseado (%dx%d)."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "Archivo no encontrado"
Msg_Error_File_Read                     = "No se puede leer del archivo"
Msg_Error_File_Empty                    = "Archivo vacio"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "Esta version no soporta archivos ZIP"
Msg_Error_ZIP_Loading                   = "Incapaz de leer archivos ZIP"
Msg_Error_ZIP_Internal                  = "Error interno durante la carga del archivo ZIP"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Error abriendo el directorio"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Resetea el juego para que los cambios surtan efecto"
Msg_No_ROM                              = "Ninguna ROM fue cargada"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Iniciando Allegro.."
Msg_Init_GUI                            = "Iniciando GUI.."
Msg_Init_Completed                      = "[PREPARADO]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Soundcard_Select              = "Por favor seleccione su dispositivo de audio:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(Direct X es altamente recomendado, si esta disponible)"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Pantalla capturada a %s"
Msg_Capture_Error                       = "Incapaz de crear captura de pantalla"
Msg_Capture_Error_File                  = "Incapaz de encontrar un nombre de fichero valido para salvar la pantalla capturada"

; On-Board Memory ------------------------------------------------------------
Msg_SRAM_Loaded                         = "Cargada SRAM desde disco (%d Kb)"
Msg_SRAM_Load_Unable                    = "Incapaz de cargar SRAM desde disco"
Msg_SRAM_Wrote                          = "Escribio SRAM a disco (%d Kb)"
Msg_SRAM_Write_Unable                   = "Incapaz de escribir SRAM a disco (%d Kb)"
Msg_93c46_Reset                         = "Reseteando 93c46"
Msg_93c46_Loaded                        = "Cargado datos del 93c46 EEPROM desde disco (%d bytes)"
Msg_93c46_Load_Unable                   = "Incapaz de cargar datos del 93c46 EEPROM desde disco"
Msg_93c46_Wrote                         = "Escribio datos del 93c46 EEPROM a disco (%d bytes)"
Msg_93c46_Write_Unable                  = "Incapaz de escribir datos del 93c46 EEPROM a disco (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "Tipo de TV es ahora %s"
Msg_TVType_Info_Speed                   = "(Velocidad original es %d Hz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Cargando MEKA.BLT (video modes / blitters)... "
Msg_Blitters_Error_Not_Enough           = "Insuficientes blitters encontrados! Comprueba el archivo MEKA.BLT o reinstala MEKA"
Msg_Blitters_Error_Not_Found            = "Blitter no encontrado"
Msg_Blitters_Error_Missing              = "On line %d: Sin nombre definido de blitter para almacenar valores!"
Msg_Blitters_Error_Unrecognized         = "On line %d: Variable irreconocible"
Msg_Blitters_Set                        = "Usando \"%s\" blitter a pantalla completa"

; Nintendo -------------------------------------------------------------------
Msg_NES_Activate                        = "Modo Fat plumber activado"
Msg_NES_Sucks                           = "Error fatal: al usuario le gusta Nintendo!"
Msg_NES_Mapper_Unknown                  = "Mapeado no soportado: %d. El juego no deberia funcionar"
Msg_NES_Deny_Facts                      = "Puedes negar los hechos."

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Iniciando debugger.. "
Msg_Debug_Welcome                       = "Bienvenido al MEKA Debugger!"
Msg_Debug_Not_Available                 = "Debugger no disponible en esta version!"
Msg_Debug_Brk_Need_Param                = "La opcion DEBUGBRK necesita una direccion!"
Msg_Debug_Trap_Read                     = "A PC:%04X - Leido desde %04X"
Msg_Debug_Trap_Write                    = "A PC:%04X - Escrito %02X a %04X"
Msg_Debug_Trap_Port_Read                = "A PC:%04X - Puerto [%02X] Leido"
Msg_Debug_Trap_Port_Write               = "A PC:%04X - Puerto [%02X] Escrito %02X"

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Datos de dumpeo ahora estaran hechos en codigo ASCII"
Msg_DataDump_Mode_Raw                   = "Datos de dumpeo ahora estaran hechos en modo Raw."
Msg_DataDump_Error                      = "Error creando archivo a dumpear %s!"
Msg_DataDump_Error_OB_Memory            = "Sin memoria en placa para dumpear!"
Msg_DataDump_Error_Palette              = "Sin paleta para dumpear!"
Msg_DataDump_Error_Sprites              = "Sin sprites para dumpear!"
Msg_DataDump_Main                       = "Dumpeo %s (%d %s) en %s modo"

; Documentation --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Documentación"
Msg_Doc_File_Error                      = "Incapaz de abrir documentacion."
Msg_Doc_Enabled                         = "Documentacion mostrandose."
Msg_Doc_Disabled                        = "No hay mas documentacion."

; Sprite Flickering ----------------------------------------------------------
Msg_Flickering_Auto                     = "Sprite flickering sera manejado automacicamente."
Msg_Flickering_Yes                      = "Emulacion de Sprite flickering activada."
Msg_Flickering_No                       = "Emulacion de Sprite flickering desactivada."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Refresco de fondo desactivada"
Msg_Layer_BG_Enabled                    = "Refresco de fondo activada"
Msg_Layer_Spr_Disabled                  = "Refresco de sprites desactivada"
Msg_Layer_Spr_Enabled                   = "Refresco de sprites activada"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Comando de lectura de disco desconocido %3i"
Msg_FDC765_Unknown_Write                = "FDC765: Comando de escritura en disco desconocido %3i"
Msg_FDC765_Disk_Too_Large1              = "Imagen de disco es mas grande de lo esperado (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Datos adicionales seran ignorados"
Msg_FDC765_Disk_Too_Small1              = "La imagen del disco es más pequeña de lo esperado (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Datos perdidos serán asumidos como vacíos"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "La pluma esta tocando la tableta grafica"
Msg_TVOekaki_Pen_Away                   = "La pluma no esta tocando la tableta grafica"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Paletas"
Msg_Palette_Disabled                    = "Visor de paletas desactivado"
Msg_Palette_Enabled                     = "Visor de paletas activado"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Mensajes"
Msg_Message_Disabled                    = "Caja de mensajes desactivada"
Msg_Message_Enabled                     = "Caja de mensajes activada"

; Technical Information box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Información técnica"
Msg_TechInfo_Disabled                   = "Caja de informacion tecnica desativada"
Msg_TechInfo_Enabled                    = "Caja de informacion tecnica activada"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Visor de retales"
Msg_TilesViewer_Disabled                = "Visor de retales desactivado"
Msg_TilesViewer_Enabled                 = "Visor de retales activado"
Msg_TilesViewer_Tile                    = "Retal %d ($%X) @ %s"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Fuego rapido activado para jugador %d boton %d"
Msg_RapidFire_JxBx_Off                  = "Fuego rapido desactivado para jugador %d boton %d"

; FM Unit (YM-2413 chipset) --------------------------------------------------
Msg_FM_Enabled                          = "Emulacion de la unidad FM (YM-2413 chipset) activada"
Msg_FM_Disabled                         = "Emulacion de la unidad FM (YM-2413 chipset) desactivada"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "La maquina emulada es ahora europea/americana"
Msg_Country_JAP                         = "La maquina meulada es ahora japonesa"

; Patching System ------------------------------------------------------------
Msg_Patch_Loading                       = "Cargando MEKA.PAT (parches)... "
;Msg_Patch_Missing                       = "On line %d: No hay suma de comprobaciones definidas para esta instruccion"
;Msg_Patch_Unrecognized                  = "On line %d: Instruccion no reconocida"
;Msg_Patch_Out_of_Bound                  = "Durante el parcheo de la ROM: intentando escribir fuera de limites, en la positcion $%02X. Abortando parcheo"

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "Emulacion de gafas 3D activada"
Msg_Glasses_Disabled                    = "Emulacion de gafas 3D desactivada"
Msg_Glasses_Show_Both                   = "Efecto 3D mantenido tal como es"
Msg_Glasses_Show_Left                   = "Efecto 3D desactivado por mostrar solo el lado izquierdo"
Msg_Glasses_Show_Right                  = "Efecto 3D desactivado por mostrar solo el lado derecho"
Msg_Glasses_Com_Port                    = "Emulacion de gafas 3D activada a traves del puerto COM %d."
;Msg_Glasses_Com_Port2                   = "(editar MEKA.CFG para cambiar el puerto de comunicacion)"
Msg_Glasses_Unsupported                 = "Este juego parece no soportar gafas 3D"

; Inputs Initialization ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Iniciando joystick... "
Msg_Inputs_Joy_Init_None                = "ninguno encontrado."
Msg_Inputs_Joy_Init_Found               = "%d encontrado."
Msg_Inputs_Joy_Calibrate_Error          = "Error calibrando joystick. Abortando."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Periferico de entrada: Joypad"
Msg_Inputs_LightPhaser                  = "Periferico de entrada: Light Phaser"
Msg_Inputs_PaddleControl                = "Periferico de entrada: Paddle Control"
Msg_Inputs_SportsPad                    = "Periferico de entrada: Sports Pad"
Msg_Inputs_TVOekaki                     = "Periferico de entrada: TV Oekaki Graphic Board"
Msg_Inputs_Play_Digital                 = "Control con dispositivo de entrada digital"
Msg_Inputs_Play_Mouse                   = "Control con dispositivo de entrada analogico (raton)"
Msg_Inputs_Play_Pen                     = "Control de pluma con raton\nboton derecho simula quitando la pluma de la tableta"

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Configuración de entrada"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Cargando MEKA.INP (definicion de rutas de entrada)... "
Msg_Inputs_Src_Not_Enough               = "Insuficientes caminos de entrada encontrados! Comprueba el archivo MEKA.INP o reinstala MEKA"
Msg_Inputs_Src_Missing                  = "On line %d: Sin camino de entrada definido para almacenar valores"
Msg_Inputs_Src_Equal                    = "On line %d: Señal de equilibrio perdida para asignamiento de variable"
Msg_Inputs_Src_Unrecognized             = "On line %d: Variable irreconocible \"%s\"!"
Msg_Inputs_Src_Syntax_Param             = "On line %d: Error de sintaxis en parametro de variable!"
Msg_Inputs_Src_Inconsistency            = "On line %d: Inconsistencia con la configuracion previa!"
Msg_Inputs_Src_Map_Keyboard             = "Ahora presione la tecla que quiera asignar a esta accion"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Accion asignada a la tecla \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Ahora presione el boton/eje que quiera asignar a esta accion"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Accion asignada al control del joypad %i, eje %i, %c."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Accion asignada al boton de joypad %i."
Msg_Inputs_Src_Map_Mouse                = "Ahora presione el boton que quiera asignar a esta accion"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Accion asignada al boton de raton %i."
Msg_Inputs_Src_Map_Mouse_No_A           = "Lo siento - el eje de raton no ha podido ser cambiado"
Msg_Inputs_Src_Map_Cancelled            = "Asignamiento cancelado!"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Emulacion de maquina pausada"
Msg_Machine_Resume                      = "Emulacion de maquina reanudada"
Msg_Machine_Reset                       = "Maquina reseteada"

; Filenames Database ---------------------------------------------------------
;Msg_VFN_Loading                         = "Cargando MEKA.VFN (nombres de archivo virtuales)... "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Cargando MEKA.NAM (nombres de juegos)... "
Msg_DB_Name_Default                     = "Pantalla de juego"
Msg_DB_Name_NoCartridge                 = "No hay cartucho"

; Configuration File ---------------------------------------------------------
Msg_Config_Loading                      = "Cargando %s (configuracion)... "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Cargando MEKA.DAT (recursos)... "

; Drivers --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Fatal: Driver de maquina desconocido!"

; Overdump detection ---------------------------------------------------------
Msg_OverDump                            = "Esta ROM contiene %d veces los datos necesarios.\nGet \"SMS Checker\" para limpiarlo."

; Sound ----------------------------------------------------------------------
Msg_Sound_Init                          = "Iniciando sistema de sonido..."
Msg_Sound_Init_Error_Audio              = " - Error abriendo sistema de audio. Intente una tarjeta diferente y/o ratio de frecuencia."
Msg_Sound_Init_Error_Blaster            = " - BLASTER variable no encontrada, desactivando sonido FM!"
Msg_Sound_Init_Error_Blaster_A          = " - BLASTER variable no cuenta con direccion (A=xxx), desactivando sonido FM!"
Msg_Sound_Init_Error_Voices             = " - Voces iniciacion fallo."
Msg_Sound_Init_Error_Voice_N            = " - Voces #%d creacion fallo."
Msg_Sound_Init_Soundcard                = " - Iniciando tarjeta de sonido @ %d Hz..."
Msg_Sound_Init_Soundcard_No             = "No se pudo iniciar el audio: no hay tarjeta de sonido seleccionada!"
;Msg_Sound_Init_SN76496_Ok               = " - SN76496 emulador: OK"
;Msg_Sound_Init_YM2413_Ok                = " - YM2413 emulador: OK"
Msg_Sound_Stream_Error                  = "Buffer de sonido detenido. Reempezando stream."

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Cargando MEKA.THM (temas de interfaz)... "
Msg_Theme_Error_Not_Enough              = "No hay suficientes temas encontrados! Comprueba el archivo MEKA.THM o intenta reinstalando MEKA."
Msg_Theme_Error_Missing_Theme_Name      = "On line %d: Sin nombre de tema definido para almacenar valores!"
Msg_Theme_Error_BG_Big                  = "Dibujo de fondo demasiado pequeño para agrandar con scaling integrado!"
Msg_Theme_Error_BG                      = "Error cargando dibujo asociado con juego!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Cargando ROM... "
Msg_LoadROM_Success                     = "Satisfactoriamente cargado %s"
Msg_LoadDisk_Success                    = "Satisfactoriamente cargado %s como un disco flexible"
Msg_LoadROM_Comment                     = "Comentario: %s"
Msg_LoadROM_Warning                     = "-Advertencia-"
;Msg_LoadROM_Bad_Dump_Long               = "This ROM parece estar mal dumpeada.\nPosibilidades de que no funcionara propiamente."
Msg_LoadROM_Bad_Dump_Short              = "Advertencia - Esta ROM parece estar mal dumpeada."
Msg_LoadROM_Product_Num                 = "Numero de producto: %s"
Msg_LoadROM_SDSC                        = "-SDSC Header-"
Msg_LoadROM_SDSC_Name                   = "Nombre: %s"
Msg_LoadROM_SDSC_Version                = "Version: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Fecha: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Autor: %s"
Msg_LoadROM_SDSC_Release_Note           = "Nota de edicion: %s"
Msg_LoadROM_SDSC_Unknown                = "<Desconocido>"
Msg_LoadROM_SDSC_Error                  = "<Error>"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Cargar ROM"
Msg_FileBrowser_Drive                   = "[Unidad %c]"
Msg_FileBrowser_Load                    = "Cargar"
Msg_FileBrowser_Close                   = "Cerrar"
Msg_FileBrowser_LoadNames               = "Cargar nombres"
Msg_FileBrowser_ReloadDir               = "Recargar Dir"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "Editor FM"
Msg_FM_Editor_Enabled                   = "Editor de voz FM activado"
Msg_FM_Editor_Disabled                  = "Editor de voz FM activado"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Frameskip valor establecido a: Automatico (%d Hz)"
Msg_Frameskip_Standard                  = "Frameskip valor establecido a: 1/%d"
Msg_FPS_Counter_Enabled                 = "Contador de FPS habilitado"
Msg_FPS_Counter_Disabled                = "Contador de FPS deshabilitado"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "LOG de opciones necesita un nombre de archivo!"
Msg_Log_Session_Start                   = "-- Sesion de LOG empezada en %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "La opcion LOAD necesita un numero de estado salvado"
Msg_Load_Error                          = "Error abriendo %s!"
Msg_Load_Not_Valid                      = "Archivo %s no es un valido %s juego salvado!"
Msg_Load_Success                        = "Stado cargado desde %s"
Msg_Load_Version                        = "%s no es una version soportada de juego salvado!"
Msg_Load_Wrong_System                   = "%s no es un juego salvado para esta sistema!"
Msg_Load_Massage                        = "El archivo esta en formato MSD - convirtiendo"
Msg_Save_Not_in_BIOS                    = "No se puede salvar mientras se ejecuta la BIOS!"
Msg_Save_Error                          = "Error escribiendo en %s!"
Msg_Save_Success                        = "Stado salvado en %s"
Msg_Save_Slot                           = "Slot de juego salvado actual es: %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Opciones"
Msg_Options_Close                       = "Cerrar"
;Msg_Options_BIOS_Enable                 = "Mostrar logo de BIOS interna si es posible."
;Msg_Options_Product_Number              = "Mostrar numero de producto del juego si es posible."
;Msg_Options_Bright_Palette              = "Paleta de brillo para Master System / Game Gear."
Msg_Options_Load_Close                  = "Cerrar explorador despues de cargar un juego."
Msg_Options_Load_FullScreen             = "Cambiar a pantalla completa tras cargar un juego."
;Msg_Options_VFN_Support                 = "Mostrar nombre largo de fichero y pais."
Msg_Options_FullScreen_Messages         = "Mostrar mensajes en modo pantalla completa"
Msg_Options_GUI_VSync                   = "Esperar VSync en modo interfaz."
Msg_Options_NES_Enable                  = "Mario no es un fontanero gordo."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "Idioma establecido al %s"
Msg_Language_Set_Warning                = "Nota: se recomienda que reinicie MEKA para actualizar textos."

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Empezado salida de sonido dumeado a %s"
Msg_Sound_Dumping_Stop                  = "Detenido salida de sonido dumpeado (%2.02f segundos grabados)"
Msg_Sound_Dumping_Error_File_1          = "Incapaz de encontrar un nombre de fichero valido para salida de sonido dumpeado!"
Msg_Sound_Dumping_Error_File_2          = "Error abriendo archivo de entrada %s para dumpeo!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "El dumpeo VGM estará ahora con los frames precisos (archivos mas pequeños, voces excluidas)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "El dumpeo VGM estará ahora con el sample preciso (archivos mas grandes, voces dumpeadas)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(El dumpeo VGM tendrá que reempezar para tener en cuenta la nueva precisión)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "PRINCIPAL"
Msg_Menu_Main_LoadROM                   = "Cargar ROM"
Msg_Menu_Main_FreeROM                   = "Liberar ROM"
Msg_Menu_Main_SaveState                 = "Salvar estado"
Msg_Menu_Main_LoadState                 = "Cargar estado"
Msg_Menu_Main_Options                   = "Opciones"
Msg_Menu_Main_Language                  = "Idioma"
Msg_Menu_Main_Quit                      = "Salir"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Dump                     = "Dumpeo"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "MAQUINA"
Msg_Menu_Machine_Power                  = "Power"
Msg_Menu_Machine_Power_On               = "On"
Msg_Menu_Machine_Power_Off              = "Off"
Msg_Menu_Machine_Country                = "Pais"
Msg_Menu_Machine_Country_EU             = "Europeo/USA"
Msg_Menu_Machine_Country_Jap            = "Japonés"
Msg_Menu_Machine_TVType                 = "Tipo TV"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Pausa dura"
Msg_Menu_Machine_HardReset              = "Reset duro"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VIDEO"
Msg_Menu_Video_FullScreen               = "Fullscreen"
Msg_Menu_Video_Themes                   = "Temas"
Msg_Menu_Video_Blitters                 = "Blitters"
Msg_Menu_Video_Layers                   = "Capas"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Fondo"
Msg_Menu_Video_Flickering               = "Flickering"
Msg_Menu_Video_Flickering_Auto          = "Auto"
Msg_Menu_Video_Flickering_Yes           = "Si"
Msg_Menu_Video_Flickering_No            = "No"
Msg_Menu_Video_3DGlasses                = "Gafas 3-D"
Msg_Menu_Video_3DGlasses_Enabled        = "Habilitado"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Mostrar ambos lados"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Mostrar lado izqdo."
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Mostrar lado dcho."
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Usa gafas en el puerto COM"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "SONIDO"
Msg_Menu_Sound_FM                       = "Unidad FM"
Msg_Menu_Sound_FM_Enabled               = "Habilitdo"
Msg_Menu_Sound_FM_Disabled              = "Desactivado"
Msg_Menu_Sound_FM_Editor                = "Editor de instrumentos"
Msg_Menu_Sound_Volume                   = "Volumen"
Msg_Menu_Sound_Volume_Mute              = "Silencio"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Frecuencia"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Canales"
Msg_Menu_Sound_Channels_Tone            = "Tono %d"
Msg_Menu_Sound_Channels_Noises          = "Ruidos"
Msg_Menu_Sound_Dump                     = "Dumpeo"
Msg_Menu_Sound_Dump_WAV_Start           = "Empezar WAV"
Msg_Menu_Sound_Dump_WAV_Stop            = "Parar WAV"
Msg_Menu_Sound_Dump_VGM_Start           = "Empezar VGM"
Msg_Menu_Sound_Dump_VGM_Stop            = "Parar VGM"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "VGM Sample Accurate"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "ENTRADA"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Light Phaser"
Msg_Menu_Inputs_PaddleControl           = "Pad de control"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Tableta gráfica"
Msg_Menu_Inputs_SK1100                  = "TEclado Sega"
Msg_Menu_Inputs_RapidFire               = "Fuego rápido"
Msg_Menu_Inputs_RapidFire_PxBx          = "Jugador %d Botón %d"
Msg_Menu_Inputs_Configuration           = "Configuración"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "UTILES"
Msg_Menu_Tools_Messages                 = "Mensajes"
Msg_Menu_Tools_Palette                  = "Paletas"
Msg_Menu_Tools_TilesViewer              = "Visor de Retales"
Msg_Menu_Tools_TechInfo                 = "Tech Info"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "AYUDA"
Msg_Menu_Help_Documentation             = "Documentación"
Msg_Menu_Help_Documentation_W           = "Documentacion (Windows)"
Msg_Menu_Help_Compat                    = "Lista de compatibilidad"
Msg_Menu_Help_Multiplayer_Games         = "Lista de juegos multijugador"
Msg_Menu_Help_Changes                   = "Lista de cambios"
Msg_Menu_Help_About                     = "Acerca de"

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Português]
WIP
; (Portuguese accentued text - UTF Support)
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; Tradução realizada por (Translation done by):
; Rodrigo Campanini Rubio (Elsydeon_Sword) - elsydeon_swrd _@_ hotmail . com
; http://rubiopage.tripod.com
;
; Atenção: Alguns caracteres poderão ser exibidos incorretamente no DOS ou na
; interface devido à problemas de incompatibilidade entre os formatos de
; acentos DOS ANSI e UTF text format. Reporte qualquer erro para
; elsydeon_swrd _@_ hotmail . com indicando a linha de texto exibido de forma errônea.
;
; Warning: Some accentued characters may appear in incorrect form due to incompatibility
; between UTF and ANSI-DOS accentued characters table positions. If you found any
; unreadable or wrong sentences please report it to me.
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Bem vindo a %s (c) %s"
;Msg_Window_Title                        = "MEKA - 8-bit ou a morte!"
Msg_Quit                                = "Boa noite, bravo guerreiro. Boa noite, terra dos monstros..."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "Sobre Meka..."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "por %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Failed                              = "Falhou!"
Msg_Error_Base                          = "Erro: %s"
Msg_Error_Error                         = "Fatal: erro obtendo tipo do erro. Alguma coisa deve estar errada."
Msg_Error_Memory                        = "Mem¢ria insuficiente!"
Msg_Error_Param                         = "Parƒmetro \"%s\" ‚ inv lido!\n"
Msg_Error_Syntax                        = "Erro de sintaxe!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Imposs¡vel alternar para o modo de v¡deo desejado (%dx%d)."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "Arquivo nÆo encontrado!"
Msg_Error_File_Read                     = "Imposs¡vel ler do arquivo!"
Msg_Error_File_Empty                    = "O arquivo est  vazio!"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "Esta versÆo nÆo suporta arquivos tipo ZIP!"
Msg_Error_ZIP_Loading                   = "Impossível ler arquivo ZIP!"
Msg_Error_ZIP_Internal                  = "Erro interno enquanto carregando o arquivo ZIP!"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Erro abrindo diretório!"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Reinicie o jogo para que as mudanças façam efeito"
Msg_No_ROM                              = "Nenhuma ROM foi carregada!"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Inicializando Allegro..."
Msg_Init_GUI                            = "Inicializando interface gr fica com o usu rio (GUI)..."
Msg_Init_Completed                      = "[PRONTO!]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Soundcard_Select              = "Por favor, selecione sua placa de som:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(Direct X ‚ altamente recomendado, se dispon¡vel)"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Imagem gravada como %s"
Msg_Capture_Error                       = "Imposs¡vel capturar imagem!"
Msg_Capture_Error_File                  = "Não foi possível encontrar um nome válido para gravar a imagem capturada!"

; On-Board Memory ------------------------------------------------------------
Msg_SRAM_Loaded                         = "SaveRAM carregada do disco (%d Kb)"
Msg_SRAM_Load_Unable                    = "Impossível carregar a SaveRAM do disco"
Msg_SRAM_Wrote                          = "SaveRAM gravada no disco (%d Kb)"
Msg_SRAM_Write_Unable                   = "Impossível gravar a SaveRAM no disco (%d Kb)"
Msg_93c46_Reset                         = "Reiniciando 93c46"
Msg_93c46_Loaded                        = "EEPROM de dados 93c46 carregada do disco (%d bytes)"
Msg_93c46_Load_Unable                   = "Impossível carregar EEPROM de dados 93c46 do disco"
Msg_93c46_Wrote                         = "EEPROM de dados 93c46 gravada no disco (%d bytes)"
Msg_93c46_Write_Unable                  = "Impossível escrever EEPROM de dados 93c46 no disco (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "Agora o tipo de TV é %s"
Msg_TVType_Info_Speed                   = "(Velocidade original: %dhz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Carregando MEKA.BLT (modos de v¡deo / padräes)... "
Msg_Blitters_Error_Not_Enough           = "NÆo foram encontrados padräes suficientes! Verifique o arquivo MEKA.BLT ou tente reinstal -lo."
Msg_Blitters_Error_Not_Found            = "PadrÆo nÆo encontrado!"
Msg_Blitters_Error_Missing              = "Linha %d: Sem defini‡Æo de nome do padrÆo para armazenamento!"
Msg_Blitters_Error_Unrecognized         = "Linha %d: Vari vel desconhecida!"
Msg_Blitters_Set                        = "Usando '%s' exibição em tela inteira."

; Nintendo -------------------------------------------------------------------
Msg_NES_Activate                        = "Ativado modo 'Encanador Gorducho'"
Msg_NES_Sucks                           = "Erro fatal: o usu rio gosta de Nintendo!"
Msg_NES_Mapper_Unknown                  = "Mapeador não suportado: %d. O jogo pode não funcionar."
Msg_NES_Deny_Facts                      = "Você não pode negar fatos!"

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Inicializando depurador."
Msg_Debug_Welcome                       = "Bem vindo ao Depurador Meka!"
Msg_Debug_Not_Available                 = "Depurador nao esta disponivel nesta versao!"
Msg_Debug_Brk_Need_Param                = "Opção DEBUGBRK precisa de um endereço!"
Msg_Debug_Trap_Read                     = "Em PC:%04X - Ler de %04X"
Msg_Debug_Trap_Write                    = "Em PC:%04X - Escrever de %02X a %04X"
Msg_Debug_Trap_Port_Read                = "Em PC:%04X - Porta [%02X] leitura"
Msg_Debug_Trap_Port_Write               = "Em PC:%04X - Porta [%02X] escrita %02X"

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Cópia de dados será agora feitas em modo ASCII."
Msg_DataDump_Mode_Raw                   = "Cópia de dados será agora feitas em modo RAW."
Msg_DataDump_Error                      = "Erro ao criar arquivo de cópia %s!"
Msg_DataDump_Error_OB_Memory            = "Não há memória para cópia!"
Msg_DataDump_Error_Palette              = "Não há paleta para copiar!"
Msg_DataDump_Error_Sprites              = "Não há sprites para copiar!"
Msg_DataDump_Main                       = "Copiado(s) %s (%d %s) em modo %s."

; Documentation --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Documentação"
Msg_Doc_File_Error                      = "Não foi possível abrir a documentação!"
Msg_Doc_Enabled                         = "Exibindo documentação."
Msg_Doc_Disabled                        = "Documentação recolhida."

; Sprite Flickering ----------------------------------------------------------
Msg_Flickering_Auto                     = "Tremulação do vídeo será evitada automaticamente."
Msg_Flickering_Yes                      = "Controle de tremulação do vídeo ligado."
Msg_Flickering_No                       = "Controle de tremulação do vídeo desligado."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Atualização do segundo plano desligada"
Msg_Layer_BG_Enabled                    = "Atualização do segundo plano ligada"
Msg_Layer_Spr_Disabled                  = "Atualização da imagem desligada"
Msg_Layer_Spr_Enabled                   = "Atualização da imagem ligada"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Comando desconhecido de leitura do disco %3i"
Msg_FDC765_Unknown_Write                = "FDC765: Comando desconhecido de escrita em disco %3i"
Msg_FDC765_Disk_Too_Large1              = "Imagem de disco é maior que o esperado (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Dados adicionais serão ignorados."
Msg_FDC765_Disk_Too_Small1              = "Imagem de disco é menor que o esperado(%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Dados faltantes serão tratados como vazios"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "A caneta está tocando a prancheta gráfica"
Msg_TVOekaki_Pen_Away                   = "A caneta não está mais tocando a prancheta gráfica"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Paleta"
Msg_Palette_Disabled                    = "Visualização de paleta desativada"
Msg_Palette_Enabled                     = "Visualização de paleta ativa"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Mensagens"
Msg_Message_Disabled                    = "Caixa de mensagem desativada"
Msg_Message_Enabled                     = "Caixa de mensagem ativa"

; Technical Information box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Informações Técnicas"
Msg_TechInfo_Disabled                   = "Informações técnicas desabilitadas"
Msg_TechInfo_Enabled                    = "Informações técnicas ativas"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Visualizador de Tiles"
Msg_TilesViewer_Disabled                = "Visualização de tiles desativada"
Msg_TilesViewer_Enabled                 = "Visualização de tiles ativa"
Msg_TilesViewer_Tile                    = "Tile %d ($%X) @ %s"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Rapid Fire ativado para o jogador %d, botão %d"
Msg_RapidFire_JxBx_Off                  = "Rapid Fire desativado para o jogador %d, botão %d"

; FM Unit (YM-2413 chipset) --------------------------------------------------
Msg_FM_Enabled                          = "Emulação da unidade de FM (chip YM-2413) ligada"
Msg_FM_Disabled                         = "Emulação da unidade de FM (chip YM-2413) desligada"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "Máquina emulada é Brasileira/Européia/Estados Unidos"
Msg_Country_JAP                         = "Máquina emulada é Japonesa"

; Patching System ------------------------------------------------------------
Msg_Patch_Loading                       = "Carregando MEKA.PAT (altera‡äes/patches)... "
;Msg_Patch_Missing                       = "Linha %d: Nenhuma checagem (checksum) definido para esta instru‡Æo!"
;Msg_Patch_Unrecognized                  = "Linha %d: Instru‡Æo desconhecida!"
;Msg_Patch_Out_of_Bound                  = "Enquanto modificando ROM: tentativa de escrita fora da área permitida, posição $%02X. Cancelando modificações."

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "Emulação de óculos 3-D ativa"
Msg_Glasses_Disabled                    = "Emulação de óculos 3-D desativada"
Msg_Glasses_Show_Both                   = "Efeito 3-D mantido original (duas imagens)."
Msg_Glasses_Show_Left                   = "Efeito 3-D desabilitado mostrando apenas lado esquerdo."
Msg_Glasses_Show_Right                  = "Efeito 3-D desabilitado mostrando apenas lado direito."
Msg_Glasses_Com_Port                    = "Emulação de óculos 3-D habilitado através da porta COM %d."
;Msg_Glasses_Com_Port2                   = "(edite MEKA.CFG para mudar a porta de comunicações COM)."
Msg_Glasses_Unsupported                 = "Este jogo parece não utilizar óculos 3-D."

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Configuração de entradas"

; Inputs Initialization ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Inicializando joystick... "
Msg_Inputs_Joy_Init_None                = "nenhum encontrado."
Msg_Inputs_Joy_Init_Found               = "encontrado(s) %d joystick(s)."
Msg_Inputs_Joy_Calibrate_Error          = "Erro ao calibrar o joystick. Cancelado."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Periférico de entrada: Controle"
Msg_Inputs_LightPhaser                  = "Periférico de entrada: Pistola Light Phaser"
Msg_Inputs_PaddleControl                = "Periférico de entrada: Controle com leme"
Msg_Inputs_SportsPad                    = "Periférico de entrada: Sports Pad"
Msg_Inputs_TVOekaki                     = "Periférico de entrada: Prancheta Gráfica TV Oekaki "
Msg_Inputs_Play_Digital                 = "Dispositivo de controle com entrada digital"
Msg_Inputs_Play_Mouse                   = "Mouse ou outro dispositivo de controle com entrada analógica"
Msg_Inputs_Play_Pen                     = "Controlar caneta com o mouse\nBotão_direito simula remover a caneta da prancheta"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Carregando MEKA.INP (defini‡äes de dispositivos de entrada)... "
Msg_Inputs_Src_Not_Enough               = "Dispositivos de entrada insuficientes! Verifique o arquivo MEKA.INP ou tente reinstal -lo."
Msg_Inputs_Src_Missing                  = "Linha %d: NÆo h  dispositivo definido para armazenamento!"
Msg_Inputs_Src_Equal                    = "Linha %d: Faltando sinal de igual para declara‡Æo da vari vel!"
Msg_Inputs_Src_Unrecognized             = "Linha %d: Vari vel desconhecida \"%s\"!"
Msg_Inputs_Src_Syntax_Param             = "Linha %d: Erro de sintaxe no parƒmetro da vari vel!"
Msg_Inputs_Src_Inconsistency            = "Linha %d: Inconsistˆncia em rela‡Æo … declara‡Æo anterior!"
Msg_Inputs_Src_Map_Keyboard             = "Agora pressione a tecla que você deseja atribuir a esta ação"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Ação designada à tecla \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Agora pressione o botão/eixo a que você deseja atribuir esta ação"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Ação designada ao joypad stick %i, eixo %i, %c."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Ação designada ao botão %i."
Msg_Inputs_Src_Map_Mouse                = "Agora aperte o botão a que você deseja atribuir esta ação"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Ação designada ao botão %i do mouse."
Msg_Inputs_Src_Map_Mouse_No_A           = "Desculpe, mas o eixo do mouse não pode ser alterado"
Msg_Inputs_Src_Map_Cancelled            = "Atribuição cancelada"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Emulação pausada"
Msg_Machine_Resume                      = "Continuando emulação"
Msg_Machine_Reset                       = "Reiniciando sistema"

; Filenames Database ---------------------------------------------------------
;Msg_VFN_Loading                         = "Carregando MEKA.VFN (nomes virtuais dos arquivos)... "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Carregando MEKA.NAM (nomes dos jogos)... "
Msg_DB_Name_Default                     = "Imagem do jogo"
Msg_DB_Name_NoCartridge                 = "Sem cartucho"

; Configuration File ---------------------------------------------------------
Msg_Config_Loading                      = "Carregando %s (configura‡Æo)... "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Carregando MEKA.DAT (recursos)... "

; Drivers --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Fatal: Driver de m quina desconhecido!"

; Overdump detection ---------------------------------------------------------
;Msg_OverDump                            = "Esta ROM possui %d vezes o tamanho necessário de dados.\nPegue o "SMS Checker" para resolver este problema."

; Sound ----------------------------------------------------------------------
Msg_Sound_Init                          = "Inicializando dispositivo de som..."
Msg_Sound_Init_Error_Audio              = " - Erro ao acessar dispositivo de  udio. Tente outra placa de som ou taxa de\namostragem."
Msg_Sound_Init_Error_Blaster            = " - vari vel de ambiente BLASTER nÆo encontrada. Desabilitando som FM!"
Msg_Sound_Init_Error_Blaster_A          = " - vari vel de ambiente BLASTER nÆo possui o endere‡o (A=xxx). Desabilitando som FM!"
Msg_Sound_Init_Error_Voices             = " - Inicializa‡Æo das vozes falhou."
Msg_Sound_Init_Error_Voice_N            = " - Falha na cria‡Æo da voz n£mero %d."
Msg_Sound_Init_Soundcard                = " - Inicializando placa de som @ %d Hz..."
Msg_Sound_Init_Soundcard_No             = "Imposs¡vel iniciar  udio: nÆo foi selecionada uma placa de som!"
;Msg_Sound_Init_SN76496_Ok               = " - Emulador do SN76496: Ok"
;Msg_Sound_Init_YM2413_Ok                = " - Emulador do YM2413: Ok"
Msg_Sound_Stream_Error                  = "Cache de som parado. Reiniciando sequência"

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Carregando MEKA.THM (temas da interface)... "
Msg_Theme_Error_Not_Enough              = "Temas insuficientes! Verifique o\narquivo MEKA.THM ou tente reinstal -lo."
Msg_Theme_Error_Missing_Theme_Name      = "Linha %d: Nenhum tema definido para armazenamento do valor!"
Msg_Theme_Error_BG_Big                  = "Figura de fundo muito pequena para ser dimensionada com escala de inteiros!"
Msg_Theme_Error_BG                      = "Erro ao carregar a figura associada ao tema!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Carregando ROM... "
Msg_LoadROM_Success                     = "Carregada com sucesso %s"
Msg_LoadDisk_Success                    = "carregado %s com sucesso como um disco flexível"
Msg_LoadROM_Comment                     = "Comentário: %s"
Msg_LoadROM_Warning                     = "-Avisos-"
;Msg_LoadROM_Bad_Dump_Long               = "Esta ROM é conhecida como sendo uma cópia com defeito.\nHá grandes chances de que ela não funcione corretamente."
Msg_LoadROM_Bad_Dump_Short              = "Atenção - Esta ROM é conhecida como uma cópia com defeito."
Msg_LoadROM_Product_Num                 = "Número do produto: %s"
Msg_LoadROM_SDSC_Author                 = "Autor: %s"
Msg_LoadROM_SDSC                        = "-Cabeçalho SDSC-"
Msg_LoadROM_SDSC_Name                   = "Nome: %s"
Msg_LoadROM_SDSC_Version                = "Versão: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Data: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Release_Note           = "Nota de Lançamento: %s"
Msg_LoadROM_SDSC_Unknown                = "<Desconhecido>"
Msg_LoadROM_SDSC_Error                  = "<Erro>"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Carrega ROM"
Msg_FileBrowser_Drive                   = "[Drive %c]"
Msg_FileBrowser_Load                    = "Ok"
Msg_FileBrowser_Close                   = "Fechar"
Msg_FileBrowser_LoadNames               = "Nomes reais"
Msg_FileBrowser_ReloadDir               = "Atualizar"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "Editor de Instrumentos FM"
Msg_FM_Editor_Enabled                   = "Editor de voz FM ligado"
Msg_FM_Editor_Disabled                  = "Editor de voz FM desligado"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Taxa de quadros : Automática (%d Hz)"
Msg_Frameskip_Standard                  = "Taxa de quadros ajustada para: 1/%d"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "Op‡Æo LOG precisa de um nome de arquivo!"
Msg_Log_Session_Start                   = "-- Registrando sessÆo iniciada em %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "A op‡Æo LOAD precisa de um n£mero de estado gravado"
Msg_Load_Error                          = "Erro abrindo %s!"
Msg_Load_Not_Valid                      = "Arquivo %s não é uma gravação de estado válida!"
Msg_Load_Success                        = "Estado carregado de %s"
Msg_Load_Version                        = "%s é uma gravação de estado não suportada!"
Msg_Load_Wrong_System                   = "%s não é uma gravação de estado para este sistema!"
Msg_Load_Massage                        = "Arquivo em formato MSD - convertendo"
Msg_Save_Not_in_BIOS                    = "Não é possível gravar enquanto estiver rodando a BIOS!"
Msg_Save_Error                          = "Erro gravando em %s!"
Msg_Save_Success                        = "Estado gravado em %s"
Msg_Save_Slot                           = "Área para gravação de estado atual: %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Opções"
Msg_Options_Close                       = "Fechar"
;Msg_Options_BIOS_Enable                 = "Mostrar logotipo da BIOS, se disponível no sistema."
;Msg_Options_Product_Number              = "Mostrar número de produto do jogo, se disponível."
;Msg_Options_Bright_Palette              = "Paleta brilhante para o Master System/Game Gear."
Msg_Options_Load_Close                  = "Fechar janela dos arquivos após carregar um jogo."
Msg_Options_Load_FullScreen             = "Alternar para tela inteira após carregar um jogo."
;Msg_Options_VFN_Support                 = "Mostrar nomes longos (Win9X) e bandeira dos países."
Msg_Options_FullScreen_Messages         = "Mostrar mensagem em tela inteira."
Msg_Options_GUI_VSync                   = "Aguardar sincronismo vertical no modo interface."
Msg_Options_NES_Enable                  = "Mario não é um encanador gorducho."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "Idioma mudado para %s"
Msg_Language_Set_Warning                = "Nota: É recomendado que você reinicie MEKA para ter certeza que todo o texto será atualizado."

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Iniciada cópia de dados do som para o arquivo %s"
Msg_Sound_Dumping_Stop                  = "Término da cópia de dados do som (%2.02f segundos gravados)"
Msg_Sound_Dumping_Error_File_1          = "Impossível encontrar um nome válido de arquivo para copiar dados do som!"
Msg_Sound_Dumping_Error_File_2          = "Erro abrindo arquivo de saída %s para cópia de dados!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "Cópia de VGM terá precisão de quadros (arquivos menores, vozes ignoradas)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "Cópia de VGM será fiel ao 'sample' (arquivos maiores, vozes copiadas)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(A cópia de VGM deverá ser reiniciada para vigorar a nova precisão)"
;-----------------------------------------------------------------------------

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "PRINCIPAL"
Msg_Menu_Main_LoadROM                   = "Abre ROM"
Msg_Menu_Main_FreeROM                   = "Fecha ROM"
Msg_Menu_Main_SaveState                 = "Gravar Estado"
Msg_Menu_Main_LoadState                 = "Carregar Estado"
Msg_Menu_Main_Options                   = "Opções"
Msg_Menu_Main_Language                  = "Idioma"
Msg_Menu_Main_Quit                      = "Sair"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEPURAR"
Msg_Menu_Debug_Dump                     = "Copiar"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "CONSOLE"
Msg_Menu_Machine_Power                  = "Liga/Desliga"
Msg_Menu_Machine_Power_On               = "Ligado"
Msg_Menu_Machine_Power_Off              = "Desligado"
Msg_Menu_Machine_Country                = "País"
Msg_Menu_Machine_Country_EU             = "Europa/EUA/Brasil"
Msg_Menu_Machine_Country_Jap            = "Japão"
Msg_Menu_Machine_TVType                 = "Tipo de TV"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Pausar"
Msg_Menu_Machine_HardReset              = "Reiniciar"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VÍDEO"
Msg_Menu_Video_FullScreen               = "Tela Inteira"
Msg_Menu_Video_Themes                   = "Temas"
Msg_Menu_Video_Blitters                 = "Padrões"
Msg_Menu_Video_Layers                   = "Camadas"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Imagem de fundo"
Msg_Menu_Video_Flickering               = "Tremulação"
Msg_Menu_Video_Flickering_Auto          = "Automático"
Msg_Menu_Video_Flickering_Yes           = "Sim"
Msg_Menu_Video_Flickering_No            = "Não"
Msg_Menu_Video_3DGlasses                = "Óculos 3-D"
Msg_Menu_Video_3DGlasses_Enabled        = "Ligado"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Mostrar ambos os lados"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Mostrar lado esquerdo"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Mostrar lado direito"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Usar óculos na porta COM"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "SOM"
Msg_Menu_Sound_FM                       = "Unidade de FM"
Msg_Menu_Sound_FM_Enabled               = "Ligada"
Msg_Menu_Sound_FM_Disabled              = "Desligada"
Msg_Menu_Sound_FM_Editor                = "Editor de Instrumentos"
Msg_Menu_Sound_Volume                   = "Volume"
Msg_Menu_Sound_Volume_Mute              = "Mudo"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Taxa"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Canais"
Msg_Menu_Sound_Channels_Tone            = "Tom %d"
Msg_Menu_Sound_Channels_Noises          = "Ruídos"
Msg_Menu_Sound_Dump                     = "Gravar"
Msg_Menu_Sound_Dump_WAV_Start           = "Inicia WAV"
Msg_Menu_Sound_Dump_WAV_Stop            = "Pára WAV"
Msg_Menu_Sound_Dump_VGM_Start           = "Inicia VGM"
Msg_Menu_Sound_Dump_VGM_Stop            = "Pára VGM"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "VGM com sample preciso"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "ENTRADAS"
Msg_Menu_Inputs_Joypad                  = "Controle"
Msg_Menu_Inputs_LightPhaser             = "Pistola Light Phaser"
Msg_Menu_Inputs_PaddleControl           = "Controle Paddle"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Prancheta gráfica"
Msg_Menu_Inputs_SK1100                  = "Teclado Sega"
Msg_Menu_Inputs_RapidFire               = "Turbo"
Msg_Menu_Inputs_RapidFire_PxBx          = "Jogador %d Botão %d"
Msg_Menu_Inputs_Configuration           = "Configuração"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "FERRAMENTAS"
Msg_Menu_Tools_Messages                 = "Mensagens"
Msg_Menu_Tools_Palette                  = "Paleta"
Msg_Menu_Tools_TilesViewer              = "Visualizador de Tiles"
Msg_Menu_Tools_TechInfo                 = "Informações Técnicas"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "AJUDA"
Msg_Menu_Help_Documentation             = "Ajuda"
Msg_Menu_Help_About                     = "Sobre"

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Italiano]
WIP
; Translation by Andrea Ballandino (Adol)
; Maintainer: Andrea Ballandino (adol AT email.it)
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Benvenuto in %s (c) %s"
;Msg_Window_Title                        = "MEKA - 8-bit o morte!"
Msg_Quit                                = "Buona notte, valoroso guerriero. Notte, terra dei mostri...."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "About Meka.."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "by %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Failed                              = "Fallito!"
Msg_Error_Base                          = "Errore: %s"
Msg_Error_Error                         = "Fatale: errore di tipo errore. Qualcosa e' andato storto."
Msg_Error_Memory                        = "Memoria insufficente!"
Msg_Error_Param                         = "Parametero \"%s\" non valido!\n"
Msg_Error_Syntax                        = "Comando non valido!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Impossibile settare il modo video desiderato (%dx%d)."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "File non trovato!"
Msg_Error_File_Read                     = "Impossibile leggere dal file!"
Msg_Error_File_Empty                    = "File vuoto!"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "Questa versione non usa il formato ZIP!"
Msg_Error_ZIP_Loading                   = "Impossibile leggere il file ZIP!"
Msg_Error_ZIP_Internal                  = "Errore interno leggendo il file ZIP!"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Errore di apertura directory!"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Resetta il gioco per avere le nuove modifiche"
Msg_No_ROM                              = "Nessuna ROM caricata!"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Initializzo Allegro.."
Msg_Init_GUI                            = "Initializzo parti grafiche.."
Msg_Init_Completed                      = "[PRONTO]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Soundcard_Select              = "Per favore scegliere dispositivo audio:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(DirectSound e' altamente raccomandato, se disponibile)"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Schermata salvata come %s"
Msg_Capture_Error                       = "Impossibile creare la schermata!"
Msg_Capture_Error_File                  = "Impossibile trovare un nome file valido per salvare la schermata!"

; On-Board Memory ------------------------------------------------------------
Msg_SRAM_Loaded                         = "Caricata SaveRAM da disco (%d Kb)"
Msg_SRAM_Load_Unable                    = "Impossibile caricare SaveRAM da disco"
Msg_SRAM_Wrote                          = "Salvata SaveRAM su disco (%d Kb)"
Msg_SRAM_Write_Unable                   = "Impossibile salvare SaveRAM su disco (%d Kb)"
Msg_93c46_Reset                         = "Resetto 93c46"
Msg_93c46_Loaded                        = "Caricata memoria EEPROM 93c46 da disco (%d bytes)"
Msg_93c46_Load_Unable                   = "Impossibile caricare memoria EEPROM 93c46 da disco"
Msg_93c46_Wrote                         = "Salvata memoria EEPROM 93c46 su disco (%d bytes)"
Msg_93c46_Write_Unable                  = "Impossibile salvare memoria EEPROM 93c46 su disco (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "Tipo di TV e' ora %s"
Msg_TVType_Info_Speed                   = "(Frequenza di clock originale %d Hz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Carico MEKA.BLT (modi video / blitters)... "
Msg_Blitters_Error_Not_Enough           = "Non ci sono abbastanza blitter! Controllare MEKA.BLT o reinstallare Meka."
Msg_Blitters_Error_Not_Found            = "Blitter non trovato!"
Msg_Blitters_Error_Missing              = "On line %d: Nome blitter non definito!"
Msg_Blitters_Error_Unrecognized         = "On line %d: Variabile non riconosciuta!"
Msg_Blitters_Set                        = "Uso \"%s\" Blitter schermo intero."

; Nintendo -------------------------------------------------------------------
Msg_NES_Activate                        = "Modalita' Idraulico ciccione attivata"
Msg_NES_Sucks                           = "Errore fatale: All'utente piace il Nintendo!"
Msg_NES_Mapper_Unknown                  = "Mappa non supportata: %d. Il gioco non funziona."
Msg_NES_Deny_Facts                      = "Non puoi negare i fatti."

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Inizializzo il debugger.. "
Msg_Debug_Welcome                       = "Benvenuto nel Debugger di Meka!"
Msg_Debug_Not_Available                 = "Debugger non disponibile in questa versione!"
Msg_Debug_Brk_Need_Param                = "DEBUGBRK serve un indirizzo!"
Msg_Debug_Trap_Read                     = "At PC:%04X - Leggo da %04X"
Msg_Debug_Trap_Write                    = "At PC:%04X - Scrivo %02X to %04X"
Msg_Debug_Trap_Port_Read                = "At PC:%04X - Porta [%02X] Leggo"
Msg_Debug_Trap_Port_Write               = "At PC:%04X - Porta [%02X] Scrivo %02X"

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Dump dei dati in modalita' ASCII."
Msg_DataDump_Mode_Raw                   = "Dump dei dati in modalita' Raw."
Msg_DataDump_Error                      = "Errore creazione del file per il dump %s!"
Msg_DataDump_Error_OB_Memory            = "Insufficiente memoria per il dump!"
Msg_DataDump_Error_Palette              = "Nessuna palette per il dump!"
Msg_DataDump_Error_Sprites              = "Nessuno sprite per il dump!"
Msg_DataDump_Main                       = "Dump %s (%d %s) in modo %s"

; Documentation --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Documentazione"
Msg_Doc_File_Error                      = "Impossibile aprire la documentazione!"
Msg_Doc_Enabled                         = "Apro documentazione."
Msg_Doc_Disabled                        = "Chiudo documentazione."

; Sprite Flickering ----------------------------------------------------------
Msg_Flickering_Auto                     = "Sprite Flickering degli sprite automatico."
Msg_Flickering_Yes                      = "Emulazione flickering sprite attivata."
Msg_Flickering_No                       = "Emulazione flickering sprite attivata."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Refresh di background disabilitato"
Msg_Layer_BG_Enabled                    = "Refresh di background attivato"
Msg_Layer_Spr_Disabled                  = "Refresh degli sprite disabilitato"
Msg_Layer_Spr_Enabled                   = "Refresh degli sprite attivato"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Comando lettura disco sconosciuto %3i"
Msg_FDC765_Unknown_Write                = "FDC765: Comando lettura disco sconosciuto %3i"
Msg_FDC765_Disk_Too_Large1              = "Immagine disco inaspettata (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Dati addizionali saranno ignorati"
Msg_FDC765_Disk_Too_Small1              = "Immagine disco piu' piccola di quella aspettata (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Dati assenti saranno presi come vuoti"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "La penna sta toccando la tavola grafica"
Msg_TVOekaki_Pen_Away                   = "La penna non sta toccando la tavola grafica"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Palette"
Msg_Palette_Disabled                    = "Visore Palette disabilitato"
Msg_Palette_Enabled                     = "Visore Palette attivato"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Messaggi"
Msg_Message_Disabled                    = "Box messaggi disabilitato"
Msg_Message_Enabled                     = "Box messaggi attivato"

; Technical Information box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Informazioni tecniche"
Msg_TechInfo_Disabled                   = "Box informazioni tecniche disattivato"
Msg_TechInfo_Enabled                    = "Box informazioni tecniche attivato"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Visore Tiles"
Msg_TilesViewer_Disabled                = "Visore tile disabilitato"
Msg_TilesViewer_Enabled                 = "Visore tile attivato"
Msg_TilesViewer_Tile                    = "Tile %d ($%X) @ %s"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Autofuoco attivato per giocatore %d Pulsante %d"
Msg_RapidFire_JxBx_Off                  = "Autofuoco disattivato per giocatore %d Pulsante %d"

; FM Unit (YM-2413 chipset) --------------------------------------------------
Msg_FM_Enabled                          = "Emulazione Unita' FM (YM-2413 chipset) attivata"
Msg_FM_Disabled                         = "Emulazione Unita' FM (YM-2413 chipset) disattivata"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "Sistema emulato : Europeo/US"
Msg_Country_JAP                         = "Sistema emulato : Giapponese"

; Patching System ------------------------------------------------------------
Msg_Patch_Loading                       = "Carico MEKA.PAT (patches)... "
;Msg_Patch_Missing                       = "On line %d: Nessun checksum definito per l'istruzione!"
;Msg_Patch_Unrecognized                  = "On line %d: Instruzione sconosciuta!"
;Msg_Patch_Out_of_Bound                  = "Patchando la ROM: si sta cercando di scrivere fuori range, alla posizione $%02X. Patch fallito."

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "Emulazione occhiali 3-D attivata"
Msg_Glasses_Disabled                    = "Emulazione occhiali 3-D disattivata"
Msg_Glasses_Show_Both                   = "Effetto 3-D mantenuto."
Msg_Glasses_Show_Left                   = "Effetto 3-D disabilitato visualizzo lato sinistro."
Msg_Glasses_Show_Right                  = "Effetto 3-D disabilitato visualizzo lato destro."
Msg_Glasses_Com_Port                    = "Emulazione Occhiali 3-D attivata su Porta COM %d."
;Msg_Glasses_Com_Port2                   = "(modifica MEKA.CFG per cambiare Porta di comunicazione)"
Msg_Glasses_Unsupported                 = "Questo gioco non supporta gli occhiali 3-D."

; Inputs Initialization ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Inizializzo joystick.. "
Msg_Inputs_Joy_Init_None                = "non trovato."
Msg_Inputs_Joy_Init_Found               = "%d Trovato."
Msg_Inputs_Joy_Calibrate_Error          = "Errore calibrazione joystick. Esco."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Periferica Input: Joypad"
Msg_Inputs_LightPhaser                  = "Periferica Input: Light Phaser"
Msg_Inputs_PaddleControl                = "Periferica Input: Paddle Control"
Msg_Inputs_SportsPad                    = "Periferica Input: Sports Pad"
Msg_Inputs_TVOekaki                     = "Periferica Input: TV Oekaki Graphic Board"
Msg_Inputs_Play_Digital                 = "Controllo con periferica input digitale"
Msg_Inputs_Play_Mouse                   = "Controllo con periferica input analogica (mouse)"
Msg_Inputs_Play_Pen                     = "Controllo con penna+mouse\nClick-Destro simula la rimozione penna dalla tavola"

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Configurazione Input"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Carico MEKA.INP (Definizione sorgenti input)... "
Msg_Inputs_Src_Not_Enough               = "Non ci sono abbastanza sorgenti di input! Controllare il file MEKA.INP o re-installare."
Msg_Inputs_Src_Missing                  = "On line %d: Nessuna sorgente di input definita per memoria valore!"
Msg_Inputs_Src_Equal                    = "On line %d: Manca il segno uguale per l'assegnazione di variabile!"
Msg_Inputs_Src_Unrecognized             = "On line %d: Variabile sconosciuta \"%s\"!"
Msg_Inputs_Src_Syntax_Param             = "On line %d: Errore di sintassi nel parametro variabile!"
Msg_Inputs_Src_Inconsistency            = "On line %d: inconsistenza di settaggio precedente!"
Msg_Inputs_Src_Map_Keyboard             = "Premere il tasto per assegnare azione"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Azione assignata al tasto \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Premere il buttone/asse per assegnare azione"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Azione assegnata al joypad stick %i, asse %i, %c."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Azione assignata al joypad buttone %i."
Msg_Inputs_Src_Map_Mouse                = "Premere il bottone per assegnare azione"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Azione assegnata al mouse buttone %i."
Msg_Inputs_Src_Map_Mouse_No_A           = "Mi spiace - asse Mouse non puo' essere cambiato"
Msg_Inputs_Src_Map_Cancelled            = "Assegnamento cancellato!"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Pausa emulazione sistema"
Msg_Machine_Resume                      = "Riprendo l'emulazione"
Msg_Machine_Reset                       = "Reset del Sistema"

; Filenames Database ---------------------------------------------------------
;Msg_VFN_Loading                         = "Carico MEKA.VFN (Nomi file virtuali)... "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Carico MEKA.NAM (nomi giochi)... "
Msg_DB_Name_Default                     = "Schermata gioco"
Msg_DB_Name_NoCartridge                 = "Nessuna Cartuccia Caricata"

; Configuration File ---------------------------------------------------------
Msg_Config_Loading                      = "Carico %s (configurazione)... "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Carico MEKA.DAT (risorse)... "

; Drivers --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Fatale: Driver sistema sconosciuto!"

; Overdump detection ---------------------------------------------------------
;Msg_OverDump                            = "Questa ROM contiene %d numero/i non necessario di dati.\nUsa "SMS Checker" per ripulirla."

; Sound ----------------------------------------------------------------------
Msg_Sound_Init                          = "Inizializzo emulazione sonora.."
Msg_Sound_Init_Error_Audio              = " - Errore apertura sistema Audio. Prova una scheda/campione diversi."
Msg_Sound_Init_Error_Blaster            = " - BLASTER variable non trovata, disabilito suono FM sound!"
Msg_Sound_Init_Error_Blaster_A          = " - BLASTER variabile non contiene indirizzi (A=xxx), disabilito suono FM!"
Msg_Sound_Init_Error_Voices             = " - Inizializzazione voci fallita."
Msg_Sound_Init_Error_Voice_N            = " - Creazione Voce #%d fallita."
Msg_Sound_Init_Soundcard                = " - Inizializzazione scheda sonora @ %d Hz"
Msg_Sound_Init_Soundcard_No             = "Impossibile inizializzare audio: nessuna scheda sonora scelta!"
;Msg_Sound_Init_SN76496_Ok               = " - emulatore SN76496 : Ok"
;Msg_Sound_Init_YM2413_Ok                = " - emulatore YM2413  : Ok"
Msg_Sound_Stream_Error                  = "Buffer sonoro fermato. Ricomincio il flusso."

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Carico MEKA.THM (interfacce temi)... "
Msg_Theme_Error_Not_Enough              = "Non ci sono temi! Controlla MEKA.THM o reinstalla Meka."
Msg_Theme_Error_Missing_Theme_Name      = "On line %d: Il tema non ha nome!"
Msg_Theme_Error_BG_Big                  = "Immagine di sfondo troppo piccola per scala di ingrandimento!"
Msg_Theme_Error_BG                      = "Errore caricamento immagine associata al tema!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Carico ROM.. "
Msg_LoadROM_Success                     = "Caricamento completato %s"
Msg_LoadDisk_Success                    = "Caricamento completato %s floppy disk"
Msg_LoadROM_Comment                     = "Commento: %s"
Msg_LoadROM_Warning                     = "-Attenzione-"
;Msg_LoadROM_Bad_Dump_Long               = "Questa ROM ha un dump non esatto.\nForse non funzionera' correttamente."
Msg_LoadROM_Bad_Dump_Short              = "Attenzione - E' risaputo che questa ROM ha un dump non esatto."
Msg_LoadROM_Product_Num                 = "Numero Gioco: %s"
Msg_LoadROM_SDSC                        = "-SDSC Header-"
Msg_LoadROM_SDSC_Name                   = "Nome: %s"
Msg_LoadROM_SDSC_Version                = "Versione: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Data: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Autore: %s"
Msg_LoadROM_SDSC_Release_Note           = "Note di rilascita: %s"
Msg_LoadROM_SDSC_Unknown                = "<Sconosciuto>"
Msg_LoadROM_SDSC_Error                  = "<Errore>"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Quale ROM carico?"
Msg_FileBrowser_Drive                   = "[Drive %c]"
Msg_FileBrowser_Load                    = "Carica"
Msg_FileBrowser_Close                   = "Chiudi"
Msg_FileBrowser_LoadNames               = "Carica Nomi"
Msg_FileBrowser_ReloadDir               = "Ricarica Dir"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "Editor strumenti FM"
Msg_FM_Editor_Enabled                   = "Editor voci FM abilitato"
Msg_FM_Editor_Disabled                  = "Editor voci FM disabilitato"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Valore di Frameskip settato in automatico (%d Hz)"
Msg_Frameskip_Standard                  = "Valore di Frameskip settato in: 1/%d"
Msg_FPS_Counter_Enabled                 = "Contatore FPS attivato"
Msg_FPS_Counter_Disabled                = "Contatore FPS disattivato"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "Opzione LOG, serve un nome-file!"
Msg_Log_Session_Start                   = "-- Logging sessione partito %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "All'opzione LOAD serve un numero di file di stato!"
Msg_Load_Error                          = "Errore di apertura %s!"
Msg_Load_Not_Valid                      = "File %s non Š %s un salvataggio di Stato!"
Msg_Load_Success                        = "Stato caricato da %s"
Msg_Load_Version                        = "%s non e' un salvataggio supportato!"
Msg_Load_Wrong_System                   = "%s non un salvataggio per questo sistema!"
Msg_Load_Massage                        = "Formato MSD - converto.."
Msg_Save_Not_in_BIOS                    = "Non posso salvare durante il logo del BIOS!"
Msg_Save_Error                          = "Errore scrittura su %s!"
Msg_Save_Success                        = "Stato salvato in %s"
Msg_Save_Slot                           = "Corrente slot di salvataggio : %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Opzioni"
Msg_Options_Close                       = "Chiudi"
;Msg_Options_BIOS_Enable                 = "Mostra il logo BIOS se disponibile."
;Msg_Options_Product_Number              = "Mostra numero di serie del gioco se in archivio."
;Msg_Options_Bright_Palette              = "Palette chiare per Master System / Game Gear."
Msg_Options_Load_Close                  = "Chiudi il browser dopo il caricamento del gioco."
Msg_Options_Load_FullScreen             = "Schermo intero dopo il caricamento del gioco."
;Msg_Options_VFN_Support                 = "Mostra nomi file lunghi e bandiere degli stati."
Msg_Options_FullScreen_Messages         = "Mostra i messaggi in modalita' schermo intero."
Msg_Options_GUI_VSync                   = "Aspetta il VSync nella modalita' interfaccia."
Msg_Options_NES_Enable                  = "Mario non e' un idraulico ciccione."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "Lingua settata : %s"
Msg_Language_Set_Warning                = "Nota: Raccomando di riaprire MEKA per essere sicuri delle modifiche."

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Comincio dump sonoro di output in %s"
Msg_Sound_Dumping_Stop                  = "Fermo dump sonoro di output sound output (%2.02f secondi contati)"
Msg_Sound_Dumping_Error_File_1          = "Impossibile trovare un nome-file valido per dump sonoro!"
Msg_Sound_Dumping_Error_File_2          = "Errore di apertura file di output %s per il dump!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "Dump VGM piu' accurato (piccoli files, salto le voci)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "Nel Dump VGM il campione sara' piu' accurato (grandi files, Dump voci)"
Msg_Sound_Dumping_VGM_Acc_Change        = "Ricomincio Dump VGM (tengo conto delle nuove opzioni)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "PRINCIPALE"
Msg_Menu_Main_LoadROM                   = "Carica ROM"
Msg_Menu_Main_FreeROM                   = "Libera ROM"
Msg_Menu_Main_SaveState                 = "Salva  Stato"
Msg_Menu_Main_LoadState                 = "Carica Stato"
Msg_Menu_Main_Options                   = "Opzioni"
Msg_Menu_Main_Language                  = "Lingua"
Msg_Menu_Main_Quit                      = "Chiudi"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Dump                     = "Dump"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "CONSOLE"
Msg_Menu_Machine_Power                  = "Accensione"
Msg_Menu_Machine_Power_On               = "On"
Msg_Menu_Machine_Power_Off              = "Off"
Msg_Menu_Machine_Country                = "Paese"
Msg_Menu_Machine_Country_EU             = "Europa/Stati Uniti"
Msg_Menu_Machine_Country_Jap            = "Giappone"
Msg_Menu_Machine_TVType                 = "Tipo di TV"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Pausa hardware"
Msg_Menu_Machine_HardReset              = "Reset hardware"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VIDEO"
Msg_Menu_Video_FullScreen               = "Schermo Intero"
Msg_Menu_Video_Themes                   = "Temi"
Msg_Menu_Video_Blitters                 = "Blitter"
Msg_Menu_Video_Layers                   = "Layer"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Fondali"
Msg_Menu_Video_Flickering               = "Flickering"
Msg_Menu_Video_Flickering_Auto          = "Auto"
Msg_Menu_Video_Flickering_Yes           = "Si"
Msg_Menu_Video_Flickering_No            = "No"
Msg_Menu_Video_3DGlasses                = "Occhiali 3-D"
Msg_Menu_Video_3DGlasses_Enabled        = "Abilitati"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Mostra entrambi i lati"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Mostra lato sinistro"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Mostra lato destro"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Usa Occhiali su Porta COM"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "SONORO"
Msg_Menu_Sound_FM                       = "Unita' FM"
Msg_Menu_Sound_FM_Enabled               = "Abilitata"
Msg_Menu_Sound_FM_Disabled              = "Disabilitata"
Msg_Menu_Sound_FM_Editor                = "Editor strumentale"
Msg_Menu_Sound_Volume                   = "Volume"
Msg_Menu_Sound_Volume_Mute              = "Muto"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Rate"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Canali"
Msg_Menu_Sound_Channels_Tone            = "Toni %d"
Msg_Menu_Sound_Channels_Noises          = "Rumori"
Msg_Menu_Sound_Dump                     = "Dump"
Msg_Menu_Sound_Dump_WAV_Start           = "Comincia Dump WAV"
Msg_Menu_Sound_Dump_WAV_Stop            = "Termina Dump WAV"
Msg_Menu_Sound_Dump_VGM_Start           = "Comincia Dump VGM"
Msg_Menu_Sound_Dump_VGM_Stop            = "Termina Dump VGM"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "Campioni VGM accurati"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "INPUT"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Light Phaser"
Msg_Menu_Inputs_PaddleControl           = "Paddle Control"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Tavoletta grafica"
Msg_Menu_Inputs_SK1100                  = "Tastiera Sega"
Msg_Menu_Inputs_RapidFire               = "Fuoco Rapido"
Msg_Menu_Inputs_RapidFire_PxBx          = "Giocatore %d Buttone %d"
Msg_Menu_Inputs_Configuration           = "Configurazione"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "STRUMENTI"
Msg_Menu_Tools_Messages                 = "Messaggi"
Msg_Menu_Tools_Palette                  = "Palette"
Msg_Menu_Tools_TilesViewer              = "Visore di Tiles"
Msg_Menu_Tools_TechInfo                 = "Informazioni Tecniche"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "AIUTO"
Msg_Menu_Help_Documentation             = "Documentazione"
Msg_Menu_Help_Documentation_W           = "Documentazione (Windows)"
Msg_Menu_Help_Compat                    = "Lista giochi compatibili"
Msg_Menu_Help_Multiplayer_Games         = "Lista giochi multiplayer"
Msg_Menu_Help_Changes                   = "Lista cambiamenti"
Msg_Menu_Help_About                     = "About"

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Deutsch]
WIP
; Übersetzung / translation to German: Tilman Hesse
; Maintainer: Tilman Hesse (tilman_h AT yahoo.de)
; Used accents: äöüÄÖÜß
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main ------------------------------------------------------------------------
Msg_Welcome                             = "Willkommen in %s (c) %s"
;Msg_Window_Title                        = "MEKA - 8-bit oder stirb!"
Msg_Quit                                = "Gute Nacht, mutiger Krieger. Gute Nacht, Monsterland..."

; About -----------------------------------------------------------------------
Msg_About_BoxTitle                      = "Über Meka.."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "von %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ----------------------------------------------------------------------
Msg_Failed                              = "Fehlschlag!"
Msg_Error_Base                          = "Fehler: %s"
Msg_Error_Error                         = "Abbruch: Unbekannter Fehler!"
Msg_Error_Memory                        = "Nicht genug Speicher!"
Msg_Error_Param                         = "Parameter \"%s\" nicht erlaubt!\n"
Msg_Error_Syntax                        = "Syntaxfehler!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Kann gewuenschten Videomodus nicht aktivieren (%dx%d)."

; Errors: File ----------------------------------------------------------------
Msg_Error_File_Not_Found                = "Datei nicht gefunden!"
Msg_Error_File_Read                     = "Datei kann nicht gelesen werden!"
Msg_Error_File_Empty                    = "Datei ist leer!"

; Errors: ZIP File ------------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "Diese Version von MEKA unterstützt keine gezippten Dateien!"
Msg_Error_ZIP_Loading                   = "Kann ZIP-Datei nicht lesen!"
Msg_Error_ZIP_Internal                  = "Interner Fehler beim Laden der ZIP-Datei!"

; Errors: Directory -----------------------------------------------------------
Msg_Error_Directory_Open                = "Fehler beim Anzeigen des Verzeichnisses!"

; Miscellaneous ---------------------------------------------------------------
Msg_Must_Reset                          = "Die Änderungen werden erst nach einem Reset aktiv."
Msg_No_ROM                              = "Kein ROM geladen!"

; Initialization --------------------------------------------------------------
Msg_Init_Allegro                        = "Initialisiere Allegro.."
Msg_Init_GUI                            = "Initialisiere grafische Benutzeroberflaeche.."
Msg_Init_Completed                      = "[BEREIT]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Soundcard_Select              = "Bitte waehlen Sie ihre Soundkarte:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(DirectSound wird empfohlen)"

; Screenshots Capture ---------------------------------------------------------
Msg_Capture_Done                        = "Screenshot abgelegt in %s"
Msg_Capture_Error                       = "Kann Screenshot nicht erzeugen!"
Msg_Capture_Error_File                  = "Konnte keinen gültigen Dateinamen für das Screenshot erzeugen!"

; On-Board Memory -------------------------------------------------------------
Msg_SRAM_Loaded                         = "SaveRAM wurde geladen (%d Kb)"
Msg_SRAM_Load_Unable                    = "SaveRAM konnte nicht geladen werden"
Msg_SRAM_Wrote                          = "SaveRAM wurde gespeichert (%d Kb)"
Msg_SRAM_Write_Unable                   = "SaveRAM konnte nicht gespeichert werden (%d Kb)"
Msg_93c46_Reset                         = "Reset 93c46"
Msg_93c46_Loaded                        = "93c46-EEPROM-Daten wurden geladen (%d bytes)"
Msg_93c46_Load_Unable                   = "93c46-EEPROM-Daten konnten nicht geladen werden"
Msg_93c46_Wrote                         = "93c46-EEPROM-Daten wurden geschrieben (%d bytes)"
Msg_93c46_Write_Unable                  = "93c46-EEPROM-Daten konnten nicht geschrieben werden (%d bytes)"

; TV Type ---------------------------------------------------------------------
Msg_TVType_Set                          = "TV Typ ist jetzt %s"
Msg_TVType_Info_Speed                   = "(Geschwindigkeit %d Hz)"

; Blitters --------------------------------------------------------------------
Msg_Blitters_Loading                    = "Lade MEKA.BLT (Videomodi / Blitter)... "
Msg_Blitters_Error_Not_Enough           = "Nicht genug Blitter gefunden! (Fehler in MEKA.BLT)"
Msg_Blitters_Error_Not_Found            = "Blitter nicht gefunden!"
Msg_Blitters_Error_Missing              = "Zeile %d: Name für Blitter nicht definiert!"
Msg_Blitters_Error_Unrecognized         = "Zeile %d: Unbekannte Variable!"
Msg_Blitters_Set                        = "Benutze \"%s\" Vollbild-Blitter."

; Nintendo --------------------------------------------------------------------
Msg_NES_Activate                        = "Fetter-Klempner-Modus wurde aktiviert."
Msg_NES_Sucks                           = "Abbruch: der Benutzer mag Nintendo!"
Msg_NES_Mapper_Unknown                  = "Mapper nicht unterstützt: %d. Spiel funktioniert nicht."
Msg_NES_Deny_Facts                      = "Die Fakten sind unbestreitbar!"

; Debugger --------------------------------------------------------------------
Msg_Debug_Init                          = "Initialisiere debugger.. "
Msg_Debug_Welcome                       = "Willkommen im Meka Debugger!"
Msg_Debug_Not_Available                 = "Debugger in dieser Version nicht möglich!"
Msg_Debug_Brk_Need_Param                = "DEBUGBRK Option benötigt eine Adresse!"
Msg_Debug_Trap_Read                     = "Bei PC:%04X - Lese von %04X"
Msg_Debug_Trap_Write                    = "Bei PC:%04X - Schreibe %02X nach %04X"
Msg_Debug_Trap_Port_Read                = "Bei PC:%04X - Port [%02X] Lesen"
Msg_Debug_Trap_Port_Write               = "Bei PC:%04X - Port [%02X] Schreiben %02X"

; Data Dumper -----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Daten werden als ASCII gespeichert."
Msg_DataDump_Mode_Raw                   = "Daten werden als RAW gespeichert."
Msg_DataDump_Error                      = "Fehler: Kann Datei %s nicht erstellen!"
Msg_DataDump_Error_OB_Memory            = "Kein on-board Speicher zum Sichern!"
Msg_DataDump_Error_Palette              = "Keine Palette zum Sichern!"
Msg_DataDump_Error_Sprites              = "Keine Sprites zum Sichern!"
Msg_DataDump_Main                       = "Dumped %s (%d %s) als %s"

; Documentation ---------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Dokumentation"
Msg_Doc_File_Error                      = "Dokumentation kann nicht geöffnet werden!"
Msg_Doc_Enabled                         = "Dokumentation wird angezeigt."
Msg_Doc_Disabled                        = "Dokumentation geschlossen."

; Sprite Flickering -----------------------------------------------------------
Msg_Flickering_Auto                     = "Sprite-flickering wird automatisch gehandhabt."
Msg_Flickering_Yes                      = "Sprite-flickering: Emulation wurde aktiviert."
Msg_Flickering_No                       = "Sprite-flickering: Emulation wurde deaktiviert."

; Layers ----------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Hintergrund aus"
Msg_Layer_BG_Enabled                    = "Hintergrund an"
Msg_Layer_Spr_Disabled                  = "Sprites aus"
Msg_Layer_Spr_Enabled                   = "Sprites an"

; Floppy Disk (FDC-765) -------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Unbekannter Lesezugriff %3i"
Msg_FDC765_Unknown_Write                = "FDC765: Unbekannter Schreibzugriff %3i"
Msg_FDC765_Disk_Too_Large1              = "Datei größer als erwartet (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Weitere Daten werden ignoriert"
Msg_FDC765_Disk_Too_Small1              = "Disk Image ist kleiner als erwartet (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Fehlende Daten werden als 0 gelesen"

; Terebi Oekaki (Graphic Board) -----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "Stift berührt Zeichenbrett"
Msg_TVOekaki_Pen_Away                   = "Stift berührt Zeichenbrett nicht"

; Palette Viewer --------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Palette"
Msg_Palette_Disabled                    = "Palettenbetrachter geschlossen"
Msg_Palette_Enabled                     = "Palettenbetrachter aktiviert"

; Message box -----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Meldungen"
Msg_Message_Disabled                    = "Fenster für Meldungen geschlossen"
Msg_Message_Enabled                     = "Fenster für Meldungen aktiviert"

; Technical Information box ---------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Technische Infos"
Msg_TechInfo_Disabled                   = "Infobox geschlossen"
Msg_TechInfo_Enabled                    = "Infobox aktiviert"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Kacheln"
Msg_TilesViewer_Disabled                = "Kachelbetrachter geschlossen"
Msg_TilesViewer_Enabled                 = "Kachelbetrachter aktiviert"
Msg_TilesViewer_Tile                    = "Kachel %d ($%X) @ %s"

; Rapid Fire ------------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Dauerfeuer für Spieler %d Knopf %d eingeschaltet"
Msg_RapidFire_JxBx_Off                  = "Dauerfeuer für Spieler %d Knopf %d ausgeschaltet"

; FM Unit (YM-2413 chipset) ---------------------------------------------------
Msg_FM_Enabled                          = "FM Einheit (YM-2413 chipset) Emulation aktiviert"
Msg_FM_Disabled                         = "FM Einheit (YM-2413 chipset) Emulation deaktiviert"

; Country ---------------------------------------------------------------------
Msg_Country_European_US                 = "Emuliere europäisches/US Gerät"
Msg_Country_JAP                         = "Emuliere japanisches Gerät"

; Patching System -------------------------------------------------------------
Msg_Patch_Loading                       = "Lade MEKA.PAT (Patches)... "
;Msg_Patch_Missing                       = "Zeile %d: Keine Prüfsumme für Befehl definiert!"
;Msg_Patch_Unrecognized                  = "Zeile %d: Unbekannter Befehl!"
;Msg_Patch_Out_of_Bound                  = "Der Patch versuchte einen ungültigen Zugriff an Position $%02X. Abbruch."

; 3-D Glasses -----------------------------------------------------------------
Msg_Glasses_Enabled                     = "3D-Brille: Emulation ein"
Msg_Glasses_Disabled                    = "3D-Brille: Emulation aus"
Msg_Glasses_Show_Both                   = "3D-Effekt beibehalten."
Msg_Glasses_Show_Left                   = "3D-Effekt deaktiviert, zeige nur linke Seite."
Msg_Glasses_Show_Right                  = "3D-Effekt deaktiviert, zeige nur rechte Seite."
Msg_Glasses_Com_Port                    = "3D-Brille an COM Port %d aktiviert"
;Msg_Glasses_Com_Port2                   = "(Anschluß kann in MEKA.CFG geändert werden)"
Msg_Glasses_Unsupported                 = "Dieses Spiel unterstützt keine 3D-Brille."

; Inputs Initialization -------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Initialisiere Joystick.. "
Msg_Inputs_Joy_Init_None                = "nicht gefunden."
Msg_Inputs_Joy_Init_Found               = "%d gefunden."
Msg_Inputs_Joy_Calibrate_Error          = "Fehler während der Kalibrierung. Abbruch."

; Inputs ----------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Eingabegerät: Gamepad"
Msg_Inputs_LightPhaser                  = "Eingabegerät: Light Phaser"
Msg_Inputs_PaddleControl                = "Eingabegerät: Paddle Control"
Msg_Inputs_SportsPad                    = "Eingabegerät: Sports Pad"
Msg_Inputs_TVOekaki                     = "Eingabegerät: TV Oekaki Graphic Board"
Msg_Inputs_Play_Digital                 = "Kontrolle über digitales Eingabegerät"
Msg_Inputs_Play_Mouse                   = "Kontrolle über analoges Gerät (Maus)"
Msg_Inputs_Play_Pen                     = "Kontrolle über Pen mit Maus\nRechtsklick simuliert Anheben des Stifts."

; Inputs Sources --------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Lade MEKA.INP (Eingabegeraete)... "
Msg_Inputs_Src_Not_Enough               = "Nicht genug Eingabegeraete definiert! (Fehler in MEKA.INP)"
Msg_Inputs_Src_Missing                  = "Zeile %d: Dem Wert kann kein Eingabegeraet zugeordnet werden!"
Msg_Inputs_Src_Equal                    = "Zeile %d: Fehlendes Gleichheitszeichen!"
Msg_Inputs_Src_Unrecognized             = "Zeile %d: Unbekannte Variable \"%s\"!"
Msg_Inputs_Src_Syntax_Param             = "Zeile %d: Syntaxfehler!"
Msg_Inputs_Src_Inconsistency            = "Zeile %d: Konflikt mit vorherigen Einstellungen!"
Msg_Inputs_Src_Map_Keyboard             = "Bitte drücken Sie die Taste für diesen Befehl"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Befehl wurde Taste \"%s\" zugewiesen."
Msg_Inputs_Src_Map_Joypad               = "Bitte drücken Sie Knopf oder Achse für diesen Befehl"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Befehl wurde Gamepad %i, Achse %i, %c zugewiesen."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Befehl wurde Gamepadknopf %i. zugewiesen"
Msg_Inputs_Src_Map_Mouse                = "Bitte drücken Sie die Taste für diesen Befehl"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Befehl wurde Mausknopf %i zugewiesen."
Msg_Inputs_Src_Map_Mouse_No_A           = "Die Mausachse kann leider nicht geändert werden"
Msg_Inputs_Src_Map_Cancelled            = "Zuweisung abgebrochen!"

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Eingabe-Konfiguration"

; Machine ---------------------------------------------------------------------
Msg_Machine_Pause                       = "Emulation angehalten"
Msg_Machine_Resume                      = "Emulation fortgesetzt"
Msg_Machine_Reset                       = "Reset"

; Filenames Database ---------------------------------------------------------
;Msg_VFN_Loading                         = "Lade MEKA.VFN (Virtuelle Dateinamen)... "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Lade MEKA.NAM (Spielnamen)... "
Msg_DB_Name_Default                     = "Spielbildschirm"
Msg_DB_Name_NoCartridge                 = "Keine Cartridge"

; Configuration File ----------------------------------------------------------
Msg_Config_Loading                      = "Lade %s (Konfiguration)... "

; Datafile ------------------------------------------------------------------------
Msg_Datafile_Loading                    = "Lade MEKA.DAT (Ressourcen)... "

; Drivers ---------------------------------------------------------------------
Msg_Driver_Unknown                      = "Abbruch: Unbekannter Treiber!"

; Overdump detection ----------------------------------------------------------
Msg_OverDump                            = "Das ROM enthält die benötigten Daten %d -fach.\n Der Fehler kann mit dem \"SMS Checker\" korrigiert werden."

; Sound -----------------------------------------------------------------------
Msg_Sound_Init                          = "Initialisiere Soundsystem.."
Msg_Sound_Init_Error_Audio              = " - Fehler beim Ansprechen der Soundkarte. Bitte eine andere Karte und/oder Samplerate waehlen."
Msg_Sound_Init_Error_Blaster            = " - BLASTER Variable nicht gefunden, FM kann nicht benutzt werden!"
Msg_Sound_Init_Error_Blaster_A          = " - Eintrag (A=xxx) fehlt in BLASTER, FM kann nicht benutzt werden!"
Msg_Sound_Init_Error_Voices             = " - Stimmeninitialisierung fehlgeschlagen."
Msg_Sound_Init_Error_Voice_N            = " - Stimme #%d konnte nicht angesprochen werden."
Msg_Sound_Init_Soundcard                = " - Initialisiere Soundkarte @ %d Hz"
Msg_Sound_Init_Soundcard_No             = "Audio kann nicht initialisiert werden: Es wurde keine Soundkarte gewaehlt!"
;Msg_Sound_Init_SN76496_Ok               = " - SN76496 Emulator: Ok"
;Msg_Sound_Init_YM2413_Ok                = " - YM2413 Emulator: Ok"
Msg_Sound_Stream_Error                  = "Klangpuffer angehalten. Stream wird neu gestartet."

; Theme -----------------------------------------------------------------------
Msg_Theme_Loading                       = "Lade MEKA.THM (Themen)... "
Msg_Theme_Error_Not_Enough              = "Nicht genug Themen gefunden! (Fehler in MEKA.THM)"
Msg_Theme_Error_Missing_Theme_Name      = "Zeile %d: Variable kann keinem Thema zugeordnet werden!"
Msg_Theme_Error_BG_Big                  = "Hintergrundbild zu klein, um korrekt vergrößert zu werden!"
Msg_Theme_Error_BG                      = "Fehler beim Laden des Hintergrundbilds!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ----------------------------------------------------------
Msg_LoadROM_Loading                     = "Lade ROM.. "
Msg_LoadROM_Success                     = "%s wurde erfolgreich geladen"
Msg_LoadDisk_Success                    = "%s wurde als Diskette geladen"
Msg_LoadROM_Comment                     = "Kommentar: %s"
Msg_LoadROM_Warning                     = "-Warnung-"
;Msg_LoadROM_Bad_Dump_Long               = "Dieses ROM wurde fehlerhaft gedumpt.\n Es wird wahrscheinlich nicht richtig funktionieren."
Msg_LoadROM_Bad_Dump_Short              = "Warnung - Dieses ROM wurde fehlerhaft gedumpt."
Msg_LoadROM_Product_Num                 = "Produktnummer: %s"
Msg_LoadROM_SDSC                        = "-SDSC Kopf-"
Msg_LoadROM_SDSC_Name                   = "Name: %s"
Msg_LoadROM_SDSC_Version                = "Version: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Datum: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Autor: %s"
Msg_LoadROM_SDSC_Release_Note           = "Bemerkungen: %s"
Msg_LoadROM_SDSC_Unknown                = "<Unbekannt>"
Msg_LoadROM_SDSC_Error                  = "<Fehler>"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Lade ROM"
Msg_FileBrowser_Drive                   = "[Laufwerk %c]"
Msg_FileBrowser_Load                    = "Lade"
Msg_FileBrowser_Close                   = "Abbruch"
Msg_FileBrowser_LoadNames               = "Lade Namen"
Msg_FileBrowser_ReloadDir               = "Auffrischen"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "FM-Stimmen"
Msg_FM_Editor_Enabled                   = "FM-Voice-Editor aktiviert"
Msg_FM_Editor_Disabled                  = "FM-Voice-Editor geschlossen"

; Video Options ---------------------------------------------------------------
Msg_Frameskip_Auto                      = "Frameskip: Automatisch (%d Hz)"
Msg_Frameskip_Standard                  = "Frameskip: Wert gesetzt auf: 1/%d"
Msg_FPS_Counter_Enabled                 = "FPS-Anzeige ein"
Msg_FPS_Counter_Disabled                = "FPS-Anzeige aus"

; Logging ---------------------------------------------------------------------
Msg_Log_Need_Param                      = "LOG benötigt einen Dateinamen!"
Msg_Log_Session_Start                   = "-- Logging gestartet um %s"

; Save States -----------------------------------------------------------------
Msg_Load_Need_Param                     = "Laden benötigt Nummer des Slots!"
Msg_Load_Error                          = "Fehler beim Öffnen von %s!"
Msg_Load_Not_Valid                      = "Datei %s ist kein gültiges Savegame für %s!"
Msg_Load_Success                        = "Status geladen aus %s"
Msg_Load_Version                        = "Die Dateiversion von %s wird nicht unterstützt!"
Msg_Load_Wrong_System                   = "%s ist kein Savegame für dieses System!"
Msg_Load_Massage                        = "Datei entspricht dem MSD Format - Konvertiere"
Msg_Save_Error                          = "Fehler beim Schreiben von %s!"
Msg_Save_Success                        = "Spiel gespeichert in %s"
Msg_Save_Slot                           = "Slot zum Speichern: %d"
Msg_Save_Not_in_BIOS                    = "Speichern im BIOS nicht möglich!"

; Options ---------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Optionen"
Msg_Options_Close                       = "OK"
;Msg_Options_BIOS_Enable                 = "Zeige BIOS-Logo, wenn vom System unterstützt."
;Msg_Options_Product_Number              = "Zeige Produktnummer des Spiels, wenn bekannt."
;Msg_Options_Bright_Palette              = "Helle Palette für Master System / Game Gear."
Msg_Options_Load_Close                  = "Schließe Dateifenster, nachdem Spiel geladen wurde."
Msg_Options_Load_FullScreen             = "Nach Laden automatisch Vollbild aktivieren."
;Msg_Options_VFN_Support                 = "Lange Dateinamen, sowie Länderfahnen anzeigen."
Msg_Options_FullScreen_Messages         = "Meldungen im Vollbildmodus anzeigen."
Msg_Options_GUI_VSync                   = "Auf VSync im Interfacemodus warten."
Msg_Options_NES_Enable                  = "Mario ist kein fetter Klempner."

; Languages / Localization ----------------------------------------------------
Msg_Language_Set                        = "Sprache für Meldungen ist jetzt %s"
Msg_Language_Set_Warning                = "Hinweis: Zum Aktualisieren aller Nachrichten bitte MEKA neu starten."

; Sound Dumping ---------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Sound-Dump in Datei %s wurde begonnen"
Msg_Sound_Dumping_Stop                  = "Sound-Dump angehalten. (%2.02f Sekunden gespeichert)"
Msg_Sound_Dumping_Error_File_1          = "Es konnte kein gültiger Dateiname für den Sound-Dump erzeugt werden!"
Msg_Sound_Dumping_Error_File_2          = "Fehler beim Öffnen von Datei %s!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "Frame-genaue Sicherung der VGM-Daten (kleinere Dateien, ohne Stimmen)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "Sample-genaue Sicherung der VGM-Daten (größere Dateien, mit Stimmen)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(Der VGM-Dump muß neu gestartet werden, um die geänderte Genauigkeit zu übernehmen)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "DATEI"
Msg_Menu_Main_LoadROM                   = "ROM laden"
Msg_Menu_Main_FreeROM                   = "ROM schließen"
Msg_Menu_Main_SaveState                 = "Status sichern"
Msg_Menu_Main_LoadState                 = "Status laden"
Msg_Menu_Main_Options                   = "Optionen"
Msg_Menu_Main_Language                  = "Sprache"
Msg_Menu_Main_Quit                      = "Ende"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Dump                     = "Sichern"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "MASCHINE"
Msg_Menu_Machine_Power                  = "Power"
Msg_Menu_Machine_Power_On               = "An"
Msg_Menu_Machine_Power_Off              = "Aus"
Msg_Menu_Machine_Country                = "Land"
Msg_Menu_Machine_Country_EU             = "Europa/US"
Msg_Menu_Machine_Country_Jap            = "Japanisch"
Msg_Menu_Machine_TVType                 = "TV-Typ"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Hardwarepause"
Msg_Menu_Machine_HardReset              = "Hardwarereset"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VIDEO"
Msg_Menu_Video_FullScreen               = "Vollbild"
Msg_Menu_Video_Themes                   = "Themen"
Msg_Menu_Video_Blitters                 = "Blitter"
Msg_Menu_Video_Layers                   = "Layer"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Hintergrund"
Msg_Menu_Video_Flickering               = "Flimmern"
Msg_Menu_Video_Flickering_Auto          = "Auto"
Msg_Menu_Video_Flickering_Yes           = "Ja"
Msg_Menu_Video_Flickering_No            = "Nein"
Msg_Menu_Video_3DGlasses                = "3D-Brille"
Msg_Menu_Video_3DGlasses_Enabled        = "Ein"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Zeige beide Seiten"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Zeige linke Seite"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Zeige rechte Seite"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Brille an COM-Port"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "SOUND"
Msg_Menu_Sound_FM                       = "FM-Chip"
Msg_Menu_Sound_FM_Enabled               = "Aktiviert"
Msg_Menu_Sound_FM_Disabled              = "Deaktiviert"
Msg_Menu_Sound_FM_Editor                = "Instrumente.."
Msg_Menu_Sound_Volume                   = "Lautstärke"
Msg_Menu_Sound_Volume_Mute              = "Stumm"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Rate"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Kanäle"
Msg_Menu_Sound_Channels_Tone            = "Ton %d"
Msg_Menu_Sound_Channels_Noises          = "Rauschen"
Msg_Menu_Sound_Dump                     = "Speichern"
Msg_Menu_Sound_Dump_WAV_Start           = "WAV Start"
Msg_Menu_Sound_Dump_WAV_Stop            = "WAV Stop"
Msg_Menu_Sound_Dump_VGM_Start           = "VGM Start"
Msg_Menu_Sound_Dump_VGM_Stop            = "VGM Stop"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "VGM samplegenau"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "EINGABE"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Light Phaser"
Msg_Menu_Inputs_PaddleControl           = "Paddle Control"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Graphiktablett"
Msg_Menu_Inputs_SK1100                  = "Sega Keyboard"
Msg_Menu_Inputs_RapidFire               = "Dauerfeuer"
Msg_Menu_Inputs_RapidFire_PxBx          = "Spieler %d Knopf %d"
Msg_Menu_Inputs_Configuration           = "Konfiguration"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "WERKZEUGE"
Msg_Menu_Tools_Messages                 = "Meldungen"
Msg_Menu_Tools_Palette                  = "Palette"
Msg_Menu_Tools_TilesViewer              = "Kachelbetrachter"
Msg_Menu_Tools_TechInfo                 = "Technische Infos"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "HILFE"
Msg_Menu_Help_Documentation             = "Dokumentation"
Msg_Menu_Help_Documentation_W           = "Dokumentation (Windows)"
Msg_Menu_Help_Compat                    = "Unterstützte Spiele"
Msg_Menu_Help_Multiplayer_Games         = "Multiplayer"
Msg_Menu_Help_Changes                   = "Was ist neu?"
Msg_Menu_Help_About                     = "Über"

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Swedish]
WIP
; Translation by Henrik Sankala
; Maintainer: Henrik Sankala (hensan-7 AT student.ltu.se)
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Välkommen till %s (c) %s"
;Msg_Window_Title                        = "MEKA - 8-bitar eller dö!"
Msg_Quit                                = "God natt, modiga krigare. God natt, monster-land...."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "Om Meka.."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "av %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Ok                                  = "Ok"
Msg_Failed                              = "Misslyckades!"
Msg_Error_Base                          = "Fel: %s"
Msg_Error_Error                         = "Allvarligt fel: Fel vid hämtning av feltyp. Något måste vara fel."
Msg_Error_Memory                        = "Ej tillräckligt med minne!"
Msg_Error_Param                         = "Parameter \"%s\" är ogiltig!\n"
Msg_Error_Syntax                        = "Syntaxfel!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Kan ej byta till önskat videoläge (%dx%d)."
Msg_Error_Video_Mode_Back_To_GUI        = "Växlar tillbaka till grafiskt användargränssnitt."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "Kan ej hitta fil!"
Msg_Error_File_Read                     = "Kan ej läsa från fil!"
Msg_Error_File_Empty                    = "Filen är tom!"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "Denna version kan ej hantera ZIP filer!"
Msg_Error_ZIP_Loading                   = "Kan ej läsa ZIP fil!"
Msg_Error_ZIP_Internal                  = "Internt fel vid laddning av ZIP fil!"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Fel vid öppning av katalog!"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Resetta spelet för att verkställa ändringarna"
Msg_No_ROM                              = "Ingen ROM laddades!"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Initialiserar Allegro.."
Msg_Init_GUI                            = "Initialiserar grafiskt användargränssitt.."
Msg_Init_Completed                      = "[REDO]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Setup                         = "Inställningar"
Msg_Setup_Soundcard_Select              = "Välj ljudenhet:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(DirectSound är rekommenderat, om tillgängligt)"
Msg_Setup_SampleRate_Select             = "Samplingshastighet:"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Skärmdump sparad till %s"
Msg_Capture_Error                       = "Kan ej skapa skärmdump!"
Msg_Capture_Error_File                  = "Kan ej hitta giltigt filnamn för sparande av skärmdump!"

; On-Board Memory ------------------------------------------------------------
Msg_SRAM_Loaded                         = "Laddade SaveRAM från disk (%d Kb)"
Msg_SRAM_Load_Unable                    = "Kan ej ladda SaveRAM från disk"
Msg_SRAM_Wrote                          = "Sparade SaveRAM till disk (%d Kb)"
Msg_SRAM_Write_Unable                   = "Kan ej spara SaveRAM till disk (%d Kb)"
Msg_93c46_Reset                         = "Resettar 93c46"
Msg_93c46_Loaded                        = "Laddade 93c46 EEPROM data från disk (%d byte)"
Msg_93c46_Load_Unable                   = "Kan ej ladda 93c46 EEPROM data från disk"
Msg_93c46_Wrote                         = "Sparade 93c46 EEPROM data till disk (%d bytes)"
Msg_93c46_Write_Unable                  = "Kan ej spara 93c46 EEPROM data till disk (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "TV typ är nu %s"
Msg_TVType_Info_Speed                   = "(Ursprunglig hastighet är %d Hz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Laddar MEKA.BLT (video lägen / blitters).. "
Msg_Blitters_Error_Not_Enough           = "Kunde ej hitta tillräckligt med Blitters! Kontrollera filen MEKA.BLT eller försök installera om."
Msg_Blitters_Error_Not_Found            = "Kunde ej hitta Blitter!"
Msg_Blitters_Error_Missing              = "På rad %d: Inget blitternamn definierat för lagrande av värde!"
Msg_Blitters_Error_Unrecognized         = "På rad %d: Okänd variabel!"
Msg_Blitters_Set                        = "Använder nu \"%s\" fullskärmsblitter."

; Nintendo -------------------------------------------------------------------
Msg_NES_Activate                        = "Fet rörmokare läge aktiverat"
Msg_NES_Sucks                           = "Allvarligt fel: Användaren gillar Nintendo!"
Msg_NES_Mapper_Unknown                  = "Ostödd mapper: %d. Spelet kommer ej att fungera."
Msg_NES_Deny_Facts                      = "Du kan inte förneka fakta."

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Initialiserar debugger.. "
Msg_Debug_Welcome                       = "Välkommen till Meka Debugger!"
Msg_Debug_Not_Available                 = "Debugger ej tillgänglig i denna version!"
Msg_Debug_Brk_Need_Param                = "DEBUGBRK valet kräver en adress!"
Msg_Debug_Trap_Read                     = "Vid PC:%04X - Läsning från %04X"
Msg_Debug_Trap_Write                    = "Vid PC:%04X - Skrivning %02X till %04X"
Msg_Debug_Trap_Port_Read                = "Vid PC:%04X - Port [%02X] Läsning"
Msg_Debug_Trap_Port_Write               = "Vid PC:%04X - Port [%02X] Skrivning %02X"

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Data dumpar kommer nu att göras i ASCII läge."
Msg_DataDump_Mode_Raw                   = "Data dumpar kommer nu att göras i Raw läge."
Msg_DataDump_Error                      = "Fel vid skapande av fil att dumpa %s till!"
Msg_DataDump_Error_OB_Memory            = "Inget on-board minne att dumpa!"
Msg_DataDump_Error_Palette              = "Ingen palett att dumpa!"
Msg_DataDump_Error_Sprites              = "Inga sprites att dumpa!"
Msg_DataDump_Main                       = "Dumpade %s (%d %s) i %s läge"

; Documentation --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Dokumentation"
Msg_Doc_File_Error                      = "Kan ej öppna dokumentation!"
Msg_Doc_Enabled                         = "Dokumentation visas."
Msg_Doc_Disabled                        = "Dokumentation stängd."

; Sprite Flickering ----------------------------------------------------------
Msg_Flickering_Auto                     = "Sprite flimmer kommer att hanteras automatiskt."
Msg_Flickering_Yes                      = "Sprite flimmer emulering är nu på."
Msg_Flickering_No                       = "Sprite flimmer emulering är nu av."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Bakgrundsuppdatering av"
Msg_Layer_BG_Enabled                    = "Bakgrundsuppdatering på"
Msg_Layer_Spr_Disabled                  = "Spriteuppdatering av"
Msg_Layer_Spr_Enabled                   = "Spriteuppdatering på"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Okänt diskläsningskommando %3i"
Msg_FDC765_Unknown_Write                = "FDC765: Okänt diskskrivningskommando %3i"
Msg_FDC765_Disk_Too_Large1              = "Disk bilden är större än väntat (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Ytterligare data kommer att ignoreras"
Msg_FDC765_Disk_Too_Small1              = "Disk bilden är mindre än väntat (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Saknad data kommer att antas vara tomt"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "Pennan rör vid grafikbordet"
Msg_TVOekaki_Pen_Away                   = "Pennan rör ej vid grafikbordet"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Palett"
Msg_Palette_Disabled                    = "Palettgranskare av"
Msg_Palette_Enabled                     = "Palettgranskare på"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Meddelanden"
Msg_Message_Disabled                    = "Meddelandefönster av"
Msg_Message_Enabled                     = "Meddelandefönster på"

; Technical Information box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Teknisk information"
Msg_TechInfo_Disabled                   = "Fönster för Teknisk information av"
Msg_TechInfo_Enabled                    = "Fönster för Teknisk information på"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Tilegranskare"
Msg_TilesViewer_Disabled                = "Tilegranskare av"
Msg_TilesViewer_Enabled                 = "Tilegranskare på"
Msg_TilesViewer_Tile                    = "Tile %d ($%X) @ %s"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Automateld är nu aktiverat för spelare %d knapp %d"
Msg_RapidFire_JxBx_Off                  = "Automateld är nu avaktiverat för spelare %d knapp %d"

; FM Unit (YM-2413 chipset) --------------------------------------------------
Msg_FM_Enabled                          = "Emulering av FM enhet (YM-2413 chipset) på"
Msg_FM_Disabled                         = "Emulering av FM enhet (YM-2413 chipset) av"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "Den emulerade maskinen är nu Europeisk/Amerikansk"
Msg_Country_JAP                         = "Den emulerade maskinen är nu Japansk"

; Patching System ------------------------------------------------------------
Msg_Patch_Loading                       = "Laddar MEKA.PAT (patchar).. "
Msg_Patch_Missing                       = "På rad %d: Ingen checksum definierad för att köra kommando på!"
Msg_Patch_Unrecognized                  = "På rad %d: Syntaxfel eller okänt kommando!"
Msg_Patch_Value_Not_a_Byte              = "På rad %d: Värdet %s rymms inte i en unsigned byte!"
Msg_Patch_Out_of_Bound                  = "Under patchning %s: Address $%04X är utanför gränserna. Patch ej applicerad."

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "Emulering av 3D glasögon på"
Msg_Glasses_Disabled                    = "Emulering av 3D glasögon av"
Msg_Glasses_Show_Both                   = "3D effekter oförändrade."
Msg_Glasses_Show_Left                   = "3D effekter av genom att visa endast vänster sida."
Msg_Glasses_Show_Right                  = "3D effekter av genom att visa endast höger sida."
Msg_Glasses_Com_Port                    = "Emulering av 3D glasögon genom COM port %d."
Msg_Glasses_Com_Port2                   = "(Redigera MEKA.CFG för att välja COM port)"
Msg_Glasses_Com_Port_Open_Error         = "Fel vid öppnande av COM port %d. 3-D glasögonen kommer ej att fungera."
Msg_Glasses_Unsupported                 = "Detta spel verkar inte stödja 3D glasögon."

; Inputs Initialization ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Initialiserar joystick.. "
Msg_Inputs_Joy_Init_None                = "hittades ej."
Msg_Inputs_Joy_Init_Found               = "%d hittades."
Msg_Inputs_Joy_Calibrate_Error          = "Fel vid kalibrering av joystick. Avbryter."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Styrenhet: Joypad"
Msg_Inputs_LightPhaser                  = "Styrenhet: Ljuspistol"
Msg_Inputs_PaddleControl                = "Styrenhet: Paddel Kontroll"
Msg_Inputs_SportsPad                    = "Styrenhet: Sports Pad"
Msg_Inputs_TVOekaki                     = "Styrenhet: TV Oekaki Grafikbord"
Msg_Inputs_Play_Digital                 = "Kontrollera med digital styrenhet"
Msg_Inputs_Play_Mouse                   = "Kontrollera med analog styrenhet (mus)"
Msg_Inputs_Play_Digital_Unrecommended   = "Digitala styrenheter (t.ex: tangentbord) stöds men rekommenderas EJ"
Msg_Inputs_Play_Pen                     = "Kontrollera penna med mus\nHöger-klick simulerar att pennan avlägsnas från bordet"

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Konfigurering av styrenheter"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Laddar MEKA.INP (definition av källor för styrenheter).. "
Msg_Inputs_Src_Not_Enough               = "Kunde ej hitta tillräckligt med styrenhetskällor! Kontrollera filen MEKA.INP eller försök installera om."
Msg_Inputs_Src_Missing                  = "På rad %d: Ingen källa definierad för lagring av värde!"
Msg_Inputs_Src_Equal                    = "På rad %d: Likhetstecken saknas för variabeltilldelning!"
Msg_Inputs_Src_Unrecognized             = "På rad %d: Okänd variabel \"%s\"!"
Msg_Inputs_Src_Syntax_Param             = "På rad %d: Syntaxfel på variabelparameter!"
Msg_Inputs_Src_Inconsistency            = "På rad %d: Oförenlighet med föregående inställningar!"
Msg_Inputs_Src_Map_Keyboard             = "Tryck nu på den tangent du vill tilldela denna åtgärd till"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Åtgärden tilldelad till tangent \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Tryck nu på den knapp/axel du vill tilldela denna åtgärd till"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Åtgärden tilldelad till joypad spak %i, axel %i, %c."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Åtgärden tilldelad till joypad knapp %i."
Msg_Inputs_Src_Map_Mouse                = "Tryck nu på den knapp du vill tilldela denna åtgärd"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Åtgärden tilldelad till musknapp %i."
Msg_Inputs_Src_Map_Mouse_No_A           = "Tyvärr - Mus axlarna kan ej ändras"
Msg_Inputs_Src_Map_Cancelled            = "Tilldelning avbruten!"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Maskinemulering pausad"
Msg_Machine_Resume                      = "Maskinemulering återupptagen"
Msg_Machine_Reset                       = "Maskinen resettad"

; Virtual File Names ---------------------------------------------------------
;Msg_VFN_Loading                         = "Laddar MEKA.VFN (virtuella filnamn).. "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Laddar MEKA.NAM (spelnamn).. "
Msg_DB_Name_Default                     = "Spelskärm"
Msg_DB_Name_NoCartridge                 = "Ingen spelkassett"
Msg_DB_SyntaxError                      = "På rad %d: Syntaxfel!"

; Configuration File ---------------------------------------------------------
Msg_Config_Loading                      = "Laddar MEKA.CFG (konfiguration).. "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Laddar MEKA.DAT (resurser).. "

; Drivers --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Allvarligt fel: Okänd maskindrivrutin!"

; Overdump detection ---------------------------------------------------------
;Msg_OverDump                            = "Denna ROM innehåller %d gånger mer data än nödvändigt.\nLadda hem \"SMS Checker\" för att fixa den (http://www.smspower.org/maxim/smschecker/)."

; Sound ----------------------------------------------------------------------
Msg_Sound_Init                          = "Initialiserar ljudsystem.."
Msg_Sound_Init_Error_Audio              = " - Fel vid öppning av ljudsystem. Försök med ett annat kort och/eller samplingsfrekvens."
Msg_Sound_Init_Error_Blaster            = " - BLASTER variabel hittades ej, FM ljud av!"
Msg_Sound_Init_Error_Blaster_A          = " - BLASTER variabel innehåller ej adress (A=xxx), FM ljud av!"
Msg_Sound_Init_Error_Voices             = " - Röstinitialisering misslyckades."
Msg_Sound_Init_Error_Voice_N            = " - Skapande av röst #%d misslyckades."
Msg_Sound_Init_Soundcard                = " - Initialiserar ljudkort @ %d Hz"
Msg_Sound_Init_Soundcard_No             = "Kan ej initialisera ljud: inget ljudkort valt!"
Msg_Sound_Init_SN76496                  = " - SN76496 digital emulator:"
Msg_Sound_Init_YM2413_Digital           = " - YM2413 digital emulator:"
Msg_Sound_Stream_Error                  = "Ljudbuffer stop. Startar om strömmen."
Msg_Sound_Volume_Changed                = "Volym ändrad till %d%%."

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Laddar MEKA.THM (gränssnittsteman).. "
Msg_Theme_Error_Not_Enough              = "Kunde ej hitta tillräckligt med teman! Kontrollera filen MEKA.THM eller försök installera om."
Msg_Theme_Error_Missing_Theme_Name      = "På rad %d: Inget temanamn definierat för lagring av värde!"
Msg_Theme_Error_BG_Big                  = "Bakgrundsbild för liten för att passa med heltals skalning!"
Msg_Theme_Error_BG                      = "Fel vid laddning av bild associerad med temat!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Laddar ROM.. "
Msg_LoadROM_Success                     = "Laddade %s"
Msg_LoadDisk_Success                    = "Laddade %s som en diskett"
Msg_LoadROM_Comment                     = "Kommentar: %s"
Msg_LoadROM_SMSGG_Mode_Comment          = "Detta är en Game Gear ROM som körs i Master System-kompatabilitetsläge."
Msg_LoadROM_Warning                     = "-Varning-"
Msg_LoadROM_Bad_Dump_Long               = "Denna ROM är en känd dålig dump.\nDen kan komma att fungera otillfredsställande. Ladda hem SMS Checker för att kontrollera den (http://www.smspower.org/maxim/smschecker/)"
Msg_LoadROM_Bad_Dump_Short              = "Varning - Denna ROM är felaktigt dumpad."
Msg_LoadROM_Product_Num                 = "Produktnummer: %s"
Msg_LoadROM_SDSC                        = "-SDSC Header-"
Msg_LoadROM_SDSC_Name                   = "Namn: %s"
Msg_LoadROM_SDSC_Version                = "Version: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Datum: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Författare: %s"
Msg_LoadROM_SDSC_Release_Note           = "Information: %s"
Msg_LoadROM_SDSC_Unknown                = "<Okänt>"
Msg_LoadROM_SDSC_Error                  = "<Fel>"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Ladda ROM"
Msg_FileBrowser_Drive                   = "[Enhet %c]"
Msg_FileBrowser_Load                    = "Ladda"
Msg_FileBrowser_Close                   = "Stäng"
Msg_FileBrowser_LoadNames               = "Ladda namn"
Msg_FileBrowser_ReloadDir               = "Uppdatera"

; FM Instruments Editor ------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "FM Instrumentediterare"
Msg_FM_Editor_Enabled                   = "FM Instrumentediterare på"
Msg_FM_Editor_Disabled                  = "FM Instrumentediterare av"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Frameskip värde satt till: Automatisk (%d Hz)"
Msg_Frameskip_Standard                  = "Frameskip värde satt till: 1/%d"
Msg_FPS_Counter_Enabled                 = "FPS-räknare på"
Msg_FPS_Counter_Disabled                = "FPS-räknare av"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "LOG valet kräver ett filnamn!"
Msg_Log_Session_Start                   = "-- Loggnings session startad på %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "LADDA valet kräver ett savestate nummer!"
Msg_Load_Error                          = "Fel vid öppnande av %s!"
Msg_Load_Not_Valid                      = "Filen %s är inte en giltig %s savegame!"
Msg_Load_Success                        = "Tillstånd laddat från %s"
Msg_Load_Version                        = "%s är en ostödd savegame version!"
Msg_Load_Wrong_System                   = "%s är inte ett savegame för detta system!"
Msg_Load_Massage                        = "Filen är i MSD format - konverterar"
Msg_Save_Not_in_BIOS                    = "Kan ej spara när BIOS körs!"
Msg_Save_Error                          = "Fel vid skrivning till %s!"
Msg_Save_Success                        = "Tillstånd sparat till %s"
Msg_Save_Slot                           = "Nuvarande savegame slot är: %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Inställningar"
Msg_Options_Close                       = "Stäng"
Msg_Options_BIOS_Enable                 = "Visa BIOS logo."
Msg_Options_DB_Display                  = "Visa namn, flaggor och ikoner från databasen."
Msg_Options_Product_Number              = "Visa produktnummer från databasen."
Msg_Options_Bright_Palette              = "Aktivera ljus palett på SMS och GG."
Msg_Options_Load_Close                  = "Stäng filgranskaren efter laddning."
Msg_Options_Load_FullScreen             = "Växla till fullskärmsläge efter laddning."
Msg_Options_FullScreen_Messages         = "Visa meddelanden i fullskärm."
Msg_Options_GUI_VSync                   = "Vänta på VSync i interface läge."
Msg_Options_NES_Enable                  = "Mario är inte en fet rörmokare."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "Språk satt till %s"
Msg_Language_Set_Warning                = "OBS: Du rekommenderas att starta om MEKA för att vara säker på att all text uppdateras."

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Började dumpa ljud till %s"
Msg_Sound_Dumping_Stop                  = "Slutade dumpa ljud (%2.02f sekunder loggade)"
Msg_Sound_Dumping_Error_File_1          = "Kan ej hitta giltigt filnamn för att dumpa ljud!"
Msg_Sound_Dumping_Error_File_2          = "Fel vid öppning av %s för dumpning!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "VGM dumpar kommer nu vara frame exakta (mindre filer, skippa röster)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "VGM dumpar kommer nu vara sampel exakta (större filer, dumpa röster)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(VGM dumpning måste startas om för att ta hänsyn till ny noggranhet)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "ARKIV"
Msg_Menu_Main_LoadROM                   = "Ladda ROM"
Msg_Menu_Main_FreeROM                   = "Frigör ROM"
Msg_Menu_Main_SaveState                 = "Spara tillstånd"
Msg_Menu_Main_LoadState                 = "Ladda tillstånd"
Msg_Menu_Main_Options                   = "Inställningar"
Msg_Menu_Main_Language                  = "Språk"
Msg_Menu_Main_Quit                      = "Avsluta"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Enabled                  = "Aktiverad"
Msg_Menu_Debug_Dump                     = "Dump"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "MASKIN"
Msg_Menu_Machine_Power                  = "Ström"
Msg_Menu_Machine_Power_On               = "På"
Msg_Menu_Machine_Power_Off              = "Av"
Msg_Menu_Machine_Country                = "Land"
Msg_Menu_Machine_Country_EU             = "Europa/USA"
Msg_Menu_Machine_Country_Jap            = "Japan"
Msg_Menu_Machine_TVType                 = "TV Typ"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Hård Paus"
Msg_Menu_Machine_HardReset              = "Hård Reset"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "BILD"
Msg_Menu_Video_FullScreen               = "Fullskärm"
Msg_Menu_Video_Themes                   = "Teman"
Msg_Menu_Video_Blitters                 = "Blitters"
Msg_Menu_Video_Layers                   = "Lager"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Bakgrund"
Msg_Menu_Video_Flickering               = "Flimmer"
Msg_Menu_Video_Flickering_Auto          = "Auto"
Msg_Menu_Video_Flickering_Yes           = "Ja"
Msg_Menu_Video_Flickering_No            = "Nej"
Msg_Menu_Video_3DGlasses                = "3D Glasögon"
Msg_Menu_Video_3DGlasses_Enabled        = "På"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Visa båda sidor"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Visa vänster sida"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Visa höger sida"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Använder glasögon på COM port"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "LJUD"
Msg_Menu_Sound_FM                       = "FM enhet"
Msg_Menu_Sound_FM_Enabled               = "På"
Msg_Menu_Sound_FM_Disabled              = "Av"
Msg_Menu_Sound_FM_Editor                = "Instrumentediterare"
Msg_Menu_Sound_Volume                   = "Volym"
Msg_Menu_Sound_Volume_Mute              = "Ljud av"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Kvalitet"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Kanaler"
Msg_Menu_Sound_Channels_Tone            = "Ton %d"
Msg_Menu_Sound_Channels_Noises          = "Oljud"
Msg_Menu_Sound_Dump                     = "Dump"
Msg_Menu_Sound_Dump_WAV_Start           = "WAV Start"
Msg_Menu_Sound_Dump_WAV_Stop            = "WAV Stop"
Msg_Menu_Sound_Dump_VGM_Start           = "VGM Start"
Msg_Menu_Sound_Dump_VGM_Stop            = "VGM Stop"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "VGM Sampel exakt"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "STYRENHETER"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Ljuspistol"
Msg_Menu_Inputs_PaddleControl           = "Paddel Kontroll"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Grafikbord"
Msg_Menu_Inputs_SK1100                  = "Sega Tangentbord"
Msg_Menu_Inputs_RapidFire               = "Automateld"
Msg_Menu_Inputs_RapidFire_PxBx          = "Spelare %d Knapp %d"
Msg_Menu_Inputs_Configuration           = "Konfiguration"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "VERKTYG"
Msg_Menu_Tools_Messages                 = "Meddelanden"
Msg_Menu_Tools_Palette                  = "Palett"
Msg_Menu_Tools_TilesViewer              = "Tilegranskare"
Msg_Menu_Tools_TechInfo                 = "Teknisk information"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "HJÄLP"
Msg_Menu_Help_Documentation             = "Dokumentation"
Msg_Menu_Help_Documentation_W           = "Dokumentation (Windows)"
Msg_Menu_Help_Documentation_U           = "Dokumentation (Unix)"
Msg_Menu_Help_Compat                    = "Kompatabilitet"
Msg_Menu_Help_Multiplayer_Games         = "Multiplayerspel"
Msg_Menu_Help_Changes                   = "Ändringar"
Msg_Menu_Help_About                     = "Om MEKA"


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
[Danish]
WIP
; Translation by Marc Klemp
; Maintainer: Marc Klemp (Supreme1 AT gmx.co.uk)
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

; Main -----------------------------------------------------------------------
Msg_Welcome                             = "Velkommen til %s (c) %s"
;Msg_Window_Title                        = "MEKA - 8-bit eller dø!"
Msg_Quit                                = "Godnat modige kriger. Godnat monster-land...."

; About ----------------------------------------------------------------------
Msg_About_BoxTitle                      = "Om Meka.."
Msg_About_Line_Meka_Date                = "%s (c) %s"
Msg_About_Line_Authors                  = "af %s"
Msg_About_Line_Homepage                 = "%s"

; Errors ---------------------------------------------------------------------
Msg_Ok                                  = "Ok"
Msg_Failed                              = "Fejlede!"
Msg_Error_Base                          = "Fejl: %s"
Msg_Error_Error                         = "Fatalt: fejl i hentning af fejltype. Noget må være galt."
Msg_Error_Memory                        = "Ikke tilstraekkeligt med hukommelse"
Msg_Error_Param                         = "Parameter \"%s\" er ugyldig\n"
Msg_Error_Syntax                        = "Syntaksfejl!"

; Errors: Video --------------------------------------------------------------
Msg_Error_Video_Mode                    = "Ikke i stand til at skifte til den oenskede oploesning (%dx%d)."
Msg_Error_Video_Mode_Back_To_GUI        = "Skifter tilbage til den grafiske brugergraenseflade."

; Errors: File ---------------------------------------------------------------
Msg_Error_File_Not_Found                = "Fil ikke fundet!"
Msg_Error_File_Read                     = "Ikke i stand til at læse fra fil!"
Msg_Error_File_Empty                    = "Filen er tom!"

; Errors: ZIP File -----------------------------------------------------------
Msg_Error_ZIP_Not_Supported             = "Denne version håndterer ikke ZIP filer!"
Msg_Error_ZIP_Loading                   = "Ikke i stand til at læse ZIP fil!"
Msg_Error_ZIP_Internal                  = "Intern fejl under læsning af ZIP fil!"

; Errors: Directory ----------------------------------------------------------
Msg_Error_Directory_Open                = "Kan ikke åbne biblioteket!"

; Miscellaneous --------------------------------------------------------------
Msg_Must_Reset                          = "Genstart spillet for, at ændringerne skal tage effekt"
Msg_No_ROM                              = "Ingen ROM var indlæst!"

; Initialization -------------------------------------------------------------
Msg_Init_Allegro                        = "Initialiserer Allegro.."
Msg_Init_GUI                            = "Indlaeser grafisk brugergraenseflade.."
Msg_Init_Completed                      = "[KLAR]"

; Setup ----------------------------------------------------------------------
Msg_Setup_Setup                         = "Opsætning"
Msg_Setup_Soundcard_Select              = "Vaelg venligst dit lydkort:"
Msg_Setup_Soundcard_Select_Tips_Win32   = "(DirectSound er, om muligt, hoejest anbefalelsesvaerdigt)"
Msg_Setup_SampleRate_Select             = "Sample rate:"

; Screenshots Capture --------------------------------------------------------
Msg_Capture_Done                        = "Skærmbillede gemt som %s"
Msg_Capture_Error                       = "Ikke i stand til at gemme skærmbilledet!"
Msg_Capture_Error_File                  = "Ikke i stand til at finde et gyldigt navn til skærmbilledet!"

; On-Board Memory ------------------------------------------------------------
Msg_SRAM_Loaded                         = "Indlæst SaveRAM fra disk (%d Kb)"
Msg_SRAM_Load_Unable                    = "Ikke i stand til at indlæse SaveRAM fra disk"
Msg_SRAM_Wrote                          = "Skrev SaveRAM til disk (%d Kb)"
Msg_SRAM_Write_Unable                   = "Ikke i stand til at skrive SaveRAM til disk (%d Kb)"
Msg_93c46_Reset                         = "Genstarter 93c46"
Msg_93c46_Loaded                        = "Indlæst 93c46 EEPROM data fra disk (%d bytes)"
Msg_93c46_Load_Unable                   = "Ikke i stand til at indlæse 93c46 EEPROM data fra disk"
Msg_93c46_Wrote                         = "Skrev 93c46 EEPROM data til disk (%d bytes)"
Msg_93c46_Write_Unable                  = "Ikke i stand til at skrive 93c46 EEPROM data til disk (%d bytes)"

; TV Type --------------------------------------------------------------------
Msg_TVType_Set                          = "TV-type er nu %s"
Msg_TVType_Info_Speed                   = "(Original hastighed er %d Hz)"

; Blitters -------------------------------------------------------------------
Msg_Blitters_Loading                    = "Indlaeser MEKA.BLT (video modes / blitters)... "
Msg_Blitters_Error_Not_Enough           = "Ikke tilstraekkelige blitters fundet! Check MEKA.BLT eller proev at geninstallere."
Msg_Blitters_Error_Not_Found            = "Blitter ikke fundet!"
Msg_Blitters_Error_Missing              = "I linie %d: Intet blitternavn defineret til lagring!"
Msg_Blitters_Error_Unrecognized         = "I linie %d: Ukendt variabel!"
Msg_Blitters_Set                        = "Benytter nu \"%s\" fullskærms-blitter."

; Nintendo -------------------------------------------------------------------
Msg_NES_Activate                        = "Fed VVS-mands-tilstand aktiveret!"
Msg_NES_Sucks                           = "Fatal fejl: Brugeren bryder sig om Nintendo!"
Msg_NES_Mapper_Unknown                  = "Ikke understøttet mapper: %d. Spillet burde ikke virke."
Msg_NES_Deny_Facts                      = "Du kan ikke benægte fakta."

; Debugger -------------------------------------------------------------------
Msg_Debug_Init                          = "Initialiserer debugger.. "
Msg_Debug_Welcome                       = "Velkommen til Meka Debugger!"
Msg_Debug_Not_Available                 = "Debuggeren findes ikke i denne version!"
Msg_Debug_Brk_Need_Param                = "DEBUGBRK mulighed har brug for en adresse!"
Msg_Debug_Trap_Read                     = "Ved PC:%04X - Læs fra %04X"
Msg_Debug_Trap_Write                    = "Ved PC:%04X - Skriv %02X til %04X"
Msg_Debug_Trap_Port_Read                = "Ved PC:%04X - Port [%02X] læs"
Msg_Debug_Trap_Port_Write               = "Ved PC:%04X - Port [%02X] Skriv %02X"

; Data Dumper ----------------------------------------------------------------
Msg_DataDump_Mode_Ascii                 = "Datadumps vil nu blive skrevet i ASCII mode."
Msg_DataDump_Mode_Raw                   = "Datadumps vil nu blive skrevet i Raw mode."
Msg_DataDump_Error                      = "Fejl ved oprettelse af fil til dump %s!"
Msg_DataDump_Error_OB_Memory            = "Ingen on-board hukommelse at dumpe!"
Msg_DataDump_Error_Palette              = "Ingen palette at dumpe!"
Msg_DataDump_Error_Sprites              = "Ingen sprites at dumpe!"
Msg_DataDump_Main                       = "Dumpet %s (%d %s) i %s mode"

; Documentation --------------------------------------------------------------
Msg_Doc_BoxTitle                        = "Dokumentation"
Msg_Doc_File_Error                      = "Ikke i stand til at åbne dokumentationen!"
Msg_Doc_Enabled                         = "Dokumentationen vises."
Msg_Doc_Disabled                        = "Dokumentationen vises ikke længere."

; Sprite Flickering ----------------------------------------------------------
Msg_Flickering_Auto                     = "Sprite flickering vil blive håndteret automatisk."
Msg_Flickering_Yes                      = "Sprite flickering emulation er nu sat til."
Msg_Flickering_No                       = "Sprite flickering emulation er nu slået fra."

; Layers ---------------------------------------------------------------------
Msg_Layer_BG_Disabled                   = "Baggrundsopdatering slået fra"
Msg_Layer_BG_Enabled                    = "Baggrundsopdatering slået til"
Msg_Layer_Spr_Disabled                  = "Sprites-opdatering slået fra"
Msg_Layer_Spr_Enabled                   = "Sprites-opdatering slået til"

; Floppy Disk (FDC-765) ------------------------------------------------------
Msg_FDC765_Unknown_Read                 = "FDC765: Ukendt disk læsekommando %3i"
Msg_FDC765_Unknown_Write                = "FDC765: Ukendt disk skrivekommando %3i"
Msg_FDC765_Disk_Too_Large1              = "Diskkopi er større end forventet (%i > %i)"
Msg_FDC765_Disk_Too_Large2              = "Yderligere data vil blive ignoreret"
Msg_FDC765_Disk_Too_Small1              = "Diskkopi er mindre end forventet (%i < %i)"
Msg_FDC765_Disk_Too_Small2              = "Manglende data vil blive regnet som tom"

; Terebi Oekaki (Graphic Board) ----------------------------------------------
Msg_TVOekaki_Pen_Touch                  = "Pennen rører grafikpladen"
Msg_TVOekaki_Pen_Away                   = "Pennen rører ikke grafikpladen"

; Palette Viewer -------------------------------------------------------------
Msg_Palette_BoxTitle                    = "Palette"
Msg_Palette_Disabled                    = "Paletteviser slået fra"
Msg_Palette_Enabled                     = "Paletteviser slået til"

; Message box ----------------------------------------------------------------
Msg_Message_BoxTitle                    = "Beskeder"
Msg_Message_Disabled                    = "Beskeder slået fra"
Msg_Message_Enabled                     = "Beskeder slået til"

; Technical Information box --------------------------------------------------
Msg_TechInfo_BoxTitle                   = "Teknisk information"
Msg_TechInfo_Disabled                   = "Teknisk information slået fra"
Msg_TechInfo_Enabled                    = "Teknisk information slået til"

; Tile Viewer ----------------------------------------------------------------
Msg_TilesViewer_BoxTitle                = "Tile-viser"
Msg_TilesViewer_Disabled                = "Tile-viser slået fra"
Msg_TilesViewer_Enabled                 = "Tile-viser slået til"
Msg_TilesViewer_Tile                    = "Tile %d ($%X) @ %s"

; Rapid Fire -----------------------------------------------------------------
Msg_RapidFire_JxBx_On                   = "Rapid Fire er nu aktiveret for Spiller %d Knap %d"
Msg_RapidFire_JxBx_Off                  = "Rapid Fire er nu deaktiveret for Spiller %d Knap %d"

; FM Unit (YM-2413 chipset) --------------------------------------------------
Msg_FM_Enabled                          = "FM-enheds (YM-2413 chipset) emulering slået til"
Msg_FM_Disabled                         = "FM-enheds (YM-2413 chipset) emulering slået fra"

; Country --------------------------------------------------------------------
Msg_Country_European_US                 = "Emuleret maskine er nu Europæisk/Amerikansk"
Msg_Country_JAP                         = "Emuleret maskine er nu Japansk"

; Patching System ------------------------------------------------------------
Msg_Patch_Loading                       = "Indlaeser MEKA.PAT (patcher)... "
;Msg_Patch_Missing                       = "I linie %d: Ingen checksum defineret for instruktion!"
;Msg_Patch_Unrecognized                  = "I linie %d: Ukendt instruktion!"
;Msg_Patch_Out_of_Bound                  = "Under patching af ROM: prøver at skrive uden for grænsen, ved position $%02X. Afbryder patch."

; 3-D Glasses ----------------------------------------------------------------
Msg_Glasses_Enabled                     = "3-D brille emulation slået til"
Msg_Glasses_Disabled                    = "3-D brille emulation slået fra"
Msg_Glasses_Show_Both                   = "3-D effekt uforandret."
Msg_Glasses_Show_Left                   = "3-D effekt slået fra ved kun at vise venstre side."
Msg_Glasses_Show_Right                  = "3-D effekt slået fra ved kun at vise højre side."
Msg_Glasses_Com_Port                    = "3-D brille emulation slået til gennem COM port %d."
;Msg_Glasses_Com_Port2                   = "(editér MEKA.CFG for at ændre COM port)"
Msg_Glasses_Unsupported                 = "Det ser ikke ud til at dette spil understøtter 3-D briller."

; Inputs Initialization ------------------------------------------------------
Msg_Inputs_Joy_Init                     = "Initialiserer joystick.. "
Msg_Inputs_Joy_Init_None                = "Intet fundet."
Msg_Inputs_Joy_Init_Found               = "%d fundet."
Msg_Inputs_Joy_Calibrate_Error          = "Fejl under kalibrering af joystick. Afbryder."

; Inputs ---------------------------------------------------------------------
Msg_Inputs_Joypad                       = "Tilbehør: Joypad"
Msg_Inputs_LightPhaser                  = "Tilbehør: Light Phaser"
Msg_Inputs_PaddleControl                = "Tilbehør: Paddle Control"
Msg_Inputs_SportsPad                    = "Tilbehør: Sports Pad"
Msg_Inputs_TVOekaki                     = "Tilbehør: TV Oekaki Graphic Board"
Msg_Inputs_Play_Digital                 = "Kontrol med digital styreenhed"
Msg_Inputs_Play_Mouse                   = "Kontrol med analog styreenhed (mus)"
Msg_Inputs_Play_Pen                     = "Kontrollér pennen med musen\nHøjre-klik simulerer, at pennen løftes"

; Inputs Configuration -------------------------------------------------------
Msg_Inputs_Config_BoxTitle              = "Input Konfiguration"

; Inputs Sources -------------------------------------------------------------
Msg_Inputs_Src_Loading                  = "Indlaeser MEKA.INP (definering af styreenheder)... "
Msg_Inputs_Src_Not_Enough               = "Ikke tilstraekkeligt input af styreenheder! Check MEKA.INP eller proev at geninstallere meka."
Msg_Inputs_Src_Missing                  = "I linie %d: Ingen styreenhed defineret til lagring af vaerdi!"
Msg_Inputs_Src_Equal                    = "I linie %d: Lighedstegn savnes til vaerditildeling af variabel!"
Msg_Inputs_Src_Unrecognized             = "I linie %d: Ukendt variabel \"%s\"!"
Msg_Inputs_Src_Syntax_Param             = "I linie %d: Syntax-fejl i variabel parameter!"
Msg_Inputs_Src_Inconsistency            = "I linie %d: Uforenlighed med tidligere indstillinger!"
Msg_Inputs_Src_Map_Keyboard             = "Tryk på den tast, du vil tildele denne handling til"
Msg_Inputs_Src_Map_Keyboard_Ok          = "Handling tildelt til tast \"%s\"."
Msg_Inputs_Src_Map_Joypad               = "Tryk på den tast/akse, du vil tildele denne handling til"
Msg_Inputs_Src_Map_Joypad_Ok_A          = "Handling tildelt til joypad stik %i, akse %i, %c."
Msg_Inputs_Src_Map_Joypad_Ok_B          = "Handling tildelt til joypad knap %i."
Msg_Inputs_Src_Map_Mouse                = "Tryk på den knap, du vil tildele denne handling til"
Msg_Inputs_Src_Map_Mouse_Ok_B           = "Handling tildelt til mus-knap %i."
Msg_Inputs_Src_Map_Mouse_No_A           = "Desværre - mus-akserne kan ikke ændres"
Msg_Inputs_Src_Map_Cancelled            = "Tildeling afbrudt!"

; Machine --------------------------------------------------------------------
Msg_Machine_Pause                       = "Maskineemulering pauset"
Msg_Machine_Resume                      = "Maskineemulering fortsat"
Msg_Machine_Reset                       = "Maskine genstartet"

; Filenames Database ---------------------------------------------------------
;Msg_VFN_Loading                         = "Indlaeser MEKA.VFN (virtuelle filnavne)... "

; DataBase / Names -----------------------------------------------------------
Msg_DB_Loading                          = "Indlaeser MEKA.NAM (spiltitler)... "
Msg_DB_Name_Default                     = "Spilskærm"
Msg_DB_Name_NoCartridge                 = "Ingen spilkasette"

; Configuration File ---------------------------------------------------------
Msg_Config_Loading                      = "Indlaeser %s (konfiguration)... "

; Datafile -------------------------------------------------------------------
Msg_Datafile_Loading                    = "Indlaeser MEKA.DAT (resourcer)... "

; Drivers --------------------------------------------------------------------
Msg_Driver_Unknown                      = "Fatalt: Ukendt maskinedriver!"

; Overdump detection ---------------------------------------------------------
;Msg_OverDump                            = "Denne ROM indeholder %d gange den nødvendige data.\nDownload venligst \"SMS Checker\" for, at checke den (http://www.smspower.org/maxim/smschecker/)."

; Sound ----------------------------------------------------------------------
Msg_Sound_Init                          = "Initialiserer lydsystem.."
Msg_Sound_Init_Error_Audio              = " - Fejl under aabning af lydsystem. Proev et andet kort og/eller en anden sample rate."
Msg_Sound_Init_Error_Blaster            = " - BLASTER variabel ikke fundet, slaar FM lyd fra!"
Msg_Sound_Init_Error_Blaster_A          = " - BLASTER variabel indeholder ikke adresse (A=xxx), slaar FM lyd fra!"
Msg_Sound_Init_Error_Voices             = " - Voices initialisation fejlede."
Msg_Sound_Init_Error_Voice_N            = " - Voice #%d dannelse fejlede."
Msg_Sound_Init_Soundcard                = " - Initialiserer lydkort ved %d Hz"
Msg_Sound_Init_Soundcard_No             = "Kan ikke initialisere lyd: intet lydkort valgt!"
Msg_Sound_Init_SN76496                  = " - SN76496 digital emulator:"
Msg_Sound_Init_YM2413_Digital           = " - YM2413 digital emulator:"
Msg_Sound_Stream_Error                  = "Lyd buffer stop. Genstarter datastrømmen."
Msg_Sound_Volume_Changed                = "Volumen ændret til %d%%."

; Theme ----------------------------------------------------------------------
Msg_Theme_Loading                       = "Indlaeser MEKA.THM (brugergraenseflade temaer)... "
Msg_Theme_Error_Not_Enough              = "Ikke tilstraekkelige temaer fundet! Check MEKA.THM eller proev at geninstallere."
Msg_Theme_Error_Missing_Theme_Name      = "I linie %d: Intet tema defineret til lagring af vaerdi!"
Msg_Theme_Error_BG_Big                  = "Baggrundsbilledet er for småt til at passe med heltalsskalering!"
Msg_Theme_Error_BG                      = "Fejl under indlæsning af billede associeret med temaet!"
Msg_Theme_Error_BG_FileName             = "(%s)"

; Loading ROM / Disk ---------------------------------------------------------
Msg_LoadROM_Loading                     = "Indlæser ROM... "
Msg_LoadROM_Success                     = "Indlæste %s"
Msg_LoadDisk_Success                    = "Indlæste %s som en diskette"
Msg_LoadROM_Comment                     = "Kommentar: %s"
Msg_LoadROM_Warning                     = "-Advarsel-"
Msg_LoadROM_Bad_Dump_Long               = "Denne ROM er kendt som et dårligt dump.\nDen fungerer muligvis ikke ordentligt. Download venligst SMS Checker for, at checke den (http://www.smspower.org/maxim/smschecker/)."
Msg_LoadROM_Bad_Dump_Short              = "Advarsel - Denne ROM er kendt som et dårligt dump."
Msg_LoadROM_Product_Num                 = "Produkt Kode: %s"
Msg_LoadROM_SDSC                        = "-SDSC Header-"
Msg_LoadROM_SDSC_Name                   = "Navn: %s"
Msg_LoadROM_SDSC_Version                = "Version: %d.%02d"
Msg_LoadROM_SDSC_Date                   = "Dato: %04d.%02d.%02d"
Msg_LoadROM_SDSC_Author                 = "Udvikler: %s"
Msg_LoadROM_SDSC_Release_Note           = "Udgivelses Note: %s"
Msg_LoadROM_SDSC_Unknown                = "<Ukendt>"
Msg_LoadROM_SDSC_Error                  = "<Fejl>"

; File Browser ---------------------------------------------------------------
Msg_FileBrowser_BoxTitle                = "Indlæs ROM"
Msg_FileBrowser_Drive                   = "[Drev %c]"
Msg_FileBrowser_Load                    = "Indlæs"
Msg_FileBrowser_Close                   = "Luk"
Msg_FileBrowser_LoadNames               = "Indlæs Navne"
Msg_FileBrowser_ReloadDir               = "Opdatér"

; FM Editor ------------------------------------------------------------------
Msg_FM_Editor_BoxTitle                  = "FM Instrument Editor"
Msg_FM_Editor_Enabled                   = "FM Instrument Editor slået til"
Msg_FM_Editor_Disabled                  = "FM Instrument Editor slået fra"

; Video Options --------------------------------------------------------------
Msg_Frameskip_Auto                      = "Frameskip værdi sat til: Automatisk (%d Hz)"
Msg_Frameskip_Standard                  = "Frameskip værdi sat til: 1/%d"
Msg_FPS_Counter_Enabled                 = "FPS tæller slået til"
Msg_FPS_Counter_Disabled                = "FPS tæller slået fra"

; Logging --------------------------------------------------------------------
Msg_Log_Need_Param                      = "LOG-mulighed behoever et filnavn!"
Msg_Log_Session_Start                   = "-- Logning startet %s"

; Save States ----------------------------------------------------------------
Msg_Load_Need_Param                     = "LOAD-muligheden har brug for et tilstandsnummer!"
Msg_Load_Error                          = "Fejl under åbning af %s!"
Msg_Load_Not_Valid                      = "Filen %s er ikke en gyldig %s spiltilstand!"
Msg_Load_Success                        = "Tilstand indlæst fra %s"
Msg_Load_Version                        = "%s er ikke en understøttet spiltilstand version!"
Msg_Load_Wrong_System                   = "%s er ikke en spiltilstand for dette system!"
Msg_Load_Massage                        = "Filen er i MSD format - konverterer"
Msg_Save_Not_in_BIOS                    = "Kan ikke gemme under kørsel af BIOS!"
Msg_Save_Error                          = "Fejl under skrivning til %s!"
Msg_Save_Success                        = "Tilstand gemt som %s"
Msg_Save_Slot                           = "Nuværende spiltilstands-nummer er: %d"

; Options --------------------------------------------------------------------
Msg_Options_BoxTitle                    = "Indstilninger"
Msg_Options_Close                       = "Luk"
;Msg_Options_BIOS_Enable                 = "Vis internt BIOS logo hvis tilgængeligt i systemet."
;Msg_Options_Product_Number              = "Vis spillets produkt kode hvis tilgængeligt."
;Msg_Options_Bright_Palette              = "Lys palette for Master System / Game Gear."
Msg_Options_Load_Close                  = "Luk filbrowseren efter åbning af et spil."
Msg_Options_Load_FullScreen             = "Skift til fuld skærm efter åbning af et spil."
;Msg_Options_VFN_Support                 = "Vis lange filnavne og landeflag."
Msg_Options_FullScreen_Messages         = "Vis beskeder i fuldskærms mode."
Msg_Options_GUI_VSync                   = "Vent på VSync i interface mode."
Msg_Options_NES_Enable                  = "Mario er ikke en fed VVS-mand."

; Languages / Localization ---------------------------------------------------
Msg_Language_Set                        = "Sprog sat til %s"
Msg_Language_Set_Warning                = "NB: det er anbefalet at du genstarter MEKA for at være sikker på at al tekst opdateres."

; Sound Dumping --------------------------------------------------------------
Msg_Sound_Dumping_Start                 = "Startede dumpning af lyd til %s"
Msg_Sound_Dumping_Stop                  = "Stoppede dumpning af lyd (%2.02f sekunder logget)"
Msg_Sound_Dumping_Error_File_1          = "Kan ikke finde et gyldigt filnavn til lyd-dump!"
Msg_Sound_Dumping_Error_File_2          = "Fejl under åbning af skrive-filen %s for dumpning!"
Msg_Sound_Dumping_VGM_Acc_Frame         = "VGM dumps vil nu være frame-eksakte (mindre filer, undgå voices)"
Msg_Sound_Dumping_VGM_Acc_Sample        = "VGM dumps vil nu være sample-eksakte (større filer, dump voices)"
Msg_Sound_Dumping_VGM_Acc_Change        = "(VGM dumpning bliver nødt til at blive genstartet for at tage hensyn til den nye kvalitet)"

; Menu: Main -----------------------------------------------------------------
Msg_Menu_Main                           = "FILER"
Msg_Menu_Main_LoadROM                   = "Indlæs ROM"
Msg_Menu_Main_FreeROM                   = "Frigør ROM"
Msg_Menu_Main_SaveState                 = "Gem Savestate"
Msg_Menu_Main_LoadState                 = "Hent Savestate"
Msg_Menu_Main_Options                   = "Indstilninger"
Msg_Menu_Main_Language                  = "Sprog"
Msg_Menu_Main_Quit                      = "Afslut"

; Menu: Debug ----------------------------------------------------------------
Msg_Menu_Debug                          = "DEBUG"
Msg_Menu_Debug_Dump                     = "Dump"

; Menu: Machine --------------------------------------------------------------
Msg_Menu_Machine                        = "MASKINE"
Msg_Menu_Machine_Power                  = "Strøm"
Msg_Menu_Machine_Power_On               = "Tænd"
Msg_Menu_Machine_Power_Off              = "Sluk"
Msg_Menu_Machine_Country                = "Land"
Msg_Menu_Machine_Country_EU             = "Europæisk/Amerikansk"
Msg_Menu_Machine_Country_Jap            = "Japansk"
Msg_Menu_Machine_TVType                 = "TV Type"
Msg_Menu_Machine_TVType_NTSC            = "NTSC"
Msg_Menu_Machine_TVType_PALSECAM        = "PAL/SECAM"
Msg_Menu_Machine_HardPause              = "Hardware Pause"
Msg_Menu_Machine_HardReset              = "Hardware Genstart"

; Menu: Video ----------------------------------------------------------------
Msg_Menu_Video                          = "VIDEO"
Msg_Menu_Video_FullScreen               = "Fuld skærm"
Msg_Menu_Video_Themes                   = "Temaer"
Msg_Menu_Video_Blitters                 = "Blitters"
Msg_Menu_Video_Layers                   = "Lag"
Msg_Menu_Video_Layers_Sprites           = "Sprites"
Msg_Menu_Video_Layers_Background        = "Baggrund"
Msg_Menu_Video_Flickering               = "Flickering"
Msg_Menu_Video_Flickering_Auto          = "Auto"
Msg_Menu_Video_Flickering_Yes           = "Ja"
Msg_Menu_Video_Flickering_No            = "Nej"
Msg_Menu_Video_3DGlasses                = "3-D Briller"
Msg_Menu_Video_3DGlasses_Enabled        = "Slået til"
Msg_Menu_Video_3DGlasses_ShowBothSides  = "Vis begge sider"
Msg_Menu_Video_3DGlasses_ShowLeftSide   = "Vis venstre side"
Msg_Menu_Video_3DGlasses_ShowRightSide  = "Vis højre side"
Msg_Menu_Video_3DGlasses_UsesCOMPort    = "Benytter briller via COM Port"

; Menu: Sound ----------------------------------------------------------------
Msg_Menu_Sound                          = "LYD"
Msg_Menu_Sound_FM                       = "FM Enhed"
Msg_Menu_Sound_FM_Enabled               = "Slået til"
Msg_Menu_Sound_FM_Disabled              = "Slået fra"
Msg_Menu_Sound_FM_Editor                = "Instrument Editor"
Msg_Menu_Sound_Volume                   = "Volume"
Msg_Menu_Sound_Volume_Mute              = "Mute"
Msg_Menu_Sound_Volume_Value             = "%d%%"
Msg_Menu_Sound_Rate                     = "Rate"
Msg_Menu_Sound_Rate_Hz                  = "%d Hz"
Msg_Menu_Sound_Channels                 = "Kanaler"
Msg_Menu_Sound_Channels_Tone            = "Tone %d"
Msg_Menu_Sound_Channels_Noises          = "Noises"
Msg_Menu_Sound_Dump                     = "Dump"
Msg_Menu_Sound_Dump_WAV_Start           = "WAV Start"
Msg_Menu_Sound_Dump_WAV_Stop            = "WAV Stop"
Msg_Menu_Sound_Dump_VGM_Start           = "VGM Start"
Msg_Menu_Sound_Dump_VGM_Stop            = "VGM Stop"
Msg_Menu_Sound_Dump_VGM_SampleAccurate  = "VGM Sample-Eksakt"

; Menu: Inputs ---------------------------------------------------------------
Msg_Menu_Inputs                         = "STYREENHEDER"
Msg_Menu_Inputs_Joypad                  = "Joypad"
Msg_Menu_Inputs_LightPhaser             = "Light Phaser"
Msg_Menu_Inputs_PaddleControl           = "Paddle Control"
Msg_Menu_Inputs_SportsPad               = "Sports Pad"
Msg_Menu_Inputs_GraphicBoard            = "Graphic Board"
Msg_Menu_Inputs_SK1100                  = "Sega Keyboard"
Msg_Menu_Inputs_RapidFire               = "Rapid Fire"
Msg_Menu_Inputs_RapidFire_PxBx          = "Spiller %d Knap %d"
Msg_Menu_Inputs_Configuration           = "Konfiguration"

; Menu: Tools ----------------------------------------------------------------
Msg_Menu_Tools                          = "VÆRKTØJER"
Msg_Menu_Tools_Messages                 = "Beskeder"
Msg_Menu_Tools_Palette                  = "Palette"
Msg_Menu_Tools_TilesViewer              = "Tiles"
Msg_Menu_Tools_TechInfo                 = "Teknisk Info"

; Menu: Help -----------------------------------------------------------------
Msg_Menu_Help                           = "HJÆLP"
Msg_Menu_Help_Documentation             = "Dokumentation"
Msg_Menu_Help_Documentation_W           = "Dokumentation (Windows)"
Msg_Menu_Help_Documentation_U           = "Dokumentation (Unix)"
Msg_Menu_Help_Compat                    = "Kompatibilitetsliste"
Msg_Menu_Help_Multiplayer_Games         = "Multiplayer-spilsliste"
Msg_Menu_Help_Changes                   = "Ændringer"
Msg_Menu_Help_About                     = "Om"

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; [Test Accents]
; WIP
; Msg_Welcome                           = "Test accent: eéèêë aàâä iîï uù ç"
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
