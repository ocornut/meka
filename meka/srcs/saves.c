//-----------------------------------------------------------------------------
// MEKA - saves.c
// Save States - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "bios.h"
#include "commport.h"
#include "lightgun.h"
#include "mappers.h"
#include "palette.h"
#include "saves.h"
#include "sf7000.h"
#include "tvoekaki.h"
#include "vdp.h"
#include "sound/fmunit.h"
#include "sound/psg.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Fix up emulator data after loading a save state
void        Load_Game_Fixup(void)
{
    int     i;
    u8      b;

    // CPU
    #ifdef MARAT_Z80
        sms.R.IPeriod = opt.Cur_IPeriod;
        /*
        #ifdef MEKA_Z80_DEBUGGER
        if (Debugger.Enabled && Debugger.break_point_set)
            sms.R.Trap = Debugger.break_point_address; // FIXME, argh
        #endif
        */
    #endif

    // Memory
    // FIXME: need to clean all those stuff.. it's messy
    //if (g_machine.driver_id != DRV_NES)
	{
        switch (g_machine.mapper)
        {
		case MAPPER_SMS_NoMapper:
			break;
        case MAPPER_Standard:
        case MAPPER_93c46:
            // We save previous RAM content, because:
            // The code could do: LD (FFFF), xx then LD (DFFF), yy
            // In this case, FCR[2]==xx but RAM[1FFF]==yy
            b = RAM[0x1FFC]; WrZ80_NoHook (0xFFFC, sms.SRAM_Mapping_Register); RAM[0x1FFC] = b;
            b = RAM[0x1FFD]; WrZ80_NoHook (0xFFFD, g_machine.mapper_regs[0]);  RAM[0x1FFD] = b;
            b = RAM[0x1FFE]; WrZ80_NoHook (0xFFFE, g_machine.mapper_regs[1]);  RAM[0x1FFE] = b;
            b = RAM[0x1FFF]; WrZ80_NoHook (0xFFFF, g_machine.mapper_regs[2]);  RAM[0x1FFF] = b;
            break;
		case MAPPER_SMS_Korean_MSX_8KB:
			WrZ80_NoHook (0x0000, g_machine.mapper_regs[0]);
			WrZ80_NoHook (0x0001, g_machine.mapper_regs[1]);
			WrZ80_NoHook (0x0002, g_machine.mapper_regs[2]);
			WrZ80_NoHook (0x0003, g_machine.mapper_regs[3]);
			break;
		case MAPPER_SMS_Korean_Janggun:
			WrZ80_NoHook (0xFFFE, g_machine.mapper_regs[0]);
			WrZ80_NoHook (0x4000, g_machine.mapper_regs[1]);
			WrZ80_NoHook (0x6000, g_machine.mapper_regs[2]);
			WrZ80_NoHook (0xFFFF, g_machine.mapper_regs[3]);
			WrZ80_NoHook (0x8000, g_machine.mapper_regs[4]);
			WrZ80_NoHook (0xA000, g_machine.mapper_regs[5]);
			break;
        case MAPPER_CodeMasters:
            WrZ80_NoHook (0x0000, g_machine.mapper_regs[0]);
            WrZ80_NoHook (0x4000, g_machine.mapper_regs[1]);
            WrZ80_NoHook (0x8000, g_machine.mapper_regs[2]);
            break;
		case MAPPER_SMS_4PakAllAction:
            WrZ80_NoHook (0x3FFE, g_machine.mapper_regs[0]);
            WrZ80_NoHook (0x7FFF, g_machine.mapper_regs[1]);
            WrZ80_NoHook (0xBFFF, g_machine.mapper_regs[2]);
            break;
        case MAPPER_SMS_Korean:
            WrZ80_NoHook (0xA000, g_machine.mapper_regs[2]);
            break;
        case MAPPER_ColecoVision:
            for (i = 0x0400; i < 0x2000; i += 0x400)
                memcpy (RAM + i, RAM, 0x0400);
            break;
        case MAPPER_SG1000:
            b = RAM[0x1FFD]; WrZ80_NoHook(0xFFFD, g_machine.mapper_regs[0]);     RAM[0x1FFD] = b;
            b = RAM[0x1FFE]; WrZ80_NoHook(0xFFFE, g_machine.mapper_regs[1]);     RAM[0x1FFE] = b;
            b = RAM[0x1FFF]; WrZ80_NoHook(0xFFFF, g_machine.mapper_regs[2]);     RAM[0x1FFF] = b;
            memcpy (RAM + 0x1000, RAM, 0x1000);
            break;
        case MAPPER_SF7000:
            SF7000_IPL_Mapping_Update();
            break;
		case MAPPER_SG1000_Taiwan_MSX_Adapter_TypeA:
			break;
		case MAPPER_SMS_Korean_Xin1:
			WrZ80_NoHook (0xFFFF, g_machine.mapper_regs[0]);
			break;
		case MAPPER_SC3000_Survivors_Multicart:
			Out_SMS(0xE0, g_machine.mapper_regs[0]);
			break;
        }
	}

    // VDP/Graphic related
    tsms.VDP_Video_Change |= VDP_VIDEO_CHANGE_ALL;
    VDP_UpdateLineLimits();
    // FALSE!!! // tsms.VDP_Line = 224;

    // Rewrite all VDP registers (we can do that since it has zero side-effect)
    for (i = 0; i < 16; i ++)
        Tms_VDP_Out (i, sms.VDP[i]);

    // Set all tiles as dirty
    for (i = 0; i < MAX_TILES; i++)
        tgfx.Tile_Dirty [i] = TILE_DIRTY_DECODE | TILE_DIRTY_REDRAW;

    // Reload palette
    Palette_Emulation_Reload();

    // Msg(MSGT_DEBUG, "ICount %d VDP Line %d", CPU_GetICount(), tsms.VDP_Line);
}

