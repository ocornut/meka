//-----------------------------------------------------------------------------
// MEKA - machine.c
// Emulated Machines Initialization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "bios.h"
#include "commport.h"
#include "db.h"
#include "debugger.h"
#include "eeprom.h"
#include "glasses.h"
#include "mappers.h"
#include "palette.h"
#include "vdp.h"
#include "video.h"
#include "tvoekaki.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

int Machine_Pause_Need_To = false;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// HARD PAUSE EMULATED MACHINE ------------------------------------------------
void    Machine_Pause (void)
{
    machine ^= MACHINE_PAUSED;
    CPU_Loop_Stop = TRUE;
    if (Machine_Pause_Need_To > 0)
        Machine_Pause_Need_To --;

    // Verbose
    if (machine & MACHINE_PAUSED)
    {
        Msg (MSGT_USER, Msg_Get (MSG_Machine_Pause));
        // gui_menu_check (menus_ID.machine, 2);
        Screen_Save_to_Next_Buffer ();
    }
    else
    {
        Msg (MSGT_USER, Msg_Get (MSG_Machine_Resume));
        // gui_menu_un_check_one (menus_ID.machine, 2);
    }
}

void    Machine_Debug_Start (void)
{
    // Msg (MSGT_DEBUG, "Machine_Debug_Start()");
    machine |= MACHINE_PAUSED | MACHINE_DEBUGGING;
    CPU_Loop_Stop = TRUE;
    Screen_Save_to_Next_Buffer ();
}

void    Machine_Debug_Stop (void)
{
    // Msg (MSGT_DEBUG, "Machine_Debug_Stop()");
    machine &= ~(MACHINE_PAUSED | MACHINE_DEBUGGING);
    // next pass in MainLoop() will restart CPU emulation
    // We however set the flag in case this function is called with emulation
    // is already running (eg: setting a breakpoint while running)
    CPU_Loop_Stop = TRUE;
}

void    Machine_Set_Handler_Loop (void)
{
    switch (cur_drv->id)
    {
    case DRV_COLECO:                   LoopZ80 = Loop_Coleco;          return;
    case DRV_SG1000: case DRV_SC3000:  LoopZ80 = Loop_SG1000_SC3000;   return;
    case DRV_SF7000:                   LoopZ80 = Loop_SG1000_SC3000;   return;
    default:                           LoopZ80 = Loop_SMS;             return;
    }
}

void    Machine_Set_Handler_IO (void)
{
    switch (cur_drv->id)
    {
    case DRV_COLECO:
        InZ80 = InZ80_NoHook = Coleco_Port_In;
        OutZ80 = OutZ80_NoHook = Coleco_Port_Out;
        // sms.Input_Mode = 1; // Useless since it is overwritten later
        return;
    case DRV_SF7000:
        InZ80 = InZ80_NoHook = In_SF7000;
        OutZ80 = OutZ80_NoHook = Out_SF7000;
        return;
    default:
        InZ80 = InZ80_NoHook = In_SMS;
        OutZ80 = OutZ80_NoHook = Out_SMS;
        return;
    }
}

#ifdef X86_ASM
extern "C"
{	
	READ_FUNC (Read_Default_ASM);
    WRITE_FUNC (Write_Mapper_32kRAM_ASM);
}
#endif

void    Machine_Set_Handler_Read (void)
{
    switch (cur_machine.mapper)
    {
    case MAPPER_93c46: // Used by Game Gear baseball games
        RdZ80 = RdZ80_NoHook = Read_Mapper_93c46;
        break;
    case MAPPER_TVOekaki: // Terebi Oekaki Graphic Board
        RdZ80 = RdZ80_NoHook = Read_Mapper_TVOekaki;
        break;
    case MAPPER_SMS_DisplayUnit: // SMS Display Unit
        RdZ80 = RdZ80_NoHook = Read_Mapper_SMS_DisplayUnit;
        break;
    default:
        #ifdef X86_ASM
            RdZ80 = RdZ80_NoHook = Read_Default_ASM;
            // RdZ80 = RdZ80_NoHook = Read_Default;
        #else
            RdZ80 = RdZ80_NoHook = Read_Default;
        #endif
        break;
    }
}

