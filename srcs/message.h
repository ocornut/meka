//-----------------------------------------------------------------------------
// MEKA - message.h
// Messaging System, Languages, Console - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Messages types
#define MSGT_USER_BOX                   (0x01)
#define MSGT_USER_INFOLINE              (0x02)
#define MSGT_ATTR_DEBUG                 (0x04)

#define MSGT_USER                       (MSGT_USER_BOX | MSGT_USER_INFOLINE)
#define MSGT_DEBUG                      (MSGT_USER_BOX | MSGT_ATTR_DEBUG)

//-----------------------------------------------------------------------------

// String used to enable WIP for a language
#define MSG_LANG_WIP_STR                "WIP"

// Messages definitions
#define MSG_NULL                                (0)
#define MSG_Quit                                (1)
#define MSG_About_Line_Meka_Date                (2)
#define MSG_About_Line_Authors                  (3)
#define MSG_About_Line_Homepage                 (4)
#define MSG_Capture_Done                        (5)
#define MSG_Capture_Error_File                  (6)
#define MSG_SRAM_Loaded                         (7)
#define MSG_SRAM_Load_Unable                    (8)
#define MSG_SRAM_Wrote                          (9)
#define MSG_SRAM_Write_Unable                   (10)
#define MSG_93c46_Reset                         (11)
#define MSG_93c46_Loaded                        (12)
#define MSG_93c46_Load_Unable                   (13)
#define MSG_93c46_Wrote                         (14)
#define MSG_93c46_Write_Unable                  (15)
#define MSG_TVType_Set                          (16)
#define MSG_TVType_Info_Speed                   (17)
#define MSG_Blitters_Loading                    (18)
#define MSG_Blitters_Not_Enough                 (19)
#define MSG_Blitters_Not_Found                  (20)
#define MSG_Blitters_Missing                    (21)
#define MSG_Blitters_Unrecognized               (22)
#define MSG_Blitters_Set                        (23)
#define MSG_NES_Activate                        (24)
#define MSG_NES_Sucks                           (25)
#define MSG_NES_Mapper_Unknown                  (26)
#define MSG_NES_Deny_Facts                      (27)
#define MSG_Debug_Trap_Read                     (28)
#define MSG_Debug_Trap_Write                    (29)
#define MSG_Debug_Trap_Port_Read                (30)
#define MSG_Debug_Trap_Port_Write               (31)
#define MSG_DataDump_Mode_Ascii                 (32)
#define MSG_DataDump_Mode_Raw                   (33)
#define MSG_DataDump_Error                      (34)
#define MSG_DataDump_Error_OB_Memory            (35)
#define MSG_DataDump_Error_Palette              (36)
#define MSG_DataDump_Error_Sprites              (37)
#define MSG_DataDump_Main                       (38)
#define MSG_Doc_File_Error                      (39)
#define MSG_Doc_Enabled                         (40)
#define MSG_Doc_Disabled                        (41)
#define MSG_Flickering_Auto                     (42)
#define MSG_Flickering_Yes                      (43)
#define MSG_Flickering_No                       (44)
#define MSG_Layer_BG_Disabled                   (45)
#define MSG_Layer_BG_Enabled                    (46)
#define MSG_Layer_Spr_Disabled                  (47)
#define MSG_Layer_Spr_Enabled                   (48)
#define MSG_FDC765_Unknown_Read                 (49)
#define MSG_FDC765_Unknown_Write                (50)
#define MSG_FDC765_Disk_Too_Large1              (51)
#define MSG_FDC765_Disk_Too_Large2              (52)
#define MSG_TVOekaki_Pen_Touch                  (53)
#define MSG_TVOekaki_Pen_Away                   (54)
#define MSG_Palette_Disabled                    (55)
#define MSG_Palette_Enabled                     (56)
#define MSG_Message_Disabled                    (57)
#define MSG_Message_Enabled                     (58)
#define MSG_TechInfo_Disabled                   (59)
#define MSG_TechInfo_Enabled                    (60)
#define MSG_TilesViewer_Disabled                (61)
#define MSG_TilesViewer_Enabled                 (62)
#define MSG_RapidFire_JxBx_On                   (63)
#define MSG_RapidFire_JxBx_Off                  (64)
#define MSG_FM_Enabled                          (65)
#define MSG_FM_Disabled                         (66)
#define MSG_Country_European_US                 (67)
#define MSG_Country_JAP                         (68)
#define MSG_Must_Reset                          (69)
#define MSG_Error_File_Not_Found                (70)
#define MSG_Error_File_Read                     (71)
#define MSG_Error_File_Empty                    (72)
#define MSG_Error_ZIP_Not_Supported             (73)
#define MSG_Error_ZIP_Loading                   (74)
#define MSG_Error_ZIP_Internal                  (75)
#define MSG_Error_Base                          (76)
#define MSG_Error_Error                         (77)
#define MSG_Error_Memory                        (78)
#define MSG_Error_Param                         (79)
#define MSG_Error_Syntax                        (80)
#define MSG_Error_Video_Mode                    (81)
#define MSG_Patch_Value_Not_a_Byte              (82)
#define MSG_Patch_Missing                       (83)
#define MSG_Patch_Unrecognized                  (84)
#define MSG_Patch_Out_of_Bound                  (85)
#define MSG_Glasses_Enabled                     (86)
#define MSG_Glasses_Disabled                    (87)
#define MSG_Glasses_Show_Both                   (88)
#define MSG_Glasses_Show_Left                   (89)
#define MSG_Glasses_Show_Right                  (90)
#define MSG_Glasses_Com_Port                    (91)
#define MSG_Glasses_Com_Port2                   (92)
#define MSG_Glasses_Unsupported                 (93)
#define MSG_Inputs_Src_Loading                  (94)
#define MSG_Inputs_Src_Not_Enough               (95)
#define MSG_Inputs_Src_Missing                  (96)
#define MSG_Inputs_Src_Equal                    (97)
#define MSG_Inputs_Src_Unrecognized             (98)
#define MSG_Inputs_Src_Syntax_Param             (99)
#define MSG_Inputs_Src_Inconsistency            (100)
#define MSG_Inputs_Src_Map_Keyboard             (101)
#define MSG_Inputs_Src_Map_Keyboard_Ok          (102)
#define MSG_Inputs_Src_Map_Joypad               (103)
#define MSG_Inputs_Src_Map_Joypad_Ok_A          (104)
#define MSG_Inputs_Src_Map_Joypad_Ok_B          (105)
#define MSG_Inputs_Src_Map_Mouse                (106)
#define MSG_Inputs_Src_Map_Mouse_Ok_B           (107)
#define MSG_Inputs_Src_Map_Mouse_No_A           (108)
#define MSG_Inputs_Src_Map_Cancelled            (109)
#define MSG_Machine_Pause                       (110)
#define MSG_Machine_Resume                      (111)
#define MSG_Machine_Reset                       (112)   // Unused
#define MSG_Inputs_Joy_Init                     (113)
#define MSG_Inputs_Joy_Init_None                (114)
#define MSG_Inputs_Joy_Init_Found               (115)
#define MSG_Inputs_Joy_Calibrate_Error          (116)
#define MSG_FDB_Loading                         (117)
#define MSG_Welcome                             (118)
#define MSG_Window_Title                        (119)
#define MSG_Patch_Loading                       (120)
#define MSG_DB_Loading                          (121)
#define MSG_DB_Name_Default                     (122)
#define MSG_DB_Name_NoCartridge                 (123)
#define MSG_Config_Loading                      (124)
#define MSG_Datafile_Loading                    (125)
#define MSG_Failed                              (126)
#define MSG_Driver_Unknown                      (127)
#define MSG_OverDump                            (128)
#define MSG_Debug_Init                          (129)
#define MSG_Debug_Welcome                       (130)
#define MSG_Debug_Not_Available                 (131)
#define MSG_Debug_Brk_Need_Param                (132)
//#define MSG_Debug_Brk_Enable                  (133)
#define MSG_Menu_Debug_Enabled                  (133)
#define MSG_Sound_Init                          (134)
#define MSG_Sound_Init_Error_SEAL               (135)
#define MSG_Sound_Init_Error_Audio              (136)
#define MSG_Sound_Init_Error_Blaster            (137)
#define MSG_Sound_Init_Error_Blaster_A          (138)
#define MSG_Sound_Init_Error_Voices             (139)
#define MSG_Sound_Init_Error_Voice_N            (140)
#define MSG_Sound_Init_Soundcard                (141)
#define MSG_Setup_Soundcard_Select              (142)
#define MSG_Setup_Soundcard_Select_Tips_DOS     (143)
#define MSG_Setup_Soundcard_Select_Tips_Win32   (144)
#define MSG_Sound_Init_Soundcard_No             (145)
#define MSG_Sound_Init_SN76496                  (146)
#define MSG_Sound_Init_YM2413_OPL               (147)
#define MSG_Sound_Stream_Error                  (148)
#define MSG_Sound_Rate_Changed                  (149)
#define MSG_Theme_Loading                       (150)
#define MSG_Theme_Not_Enough                    (151)
#define MSG_Theme_Missing                       (152)
#define MSG_Theme_Unrecognized                  (153)
#define MSG_Theme_Error_BG_Big                  (154)
#define MSG_Theme_Error_BG                      (155)
#define MSG_Theme_Error_BG_FileName             (156)
#define MSG_LoadROM_Loading                     (157)
#define MSG_LoadROM_Success                     (158)
#define MSG_LoadDisk_Success                    (159)
#define MSG_LoadROM_Comment                     (160)
#define MSG_LoadROM_Warning                     (161)
#define MSG_LoadROM_Bad_Dump_Long               (162)
#define MSG_LoadROM_Bad_Dump_Short              (163)
#define MSG_LoadROM_Product_Num                 (164)
#define MSG_FM_Editor_Enabled                   (165)
#define MSG_FM_Editor_Disabled                  (166)
#define MSG_Frameskip_Auto                      (167)
#define MSG_Frameskip_Standard                  (168)
#define MSG_Log_Need_Param                      (169)
#define MSG_Inputs_Joypad                       (170)
#define MSG_Inputs_LightPhaser                  (171)
#define MSG_Inputs_PaddleControl                (172)
#define MSG_Inputs_SportsPad                    (173)
#define MSG_Inputs_TVOekaki                     (174)
#define MSG_Inputs_Play_Digital                 (175)
#define MSG_Inputs_Play_Mouse                   (176)
#define MSG_Inputs_Play_Pen                     (177)
#define MSG_No_ROM                              (178)
#define MSG_Init_Allegro                        (179)
#define MSG_Init_GUI                            (180)
#define MSG_Init_Completed                      (181)
#define MSG_Load_Error                          (182)
#define MSG_Load_Not_Valid                      (183)
#define MSG_Load_Success                        (184)
#define MSG_Load_Version                        (185)
#define MSG_Load_Wrong_System                   (186)
#define MSG_Load_Massage                        (187)
#define MSG_Save_Error                          (188)
#define MSG_Save_Success                        (189)
#define MSG_Save_Slot                           (190)
#define MSG_Error_Directory_Open                (191)
#define MSG_Log_Session_Start                   (192)
#define MSG_Options_BIOS_Enable                 (193)
#define MSG_Options_Product_Number              (194)
#define MSG_Options_Bright_Palette              (195)
#define MSG_Options_Load_Close                  (196)
#define MSG_Options_Load_FullScreen             (197)
#define MSG_Options_DB_Display                  (198)
#define MSG_Options_FullScreen_Messages         (199)
#define MSG_Options_GUI_VSync                   (200)
#define MSG_Options_NES_Activated               (201)
#define MSG_Language_Set                        (202)
#define MSG_Language_Set_Warning                (203)
#define MSG_Sound_Dumping_Start                 (204)
#define MSG_Sound_Dumping_Stop                  (205)
#define MSG_Sound_Dumping_Error_File_1          (206)
#define MSG_Sound_Dumping_Error_File_2          (207)
#define MSG_Load_Need_Param                     (208)
#define MSG_LoadROM_SDSC                        (209)
#define MSG_LoadROM_SDSC_Name                   (210)
#define MSG_LoadROM_SDSC_Version                (211)
#define MSG_LoadROM_SDSC_Date                   (212)
#define MSG_LoadROM_SDSC_Release_Note           (213)
#define MSG_LoadROM_SDSC_Unknown                (214)
#define MSG_LoadROM_SDSC_Error                  (215)
#define MSG_Save_Not_in_BIOS                    (216)
#define MSG_Sound_Dumping_VGM_Acc_Frame         (217)
#define MSG_Sound_Dumping_VGM_Acc_Sample        (218)
#define MSG_Sound_Dumping_VGM_Acc_Change        (219)
// 0.61
#define MSG_FDC765_Disk_Too_Small1              (220)
#define MSG_FDC765_Disk_Too_Small2              (221)
#define MSG_LoadROM_SDSC_Author                 (222)
#define MSG_VoiceRecognition_BoxTitle           (223)
#define MSG_VoiceRecognition_Enabled            (224)
#define MSG_VoiceRecognition_Disabled           (225)
#define MSG_TilesViewer_BoxTitle                (226)
#define MSG_TilesViewer_Tile                    (227)
#define MSG_Options_BoxTitle                    (228)
#define MSG_Options_Close                       (229)
#define MSG_FileBrowser_BoxTitle                (230)
#define MSG_FileBrowser_Drive                   (231)
#define MSG_FileBrowser_Load                    (232)
#define MSG_FileBrowser_Close                   (233)
#define MSG_FileBrowser_LoadNames               (234)
#define MSG_FileBrowser_ReloadDir               (235)
#define MSG_Inputs_Configuration_BoxTitle       (236)
#define MSG_Message_BoxTitle                    (237)
#define MSG_TechInfo_BoxTitle                   (238)
#define MSG_Doc_BoxTitle                        (239)
#define MSG_About_BoxTitle                      (240)
#define MSG_Palette_BoxTitle                    (241)
#define MSG_Menu_Main                           (242)
#define MSG_Menu_Main_LoadROM                   (243)
#define MSG_Menu_Main_FreeROM                   (244)
#define MSG_Menu_Main_SaveState                 (245)
#define MSG_Menu_Main_LoadState                 (246)
#define MSG_Menu_Main_Options                   (247)
#define MSG_Menu_Main_Language                  (248)
#define MSG_Menu_Main_Quit                      (249)
#define MSG_Menu_Debug                          (250)
#define MSG_Menu_Debug_Dump                     (251)
#define MSG_Menu_Debug_Watch                    (252)
#define MSG_Menu_Machine                        (253)
#define MSG_Menu_Machine_Power                  (254)
#define MSG_Menu_Machine_Power_On               (255)
#define MSG_Menu_Machine_Power_Off              (256)
#define MSG_Menu_Machine_Country                (257)
#define MSG_Menu_Machine_Country_EU             (258)
#define MSG_Menu_Machine_Country_Jap            (259)
#define MSG_Menu_Machine_TVType                 (260)
#define MSG_Menu_Machine_TVType_NTSC            (261)
#define MSG_Menu_Machine_TVType_PALSECAM        (262)
#define MSG_Menu_Machine_HardPause              (263)
#define MSG_Menu_Machine_HardReset              (264)
#define MSG_Menu_Video                          (265)
#define MSG_Menu_Video_FullScreen               (266)
#define MSG_Menu_Video_Themes                   (267)
#define MSG_Menu_Video_Blitters                 (268)
#define MSG_Menu_Video_Layers                   (269)
#define MSG_Menu_Video_Layers_Sprites           (270)
#define MSG_Menu_Video_Layers_Background        (271)
#define MSG_Menu_Video_Flickering               (272)
#define MSG_Menu_Video_Flickering_Auto          (273)
#define MSG_Menu_Video_Flickering_Yes           (274)
#define MSG_Menu_Video_Flickering_No            (275)
#define MSG_Menu_Video_3DGlasses                (276)
#define MSG_Menu_Video_3DGlasses_Enabled        (277)
#define MSG_Menu_Video_3DGlasses_ShowBothSides  (278)
#define MSG_Menu_Video_3DGlasses_ShowLeftSide   (279)
#define MSG_Menu_Video_3DGlasses_ShowRightSide  (280)
#define MSG_Menu_Video_3DGlasses_UsesCOMPort    (281)
#define MSG_Menu_Video_Screens                  (282)
#define MSG_Menu_Video_Screens_New              (283)
#define MSG_Menu_Video_Screens_KillLast         (284)
#define MSG_Menu_Video_Screens_KillAll          (285)
#define MSG_Menu_Sound                          (286)
#define MSG_Menu_Sound_FM                       (287)
#define MSG_Menu_Sound_FM_Enabled               (288)
#define MSG_Menu_Sound_FM_Disabled              (289)
#define MSG_Menu_Sound_FM_Editor                (290)
#define MSG_Menu_Sound_Volume                   (291)
#define MSG_Menu_Sound_Volume_Mute              (292)
#define MSG_Menu_Sound_Volume_Value             (293)
#define MSG_Menu_Sound_Rate                     (294)
#define MSG_Menu_Sound_Rate_Hz                  (295)
#define MSG_Menu_Sound_Channels                 (296)
#define MSG_Menu_Sound_Channels_Tone            (297)
#define MSG_Menu_Sound_Channels_Noises          (298)
#define MSG_Menu_Sound_Dump                     (299)
#define MSG_Menu_Sound_Dump_WAV_Start           (300)
#define MSG_Menu_Sound_Dump_WAV_Stop            (301)
#define MSG_Menu_Sound_Dump_VGM_Start           (302)
#define MSG_Menu_Sound_Dump_VGM_Stop            (303)
#define MSG_Menu_Sound_Dump_VGM_SampleAccurate  (304)
#define MSG_Menu_Sound_VoiceRecognition         (305)
#define MSG_Menu_Inputs                         (306)
#define MSG_Menu_Inputs_Joypad                  (307)
#define MSG_Menu_Inputs_LightPhaser             (308)
#define MSG_Menu_Inputs_PaddleControl           (309)
#define MSG_Menu_Inputs_SportsPad               (310)
#define MSG_Menu_Inputs_GraphicBoard            (311)
#define MSG_Menu_Inputs_SK1100                  (312)
#define MSG_Menu_Inputs_RapidFire               (313)
#define MSG_Menu_Inputs_RapidFire_PxBx          (314)
#define MSG_Menu_Inputs_Configuration           (315)
#define MSG_Menu_Tools                          (316)
#define MSG_Menu_Tools_Messages                 (317)
#define MSG_Menu_Tools_Palette                  (318)
#define MSG_Menu_Tools_TilesViewer              (319)
#define MSG_Menu_Tools_TechInfo                 (320)
#define MSG_Menu_Help                           (321)
#define MSG_Menu_Help_Documentation             (322)
#define MSG_Menu_Help_About                     (323)
#define MSG_FM_Editor_BoxTitle                  (324)
// 0.62
#define MSG_Capture_Error                       (325)
// 0.62c
#define MSG_Menu_Help_Documentation_W           (326)
#define MSG_Menu_Help_Compat                    (327)
#define MSG_Menu_Help_Changes                   (328)
// 0.63
#define MSG_FPS_Counter_Enabled                 (329)
#define MSG_FPS_Counter_Disabled                (330)
#define MSG_Menu_Help_Multiplayer_Games         (331)
// 0.63c
#define MSG_Menu_Sound_FM_Emulator              (332)
#define MSG_Menu_Sound_FM_Emulator_OPL          (333)
#define MSG_Menu_Sound_FM_Emulator_Digital      (334)
#define MSG_Sound_Init_YM2413_Digital           (335)
#define MSG_Ok                                  (336)
#define MSG_Sound_Volume_Changed                (337)
// 0.64c
#define MSG_Error_Video_Mode_Back_To_GUI        (338)
// 0.65
#define MSG_Menu_Help_Documentation_U           (339)
// 0.65b
#define MSG_Setup_SampleRate_Select             (340)
#define MSG_Setup_Setup                         (341)
// 0.66
#define MSG_Glasses_Com_Port_Open_Error         (342)
// 0.67
#define MSG_Inputs_Play_Digital_Unrecommended   (343)
// 0.68
#define MSG_DB_SyntaxError                      (344)
#define MSG_LoadROM_SMSGG_Mode_Comment          (345)
// 0.69
#define MSG_MemoryEditor_BoxTitle               (346)
#define MSG_MemoryEditor_Disabled               (347)
#define MSG_MemoryEditor_Enabled                (348)
#define MSG_Menu_Tools_MemoryEditor             (349)
#define MSG_MemoryEditor_WriteZ80_Unable        (350)
#define MSG_MemoryEditor_Address_Out_of_Bound   (351)
#define MSG_Setup_Running                       (352)
// 0.69b
#define MSG_Options_Allow_Opposite_Directions   (353)
#define MSG_Inputs_SK1100_Enabled               (354)
#define MSG_Inputs_SK1100_Disabled              (355)
// Number of messages
#define MSG_MAX                                 (356)

