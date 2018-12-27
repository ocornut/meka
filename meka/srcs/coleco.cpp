//-----------------------------------------------------------------------------
// MEKA - coleco.c
// Coleco Vision Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "coleco.h"
#include "debugger.h"
#include "fskipper.h"
#include "inputs_t.h"
#include "mappers.h"
#include "patch.h"
#include "vdp.h"
#include "video.h"
#include "video_m2.h"
#include "sound/psg.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

static u8 Coleco_Joy_Table_Conv [64];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// [MAPPER: COLECOVISION] WRITE BYTE ------------------------------------------
WRITE_FUNC (Write_Mapper_Coleco)
{
    if ((Addr >= 0x6000) && (Addr < 0x8000))
    {
        // 0x6000 & 0xE3FF = 0x6000
        // 0x6001 & 0xE3FF = 0x6001
        // 0x6401 & 0xE3FF = 0x6001
        // 0x7001 & 0xE3FF = 0x6001
        // 0x7FFF & 0xE3FF = 0x63FF
        // etc...
        Addr &= 0xE3FF;
        Mem_Pages[3][Addr] = Mem_Pages[3][Addr|0x0400] = Mem_Pages[3][Addr|0x0800] =
            Mem_Pages[3][Addr|0x0C00] = Mem_Pages[3][Addr|0x1000] = Mem_Pages[3][Addr|0x1400] =
            Mem_Pages[3][Addr|0x1800] = Mem_Pages[3][Addr|0x1C00] = Value;
        return;
    }
    // Msg(MSGT_DEBUG, Msg_Get(MSG_Debug_Trap_Write), sms.R.PC.W, Value, Addr);
}

void    Coleco_Port_Out (word Port, byte Value)
{
    switch (Port & 0xE0)
    {
    case 0xA0: // Video
        if (Port & 1) Tms_VDP_Out_Address (Value);
        else Tms_VDP_Out_Data (Value); return;
    case 0xE0: // Sound
        SN76489_Write(Value); /* PSG_0_Write (Value); */ return;
    case 0x80: // Change Input Mode
        sms.Input_Mode = 0; return;
    case 0xC0: // Change Input Mode
        sms.Input_Mode = 1; return;
    }
#ifdef DEBUG_IO
    Msg(MSGT_DEBUG, Msg_Get(MSG_Debug_Trap_Port_Write), sms.R.PC.W, Port, Value);
#endif
}

byte    Coleco_Port_In (word Port)
{
    switch (Port & 0xE0)
    {
    case 0xA0: // Video
        if (Port & 1) return (Tms_VDP_In_Status ());
        else return (Tms_VDP_In_Data ());
    case 0xE0: // Controls
        return (Coleco_Inputs (Port));
    }
#ifdef DEBUG_IO
    Msg(MSGT_DEBUG, Msg_Get(MSG_Debug_Trap_Port_Read), sms.R.PC.W, Port);
#endif
    return (0xFF);
}

// Colecovision Inputs emulation ----------------------------------------------
byte    Coleco_Inputs (word Port)
{
    // Msg(MSGT_DEBUG, "[coleco_inputs] %d, %d", Port & 2, sms.Input_Mode);
    if ((Port & 2) == 0)
    {
        // Player 1
        if (sms.Input_Mode == 0)
            return (Coleco_Keypad_1 ());
        return (Coleco_Joy_Table_Conv [tsms.Control[7] & 0x3F]);
    }
    else
    {
        // Player 2
        if (sms.Input_Mode == 0)
            return (Coleco_Keypad_2 ());
        return (Coleco_Joy_Table_Conv [(tsms.Control[7] >> 8) & 0x3F]);
    }
}

