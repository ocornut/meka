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
