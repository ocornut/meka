//-----------------------------------------------------------------------------
// MEKA - osd/misc.h
// OSD Miscellaneous Functions - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

bool		OSD_ClipboardSetText(const char* text, const char* text_end = NULL);
char *		OSD_ClipboardGetText();					// User takes memory ownership

//-----------------------------------------------------------------------------
// Inlines
//-----------------------------------------------------------------------------

static INLINE s64  OSD_X86CPU_RDTSC (void)
{
    s64     result;

    #ifdef ARCH_WIN32
    s64 *   result_addr = &result;
    __asm
    {
        rdtsc
        mov ebx, result_addr
        mov [ebx], eax
        mov [ebx+4], edx
    }
    #else
    __asm__ __volatile__ 
    (
        "rdtsc"
	: "=A" (result)
    );
    #endif
    return (result);
}

//-----------------------------------------------------------------------------
