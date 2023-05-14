//-----------------------------------------------------------------------------
// MEKA - mappers.c
// Memory Mapper Emulation - Code
//-----------------------------------------------------------------------------
// FIXME: Need to refactor this whole file. Some things we want:
// - Create proper named mapper-id and try to standardize with community.
// - Gather all infos and code about a mapper in a central place (grep for MAPPER_* 
//   usage in code, mainly the switches in machine.c and saves.c)
// - Generally clean up implementation.
//-----------------------------------------------------------------------------

//#define DEBUG_MEM
//#define DEBUG_PAGES
#include "shared.h"
#include "mappers.h"
#include "eeprom.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

#ifdef DEBUG_UNINITIALIZED_RAM_ACCESSES
u8 RAM_IsUninitialized[0x2000];
#endif

//-----------------------------------------------------------------------------
// Macros / Inline functions
//-----------------------------------------------------------------------------

void Map_8k_Other (int page, void *data)
{ 
    Mem_Pages [page] = (u8 *)data - (page * 0x2000); 
}

void Map_16k_Other (int page, void *data)
{ 
    Mem_Pages [page] = Mem_Pages [page + 1] = (u8*)data - (page * 0x2000);
}

void Map_8k_RAM (int page, int ram_page)
{
    Mem_Pages [page] = RAM + ((ram_page - page) * 0x2000);
}

void Map_8k_ROM (int page, int rom_page)
{
    Mem_Pages [page] = ROM + ((rom_page - page) * 0x2000); 
}

void Map_16k_ROM (int page, int rom_page)
{
    Mem_Pages [page] = Mem_Pages [page + 1] = ROM + ((rom_page - page) * 0x2000);
}

#ifdef DEBUG_MEM
void    Write_Error (int Addr, u8 Value)
{
    Msg(MSGT_DEBUG, Msg_Get(MSG_Debug_Trap_Write), sms.R.PC.W, Value, Addr);
}
#endif

//-----------------------------------------------------------------------------

static u8   Mapper_SMS_Korean_Janggun_BitReverseLUT[256];

void    Mapper_InitializeLookupTables()
{
    for (int i = 0; i != 256; i++)
    {
        int reversed = 0;
        for (int b = 0; b != 8; b++)
            if (i & (1<<b))
                reversed |= (1<<(7-b));
        Mapper_SMS_Korean_Janggun_BitReverseLUT[i] = reversed;
    }
}

void    Out_SC3000_SurvivorsMulticarts_DataWrite(u8 v)
{
    // Control Byte format
    // Q0..Q4 controls the A15 to A19 address lines on the EPROMs.  
    //        So these 5 bits let you select a 32KB logical block within one of the EPROMs.  Q0 => A15, Q1 => A16, Q2 => A17, Q3 => A18, Q4 => A19
    // Q5     is not connected to anything
    // Q6     ROM 0 / ROM 1 select (0 =>ROM 0, 1 => ROM 1)
    // Q7     Enables / Disables latch
    //        If the latch is disabled, then pull up resistors ensure that block 31 in ROM 1 is selected
    g_machine.mapper_regs[0] = v;
    int game_no = (v & 0x80) ? ((v & 0x1f) | ((v & 0x40) ? 0x20 : 0x00)) : 0x3F;

    Map_8k_ROM(0, game_no*4+0);
    Map_8k_ROM(1, game_no*4+1);
    Map_8k_ROM(2, game_no*4+2);
    Map_8k_ROM(3, game_no*4+3);
}

void    Mapper_Get_RAM_Infos(int *plen, int *pstart_addr)
{
    int len, start_addr;

    switch (g_machine.mapper)
    {
        case MAPPER_32kRAM:                         len = 0x08000; start_addr = 0x8000; break;
        case MAPPER_ColecoVision:                   len = 0x00400; start_addr = 0x6000; break;
        case MAPPER_SG1000:                         len = 0x01000; start_addr = 0xC000; break;
        case MAPPER_TVOekaki:                       len = 0x01000; start_addr = 0xC000; break;
        case MAPPER_SF7000:                         len = 0x10000; start_addr = 0x0000; break;
        case MAPPER_SMS_DisplayUnit:                len = 0x02800; start_addr = 0x4000; break; // FIXME: Incorrect, due to scattered mapping!
        case MAPPER_SG1000_Taiwan_MSX_Adapter_TypeA:len = 0x02000+0x800; start_addr = 0x2000; break; // FIXME: Two memory regions
        case MAPPER_SC3000_Survivors_Multicart:     len = 0x08000; start_addr = 0x8000; break;
        // FIXME: ActionReplay!!
        // default, Codemaster, Korean..
        default:                                len = 0x02000; start_addr = 0xC000; break;
    }
    if (plen)
        *plen = len;
    if (pstart_addr)
        *pstart_addr = start_addr;
}