void    Machine_Set_Handler_Write (void)
{
    switch (cur_machine.mapper)
    {
	case MAPPER_SMS_NoMapper:			 // SMS games with no bank switching
		WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_NoMapper;
		break;
    case MAPPER_32kRAM:                  // Used by Sega Basic, The Castle, ..
        #ifdef X86_ASM
            WrZ80 = WrZ80_NoHook = Write_Mapper_32kRAM_ASM;  // ASM version
        #else
            WrZ80 = WrZ80_NoHook = Write_Mapper_32kRAM;
        #endif
        break;
    case MAPPER_ColecoVision:            // Colecovision
        WrZ80 = WrZ80_NoHook = Write_Mapper_Coleco;
        break;
    case MAPPER_CodeMasters:             // CodeMasters games
        WrZ80 = WrZ80_NoHook = Write_Mapper_CodeMasters;
        break;
    case MAPPER_93c46:                   // Used by Game Gear baseball games
        WrZ80 = WrZ80_NoHook = Write_Mapper_93c46;
        break;
    case MAPPER_SG1000:                  // 4 kb of RAM
        WrZ80 = WrZ80_NoHook = Write_Mapper_SG1000;
        break;
    case MAPPER_SMS_ActionReplay:        // SMS Action Replay with embedded RAM
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_ActionReplay;
        break;
    case MAPPER_TVOekaki:                // Terebi Oekaki Graphic Board
        WrZ80 = WrZ80_NoHook = Write_Mapper_TVOekaki;
        break;
    case MAPPER_SF7000:                  // SF-7000
        WrZ80 = WrZ80_NoHook = Write_Mapper_SF7000;
        break;
    case MAPPER_SMS_Korean:              // SMS Korean games
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean;
        break;
    case MAPPER_SMS_Korean_MSX_8KB:      // SMS Korean games with MSX-based 8KB mapper
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_MSX_8KB;
        break;
    case MAPPER_SMS_DisplayUnit:         // SMS Display Unit (RAM from 4000-47FF)
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_DisplayUnit;
        break;
    default:                             // Standard mapper
        WrZ80 = WrZ80_NoHook = Write_Default;
        break;
    }
}

void        Machine_Set_Mapper (void)
{
    if (DB.current_entry != NULL && DB.current_entry->emu_mapper != -1)
    {
        cur_machine.mapper = DB.current_entry->emu_mapper;
        return;
    }

    // Select default mapper per driver
    switch (cur_machine.driver_id)
    {
    case DRV_SC3000:
        cur_machine.mapper = MAPPER_32kRAM;
        return;
    case DRV_COLECO:
        cur_machine.mapper = MAPPER_ColecoVision;
        return;
    case DRV_SG1000:
        cur_machine.mapper = MAPPER_SG1000;
        return;
    case DRV_SF7000:
        cur_machine.mapper = MAPPER_SF7000;
        return;
        // case DRV_MSX:
        //     cur_machine.mapper = MAPPER_Msx;
        //     return;
    case DRV_SMS:
    case DRV_GG:
        cur_machine.mapper = MAPPER_Standard;
        if (DB.current_entry == NULL)    // Detect mapper for unknown ROM
        {
            const int m = Mapper_Autodetect();
            if (m != MAPPER_Auto)
                cur_machine.mapper = m;
        }
        return;
    default: // All Others (which ?)
        cur_machine.mapper = MAPPER_Standard;
        return;
    }
}

