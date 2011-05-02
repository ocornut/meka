//-----------------------------------------------------------------------------
// MEKA - fskipper.c
// Frame Skipper - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fskipper.h"
#include "glasses.h"
#include "osd/misc.h"
#include "osd/timer.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Frame_Skipper_Init_Values (void)
{
    fskipper.Mode                       = FRAMESKIP_MODE_AUTO;
    fskipper.Automatic_Speed            = 60;
    fskipper.Automatic_Frame_Elapsed    = 0;
    fskipper.Standard_Frameskip         = 1; // 1/1
    fskipper.Standard_Counter           = 0;
    fskipper.Show_Current_Frame         = TRUE;
    fskipper.FPS                        = 0.0f;
    fskipper.FPS_Display                = FALSE;
    //fskipper.FPS_LastComputedTime       = 0;
    fskipper.New_Second                 = FALSE;
    fskipper.Frame_Rendered             = 0;
}

void    Frame_Skipper_Auto_Adjust_Handler (void)
{
    fskipper.Automatic_Frame_Elapsed ++;
}
END_OF_FUNCTION (Frame_Skipper_Auto_Adjust_Handler);

void    Frame_Skipper_New_Second_Handler (void)
{
    fskipper.New_Second = TRUE;
}
END_OF_FUNCTION (Frame_Skipper_New_Second_Handler);

// Calculate the nearest and most appropriate value for the auto frame
// skipper timed interrupt.
void    Frame_Skipper_Auto_Install_Handler (void)
{
    int   c;

    /*
    int   c1, c2;
    c = 1000 / fskipper.Automatic_Speed;
    c1 = 1000 - (c * fskipper.Automatic_Speed);
    c2 = 1000 - ((c + 1) * fskipper.Automatic_Speed);
    //if (Meka_State == MEKA_STATE_GUI)
    //    Msg (MSGT_DEBUG, "%d hz : c1=%d | c2=%d", fskipper.Automatic_Speed, c1, c2);
    if (c1 < 0) c1 = -c1;
    if (c2 < 0) c2 = -c2;
    if (c2 < c1) c += 1;
    install_int (Frame_Skipper_Auto_Adjust_Handler, c);
    */

#if 0	// FIXME-ALLEGRO5: auto frame skipper
    c = TIMERS_PER_SECOND / fskipper.Automatic_Speed;
    install_int_ex (Frame_Skipper_Auto_Adjust_Handler, c);
#endif

    //TIMERS_PER_SECOND     1193181
    //MSEC_TO_TIMER(x)      ((long)(x) * (TIMERS_PER_SECOND / 1000))

}

void    Frame_Skipper_Auto_Reinstall_Handler (void)
{
#if 0	// FIXME-ALLEGRO5: auto frame skipper
    remove_int (Frame_Skipper_Auto_Adjust_Handler);
#endif
	Frame_Skipper_Auto_Install_Handler ();
}

void    Frame_Skipper_Init (void)
{
    // Auto Frame Skipper
#if 0	// FIXME-ALLEGRO5: auto frame skipper
    LOCK_VARIABLE (fskipper.Automatic_Frame_Elapsed);
    LOCK_FUNCTION (Frame_Skipper_Auto_Adjust_Handler);
    Frame_Skipper_Auto_Install_Handler ();
#endif
    // FPS Counter
#if 0	// FIXME-ALLEGRO5: FPS
	LOCK_VARIABLE (fskipper.New_Second);
    LOCK_FUNCTION (Frame_Skipper_New_Second_Handler);
    Frame_Skipper_New_Second_Handler ();
    install_int_ex (Frame_Skipper_New_Second_Handler, TIMERS_PER_SECOND);
#endif
}

