//-----------------------------------------------------------------------------
// MEKA - timer.c
// OSD Timer - Code
//-----------------------------------------------------------------------------
// Most of the theory behind this code comes from MAME.
// Not really used as of yet...
//-----------------------------------------------------------------------------
// MS-DOS:
// - RDTSC
// - uclock()
// MS-Windows:
// - QueryPerformanceCounter()  // FIXME: yet unused
// - RDTSC
// - timeGetTime()
//-----------------------------------------------------------------------------

#include "shared.h"
#include "osd/timer.h"
#include "osd/misc.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

s64     timer_cycles_per_second = 0;
s64     (*timer_func_get_cycles_current) (void) = NULL;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static s64  OSD_Timer_GetCyclesCurrent_RDTSC                (void);
#ifdef ARCH_DOS
static s64  OSD_Timer_GetCyclesCurrent_DOS_uclock           (void);
#endif
#ifdef ARCH_WIN32
static s64  OSD_Timer_GetCyclesCurrent_Win32_timeGetTime    (void);
#endif

//-----------------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSD_Timer_Initialize (void)
//-----------------------------------------------------------------------------
#ifdef ARCH_WIN32

void        OSD_Timer_Initialize (void)
{
    int     thread_priority;
    DWORD   a, b;
    s64     time_start, time_end;

    // Select timer to use
    if (OSD_X86CPU_Has_RDTSC ())
    {
        timer_func_get_cycles_current = OSD_Timer_GetCyclesCurrent_RDTSC;
    }
    else
    {
        timer_func_get_cycles_current = OSD_Timer_GetCyclesCurrent_Win32_timeGetTime;
    }

    // Temporarily set our priority higher
    thread_priority = GetThreadPriority(GetCurrentThread());
    SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    // Wait for an edge on the timeGetTime call
    a = timeGetTime();
    do { b = timeGetTime(); } while (a == b);

    // Get the starting cycle count
    time_start = (*timer_func_get_cycles_current)();

    // Now wait for 1/4 second total
    // Note: can we afford waiting less than that?
    do { a = timeGetTime(); } while (a - b < 250);

    // Get the ending cycle count
    time_end = (*timer_func_get_cycles_current)();

    // Compute timer_cycles_per_second
    timer_cycles_per_second = (time_end - time_start) * 4;

    // Restore thread priority
    SetThreadPriority(GetCurrentThread(), thread_priority);
}

#elif ARCH_DOS

void        OSD_Timer_Initialize (void)
{
    // Select timer to use
    if (OSD_X86CPU_Has_RDTSC ())
    {
        DWORD   a, b;
        s64     time_start, time_end;

        timer_func_get_cycles_current = OSD_Timer_GetCyclesCurrent_RDTSC;

        a = uclock();
        // Wait some time to let everything stabilize
        do
        {
            b = uclock();
            // get the starting cycle count
            time_start = OSD_Timer_GetCyclesCurrent_RDTSC();
        } while (b - a < UCLOCKS_PER_SEC/2);

        // Now wait for 1/2 second
        do
        {
            a = uclock();
            // get the ending cycle count
            time_end = OSD_Timer_GetCyclesCurrent_RDTSC();
        } while (a - b < UCLOCKS_PER_SEC/2);

        // Compute timer_cycles_per_second
        timer_cycles_per_second = (time_end - time_start) * 2;
    }
    else
    {
        Quit_Msg ("Error initializating timer!");
    }
    /*
    else
    {
        timer_func_get_cycles_current = OSD_Timer_GetCyclesCurrent_DOS_uclock;
        timer_cycles_per_second = UCLOCKS_PER_SEC;
    }
    */
}

#endif

//-----------------------------------------------------------------------------
// OSD_Timer_GetCyclesCurrent (void)
//-----------------------------------------------------------------------------
s64     OSD_Timer_GetCyclesCurrent (void)
{
    return timer_func_get_cycles_current();
}

static s64  OSD_Timer_GetCyclesCurrent_RDTSC (void)
{
    return OSD_X86CPU_RDTSC();
}

#ifdef ARCH_DOS
static s64  OSD_Timer_GetCyclesCurrent_DOS_uclock (void)
{
    return uclock();
}
#endif

#ifdef ARCH_WIN32
static s64  OSD_Timer_GetCyclesCurrent_Win32_timeGetTime (void)
{
    return (s64)timeGetTime();
}
#endif

//-----------------------------------------------------------------------------
// OSD_Timer_GetCyclesPerSecond (void)
//-----------------------------------------------------------------------------
s64     OSD_Timer_GetCyclesPerSecond (void)
{
    return timer_cycles_per_second;
}

//-----------------------------------------------------------------------------