//-----------------------------------------------------------------------------

#ifdef __MESSAGE_C__
#define __MSG_ADD(ID)   { #ID, ID }
static S2I_TYPE Msg_Translation_Table [] =
{
    __MSG_ADD(MSG_Quit),
    __MSG_ADD(MSG_About_Line_Meka_Date),
    __MSG_ADD(MSG_About_Line_Authors),
    __MSG_ADD(MSG_About_Line_Homepage),
    __MSG_ADD(MSG_Capture_Done),
    __MSG_ADD(MSG_Capture_Error_File),
    __MSG_ADD(MSG_SRAM_Loaded),
    __MSG_ADD(MSG_SRAM_Load_Unable),
    __MSG_ADD(MSG_SRAM_Wrote),
    __MSG_ADD(MSG_SRAM_Write_Unable),
    __MSG_ADD(MSG_93c46_Reset),
    __MSG_ADD(MSG_93c46_Loaded),
    __MSG_ADD(MSG_93c46_Load_Unable),
    __MSG_ADD(MSG_93c46_Wrote),
    __MSG_ADD(MSG_93c46_Write_Unable),
    __MSG_ADD(MSG_TVType_Set),
    __MSG_ADD(MSG_TVType_Info_Speed),
    __MSG_ADD(MSG_Blitters_Loading),
    __MSG_ADD(MSG_Blitters_Not_Enough),
    __MSG_ADD(MSG_Blitters_Not_Found),
    __MSG_ADD(MSG_Blitters_Missing),
    __MSG_ADD(MSG_Blitters_Unrecognized),
    __MSG_ADD(MSG_Blitters_Set),
    __MSG_ADD(MSG_NES_Activate),
    __MSG_ADD(MSG_NES_Sucks),
    __MSG_ADD(MSG_NES_Mapper_Unknown),
    __MSG_ADD(MSG_NES_Deny_Facts),
    __MSG_ADD(MSG_Debug_Trap_Read),
    __MSG_ADD(MSG_Debug_Trap_Write),
    __MSG_ADD(MSG_Debug_Trap_Port_Read),
    __MSG_ADD(MSG_Debug_Trap_Port_Write),
    __MSG_ADD(MSG_DataDump_Mode_Ascii),
    __MSG_ADD(MSG_DataDump_Mode_Raw),
    __MSG_ADD(MSG_DataDump_Error),
    __MSG_ADD(MSG_DataDump_Error_OB_Memory),
    __MSG_ADD(MSG_DataDump_Error_Palette),
    __MSG_ADD(MSG_DataDump_Error_Sprites),
    __MSG_ADD(MSG_DataDump_Main),
    __MSG_ADD(MSG_Doc_File_Error),
    __MSG_ADD(MSG_Doc_Enabled),
    __MSG_ADD(MSG_Doc_Disabled),
    __MSG_ADD(MSG_Flickering_Auto),
    __MSG_ADD(MSG_Flickering_Yes),
    __MSG_ADD(MSG_Flickering_No),
    __MSG_ADD(MSG_Layer_BG_Disabled),
    __MSG_ADD(MSG_Layer_BG_Enabled),
    __MSG_ADD(MSG_Layer_Spr_Disabled),
    __MSG_ADD(MSG_Layer_Spr_Enabled),
    __MSG_ADD(MSG_FDC765_Unknown_Read),
    __MSG_ADD(MSG_FDC765_Unknown_Write),
    __MSG_ADD(MSG_FDC765_Disk_Too_Large1),
    __MSG_ADD(MSG_FDC765_Disk_Too_Large2),
    __MSG_ADD(MSG_TVOekaki_Pen_Touch),
    __MSG_ADD(MSG_TVOekaki_Pen_Away),
    __MSG_ADD(MSG_Palette_Disabled),
    __MSG_ADD(MSG_Palette_Enabled),
    __MSG_ADD(MSG_Message_Disabled),
    __MSG_ADD(MSG_Message_Enabled),
    __MSG_ADD(MSG_TechInfo_Disabled),
    __MSG_ADD(MSG_TechInfo_Enabled),
    __MSG_ADD(MSG_TilesViewer_Disabled),
    __MSG_ADD(MSG_TilesViewer_Enabled),
    __MSG_ADD(MSG_RapidFire_JxBx_On),
    __MSG_ADD(MSG_RapidFire_JxBx_Off),
    __MSG_ADD(MSG_FM_Enabled),
    __MSG_ADD(MSG_FM_Disabled),
    __MSG_ADD(MSG_Country_European_US),
    __MSG_ADD(MSG_Country_JAP),
    __MSG_ADD(MSG_Must_Reset),
    __MSG_ADD(MSG_Error_File_Not_Found),
    __MSG_ADD(MSG_Error_File_Read),
    __MSG_ADD(MSG_Error_File_Empty),
    __MSG_ADD(MSG_Error_ZIP_Not_Supported),
    __MSG_ADD(MSG_Error_ZIP_Loading),
    __MSG_ADD(MSG_Error_ZIP_Internal),
    __MSG_ADD(MSG_Error_Base),
    __MSG_ADD(MSG_Error_Error),
    __MSG_ADD(MSG_Error_Memory),
    __MSG_ADD(MSG_Error_Param),
    __MSG_ADD(MSG_Error_Syntax),
    __MSG_ADD(MSG_Error_Video_Mode),
    __MSG_ADD(MSG_Patch_Value_Not_a_Byte),
    __MSG_ADD(MSG_Patch_Missing),
    __MSG_ADD(MSG_Patch_Unrecognized),
    __MSG_ADD(MSG_Patch_Out_of_Bound),
    __MSG_ADD(MSG_Glasses_Enabled),
    __MSG_ADD(MSG_Glasses_Disabled),
    __MSG_ADD(MSG_Glasses_Show_Both),
    __MSG_ADD(MSG_Glasses_Show_Left),
    __MSG_ADD(MSG_Glasses_Show_Right),
    __MSG_ADD(MSG_Glasses_Com_Port),
    __MSG_ADD(MSG_Glasses_Com_Port2),
    __MSG_ADD(MSG_Glasses_Unsupported),
    __MSG_ADD(MSG_Inputs_Src_Loading),
    __MSG_ADD(MSG_Inputs_Src_Not_Enough),
    __MSG_ADD(MSG_Inputs_Src_Missing),
    __MSG_ADD(MSG_Inputs_Src_Equal),
    __MSG_ADD(MSG_Inputs_Src_Unrecognized),
    __MSG_ADD(MSG_Inputs_Src_Syntax_Param),
    __MSG_ADD(MSG_Inputs_Src_Inconsistency),
    __MSG_ADD(MSG_Inputs_Src_Map_Keyboard),
    __MSG_ADD(MSG_Inputs_Src_Map_Keyboard_Ok),
    __MSG_ADD(MSG_Inputs_Src_Map_Joypad),
    __MSG_ADD(MSG_Inputs_Src_Map_Joypad_Ok_A),
    __MSG_ADD(MSG_Inputs_Src_Map_Joypad_Ok_B),
    __MSG_ADD(MSG_Inputs_Src_Map_Mouse),
    __MSG_ADD(MSG_Inputs_Src_Map_Mouse_Ok_B),
    __MSG_ADD(MSG_Inputs_Src_Map_Mouse_No_A),
    __MSG_ADD(MSG_Inputs_Src_Map_Cancelled),
    __MSG_ADD(MSG_Machine_Pause),
    __MSG_ADD(MSG_Machine_Resume),
    __MSG_ADD(MSG_Machine_Reset),
    __MSG_ADD(MSG_Inputs_Joy_Init),
    __MSG_ADD(MSG_Inputs_Joy_Init_None),
    __MSG_ADD(MSG_Inputs_Joy_Init_Found),
    __MSG_ADD(MSG_Inputs_Joy_Calibrate_Error),
    __MSG_ADD(MSG_FDB_Loading),
    __MSG_ADD(MSG_Welcome),
    __MSG_ADD(MSG_Window_Title),
    __MSG_ADD(MSG_Patch_Loading),
    __MSG_ADD(MSG_DB_Loading),
    __MSG_ADD(MSG_DB_Name_Default),
    __MSG_ADD(MSG_DB_Name_NoCartridge),
    __MSG_ADD(MSG_Config_Loading),
    __MSG_ADD(MSG_Datafile_Loading),
    __MSG_ADD(MSG_Failed),
    __MSG_ADD(MSG_Driver_Unknown),
    __MSG_ADD(MSG_OverDump),
    __MSG_ADD(MSG_Debug_Init),
    __MSG_ADD(MSG_Debug_Welcome),
    __MSG_ADD(MSG_Debug_Not_Available),
    __MSG_ADD(MSG_Debug_Brk_Need_Param),
    //__MSG_ADD(MSG_Debug_Brk_Enable),
    __MSG_ADD(MSG_Menu_Debug_Enabled),
    __MSG_ADD(MSG_Sound_Init),
    __MSG_ADD(MSG_Sound_Init_Error_SEAL),
    __MSG_ADD(MSG_Sound_Init_Error_Audio),
    __MSG_ADD(MSG_Sound_Init_Error_Blaster),
    __MSG_ADD(MSG_Sound_Init_Error_Blaster_A),
    __MSG_ADD(MSG_Sound_Init_Error_Voices),
    __MSG_ADD(MSG_Sound_Init_Error_Voice_N),
    __MSG_ADD(MSG_Sound_Init_Soundcard),
    __MSG_ADD(MSG_Setup_Soundcard_Select),
    __MSG_ADD(MSG_Setup_Soundcard_Select_Tips_DOS),
    __MSG_ADD(MSG_Setup_Soundcard_Select_Tips_Win32),
    __MSG_ADD(MSG_Sound_Init_Soundcard_No),
    __MSG_ADD(MSG_Sound_Init_SN76496),
    __MSG_ADD(MSG_Sound_Init_YM2413_OPL),
    __MSG_ADD(MSG_Sound_Stream_Error),
    __MSG_ADD(MSG_Sound_Rate_Changed),
    __MSG_ADD(MSG_Theme_Loading),
    __MSG_ADD(MSG_Theme_Not_Enough),
    __MSG_ADD(MSG_Theme_Missing),
    __MSG_ADD(MSG_Theme_Unrecognized),
    __MSG_ADD(MSG_Theme_Error_BG_Big),
    __MSG_ADD(MSG_Theme_Error_BG),
    __MSG_ADD(MSG_Theme_Error_BG_FileName),
    __MSG_ADD(MSG_LoadROM_Loading),
    __MSG_ADD(MSG_LoadROM_Success),
    __MSG_ADD(MSG_LoadDisk_Success),
    __MSG_ADD(MSG_LoadROM_Comment),
    __MSG_ADD(MSG_LoadROM_Warning),
    __MSG_ADD(MSG_LoadROM_Bad_Dump_Long),
    __MSG_ADD(MSG_LoadROM_Bad_Dump_Short),
    __MSG_ADD(MSG_LoadROM_Product_Num),
    __MSG_ADD(MSG_FM_Editor_Enabled),
    __MSG_ADD(MSG_FM_Editor_Disabled),
    __MSG_ADD(MSG_Frameskip_Auto),
    __MSG_ADD(MSG_Frameskip_Standard),
    __MSG_ADD(MSG_Log_Need_Param),
    __MSG_ADD(MSG_Inputs_Joypad),
    __MSG_ADD(MSG_Inputs_LightPhaser),
    __MSG_ADD(MSG_Inputs_PaddleControl),
    __MSG_ADD(MSG_Inputs_SportsPad),
    __MSG_ADD(MSG_Inputs_TVOekaki),
    __MSG_ADD(MSG_Inputs_Play_Digital),
    __MSG_ADD(MSG_Inputs_Play_Mouse),
    __MSG_ADD(MSG_Inputs_Play_Pen),
    __MSG_ADD(MSG_No_ROM),
    __MSG_ADD(MSG_Init_Allegro),
    __MSG_ADD(MSG_Init_GUI),
    __MSG_ADD(MSG_Init_Completed),
    __MSG_ADD(MSG_Load_Need_Param),
    __MSG_ADD(MSG_Load_Error),
    __MSG_ADD(MSG_Load_Not_Valid),
    __MSG_ADD(MSG_Load_Success),
    __MSG_ADD(MSG_Load_Version),
    __MSG_ADD(MSG_Load_Wrong_System),
    __MSG_ADD(MSG_Load_Massage),
    __MSG_ADD(MSG_Save_Error),
    __MSG_ADD(MSG_Save_Success),
    __MSG_ADD(MSG_Save_Slot),
    __MSG_ADD(MSG_Error_Directory_Open),
    __MSG_ADD(MSG_Log_Session_Start),
    __MSG_ADD(MSG_Options_BIOS_Enable),
    __MSG_ADD(MSG_Options_Product_Number),
    __MSG_ADD(MSG_Options_Bright_Palette),
    __MSG_ADD(MSG_Options_Load_Close),
    __MSG_ADD(MSG_Options_Load_FullScreen),
    __MSG_ADD(MSG_Options_DB_Display),
    __MSG_ADD(MSG_Options_FullScreen_Messages),
    __MSG_ADD(MSG_Options_GUI_VSync),
    __MSG_ADD(MSG_Options_NES_Activated),
    __MSG_ADD(MSG_Language_Set),
    __MSG_ADD(MSG_Language_Set_Warning),
    __MSG_ADD(MSG_Sound_Dumping_Start),
    __MSG_ADD(MSG_Sound_Dumping_Stop),
    __MSG_ADD(MSG_Sound_Dumping_Error_File_1),
    __MSG_ADD(MSG_Sound_Dumping_Error_File_2),
    __MSG_ADD(MSG_LoadROM_SDSC),
    __MSG_ADD(MSG_LoadROM_SDSC_Name),
    __MSG_ADD(MSG_LoadROM_SDSC_Version),
    __MSG_ADD(MSG_LoadROM_SDSC_Date),
    __MSG_ADD(MSG_LoadROM_SDSC_Release_Note),
    __MSG_ADD(MSG_LoadROM_SDSC_Unknown),
    __MSG_ADD(MSG_LoadROM_SDSC_Error),
    __MSG_ADD(MSG_Save_Not_in_BIOS),
    __MSG_ADD(MSG_Sound_Dumping_VGM_Acc_Frame),
    __MSG_ADD(MSG_Sound_Dumping_VGM_Acc_Sample),
    __MSG_ADD(MSG_Sound_Dumping_VGM_Acc_Change),
    __MSG_ADD(MSG_FDC765_Disk_Too_Small1),
    __MSG_ADD(MSG_FDC765_Disk_Too_Small2),
    __MSG_ADD(MSG_LoadROM_SDSC_Author),
    __MSG_ADD(MSG_VoiceRecognition_BoxTitle),
    __MSG_ADD(MSG_VoiceRecognition_Enabled),
    __MSG_ADD(MSG_VoiceRecognition_Disabled),
    __MSG_ADD(MSG_TilesViewer_BoxTitle),
    __MSG_ADD(MSG_TilesViewer_Tile),
    __MSG_ADD(MSG_Options_BoxTitle),
    __MSG_ADD(MSG_Options_Close),
    __MSG_ADD(MSG_FileBrowser_BoxTitle),
    __MSG_ADD(MSG_FileBrowser_Drive),
    __MSG_ADD(MSG_FileBrowser_Load),
    __MSG_ADD(MSG_FileBrowser_Close),
    __MSG_ADD(MSG_FileBrowser_LoadNames),
    __MSG_ADD(MSG_FileBrowser_ReloadDir),
    __MSG_ADD(MSG_Inputs_Configuration_BoxTitle),
    __MSG_ADD(MSG_Message_BoxTitle),
    __MSG_ADD(MSG_TechInfo_BoxTitle),
    __MSG_ADD(MSG_Doc_BoxTitle),
    __MSG_ADD(MSG_About_BoxTitle),
    __MSG_ADD(MSG_Palette_BoxTitle),
    __MSG_ADD(MSG_Palette_BoxTitle),
    __MSG_ADD(MSG_Menu_Main),
    __MSG_ADD(MSG_Menu_Main_LoadROM),
    __MSG_ADD(MSG_Menu_Main_FreeROM),
    __MSG_ADD(MSG_Menu_Main_SaveState),
    __MSG_ADD(MSG_Menu_Main_LoadState),
    __MSG_ADD(MSG_Menu_Main_Options),
    __MSG_ADD(MSG_Menu_Main_Language),
    __MSG_ADD(MSG_Menu_Main_Quit),
    __MSG_ADD(MSG_Menu_Debug),
    __MSG_ADD(MSG_Menu_Debug_Dump),
    __MSG_ADD(MSG_Menu_Debug_Watch),
    __MSG_ADD(MSG_Menu_Machine),
    __MSG_ADD(MSG_Menu_Machine_Power),
    __MSG_ADD(MSG_Menu_Machine_Power_On),
    __MSG_ADD(MSG_Menu_Machine_Power_Off),
    __MSG_ADD(MSG_Menu_Machine_Country),
    __MSG_ADD(MSG_Menu_Machine_Country_EU),
    __MSG_ADD(MSG_Menu_Machine_Country_Jap),
    __MSG_ADD(MSG_Menu_Machine_TVType),
    __MSG_ADD(MSG_Menu_Machine_TVType_NTSC),
    __MSG_ADD(MSG_Menu_Machine_TVType_PALSECAM),
    __MSG_ADD(MSG_Menu_Machine_HardPause),
    __MSG_ADD(MSG_Menu_Machine_HardReset),
    __MSG_ADD(MSG_Menu_Video),
    __MSG_ADD(MSG_Menu_Video_FullScreen),
    __MSG_ADD(MSG_Menu_Video_Themes),
    __MSG_ADD(MSG_Menu_Video_Blitters),
    __MSG_ADD(MSG_Menu_Video_Layers),
    __MSG_ADD(MSG_Menu_Video_Layers_Sprites),
    __MSG_ADD(MSG_Menu_Video_Layers_Background),
    __MSG_ADD(MSG_Menu_Video_Flickering),
    __MSG_ADD(MSG_Menu_Video_Flickering_Auto),
    __MSG_ADD(MSG_Menu_Video_Flickering_Yes),
    __MSG_ADD(MSG_Menu_Video_Flickering_No),
    __MSG_ADD(MSG_Menu_Video_3DGlasses),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_Enabled),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_ShowBothSides),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_ShowLeftSide),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_ShowRightSide),
    __MSG_ADD(MSG_Menu_Video_3DGlasses_UsesCOMPort),
    __MSG_ADD(MSG_Menu_Video_Screens),
    __MSG_ADD(MSG_Menu_Video_Screens_New),
    __MSG_ADD(MSG_Menu_Video_Screens_KillLast),
    __MSG_ADD(MSG_Menu_Video_Screens_KillAll),
    __MSG_ADD(MSG_Menu_Sound),
    __MSG_ADD(MSG_Menu_Sound_FM),
    __MSG_ADD(MSG_Menu_Sound_FM_Enabled),
    __MSG_ADD(MSG_Menu_Sound_FM_Disabled),
    __MSG_ADD(MSG_Menu_Sound_FM_Editor),
    __MSG_ADD(MSG_Menu_Sound_Volume),
    __MSG_ADD(MSG_Menu_Sound_Volume_Mute),
    __MSG_ADD(MSG_Menu_Sound_Volume_Value),
    __MSG_ADD(MSG_Menu_Sound_Rate),
    __MSG_ADD(MSG_Menu_Sound_Rate_Hz),
    __MSG_ADD(MSG_Menu_Sound_Channels),
    __MSG_ADD(MSG_Menu_Sound_Channels_Tone),
    __MSG_ADD(MSG_Menu_Sound_Channels_Noises),
    __MSG_ADD(MSG_Menu_Sound_Dump),
    __MSG_ADD(MSG_Menu_Sound_Dump_WAV_Start),
    __MSG_ADD(MSG_Menu_Sound_Dump_WAV_Stop),
    __MSG_ADD(MSG_Menu_Sound_Dump_VGM_Start),
    __MSG_ADD(MSG_Menu_Sound_Dump_VGM_Stop),
    __MSG_ADD(MSG_Menu_Sound_Dump_VGM_SampleAccurate),
    __MSG_ADD(MSG_Menu_Sound_VoiceRecognition),
    __MSG_ADD(MSG_Menu_Inputs),
    __MSG_ADD(MSG_Menu_Inputs_Joypad),
    __MSG_ADD(MSG_Menu_Inputs_LightPhaser),
    __MSG_ADD(MSG_Menu_Inputs_PaddleControl),
    __MSG_ADD(MSG_Menu_Inputs_SportsPad),
    __MSG_ADD(MSG_Menu_Inputs_GraphicBoard),
    __MSG_ADD(MSG_Menu_Inputs_SK1100),
    __MSG_ADD(MSG_Menu_Inputs_RapidFire),
    __MSG_ADD(MSG_Menu_Inputs_RapidFire_PxBx),
    __MSG_ADD(MSG_Menu_Inputs_Configuration),
    __MSG_ADD(MSG_Menu_Tools),
    __MSG_ADD(MSG_Menu_Tools_Messages),
    __MSG_ADD(MSG_Menu_Tools_Palette),
    __MSG_ADD(MSG_Menu_Tools_TilesViewer),
    __MSG_ADD(MSG_Menu_Tools_TechInfo),
    __MSG_ADD(MSG_Menu_Help),
    __MSG_ADD(MSG_Menu_Help_Documentation),
    __MSG_ADD(MSG_Menu_Help_About),
    __MSG_ADD(MSG_FM_Editor_BoxTitle),
    __MSG_ADD(MSG_Capture_Error),
    __MSG_ADD(MSG_Menu_Help_Documentation_W),
    __MSG_ADD(MSG_Menu_Help_Compat),
    __MSG_ADD(MSG_Menu_Help_Changes),
    __MSG_ADD(MSG_FPS_Counter_Enabled),
    __MSG_ADD(MSG_FPS_Counter_Disabled),
    __MSG_ADD(MSG_Menu_Help_Multiplayer_Games),
    __MSG_ADD(MSG_Menu_Sound_FM_Emulator),
    __MSG_ADD(MSG_Menu_Sound_FM_Emulator_OPL),
    __MSG_ADD(MSG_Menu_Sound_FM_Emulator_Digital),
    __MSG_ADD(MSG_Sound_Init_YM2413_Digital),
    __MSG_ADD(MSG_Ok),
    __MSG_ADD(MSG_Sound_Volume_Changed),
    __MSG_ADD(MSG_Error_Video_Mode_Back_To_GUI),
    __MSG_ADD(MSG_Menu_Help_Documentation_U),
    __MSG_ADD(MSG_Setup_SampleRate_Select),
    __MSG_ADD(MSG_Setup_Setup),
    __MSG_ADD(MSG_Glasses_Com_Port_Open_Error),
    __MSG_ADD(MSG_Inputs_Play_Digital_Unrecommended),
    __MSG_ADD(MSG_DB_SyntaxError),
    __MSG_ADD(MSG_LoadROM_SMSGG_Mode_Comment),
    __MSG_ADD(MSG_MemoryEditor_BoxTitle),
    __MSG_ADD(MSG_MemoryEditor_Disabled),
    __MSG_ADD(MSG_MemoryEditor_Enabled),
    __MSG_ADD(MSG_Menu_Tools_MemoryEditor),
    __MSG_ADD(MSG_MemoryEditor_WriteZ80_Unable),
    __MSG_ADD(MSG_MemoryEditor_Address_Out_of_Bound),
    __MSG_ADD(MSG_Setup_Running),
    __MSG_ADD(MSG_Options_Allow_Opposite_Directions),
    __MSG_ADD(MSG_Inputs_SK1100_Enabled),
    __MSG_ADD(MSG_Inputs_SK1100_Disabled),
    { NULL, MSG_NULL }
};
#undef __MSG_ADD
#endif

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// Structure storing messages for one supported language
typedef struct
{
    char *      Name;
    char *      Messages[MSG_MAX];
    int         WIP;
}               t_lang;

