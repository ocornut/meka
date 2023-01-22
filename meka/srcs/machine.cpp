
// MEKA - machine.c
// Emulated Machines Initialization - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "bios.h"
#include "coleco.h"
#include "commport.h"
#include "db.h"
#include "debugger.h"
#include "eeprom.h"
#include "glasses.h"
#include "mappers.h"
#include "palette.h"
#include "sf7000.h"
#include "sg1ksc3k.h"
#include "vdp.h"
#include "video.h"
#include "vmachine.h"
#include "tvoekaki.h"
#include "tvtype.h"
#include "sound/fmunit.h"
#include "sound/psg.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

int g_machine_pause_requests = 0;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Machine_Pause()
{
    g_machine_flags ^= MACHINE_PAUSED;
    CPU_Loop_Stop = TRUE;
    if (g_machine_pause_requests > 0)
        g_machine_pause_requests--;

    // Verbose
    if (g_machine_flags & MACHINE_PAUSED)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Machine_Pause));
        // gui_menu_check (menus_ID.machine, 2);
        Screen_Save_to_Next_Buffer();
    }
    else
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Machine_Resume));
        // gui_menu_uncheck_range(menus_ID.machine, 2, 2);
    }
}

void    Machine_Debug_Start (void)
{
    // Msg(MSGT_DEBUG, "Machine_Debug_Start()");
    g_machine_flags |= MACHINE_PAUSED | MACHINE_DEBUGGING;
    CPU_Loop_Stop = TRUE;
    Screen_Save_to_Next_Buffer();
}

void    Machine_Debug_Stop (void)
{
    // Msg(MSGT_DEBUG, "Machine_Debug_Stop()");
    g_machine_flags &= ~(MACHINE_PAUSED | MACHINE_DEBUGGING);
    // next pass in MainLoop() will restart CPU emulation
    // We however set the flag in case this function is called with emulation
    // is already running (eg: setting a breakpoint while running)
    CPU_Loop_Stop = TRUE;
}

// Note: called everytime we change video mode.
void    Machine_Set_Handler_Loop(void)
{
    switch (g_driver->id)
    {
    case DRV_COLECO:                   LoopZ80 = Loop_Coleco;          return;
    case DRV_SG1000: case DRV_SC3000:  LoopZ80 = Loop_SG1000_SC3000;   return;
    case DRV_SF7000:                   LoopZ80 = Loop_SG1000_SC3000;   return;
    default:                           LoopZ80 = Loop_SMS;             return;
    }
}

