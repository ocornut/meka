//-----------------------------------------------------------------------------
// MEKA - osd/misc.c
// OSD Miscellaneous Functions - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "osd/misc.h"

bool	OSD_ClipboardSetText(const char* text, const char* text_end)
{ 
#ifdef ARCH_WIN32
	if (!OpenClipboard(NULL))
		return false;

	if (!text_end)
		text_end = text + strlen(text);

	const int buf_length = (text_end - text) + 1;
	HGLOBAL buf_handle = GlobalAlloc(GMEM_MOVEABLE, buf_length * sizeof(char)); 
	if (buf_handle == NULL)
		return false;

	char* buf_global = (char *)GlobalLock(buf_handle); 
	sprintf(buf_global, "%.*s", text_end - text, text);
	GlobalUnlock(buf_handle); 

	EmptyClipboard();
	SetClipboardData(CF_TEXT, buf_handle);
	CloseClipboard();

	return true;
#else
	return false;
#endif
}

char*	OSD_ClipboardGetText()
{
#ifdef ARCH_WIN32
	if (!OpenClipboard(NULL)) 
		return NULL;

	HANDLE buf_handle = GetClipboardData(CF_TEXT); 
	if (buf_handle == NULL)
		return NULL;

	char* buf_global = (char*)GlobalLock(buf_handle); 
	char* buf_local = buf_global ? strdup(buf_global) : NULL;
	GlobalUnlock(buf_handle); 
	CloseClipboard(); 

	return buf_local;
#else
	return NULL;
#endif
}

//-----------------------------------------------------------------------------
// OSD_X86CPU_Has_RDTSC (void)
// Return whether the return X86 CPU has RDTSC
//-----------------------------------------------------------------------------
bool    OSD_X86CPU_Has_RDTSC (void)
{
#ifdef ARCH_MACOSX
    return TRUE;
#endif // ARCH_MACOSX

	int cpu_id;

    //if (!(cpu_capabilities & CPU_ID))
    //    return false;

#ifdef ARCH_WIN32

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

    // Msg(MSGT_DEBUG, "cpu_id = %08X", cpu_id);

    if (cpu_id & 0x10)
        return true;
    return false;
}

//-----------------------------------------------------------------------------
