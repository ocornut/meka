//-----------------------------------------------------------------------------
// MEKA - tools.c
// Various tools - Code
//-----------------------------------------------------------------------------
// FIXME: OBSOLETE FUNCTIONS. Remove all whenever possible.
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

// KILL EXTENSION FROM A FILENAME ---------------------------------------------
// FIXME: move to libmy
void    killext (char *org)
{
	char *p = strrchr (org, '.');
	if (p != NULL) 
		*p = EOSTR;
}

// KEEP EXTENSION FROM A FILENAME ---------------------------------------------
// FIXME: move to libmy
void    keepext (char *org)
{
	char s [FILENAME_LEN];
	char *p = strrchr (org, '.');

	if (p == NULL) 
		return;
	strcpy (s, p + 1);
	strcpy (org, s);
}

// KILL PATH FROM A FILENAME --------------------------------------------------
// FIXME: move to libmy
void    killpath (char *org)
{
	char   *p;
	char   str1 [FILENAME_LEN];

	if ((p = strrchr (org, '/')))
	{
		p ++;
		strcpy (str1, p);
		strcpy (org, str1);
	}
#ifndef ARCH_UNIX
	else if ((p = strrchr (org, '\\')))
	{
		p ++;
		strcpy (str1, p);
		strcpy (org, str1);
	}
#endif
}

// Extract filename from 'src' and copy it to location 'dst'
void    StrCpyPathRemoved(char *dst, const char *src)
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

