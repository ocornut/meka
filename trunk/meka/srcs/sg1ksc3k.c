//-----------------------------------------------------------------------------
// MEKA - sg1ksc3k.c
// SG-1000/SC-3000 Specifics Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fskipper.h"
#include "inputs_t.h"
#include "patch.h"
#include "vdp.h"
#include "video.h"
#include "video_m2.h"
#include "debugger.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

word    Loop_SG1000_SC3000 (void)
{
    int Interrupt = INT_NONE;

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
        Interrupt_Loop_Misc;
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
        if (fskipper.Show_Current_Frame)
        {
            // Msg(MSGT_DEBUG, "Loop_SG1000_SC3000: Refresh_Modes_0_1_2_3()");
            Refresh_Modes_0_1_2_3();
        }

        sms.VDP_Status |= VDP_STATUS_VBlank;
        //if (!(sms.VDP_Status & VDP_STATUS_SpriteCollision))
        //   Check_Sprites_Collision_Modes_1_2_3();

        // Note: refresh screen may reset the system, so you can NOT change
        // the status AFTER it, or else it would screw the newly emulated code
        // Msg(MSGT_DEBUG, "Loop_SG1000_SC3000: Video_RefreshScreen()");
        Video_RefreshScreen();

        if ((opt.Force_Quit) || (CPU_Loop_Stop))
            Macro_Stop_CPU;
    }

    if ((VBlank_ON) /* && (sms.VDP_Access_Mode == VDP_Access_Mode_1) */
        && (sms.VDP_Status & VDP_STATUS_VBlank))
    {
        Interrupt = INT_IRQ;
        // Msg(MSGT_DEBUG, "At PC=%04X: V-Blank", CPU_GetPC);
    }

    if (Interrupt == INT_IRQ)
    {
        #ifdef MARAT_Z80
            sms.R.IRequest = Interrupt;
        #elif MAME_Z80
            z80_set_irq_line (0, ASSERT_LINE);
        #endif
    }
    else
        // Note: NMI should have the priority over standard Interrupts (by definition)
        // but this behavior is weird to emulate, and should not to be needed in the
        // case we are using CPU_ForceNMI now.
        if (CPU_ForceNMI)
        {
            CPU_ForceNMI = FALSE;
            sms.R.IRequest = Interrupt;
            return (INT_NMI);
        }

        return (Interrupt);
}

//-----------------------------------------------------------------------------

