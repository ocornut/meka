//-----------------------------------------------------------------------------
// MEKA - cpu.c
// CPU related things (ports, memory accesses, interrupts) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "patch.h"
#include "vdp.h"
#include "video_m2.h"
#include "video_m5.h"
#include "debugger.h"

//-----------------------------------------------------------------------------
// Note: only MARAT_Z80 is functional/compiles now.
//-----------------------------------------------------------------------------

// Z80 scanline handler
#ifdef MDK_Z80
int     Z80_Interrupt (void)
#elif NEIL_Z80
int     LoopZ80 (void)
#else
word    Loop_SMS (void)
#endif
{
    int Interrupt = INT_NONE;

    tsms.VDP_Line = (tsms.VDP_Line + 1) % cur_machine.TV_lines;
    // Debugger hook
    #ifdef MEKA_Z80_DEBUGGER
		if (Debugger.active)
			Debugger_RasterLine_Hook(tsms.VDP_Line);
	#endif

    // Update sound cycle counter
    Sound_Update_Count += opt.Cur_IPeriod; // Should be made obsolete
    Sound_CycleCounter += 1;//opt.Cur_IPeriod;

    if (tsms.VDP_Line == 0)
    {
        Interrupt_Loop_Misc_Line_Zero ();
        cur_machine.VDP.scroll_x_latched = sms.VDP[8];
        cur_machine.VDP.scroll_y_latched = sms.VDP[9];
        sms.Lines_Left = sms.VDP [10];
    }

    // Screen Refresh
    if (tsms.VDP_Line >= cur_drv->y_show_start && tsms.VDP_Line <= cur_drv->y_show_end)
    {
        cur_machine.VDP.scroll_x_latched_table[tsms.VDP_Line] = cur_machine.VDP.scroll_x_latched;
        if (tsms.VDP_VideoMode > 3)
            Refresh_Line_5();
        cur_machine.VDP.scroll_x_latched = sms.VDP[8];
        if (cur_drv->vdp == VDP_TMS9918)
            Check_Sprites_Collision_Modes_1_2_3_Line (tsms.VDP_Line);
        if (tsms.VDP_Line == cur_drv->y_show_end)
        {
            //if (cur_drv->vdp == VDP_TMS)
            //   Check_Sprites_Collision_Modes_1_2_3 ();
            // Msg (MSGT_DEBUG, "Loop_SMS: Refresh_Screen()");
            Refresh_Screen ();
            if ((opt.Force_Quit) || (CPU_Loop_Stop))
                Macro_Stop_CPU;
        }
    }

    if (tsms.VDP_Line <= 192)
    {
        if (sms.Lines_Left -- <= 0)
        {
            sms.Lines_Left = sms.VDP [10];
            sms.Need_HBlank = TRUE;
            #ifdef DEBUG_VDP
                Msg (MSGT_DEBUG, "%d @ Lines_Left == 0, HBlank == %d, Reloading VDP[10] = %d", tsms.VDP_Line, (HBlank_ON) ? 1 : 0, sms.VDP [10]);
            #endif
        }
        if ((sms.Need_HBlank) && (HBlank_ON))
            Interrupt = INT_IRQ;
    }
    else
        // --------------------------------------------------------------------------
        // New interrupt code
        // --------------------------------------------------------------------------
#if 1
    {
        if (tsms.VDP_Line == cur_drv->y_int + 1)
            sms.VDP_Status |= VDP_STATUS_VBlank;
        else
            if (tsms.VDP_Line > cur_drv->y_int && tsms.VDP_Line <= (cur_drv->y_int + 32) && (sms.VDP_Status & VDP_STATUS_VBlank) && (VBlank_ON))
                Interrupt = INT_IRQ;
            else
                if (tsms.VDP_Line == cur_drv->y_int + 33)   // Interruption duration. Isn't that scary-lame?
                    Interrupt_Loop_Misc;
    }
#else
        // --------------------------------------------------------------------------
        // Original interrupt code
        // --------------------------------------------------------------------------
    {
        if (tsms.VDP_Line == 193)
            sms.VDP_Status |= VDP_STATUS_VBlank;
        else
            if (tsms.VDP_Line <= 224 && (sms.VDP_Status & 0x80) && (VBlank_ON))
                Interrupt = INT_IRQ;
            else
                if (tsms.VDP_Line == 225)
                    Interrupt_Loop_Misc;
    }
#endif
    // ----------------------------------------------------------------------------
    if (Interrupt == INT_IRQ)
    {
        #ifdef MARAT_Z80
            sms.R.IRequest = INT_IRQ;
        #elif MAME_Z80
            z80_set_irq_line (0, ASSERT_LINE);
        #endif
    }

    return (Interrupt);
}

//-----------------------------------------------------------------------------

#ifdef MAME_Z80
// MAIN EMULATION LOOP (MAME CORE) --------------------------------------------
void    CPU_Loop (void)
{
    for (;;)
    {
        z80_execute (opt.Cur_IPeriod);
        switch (LoopZ80 ())
        {
        case INT_IRQ: break;
        case INT_NMI: z80_set_nmi_line (ASSERT_LINE); z80_set_nmi_line (CLEAR_LINE); break;
        case INT_QUIT: return;
        }
    }
    /* z80_stop_emulating(); */
}
#endif

//-----------------------------------------------------------------------------

#ifdef RAZE_Z80
// MAIN EMULATION LOOP (RAZE CORE) --------------------------------------------
void    CPU_Loop (void)
{
    for (;;)
    {
        z80_emulate (opt.Cur_IPeriod);
        switch (LoopZ80 ())
        {
        case INT_IRQ: z80_raise_IRQ (0xff); z80_lower_IRQ (); break;
        case INT_NMI: z80_cause_NMI (); break;
        case INT_QUIT: return;
        }
    }
}
#endif

//-----------------------------------------------------------------------------

#ifdef NEIL_Z80
// MAIN EMULATION LOOP (NEIL'S CORE) ------------------------------------------
void    CPU_Loop (void)
{
    for (;;)
    {
        mz80exec (opt.Cur_IPeriod);
        mz80GetElapsedTicks (TRUE);
        switch (LoopZ80 ())
        {
        case INT_IRQ: mz80int (INT_IRQ); break;
        case INT_NMI: mz80nmi (); break;
        case INT_QUIT: return;
        }
    }
}
#endif

//-----------------------------------------------------------------------------
// Empty/Defaults Functions
//-----------------------------------------------------------------------------

#ifdef MARAT_Z80
    void  PatchZ80 (Z80 *R) { }
#endif

#ifdef MDK_Z80
    void  Z80_Patch (Z80_Regs *Regs) { }
    void  Z80_Reti (void) { }
    void  Z80_Retn (void) { }
#endif

#ifdef MAME_Z80
    INLINE int Get_Active_CPU (void)      { return (0);    }
    INLINE int Get_IRQ_Vector (int p)     { return (0x38); }
#endif

//-----------------------------------------------------------------------------