// Save current state / dispatch to format saving code
void        SaveState_Save()
{
    // Do not allow saving if machine is not running
    if ((g_machine_flags & MACHINE_RUN) != MACHINE_RUN)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_No_ROM));
        return;
    }

    // Do not allow saving in BIOS
    // This would be messy with file handling currently
    if ((g_machine_flags & MACHINE_NOT_IN_BIOS) == 0)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Save_Not_in_BIOS));
        return;
    }

	char buf[FILENAME_LEN+1];
    Save_Get_Filename(buf);
	FILE* f;
	int result;
    if (!(f = fopen(buf, "wb")))
        result = 2;
    else
    {
        result = Save_Game_MSV(f);
        fclose (f);
    }

    StrPath_RemoveDirectory (buf);
    switch (result)
    {
    case 1: Msg(MSGT_USER, Msg_Get(MSG_Save_Success), buf);
        break;
    case 2: Msg(MSGT_USER, Msg_Get(MSG_Save_Error), buf);
        break;
    }
}

// Load state from current slot
void        SaveState_Load()
{
    // Do not allow loading if machine is not running
    if ((g_machine_flags & MACHINE_RUN) != MACHINE_RUN)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_No_ROM));
        return;
    }

    // If loading while in BIOS, disable BIOS and switch to game
    if ((g_machine_flags & MACHINE_NOT_IN_BIOS) == 0)
    {
        // Note: I'm not sure why I saved the current VDP line here...
        const int line = tsms.VDP_Line;
        BIOS_Switch_to_Game();
        tsms.VDP_Line = line;
    }

    /*
    #ifdef MEKA_Z80_DEBUGGER
        if (Debugger.Enabled)
            Debugger.Break_Point = sms.R.Trap;
    #endif
    */

	char buf[FILENAME_LEN+1];
    Save_Get_Filename(buf);
	
	FILE* f;
	int result;
    if (!(f = fopen(buf, "rb")))
        result = 2;
    else
    {
        result = Load_Game_MSV (f);
        fclose (f);
    }

    StrPath_RemoveDirectory (buf);
    switch (result)
    {
    case 1: Msg(MSGT_USER, Msg_Get(MSG_Load_Success), buf);
        Load_Game_Fixup();
        break;
    case 2: Msg(MSGT_USER, Msg_Get(MSG_Load_Error), buf);         break;
    case 3: Msg(MSGT_USER, Msg_Get(MSG_Load_Not_Valid), buf);     break;
    case 4: Msg(MSGT_USER, Msg_Get(MSG_Load_Version), buf);       break;
    case 5: Msg(MSGT_USER, Msg_Get(MSG_Load_Wrong_System), buf);  break;
    }

    // Msg(MSGT_USER, "Debugger.Break_Point = %04X", Debugger.Break_Point);
    // Msg(MSGT_USER, "Loaded sms.R.Trap = %04X, %d", sms.R.Trap, sms.R.Trace);
}