//-----------------------------------------------------------------------------

void            Lang_Set (int n);
void            Lang_Set_by_Name (char *name);
void            Langs_Menu_Add (int menu_id);

//-----------------------------------------------------------------------------

// Structure storing localization basis stuff and all supported languages
typedef struct
{
    char        FileName [FILENAME_LEN];      // path to the MEKA.MSG file
    t_lang *    Lang_Cur;
    t_lang *    Lang_Default;
    t_list *    Langs;
}               t_messages;

t_messages      Messages;

//-----------------------------------------------------------------------------
// Functions - Message
//-----------------------------------------------------------------------------

// Load messages from MEKA.MSG file (path given in structure)
// Return a MEKA_ERR_xxx code
int     Messages_Init (void);

// Get specified message string
static INLINE char *    Msg_Get(int n)
{
    return Messages.Lang_Cur->Messages[n];
}

// Send a message to the user and/or debugging message
void    Msg (int attr, const char *format, ...) FORMAT_PRINTF (2);

//-----------------------------------------------------------------------------
// Functions - Console
//-----------------------------------------------------------------------------

void    ConsoleInit         (void);
void    ConsoleClose        (void);
void    ConsolePrintf       (const char *format, ...) FORMAT_PRINTF (1);
void    ConsolePrint        (const char *msg);
void    ConsoleEnablePause  (void);
bool    ConsoleWaitForAnswer(bool allow_run);

//-- Old Messages (friends versions) ------------------------------------------
// #define Message_Welcome_Joseph   "Joyeux Anniversaire Joseph !! ^_^"
// #define Message_Welcome_Arnaud   "Que demander de mieux qu'un entourage de personnes a la fois passionnantes et talentueuses ? .. Donc voila une superbe version dedicacee de Mekarno, et tout et tout. Tu pourras meme prendre des captures avec F12 et les accrocher sur tes murs - j'en suis sur que tu le feras d'abord, hein avoues! Finissons donc cette dedicace avec des mots offrant le plus doux tes spectacles a ton ame talentueuse. Que le bonheur t'ennivre milles et une fois et que chaque moment puisse t'etre unique et jouissif a sa facon. Mine de rien je pourrais faire poete si je ne suis pas programmeur. Hehehe. Amuse toi bien! (PS: je passes chercher les cartons de jeux SMS quand tu veux! :) (PS2: et je te prend a n'importe quel jeu!)"

//-----------------------------------------------------------------------------