void    Machine_Set_Mapping (void)
{
    sms.Mapping_Register = 0;
    sms.SRAM_Pages = 0;
    sms.Pages_Reg [0] = 0; sms.Pages_Reg [1] = 1; sms.Pages_Reg [2] = 2;

    switch (cur_machine.mapper)
    {
    case MAPPER_32kRAM: // 32k RAM MAPPER --------------------------------------
        Map_8k_ROM (0, 0);
        Map_8k_ROM (1, 1 & tsms.Pages_Mask_8k);
        Map_8k_ROM (2, 2 & tsms.Pages_Mask_8k);
        Map_8k_ROM (3, 3 & tsms.Pages_Mask_8k);
        Map_8k_RAM (4, 0);
        Map_8k_RAM (5, 1);
        Map_8k_RAM (6, 2);
        Map_8k_RAM (7, 3);
        break;
    case MAPPER_ColecoVision: // COLECOVISION MAPPER ---------------------------
        Map_8k_Other (0, BIOS_ROM_Coleco);
        Map_8k_RAM (1, 1);
        Map_8k_RAM (2, 1);
        Map_8k_RAM (3, 0);
        Map_8k_ROM (4, 0);
        Map_8k_ROM (5, 1 & tsms.Pages_Mask_8k);
        Map_8k_ROM (6, 2 & tsms.Pages_Mask_8k);
        Map_8k_ROM (7, 3 & tsms.Pages_Mask_8k);
        break;
    case MAPPER_SMS_ActionReplay: // ACTION REPLAY MAPPER ----------------------
        Map_8k_ROM (0, 0);
        Map_8k_ROM (1, 1);
        Map_8k_RAM (2, 1);
        Map_8k_RAM (3, 1);
        Map_8k_RAM (4, 2); // FIXME
        Map_8k_RAM (5, 2);
        Map_8k_RAM (6, 0);
        Map_8k_RAM (7, 0);
        break;
    case MAPPER_SF7000: // SF-7000 ---------------------------------------------
        Map_16k_Other (0, BIOS_ROM_SF7000);
        Map_8k_RAM (2, 2);
        Map_8k_RAM (3, 3);
        Map_8k_RAM (4, 4);
        Map_8k_RAM (5, 5);
        Map_8k_RAM (6, 6);
        Map_8k_RAM (7, 7);
        break;
    case MAPPER_SMS_DisplayUnit: // DISPLAY UNIT MAPPER ----------------------
        Map_8k_ROM (0, 0);  // ROM
        Map_8k_ROM (1, 1);
        Map_8k_RAM (2, 1);  // RAM - Mapping 2k (0x800) from 4000 to BFFF
        Map_8k_RAM (3, 1);
        Map_8k_RAM (4, 1);
        Map_8k_RAM (5, 1);
        Map_8k_RAM (6, 0);  // RAM - Standard
        Map_8k_RAM (7, 0);
        break;
    case MAPPER_SMS_Korean_MSX_8KB:
        Map_8k_ROM (0, 0);
        Map_8k_ROM (1, 1);
        Map_8k_ROM (2, 2);
        Map_8k_ROM (3, 3);
        Map_8k_ROM (4, 4);
        Map_8k_ROM (5, 5);
        Map_8k_RAM (6, 0);
        Map_8k_RAM (7, 0);
        break;
    default: // Other mappers
        Map_8k_ROM (0, 0);
        Map_8k_ROM (1, 1);
        Map_8k_ROM (2, 2);
        Map_8k_ROM (3, 3);
        Map_8k_ROM (4, 4);
        Map_8k_ROM (5, 5);
        Map_8k_RAM (6, 0);
        Map_8k_RAM (7, 0);
        switch (cur_machine.mapper)
        {
		case MAPPER_SMS_NoMapper:
			break;
        case MAPPER_CodeMasters:
            // ROM [0x3FFF] = 0; ROM [0x7FFF] = 1; ROM [0xBFFF] = 2;
            break;
        case MAPPER_SMS_Korean:
            // ROM [0xA000] = 0;
            break;
        case MAPPER_93c46:
            RAM [0x1FFC] = 0; RAM [0x1FFD] = 0; RAM [0x1FFE] = 1; RAM [0x1FFF] = 2;
            EEPROM_93c46_Init (EEPROM_93C46_INIT_ALL);
            break;
        case MAPPER_TVOekaki:
            TVOekaki_Init ();
            break;
        default:
            RAM [0x1FFC] = 0; RAM [0x1FFD] = 0; RAM [0x1FFE] = 1; RAM [0x1FFF] = 2;
            memcpy (Game_ROM_Computed_Page_0, ROM, 0x4000);
			//Map_16k_Other (0, Game_ROM_Computed_Page_0);
			Map_16k_ROM (0, 0);	// Mapping may change to Game_ROM_Computed_Page_0 at runtime
            break;
        }
        break;
    }
}

void    Machine_Set_Country (void)
{
    if (DB.current_entry && DB.current_entry->emu_country != -1)
        sms.Country = DB.current_entry->emu_country;
    else
        sms.Country = g_Configuration.country;
}

void    Machine_Set_IPeriod (void)
{
    if (DB.current_entry && DB.current_entry->emu_iperiod != -1)
    {
        opt.Cur_IPeriod = DB.current_entry->emu_iperiod;
        return;
    }

    switch (cur_drv->id)
    {
    case DRV_COLECO:
        opt.Cur_IPeriod = opt.IPeriod_Coleco;
        break;
    case DRV_SG1000:
    case DRV_SC3000:
    case DRV_SF7000:
        opt.Cur_IPeriod = opt.IPeriod_Sg1000_Sc3000;
        break;
    default:
        opt.Cur_IPeriod = opt.IPeriod;
        break;
    }
}

