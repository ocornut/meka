//-----------------------------------------------------------------------------
// MEKA - tools.c
// Various tools - Code
//-----------------------------------------------------------------------------
// FIXME: OBSOLETE FUNCTIONS. Remove all whenever possible.
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

void    StrPath_RemoveExtension(char* buf)
{
	// Assume single dotted extension
	char *p = strrchr(buf, '.');
	if (p != NULL) 
		*p = EOSTR;
}

void    StrPath_GetExtension(char* buf)
{
	const char *p = strrchr (buf, '.');
	if (p == NULL) 
		return;

	char tmp[FILENAME_LEN];
	strcpy(tmp, p + 1);
	strcpy(buf, tmp);
}

void    StrPath_RemoveDirectory(char* buf)
{
	char tmp[FILENAME_LEN];

	const char* p = strrchr(buf, '/');

#ifndef ARCH_UNIX
    const char *p2 = strrchr(buf, '\\');
    if (p2 != NULL && p2 > p)
        p = p2;
#endif

	if (p != NULL)
	{
		p++;
		strcpy (tmp, p);
		strcpy (buf, tmp);
	}
}

// Extract filename from 'src' and copy it to location 'dst'
void    StrPath_RemoveDirectory(char *dst, const char *src)
{
    const char* p = strrchr(src, '/');

#ifndef ARCH_UNIX
    const char *p2 = strrchr(src, '\\');
    if (p2 != NULL && p2 > p)
        p = p2;
#endif

    p = (p ? p + 1 : src);
    strcpy (dst, p);
}

//-----------------------------------------------------------------------------