void    Machine_Set_Handler_IO(void)
{
    switch (g_driver->id)
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

void    Machine_Set_Handler_MemRW(void)
{
    RdZ80 = RdZ80_NoHook = Read_Default;
    WrZ80 = WrZ80_NoHook = Write_Default;

    switch (g_machine.mapper)
    {
    case MAPPER_SMS_NoMapper:            // SMS games with no bank switching
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_NoMapper;
        break;
    case MAPPER_32kRAM:                  // Used by Sega Basic, The Castle, ..
    case MAPPER_SC3000_Survivors_Multicart:
        WrZ80 = WrZ80_NoHook = Write_Mapper_32kRAM;
        break;
    case MAPPER_ColecoVision:            // Colecovision
        WrZ80 = WrZ80_NoHook = Write_Mapper_Coleco;
        break;
    case MAPPER_CodeMasters:             // CodeMasters games
        WrZ80 = WrZ80_NoHook = Write_Mapper_CodeMasters;
        break;
    case MAPPER_93c46:                   // Used by Game Gear baseball games
        RdZ80 = RdZ80_NoHook = Read_Mapper_93c46;
        WrZ80 = WrZ80_NoHook = Write_Mapper_93c46;
        break;
    case MAPPER_SG1000:                  // 4 kb of RAM
        WrZ80 = WrZ80_NoHook = Write_Mapper_SG1000;
        break;
    case MAPPER_SMS_ActionReplay:        // SMS Action Replay with embedded RAM
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_ActionReplay;
        break;
    case MAPPER_TVOekaki:                // Terebi Oekaki Graphic Board
        RdZ80 = RdZ80_NoHook = Read_Mapper_TVOekaki;
        WrZ80 = WrZ80_NoHook = Write_Mapper_TVOekaki;
        break;
    case MAPPER_SF7000:                  // SF-7000
        WrZ80 = WrZ80_NoHook = Write_Mapper_SF7000;
        break;
    case MAPPER_SMS_Korean_A000:         // SMS Korean games with A000 register
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_A000;
        break;
        break;
    case MAPPER_SMS_Korean_BFFC:         // SMS Korean games with BFFC register
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_BFFC;
        break;
    case MAPPER_SMS_Korean_MSX_8KB_0003: // SMS Korean games with MSX-based 8KB mapper
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_MSX_8KB_0003;
        break;
    case MAPPER_SMS_Korean_MSX_8KB_0300: // SMS Korean games with MSX-based 8KB mapper
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_MSX_8KB_0300;
        break;
    case MAPPER_SMS_Korean_Janggun:      // SMS Korean Janggun-ui Adeul
        RdZ80 = RdZ80_NoHook = Read_Mapper_SMS_Korean_Janggun;
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_Janggun;
        break;
    case MAPPER_SMS_4PakAllAction:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_4PakAllAction;
        break;
    case MAPPER_SMS_DisplayUnit:         // SMS Display Unit (RAM from 4000-47FF)
        RdZ80 = RdZ80_NoHook = Read_Mapper_SMS_DisplayUnit;
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_DisplayUnit;
        break;
    case MAPPER_SG1000_Taiwan_MSX_Adapter_TypeA:
        RdZ80 = RdZ80_NoHook = Read_Mapper_SG1000_Taiwan_MSX_Adapter_TypeA;
        WrZ80 = WrZ80_NoHook = Write_Mapper_SG1000_Taiwan_MSX_Adapter_TypeA;
        break;
    case MAPPER_SMS_Korean_FFFF_HiCom:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_FFFF_HiCom;
        break;
    case MAPPER_SMS_Korean_2000_xor_1F:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_2000_xor_1F;
        break;
    case MAPPER_SMS_Korean_FFFE:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_FFFE;
        break;
    case MAPPER_SMS_Korean_FFF3_FFFC:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_FFF3_FFFC;
        break;
    case MAPPER_SMS_Korean_0000_xor_FF:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_0000_xor_FF;
        break;
    case MAPPER_SMS_Korean_MD_FFF0:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_MD_FFF0;
        break;
    case MAPPER_SMS_Korean_MD_FFF5:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_MD_FFF5;
        break;
    case MAPPER_SMS_Korean_MD_FFFA:
        WrZ80 = WrZ80_NoHook = Write_Mapper_SMS_Korean_MD_FFFA;
        break;
    }
}

void        Machine_Set_Mapper(void)
{
    if (DB.current_entry != NULL && DB.current_entry->emu_mapper != -1)
    {
        g_machine.mapper = DB.current_entry->emu_mapper;
        return;
    }

    // Select default mapper per driver
    switch (g_machine.driver_id)
    {
    case DRV_SC3000:
        if (tsms.Size_ROM <= 32*1024)
            g_machine.mapper = MAPPER_32kRAM;       // FIXME: Not technically correct. Should be enabled for BASIC.
        else
            g_machine.mapper = MAPPER_SG1000;
        if (DB.current_entry == NULL && tsms.Size_ROM >= 0x200000)
            if (memcmp(ROM+0x1F8004, "SC-3000 SURVIVORS MULTICART BOOT MENU", 38) == 0)
                g_machine.mapper = MAPPER_SC3000_Survivors_Multicart;
        return;
    case DRV_COLECO:
        g_machine.mapper = MAPPER_ColecoVision;
        return;
    case DRV_SG1000:
        g_machine.mapper = MAPPER_SG1000;
        return;
    case DRV_SF7000:
        g_machine.mapper = MAPPER_SF7000;
        return;
    case DRV_SMS:
    case DRV_GG:
        if (tsms.Size_ROM <= 48*1024)
            g_machine.mapper = MAPPER_SMS_NoMapper;
        else 
            g_machine.mapper = MAPPER_Standard;
        if (DB.current_entry == NULL)    // Detect mapper for unknown ROM
        {
            const int m = Mapper_Autodetect();
            if (m != MAPPER_Auto)
                g_machine.mapper = m;
        }
        return;
    default: // All Others (which ?)
        g_machine.mapper = MAPPER_Standard;
        return;
    }
}

void    Machine_Set_Mapping (void)
{
    sms.SRAM_Mapping_Register = 0;
    sms.SRAM_Pages = 0;

    g_machine.mapper_regs_count = 3;    // Default
    for (int i = 0; i != MAPPER_REGS_MAX; i++)
        g_machine.mapper_regs[i] = 0;
    g_machine.mapper_regs[0] = 0;
    g_machine.mapper_regs[1] = 1;
    g_machine.mapper_regs[2] = 2;

    switch (g_machine.mapper)
    {
    case MAPPER_32kRAM: // 32k RAM MAPPER --------------------------------------
        Map_8k_ROM(0, 0);
        Map_8k_ROM(1, 1 & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, 2 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 3 & tsms.Pages_Mask_8k);
        Map_8k_RAM(4, 0);
        Map_8k_RAM(5, 1);
        Map_8k_RAM(6, 2);
        Map_8k_RAM(7, 3);
        break;
    case MAPPER_ColecoVision: // COLECOVISION MAPPER ---------------------------
        Map_8k_Other(0, BIOS_ROM_Coleco);
        Map_8k_RAM(1, 1);
        Map_8k_RAM(2, 1);
        Map_8k_RAM(3, 0);
        Map_8k_ROM(4, 0);
        Map_8k_ROM(5, 1 & tsms.Pages_Mask_8k);
        Map_8k_ROM(6, 2 & tsms.Pages_Mask_8k);
        Map_8k_ROM(7, 3 & tsms.Pages_Mask_8k);
        break;
    case MAPPER_SMS_ActionReplay: // ACTION REPLAY MAPPER ----------------------
        Map_8k_ROM(0, 0);
        Map_8k_ROM(1, 1);
        Map_8k_RAM(2, 1);
        Map_8k_RAM(3, 1);
        Map_8k_RAM(4, 2); // FIXME
        Map_8k_RAM(5, 2);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        break;
    case MAPPER_SF7000: // SF-7000 ---------------------------------------------
        Map_16k_Other(0, BIOS_ROM_SF7000);
        Map_8k_RAM(2, 2);
        Map_8k_RAM(3, 3);
        Map_8k_RAM(4, 4);
        Map_8k_RAM(5, 5);
        Map_8k_RAM(6, 6);
        Map_8k_RAM(7, 7);
        break;
    case MAPPER_SMS_DisplayUnit: // DISPLAY UNIT MAPPER ----------------------
        Map_8k_ROM(0, 0);  // ROM
        Map_8k_ROM(1, 1);
        Map_8k_RAM(2, 1);  // RAM - Mapping 2k (0x800) from 4000 to BFFF
        Map_8k_RAM(3, 1);
        Map_8k_RAM(4, 1);
        Map_8k_RAM(5, 1);
        Map_8k_RAM(6, 0);  // RAM - Standard
        Map_8k_RAM(7, 0);
        break;
    case MAPPER_SMS_Korean_MSX_8KB_0003:
    case MAPPER_SMS_Korean_MSX_8KB_0300:
        Map_8k_ROM(0, 0);
        Map_8k_ROM(1, 1);
        Map_8k_ROM(2, 2);
        Map_8k_ROM(3, 3);
        Map_8k_ROM(4, 4);
        Map_8k_ROM(5, 5);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 4;

        // Nemesis has boot code on page 0x0F which seems to be auto-mapped at 0x0000-0x1fff on boot
        // I'm not sure what is really "correct" but this work and doesn't trigger on other Zemina games for now.
        if (g_machine.mapper == MAPPER_SMS_Korean_MSX_8KB_0003)
            if (tsms.Size_ROM == 16*0x2000 && ROM[0] == 0x00 && ROM[1] == 0x00 && ROM[2] == 0x00 && ROM[15*0x2000+0] == 0xF3 && ROM[15*0x2000+1] == 0xED && ROM[15*0x2000+2] == 0x56)
                Map_8k_ROM(0, 0x0F);
        break;
    case MAPPER_SMS_Korean_Janggun:
        Map_8k_ROM(0, 0);
        Map_8k_ROM(1, 1);
        Map_8k_ROM(2, 2);
        Map_8k_ROM(3, 3);
        Map_8k_ROM(4, 4);
        Map_8k_ROM(5, 5);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 6;
        g_machine.mapper_janggun_bytes_flipping_flags = 0x00;
        break;

    case MAPPER_SG1000_Taiwan_MSX_Adapter_TypeA:
        Map_8k_ROM(0, 0);
        Map_8k_RAM(1, 0);
        Map_8k_ROM(2, 2);
        Map_8k_ROM(3, 3);
        Map_8k_ROM(4, 4);
        Map_8k_ROM(5, 5);
        Map_8k_RAM(6, 1);
        Map_8k_RAM(7, 1);
        g_machine.mapper_regs_count = 0;
        break;

    case MAPPER_SMS_Korean_FFFF_HiCom:
        Map_8k_ROM(0, 0);
        Map_8k_ROM(1, 1 & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, 2 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 3 & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, 0);
        Map_8k_ROM(5, 1 & tsms.Pages_Mask_8k);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 1;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = 0;
        break;

    case MAPPER_SMS_Korean_2000_xor_1F:
        Map_8k_ROM(0, 0);
        Map_8k_ROM(1, 0);
        Map_8k_ROM(2, 0x60 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 0x61 & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, 0x62 & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, 0x63 & tsms.Pages_Mask_8k);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 1;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = 0;
        break;

    case MAPPER_SMS_Korean_FFFE:
        Map_8k_ROM(0, 0x00 & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, 0x01 & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, 0x02 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 0x03 & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, 0x3f & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, 0x3f & tsms.Pages_Mask_8k);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 1;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = (i == 0) ? 0x01 : 0;
        break;

    case MAPPER_SMS_Korean_FFF3_FFFC:
        Map_8k_ROM(0, 0 & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, 1 & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, 0 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 1 & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, 0xFF & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, 0xFF & tsms.Pages_Mask_8k);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 2;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = 0;
        break;

    case MAPPER_SMS_Korean_0000_xor_FF:
        Map_8k_ROM(0, 0 & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, 1 & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, 2 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 3 & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, 2 & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, 3 & tsms.Pages_Mask_8k);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 1;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = 0;
        g_machine.mapper_regs[0] = 0xFF;
        break;

    case MAPPER_SMS_Korean_MD_FFF0:
        Map_8k_ROM(0, 0x00 & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, 0x01 & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, 0x02 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 0x03 & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, 0x00 & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, 0x01 & tsms.Pages_Mask_8k);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 2;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = 0;
        g_machine.mapper_regs[2] = 1;
        break;

    case MAPPER_SMS_Korean_MD_FFF5:
        Map_8k_ROM(0, 0x00 & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, 0x01 & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, 0x02 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 0x03 & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, 0x02 & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, 0x03 & tsms.Pages_Mask_8k);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 2;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = 0;
        g_machine.mapper_regs[1] = 1;
        g_machine.mapper_regs[2] = 1;
        break;

    case MAPPER_SMS_Korean_MD_FFFA:
        Map_8k_ROM(0, 0x00 & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, 0x01 & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, 0x02 & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, 0x03 & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, 0x00 & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, 0x01 & tsms.Pages_Mask_8k);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        g_machine.mapper_regs_count = 2;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = 0;
        g_machine.mapper_regs[2] = 1;
        break;

    case MAPPER_SC3000_Survivors_Multicart:
        g_machine.mapper_regs_count = 1;
        for (int i = 0; i != MAPPER_REGS_MAX; i++)
            g_machine.mapper_regs[i] = 0;
        g_machine.mapper_regs[0] = 0xDF; // $11011111; // Menu
        //Map_8k_ROM(0, g_machine.mapper_regs[0]*4+0);
        //Map_8k_ROM(1, g_machine.mapper_regs[0]*4+1);// & tsms.Pages_Mask_8k);
        //Map_8k_ROM(2, g_machine.mapper_regs[0]*4+2);// & tsms.Pages_Mask_8k);
        //Map_8k_ROM(3, g_machine.mapper_regs[0]*4+3);// & tsms.Pages_Mask_8k);
        Map_8k_RAM(4, 0);
        Map_8k_RAM(5, 1);
        Map_8k_RAM(6, 2);
        Map_8k_RAM(7, 3);
        Out_SMS(0xE0, g_machine.mapper_regs[0]);
        break;

    default: // Other mappers
        Map_8k_ROM(0, 0);
        Map_8k_ROM(1, 1);
        Map_8k_ROM(2, 2);
        Map_8k_ROM(3, 3);
        Map_8k_ROM(4, 4);
        Map_8k_ROM(5, 5);
        Map_8k_RAM(6, 0);
        Map_8k_RAM(7, 0);
        switch (g_machine.mapper)
        {
        case MAPPER_SMS_NoMapper:
            break;
        case MAPPER_CodeMasters:
            // ROM [0x3FFF] = 0; ROM [0x7FFF] = 1; ROM [0xBFFF] = 2;
            break;
        case MAPPER_SMS_Korean_A000:
        case MAPPER_SMS_Korean_BFFC:
        case MAPPER_SMS_4PakAllAction:
            // ROM [0xA000] = 0;
            break;
        case MAPPER_93c46:
            // RAM [0x1FFC] = 0; RAM [0x1FFD] = 0; RAM [0x1FFE] = 1; RAM [0x1FFF] = 2;
            EEPROM_93c46_Init(EEPROM_93C46_INIT_ALL);
            break;
        case MAPPER_TVOekaki:
            TVOekaki_Init();
            break;
        default:
            // RAM [0x1FFC] = 0; RAM [0x1FFD] = 0; RAM [0x1FFE] = 1; RAM [0x1FFF] = 2;
            memcpy (Game_ROM_Computed_Page_0, ROM, 0x4000);
            //Map_16k_Other (0, Game_ROM_Computed_Page_0);
            Map_16k_ROM(0, 0);  // Mapping may change to Game_ROM_Computed_Page_0 at runtime
            break;
        }
        break;
    }
}

void    Machine_Set_Country(void)
{
    if (DB.current_entry && DB.current_entry->emu_country != -1)
        sms.Country = DB.current_entry->emu_country;
    else
        sms.Country = g_config.country;
}

void    Machine_Set_IPeriod(void)
{
    if (DB.current_entry && DB.current_entry->emu_iperiod != -1)
    {
        opt.Cur_IPeriod = DB.current_entry->emu_iperiod;
        return;
    }

    switch (g_driver->id)
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
void    Machine_Set_TV_Lines(void)
{
    if (DB.current_entry && DB.current_entry->emu_tvtype != -1)
        g_machine.TV = &TV_Type_Table [DB.current_entry->emu_tvtype];
    else
        g_machine.TV = TV_Type_User;
    g_machine.TV_lines = g_machine.TV->screen_lines;
}

// RESET EMULATED MACHINE -----------------------------------------------------
void        Machine_Reset(void)
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
        Msg(MSGT_DEBUG, "Machine_Reset();");
    #endif

    // Unpause machine if necessary
    if (g_machine_flags & MACHINE_PAUSED)
        Machine_Pause();

    // Set driver & machine stuff
    drv_set (g_machine.driver_id);

    Machine_Set_Mapper         ();
    if ((g_machine_flags & MACHINE_RUN) != 0 /*== MACHINE_RUN */)
        Machine_Set_Mapping    (); // ^^ FIXME: the test above isn't beautiful since MACHINE_RUN contains multiple flags, but I'm unsure which of them is actually needed to perform the correct test
    Machine_Set_Handler_IO     ();
    Machine_Set_Handler_MemRW  ();
    Machine_Set_Handler_Loop   ();
    Machine_Set_Country        ();
    Machine_Set_IPeriod        ();
    Machine_Set_TV_Lines       ();

    // VDP MODEL --------------------------------------------------------------
    if (DB.current_entry && DB.current_entry->emu_vdp_model != -1)
    {
        g_machine.VDP.model = DB.current_entry->emu_vdp_model;
    }
    else
    {
        if (g_driver->id == DRV_GG)
            g_machine.VDP.model = VDP_MODEL_315_5378;
        else
            g_machine.VDP.model = VDP_MODEL_315_5226;
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
    z80_init_memmap();
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
    z80_end_memmap();
    z80_reset();
#endif

    // MEMORY -----------------------------------------------------------------

    // Clear RAM
    if (g_driver->id == DRV_SMS && sms.Country == COUNTRY_JAPAN)
    {
        // On Japanese SMS clear RAM with 0xF0 patterns
        // I am not sure how reliable is that pattern but this is what I'm seeing on my JSMS
        // In theory this should be applied to all drivers, all countries, etc. but the exact
        // behavior of other systems and bioses should be inspected more in details before
        // generalizing that.
        // The game "Ali Baba" require a non-00 and non-FF memory pattern to run to a bug
        // in the code which makes it execute uninitialized memory from 0xFF07 onward.
        // An F0 pattern correspond to RET P which happens to fix the game.

        memset (RAM, 0xF0, 0x10000);
    }
    else
    {
        memset (RAM, 0x00, 0x10000);
    }
    memset (VRAM, 0, 0x04000);      // Clear all VRAM
    memset (PRAM, 0, 0x00040);      // Clear all PRAM (palette)

    #ifdef DEBUG_UNINITIALIZED_RAM_ACCESSES
        memset (RAM_IsUninitialized, 1, 0x2000);
    #endif

    // Unload BIOS if...
    if ((g_driver->id != DRV_SMS || sms.Country != COUNTRY_EXPORT) && (g_machine_flags & MACHINE_ROM_LOADED))
    {
        #ifdef DEBUG_WHOLE
            Msg(MSGT_DEBUG, "Machine_Reset(): BIOS_Unload()");
        #endif
        BIOS_Unload();
    }

    // GRAPHICS ---------------------------------------------------------------
    for (i = 0; i < 16; i++)
        Tms_VDP_Out (i, ((i == 1 && g_driver->id == DRV_COLECO) ? 0x00 : VDPInit [i]));
    for (i = 0; i < MAX_TILES; i++)
        tgfx.Tile_Dirty [i] = TILE_DIRTY_DECODE | TILE_DIRTY_REDRAW;

    VDP_UpdateLineLimits();

    //assert(!Screenbuffer_IsLocked());
    al_set_target_bitmap(screenbuffer_1);
    al_clear_to_color(COLOR_BLACK);
    al_set_target_bitmap(screenbuffer_2);
    al_clear_to_color(COLOR_BLACK);
    //screenbuffer = screenbuffer_1;
    //screenbuffer_next = screenbuffer_2;

    g_machine.VDP.sprite_shift_x = 0;
    g_machine.VDP.scroll_x_latched = 0;
    g_machine.VDP.scroll_y_latched = 0;
    memset(g_machine.VDP.scroll_x_latched_table, 0, sizeof(g_machine.VDP.scroll_x_latched_table));
    tsms.VDP_Video_Change = VDP_VIDEO_CHANGE_ALL;

    // GRAPHICS: SPRITE FLICKERING --------------------------------------------
    if (g_config.sprite_flickering & SPRITE_FLICKERING_AUTO)
    {
        if (DB.current_entry && (DB.current_entry->flags & DB_FLAG_EMU_SPRITE_FLICKER))
            g_config.sprite_flickering |= SPRITE_FLICKERING_ENABLED;
        else
            g_config.sprite_flickering &= ~SPRITE_FLICKERING_ENABLED;
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
    tsms.Port3F = 0;
    sms.Input_Mode = 0x07;

    // SOUND ------------------------------------------------------------------
    sms.FM_Register = 0;
    sms.FM_Magic = 0;
    // if (fm_use == TRUE) fm_init (FM_ALL_INIT);
    // resume_fm();
    FM_Reset();
    SN76489_Reset (g_machine.TV->CPU_clock, Sound.SampleRate);
    if (Sound.LogVGM.Logging == VGM_LOGGING_ACCURACY_SAMPLE)
        VGM_Update_Timing (&Sound.LogVGM);

    Sound_ResetCycleCounter();

    // FIXME: add a reset handler per driver, instead of the code below...

    // GAME GEAR COMMUNICATION PORT
    if (g_machine.driver_id == DRV_GG)
        Comm_Reset();

    // SF-7000
    if (g_machine.driver_id == DRV_SF7000)
        SF7000_Reset();

    // DEBUGGER ---------------------------------------------------------------
    #ifdef MEKA_Z80_DEBUGGER
        Debugger_MachineReset();
    #endif
}

//-----------------------------------------------------------------------------