// FIXME: rename function
void    Machine_Set_TV_Lines (void)
{
    if (DB.current_entry && DB.current_entry->emu_tvtype != -1)
        cur_machine.TV = &TV_Type_Table [DB.current_entry->emu_tvtype];
    else
        cur_machine.TV = TV_Type_User;
    cur_machine.TV_lines = cur_machine.TV->screen_lines;
}

// RESET EMULATED MACHINE -----------------------------------------------------
void        Machine_Reset (void)
{
    int     i;
    static byte VDPInit [16] =
    {
        /* Values set by BIOS */
        0x36, /*0xA0*/ 0x80 /* zero for Coleco */, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0x00,
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00
        /* Old (guessed ?) values */
        // 0x06, 0x00, 0x0E, 0xFF, 0xFF, 0x7F, 0x00, 0x00,
        // 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    #ifdef DEBUG_WHOLE
        Msg (MSGT_DEBUG, "Machine_Reset();");
    #endif

    // Unpause machine if necessary
    if (machine & MACHINE_PAUSED)
        Machine_Pause ();

    // Set driver & machine stuff
    drv_set (cur_machine.driver_id);

    Machine_Set_Mapper          ();
    if ((machine & MACHINE_RUN) != 0 /*== MACHINE_RUN */)
        Machine_Set_Mapping      (); // ^^ FIXME: the test above isn't beautiful since MACHINE_RUN contains multiple flags, but I'm unsure which of them is actually needed to perform the correct test
    Machine_Set_Handler_IO      ();
    Machine_Set_Handler_Read    ();
    Machine_Set_Handler_Write   ();
    Machine_Set_Handler_Loop    ();
    Machine_Set_Country         ();
    Machine_Set_IPeriod         ();
    Machine_Set_TV_Lines        ();

    // VDP MODEL --------------------------------------------------------------
    if (DB.current_entry && DB.current_entry->emu_vdp_model != -1)
	{
        cur_machine.VDP.model = DB.current_entry->emu_vdp_model;
	}
    else
    {
        if (cur_drv->id == DRV_GG)
            cur_machine.VDP.model = VDP_MODEL_315_5378;
        else
            cur_machine.VDP.model = VDP_MODEL_315_5226;
    }

    // 3-D GLASSES ------------------------------------------------------------
    sms.Glasses_Register = ((Glasses.Mode == GLASSES_MODE_SHOW_ONLY_LEFT) ? 1 : 0);

    // CPU (MARAT FAIZULLIN'S CORE) -------------------------------------------
    CPU_Loop_Stop = TRUE;
    CPU_ForceNMI  = FALSE;

#ifdef MARAT_Z80
    sms.R.IPeriod = opt.Cur_IPeriod;
    sms.R.Trace = FALSE;
    ResetZ80 (&sms.R);
    sms.R.IFF |= IFF_IM1;
    sms.R.IAutoReset = FALSE;
    sms.R.TrapBadOps = FALSE;

    // CPU (MAME'S CORE) ------------------------------------------------------
#elif MAME_Z80
    z80_reset (NULL);
    z80_set_irq_callback (Get_IRQ_Vector);
    z80_set_irq_line (0, ASSERT_LINE);
    z80_set_irq_line (0, CLEAR_LINE);

    // CPU (RICHARD MITTON'S CORE) --------------------------------------------
#elif RAZE_Z80
    z80_init_memmap ();
    z80_add_write (0x0000, 0xFFFF, Z80_MAP_HANDLED, WrZ80);
    for (i = 0; i < 4; i ++)
    {
        z80_add_read ((i * 0x2000), (i * 0x2000) + 0x1FFF, Z80_MAP_DIRECT, Mem_Pages [i]);
        z80_map_fetch ((i * 0x2000), (i * 0x2000) + 0x1FFF, Mem_Pages [i]);
    }
    for (i = 4; i < 8; i ++)
    {
        z80_map_fetch ((i * 0x2000), (i * 0x2000) + 0x1FFF, Mem_Pages [i]);
        z80_map_read ((i * 0x2000), (i * 0x2000) + 0x1FFF, Mem_Pages [i]);
    }
    z80_set_in (InZ80);
    z80_set_out (OutZ80);
    z80_end_memmap ();
    z80_reset ();
#endif

    // MEMORY -----------------------------------------------------------------
    memset (RAM,  0, 0x10000);      // Clear all RAM
    memset (VRAM, 0, 0x04000);      // Clear all VRAM
    memset (PRAM, 0, 0x00040);      // Clear all PRAM (palette)

    // Unload BIOS if...
    if ((cur_drv->id != DRV_SMS || sms.Country != COUNTRY_EXPORT) && (machine & MACHINE_ROM_LOADED))
    {
        #ifdef DEBUG_WHOLE
            Msg (MSGT_DEBUG, "Machine_Reset(): BIOS_Unload()");
        #endif
        BIOS_Unload ();
    }

    // GRAPHICS ---------------------------------------------------------------
    for (i = 0; i < 16; i++)
        Tms_VDP_Out (i, ((i == 1 && cur_drv->id == DRV_COLECO) ? 0x00 : VDPInit [i]));
    for (i = 0; i < MAX_TILES; i++)
        tgfx.Tile_Dirty [i] = TILE_DIRTY_DECODE | TILE_DIRTY_REDRAW;

    VDP_UpdateLineLimits();

	//assert(!Screenbuffer_IsLocked());
	al_set_target_bitmap(screenbuffer_1);
    al_clear_to_color(COLOR_BLACK);
	al_set_target_bitmap(screenbuffer_2);
    al_clear_to_color(COLOR_BLACK);
    screenbuffer = screenbuffer_1;
    screenbuffer_next = screenbuffer_2;

    cur_machine.VDP.sprite_shift_x = 0;
    cur_machine.VDP.scroll_x_latched = 0;
    cur_machine.VDP.scroll_y_latched = 0;
    memset(cur_machine.VDP.scroll_x_latched_table, 0, sizeof(cur_machine.VDP.scroll_x_latched_table));
    tsms.VDP_Video_Change = VDP_VIDEO_CHANGE_ALL;

    // GRAPHICS: SPRITE FLICKERING --------------------------------------------
    if (g_Configuration.sprite_flickering & SPRITE_FLICKERING_AUTO)
    {
        if (DB.current_entry && (DB.current_entry->flags & DB_FLAG_EMU_SPRITE_FLICKER))
            g_Configuration.sprite_flickering |= SPRITE_FLICKERING_ENABLED;
        else
            g_Configuration.sprite_flickering &= ~SPRITE_FLICKERING_ENABLED;
    }

    // PALETTE ----------------------------------------------------------------
    //if (machine & MACHINE_POWER_ON)
    Palette_Emulation_Reset();

    // INPUT/OUTPUT/VDP -------------------------------------------------------
    sms.VDP_Status = 0x00;
    sms.VDP_Address = 0x0000;
    sms.VDP_Access_Mode = VDP_Access_Mode_1;
    sms.VDP_Access_First = 0x00;
    sms.VDP_Pal = 0x00;
    sms.VDP_ReadLatch = 0x00;
    sms.Lines_Left = 255;
    sms.Pending_HBlank = FALSE;
	sms.Pending_NMI = FALSE;
    tsms.VDP_Line = 0;

    // CONTROLLERS ------------------------------------------------------------
    for (i = 0; i < 8; i ++)
        tsms.Control [i] = 0xFFFF; /* 0x3FFF */
    tsms.Control_GG = /*0x20 | 0x80*/ 0;
    tsms.Control_Start_Pause = 0;
    tsms.Periph_Nat = 0;
    sms.Input_Mode = 0x07;

    // SOUND ------------------------------------------------------------------
    sms.FM_Register = 0;
    sms.FM_Magic = 0;
    // if (fm_use == TRUE) fm_init (FM_ALL_INIT);
    // resume_fm ();
    FM_Reset ();
    SN76489_Reset (cur_machine.TV->CPU_clock, g_sasound.audio_sample_rate);
    if (Sound.LogVGM.Logging == VGM_LOGGING_ACCURACY_SAMPLE)
        VGM_Update_Timing (&Sound.LogVGM);

    // Reset sound cycle counter
    Sound_Update_Count = 0;
    Sound.CycleCounter = 0;

    // FIXME: add a reset handler per driver, instead of the code below...

    // GAME GEAR COMMUNICATION PORT
    if (cur_machine.driver_id == DRV_GG)
    {
        Comm_Reset ();
    }

    // SF-7000
    if (cur_machine.driver_id == DRV_SF7000)
    {
        SF7000_Reset ();
    }

    // DEBUGGER ---------------------------------------------------------------
    #ifdef MEKA_Z80_DEBUGGER
        Debugger_MachineReset ();
    #endif
}

//-----------------------------------------------------------------------------