// Save current state to given file, in MEKA save state format
int     Save_Game_MSV (FILE *f)
{
    u8  b;
    u16 w;

    BMemory_Verify_Usage();

    // Write Header
    fwrite ("MEKA", 4, 1, f);
    b = 0x1A;
    fwrite (&b, 1, 1, f);
    b = MEKA_SAVESTATE_VERSION;
    fwrite (&b, 1, 1, f);
    b = g_machine.driver_id;  // Do NOT save g_driver->id, as it may change with legacy video mode in the current code
    fwrite (&b, 1, 1, f);

    // Write CRC32, introduced in version 0x0C
    fwrite (&g_media_rom.crc32, sizeof (u32), 1, f);

    // Write 'sms' structure (misc stuff)
	sms.R.R = (sms.R.R & 0x7F) | (sms.R.R7);
	sms.R.R7 = 0;
    fwrite (&sms, sizeof (struct SMS_TYPE), 1, f);
	sms.R.R7 = (sms.R.R & 0x80);

	const int mappers_regs_to_save = (g_machine.mapper_regs_count <= 4) ? 4 : g_machine.mapper_regs_count;
	fwrite (&g_machine.mapper_regs[0], sizeof(u8), mappers_regs_to_save, f);

    // Write VDP scanline counter
    w = tsms.VDP_Line;
    fwrite (&w, sizeof (u16), 1, f);

    // Write RAM & mapper specific data
    // FIXME: RAM size should be based on value got from current driver
    switch (g_machine.mapper)
    {
    case MAPPER_32kRAM:
	case MAPPER_SC3000_Survivors_Multicart:
        fwrite (RAM, 0x08000, 1, f);
        break;
    case MAPPER_ColecoVision:
        fwrite (RAM, 0x00400, 1, f);
        break;
    case MAPPER_SG1000:
        fwrite (RAM, 0x01000, 1, f);
        break;
    case MAPPER_TVOekaki:
        fwrite (RAM, 0x01000, 1, f);
        fwrite (&TVOekaki, sizeof (TVOekaki), 1, f);
        break;
    case MAPPER_SF7000:
        fwrite (RAM, 0x10000, 1, f);
        fwrite (&SF7000, sizeof (SF7000), 1, f);
        break;
    case MAPPER_CodeMasters:
        if (sms.SRAM_Mapping_Register & ONBOARD_RAM_EXIST) // Ernie Els Golf Onboard RAM
            fwrite (RAM, 0x2000 + 0x2000, 1, f);
        else
            fwrite (RAM, 0x2000, 1, f);
        break;
	case MAPPER_SG1000_Taiwan_MSX_Adapter_TypeA:
		fwrite (RAM, 0x02000, 1, f);
		fwrite (RAM + 0x2000, 0x00800, 1, f);
		break;
	case MAPPER_SMS_NoMapper:
	case MAPPER_SMS_Korean_MSX_8KB:
	case MAPPER_SMS_Korean_Janggun:
	case MAPPER_SMS_Korean_Xin1:
    default:                  
        fwrite (RAM, 0x2000, 1, f); // Do not use g_driver->ram because of g_driver video mode change
        break;
    }
    
    // Write VRAM
    fwrite (VRAM, 0x4000, 1, f);
    
    // Write Palette
    switch (g_machine.driver_id)
    {
    case DRV_SMS:
        fwrite (PRAM, 32, 1, f);
        break;
    case DRV_GG:
        fwrite (PRAM, 64, 1, f);
        fwrite (&Gear_to_Gear, sizeof (t_gear_to_gear), 1, f);
        break;
    }

    // Write PSG & FM state
    PSG_Save (f);
    FM_Save (f);

    // Write Port 3F value
    fwrite (&tsms.Port3F, 1, 1, f);

    // Write backed memory (SRAM, 93c46 EEPROM, etc.)
    BMemory_Save_State (f);
 
    // Write "EOF" characters
    fwrite ("EOF", 3, 1, f);

    return (1);
}