// Return -1 if can't tell, else a mapper number
int         Mapper_Autodetect(void)
{
    // If ROM is smaller than 32KB, auto-detected SMS mapper are not needed
    if (tsms.Size_ROM <= 0x8000)
        return (-1);

    // Search for code to access mapper -> LD (8000)|(FFFF), A
    int c0002 = 0, c2000 = 0, c8000 = 0, cA000 = 0, cBFFC = 0, cBFFF = 0, cFFFF = 0;
    int c0000 = 0, c0100 = 0, c0200 = 0, c0300 = 0;
    for (int i = 0; i < 0x8000; i++)
    {
        if (ROM[i] == 0x32) // Z80 opcode for: LD (xxxx), A
        {
            const u16 addr = *(u16 *)&ROM [i + 1];
            if (addr == 0xFFFF)
            { i += 2; cFFFF++; continue; }
            if (addr == 0x0002 || addr == 0x0003 || addr == 0x0004)
            { i += 2; c0002++; continue; }
            if (addr == 0x2000)
            { i += 2; c2000++; continue; }
            if (addr == 0x8000)
            { i += 2; c8000++; continue; }
            if (addr == 0xA000)
            { i += 2; cA000++; continue; }
            if (addr == 0xBFFF)
            { i += 2; cBFFF++; continue; }

            // MAPPER_SMS_Korean_MSX_8KB_0300
            if (addr == 0x0000) { i += 2; c0000++; continue; }
            if (addr == 0x0100) { i += 2; c0100++; continue; }
            if (addr == 0x0200) { i += 2; c0200++; continue; }
            if (addr == 0x0300) { i += 2; c0300++; continue; }
        }

        if (ROM[i] == 0x21) // Z80 opcode for LD HL,XXXX
        {
            const u16 addr = *(u16*)&ROM[i + 1];

            // Super Game 200 - 200 Hap ~ Super Game (KR)
            if (addr == 0xBFFC)
                if (ROM[i + 3] == 0x3A) // LD A,XXX
                    if (ROM[i + 6] == 0x77) // LD (HL), A
                        cBFFC++;
        }
    }

    //Msg(MSGT_USER, "c002=%d, c8000=%d, cA000=%d, cBFFF=%d, cFFFF=%d\n", c0002, c8000, cA000, cBFFF, cFFFF);

    // 4 Pak All Action "games"
    // this is not strictly necessary, since the full 4 Pak All Action is in meka.nam, but this allows extracted games to run standalone
    // NB: twin mouse has a false 0x0002 matching so it must be tested before
    if (cBFFF > 0 && /*c0002 == 0 &&*/ c8000 == 0 && cA000 == 0 && cFFFF == 0)
        return (MAPPER_SMS_4PakAllAction);

    // 2 is a security measure, although tests on existing ROM showed it was not needed
    if (c0000 >= 1 && c0100 >= 1 && c0200 >= 1 && c0300 >= 1 && (c0000 + c0100 + c0200 + c0300 > cFFFF)) // Need to be BEFORE MAPPER_SMS_Korean_MSX_8KB_0003 for 30-in-1
        return (MAPPER_SMS_Korean_MSX_8KB_0300);
    if (c0002 > cFFFF + 2 || (c0002 > 0 && cFFFF == 0))
        return (MAPPER_SMS_Korean_MSX_8KB_0003);
    if (c2000 >= 2 && cFFFF == 0)
        return (MAPPER_SMS_Korean_2000_xor_1F);
    if (c8000 > cFFFF + 2 || (c8000 > 0 && cFFFF == 0))
        return (MAPPER_CodeMasters);
    if (cBFFC >= 1 && cFFFF == 0)
        return (MAPPER_SMS_Korean_BFFC);
    if (cA000 > cFFFF + 2 || (cA000 > 0 && cFFFF == 0))
        return (MAPPER_SMS_Korean_A000);

    return (MAPPER_Auto);
}

