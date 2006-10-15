//-----------------------------------------------------------------------------
// MEKA - mappers.c
// Memory Mapper Emulation - Code
//-----------------------------------------------------------------------------

//#define DEBUG_MEM
//#define DEBUG_PAGES
#include "shared.h"
#include "mappers.h"

//-----------------------------------------------------------------------------
// Macros / Inline functions
//-----------------------------------------------------------------------------

INLINE void Map_8k_Other (int page, void *data)
{ Mem_Pages [page] = (void *)((char *)data - (page * 0x2000)); }

INLINE void Map_16k_Other (int page, void *data)
{ Mem_Pages [page] = Mem_Pages [page + 1] = (void *)((char *)data - (page * 0x2000)); }

INLINE void Map_8k_RAM (int page, int ram_page)
{ Mem_Pages [page] = RAM + ((ram_page - page) * 0x2000); }

INLINE void Map_16k_RAM (int page, int ram_page)
{ Mem_Pages [page] = Mem_Pages [page + 1] = RAM + ((ram_page - page) * 0x2000); }

INLINE void Map_8k_ROM (int page, int rom_page)
{ Mem_Pages [page] = ROM + ((rom_page - page) * 0x2000); }

INLINE void Map_16k_ROM (int page, int rom_page)
{ Mem_Pages [page] = Mem_Pages [page + 1] = ROM + ((rom_page - page) * 0x2000); }

#ifdef DEBUG_MEM
void    Write_Error (int Addr, u8 Value)
{
    Msg (MSGT_DEBUG, Msg_Get (MSG_Debug_Trap_Write), sms.R.PC.W, Value, Addr);
}
#endif

//-----------------------------------------------------------------------------

void    Mapper_Get_RAM_Infos (int *plen, int *pstart_addr)
{
    int len, start_addr;

    if (cur_drv->id == DRV_NES)
    { 
        len = 0x0800;
        start_addr = 0x0000;
    }
    else
        switch (cur_machine.mapper)
    {
        case MAPPER_32kRAM:         len = 0x08000; start_addr = 0x8000; break;
        case MAPPER_ColecoVision:   len = 0x00400; start_addr = 0x6000; break;
        case MAPPER_SG1000:         len = 0x01000; start_addr = 0xC000; break;
        case MAPPER_TVOekaki:       len = 0x01000; start_addr = 0xC000; break;
        case MAPPER_SF7000:         len = 0x10000; start_addr = 0x0000; break;
        case MAPPER_SMS_DisplayUnit:len = 0x02800; start_addr = 0x4000; break; // FIXME: Incorrect, due to scattered mapping!
            // FIXME: ActionReplay!!
            // default, codemaster, korean..
        default:                    len = 0x02000; start_addr = 0xC000; break;
    }
    if (plen)
        *plen = len;
    if (pstart_addr)
        *pstart_addr = start_addr;
}

// Return -1 if can't tell, else a mapper number
int         Mapper_Autodetect (void)
{
    int     i;
    int     c8000, cA000, cFFFF;

    // If ROM is smaller than 32 kb, autodetected SMS mapper are not needed
    if (tsms.Size_ROM <= 0x8000)
        return (-1);

    // Search for code to access mapper -> LD (8000)|(FFFF), A
    c8000 = cA000 = cFFFF = 0;
    for (i = 0; i < 0x8000; i++)
        if (ROM [i] == 0x32) // Z80 opcode for: LD (xxxx), A
        {
            if (*(word *)&ROM [i + 1] == 0xFFFF)
            { i += 2; cFFFF++; continue; }
            if (*(word *)&ROM [i + 1] == 0x8000)
            { i += 2; c8000++; continue; }
            if (*(word *)&ROM [i + 1] == 0xA000)
            { i += 2; cA000++; continue; }
        }

    // 2 is a security measure, although experience showed if was not needed
    if (c8000 > cFFFF + 2 || (c8000 > 0 && cFFFF == 0))
        return (MAPPER_CodeMasters);
    if (cA000 > cFFFF + 2 || (cA000 > 0 && cFFFF == 0))
        return (MAPPER_SMS_Korean);

    return (MAPPER_Auto);
}

