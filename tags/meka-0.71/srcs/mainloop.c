//-----------------------------------------------------------------------------
// MEKA - mainloop.c
// Main Emulation / GUI loop - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "debugger.h"
#include "fskipper.h"
#include "osd/misc.h"

//-----------------------------------------------------------------------------

void    Main_Loop (void)
{
    #ifdef DEBUG_WHOLE
        Msg (MSGT_DEBUG, "Main_Loop ();");
    #endif

    // Print debugging information
    if (Debug_Print_Infos)
    {
        Msg (MSGT_DEBUG, "### Debugging information ###");
        Msg (MSGT_DEBUG, "Meka %s (%s %s)", VERSION, MEKA_BUILD_DATE, MEKA_BUILD_TIME);
        Msg (MSGT_DEBUG, "allegro_id = %s", allegro_id);
        Msg (MSGT_DEBUG, "os_type = { %c%c%c%c (%d.%d) }", os_type>>24, (os_type>>16)&0xFF, (os_type>>8)&0xFF, (os_type)&0xFF, os_version, os_revision);
        Msg (MSGT_DEBUG, "cpu = { fpu: %d, mmx:%d, 3dnow:%d-%d, sse:%d, sse2:%d }", CPU_FPU?1:0, CPU_MMX?1:0, CPU_3DNOW?1:0, CPU_ENH3DNOW?1:0, CPU_SSE?1:0, CPU_SSE2?1:0);
        //Msg (MSGT_DEBUG, "cpu = { has_rdtsc: %d }", OSD_X86CPU_Has_RDTSC ());
        Msg (MSGT_DEBUG, "gui = { driver:%ld, %d*%d @ %d Hz, depth:%d }", cfg.GUI_Driver, cfg.GUI_Res_X, cfg.GUI_Res_Y, cfg.GUI_Refresh_Rate, cfg.Video_Depth);
        Msg (MSGT_DEBUG, "mouse = %d", cfg.Mouse_Installed);
        Msg (MSGT_DEBUG, "joystick = %d", num_joysticks);
        Msg (MSGT_DEBUG, "sound = { enable:%d, init:%d, soundcard:%d }", Sound.Enabled, Sound.Initialized, Sound.SoundCard);
        Msg (MSGT_DEBUG, "samplerate = { %d, audio=%d, nominal=%d }", Sound.SampleRate, audio_sample_rate, nominal_sample_rate);
        Msg (MSGT_DEBUG, "saGetSoundRate() = %d", saGetSoundRate());
        #ifdef MEKA_OPL
            Msg (MSGT_DEBUG, "sound.opl = { address: %d }", Sound.OPL_Address);
        #endif
        Msg (MSGT_DEBUG, "### Debugging information ###");
    }

    for (;;)
    {
        #ifdef DEBUG_WHOLE
            Msg (MSGT_DEBUG, "Main_Loop(), Loop;");
        #endif
        CPU_Loop_Stop = NO;
        if ((machine & (MACHINE_POWER_ON | MACHINE_PAUSED)) == MACHINE_POWER_ON)
        {
            Sound_Playback_Start ();
            if (cur_drv->id == DRV_NES)
            {
                Run6502 (&nes->Regs);
            }
            else
            {
                #ifdef MARAT_Z80
                    #ifdef MEKA_Z80_DEBUGGER
                        if (Debugger.Active /* && (Debugger.break_point_set || sms.R.Trace)*/)
                        {
                            // Msg (MSGT_USER, "Entering RunZ80_Debugging()");
                            RunZ80_Debugging (&sms.R);
                        }
                        else
                    #endif
                        {
                            // Msg (MSGT_USER, "Entering RunZ80()");
                            RunZ80 (&sms.R);
                        }
                #elif MDK_Z80
                    Z80 ();
                #else
                    CPU_Loop ();
                #endif
            }
        }
        else // Machine is powered off or paused
        {
            Sound_Playback_Stop ();
            Main_Loop_No_Emulation ();
        }
        if (opt.Force_Quit)
        {
            break;
        }
    }

    // Clear screen to black, so that palette switch doesn't look ugly when quitting
    clear_to_color (screen, Border_Color);  // FIXME: Border_Color should be actually emulated properly one day, then it won't be Black anymore

    // Stop sound
    Sound_Playback_Stop ();
}

// MAIN LOOP WHEN NO ROM IS LOADED --------------------------------------------
void    Main_Loop_No_Emulation (void)
{
#ifdef DEBUG_WHOLE
    Msg (MSGT_DEBUG, "Main_Loop_No_Emulation ();");
#endif

    if (!(machine & MACHINE_POWER_ON))
        Effects_TV_Init_Colors ();

    for (;;)
    {
        Inputs_Sources_Update ();
        Inputs_Emulation_Update (FALSE); // [Omar-20050327] Allows changing inputs data from the debugger.// FIXME: but the debugger has exclusive inputs.
        Inputs_Check_GUI (FALSE);
        if ((opt.Force_Quit) || ((machine & (MACHINE_POWER_ON | MACHINE_PAUSED)) == MACHINE_POWER_ON))
        {
            return;
        }
        // FIXME: debugger there
        // Update TV effect or Games
        if ((fskipper.Show_Current_Frame) && (!(machine & MACHINE_POWER_ON)))
        {
            sms.VDP[0] &= ~0x20; // no mask left 8 (for GUI windows) // FIXME: blah
            switch (game_running)
            {
            case GAME_RUNNING_NONE:
                if (effects.TV_Enabled)
                {
                    Effects_TV_Update ();
                };
                break;
            case GAME_RUNNING_BREAKOUT:
                BreakOut_Update ();
                break;
            case GAME_RUNNING_TETRIS:
                Tetris_Update ();
                break;
            case GAME_RUNNING_BRAINWASH:
                BrainWash_Update ();
                break;
            case GAME_RUNNING_PONG:
                Pong_Update ();
                break;
            }
        }
        // Refresh GUI screen
        Refresh_Screen ();
    }
}

//-----------------------------------------------------------------------------

