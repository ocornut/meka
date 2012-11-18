//-----------------------------------------------------------------------------
// MEKA - errors.c
// Error codes and handling
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

int meka_errno = MEKA_ERR_OK;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// FIXME: switch that to a table with the #define inside
const char *  meka_strerror(void)
{
	switch (meka_errno)
	{
	case MEKA_ERR_MEMORY:            return (Msg_Get(MSG_Error_Memory));
	case MEKA_ERR_FILE_OPEN:         return (Msg_Get(MSG_Error_File_Not_Found));
	case MEKA_ERR_FILE_READ:         return (Msg_Get(MSG_Error_File_Read));
	case MEKA_ERR_FILE_EMPTY:        return (Msg_Get(MSG_Error_File_Empty));
	case MEKA_ERR_ZIP_NOT_SUPPORTED: return (Msg_Get(MSG_Error_ZIP_Not_Supported));
	case MEKA_ERR_ZIP_LOADING:       return (Msg_Get(MSG_Error_ZIP_Loading));
	case MEKA_ERR_ZIP_INTERNAL:      return (Msg_Get(MSG_Error_ZIP_Internal));
	}
	return (Msg_Get(MSG_Error_Error));
}

#ifdef ARCH_WIN32
static LPVOID   GetWindowsErrorMessage(void)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);
	return (lpMsgBuf);
}

void            ShowWindowsErrorMessage(void)
{
	LPVOID lpMsgBuf;

	lpMsgBuf = GetWindowsErrorMessage();
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
	LocalFree(lpMsgBuf);
}
#endif

//-----------------------------------------------------------------------------