// [MAPPER: DEFAULT] WRITE BYTE -----------------------------------------------
WRITE_FUNC (Write_Default)
{
 if ((Addr & 0xFFF8) == 0xFFF8)
 switch (Addr & 0x7)
 {
 case 4: // 0xFFFC: SRAM Register ---------------------------------------------
         RAM [0x1FFC] = sms.Mapping_Register = Value;
         if (SRAM_Active)
            {
            if (SRAM_Page)
               {
               sms.SRAM_Pages = 4; // 4 x 8 kb
               Map_8k_Other (4, &SRAM [0x4000]);
               }
            else
               {
               if (sms.SRAM_Pages < 2) sms.SRAM_Pages = 2; // 2 x 8 kb
                  Map_8k_Other (4, &SRAM [0x0000]);
               }
            }
          else
            {
            Map_8k_ROM (4, sms.Pages_Reg [2] * 2);
            }
          Mem_Pages [5] = Mem_Pages [4];
          return;
 case 5: // 0xFFFD: Frame 0 ---------------------------------------------------
         #ifdef DEBUG_PAGES
           if (Value != 0)
              { Msg (MSGT_DEBUG, "At PC=%04X: Frame 0 set to page %d !", CPU_GetPC, Value); }
         #endif
         Value &= tsms.Pages_Mask_16k;
         if (sms.Pages_Reg [0] != Value)
            {
            RAM [0x1FFD] = sms.Pages_Reg [0] = Value;
            memcpy (Game_ROM_Computed_Page_0 + 0x400, ROM + (Value << 14) + 0x400, 0x3C00);
            }
         return;
 case 6: // 0xFFFE: Frame 1 ---------------------------------------------------
         #ifdef DEBUG_PAGES
           if (Value > tsms.Pages_Count_16k)
              { Msg (MSGT_DEBUG, "At PC=%04X: Frame 1 set to non-existant page: %d", CPU_GetPC, Value); }
         #endif
         RAM [0x1FFE] = sms.Pages_Reg [1] = Value & tsms.Pages_Mask_16k;
         Map_16k_ROM (2, sms.Pages_Reg [1] * 2);
         return;
 case 7: // 0xFFFF: Frame 2 ---------------------------------------------------
         #ifdef DEBUG_PAGES
            if (Value > tsms.Pages_Count_16k)
               { Msg (MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existant page: %d", CPU_GetPC, Value); }
         #endif
         RAM [0x1FFF] = sms.Pages_Reg [2] = Value & tsms.Pages_Mask_16k;
         if (!SRAM_Active)
            {
            Map_16k_ROM (4, sms.Pages_Reg [2] * 2);
            }
         return;
 default: // 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB: Glasse Register ------------------
         Mem_Pages [7] [Addr] = sms.Glasses_Register = Value;
         return;
 }

 // Memory Write Trapping -----------------------------------------------------
//if (Addr == 0xDEFE || Addr == 0xFEFE)
//   {
//   Write_Error (Addr, Value);
//   }

 // RAM -----------------------------------------------------------------------

 // Rambo III
 // if (Addr == 0xD4B0 || Addr == 0xDE84/*|| (Addr >= 0xD4BF && Addr < 0xD4DF + 32*2)*/)
 //   { Msg (MSGT_DEBUG, "At PC=%04X: RAM[%04X] = %02X", CPU_GetPC, Addr, Value); }

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
 switch (Addr >> 13)
   {
   case 4: Mem_Pages [4] [Addr] = Value; return;
   case 5: Mem_Pages [5] [Addr] = Value; return;
   case 6: Mem_Pages [6] [Addr] = Value; return;
   case 7: Mem_Pages [7] [Addr] = Value; return;
   }
}

// [MAPPER: SG-1000] WRITE BYTE -----------------------------------------------
WRITE_FUNC (Write_Mapper_SG1000)
{
 switch (Addr)
 {
 case 0xFFFD: // Frame 0 ------------------------------------------------------
              #ifdef DEBUG_PAGES
                if (Value != 0)
                   { Msg (MSGT_DEBUG, "At PC=%04X: Frame 0 set to page %d !", CPU_GetPC, Value); }
              #endif
              Value &= tsms.Pages_Mask_16k;
              if (sms.Pages_Reg [0] != Value)
                 {
                 RAM [0x1FFD] = sms.Pages_Reg [0] = Value;
                 memcpy (Game_ROM_Computed_Page_0 + 0x400, ROM + (Value << 14) + 0x400, 0x3C00);
                 }
              return;
 case 0xFFFE: // Frame 1 ------------------------------------------------------
              #ifdef DEBUG_PAGES
                if (Value > tsms.Pages_Count_16k)
                   { Msg (MSGT_DEBUG, "At PC=%04X: Frame 1 set to non-existant page: %d", CPU_GetPC, Value); }
              #endif
              RAM [0x1FFE] = sms.Pages_Reg [1] = Value & tsms.Pages_Mask_16k;
              Map_16k_ROM (2, sms.Pages_Reg [1] * 2);
              return;
 case 0xFFFF: // Frame 2 ------------------------------------------------------
              #ifdef DEBUG_PAGES
                if (Value > tsms.Pages_Count_16k)
                   { Msg (MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existant page: %d", CPU_GetPC, Value); }
              #endif
              RAM [0x1FFF] = sms.Pages_Reg [2] = Value & tsms.Pages_Mask_16k;
              Map_16k_ROM (4, sms.Pages_Reg [2] * 2);
              return;
 }

 switch (Addr >> 13)
    {
    // RAM [0xC000] = [0xE000] -----------------------------------------------
    case 6: Mem_Pages [6] [Addr & 0xEFFF] = Mem_Pages [6] [Addr | 0x1000] = Value; return;
    case 7: Mem_Pages [7] [Addr & 0xEFFF] = Mem_Pages [7] [Addr | 0x1000] = Value; return;
    }

 Write_Error (Addr, Value);
}

// [MAPPER: CODEMASTERS] WRITE BYTE -------------------------------------------
WRITE_FUNC (Write_Mapper_CodeMasters)
{
 switch (Addr)
   {
   case 0x0000: // Frame 0 ----------------------------------------------------
        #ifdef DEBUG_PAGES
           //if (Value > tsms.Pages_Count_16k)
              { Msg (MSGT_DEBUG, "At PC=%04X: Frame 0 set to non-existant page: %d", CPU_GetPC, Value);}
        #endif
        Value = (Value & tsms.Pages_Mask_16k);
        /*ROM [0x0000] = */ sms.Pages_Reg [0] = Value;
        Map_16k_ROM (0, sms.Pages_Reg [0] * 2);
        return;
   case 0x4000: // Frame 1 ----------------------------------------------------
        #ifdef DEBUG_PAGES
           //if (Value > tsms.Pages_Count_16k)
              { Msg (MSGT_DEBUG, "At PC=%04X: Frame 1 set to non-existant page: %d", CPU_GetPC, Value); }
        #endif
        if (Value & 0x80) // OnBoard RAM for Ernie Els Golf
           {
           sms.Mapping_Register = ONBOARD_RAM_EXIST | ONBOARD_RAM_ACTIVE;
           Map_8k_RAM (5, 1); // Mapped from 0xA000 to 0xC000 only! (0x8000 to 0xA000 has ROM!)
           }
        else
           {
           if (OnBoard_RAM_Active)
              {
              // Map Page 2 back if we just disabled On Board RAM
              Map_8k_ROM (5, sms.Pages_Reg [2] * 2 + 1);
              }
           sms.Mapping_Register &= ~ONBOARD_RAM_ACTIVE;
           Value = (Value & tsms.Pages_Mask_16k);
           /* ROM [0x4000] = */ sms.Pages_Reg [1] = Value;
           Map_16k_ROM (2, sms.Pages_Reg [1] * 2);
           }
        return;
   case 0x8000: // Frame 2 ----------------------------------------------------
        #ifdef DEBUG_PAGES
           //if (Value > tsms.Pages_Count_16k)
              { Msg (MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existant page: %d", CPU_GetPC, Value); }
        #endif
        Value = (Value & tsms.Pages_Mask_16k);
        /* ROM [0x8000] = */ /* ROM[0xBFFF] = */ sms.Pages_Reg [2] = Value;
        Map_16k_ROM (4, sms.Pages_Reg [2] * 2);
        return;
   }

 switch (Addr >> 13)
    {
    // On Board RAM [0xA000]->[0xC000] ----------------------------------------
    // (for Ernie Els Golf)
    case 5: if (OnBoard_RAM_Active) { Mem_Pages [5] [Addr] = Value; return; } break;
    // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; return;
    // SaveRAM [0x8000]->[0xC000] ---------------------------------------------
    // case 4: if (SRAM_Active) Mem_Pages [4] [Addr] = Value; return;
    // case 5: if (SRAM_Active) Mem_Pages [5] [Addr] = Value; return;
    }

 Write_Error (Addr, Value);
}

// [MAPPER: KOREAN] WRITE BYTE ------------------------------------------------
WRITE_FUNC (Write_Mapper_SMS_Korean)
{
 if (Addr == 0xA000) // Frame 2 -----------------------------------------------
    {
    #ifdef DEBUG_PAGES
      if (Value > tsms.Pages_Count_16k)
        { Msg (MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existant page: %d", CPU_GetPC, Value); }
    #endif
    // Testing Dodgeball King behavior
    //  if ((Value >= 11 && Value <= 15) || Value > 27)
    //    { Msg (MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existant page: %d", CPU_GetPC, Value); }
    Value = (Value & tsms.Pages_Mask_16k);
    /* ROM [0xA000] = */ sms.Pages_Reg [2] = Value;
    Map_16k_ROM (4, sms.Pages_Reg [2] * 2);
    return;
    }

 switch (Addr >> 13)
    {
    // RAM [0xC000] = [0xE000] ------------------------------------------------
    case 6: Mem_Pages [6] [Addr] = Value; return;
    case 7: Mem_Pages [7] [Addr] = Value; return;
    }

 Write_Error (Addr, Value);
}

// MAPPER: SMS Display Unit - Write Byte
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

// MAPPER: SMS Display Unit - Read Byte
READ_FUNC (Read_Mapper_SMS_DisplayUnit)
{
   // if (Addr == 0x4000)
   //     return (0xff);//Mem_Pages [2] [0x4000] | 0x80);  // FIXME
    if (Addr == 0x8000)
        return (Mem_Pages [2] [0x4000] | 0x80);  // FIXME
    return (Mem_Pages [Addr >> 13] [Addr]);
}

// [MAPPER: 93c46 EEPROM] WRITE BYTE ------------------------------------------
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
             { Msg (MSGT_DEBUG, "At PC=%04X: Frame 0 set to non-existant page: %d", CPU_GetPC, Value); return; }
        #endif
        RAM [0x1FFD] = sms.Pages_Reg [0] = Value & tsms.Pages_Mask_16k;
        Map_16k_ROM (0, sms.Pages_Reg [0] * 2);
        return;
   case 0xFFFE: // Frame 1 ----------------------------------------------------
        #ifdef DEBUG_PAGES
          if (Value > tsms.Pages_Count_16k)
             { Msg (MSGT_DEBUG, "At PC=%04X: Frame 1 set to non-existant page: %d", CPU_GetPC, Value); return; }
        #endif
        RAM [0x1FFE] = sms.Pages_Reg [1] = Value & tsms.Pages_Mask_16k;
        Map_16k_ROM (2, sms.Pages_Reg [1] * 2);
        return;
   case 0xFFFF: // Frame 2 ----------------------------------------------------
        #ifdef DEBUG_PAGES
          if (Value > tsms.Pages_Count_16k)
             { Msg (MSGT_DEBUG, "At PC=%04X: Frame 2 set to non-existant page: %d", CPU_GetPC, Value); return; }
        #endif
        RAM [0x1FFF] = sms.Pages_Reg [2] = Value & tsms.Pages_Mask_16k;
        Map_16k_ROM (4, sms.Pages_Reg [2] * 2);
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

// [MAPPER: ACTION REPLAY] WRITE BYTE -----------------------------------------
// FIXME
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

// [MAPPER: DEFAULT] READ BYTE ------------------------------------------------
READ_FUNC (Read_Default)
{
 //if (Addr == 0x68F8)
 //   Msg (MSGT_DEBUG, Msg_Get (MSG_Debug_Trap_Read), CPU_GetPC, Addr);
 return (Mem_Pages [Addr >> 13] [Addr]);
}

// [MAPPER: 93c46 EEPROM] READ BYTE -------------------------------------------
READ_FUNC (Read_Mapper_93c46)
{
 // Addresses in the [8000h] range --------------------------------------------
 if ((Addr >> 13) == 0x04 && (EEPROM_93c46.Enabled))
    {
    // 93c46 Serial Access ----------------------------------------------------
    if (Addr == 0x8000)
       {
       // Msg (MSGT_DEBUG, Msg_Get (MSG_Debug_Trap_Read), sms.R.PC.W, Addr);
       return (EEPROM_93c46_Read ());
       }
    // 93c46? Direct Access ---------------------------------------------------
    if (Addr >= 0x8008 && Addr < 0x8088)
       return (EEPROM_93c46_Direct_Read (Addr - 0x8008));
    }

 return (Mem_Pages [Addr >> 13] [Addr]);
}

//-----------------------------------------------------------------------------