WRITE_FUNC (Write_Default)
{
    if ((Addr & 0xFFF8) == 0xFFF8)
    {
        switch (Addr & 0x7)
        {
        case 4: // 0xFFFC: SRAM Register ---------------------------------------------
            RAM [0x1FFC] = sms.SRAM_Mapping_Register = Value;
            if (SRAM_Active)
            {
                if (SRAM_Page)
                {
                    sms.SRAM_Pages = 4; // 4 x 8 KB
                    Map_8k_Other (4, &SRAM [0x4000]);
                }
                else
                {
                    if (sms.SRAM_Pages < 2) 
                        sms.SRAM_Pages = 2; // 2 x 8 kB
                    Map_8k_Other (4, &SRAM [0x0000]);
                }
            }
            else
            {
                Map_8k_ROM (4, g_machine.mapper_regs[2] * 2);
            }
            // FIXME: Use Map_16k* above instead of this.
            Mem_Pages [5] = Mem_Pages [4];
            return;
        case 5: // 0xFFFD: Frame 0 ---------------------------------------------------
#ifdef DEBUG_PAGES
            if (Value != 0)
            { Msg(MSGT_DEBUG, "At PC=%04X: Frame 0 set to page %d !", CPU_GetPC, Value); }
#endif
            Value &= tsms.Pages_Mask_16k;
            if (g_machine.mapper_regs[0] != Value)
            {
                RAM [0x1FFD] = g_machine.mapper_regs[0] = Value;
                if (Value != 0)
                {
                    Map_16k_Other (0, Game_ROM_Computed_Page_0);
                    memcpy (Game_ROM_Computed_Page_0 + 0x400, ROM + (Value << 14) + 0x400, 0x3C00);
                }
                else
                {
                    Map_16k_ROM (0, 0);
                }
                return;
        case 6: // 0xFFFE: Frame 1 ---------------------------------------------------
#ifdef DEBUG_PAGES
            if (Value > tsms.Pages_Count_16k)
            { Msg(MSGT_DEBUG, "At PC=%04X: Frame 1 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
            RAM [0x1FFE] = g_machine.mapper_regs[1] = Value & tsms.Pages_Mask_16k;
            Map_16k_ROM (2, g_machine.mapper_regs[1] * 2);
            return;
        case 7: // 0xFFFF: Frame 2 ---------------------------------------------------
#ifdef DEBUG_PAGES
            if (Value > tsms.Pages_Count_16k)
            { Msg(MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
            RAM [0x1FFF] = g_machine.mapper_regs[2] = Value & tsms.Pages_Mask_16k;
            if (!SRAM_Active)
            {
                Map_16k_ROM (4, g_machine.mapper_regs[2] * 2);
            }
            return;
        default: // 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB: Glasse Register ------------------
            Mem_Pages [7] [Addr] = sms.Glasses_Register = Value;
            return;
            }
        }
    }

#ifdef DEBUG_UNINITIALIZED_RAM_ACCESSES
    if (Addr >= 0xC000 && Addr <= 0xFFFF)
        RAM_IsUninitialized[Addr&0x1FFF] = 0;
#endif

    // RAM -----------------------------------------------------------------------
    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; return;
        // SaveRAM [0x8000]->[0xC000] ---------------------------------------------
    case 4: if (SRAM_Active) { Mem_Pages [4] [Addr] = Value; return; } break;
    case 5: if (SRAM_Active) { Mem_Pages [5] [Addr] = Value; return; } break;
    }

    Write_Error (Addr, Value);
}

// [MAPPER: 32K RAM/SC-3000] WRITE BYTE ---------------------------------------
WRITE_FUNC (Write_Mapper_32kRAM)
{
    const unsigned int page = (Addr >> 13);
    if (page >= 4)
    {
        Mem_Pages[page][Addr] = Value; return;
    }
}

WRITE_FUNC (Write_Mapper_SMS_NoMapper)
{
#ifdef DEBUG_UNINITIALIZED_RAM_ACCESSES
    if (Addr >= 0xC000 && Addr <= 0xFFFF)
        RAM_IsUninitialized[Addr&0x1FFF] = 0;
#endif

    // RAM -----------------------------------------------------------------------
    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; if (Addr >= 0xFFF8 && Addr <= 0xFFFB) sms.Glasses_Register = Value; return;
    }

    Write_Error (Addr, Value);
}

// FIXME: Amount of RAM is totally incorrect.
WRITE_FUNC (Write_Mapper_SG1000)
{
    switch (Addr)
    {
    case 0xFFFD: // Frame 0 ------------------------------------------------------
#ifdef DEBUG_PAGES
        if (Value != 0)
        { Msg(MSGT_DEBUG, "At PC=%04X: Frame 0 set to page %d !", CPU_GetPC, Value); }
#endif
        Value &= tsms.Pages_Mask_16k;
        if (g_machine.mapper_regs[0] != Value)
        {
            RAM [0x1FFD] = g_machine.mapper_regs[0] = Value;
            if (Value != 0)
            {
                Map_16k_Other (0, Game_ROM_Computed_Page_0);
                memcpy (Game_ROM_Computed_Page_0 + 0x400, ROM + (Value << 14) + 0x400, 0x3C00);
            }
            else
            {
                Map_16k_ROM (0, 0);
            }
        }
        return;
    case 0xFFFE: // Frame 1 ------------------------------------------------------
#ifdef DEBUG_PAGES
        if (Value > tsms.Pages_Count_16k)
        { Msg(MSGT_DEBUG, "At PC=%04X: Frame 1 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
        RAM [0x1FFE] = g_machine.mapper_regs[1] = Value & tsms.Pages_Mask_16k;
        Map_16k_ROM(2, g_machine.mapper_regs[1] * 2);
        return;
    case 0xFFFF: // Frame 2 ------------------------------------------------------
#ifdef DEBUG_PAGES
        if (Value > tsms.Pages_Count_16k)
        { Msg(MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
        RAM [0x1FFF] = g_machine.mapper_regs[2] = Value & tsms.Pages_Mask_16k;
        Map_16k_ROM (4, g_machine.mapper_regs[2] * 2);
        return;
    }

#ifdef DEBUG_UNINITIALIZED_RAM_ACCESSES
    if (Addr >= 0xC000 && Addr <= 0xFFFF)
        RAM_IsUninitialized[Addr&0x1FFF] = 0;
#endif

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] -----------------------------------------------
    case 6: Mem_Pages [6] [Addr & ~0x1000] = Mem_Pages [6] [Addr | 0x1000] = Value; return;
    case 7: Mem_Pages [7] [Addr & ~0x1000] = Mem_Pages [7] [Addr | 0x1000] = Value; return;
    }

    Write_Error (Addr, Value);
}

WRITE_FUNC (Write_Mapper_SG1000_Taiwan_MSX_Adapter_TypeA)
{
    // RAM 
    switch (Addr >> 13)
    {
    case 1: Mem_Pages [1] [Addr] = Value; return;
    case 6: Mem_Pages [6] [Addr & ~0x1800] = Value; return;
    case 7: Mem_Pages [7] [Addr & ~0x1800] = Value; return;
    }

    Write_Error (Addr, Value);
}

WRITE_FUNC (Write_Mapper_CodeMasters)
{
    switch (Addr)
    {
    case 0x0000: // Frame 0 ----------------------------------------------------
#ifdef DEBUG_PAGES
        //if (Value > tsms.Pages_Count_16k)
        { Msg(MSGT_DEBUG, "At PC=%04X: Frame 0 set to non-existent page: %d", CPU_GetPC, Value);}
#endif
        Value = (Value & tsms.Pages_Mask_16k);
        /*ROM [0x0000] = */ g_machine.mapper_regs[0] = Value;
        Map_16k_ROM (0, g_machine.mapper_regs[0] * 2);
        return;
    case 0x4000: // Frame 1 ----------------------------------------------------
#ifdef DEBUG_PAGES
        //if (Value > tsms.Pages_Count_16k)
        { Msg(MSGT_DEBUG, "At PC=%04X: Frame 1 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
        if (Value & 0x80) // OnBoard RAM for Ernie Els Golf
        {
            sms.SRAM_Mapping_Register = ONBOARD_RAM_EXIST | ONBOARD_RAM_ACTIVE;
            Map_8k_RAM(5, 1); // Mapped from 0xA000 to 0xC000 only! (0x8000 to 0xA000 has ROM!)
        }
        else
        {
            if (sms.SRAM_Mapping_Register & ONBOARD_RAM_ACTIVE)
            {
                // Map Page 2 back if we just disabled On Board RAM
                Map_8k_ROM(5, g_machine.mapper_regs[2] * 2 + 1);
            }
            sms.SRAM_Mapping_Register &= ~ONBOARD_RAM_ACTIVE;
            Value = (Value & tsms.Pages_Mask_16k);
            /* ROM [0x4000] = */ g_machine.mapper_regs[1] = Value;
            Map_16k_ROM(2, g_machine.mapper_regs[1] * 2);
        }
        return;
    case 0x8000: // Frame 2 ----------------------------------------------------
#ifdef DEBUG_PAGES
        //if (Value > tsms.Pages_Count_16k)
        { Msg(MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
        Value = (Value & tsms.Pages_Mask_16k);
        /* ROM [0x8000] = */ /* ROM[0xBFFF] = */ g_machine.mapper_regs[2] = Value;
        Map_16k_ROM(4, g_machine.mapper_regs[2] * 2);
        return;
    }

    switch (Addr >> 13)
    {
        // On Board RAM [0xA000]->[0xC000] ----------------------------------------
        // (for Ernie Els Golf)
    case 5: if (sms.SRAM_Mapping_Register & ONBOARD_RAM_ACTIVE) { Mem_Pages [5] [Addr] = Value; return; } break;
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; return;
        // SaveRAM [0x8000]->[0xC000] ---------------------------------------------
        // case 4: if (SRAM_Active) Mem_Pages [4] [Addr] = Value; return;
        // case 5: if (SRAM_Active) Mem_Pages [5] [Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

WRITE_FUNC (Write_Mapper_SMS_4PakAllAction)
{
    switch (Addr)
    {
    case 0x3FFE: // Frame 0 ----------------------------------------------------
        {
            g_machine.mapper_regs[0] = Value;
            const int page = (g_machine.mapper_regs[0]) & tsms.Pages_Mask_16k;
            Map_16k_ROM(0, page * 2);
            return;
        }
    case 0x7FFF: // Frame 1 ----------------------------------------------------
        {
            g_machine.mapper_regs[1] = Value;
            const int page = (g_machine.mapper_regs[1]) & tsms.Pages_Mask_16k;
            Map_16k_ROM(2, page * 2);
            return;
        }
    case 0xBFFF: // Frame 2 ----------------------------------------------------
        {
            g_machine.mapper_regs[2] = Value;
            const int page = ((g_machine.mapper_regs[0]&0x30) + (g_machine.mapper_regs[2])) & tsms.Pages_Mask_16k;
            Map_16k_ROM(4, page * 2);
            return;
        }
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

// Write function for Mapper #9: MAPPER_SMS_Korean_A000
WRITE_FUNC (Write_Mapper_SMS_Korean_A000)
{
    if (Addr == 0xA000) // Frame 2 -----------------------------------------------
    {
        Value = (Value & tsms.Pages_Mask_16k);
        /* ROM [0xA000] = */ g_machine.mapper_regs[2] = Value;
        Map_16k_ROM(4, g_machine.mapper_regs[2] * 2);
        return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

// Write function for Mapper #20: MAPPER_SMS_Korean_BFFC
// (Note: somehow similar to #21)
WRITE_FUNC(Write_Mapper_SMS_Korean_BFFC)
{
    if (Addr == 0xBFFC)
    {
        g_machine.mapper_regs[0] = Value;

        const int mask_16 = tsms.Pages_Mask_16k; // 0x3F for a 1 MB rom
        u8 lower;
        u8 upper;
        switch (Value & (0x40 | 0x80))
        {
        case 0x00: // 32 KB SMS/SG game
            lower = (Value & 0x3E);
            upper = (Value & 0x3E) | 1;
            Map_16k_ROM(0, (lower & mask_16) * 2);
            Map_16k_ROM(2, (upper & mask_16) * 2);
            Map_8k_ROM(4, (0x3f & mask_16) * 2 + 1); // actually should be open bus
            Map_8k_ROM(5, (0x3f & mask_16) * 2 + 1); // actually should be open bus
            break;
        case 0x40: // 16 KB SMS/SG game
            lower = upper = (Value & 0x3F);
            Map_16k_ROM(0, (lower & mask_16) * 2);
            Map_16k_ROM(2, (upper & mask_16) * 2);
            Map_8k_ROM(4, (0x3f & mask_16) * 2 + 1); // actually should be open bus
            Map_8k_ROM(5, (0x3f & mask_16) * 2 + 1); // actually should be open bus
            break;
        case 0x80: // MSX Regular (16 KB bios + 16 KB ROM)
            lower = 0x20;
            upper = (Value & 0x3F);
            Map_16k_ROM(0, (lower & mask_16) * 2);
            Map_16k_ROM(2, (upper & mask_16) * 2);
            Map_8k_ROM(4, (0x3f & mask_16) * 2 + 1); // actually should be open bus
            Map_8k_ROM(5, (0x3f & mask_16) * 2 + 1); // actually should be open bus
            break;
        case 0xC0: // MSX Namco (16 KB bios + 2x8 KB ROM each mirrored once in A B B A order)
            lower = 0x20;
            upper = (Value & 0x3F);
            Map_16k_ROM(0, (lower & mask_16) * 2);
            Map_16k_ROM(2, (upper & mask_16) * 2);
            Map_8k_ROM(4, (upper & mask_16) * 2 + 1);
            Map_8k_ROM(5, (upper & mask_16) * 2);
            break;
        }

        return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error(Addr, Value);
}


// Write function for Mapper #16: MAPPER_SMS_Korean_FFFF_HiCom
WRITE_FUNC (Write_Mapper_SMS_Korean_FFFF_HiCom)
{
    if (Addr == 0xFFFF) // Frame 2 -----------------------------------------------
    {
        RAM[0x1FFF] = Value;
        Value = (Value & tsms.Pages_Mask_16k);
        g_machine.mapper_regs[0] = Value;
        Map_16k_ROM(0, g_machine.mapper_regs[0] * 4);
        Map_16k_ROM(2, g_machine.mapper_regs[0] * 4 + 2);
        return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

// Mapper #19
WRITE_FUNC (Write_Mapper_SMS_Korean_2000_xor_1F)
{
    if ((Addr & 0x6000) == 0x2000) // Configurable segment -----------------------------------------------
    {
        //RAM[0x1FFF] = Value;

        // This is technically incorrect: to mimic the actual hardware
        // we would either need to use an overdumped 2MB ROM, or we
        // would need to preserve all the segment base bits, as page
        // numbers past the end of the ROM return zeroes in real
        // hardware.
        g_machine.mapper_regs[0] = Value;
        Value = ((Value ^ 0x1F) & tsms.Pages_Mask_8k) ^ 0x1F;
        Map_8k_ROM(2, Value ^ 0x1f);
        Map_8k_ROM(3, Value ^ 0x1e);
        Map_8k_ROM(4, Value ^ 0x1d);
        Map_8k_ROM(5, Value ^ 0x1c);
        return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error (Addr, Value);
} 

// Mapper #21
// (Note: somehow similar to #20, see how #20 is written as it is somehow neater)
WRITE_FUNC (Write_Mapper_SMS_Korean_FFFE)
{
    if (Addr == 0xFFFE) // Configurable segment -----------------------------------------------
    {
        g_machine.mapper_regs[0] = Value;
        // 0abcccccd
        //  a (1-bit)  when 0 = map MSX BIOS in page 0 and 1, when 1 = use regular register
        //  b (1-bit)  
        //  c (5-bits)
        //  d (1-bit)  
        Map_8k_ROM(0, (((Value & 0x40) == 0x40) ? ((Value & 0x1e) * 2) : 0) & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, (((Value & 0x40) == 0x40) ? ((Value & 0x1e) * 2 + 1) : 1) & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, (((Value & 0x40) == 0x40) ? (((Value & 0x1e) + 1) * 2) : ((Value & 0x1f) * 2)) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (((Value & 0x40) == 0x40) ? (((Value & 0x1e) + 1) * 2 + 1) : ((Value & 0x1f) * 2 + 1)) & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, (((Value & 0x60) == 0x20) ? ((Value & 0x1f) * 2 + 1) : 0x3f) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (((Value & 0x60) == 0x20) ? ((Value & 0x1f) * 2) : 0x3f) & tsms.Pages_Mask_8k);
        //return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error (Addr, Value);
} 

// Mapper #22
// Super Game 150, Super Game 270
WRITE_FUNC(Write_Mapper_SMS_Korean_FFF3_FFFC)
{
    const int incomplete_address_decoding = 0x4000;
    const u16 addr_assumed = Addr | incomplete_address_decoding;
    if (addr_assumed == 0xFFF3 || addr_assumed == 0xFFFC) // Configurable segment -----------------------------------------------
    {
        if (addr_assumed == 0xFFF3) {
            g_machine.mapper_regs[0] = Value;
        }
        else if (addr_assumed == 0xFFFC) {
            g_machine.mapper_regs[1] = Value;
        }
        const int Mapper = g_machine.mapper_regs[0];
        const int Mode = g_machine.mapper_regs[1];

        const int Config = Mode & 0xE0;

        const int Page0 = (Mode & 0x10) * 8 + (Mapper & 0x3E) * 2;
        const int Page1 = Page0 + 1;
        const int Page2 = Page0 + 2;
        const int Page3 = Page0 + 3;

        const int Odd = Mapper & 0x01;

        if (Config == 0x00) {
            // 16KB SMS
            Map_8k_ROM(0, (Odd ? Page2 : Page0) & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, (Odd ? Page3 : Page1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(2, (Odd ? Page2 : Page0) & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, (Odd ? Page3 : Page1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, 0xFF & tsms.Pages_Mask_8k);
        }
        else if (Config == 0x20) {
            // 32KB SMS
            Map_8k_ROM(0, Page0 & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, Page1 & tsms.Pages_Mask_8k);
            Map_8k_ROM(2, Page2 & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, Page3 & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, 0xFF & tsms.Pages_Mask_8k);
        }
        else if (Config == 0x40) {
            // MSX mapped at 0x4000
            Map_8k_ROM(0, 0x80 & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, 0x81 & tsms.Pages_Mask_8k);
            Map_8k_ROM(2, (Odd ? Page2 : Page0) & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, (Odd ? Page3 : Page1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, 0xFF & tsms.Pages_Mask_8k);
        }
        else if (Config == 0x60) {
            // MSX mapped at 0x8000
            Map_8k_ROM(0, 0x80 & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, 0x81 & tsms.Pages_Mask_8k);
            Map_8k_ROM(2, 0xFE & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, (Odd ? Page2 : Page0) & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, (Odd ? Page3 : Page1) & tsms.Pages_Mask_8k);
        }
        else if (Config == 0x80) {
            // MSX (8KB permuted)
            Map_8k_ROM(0, 0x80 & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, 0x81 & tsms.Pages_Mask_8k);
            Map_8k_ROM(2, (Odd ? Page2 : Page0) & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, (Odd ? Page3 : Page1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, (Odd ? Page3 : Page1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, (Odd ? Page2 : Page0) & tsms.Pages_Mask_8k);
        }
        else if (Config == 0xA0) {
            // MSX (16KB permuted)
            Map_8k_ROM(0, 0x80 & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, 0x81 & tsms.Pages_Mask_8k);
            Map_8k_ROM(2, (Odd ? Page2 : Page0) & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, (Odd ? Page3 : Page1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, (Odd ? Page0 : Page2) & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, (Odd ? Page1 : Page3) & tsms.Pages_Mask_8k);
        }
        else {
            // ???
            Map_8k_ROM(0, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(2, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, 0xFF & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, 0xFF & tsms.Pages_Mask_8k);
        }
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error(Addr, Value);
}

// Mapper #23
// Zemina 4-in-1 (Q-Bert, Sports 3, Gulkave, Pooyan)
WRITE_FUNC(Write_Mapper_SMS_Korean_0000_xor_FF)
{
    if (Addr == 0x0000) // Configurable segment -----------------------------------------------
    {
        g_machine.mapper_regs[0] = Value;
        if ((Value & 0xF0) == 0xF0)
        {
            // 0xFF maps the same 16KB (of all 0xFF bytes!) twice
            Map_8k_ROM(2, 2 & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, 3 & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, 2 & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, 3 & tsms.Pages_Mask_8k);
        }
        else {
            int segment_start_8k = ((Value ^ 0xF0) & 0xF0) >> 2;
            Map_8k_ROM(2, segment_start_8k & tsms.Pages_Mask_8k);
            Map_8k_ROM(3, (segment_start_8k + 1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(4, (segment_start_8k + 2) & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, (segment_start_8k + 3) & tsms.Pages_Mask_8k);
        }
        return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error(Addr, Value);
}

// Mapper #24
// Mega Mode Super Game 30 [SMS-MD] (KR)
WRITE_FUNC (Write_Mapper_SMS_Korean_MD_FFF0)
{
    if (Addr == 0xFFF0) // Configurable segment -----------------------------------------------
    {
        g_machine.mapper_regs[0] = Value;
        g_machine.mapper_regs[1] = 0;
        g_machine.mapper_regs[2] = 1;
        Map_8k_ROM(0, (Value * 4) & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, (Value * 4 + 1) & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, (Value * 4 + 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (Value * 4 + 3) & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, (Value * 4) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (Value * 4 + 1) & tsms.Pages_Mask_8k);
        //return;
    }
    if (Addr == 0xFFFE)
    {
        g_machine.mapper_regs[1] = Value;
        Map_8k_ROM(2, (g_machine.mapper_regs[0] * 4 + (Value & 0x0F) * 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (g_machine.mapper_regs[0] * 4 + (Value & 0x0F) * 2 + 1) & tsms.Pages_Mask_8k);
        //return;
    }
    if (Addr == 0xFFFF)
    {
        g_machine.mapper_regs[2] = Value;
        Map_8k_ROM(4, (g_machine.mapper_regs[0] * 4 + (Value & 0x0F) * 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (g_machine.mapper_regs[0] * 4 + (Value & 0x0F) * 2 + 1) & tsms.Pages_Mask_8k);
        //return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error (Addr, Value);
} 

// Mapper #25
// Jaemiissneun Game Mo-eumjip 42/65 Hap [SMS-MD]
// Pigu-Wang 7 Hap ~ Jaemiiss-neun Game Mo-eumjip [SMS-MD]
WRITE_FUNC (Write_Mapper_SMS_Korean_MD_FFF5)
{
    // These bits in the address apparently are not checked when
    // determining whether a write corresponds to a mapper register
    // and are assumed to be set. Some cartridges using this mapper
    // contain code that accesses the registers by their aliases.
    const int incomplete_address_decoding = 0x4010;
    if ((Addr | incomplete_address_decoding) == 0xFFF5) // Configurable segment -----------------------------------------------
    {
        // Not yet implemented: some multicarts with this mapper seem
        // to require the mapping to be left-rotated through the
        // register one bit at a time. So instead of writing simple
        // 0x04, you would need to write (in sequence) 0x08, 0x10,
        // 0x20, 0x40, 0x80, 0x01, 0x02, 0x04.
        //
        // However other cartridges have a mapper that does not
        // require this "unlock" and their code does not perform the unlock.
        //
        // For now, only the simpler mapper implementation is present,
        // as this is apparently sufficient to run the menus and games
        // for both mapper types.
        g_machine.mapper_regs[0] = Value;
        g_machine.mapper_regs[1] = 1;
        g_machine.mapper_regs[2] = 1;
        Map_8k_ROM(0, (Value * 4) & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, (Value * 4 + 1) & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, (Value * 4 + 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (Value * 4 + 3) & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, (Value * 4 + 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (Value * 4 + 3) & tsms.Pages_Mask_8k);
        //return;
    }
    const unsigned int mapper_page_mask_16k = g_machine.mapper_regs[0] >= 0x10 ? 0x1f : 0x0f;
    if ((Addr | incomplete_address_decoding) == 0xFFFE)
    {
        g_machine.mapper_regs[1] = Value;
        Map_8k_ROM(2, (g_machine.mapper_regs[0] * 4 + (Value & mapper_page_mask_16k) * 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (g_machine.mapper_regs[0] * 4 + (Value & mapper_page_mask_16k) * 2 + 1) & tsms.Pages_Mask_8k);
        //return;
    }
    if ((Addr | incomplete_address_decoding) == 0xFFFF)
    {
        g_machine.mapper_regs[2] = Value;
        Map_8k_ROM(4, (g_machine.mapper_regs[0] * 4 + (Value & mapper_page_mask_16k) * 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (g_machine.mapper_regs[0] * 4 + (Value & mapper_page_mask_16k) * 2 + 1) & tsms.Pages_Mask_8k);
        //return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error (Addr, Value);
} 

// Mapper #26
// Game Jiphap 30 Hap [SMS-MD] (KR)
WRITE_FUNC(Write_Mapper_SMS_Korean_MD_FFFA)
{
    if (Addr == 0xFFFA) // Configurable segment -----------------------------------------------
    {
        g_machine.mapper_regs[0] = Value;
        g_machine.mapper_regs[1] = 0;
        g_machine.mapper_regs[2] = 1;
        Map_8k_ROM(0, (Value * 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(1, (Value * 2 + 1) & tsms.Pages_Mask_8k);
        Map_8k_ROM(2, (Value * 2 + 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (Value * 2 + 3) & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, (Value * 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (Value * 2 + 1) & tsms.Pages_Mask_8k);
        //return;
    }
    if (Addr == 0xFFFF)
    {
        g_machine.mapper_regs[1] = Value;
        Map_8k_ROM(4, (g_machine.mapper_regs[0] * 2 + (Value & 0x0F) * 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (g_machine.mapper_regs[0] * 2 + (Value & 0x0F) * 2 + 1) & tsms.Pages_Mask_8k);
        //return;
    }
    if (Addr == 0xFFFE)
    {
        g_machine.mapper_regs[2] = Value;
        Map_8k_ROM(2, (g_machine.mapper_regs[0] * 2 + (Value & 0x0F) * 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (g_machine.mapper_regs[0] * 2 + (Value & 0x0F) * 2 + 1) & tsms.Pages_Mask_8k);
        //return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error(Addr, Value);
}

// Mapper #27
// 2 Hap in 1 (Moai-ui bomul, David-2)
WRITE_FUNC (Write_Mapper_SMS_Korean_MSX_32KB_2000)
{
    if (Addr == 0x2000) // Configurable segment -----------------------------------------------
    {
        g_machine.mapper_regs[0] = Value;
        Map_8k_ROM(2, (Value * 4 + 2) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (Value * 4 + 3) & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, (Value * 4 + 4) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (Value * 4 + 5) & tsms.Pages_Mask_8k);
        return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

// Mapper #40
// Zemina Best 88 (KR)
// Zemina Best 25 (KR)
// Zemina Best 39 (KR)
WRITE_FUNC (Write_Mapper_SMS_Korean_MSX_SMS_8000)
{
    if (Addr == 0x8000) // Configurable segment -----------------------------------------------
    {
        // Special case to support Zemina Best 25 [Best 88] (KR)
        if (g_machine.mapper_regs[0] == 0xFF) {
            Value ^= 0x22;
        }
        g_machine.mapper_regs[0] = Value;
        if (Value & 0x80) {
            Map_8k_ROM(0, (Value ^ 3) & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, (Value ^ 2) & tsms.Pages_Mask_8k);
        } else {
            Map_8k_ROM(0, 0x3c & tsms.Pages_Mask_8k);
            Map_8k_ROM(1, 0x3c & tsms.Pages_Mask_8k);
        }
        Map_8k_ROM(2, (Value ^ 1) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (Value ^ 0) & tsms.Pages_Mask_8k);
        Map_8k_ROM(4, (Value ^ 3) & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, (Value ^ 2) & tsms.Pages_Mask_8k);
        return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

// Mapper #44
// Super Game 45 [Road Fighter] (KR)
WRITE_FUNC(Write_Mapper_SMS_Korean_MSX_16KB_BFFE)
{
    if (Addr == 0xBFFE) // Configurable segment -----------------------------------------------
    {
        g_machine.mapper_regs[0] = Value;
        unsigned int page_mask = ((Value & 0x3f) == 0x21) ? 0x3f : 0x1f;
        Map_8k_ROM(2, (((Value & page_mask) * 2)) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (((Value & page_mask) * 2) | 1) & tsms.Pages_Mask_8k);
        if (Value & 0x20) {
            // "Namco" mapping: A B B A
            Map_8k_ROM(4, (((Value & page_mask) * 2) | 1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, (((Value & page_mask) * 2)) & tsms.Pages_Mask_8k);
        } else {
            // normal mapping: A B A B
            Map_8k_ROM(4, (((Value & page_mask) * 2)) & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, (((Value & page_mask) * 2) | 1) & tsms.Pages_Mask_8k);
        }
        return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error(Addr, Value);
}

// Mapper #45
// Super Game 30 [Road Fighter] [Super Game 45] (KR)
WRITE_FUNC(Write_Mapper_SMS_Korean_MSX_16KB_FFFF)
{
    if (Addr == 0xFFFF) // Configurable segment -----------------------------------------------
    {
        g_machine.mapper_regs[0] = Value;
        unsigned int page_mask = ((Value & 0x3f) == 0x21) ? 0x3f : 0x1f;
        Map_8k_ROM(2, (((Value & page_mask) * 2)) & tsms.Pages_Mask_8k);
        Map_8k_ROM(3, (((Value & page_mask) * 2) | 1) & tsms.Pages_Mask_8k);
        if (Value & 0x20) {
            // "Namco" mapping: A B B A
            Map_8k_ROM(4, (((Value & page_mask) * 2) | 1) & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, (((Value & page_mask) * 2)) & tsms.Pages_Mask_8k);
        } else {
            // normal mapping: A B A B
            Map_8k_ROM(4, (((Value & page_mask) * 2)) & tsms.Pages_Mask_8k);
            Map_8k_ROM(5, (((Value & page_mask) * 2) | 1) & tsms.Pages_Mask_8k);
        }
        //return;
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error(Addr, Value);
}

// Based on MSX ASCII 8KB mapper? http://bifi.msxnet.org/msxnet/tech/megaroms.html#ascii8
// - This mapper requires 4 registers to save bank switching state.
//   However, all other mappers so far used only 3 registers, stored as 3 bytes.
//   Because of the current development state of MEKA and to avoid breaking save-state format 
//   for emulators that import the current MEKA save format (because the 3 mapper bytes are
//   in the static "SMS_TYPE" structure), I decided to store those 4 registers packed each
//   into 4-bits of 2 of the available bytes. It's not technically incorrect anyway since 
//   those variable are just our own representation of the hardware, but its error prone.
// - Using 4-bits limits number of banks to 16 which is 128 KB, corresponding to the maximum
//   game size currently known for this mapper.
// - Using 4-bits chunks in 2 bytes instead of 6-bits chunks in all 3 bytes allows seeing
//   the values in techinfo.c box in more intuitive way (since the first 2 8KB pages are
//   not switchable the first register is kept as zero).
// - If ever it happens that Sega 8-bits mappers gets standardized this whole system will
//   be reworked and per-mapper state be taken into account in save states.
WRITE_FUNC (Write_Mapper_SMS_Korean_MSX_8KB_0003)
{
    switch (Addr)
    {
    case 0x0000:
        {
#ifdef DEBUG_PAGES
            if (Value > tsms.Pages_Count_8k)
            { Msg(MSGT_DEBUG, "At PC=%04X: Frame 4 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
            g_machine.mapper_regs[0] = Value;
            Map_8k_ROM(4, Value & tsms.Pages_Mask_8k);
            return;
        }
    case 0x0001:
        {
#ifdef DEBUG_PAGES
            if (Value > tsms.Pages_Count_8k)
            { Msg(MSGT_DEBUG, "At PC=%04X: Frame 5 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
            g_machine.mapper_regs[1] = Value;
            Map_8k_ROM(5, Value & tsms.Pages_Mask_8k);
            return;
        }
    case 0x0002:
        {
#ifdef DEBUG_PAGES
            if (Value > tsms.Pages_Count_8k)
            { Msg(MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
            g_machine.mapper_regs[2] = Value;
            Map_8k_ROM(2, Value & tsms.Pages_Mask_8k);
            return;
        }
    case 0x0003:
        {
#ifdef DEBUG_PAGES
            if (Value > tsms.Pages_Count_8k)
            { Msg(MSGT_DEBUG, "At PC=%04X: Frame 3 set to non-existent page: %d", CPU_GetPC, Value); }
#endif
            g_machine.mapper_regs[3] = Value;
            Map_8k_ROM(3, Value & tsms.Pages_Mask_8k);
            return;
        }
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

// Write function for mapper #18: MAPPER_SMS_Korean_MSX_8KB_0300
WRITE_FUNC(Write_Mapper_SMS_Korean_MSX_8KB_0300)
{
    const int incomplete_address_decoding_mask = 0xFF00;
    const u16 addr_assumed = Addr & incomplete_address_decoding_mask;
    switch (addr_assumed)
    {
    case 0x0000:
    {
        g_machine.mapper_regs[0] = Value;
        Map_8k_ROM(4, Value & tsms.Pages_Mask_8k);
        return;
    }
    case 0x0100:
    {
        g_machine.mapper_regs[1] = Value;
        Map_8k_ROM(2, Value & tsms.Pages_Mask_8k);
        return;
    }
    case 0x0200:
    {
        g_machine.mapper_regs[2] = Value;
        Map_8k_ROM(1, Value & tsms.Pages_Mask_8k);
        Map_8k_ROM(5, Value & tsms.Pages_Mask_8k);
        return;
    }
    case 0x0300:
    {
        g_machine.mapper_regs[3] = Value;
        Map_8k_ROM(3, Value & tsms.Pages_Mask_8k);
        return;
    }
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages[6][Addr] = Value; return;
    case 7: Mem_Pages[7][Addr] = Value; return;
    }

    Write_Error(Addr, Value);
}

WRITE_FUNC (Write_Mapper_SMS_Korean_Janggun)
{
    switch (Addr)
    {
    case 0xFFFE:
        {
            g_machine.mapper_regs[0] = Value;
            if (Value & 0x40)
                g_machine.mapper_janggun_bytes_flipping_flags |= ((1 << 2)|(1 << 3));
            else
                g_machine.mapper_janggun_bytes_flipping_flags &= ~((1 << 2)|(1 << 3));
            Map_16k_ROM(2, (Value & tsms.Pages_Mask_16k)*2);
            break;
        }
    case 0x4000:
        {
            g_machine.mapper_regs[1] = Value;
            Map_8k_ROM(2, Value & tsms.Pages_Mask_8k);
            return;
        }
    case 0x6000:
        {
            g_machine.mapper_regs[2] = Value;
            Map_8k_ROM(3, Value & tsms.Pages_Mask_8k);
            return;
        }
    case 0xFFFF:
        {
            g_machine.mapper_regs[3] = Value;
            if (Value & 0x40)
                g_machine.mapper_janggun_bytes_flipping_flags |= ((1 << 4)|(1 << 5));
            else
                g_machine.mapper_janggun_bytes_flipping_flags &= ~((1 << 4)|(1 << 5));
            Map_16k_ROM(4, (Value & tsms.Pages_Mask_16k)*2);
            break;
        }
    case 0x8000:
        {
            g_machine.mapper_regs[4] = Value;
            Map_8k_ROM(4, Value & tsms.Pages_Mask_8k);
            return;
        }
    case 0xA000:
        {
            g_machine.mapper_regs[5] = Value;
            Map_8k_ROM(5, Value & tsms.Pages_Mask_8k);
            return;
        }
    }

    switch (Addr >> 13)
    {
        // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

WRITE_FUNC (Write_Mapper_SMS_DisplayUnit)
{
    switch (Addr >> 13)
    {
        // RAM [0x4000]
        case 2: Mem_Pages [2] [0x4000 + (Addr & 0x07FF)] = Value; return;
        // RAM [0xC000] = [0xE000] ------------------------------------------------
        case 6: Mem_Pages [6] [Addr] = Value; return;
        case 7: Mem_Pages [7] [Addr] = Value; return;
    }

    Write_Error (Addr, Value);
}

READ_FUNC (Read_Mapper_SMS_DisplayUnit)
{
   // if (Addr == 0x4000)
   //     return (0xff);//Mem_Pages [2] [0x4000] | 0x80);  // FIXME
    if (Addr == 0x8000)
        return (Mem_Pages [2] [0x4000] | 0x80);  // FIXME
    return (Mem_Pages [Addr >> 13] [Addr]);
}

WRITE_FUNC (Write_Mapper_93c46)
{
 switch (Addr)
   {
   case 0x8000: // 93c46 Write (Set Lines)
        EEPROM_93c46_Set_Lines (Value);
        return;
   case 0xFFFC: // 93c46 Control Register
        EEPROM_93c46_Control (Value);
        RAM [0x1FFC] = Value;
        return;
   case 0xFFFD: // Frame 0 ----------------------------------------------------
        #ifdef DEBUG_PAGES
          if (Value > tsms.Pages_Count_16k)
             { Msg(MSGT_DEBUG, "At PC=%04X: Frame 0 set to non-existent page: %d", CPU_GetPC, Value); return; }
        #endif
        RAM [0x1FFD] = g_machine.mapper_regs[0] = Value & tsms.Pages_Mask_16k;
        Map_16k_ROM (0, g_machine.mapper_regs[0] * 2);
        return;
   case 0xFFFE: // Frame 1 ----------------------------------------------------
        #ifdef DEBUG_PAGES
          if (Value > tsms.Pages_Count_16k)
             { Msg(MSGT_DEBUG, "At PC=%04X: Frame 1 set to non-existent page: %d", CPU_GetPC, Value); return; }
        #endif
        RAM [0x1FFE] = g_machine.mapper_regs[1] = Value & tsms.Pages_Mask_16k;
        Map_16k_ROM (2, g_machine.mapper_regs[1] * 2);
        return;
   case 0xFFFF: // Frame 2 ----------------------------------------------------
        #ifdef DEBUG_PAGES
          if (Value > tsms.Pages_Count_16k)
             { Msg(MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existent page: %d", CPU_GetPC, Value); return; }
        #endif
        RAM [0x1FFF] = g_machine.mapper_regs[2] = Value & tsms.Pages_Mask_16k;
        Map_16k_ROM (4, g_machine.mapper_regs[2] * 2);
        return;
   }
 switch (Addr >> 13)
   {
   // 93c46? Direct Access ----------------------------------------------------
   case 4: if (Addr >= 0x8008 && Addr < 0x8088) { EEPROM_93c46_Direct_Write (Addr - 0x8008, Value); return; } break;
   // RAM [0xC000] = [0xE000] -------------------------------------------------
   case 6: Mem_Pages [6] [Addr] = Value; return;
   case 7: Mem_Pages [7] [Addr] = Value; return;
   }

 Write_Error (Addr, Value);
}

// FIXME-WIP
WRITE_FUNC (Write_Mapper_SMS_ActionReplay)
{
 switch (Addr >> 13)
    {
    // RAM [0x4000]..
    case 2: Mem_Pages [2] [Addr] = Value; return;
    case 3: Mem_Pages [3] [Addr] = Value; return;
    // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; return;
    }

 Write_Error (Addr, Value);
}

READ_FUNC (Read_Default)
{
    #ifdef DEBUG_UNINITIALIZED_RAM_ACCESSES
    if (Addr >= 0xC000 && Addr <= 0xFFFF)
    {
        if (RAM_IsUninitialized[Addr&0x1FFF])
        {
            Msg(MSGT_DEBUG, "At PC=$%04x, Read uninitialized RAM[$%04x]", sms.R.PC.W, Addr);
            //sms.R.Trace = 1;
        }
    }
    #endif

    const unsigned int page = (Addr >> 13);
    return (Mem_Pages [page] [Addr]);
}

READ_FUNC (Read_Mapper_SMS_Korean_Janggun)
{
    const unsigned int page = (Addr >> 13);
    u8 b = Mem_Pages[page][Addr];

    if (g_machine.mapper_janggun_bytes_flipping_flags & (1<<page))
        b = Mapper_SMS_Korean_Janggun_BitReverseLUT[b];

    return b;
}

READ_FUNC (Read_Mapper_SG1000_Taiwan_MSX_Adapter_TypeA)
{
    const unsigned int page = (Addr >> 13);
    
    // 0xC000->0xFFFF: SG-1000 work RAM
    if (page >= 6)
        Addr &= ~0x1800;

    return Mem_Pages[page][Addr];
}

READ_FUNC (Read_Mapper_93c46)
{
    // Addresses in the [8000h] range --------------------------------------------
    if ((Addr >> 13) == 0x04 && (EEPROM_93c46.Enabled))
    {
        // 93c46 Serial Access ----------------------------------------------------
        if (Addr == 0x8000)
        {
            // Msg(MSGT_DEBUG, Msg_Get(MSG_Debug_Trap_Read), sms.R.PC.W, Addr);
            return EEPROM_93c46_Read();
        }
        // 93c46? Direct Access ---------------------------------------------------
        if (Addr >= 0x8008 && Addr < 0x8088)
            return EEPROM_93c46_Direct_Read(Addr - 0x8008);
    }

    return (Mem_Pages [Addr >> 13] [Addr]);
}

//-----------------------------------------------------------------------------

