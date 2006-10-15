//-----------------------------------------------------------------------------
// MEKA - osd/misc.c
// OSD Miscellaenous Functions - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "osd/misc.h"

//-----------------------------------------------------------------------------
// OSD_X86CPU_Has_RDTSC (void)
// Return weither the return X86 CPU has RDTSC
//-----------------------------------------------------------------------------
bool    OSD_X86CPU_Has_RDTSC (void)
{
    int cpu_id;

    //if (!(cpu_capabilities & CPU_ID))
    //    return (FALSE);

    #ifdef WIN32

    __asm
    {
        mov eax, 1
        cpuid
        mov cpu_id, edx
    }

    #else

    __asm__ (
        "movl $1,%%eax     ; "
        "xorl %%ebx,%%ebx  ; "
        "xorl %%ecx,%%ecx  ; "
        "xorl %%edx,%%edx  ; "
        "cpuid             ; "
        "movl %%edx,%%eax  ; "
        :  "=&a" (cpu_id)   // result from eax
	:                   // no inputs
	:  "%ebx", "%ecx", "%edx"
	);
    #endif

    // Msg (MSGT_DEBUG, "cpu_id = %08X", cpu_id);

    if (cpu_id & 0x10)
        return (TRUE);
    return (FALSE);
}

//-----------------------------------------------------------------------------
