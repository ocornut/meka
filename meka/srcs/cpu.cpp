//-----------------------------------------------------------------------------
// MEKA - cpu.c
// CPU related things (ports, memory accesses, interrupts) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "inputs_t.h"
#include "patch.h"
#include "vdp.h"
#include "video.h"
#include "video_m2.h"
#include "video_m5.h"
#include "debugger.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

int     CPU_Loop_Stop;  // Set to break from CPU emulation and return to mainloop()
int     CPU_ForceNMI;   // Set to force a NMI (currently only supported by the SG-1000/SC-3000 loop handlers)

//-----------------------------------------------------------------------------
// Note: only MARAT_Z80 is functional/compiles now.
//-----------------------------------------------------------------------------

void Interrupt_Loop_Misc_Line_Zero()
{
    if (tsms.VDP_Video_Change)
        VDP_VideoMode_Change();
    Patches_MEM_Apply();
}

bool Interrupt_Loop_Misc_Common()
{
    if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
        VGM_NewFrame(&Sound.LogVGM);
    Sound_Update();
    tsms.Control_Check_GUI = TRUE;
    Inputs_Sources_Update(); // Poll input sources
    Inputs_Emulation_Update(TRUE); // Might disable Control_Check_GUI
    Inputs_Check_GUI(!tsms.Control_Check_GUI);
    if ((opt.Force_Quit) || (CPU_Loop_Stop))
    {
        /*tsms.VDP_Line --;*/ // Not sure about its usefulness
        return true; // -> will call Macro_Stop_CPU
    }
    return false;
}

bool Interrupt_Loop_Misc(int* out_interrupt)
{
    bool stop = Interrupt_Loop_Misc_Common();
    if (Inputs.SK1100_Enabled)
    {
        if (Inputs_KeyDown(ALLEGRO_KEY_SCROLLLOCK))
            *out_interrupt = INT_NMI;
    }
    else if ((tsms.Control_Start_Pause == 1) && (g_driver->id != DRV_GG))
    {
        tsms.Control_Start_Pause = 2;
        *out_interrupt = INT_NMI;
    }
    return stop;
}

// Z80 scanline handler
word    Loop_SMS()
{
    int interrupt = INT_NONE;

    tsms.VDP_Line = (tsms.VDP_Line + 1) % g_machine.TV_lines;
    // Debugger hook
    #ifdef MEKA_Z80_DEBUGGER
        if (Debugger.active)
            Debugger_RasterLine_Hook(tsms.VDP_Line);
    #endif

    // Update sound cycle counter0
    Sound.CycleCounter += opt.Cur_IPeriod;

    if (tsms.VDP_Line == 0)
    {
        Interrupt_Loop_Misc_Line_Zero();
        g_machine.VDP.scroll_x_latched = sms.VDP[8];
        g_machine.VDP.scroll_y_latched = sms.VDP[9];
        sms.Lines_Left = sms.VDP [10];
    }

    // Screen Refresh
    if (tsms.VDP_Line >= g_driver->y_show_start && tsms.VDP_Line <= g_driver->y_show_end)
    {
        g_machine.VDP.scroll_x_latched_table[tsms.VDP_Line] = g_machine.VDP.scroll_x_latched;
        if (tsms.VDP_VideoMode > 3)
            Refresh_Line_5();
        g_machine.VDP.scroll_x_latched = sms.VDP[8];
        if (g_driver->vdp == VDP_TMS9918)
            Check_Sprites_Collision_Modes_1_2_3_Line (tsms.VDP_Line);
        if (tsms.VDP_Line == g_driver->y_show_end)
        {
            //if (g_driver->vdp == VDP_TMS)
            //   Check_Sprites_Collision_Modes_1_2_3();
            // Msg(MSGT_DEBUG, "Loop_SMS: Video_RefreshScreen()");
            Video_RefreshScreen();
            if ((opt.Force_Quit) || (CPU_Loop_Stop))
                Macro_Stop_CPU;
        }
    }

    if (tsms.VDP_Line <= g_driver->y_int)
    {
        if (sms.Lines_Left -- <= 0)
        {
            sms.Lines_Left = sms.VDP [10];
            sms.Pending_HBlank = TRUE;
            #ifdef DEBUG_VDP
                Msg(MSGT_DEBUG, "%d @ Lines_Left == 0, HBlank == %d, Reloading VDP[10] = %d", tsms.VDP_Line, (HBlank_ON) ? 1 : 0, sms.VDP [10]);
            #endif
        }
        if ((sms.Pending_HBlank) && (HBlank_ON))
            interrupt = INT_IRQ;
    }
    else
#if 1
    {
        // --------------------------------------------------------------------------
        // New interrupt code
        // --------------------------------------------------------------------------
        // FIXME: Needless to say, this is vastly incorrect but has given pretty
        // good result so far.
        // --------------------------------------------------------------------------
        if (tsms.VDP_Line == g_driver->y_int + 1)
            sms.VDP_Status |= VDP_STATUS_VBlank;
        else if (tsms.VDP_Line > g_driver->y_int && tsms.VDP_Line <= (g_driver->y_int + 32) && (sms.VDP_Status & VDP_STATUS_VBlank) && (VBlank_ON))
            interrupt = INT_IRQ;
        else if (tsms.VDP_Line == g_driver->y_int + 33)   // Interruption duration. Isn't that scary-lame?
            if (Interrupt_Loop_Misc(&interrupt))
                Macro_Stop_CPU;
    }
#else
        // --------------------------------------------------------------------------
        // Original interrupt code
        // --------------------------------------------------------------------------
    {
        if (tsms.VDP_Line == 193)
            sms.VDP_Status |= VDP_STATUS_VBlank;
        else if (tsms.VDP_Line <= 224 && (sms.VDP_Status & 0x80) && (VBlank_ON))
            interrupt = INT_IRQ;
        else if (tsms.VDP_Line == 225)
            if (Interrupt_Loop_Misc(&interrupt))
                Macro_Stop_CPU;
    }
#endif
    // ----------------------------------------------------------------------------
    if (interrupt == INT_IRQ)
    {
        #ifdef MARAT_Z80
            sms.R.IRequest = INT_IRQ;
        #elif MAME_Z80
            z80_set_irq_line (0, ASSERT_LINE);
        #endif
    }

    return (interrupt);
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
        case INT_IRQ: z80_raise_IRQ (0xff); z80_lower_IRQ(); break;
        case INT_NMI: z80_cause_NMI(); break;
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

#ifdef MAME_Z80
    INLINE int Get_Active_CPU (void)      { return (0);    }
    INLINE int Get_IRQ_Vector (int p)     { return (0x38); }
#endif

//-----------------------------------------------------------------------------

