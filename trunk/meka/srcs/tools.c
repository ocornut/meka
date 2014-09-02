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

// nb- not const because we overwrite the month of February
static int month_len_table[12] =
{ 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// return number of days in February for given year
#ifndef ARCH_WIN32
static int	get_february_len (int year)
{
    if ((year % 4) != 0 || (((year % 100) == 0) && ((year % 400) == 0)))
        return (28);
    return (29);
}

// FIXME: do we really have to code this manually in 2005^h^h^h^h2012? seems insane (disclaimer: this is 1998 ms-dos code)
// ctime() anymore? (or similar function?)
static void	meka_get_time_date (int *phour, int *pminute, int *psecond, int *pday, int *pmonth, int *pyear, int *pday_of_week)
{
    time_t t;
    int	cnt;
    int	day, month, year, day_of_week;

    t = time(&t);

    cnt = (int)(t % (24 * 60 * 60));
    if (phour) *phour = cnt / (60 * 60);
    cnt %= (60 * 60);
    if (pminute) *pminute = cnt / (60);
    cnt %= 60;
    if (psecond) *psecond = cnt;

    cnt = (int)(t / (24 * 60 * 60));
    day = 0; // First
    month = 0; // January
    year = 1970; // 1970
    day_of_week = 3; // it was a Wednesday!
    month_len_table[1] = get_february_len(year);
    while (cnt--)
    {
        day_of_week = (day_of_week + 1) % 7;
        if (++day == month_len_table[month])
        {
            day = 0;
            if (++month == 12)
            {
                month = 0;
                year += 1;
                month_len_table[1] = get_february_len(year);
            }
        }
    }
    if (pday) *pday = day;
    if (pmonth) *pmonth = month + 1;
    if (pyear) *pyear = year;
    if (pday_of_week) *pday_of_week = day_of_week;
}
#endif

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
    int hour, minute, second;
    meka_get_time_date(&hour, &minute, &second, 0, 0, 0, 0);
    sprintf(str, "%02i:%02i:%02i", hour, minute, second);
#endif
    return (str);
}

//-----------------------------------------------------------------------------
