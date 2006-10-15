//-----------------------------------------------------------------------------
// MEKA - tools_t.c
// Time related tools - Code
//-----------------------------------------------------------------------------
// Note:
//   This file compiles without MEKA (it is used by buildupd.c)
//   FIXME -> should be moved in tools/ directory
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#ifdef DOS
 #include <dos.h>
#endif
#ifdef WIN32
 #include <Windows.h>
 #include <Winbase.h>
#endif

//-----------------------------------------------------------------------------

char	*month_name_table[12] =
{ "January", "February", "March", "April", "May", "June", "July",
  "August", "September", "October", "November", "December" };

int	month_len_table[12] =
{ 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// return february length depending weither the year is Leap or not
int	get_february_len (int year)
{
  if ((year % 4) != 0 || (((year % 100) == 0) && ((year % 400) == 0)))
    return (28);
  return (29);
}

void	meka_get_time_date (int *phour, int *pminute, int *psecond,
			    int *pday, int *pmonth, int *pyear, int *pday_of_week)
{
  time_t t;
  int	cnt;
  int	day, month, year, day_of_week;

  t = time(&t);

  cnt = t % (24 * 60 * 60);
  if (phour) *phour = cnt / (60 * 60);
  cnt %= (60 * 60);
  if (pminute) *pminute = cnt / (60);
  cnt %= 60;
  if (psecond) *psecond = cnt;

  cnt = t / (24 * 60 * 60);
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
  if (pmonth) *pmonth = month;
  if (pyear) *pyear = year;
  if (pday_of_week) *pday_of_week = day_of_week;
}

char *  meka_date_getf (void)
{
#ifdef UNIX
  // FIXME
  return ("<NO_DATE>");
#else
  time_t t;
  time (&t);
  return (ctime (&t));
#endif
}

// WRITE A FORMATTED STRING WITH TIME -----------------------------------------
char *  meka_time_getf (char *str)
{
#ifdef DOS
  struct time t;
  gettime (&t);
  sprintf (str, "%02i:%02i:%02i", t.ti_hour, t.ti_min, t.ti_sec);
#else
#ifdef WIN32
  SYSTEMTIME t;
  GetLocalTime (&t);
  sprintf (str, "%02i:%02i:%02i", t.wHour, t.wMinute, t.wSecond);
#else
  int hour, minute, second;
  meka_get_time_date(&hour, &minute, &second, 0, 0, 0, 0);
  sprintf (str, "%02i:%02i:%02i", hour, minute, second);
#endif
#endif
  return (str);
}

//-----------------------------------------------------------------------------