//-----------------------------------------------------------------------------
// Frame Skipper
// Return FALSE if next frame is to be skipped, else TRUE
//-----------------------------------------------------------------------------
int     Frame_Skipper (void)
{
    //s64 cycle_current = OSD_Timer_GetCyclesCurrent(); 
    //const s64 cycle_per_second = OSD_Timer_GetCyclesPerSecond();
    //OSD_X86CPU_RDTSC();

    // Auto frame-skipping ----------------------------------------------------
    if (fskipper.Mode == FRAMESKIP_MODE_AUTO)
    {
        // Slow down to skip appropriate frames
        // FIXME: this takes 100% CPU and seems not to work well everywhere :(
        while (fskipper.Automatic_Frame_Elapsed == 0)
        {
            #ifdef ARCH_UNIX
                // pause (); // Wait for an interrupt
            #endif
            //#ifdef ARCH_WIN32
              //rest(4);
              //yield_timeslice();
            // FIXME-ALLEGRO5
			//rest(1);
			al_rest(0.01f);
            //#endif
        }

        // If retard is too high, force drawing a frame so it doesn't freeze
        // It is also good since huge delay (+1/4th second) will not be catched
        if (fskipper.Automatic_Frame_Elapsed > 15)
        { 
            fskipper.Automatic_Frame_Elapsed = 1;
        }

        // Skip next frame if we have more than one to go (we're late)
        // Else don't skip
        if (fskipper.Automatic_Frame_Elapsed -- > 1)
            return FALSE;

        // Software 3-D glasses emulation may require to skip this frame
        if (Glasses.Enabled && Glasses_Must_Skip_Frame ())
            return FALSE;
    }
    else
    // Standard frame-skipping ------------------------------------------------
    {
        // Software 3-D glasses emulation may require to skip this frame
        if (Glasses.Enabled && Glasses_Must_Skip_Frame ())
            return FALSE;

        // Skip Standard_Counter-1 frames every Standard_Counter frames
        if (fskipper.Standard_Counter < fskipper.Standard_Frameskip)
        {
            fskipper.Standard_Counter ++;
            return FALSE;
        }
        fskipper.Standard_Counter = 1;
    }

    fskipper.Frame_Rendered++;

    // Compute FPS if a new second has elapsed
    //Msg (MSGT_DEBUG, "%d, %016I64x, %016I64x, %016I64x", fskipper.Frame_Rendered, fskipper.FPS_LastComputedTime, cycle_per_second, cycle_current);
    //if (fskipper.FPS_LastComputedTime == 0 || (cycle_current >= fskipper.FPS_LastComputedTime + cycle_per_second))
    if (fskipper.New_Second)
    {
        //int elapsed = (int)(cycle_current - fskipper.FPS_LastComputedTime);
        //int fps = (fskipper.Frame_Rendered * cycle_per_second + (cycle_per_second / 2)) / elapsed;
        //fskipper.FPS = fps;
        //Msg (MSGT_DEBUG, "Frame_Rendered = %d, FPS = %d", fskipper.Frame_Rendered, fskipper.FPS);
        fskipper.New_Second = FALSE;
        fskipper.FPS = fskipper.Frame_Rendered;
        fskipper.Frame_Rendered = 0;
        //if (fskipper.FPS_LastComputedTime == 0)
            //fskipper.FPS_LastComputedTime = cycle_current;
        //else
            //fskipper.FPS_LastComputedTime += cycle_per_second;

        // /NIRV mode :) for Nirv who likes to do benchmarking and once made a scandal about it
        if (g_Configuration.slash_nirv)
            fskipper.FPS += 120;
    }

    return TRUE; // Will show next frame
}

// CHANGE FRAMESKIP VALUE -----------------------------------------------------
void    Frame_Skipper_Switch (void)
{
    if (fskipper.Mode == FRAMESKIP_MODE_AUTO)
    {
        fskipper.Standard_Counter = 1;
        fskipper.Mode = FRAMESKIP_MODE_STANDARD;
    }
    else
    {
        fskipper.Automatic_Frame_Elapsed = 0;
        fskipper.Mode = FRAMESKIP_MODE_AUTO;
    }
    Frame_Skipper_Show ();
}

void    Frame_Skipper_Configure (int v)
{
    switch (fskipper.Mode)
    {
    case FRAMESKIP_MODE_AUTO:
        {
            fskipper.Automatic_Speed += (v * 10);
            if (fskipper.Automatic_Speed < 10)      // Min 10 Hz
                fskipper.Automatic_Speed = 10;
            if (fskipper.Automatic_Speed > 400)     // Max 400 Hz
                fskipper.Automatic_Speed = 400;
            Frame_Skipper_Auto_Reinstall_Handler ();
            break;
        }
    case FRAMESKIP_MODE_STANDARD:
        {
            fskipper.Standard_Frameskip += v;
            if (fskipper.Standard_Frameskip < 1)    // Min 1/1
                fskipper.Standard_Frameskip = 1;
            if (fskipper.Standard_Frameskip > 9)    // Max 1/9
                fskipper.Standard_Frameskip = 9;
            break;
        }
    }
    Frame_Skipper_Show ();
}

void    Frame_Skipper_Show (void)
{
    if (fskipper.Mode == FRAMESKIP_MODE_AUTO)
        Msg (MSGT_USER, Msg_Get (MSG_Frameskip_Auto), fskipper.Automatic_Speed);
    else
        Msg (MSGT_USER, Msg_Get (MSG_Frameskip_Standard), fskipper.Standard_Frameskip);
}

void    Frame_Skipper_Switch_FPS_Counter (void)
{
    fskipper.FPS_Display = !fskipper.FPS_Display;
    if (fskipper.FPS_Display)
        Msg (MSGT_USER, Msg_Get (MSG_FPS_Counter_Enabled));
    else
        Msg (MSGT_USER, Msg_Get (MSG_FPS_Counter_Disabled));
}

//-----------------------------------------------------------------------------