// Colecovision keypad 1, emulation -------------------------------------------
byte    Coleco_Keypad_1 (void)
{
    int    v;

         if (Inputs_KeyDown(ALLEGRO_KEY_0))        v = 10; // 0
    else if (Inputs_KeyDown(ALLEGRO_KEY_1))        v = 13; // 1
    else if (Inputs_KeyDown(ALLEGRO_KEY_2))        v =  7; // 2
    else if (Inputs_KeyDown(ALLEGRO_KEY_3))        v = 12; // 3
    else if (Inputs_KeyDown(ALLEGRO_KEY_4))        v =  2; // 4
    else if (Inputs_KeyDown(ALLEGRO_KEY_5))        v =  3; // 5
    else if (Inputs_KeyDown(ALLEGRO_KEY_6))        v = 14; // 6
    else if (Inputs_KeyDown(ALLEGRO_KEY_7))        v =  5; // 7
    else if (Inputs_KeyDown(ALLEGRO_KEY_8))        v =  1; // 8
    else if (Inputs_KeyDown(ALLEGRO_KEY_9))        v = 11; // 9
    else if (Inputs_KeyDown(ALLEGRO_KEY_MINUS))    v =  9; // *
    else if (Inputs_KeyDown(ALLEGRO_KEY_EQUALS))   v =  6; // #
    else v = 0x0F;
    if (tsms.Control[7] & 0x20)
        v |= 0x40;
    return (v | 0x10 | 0x20 /*| 0x80*/);
}

// Colecovision keypad 2, non working emulation -------------------------------
// FIXME: ...
byte        Coleco_Keypad_2 (void)
{
    int     v = 0x0F;
    if ((tsms.Control[7] >> 8) & 0x20)
        v |= 0x40;
    return (v | 0x10 | 0x20 /*| 0x80*/);
}

// Build Coleco Input Conversion table
// This is because the input system currently fills input value for Sega 8-bit
void        Coleco_Init_Table_Inputs (void)
{
    int     i;

    /*
    Bit 0=Left                    1
    Bit 1=Down                    2
    Bit 2=Right                   4
    Bit 3=Up                      8
    Bit 6=Left button             64
    */

    for (i = 0; i <= 0x3F; i ++)
    {
        Coleco_Joy_Table_Conv [i] = 
            0x10 | 0x20 //| 0x80
            | ((i & 0x01) ? 0x01 : 0)
            | ((i & 0x02) ? 0x04 : 0)
            | ((i & 0x04) ? 0x08 : 0)
            | ((i & 0x08) ? 0x02 : 0)
            | ((i & 0x10) ? 0x40 : 0)
            //| ((i & 0x20) ? 0x20 : 0)
            ;
    }
}

word    Loop_Coleco (void)
{
    // Update sound cycle counter
    Sound.CycleCounter += opt.Cur_IPeriod;

    tsms.VDP_Line = (tsms.VDP_Line + 1) % g_machine.TV_lines;

    // Debugger hook
    #ifdef MEKA_Z80_DEBUGGER
	if (Debugger.active)
		Debugger_RasterLine_Hook(tsms.VDP_Line);
	#endif

    if (tsms.VDP_Line == 0)
    {
        Interrupt_Loop_Misc_Line_Zero();
    }

    if (tsms.VDP_Line >= 0 && tsms.VDP_Line < 192)
    {
        // Skip collision check if the sprite collision flag is already set
        if (!(sms.VDP_Status & VDP_STATUS_SpriteCollision))
            Check_Sprites_Collision_Modes_1_2_3_Line (tsms.VDP_Line);
    }

    if (tsms.VDP_Line == 192)
    {
        Interrupt_Loop_Misc_Common;
        if (fskipper.Show_Current_Frame)
            Refresh_Modes_0_1_2_3();

        // sms.VDP_Status &= ~VDP_STATUS_SpriteCollision;
        sms.VDP_Status |= VDP_STATUS_VBlank;
        //if (!(sms.VDP_Status & VDP_STATUS_SpriteCollision))
        //   Check_Sprites_Collision_Modes_1_2_3();

        // Note: refresh screen may reset the system, so you can NOT change
        // the status AFTER it, or else it would screw the newly emulated code
        Video_RefreshScreen();
        if ((opt.Force_Quit) || (CPU_Loop_Stop))
            Macro_Stop_CPU;
    }

    if ((VBlank_ON) && (sms.VDP_Status & VDP_STATUS_VBlank) && (sms.Pending_NMI == FALSE) /* && (sms.VDP_Access_Mode == VDP_Access_Mode_1) */ )
    {
        sms.VDP_Status &= ~VDP_STATUS_VBlank;
		sms.Pending_NMI = TRUE;
        return (INT_NMI);
    }

    return (INT_NONE);
}

//-----------------------------------------------------------------------------