// Load state from given file, in MEKA save state format
int         Load_Game_MSV(FILE *f)
{
    char    buf[5];
    u8      b, version;

    fread (&buf, 4, 1, f);
    buf[4] = 0;
    if (strcmp(buf, "MEKA") != 0)
    {
        if (strcmp(buf, "S8B ") == 0)
            return (Load_Game_MSD (f));
        return (3); // not a savegame
    }
    fseek (f, 5, SEEK_SET);
    fread (&version, 1, 1, f);
    if (version > MEKA_SAVESTATE_VERSION)
        return (4); // unsupported version

    // Msg(MSGT_DEBUG, "Loading, version = %02X", version);

    // Read driver id
    fread (&b, 1, 1, f);
    if (version >= 0x05)
    {
        // Check if we are running on the same machine
        if (b != g_machine.driver_id)
            return (5);
    }
    else
    {
        // Old kind of machine identification
        if (b != drv_id_to_mode (g_machine.driver_id))
            return (5); // not the same machine
    }

    // CRC32, introduced in version 0x0C. Currently skip this value.
    if (version >= 0x0C)
    {
        u32 crc;
        fread (&crc, sizeof (u32), 1, f);
    }

    // 'sms' structure (including CPU stuff)
    {
        // Backup then restore debugging purpose data
        // So they're not lost when loading a state while debugging
        u16 trap = sms.R.Trap;
        u8  trace = sms.R.Trace;
        fread (&sms, sizeof(struct SMS_TYPE), 1, f);
		sms.R.R7 = (sms.R.R & 0x80);
        sms.R.Trap = trap;
        sms.R.Trace = trace;
    }

	if (version >= 0x0E)
	{
		// Always save at least 4 so that legacy software can readily bump up version and read data for most games (apart from those using mappers with >4 registers)
		const int mappers_regs_to_save = (g_machine.mapper_regs_count <= 4) ? 4 : g_machine.mapper_regs_count;
		fread(&g_machine.mapper_regs[0], sizeof(u8), mappers_regs_to_save, f);
	}
	else
	{
		// Old structure which was 3+1 (due to struct alignment)
		fread(&g_machine.mapper_regs[0], sizeof(u8), 3+1, f);
		for (int i = 3; i != MAPPER_REGS_MAX; i++)				// 4th value was padding so we also invalid it.
			g_machine.mapper_regs[i] = 0;
		if (g_machine.mapper == MAPPER_SMS_Korean_MSX_8KB)
		{
			u8 r[3];
			r[0] = g_machine.mapper_regs[0];
			r[1] = g_machine.mapper_regs[1];
			r[2] = g_machine.mapper_regs[2];
			g_machine.mapper_regs[0] = (r[2] & 0x0F);
			g_machine.mapper_regs[1] = (r[2] & 0xF0) >> 4;
			g_machine.mapper_regs[2] = (r[1] & 0x0F);
			g_machine.mapper_regs[3] = (r[1] & 0xF0) >> 4;
		}
	}

    // VDP scanline counter
    if (version >= 0x0D)
    {
        u16 w;
        fread (&w, sizeof (u16), 1, f);
        tsms.VDP_Line = w;
    }
    else
    {
        // Leave as is now. Likely to be the same as same game/driver is running.
    }

    // Read RAM & mapper specific data
    // FIXME: RAM size should be based on value got from current driver
    switch (g_machine.mapper)
    {
    case MAPPER_32kRAM:
	case MAPPER_SC3000_Survivors_Multicart:
        fread (RAM, 0x08000, 1, f);
        break;
    case MAPPER_ColecoVision:
        if (version >= 0x06)
            fread (RAM, 0x00400, 1, f);
        else
            fread (RAM, 0x02000, 1, f); // Previously saved 8 Kb instead of 1 kb
        break;
    case MAPPER_SG1000:
        if (version >= 0x06)
            fread (RAM, 0x01000, 1, f);
        else
            fread (RAM, 0x02000, 1, f); // Previously saved 8 Kb instead of 4 Kb
        break;
    case MAPPER_TVOekaki:
        fread (RAM, 0x01000, 1, f);
        fread (&TVOekaki, sizeof (TVOekaki), 1, f);
        break;
    case MAPPER_SF7000:
        fread (RAM, 0x10000, 1, f);
        fread (&SF7000, sizeof (SF7000), 1, f);
        break;
    case MAPPER_CodeMasters:
        if (sms.SRAM_Mapping_Register & ONBOARD_RAM_EXIST) // Ernie Els Golf Onboard RAM
            fread (RAM, 0x2000 + 0x2000, 1, f);
        else
            fread (RAM, 0x2000, 1, f);
        break;
	case MAPPER_SG1000_Taiwan_MSX_Adapter_TypeA:
		fread (RAM, 0x2000, 1, f);
		fread (RAM + 0x2000, 0x0800, 1, f);
		break;
	case MAPPER_SMS_NoMapper:
	case MAPPER_SMS_Korean_MSX_8KB:
	case MAPPER_SMS_Korean_Janggun:
	case MAPPER_SMS_Korean_Xin1:
    default:
        fread (RAM, 0x2000, 1, f); // Do not use g_driver->ram because of g_driver video mode change
        break;
    }

    // Read VRAM
    fread (VRAM, 0x4000, 1, f);

    // Read Palette
    // This depend on g_machine.driver_id and NOT on g_driver->id
    // Eg: even if palette is unused due to legacy mode set, we save CRAM/PRAM on a SMS
    switch (g_machine.driver_id)
    {
    case DRV_SMS:
        fread (PRAM, 32, 1, f);
        break;
    case DRV_GG:
        fread (PRAM, 64, 1, f);
        fread (&Gear_to_Gear, sizeof (t_gear_to_gear), 1, f);
        break;
    }

    // Read PSG data
    if (version >= 0x08)
        PSG_Load (f, version);
    else
    {
        // ...If not available, mute PSG volumes by default
        // SN76489_Reset (opt.CPU_Clock_Current, audio_sample_rate);
        PSG_Mute();
        // There is a reason for muting the PSG volumes instead of reseting it all
        // when the PSG data is missing: a same game is more likely to have noises 
        // setup the same way, so we want to keep current settings.
    }

    // Read FM data
    if (version >= 0x03)
        FM_Load(f);

    // Read port 3F value
    if (version >= 0x04)
        fread (&tsms.Port3F, 1, 1, f);

    // Previously stored 16 kb pages instead of 8 kb pages
    if (version < 0x07)
        sms.SRAM_Pages *= 2;

    // Read backed memory unless version<0x09 & emulation 93c46
    if (!(version < 0x09 && g_machine.mapper == MAPPER_93c46))
        BMemory_Load_State(f);

    // Fix up CPU registers
    // Copy IFF2 bit for old Z80 Registers structure
    if (version < 0x0A)
        sms.R.IFF = ((sms.R.IFF & 0xB7) | ((sms.R.IFF & 0x40) ? IFF_2 : 0x00));

    return (1);
}

