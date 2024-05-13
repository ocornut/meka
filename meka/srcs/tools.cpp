//-----------------------------------------------------------------------------
// MEKA - tools.c
// Various helpers - Code
//-----------------------------------------------------------------------------

#include "shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#ifdef ARCH_WIN32
 #include <Windows.h>
 #include <Winbase.h>
 #pragma warning (disable: 4996) // ''_snprintf': This function or variable may be unsafe'
 // #include "shared.h"
#endif

//-----------------------------------------------------------------------------
// Path Functions
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

#ifdef ARCH_WIN32
    const char *p2 = strrchr(buf, '\\');
    if (p2 != NULL && p2 > p)
        p = p2;
#endif

    if (p != NULL)
    {
        p++;
        strcpy(tmp, p);
        strcpy(buf, tmp);
    }
}

// Extract filename from 'src' and copy it to location 'dst'
void    StrPath_RemoveDirectory(char *dst, const char *src)
{
    const char* p = strrchr(src, '/');

#ifdef ARCH_WIN32
    const char *p2 = strrchr(src, '\\');
    if (p2 != NULL && p2 > p)
        p = p2;
#endif

    p = (p ? p + 1 : src);
    strcpy(dst, p);
}

//-----------------------------------------------------------------------------
// Time Functions
//-----------------------------------------------------------------------------

char *  meka_date_getf (void)
{
    time_t t;
    time (&t);
    return (ctime (&t));
}

char *  meka_time_getf (char *str)
{
#ifdef ARCH_WIN32
    SYSTEMTIME t;
    GetLocalTime (&t);
    sprintf(str, "%02i:%02i:%02i", t.wHour, t.wMinute, t.wSecond);
#else
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(str, "%02i:%02i:%02i", tm.tm_hour, tm.tm_min, tm.tm_sec);
#endif
    return (str);
}

//-----------------------------------------------------------------------------