// LOAD CURRENT GAME IN MASSAGE FORMAT ----------------------------------------
int     Load_Game_MSD (FILE *f)
{
    word   w;
    byte   i, version;

    Msg(MSGT_USER, "%s", Msg_Get(MSG_Load_Massage));
    /* Skipping "SNAP" */
    fseek (f, 0x09, SEEK_SET);
    fread (&version, 1, 1, f);
    if (version > 0)
        return (4); // not the same version, only version 0 defined and supported
    fread (&i, 1, 1, f);
    // Massage 0: SMS, 1: GG, which map to our define
    if (i != 0 && i != 1)
        return (4); // We only know about SMS/GG massage savestates
    if (i != g_machine.driver_id)
        return (5); // not the same machine
    fread (&sms.Country, 1, 1, f);
    /* Skipping FM enable/disable */
    /* Skipping ports 0x00 to 0x06 */
    // FIXME: read those port
    /* Skipping ports 0x3E and 0x3F */
    /* Skipping PSG frequencies/volume */
    /* Skipping PSG latch/data bytes */
    // FIXME: read PSG stuff
    fseek (f, 0x28, SEEK_SET);
    fread (&sms.VDP [0], 11, 1, f);
    fread (&sms.VDP_Address, 2, 1, f);
    fread (&sms.VDP_Access_Mode, 1, 1, f);
    fread (&sms.VDP_Access_First, 1, 1, f);
    fread (&w, 2, 1, f); /* Current Video Scanline */
    tsms.VDP_Line = w;
    fread (&sms.Lines_Left, 1, 1, f);
    fread (&sms.Input_Mode, 1, 1, f); /* Port 0xDE */
    fread (&sms.FM_Register, 1, 1, f);
    fread (&sms.FM_Magic, 1, 1, f);
    FM_Load (f);
    fread (&sms.SRAM_Mapping_Register, 1, 1, f);
    fread (&g_machine.mapper_regs[0], 3, 1, f);
	g_machine.mapper_regs[3] = 0;
    fread (RAM, 0x2000, 1, f);
    fread (VRAM, 0x4000, 1, f);
    switch (g_machine.driver_id)
    {
    case DRV_SMS:
        fread (PRAM, 32, 1, f);
        break;
    case DRV_GG:
        fread (PRAM, 64, 1, f);
        break;
    }
    fread (SRAM, 0x8000, 1, f);
#ifdef MARAT_Z80
    fread (&sms.R.AF.B.l, 1, 1, f);
    fread (&sms.R.AF.B.h, 1, 1, f);
    fread (&sms.R.BC.B.l, 1, 1, f);
    fread (&sms.R.BC.B.h, 1, 1, f);
    fread (&sms.R.HL.B.l, 1, 1, f);
    fread (&sms.R.HL.B.h, 1, 1, f);
    fread (&sms.R.PC.B.l, 1, 1, f);
    fread (&sms.R.PC.B.h, 1, 1, f);
    fread (&sms.R.SP.B.l, 1, 1, f);
    fread (&sms.R.SP.B.h, 1, 1, f);
    fread (&sms.R.I, 1, 1, f);
    fread (&sms.R.R, 1, 1, f);
	sms.R.R7 = sms.R.R & 0x80;
    fread (&sms.R.DE.B.l, 1, 1, f);
    fread (&sms.R.DE.B.h, 1, 1, f);
    fread (&sms.R.BC1.B.l, 1, 1, f);
    fread (&sms.R.BC1.B.h, 1, 1, f);
    fread (&sms.R.DE1.B.l, 1, 1, f);
    fread (&sms.R.DE1.B.h, 1, 1, f);
    fread (&sms.R.HL1.B.l, 1, 1, f);
    fread (&sms.R.HL1.B.h, 1, 1, f);
    fread (&sms.R.AF1.B.l, 1, 1, f);
    fread (&sms.R.AF1.B.h, 1, 1, f);
    fread (&sms.R.IY.B.l, 1, 1, f);
    fread (&sms.R.IY.B.h, 1, 1, f);
    fread (&sms.R.IX.B.l, 1, 1, f);
    fread (&sms.R.IX.B.h, 1, 1, f);
    { // FIXME: should verify
        byte b[3];
        fread (b, 3, 1, f);
        sms.R.IFF = b[0];
        sms.R.IFF = (sms.R.IFF & 0xF9);
        if (b[2] == 1) sms.R.IFF |= 2;
        else
            if (b[2] == 2) sms.R.IFF |= 4;
    }
#else
#error "Load_Game_MSD() needs MARAT_Z80 core to load registers!"
#endif
    return (1);
}

void    SaveState_SetSlot(int n)
{
    if (n < 0)  
        n = 0;
    if (n > 99) 
        n = 99;
    Msg(MSGT_USER, Msg_Get(MSG_Save_Slot), n);
    opt.State_Current = n;
}

void	SaveState_SetPrevSlot()
{
	SaveState_SetSlot(opt.State_Current - 1);
}

void	SaveState_SetNextSlot()
{
	SaveState_SetSlot(opt.State_Current + 1);
}

// RETURN FILENAME OF CURRENT SAVEGAME ----------------------------------------
void    Save_Get_Filename (char *str)
{
    char buf [FILENAME_LEN];

    // Create save state directory if it doesn't exist already
    if (!al_filename_exists(g_env.Paths.SavegameDirectory))
        al_make_directory(g_env.Paths.SavegameDirectory);

    // Compute save state filename
    strcpy(buf, g_env.Paths.MediaImageFile);
    StrPath_RemoveExtension (buf);
    StrPath_RemoveDirectory (buf);
    sprintf(str, "%s/%s.S%02d", g_env.Paths.SavegameDirectory, buf, opt.State_Current);
}

//-----------------------------------------------------------------------------

